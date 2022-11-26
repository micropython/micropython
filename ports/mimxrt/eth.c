/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
 * Copyright (c) 2021 Robert Hammelrath
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

#include <string.h>
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "ticks.h"

#if defined(MICROPY_HW_ETH_MDC)

#include "pin.h"
#include "shared/netutils/netutils.h"
#include "extmod/modnetwork.h"

#include "fsl_iomuxc.h"
#include "fsl_enet.h"
#include "fsl_phy.h"
#include "hal/phy/mdio/enet/fsl_enet_mdio.h"
#include "hal/phy/device/phyksz8081/fsl_phyksz8081.h"
#include "hal/phy/device/phydp83825/fsl_phydp83825.h"
#include "hal/phy/device/phydp83848/fsl_phydp83848.h"
#include "hal/phy/device/phylan8720/fsl_phylan8720.h"
#include "hal/phy/device/phyrtl8211f/fsl_phyrtl8211f.h"

#include "eth.h"
#include "lwip/etharp.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
#include "netif/ethernet.h"

#include "ticks.h"

#define ENET_RXBD_NUM          (5)
#define ENET_TXBD_NUM          (5)

typedef struct _eth_t {
    uint32_t trace_flags;
    struct netif netif;
    struct dhcp dhcp_struct;
} eth_t;

typedef struct _iomux_table_t {
    uint32_t muxRegister;
    uint32_t muxMode;
    uint32_t inputRegister;
    uint32_t inputDaisy;
    uint32_t configRegister;
    uint32_t inputOnfield;
    uint32_t configValue;
} iomux_table_t;

// ETH0 buffers and handles
static AT_NONCACHEABLE_SECTION_ALIGN(enet_rx_bd_struct_t g_rxBuffDescrip[ENET_RXBD_NUM], ENET_BUFF_ALIGNMENT);
static AT_NONCACHEABLE_SECTION_ALIGN(enet_tx_bd_struct_t g_txBuffDescrip[ENET_TXBD_NUM], ENET_BUFF_ALIGNMENT);
static SDK_ALIGN(uint8_t g_rxDataBuff[ENET_RXBD_NUM][SDK_SIZEALIGN(ENET_FRAME_MAX_FRAMELEN, ENET_BUFF_ALIGNMENT)],
    ENET_BUFF_ALIGNMENT);
static SDK_ALIGN(uint8_t g_txDataBuff[ENET_TXBD_NUM][SDK_SIZEALIGN(ENET_FRAME_MAX_FRAMELEN, ENET_BUFF_ALIGNMENT)],
    ENET_BUFF_ALIGNMENT);

// ENET Handles & Buffers
static enet_handle_t g_handle;
static mdio_handle_t mdioHandle = {.ops = &enet_ops};
static phy_handle_t phyHandle;
eth_t eth_instance0;

static enet_buffer_config_t buffConfig[] = {{
                                                ENET_RXBD_NUM,
                                                ENET_TXBD_NUM,
                                                SDK_SIZEALIGN(ENET_FRAME_MAX_FRAMELEN, ENET_BUFF_ALIGNMENT),
                                                SDK_SIZEALIGN(ENET_FRAME_MAX_FRAMELEN, ENET_BUFF_ALIGNMENT),
                                                &g_rxBuffDescrip[0],
                                                &g_txBuffDescrip[0],
                                                &g_rxDataBuff[0][0],
                                                &g_txDataBuff[0][0],
                                                #if FSL_ENET_DRIVER_VERSION >= 0x020300
                                                true,
                                                true,
                                                NULL,
                                                #endif
                                            }};

static const iomux_table_t iomux_table_enet[] = {
    IOMUX_TABLE_ENET
};

static uint8_t hw_addr[6]; // The MAC address field

#if defined(ENET_DUAL_PORT)

// ETH1 buffers and handles
static AT_NONCACHEABLE_SECTION_ALIGN(enet_rx_bd_struct_t g_rxBuffDescrip_1[ENET_RXBD_NUM], ENET_BUFF_ALIGNMENT);
static AT_NONCACHEABLE_SECTION_ALIGN(enet_tx_bd_struct_t g_txBuffDescrip_1[ENET_TXBD_NUM], ENET_BUFF_ALIGNMENT);
static SDK_ALIGN(uint8_t g_rxDataBuff_1[ENET_RXBD_NUM][SDK_SIZEALIGN(ENET_FRAME_MAX_FRAMELEN, ENET_BUFF_ALIGNMENT)],
    ENET_BUFF_ALIGNMENT);
static SDK_ALIGN(uint8_t g_txDataBuff_1[ENET_TXBD_NUM][SDK_SIZEALIGN(ENET_FRAME_MAX_FRAMELEN, ENET_BUFF_ALIGNMENT)],
    ENET_BUFF_ALIGNMENT);

static enet_handle_t g_handle_1;
static mdio_handle_t mdioHandle_1 = {.ops = &enet_ops};
static phy_handle_t phyHandle_1;
eth_t eth_instance1;

static enet_buffer_config_t buffConfig_1[] = {{
                                                  ENET_RXBD_NUM,
                                                  ENET_TXBD_NUM,
                                                  SDK_SIZEALIGN(ENET_FRAME_MAX_FRAMELEN, ENET_BUFF_ALIGNMENT),
                                                  SDK_SIZEALIGN(ENET_FRAME_MAX_FRAMELEN, ENET_BUFF_ALIGNMENT),
                                                  &g_rxBuffDescrip_1[0],
                                                  &g_txBuffDescrip_1[0],
                                                  &g_rxDataBuff_1[0][0],
                                                  &g_txDataBuff_1[0][0],
                                                  #if FSL_ENET_DRIVER_VERSION >= 0x020300
                                                  true,
                                                  true,
                                                  NULL,
                                                  #endif
                                              }};

static const iomux_table_t iomux_table_enet_1[] = {
    IOMUX_TABLE_ENET_1
};

static uint8_t hw_addr_1[6]; // The MAC address field

#endif

#if defined(ENET_DUAL_PORT)
#if defined MIMXRT117x_SERIES
#define ENET_1 ENET_1G
#else
#define ENET_1 ENET2
#endif
#else
#define ENET_1 ENET
#endif

#define PHY_AUTONEGO_TIMEOUT_US (5000000)
#define PHY_SETTLE_TIME_US      (1000)
// Settle time must be 500000 for the 1G interface
#define PHY_SETTLE_TIME_US_1    (500000)
#define ENET_RESET_LOW_TIME_US  (10000)
#define ENET_RESET_WAIT_TIME_US (30000)

#define IOTE (iomux_table[i])

#ifndef ENET_TX_CLK_OUTPUT
#define ENET_TX_CLK_OUTPUT true
#endif

#define TRACE_ASYNC_EV  (0x0001)
#define TRACE_ETH_TX    (0x0002)
#define TRACE_ETH_RX    (0x0004)
#define TRACE_ETH_FULL  (0x0008)

STATIC void eth_trace(eth_t *self, size_t len, const void *data, unsigned int flags) {
    if (((flags & NETUTILS_TRACE_IS_TX) && (self->trace_flags & TRACE_ETH_TX))
        || (!(flags & NETUTILS_TRACE_IS_TX) && (self->trace_flags & TRACE_ETH_RX))) {
        const uint8_t *buf;
        if (len == (size_t)-1) {
            // data is a pbuf
            const struct pbuf *pbuf = data;
            buf = pbuf->payload;
            len = pbuf->len; // restricted to print only the first chunk of the pbuf
        } else {
            // data is actual data buffer
            buf = data;
        }
        if (self->trace_flags & TRACE_ETH_FULL) {
            flags |= NETUTILS_TRACE_PAYLOAD;
        }
        netutils_ethernet_trace(MP_PYTHON_PRINTER, len, buf, flags);
    }
}

STATIC void eth_process_frame(eth_t *self, uint8_t *buf, size_t length) {

    struct netif *netif = &self->netif;
    if (netif->flags & NETIF_FLAG_LINK_UP) {
        struct pbuf *p = pbuf_alloc(PBUF_RAW, length, PBUF_POOL);
        if (p != NULL) {
            // Need to create a local copy first, since ENET_ReadFrame does not
            // provide a pointer to the buffer.
            pbuf_take(p, buf, length);
            if (netif->input(p, netif) != ERR_OK) {
                pbuf_free(p);
            }
        }
    }
}

void eth_irq_handler(ENET_Type *base, enet_handle_t *handle,
    #if FSL_FEATURE_ENET_QUEUE > 1
    uint32_t ringId,
    #endif /* FSL_FEATURE_ENET_QUEUE > 1 */
    enet_event_t event, enet_frame_info_t *frameInfo, void *userData) {
    eth_t *self = (eth_t *)userData;
    uint8_t g_rx_frame[ENET_FRAME_MAX_FRAMELEN + 14];
    uint32_t length = 0;
    status_t status;

    if (event == kENET_RxEvent) {
        do {
            status = ENET_GetRxFrameSize(handle, &length, 0);
            if (status == kStatus_Success) {
                // Get the data
                ENET_ReadFrame(base, handle, g_rx_frame, length, 0, NULL);
                eth_process_frame(self, g_rx_frame, length);
            } else if (status == kStatus_ENET_RxFrameError) {
                ENET_ReadFrame(base, handle, NULL, 0, 0, NULL);
            }
        } while (status != kStatus_ENET_RxFrameEmpty);
    } else {
        ENET_ClearInterruptStatus(base, ENET_TX_INTERRUPT | ENET_ERR_INTERRUPT);
    }
}

// Configure the ethernet clock
STATIC uint32_t eth_clock_init(int eth_id, bool phy_clock) {

    CLOCK_EnableClock(kCLOCK_Iomuxc);

    #if defined MIMXRT117x_SERIES

    clock_root_config_t rootCfg = {0};

    if (eth_id == MP_HAL_MAC_ETH0) {
        // Generate 50M root clock.
        rootCfg.mux = kCLOCK_ENET1_ClockRoot_MuxSysPll1Div2; // 500 MHz
        rootCfg.div = 10;
        CLOCK_SetRootClock(kCLOCK_Root_Enet1, &rootCfg);
        // 50M ENET_REF_CLOCK output to PHY and ENET module.
        // if required, handle phy_clock direction here
        IOMUXC_GPR->GPR4 |= 0x3;
    } else {
        // Generate 125M root clock.
        rootCfg.mux = kCLOCK_ENET1_ClockRoot_MuxSysPll1Div2; // 500 MHz
        rootCfg.div = 4;
        CLOCK_SetRootClock(kCLOCK_Root_Enet2, &rootCfg);
        IOMUXC_GPR->GPR5 |= IOMUXC_GPR_GPR5_ENET1G_RGMII_EN_MASK; /* bit1:iomuxc_gpr_enet_clk_dir
                                                                     bit0:GPR_ENET_TX_CLK_SEL(internal or OSC) */
    }
    return CLOCK_GetRootClockFreq(kCLOCK_Root_Bus);

    #else

    const clock_enet_pll_config_t config = {
        .enableClkOutput = phy_clock, .enableClkOutput25M = false, .loopDivider = 1
    };
    CLOCK_InitEnetPll(&config);

    IOMUXC_EnableMode(IOMUXC_GPR, kIOMUXC_GPR_ENET1RefClkMode, false); // Drive ENET_REF_CLK from PAD
    IOMUXC_EnableMode(IOMUXC_GPR, kIOMUXC_GPR_ENET1TxClkOutputDir, phy_clock);  // Enable output driver

    return CLOCK_GetFreq(kCLOCK_IpgClk);

    #endif
}

// eth_gpio_init: Configure the GPIO pins
STATIC void eth_gpio_init(const iomux_table_t iomux_table[], size_t iomux_table_size,
    const machine_pin_obj_t *reset_pin, const machine_pin_obj_t *int_pin) {

    gpio_pin_config_t gpio_config = {kGPIO_DigitalOutput, 1, kGPIO_NoIntmode};
    (void)gpio_config;
    const machine_pin_af_obj_t *af_obj;

    if (reset_pin != NULL) {
        // Configure the Reset Pin
        af_obj = pin_find_af(reset_pin, PIN_AF_MODE_ALT5);

        IOMUXC_SetPinMux(reset_pin->muxRegister, af_obj->af_mode, 0, 0, reset_pin->configRegister, 0U);
        IOMUXC_SetPinConfig(reset_pin->muxRegister, af_obj->af_mode, 0, 0, reset_pin->configRegister,
            pin_generate_config(PIN_PULL_DISABLED, PIN_MODE_OUT, PIN_DRIVE_5, reset_pin->configRegister));
        GPIO_PinInit(reset_pin->gpio, reset_pin->pin, &gpio_config);
    }

    if (int_pin != NULL) {
        // Configure the Int Pin
        af_obj = pin_find_af(int_pin, PIN_AF_MODE_ALT5);

        IOMUXC_SetPinMux(int_pin->muxRegister, af_obj->af_mode, 0, 0, int_pin->configRegister, 0U);
        IOMUXC_SetPinConfig(int_pin->muxRegister, af_obj->af_mode, 0, 0, int_pin->configRegister,
            pin_generate_config(PIN_PULL_UP_47K, PIN_MODE_IN, PIN_DRIVE_5, int_pin->configRegister));
        GPIO_PinInit(int_pin->gpio, int_pin->pin, &gpio_config);
    }

    // Configure the Transceiver Pins, Settings except for CLK:
    // Slew Rate Field: Fast Slew Rate, Drive Strength, R0/5, Speed max(200MHz)
    // Open Drain Disabled, Pull Enabled, Pull 100K Ohm Pull Up
    // Hysteresis Disabled
    for (int i = 0; i < iomux_table_size; i++) {
        IOMUXC_SetPinMux(IOTE.muxRegister, IOTE.muxMode, IOTE.inputRegister, IOTE.inputDaisy, IOTE.configRegister, IOTE.inputOnfield);
        IOMUXC_SetPinConfig(IOTE.muxRegister, IOTE.muxMode, IOTE.inputRegister, IOTE.inputDaisy, IOTE.configRegister, IOTE.configValue);
    }

    // Reset the transceiver
    if (reset_pin != NULL) {
        GPIO_PinWrite(reset_pin->gpio, reset_pin->pin, 0);
        mp_hal_delay_us(ENET_RESET_LOW_TIME_US);
        GPIO_PinWrite(reset_pin->gpio, reset_pin->pin, 1);
        mp_hal_delay_us(ENET_RESET_WAIT_TIME_US);
    }
}

// eth_phy_init: Initilaize the PHY interface
STATIC void eth_phy_init(phy_handle_t *phyHandle, phy_config_t *phy_config,
    phy_speed_t *speed, phy_duplex_t *duplex, uint32_t phy_settle_time) {

    bool link = false;
    bool autonego = false;
    phy_config->autoNeg = true;

    status_t status = PHY_Init(phyHandle, phy_config);
    if (status == kStatus_Success) {
        uint64_t t = ticks_us64() + PHY_AUTONEGO_TIMEOUT_US;
        // Wait for auto-negotiation success and link up
        do {
            PHY_GetAutoNegotiationStatus(phyHandle, &autonego);
            PHY_GetLinkStatus(phyHandle, &link);
            if (autonego && link) {
                break;
            }
        } while (ticks_us64() < t);
        if (!autonego) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("PHY Auto-negotiation failed."));
        }
        PHY_GetLinkSpeedDuplex(phyHandle, speed, duplex);
    } else {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("PHY Init failed."));
    }
    mp_hal_delay_us(phy_settle_time);
}

// eth_init: Set up GPIO and the transceiver
void eth_init_0(eth_t *self, int eth_id, const phy_operations_t *phy_ops, int phy_addr, bool phy_clock) {
    // Configuration values
    enet_config_t enet_config;

    phy_config_t phy_config = {0};

    uint32_t source_clock = eth_clock_init(eth_id, phy_clock);

    eth_gpio_init(iomux_table_enet, ARRAY_SIZE(iomux_table_enet), ENET_RESET_PIN, ENET_INT_PIN);

    mp_hal_get_mac(0, hw_addr);

    // Init the PHY interface & negotiate the speed
    phyHandle.ops = phy_ops;
    phy_config.phyAddr = phy_addr;
    phyHandle.mdioHandle = &mdioHandle;
    mdioHandle.resource.base = ENET;
    mdioHandle.resource.csrClock_Hz = source_clock;

    phy_speed_t speed = kENET_MiiSpeed100M;
    phy_duplex_t duplex = kENET_MiiFullDuplex;
    eth_phy_init(&phyHandle, &phy_config, &speed, &duplex, PHY_SETTLE_TIME_US);

    ENET_Reset(ENET);
    ENET_GetDefaultConfig(&enet_config);
    enet_config.miiSpeed = (enet_mii_speed_t)speed;
    enet_config.miiDuplex = (enet_mii_duplex_t)duplex;
    enet_config.miiMode = kENET_RmiiMode;
    // Enable checksum generation by the ENET controller
    enet_config.txAccelerConfig = kENET_TxAccelIpCheckEnabled | kENET_TxAccelProtoCheckEnabled;
    // Set interrupt
    enet_config.interrupt |= ENET_TX_INTERRUPT | ENET_RX_INTERRUPT;

    ENET_Init(ENET, &g_handle, &enet_config, &buffConfig[0], hw_addr, source_clock);
    ENET_SetCallback(&g_handle, eth_irq_handler, (void *)self);
    NVIC_SetPriority(ENET_IRQn, IRQ_PRI_PENDSV);
    ENET_EnableInterrupts(ENET, ENET_RX_INTERRUPT);
    ENET_ClearInterruptStatus(ENET, ENET_TX_INTERRUPT | ENET_RX_INTERRUPT | ENET_ERR_INTERRUPT);
    NVIC_SetPriority(ENET_IRQn, IRQ_PRI_PENDSV);
    ENET_EnableInterrupts(ENET, ENET_RX_INTERRUPT);
    ENET_ActiveRead(ENET);
}

#if defined(ENET_DUAL_PORT)

// eth_init: Set up GPIO and the transceiver
void eth_init_1(eth_t *self, int eth_id, const phy_operations_t *phy_ops, int phy_addr, bool phy_clock) {
    // Configuration values
    enet_config_t enet_config;

    phy_config_t phy_config = {0};

    uint32_t source_clock = eth_clock_init(eth_id, phy_clock);

    eth_gpio_init(iomux_table_enet_1, ARRAY_SIZE(iomux_table_enet_1), ENET_1_RESET_PIN, ENET_1_INT_PIN);

    #if defined MIMXRT117x_SERIES
    NVIC_SetPriority(ENET_1G_MAC0_Tx_Rx_1_IRQn, IRQ_PRI_PENDSV);
    NVIC_SetPriority(ENET_1G_MAC0_Tx_Rx_2_IRQn, IRQ_PRI_PENDSV);
    NVIC_SetPriority(ENET_1G_IRQn, IRQ_PRI_PENDSV);
    EnableIRQ(ENET_1G_MAC0_Tx_Rx_1_IRQn);
    EnableIRQ(ENET_1G_MAC0_Tx_Rx_2_IRQn);
    phy_speed_t speed = kENET_MiiSpeed1000M;
    #else
    NVIC_SetPriority(ENET2_IRQn, IRQ_PRI_PENDSV);
    phy_speed_t speed = kENET_MiiSpeed100M;
    #endif

    mp_hal_get_mac(1, hw_addr_1);

    // Init the PHY interface & negotiate the speed
    phyHandle_1.ops = phy_ops;
    phy_config.phyAddr = phy_addr;
    phyHandle_1.mdioHandle = &mdioHandle_1;
    mdioHandle_1.resource.base = ENET_1;
    mdioHandle_1.resource.csrClock_Hz = source_clock;

    phy_duplex_t duplex = kENET_MiiFullDuplex;
    eth_phy_init(&phyHandle_1, &phy_config, &speed, &duplex, PHY_SETTLE_TIME_US_1);

    ENET_Reset(ENET_1);
    ENET_GetDefaultConfig(&enet_config);
    enet_config.miiSpeed = (enet_mii_speed_t)speed;
    enet_config.miiDuplex = (enet_mii_duplex_t)duplex;
    // Enable checksum generation by the ENET controller
    enet_config.txAccelerConfig = kENET_TxAccelIpCheckEnabled | kENET_TxAccelProtoCheckEnabled;
    // Set interrupt
    enet_config.interrupt = ENET_TX_INTERRUPT | ENET_RX_INTERRUPT;

    ENET_Init(ENET_1, &g_handle_1, &enet_config, &buffConfig_1[0], hw_addr_1, source_clock);
    ENET_SetCallback(&g_handle_1, eth_irq_handler, (void *)self);
    ENET_ClearInterruptStatus(ENET_1, ENET_TX_INTERRUPT | ENET_RX_INTERRUPT | ENET_ERR_INTERRUPT);
    ENET_EnableInterrupts(ENET_1, ENET_RX_INTERRUPT);
    ENET_ActiveRead(ENET_1);
}

#endif
// Initialize the phy interface
STATIC int eth_mac_init(eth_t *self) {
    return 0;
}

// Deinit the interface
STATIC void eth_mac_deinit(eth_t *self) {
    // Just as a reminder: Calling ENET_Deinit() twice causes the board to stall
    // with a bus error. Reason unclear. So don't do that for now (or ever).
}

void eth_set_trace(eth_t *self, uint32_t value) {
    self->trace_flags = value;
}

/*******************************************************************************/
// ETH-LwIP bindings

STATIC err_t eth_send_frame_blocking(ENET_Type *base, enet_handle_t *handle, uint8_t *buffer, int len) {
    status_t status;
    int i;
    #define XMIT_LOOP 10

    // Try a few times to send the frame
    for (i = XMIT_LOOP; i > 0; i--) {
        status = ENET_SendFrame(base, handle, buffer, len, 0, false, NULL);
        if (status != kStatus_ENET_TxFrameBusy) {
            break;
        }
        ticks_delay_us64(base == ENET ? 100 : 20);
    }
    return status;
}

STATIC err_t eth_netif_output(struct netif *netif, struct pbuf *p) {
    // This function should always be called from a context where PendSV-level IRQs are disabled
    status_t status;
    ENET_Type *enet = ENET;
    enet_handle_t *handle = &g_handle;

    #if defined ENET_DUAL_PORT
    if (netif->state == &eth_instance1) {
        enet = ENET_1;
        handle = &g_handle_1;
    }
    #endif

    eth_trace(netif->state, (size_t)-1, p, NETUTILS_TRACE_IS_TX | NETUTILS_TRACE_NEWLINE);

    if (p->next == NULL) {
        status = eth_send_frame_blocking(enet, handle, p->payload, p->len);
    } else {
        // frame consists of several parts. Copy them together and send them
        size_t length = 0;
        uint8_t tx_frame[ENET_FRAME_MAX_FRAMELEN + 14];

        while (p) {
            memcpy(&tx_frame[length], p->payload, p->len);
            length += p->len;
            p = p->next;
        }
        status = eth_send_frame_blocking(enet, handle, tx_frame, length);
    }
    return status == kStatus_Success ? ERR_OK : ERR_BUF;
}

STATIC err_t eth_netif_init(struct netif *netif) {
    netif->linkoutput = eth_netif_output;
    netif->output = etharp_output;
    netif->mtu = 1500;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP;
    // Checksums only need to be checked on incoming frames, not computed on outgoing frames
    NETIF_SET_CHECKSUM_CTRL(netif,
        NETIF_CHECKSUM_CHECK_IP
        | NETIF_CHECKSUM_CHECK_UDP
        | NETIF_CHECKSUM_CHECK_TCP
        | NETIF_CHECKSUM_CHECK_ICMP
        | NETIF_CHECKSUM_CHECK_ICMP6
        );
    return ERR_OK;
}

STATIC void eth_lwip_init(eth_t *self) {
    struct netif *n = &self->netif;
    ip_addr_t ipconfig[4];

    self->netif.hwaddr_len = 6;
    if (self == &eth_instance0) {
        memcpy(self->netif.hwaddr, hw_addr, 6);
        IP4_ADDR(&ipconfig[0], 192, 168, 0, 2);
    #if defined ENET_DUAL_PORT
    } else {
        memcpy(self->netif.hwaddr, hw_addr_1, 6);
        IP4_ADDR(&ipconfig[0], 192, 168, 0, 3);
    #endif
    }
    IP4_ADDR(&ipconfig[1], 255, 255, 255, 0);
    IP4_ADDR(&ipconfig[2], 192, 168, 0, 1);
    IP4_ADDR(&ipconfig[3], 8, 8, 8, 8);

    MICROPY_PY_LWIP_ENTER

    n->name[0] = 'e';
    n->name[1] = (self == &eth_instance0 ? '0' : '1');
    netif_add(n, &ipconfig[0], &ipconfig[1], &ipconfig[2], self, eth_netif_init, ethernet_input);
    netif_set_hostname(n, "MPY");
    netif_set_default(n);
    netif_set_up(n);

    dns_setserver(0, &ipconfig[3]);
    dhcp_set_struct(n, &self->dhcp_struct);
    dhcp_start(n);

    netif_set_link_up(n);

    MICROPY_PY_LWIP_EXIT
}

STATIC void eth_lwip_deinit(eth_t *self) {
    MICROPY_PY_LWIP_ENTER
    for (struct netif *netif = netif_list; netif != NULL; netif = netif->next) {
        if (netif == &self->netif) {
            netif_remove(netif);
            netif->ip_addr.addr = 0;
            netif->flags = 0;
        }
    }
    MICROPY_PY_LWIP_EXIT
}

struct netif *eth_netif(eth_t *self) {
    return &self->netif;
}

int eth_link_status(eth_t *self) {
    struct netif *netif = &self->netif;
    if ((netif->flags & (NETIF_FLAG_UP | NETIF_FLAG_LINK_UP))
        == (NETIF_FLAG_UP | NETIF_FLAG_LINK_UP)) {
        if (netif->ip_addr.addr != 0) {
            return 3; // link up
        } else {
            return 2; // link no-ip;
        }
    } else {
        bool link;
        #if defined ENET_DUAL_PORT
        PHY_GetLinkStatus(self == &eth_instance0 ? &phyHandle : &phyHandle_1, &link);
        #else
        PHY_GetLinkStatus(&phyHandle, &link);
        #endif
        if (link) {
            return 1; // link up
        } else {
            return 0; // link down
        }
    }
}

int eth_start(eth_t *self) {
    eth_lwip_deinit(self);

    // Make sure Eth is Not in low power mode.
    eth_low_power_mode(self, false);

    int ret = eth_mac_init(self);
    if (ret < 0) {
        return ret;
    }
    eth_lwip_init(self);
    return 0;
}

int eth_stop(eth_t *self) {
    eth_lwip_deinit(self);
    eth_mac_deinit(self);
    return 0;
}

void eth_low_power_mode(eth_t *self, bool enable) {
    #if defined ENET_DUAL_PORT
    ENET_EnableSleepMode(self == &eth_instance0 ? ENET : ENET_1, enable);
    #else
    ENET_EnableSleepMode(ENET, enable);
    #endif
}
#endif // defined(MICROPY_HW_ETH_MDC)
