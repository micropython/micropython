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

// RMII pinout on GD32F4 (all AF11).
//   REF_CLK = PA1, MDIO = PA2
//   MDC     = PC1, RXD0 = PC4, RXD1 = PC5
//   TX_EN   = PB11, TXD0 = PB12, TXD1 = PB13
#define ETH_AF          GPIO_AF_11

static eth_t eth_instance;

// ---- helpers ----
static void eth_pin_init(void) {
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);

    // PA1 (REF_CLK) and PA2 (MDIO).
    gpio_af_set(GPIOA, ETH_AF, GPIO_PIN_1 | GPIO_PIN_2);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1 | GPIO_PIN_2);

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
// GD32's enet_phy_config() runs a full auto-negotiation and reports the
// resulting link parameters, so we just poll it.
static bool eth_phy_link_up(eth_t *self) {
    uint16_t bsr = 0;
    if (enet_phy_write_read(ENET_PHY_READ, (uint16_t)self->phy_addr,
                            PHY_REG_BSR, &bsr) != SUCCESS) {
        return false;
    }
    // Bit 2 = link status.
    return (bsr & 0x0004) != 0;
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
    return ERR_OK;
}

static err_t eth_netif_init(struct netif *netif) {
    netif->hwaddr_len = 6;
    memcpy(netif->hwaddr, eth_instance.mac, 6);
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
int eth_init(eth_t *self, uint32_t phy_addr) {
    if (self == NULL) {
        self = &eth_instance;
    }

    self->phy_addr = phy_addr;
    self->initialized = false;

    eth_clk_init();
    eth_pin_init();

    enet_deinit();
    // 100 Mbit full duplex with auto-negotiation, store-and-forward RX.
    if (enet_init(ENET_AUTO_NEGOTIATION, ENET_NO_AUTOCHECKSUM,
                  ENET_BROADCAST_FRAMES_PASS) != SUCCESS) {
        return -1;
    }

    // Configure the PHY (auto-negotiation, 100M full duplex advertised).
    if (enet_phy_config() != SUCCESS) {
        return -2;
    }

    // Latch the negotiated media mode into the MAC.
    enet_initpara_config(STORE_OPTION, ENET_RX_MODE_STOREFORWARD);

    enet_enable();
    enet_rx_enable();
    enet_tx_enable();

    // Enable receive-complete interrupts.
    enet_interrupt_enable(ENET_DMA_INT_RIE);
    enet_interrupt_enable(ENET_DMA_INT_NIE);
    NVIC_SetPriority(ENET_IRQn, 2);
    NVIC_EnableIRQ(ENET_IRQn);

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

// ---- interrupt ----
// Pull every complete frame out of the DMA ring and hand it to lwIP.
void ENET_IRQHandler(void) {
    if (enet_interrupt_flag_get(ENET_DMA_INT_FLAG_RS) == RESET) {
        enet_interrupt_flag_clear(ENET_DMA_INT_FLAG_NI);
        return;
    }
    enet_interrupt_flag_clear(ENET_DMA_INT_FLAG_RS);

    #if MICROPY_PY_LWIP
    struct netif *netif = &eth_instance.netif;
    #endif

    for (;;) {
        uint32_t frame_len = enet_rxframe_size_get();
        if (frame_len == 0) {
            break;
        }

        #if MICROPY_PY_LWIP
        struct pbuf *p = pbuf_alloc(PBUF_RAW, frame_len, PBUF_POOL);
        if (p == NULL) {
            // Out of pbufs: drop the frame but keep the ring moving.
            uint8_t discard[ENET_MAX_FRAME_SIZE];
            enet_frame_receive(discard, sizeof(discard));
            continue;
        }
        if (enet_frame_receive(p->payload, frame_len) != SUCCESS) {
            pbuf_free(p);
            continue;
        }
        if (netif->input(p, netif) != ERR_OK) {
            pbuf_free(p);
        }
        #else
        uint8_t discard[ENET_MAX_FRAME_SIZE];
        enet_frame_receive(discard, sizeof(discard));
        #endif
    }

    enet_rxprocess_check_recovery();
}
