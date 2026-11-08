// eth.c - GD32F4 ENET driver providing an lwIP netif.
//
// The GD32 standard peripheral library owns the DMA descriptor rings and
// exposes two simple frame primitives (enet_frame_receive/_transmit), so this
// driver only has to:
//   1. bring up the RMII pins and the ENET clocks,
//   2. run PHY auto-negotiation and latch the resulting media mode,
//   3. service the DMA interrupt and hand frames to/from lwIP.
//
// Compare ports/stm32/eth.c, which drives the HAL directly; GD32's library
// hides that layer, so this file is considerably shorter.

#include <string.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "genhdr/mpversion.h"

#include "gd32f4xx.h"
#include "pin_defs.h"
#include "eth.h"

#if MICROPY_PY_LWIP
#include "lwip/etharp.h"
#include "netif/ethernet.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#endif

#define ETH_RXBUF_NUM   ENET_RXBUF_NUM
#define ETH_TXBUF_NUM   ENET_TXBUF_NUM

// RMII pinout, taken from the LCKFB "Sky Star" base-board pin table
// (doc/天空星筑基学习板-引脚分配总表.xlsx), all AF11:
//   REF_CLK = PA1, MDIO = PA2, CRS_DV = PA7
//   MDC     = PC1, RXD0 = PC4, RXD1 = PC5
//   TX_EN   = PB11, TXD0 = PB12, TXD1 = PB13
//   PHY_NRST = PE12 (plain GPIO, active low)
//
// CRS_DV (PA7) was missing here and the PHY is held in hardware reset by PE12,
// which nothing drove: without the first the MAC never sees carrier sense and
// never receives, without the second the PHY may never leave reset at all.
#define ETH_AF          GPIO_AF_11
#define ETH_PHY_RST_RCU   RCU_GPIOE
#define ETH_PHY_RST_PORT  GPIOE
#define ETH_PHY_RST_PIN   GPIO_PIN_12

static eth_t eth_instance;

// The instance that is really in use.  network_lan.c owns its own eth_t, so the
// interrupt handler and the lwIP netif callbacks must not assume the static
// eth_instance above: doing so handed lwIP a netif whose input hook was still
// NULL (the registered one is a different object), and the first received frame
// hard-faulted inside the ISR.
static eth_t *eth_active;

// Last PHY identity seen on the MDIO bus.  Kept for diagnostics: read them over
// SWD with `nm` to find the address, no need to expose them to Python.
static uint16_t self_phy_id1;
// Bring-up counters: how many frames lwIP handed to the MAC, and how many the
// receive interrupt actually drained.  Without these two numbers "no traffic"
// cannot be told apart from "nothing was ever queued for transmission".
volatile uint32_t eth_tx_frames;
volatile uint32_t eth_rx_frames;
volatile uint32_t eth_isr_count;

// Address of the last RX descriptor: the value the DMA needs in RDTADDR
// for the ring to be considered available.
static uint32_t eth_rx_desc_tail;
static uint16_t self_phy_id2;

// ---- helpers ----
static void eth_phy_hw_reset(void) {
    // The extension board wires the PHY's NRST to PE12.  Pulse it so the PHY
    // starts from a known state no matter what the previous firmware left it in.
    rcu_periph_clock_enable(ETH_PHY_RST_RCU);
    gpio_mode_set(ETH_PHY_RST_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, ETH_PHY_RST_PIN);
    gpio_output_options_set(ETH_PHY_RST_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, ETH_PHY_RST_PIN);
    gpio_bit_reset(ETH_PHY_RST_PORT, ETH_PHY_RST_PIN);      // assert reset (low)
    mp_hal_delay_ms(10);
    gpio_bit_set(ETH_PHY_RST_PORT, ETH_PHY_RST_PIN);        // release
    mp_hal_delay_ms(60);                                    // let the PHY boot
}

// Set the level held on the PHY NRST line (true = high, false = low), or
// release the pin entirely so the board's own reset circuit owns it.
static void eth_phy_rst_level(int mode) {
    if (mode < 0) {
        // High-Z: some carrier boards drive NRST from their own supervisor and
        // fighting it from the MCU keeps the PHY in reset.
        gpio_mode_set(ETH_PHY_RST_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, ETH_PHY_RST_PIN);
    } else {
        gpio_mode_set(ETH_PHY_RST_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, ETH_PHY_RST_PIN);
        gpio_output_options_set(ETH_PHY_RST_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,
                                ETH_PHY_RST_PIN);
        if (mode) {
            gpio_bit_set(ETH_PHY_RST_PORT, ETH_PHY_RST_PIN);
        } else {
            gpio_bit_reset(ETH_PHY_RST_PORT, ETH_PHY_RST_PIN);
        }
    }
    mp_hal_delay_ms(250);       // PHYs can take a while to come out of reset
}

static void eth_pin_init(void) {
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);

    // PA1 (REF_CLK), PA2 (MDIO), PA7 (CRS_DV).
    gpio_af_set(GPIOA, ETH_AF, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1 | GPIO_PIN_7);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,
                            GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7);

    // PC1 (MDC), PC4 (RXD0), PC5 (RXD1).
    gpio_af_set(GPIOC, ETH_AF, GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4 | GPIO_PIN_5);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,
                            GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5);

    // PB11 (TX_EN), PB12 (TXD0), PB13 (TXD1).
    gpio_af_set(GPIOB, ETH_AF, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,
                            GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13);
}

// Set up the MDC (management clock) divider.
//
// enet_phy_write_read() does not touch ENET_MAC_PHY_CTL's CLR field - it only
// drives the operation - so the divider has to be programmed before any PHY
// access.  The library does this inside enet_phy_config(), which runs from
// enet_init(), i.e. too late for a probe done beforehand.  At 168 MHz HCLK the
// correct setting is HCLK/102.
static void eth_mdc_clk_init(void) {
    uint32_t ahbclk = rcu_clock_freq_get(CK_AHB);
    uint32_t div;
    if (ahbclk > 150000000u) {
        div = ENET_MDC_HCLK_DIV102;
    } else if (ahbclk > 100000000u) {
        div = ENET_MDC_HCLK_DIV62;
    } else if (ahbclk > 60000000u) {
        div = ENET_MDC_HCLK_DIV42;
    } else if (ahbclk > 35000000u) {
        div = ENET_MDC_HCLK_DIV26;
    } else {
        div = ENET_MDC_HCLK_DIV16;
    }
    uint32_t reg = ENET_MAC_PHY_CTL;
    reg &= ~MAC_PHY_CTL_CLR(0x7u);
    reg |= div;
    ENET_MAC_PHY_CTL = reg;
}

// Read the PHY identifier registers and find which MDIO address answers.
// The GD32 library hard-codes PHY_ADDRESS (1); if the board's PHY strapping
// puts it somewhere else the library's auto-negotiation silently fails, so
// report the address that actually responds.
static int eth_phy_find(uint32_t *found_addr) {
    for (uint16_t addr = 0; addr < 32; addr++) {
        uint16_t id1 = 0, id2 = 0;
        if (enet_phy_write_read(ENET_PHY_READ, addr, 2, &id1) != SUCCESS) {
            continue;
        }
        if (enet_phy_write_read(ENET_PHY_READ, addr, 3, &id2) != SUCCESS) {
            continue;
        }
        // 0x0000 and 0xFFFF both mean "nothing there".
        if (id1 != 0x0000 && id1 != 0xFFFF) {
            self_phy_id1 = id1;
            self_phy_id2 = id2;
            *found_addr = addr;
            return 0;
        }
    }
    return -1;
}

static void eth_clk_init(void) {
    // The MAC needs its core, TX and RX clocks; the 50 MHz RMII reference comes
    // from the PHY on the REF_CLK pin.
    rcu_periph_clock_enable(RCU_ENET);
    rcu_periph_clock_enable(RCU_ENETTX);
    rcu_periph_clock_enable(RCU_ENETRX);
    rcu_periph_clock_enable(RCU_SYSCFG);

    // GD32F4 selects MII vs RMII through SYSCFG, not through the RCU.
    syscfg_enet_phy_interface_config(SYSCFG_ENET_PHY_RMII);
}

// ---- PHY handling ----
// GD32's enet_phy_config() only resets the PHY; when enet_init() is called
// without a link it then *forces* the media mode by writing BCR = 100M full
// duplex, which clears the auto-negotiation enable bit.  A partner that expects
// negotiation (most switches) can then never bring the link up, so the PHY has
// to be put back into auto-negotiation ourselves, with all four modes advertised.
#define PHY_REG_ADVERTISE   4u
#define PHY_ADV_ALL         0x01E1u     // 100F/100H/10F/10H + 802.3 selector

static void eth_phy_start_autoneg(void) {
    uint16_t adv = PHY_ADV_ALL;
    enet_phy_write_read(ENET_PHY_WRITE, PHY_ADDRESS, PHY_REG_ADVERTISE, &adv);

    uint16_t bcr = 0;
    if (enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_REG_BCR, &bcr) != SUCCESS) {
        return;
    }
    bcr &= ~(PHY_POWERDOWN | PHY_ISOLATE);
    bcr |= PHY_AUTONEGOTIATION | PHY_RESTART_AUTONEGOTIATION;
    enet_phy_write_read(ENET_PHY_WRITE, PHY_ADDRESS, PHY_REG_BCR, &bcr);
}

// Copy the negotiated speed/duplex into the MAC.
static void eth_mac_set_media(bool full_duplex, bool speed100) {
    uint32_t cfg = ENET_MAC_CFG;
    if (speed100) {
        cfg |= ENET_MAC_CFG_SPD;
    } else {
        cfg &= ~ENET_MAC_CFG_SPD;
    }
    if (full_duplex) {
        cfg |= ENET_MAC_CFG_DPM;
    } else {
        cfg &= ~ENET_MAC_CFG_DPM;
    }
    ENET_MAC_CFG = cfg;
}

// GD32's enet_phy_config() runs a full auto-negotiation and reports the
// resulting link parameters, so we just poll it.
static bool eth_phy_link_up(eth_t *self) {
    uint16_t bsr = 0;
    if (enet_phy_write_read(ENET_PHY_READ, (uint16_t)self->phy_addr,
                            PHY_REG_BSR, &bsr) != SUCCESS) {
        return false;
    }
    // Bit 2 = link status.
    if ((bsr & PHY_LINKED_STATUS) == 0) {
        return false;
    }
    // Link is up: latch the negotiated media into the MAC.  PHY_SR/PHY_SPEED_STATUS
    // /PHY_DUPLEX_STATUS come from the library and are selected by PHY_TYPE.
    uint16_t sr = 0;
    if (enet_phy_write_read(ENET_PHY_READ, (uint16_t)self->phy_addr, PHY_SR, &sr) == SUCCESS) {
        eth_mac_set_media((sr & PHY_DUPLEX_STATUS) != 0,
                          (sr & PHY_SPEED_STATUS) != 0);
    }
    return true;
}

// ---- lwIP netif callbacks ----
#if MICROPY_PY_LWIP
static err_t eth_netif_output(struct netif *netif, struct pbuf *p) {
    (void)netif;

    // GD32's enet_frame_transmit expects one contiguous buffer.
    static uint8_t txbuf[ENET_MAX_FRAME_SIZE];
    if (p->tot_len > sizeof(txbuf)) {
        return ERR_BUF;
    }
    pbuf_copy_partial(p, txbuf, p->tot_len, 0);

    if (enet_frame_transmit(txbuf, p->tot_len) != SUCCESS) {
        return ERR_IF;
    }
    eth_tx_frames++;
    return ERR_OK;
}

static err_t eth_netif_init(struct netif *netif) {
    netif->hwaddr_len = 6;
    memcpy(netif->hwaddr, eth_active->mac, 6);
    netif->mtu = 1500;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET;
    #if LWIP_IPV6_MLD
    netif->flags |= NETIF_FLAG_MLD6;
    #endif
    netif->output = etharp_output;
    netif->linkoutput = eth_netif_output;
    return ERR_OK;
}
#endif // MICROPY_PY_LWIP

// ---- public API ----
// Default MAC from the factory 96-bit UID, so every board gets a stable,
// locally-administered address instead of whatever happened to be in .bss.
// (eth_init used to do `eth_set_mac(self, self->mac)`, copying the uninitialised
// field onto itself, so the MAC was garbage.)
static void eth_mac_from_uid(uint8_t *mac) {
    const uint32_t *uid = (const uint32_t *)0x1FFF7A10u;
    mac[0] = 0x02;                                  // locally administered, unicast
    mac[1] = (uint8_t)(uid[0] >> 8);
    mac[2] = (uint8_t)(uid[0] >> 16);
    mac[3] = (uint8_t)(uid[0] >> 24);
    mac[4] = (uint8_t)(uid[1]);
    mac[5] = (uint8_t)(uid[1] >> 8);
}

static bool eth_mac_is_unset(const uint8_t *mac) {
    for (int i = 0; i < 6; i++) {
        if (mac[i] != 0) {
            return false;
        }
    }
    return true;
}

int eth_init(eth_t *self, uint32_t phy_addr) {
    if (self == NULL) {
        self = &eth_instance;
    }

    self->initialized = false;
    eth_active = self;

    eth_clk_init();
    eth_pin_init();
    eth_phy_hw_reset();

    enet_deinit();

    // The MAC must be out of reset and its MDC divider programmed before MDIO
    // can be driven, so this comes before enet_init().
    eth_mdc_clk_init();

    // Probe BEFORE enet_init(): enet_init(ENET_AUTO_NEGOTIATION, ...) waits for
    // the PHY to report a link, and a board with the cable unplugged (or with no
    // PHY at all) would just time out with a useless error.  Probing first tells
    // the two cases apart and lets us bring the interface up without a cable.
    uint32_t found = 0xFFFF;
    if (eth_phy_find(&found) != 0) {
        // Nothing answered with the reset line released high.  Try the other
        // level, then release the pin entirely (high-Z) in case the carrier
        // board drives NRST from its own supervisor.  A few hundred ms of
        // probing turns "no hardware" into a reliable verdict.
        for (int mode = 0; mode >= -1; mode--) {
            eth_phy_rst_level(mode);
            if (eth_phy_find(&found) == 0) {
                break;
            }
        }
        if (found == 0xFFFF) {
            eth_phy_rst_level(1);                   // leave the line released
            return -3;                              // no PHY on the MDIO bus
        }
    }
    self->phy_addr = found;
    mp_printf(&mp_plat_print, "eth: PHY at MDIO addr %u, id %04x:%04x\r\n",
              (unsigned)found, (unsigned)self_phy_id1, (unsigned)self_phy_id2);
    // The library functions take no address parameter, they use PHY_ADDRESS,
    // so the discovered address has to match what the library was built with.
    if (found != (uint32_t)PHY_ADDRESS) {
        return -4;
    }

    // Read the link bit ourselves to pick a media mode enet_init() can satisfy.
    uint16_t bsr = 0;
    bool link_up = false;
    if (enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_REG_BSR, &bsr) == SUCCESS) {
        link_up = (bsr & PHY_LINKED_STATUS) != 0;
    }

    enet_mediamode_enum media = link_up
        ? ENET_AUTO_NEGOTIATION
        : (enet_mediamode_enum)(ENET_MODE_FULLDUPLEX | ENET_SPEEDMODE_100M);

    if (enet_init(media, ENET_NO_AUTOCHECKSUM, ENET_BROADCAST_FRAMES_PASS) != SUCCESS) {
        return -1;
    }

    // The vendor library's enet_init() does NOT build the DMA descriptor rings -
    // enet_descriptors_chain_init() has to be called explicitly.  Without it the
    // MAC has no descriptors at all, so the link comes up and MDIO works while
    // not a single frame is ever received or transmitted (ARP never resolves,
    // every connect() ends in EHOSTUNREACH).
    enet_descriptors_chain_init(ENET_DMA_TX);
    enet_descriptors_chain_init(ENET_DMA_RX);

    // Always hand the PHY back to auto-negotiation, even when enet_init() just
    // forced a mode (it does that whenever no link was present at boot).
    eth_phy_start_autoneg();

    // Latch the negotiated media mode into the MAC.
    enet_initpara_config(STORE_OPTION, ENET_RX_MODE_STOREFORWARD);

    // Arm the RX ring BEFORE starting the receiver.
    //
    // enet_descriptors_chain_init() leaves RDTADDR equal to the head of the
    // table, and enet_rx_enable() immediately starts the receive process.  If
    // the tail is still on the head at that moment the controller concludes it
    // has no buffer it may fill, stops, and never recovers - writing the tail
    // pointer afterwards is too late: frames stop being delivered after the
    // first one or two, and even MAC internal loopback produces nothing.  So
    // the tail goes last-descriptor first, then the receiver is enabled.
    eth_rx_desc_tail = ENET_DMA_RDTADDR
        + (ENET_RXBUF_NUM - 1) * sizeof(enet_descriptors_struct);
    ENET_DMA_RDTADDR = eth_rx_desc_tail;

    enet_enable();
    enet_rx_enable();
    enet_tx_enable();


    // The ENET interrupt line stays off: reception is polled from SysTick (see
    // eth_poll()).  Enabling it as well would let the ISR and the poll race for
    // the same descriptors.

    if (eth_mac_is_unset(self->mac)) {
        eth_mac_from_uid(self->mac);
    }
    eth_set_mac(self, self->mac);
    self->initialized = true;
    return 0;
}

bool eth_link_status(eth_t *self) {
    if (self == NULL) {
        self = &eth_instance;
    }
    return self->initialized && eth_phy_link_up(self);
}

void eth_set_mac(eth_t *self, const uint8_t *mac) {
    if (self == NULL) {
        self = &eth_instance;
    }
    memcpy(self->mac, mac, 6);
}

#if MICROPY_PY_LWIP
struct netif *eth_netif(eth_t *self) {
    return &self->netif;
}

err_t eth_netif_add(eth_t *self) {
    struct netif *n = netif_add(&self->netif, IP4_ADDR_ANY4, IP4_ADDR_ANY4,
                                IP4_ADDR_ANY4, self, eth_netif_init, ethernet_input);
    return n == NULL ? ERR_IF : ERR_OK;
}
#endif

// ---- receive path ----
// Reception is driven by eth_poll() from SysTick, not from the ENET interrupt.
//
// With RIE, NIE and the NVIC line all enabled, frames still piled up in the RX
// ring with RS set and RBU raised: the descriptors were filled by the DMA but
// never handed to lwIP.  A polled drain is deterministic, cannot race with the
// main loop, and gives lwIP the periodic tick (sys_check_timeouts) that a NO_SYS
// port needs for ARP/TCP timers - this port never had that hook at all, so DHCP
// and ARP retransmissions could not have worked either.
// Walk the RX descriptor table directly.
//
// The vendor helpers keep their idea of "the current descriptor" in
// dma_current_rxdesc and only ever look at that one entry via
// enet_rxframe_size_get().  When the DMA fills a different entry, the frame is
// invisible: the ring showed desc0/desc1 still owned by the DMA, a complete
// 64-byte frame sitting in desc2, and the poll seeing nothing at all.  Reading
// every entry ourselves is both simpler and immune to that bookkeeping drift.
extern enet_descriptors_struct rxdesc_tab[ENET_RXBUF_NUM];

void eth_poll(void) {
    if (eth_active == NULL || !eth_active->initialized) {
        return;
    }

    // Status bits latched by the controller are informational here.
    uint32_t pending = ENET_DMA_STAT;
    ENET_DMA_STAT = pending;

    #if MICROPY_PY_LWIP
    struct netif *netif = &eth_active->netif;

    for (uint32_t i = 0; i < ENET_RXBUF_NUM; i++) {
        enet_descriptors_struct *d = &rxdesc_tab[i];
        uint32_t status = d->status;

        if (status & ENET_RDES0_DAV) {
            continue;                       // still owned by the DMA
        }
        if ((status & ENET_RDES0_ERRS)
            || !(status & ENET_RDES0_LDES)
            || !(status & ENET_RDES0_FDES)) {
            d->status = ENET_RDES0_DAV;     // bad or fragmented frame: drop, re-arm
            continue;
        }

        uint32_t frame_len = GET_RDES0_FRML(status);
        if (frame_len < 14 || frame_len > ENET_MAX_FRAME_SIZE) {
            d->status = ENET_RDES0_DAV;
            continue;
        }
        frame_len -= 4;                     // the DMA count includes the CRC

        struct pbuf *p = NULL;
        if (netif->input != NULL) {
            p = pbuf_alloc(PBUF_RAW, frame_len, PBUF_POOL);
            if (p != NULL) {
                // Copy before re-arming: once the descriptor is handed back the
                // DMA may overwrite its buffer at any moment.
                memcpy(p->payload, (const void *)d->buffer1_addr, frame_len);
            }
        }
        d->status = ENET_RDES0_DAV;         // give the buffer back

        if (p == NULL) {
            continue;
        }
        eth_rx_frames++;
        if (netif->input(p, netif) != ERR_OK) {
            pbuf_free(p);
        }
    }

    // Run the lwIP timers (ARP retries, TCP retransmit, DHCP).
    extern void sys_check_timeouts(void);
    sys_check_timeouts();
    #endif

    // Re-advertise the whole ring and resume the receive process if it parked.
    uint32_t st = ENET_DMA_STAT;
    if (st & (ENET_DMA_STAT_RBU | ENET_DMA_STAT_RPS)) {
        ENET_DMA_STAT = st & (ENET_DMA_STAT_RBU | ENET_DMA_STAT_RPS);
    }
    ENET_DMA_RDTADDR = (uint32_t)&rxdesc_tab[ENET_RXBUF_NUM - 1];
    ENET_DMA_RPEN = 0U;                     // poll demand: resume reception
}

// Still installed as the vector in case anything enables the line, but it only
// clears the flags: the work happens in eth_poll().
void ENET_IRQHandler(void) {
    eth_isr_count++;
    uint32_t pending = ENET_DMA_STAT;
    ENET_DMA_STAT = pending;
}
