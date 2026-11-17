/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Ibrahim Abdelkader <iabdalkader@openmv.io>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#if MICROPY_HW_ETH_LAN9118
// Polling-mode driver for the SMSC LAN9118 Ethernet controller as emulated by
// QEMU on the MPS2/MPS3 machines. No interrupt is wired on this port: received
// frames are drained by eth_poll() from the network poll pump, and every
// register handshake is polled with a timeout.
//
// References:
// - LAN9118 datasheet, Microchip DS00002266B.
// - LAN9118 Family Programmer Reference Guide, SMSC AN 12.12, rev 1.0.
// - U-Boot smc911x driver (drivers/net/smc911x.c).
//
// The datasheet mandates minimum delays between certain read-after-write and
// read-after-read CSR accesses. We don't honour them, as QEMU's model has no
// such timing constraints.

#include "py/mphal.h"
#include "py/mperrno.h"

#include "lwip/err.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
#include "netif/etharp.h"

#include "shared/netutils/netutils.h"
#include "shared/netutils/dhcpserver.h"
#include "eth.h"

// System CSR bits
#define HW_CFG_SRST         (1u << 0)       // 0x74 self-clearing soft reset
#define HW_CFG_TX_FIF_SZ_S  (16)            // 0x74 bits 19:16 TX FIFO size (KB)
#define PMT_CTRL_READY      (1u << 0)       // 0x84 device ready after reset
#define PMT_CTRL_PM_MODE    (3u << 12)      // Power Management Mode
#define TX_CFG_TX_ON        (1u << 1)       // 0x70 enable TX FIFO
#define TX_CFG_STOP_TX      (1u << 0)       // 0x70 graceful transmitter stop
#define TX_CFG_TXS_DUMP     (1u << 15)      // 0x70 self-clearing dump of TX status FIFO
#define TX_CFG_TXD_DUMP     (1u << 14)      // 0x70 self-clearing dump of TX data FIFO
#define RX_CFG_RX_DUMP      (1u << 15)      // 0x6c self-clearing dump of RX data/status FIFOs
#define RX_DP_CTRL_FFWD     (1u << 31)      // 0x78 fast-forward (discard) a frame
#define E2P_CMD_BUSY        (1u << 31)      // 0xb0 EEPROM/reload busy

// MAC_CSR_CMD (0xa4) indirect access
#define MAC_CSR_BUSY        (1u << 31)
#define MAC_CSR_READ        (1u << 30)      // set=read, clear=write

// MAC register indices (accessed via MAC_CSR_CMD/DATA)
#define MAC_CR              1
#define MAC_ADDRH           2
#define MAC_ADDRL           3
#define MAC_MII_ACC         6
#define MAC_MII_DATA        7
#define MAC_CR_RXEN         (1u << 2)
#define MAC_CR_TXEN         (1u << 3)

// MII_ACC bits (PHY access, itself a MAC CSR)
#define MII_ACC_BUSY        (1u << 0)
#define MII_ACC_WRITE       (1u << 1)       // set=write, clear=read
#define MII_ACC_PHY_S       (11)            // bits 15:11 PHY address
#define MII_ACC_REG_S       (6)             // bits 10:6  MII register index
#define LAN9118_PHY_ADDR    1               // internal PHY is at address 1

// PHY (internal) MII registers, accessed via phy_read()/phy_write()
#define PHY_REG_BMCR        0
#define PHY_REG_BMSR        1
#define PHY_BMCR_ANRESTART  (1u << 9)
#define PHY_BMCR_ANENABLE   (1u << 12)
#define PHY_BMSR_LINK       (1u << 2)       // link status

// TX command word A (first word to TX_DATA_FIFO)
#define TX_CMD_A_FIRST_SEG  (1u << 13)
#define TX_CMD_A_LAST_SEG   (1u << 12)
// buffer size in bits 10:0, data-start byte offset in bits 20:16
// TX command word B: packet length in bits 10:0, packet tag in bits 31:16

// RX status FIFO word (read from RX_STATUS_FIFO 0x40)
#define RX_STS_ERROR        (1u << 15)      // error summary
#define RX_STS_LEN_S        (16)            // bits 29:16 frame length (incl 4-byte FCS)
#define RX_STS_LEN_M        (0x3fffu)

typedef struct _LAN9118_t {
    volatile uint32_t RX_DATA_FIFO[8];      // 0x00: RX data FIFO (aliased port)
    volatile uint32_t TX_DATA_FIFO[8];      // 0x20: TX data FIFO (aliased port)
    volatile uint32_t RX_STATUS_FIFO;       // 0x40
    volatile uint32_t RX_STATUS_FIFO_PEEK;  // 0x44
    volatile uint32_t TX_STATUS_FIFO;       // 0x48
    volatile uint32_t TX_STATUS_FIFO_PEEK;  // 0x4c
    volatile uint32_t ID_REV;               // 0x50
    volatile uint32_t IRQ_CFG;              // 0x54
    volatile uint32_t INT_STS;              // 0x58
    volatile uint32_t INT_EN;               // 0x5c
    volatile uint32_t _reserved0;           // 0x60 (hole)
    volatile uint32_t BYTE_TEST;            // 0x64: reads 0x87654321
    volatile uint32_t FIFO_INT;             // 0x68
    volatile uint32_t RX_CFG;               // 0x6c
    volatile uint32_t TX_CFG;               // 0x70
    volatile uint32_t HW_CFG;               // 0x74: SRST bit here
    volatile uint32_t RX_DP_CTRL;           // 0x78
    volatile uint32_t RX_FIFO_INF;          // 0x7c: RX fill level
    volatile uint32_t TX_FIFO_INF;          // 0x80: TX free space
    volatile uint32_t PMT_CTRL;             // 0x84
    volatile uint32_t GPIO_CFG;             // 0x88
    volatile uint32_t GPT_CFG;              // 0x8c
    volatile uint32_t GPT_CNT;              // 0x90
    volatile uint32_t _reserved1;           // 0x94 (hole)
    volatile uint32_t WORD_SWAP;            // 0x98
    volatile uint32_t FREE_RUN;             // 0x9c
    volatile uint32_t RX_DROP;              // 0xa0
    volatile uint32_t MAC_CSR_CMD;          // 0xa4: indirect MAC access
    volatile uint32_t MAC_CSR_DATA;         // 0xa8: indirect MAC data
    volatile uint32_t AFC_CFG;              // 0xac
    volatile uint32_t E2P_CMD;              // 0xb0
    volatile uint32_t E2P_DATA;             // 0xb4
} LAN9118_t;

#define LAN9118 ((LAN9118_t *)LAN9118_BASE)

// Driver state
typedef struct _eth_t {
    bool enabled;
    uint32_t phy_addr;
    struct netif netif;
    struct dhcp dhcp_struct;
    bool last_link_status;
    // Shared TX/RX frame bounce buffer
    uint32_t frame_buf[(1518 + 3) / 4];
} eth_t;

// Single interface instance (referenced by network_lan.c).
eth_t eth_instance;

// Poll a register until its masked value equals the expected value.
static int reg_poll(volatile uint32_t *reg, uint32_t mask, uint32_t expected, uint32_t timeout_us) {
    mp_uint_t start = mp_hal_ticks_us();
    while ((*reg & mask) != expected) {
        if (mp_hal_ticks_us() - start > timeout_us) {
            return -MP_ETIMEDOUT;
        }
    }
    return 0;
}

// Indirect MAC and PHY access helpers. Return 0 on success or -MP_ETIMEDOUT.
static int mac_csr_read(uint32_t reg, uint32_t *val) {
    LAN9118->MAC_CSR_CMD = MAC_CSR_BUSY | MAC_CSR_READ | (reg & 0xff);
    int ret = reg_poll(&LAN9118->MAC_CSR_CMD, MAC_CSR_BUSY, 0, 1000);
    if (ret == 0) {
        *val = LAN9118->MAC_CSR_DATA;
    }
    return ret;
}

static int mac_csr_write(uint32_t reg, uint32_t val) {
    LAN9118->MAC_CSR_DATA = val;
    LAN9118->MAC_CSR_CMD = MAC_CSR_BUSY | (reg & 0xff);   // READ bit clear = write
    return reg_poll(&LAN9118->MAC_CSR_CMD, MAC_CSR_BUSY, 0, 1000);
}

static int phy_read(uint32_t phy_addr, uint32_t reg, uint16_t *val) {
    int ret = mac_csr_write(MAC_MII_ACC,
        (phy_addr << MII_ACC_PHY_S) | (reg << MII_ACC_REG_S) | MII_ACC_BUSY);
    if (ret != 0) {
        return ret;
    }

    // Wait for the MII operation to complete.
    uint32_t acc;
    do {
        ret = mac_csr_read(MAC_MII_ACC, &acc);
        if (ret != 0) {
            return ret;
        }
    } while (acc & MII_ACC_BUSY);

    uint32_t data;
    ret = mac_csr_read(MAC_MII_DATA, &data);
    if (ret == 0) {
        *val = (uint16_t)data;
    }

    return ret;
}

static int phy_write(uint32_t phy_addr, uint32_t reg, uint16_t val) {
    int ret = mac_csr_write(MAC_MII_DATA, val);
    if (ret != 0) {
        return ret;
    }

    ret = mac_csr_write(MAC_MII_ACC,
        (phy_addr << MII_ACC_PHY_S) | (reg << MII_ACC_REG_S)
        | MII_ACC_WRITE | MII_ACC_BUSY);
    if (ret != 0) {
        return ret;
    }

    // Wait for the MII operation to complete.
    uint32_t acc;
    do {
        ret = mac_csr_read(MAC_MII_ACC, &acc);
        if (ret != 0) {
            return ret;
        }
    } while (acc & MII_ACC_BUSY);

    return 0;
}

// lwIP TX: write the two TX command words followed by the frame data.
static err_t eth_netif_output(struct netif *netif, struct pbuf *p) {
    eth_t *self = netif->state;
    uint16_t len = p->tot_len;
    if (len == 0 || len > 1514) {
        return ERR_ARG;
    }
    uint32_t words = (len + 3u) / 4u;

    // Ensure the TX data FIFO has room for the frame plus the two command words.
    if ((LAN9118->TX_FIFO_INF & 0xffff) < words * 4 + 8) {
        return ERR_BUF;
    }

    pbuf_copy_partial(p, self->frame_buf, len, 0);

    // Single buffer, single packet: first and last segment set (AN 12.12 §6.2).
    // The controller pads short frames and appends the FCS itself.
    LAN9118->TX_DATA_FIFO[0] = TX_CMD_A_FIRST_SEG | TX_CMD_A_LAST_SEG | (len & 0x7ff);
    LAN9118->TX_DATA_FIFO[0] = (len & 0x7ff);
    for (uint32_t i = 0; i < words; i++) {
        LAN9118->TX_DATA_FIFO[0] = self->frame_buf[i];
    }

    // Drain completed TX status words so the TX status FIFO doesn't fill.
    while ((LAN9118->TX_FIFO_INF >> 16) & 0xff) {
        (void)LAN9118->TX_STATUS_FIFO;
    }
    return ERR_OK;
}

// netif init callback. netif_add() clears flags before calling this, so
// the output ops, flags and mtu are set here rather than in eth_init().
static err_t eth_netif_init(struct netif *netif) {
    netif->linkoutput = eth_netif_output;
    netif->output = etharp_output;
    netif->mtu = 1500;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP;
    return ERR_OK;
}

// Drain any received frames from the RX FIFO into lwIP. Called from
// the network poll pump (no RX interrupt is wired on this port).
void eth_poll(void) {
    eth_t *self = &eth_instance;
    if (!self->enabled || self->netif.input == NULL) {
        return;
    }

    // RXSUSED (bits 23:16 of RX_FIFO_INF) = number of frames waiting.
    while ((LAN9118->RX_FIFO_INF >> 16) & 0xff) {
        uint32_t status = LAN9118->RX_STATUS_FIFO;              // pops one status word
        uint32_t len = (status >> RX_STS_LEN_S) & RX_STS_LEN_M; // includes the 4-byte FCS
        uint32_t words = (len + 3) / 4;

        struct pbuf *p = NULL;
        if (!(status & RX_STS_ERROR) && len > 4 && len <= 1518) {
            p = pbuf_alloc(PBUF_RAW, len - 4, PBUF_POOL);       // strip the FCS
        }

        // Always drain the data words so the data/status FIFOs stay in sync.
        for (uint32_t i = 0; i < words; i++) {
            uint32_t w = LAN9118->RX_DATA_FIFO[0];
            if (p != NULL) {
                self->frame_buf[i] = w;
            }
        }

        if (p != NULL) {
            pbuf_take(p, self->frame_buf, len - 4);
            if (self->netif.input(p, &self->netif) != ERR_OK) {
                pbuf_free(p);
            }
        }
    }
}

int eth_init(eth_t *self, int mac_idx, uint32_t phy_addr, int phy_type) {
    // Return if already initialised.
    if (self->netif.input != NULL) {
        return 0;
    }

    // Only the LAN9118 internal PHY (address 1) is supported.
    if (phy_type != ETH_PHY_LAN9118 || phy_addr != LAN9118_PHY_ADDR) {
        return -MP_ENODEV;
    }

    // Ensure that the internal PHY is running
    if (LAN9118->PMT_CTRL & PMT_CTRL_PM_MODE) {
        // If any of those bits are set, the device is in a power-down state.
        // Write any data to the BYTE_TEST register to wake-up the device
        LAN9118->BYTE_TEST = 0x00;
    }

    // PMT_CTRL_READY should be high when device is ready.
    if (reg_poll(&LAN9118->PMT_CTRL, PMT_CTRL_READY, 1, 10000) != 0) {
        return -MP_ETIMEDOUT;
    }

    // Confirm the byte ordering of the host-device interface.
    if (LAN9118->BYTE_TEST != 0x87654321u) {
        return -MP_ENODEV;
    }

    // Soft-reset the controller. Initiated by writing a 1 to bit 0 of the
    // HW_CFG register (SRST). This self-clearing bit will return to 0 after
    // approximately 2us.
    LAN9118->HW_CFG |= HW_CFG_SRST;
    if (reg_poll(&LAN9118->HW_CFG, HW_CFG_SRST, 0, 10) != 0) {
        return -MP_ETIMEDOUT;
    }

    // Set TX FIFO size (in 1KB values).
    LAN9118->HW_CFG = (LAN9118->HW_CFG & ~(0xfu << HW_CFG_TX_FIF_SZ_S)) | (5u << HW_CFG_TX_FIF_SZ_S);

    // TODO: FIFO Allocation and Flow-Control Configuration

    // Prepare interface state. The lwIP netif output ops, flags and mtu
    // are set by eth_netif_init() when netif_add() runs in eth_start().
    self->enabled = false;
    self->phy_addr = phy_addr;
    self->last_link_status = false;
    self->netif.hwaddr_len = sizeof(self->netif.hwaddr);
    mp_hal_get_mac(mac_idx, &self->netif.hwaddr[0]);

    return 0;
}

int eth_start(eth_t *self) {
    // Program the MAC address (AN 12.12 §5.1.3.2: for 1a:2b:3c:4d:5e:6f,
    // ADDRL = 0x4d3c2b1a and ADDRH = 0x00006f5e).
    uint8_t *mac = self->netif.hwaddr;
    uint32_t addrl = mac[0] | (mac[1] << 8) | (mac[2] << 16) | ((uint32_t)mac[3] << 24);
    uint32_t addrh = mac[4] | (mac[5] << 8);
    if (mac_csr_write(MAC_ADDRH, addrh) != 0 || mac_csr_write(MAC_ADDRL, addrl) != 0) {
        return -MP_EIO;
    }

    // Kick off PHY auto-negotiation (QEMU brings the link up immediately).
    if (phy_write(self->phy_addr, PHY_REG_BMCR, PHY_BMCR_ANENABLE | PHY_BMCR_ANRESTART) != 0) {
        return -MP_EIO;
    }

    // Enable the transmitter FIFO and the MAC RX/TX paths.
    uint32_t macr;
    LAN9118->TX_CFG = TX_CFG_TX_ON;
    if (mac_csr_read(MAC_CR, &macr) != 0
        || mac_csr_write(MAC_CR, macr | MAC_CR_TXEN | MAC_CR_RXEN) != 0) {
        return -MP_EIO;
    }

    // Register the interface with lwIP (once) and bring it up.
    if (self->netif.input == NULL) {
        ip4_addr_t any;
        ip4_addr_set_zero(&any);
        netif_add(&self->netif, &any, &any, &any, self, eth_netif_init, ethernet_input);
        netif_set_default(&self->netif);
    }
    netif_set_up(&self->netif);
    netif_set_link_up(&self->netif);
    self->last_link_status = true;

    // Start DHCP using the preallocated dhcp struct (avoids a heap allocation).
    dhcp_set_struct(&self->netif, &self->dhcp_struct);
    dhcp_start(&self->netif);

    self->enabled = true;

    // Wait a while for a DHCP lease, so active(True) returns a usable interface.
    mp_uint_t start = mp_hal_ticks_ms();
    while (!dhcp_supplied_address(&self->netif) && mp_hal_ticks_ms() - start < 1000) {
        mp_hal_delay_ms(10);
    }
    return 0;
}

int eth_stop(eth_t *self) {
    dhcp_stop(&self->netif);
    netif_set_link_down(&self->netif);
    netif_set_down(&self->netif);

    // Stop the transmitter gracefully (AN 12.12 §5.3): request STOP_TX and wait
    // for the current frame to flush, which self-clears STOP_TX and TX_ON.
    LAN9118->TX_CFG |= TX_CFG_STOP_TX;
    reg_poll(&LAN9118->TX_CFG, TX_CFG_TX_ON, 0, 1000);

    // Clear the TX status/data FIFOs. These self-clearing dumps also force-reset
    // the transmitter should TX_ON have failed to clear above.
    LAN9118->TX_CFG |= TX_CFG_TXS_DUMP | TX_CFG_TXD_DUMP;

    // Stop the receiver (§5.4): clear RXEN, then dump the RX status/data FIFOs
    // (RX_DUMP must be set only once the receiver is halted).
    uint32_t macr;
    if (mac_csr_read(MAC_CR, &macr) == 0) {
        mac_csr_write(MAC_CR, macr & ~(MAC_CR_TXEN | MAC_CR_RXEN));
    }
    LAN9118->RX_CFG |= RX_CFG_RX_DUMP;
    reg_poll(&LAN9118->RX_CFG, RX_CFG_RX_DUMP, 0, 1000);

    self->enabled = false;
    self->last_link_status = false;
    return 0;
}

bool eth_is_enabled(eth_t *self) {
    return self->enabled;
}

struct netif *eth_netif(eth_t *self) {
    return &self->netif;
}

int eth_link_status(eth_t *self) {
    uint16_t bmsr;
    if (phy_read(self->phy_addr, PHY_REG_BMSR, &bmsr) != 0) {
        return 0;
    }
    return (bmsr & PHY_BMSR_LINK) ? 3 : 0;
}

void eth_set_trace(eth_t *self, uint32_t value) {
    // TODO: not implemented.
    (void)self;
    (void)value;
}

void eth_low_power_mode(eth_t *self, bool enable) {
    // TODO: not implemented.
    (void)self;
    (void)enable;
}

#endif // MICROPY_HW_ETH_LAN9118
