/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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
#include "py/mphal.h"
#include "py/mperrno.h"
#include "shared/netutils/netutils.h"
#include "pin_static_af.h"
#include "extmod/modnetwork.h"
#include "mpu.h"
#include "eth.h"
#include "eth_phy.h"

#if defined(MICROPY_HW_ETH_MDC)

#include "lwip/etharp.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
#include "netif/ethernet.h"

// ETH DMA RX and TX descriptor definitions
#if defined(STM32H5)
#define RX_DESCR_3_OWN_Pos      (31)
#define RX_DESCR_3_IOC_Pos      (30)
#define RX_DESCR_3_BUF1V_Pos    (24)
#define RX_DESCR_3_PL_Msk       (0x7fff)

#define TX_DESCR_3_OWN_Pos      (31)
#define TX_DESCR_3_FD_Pos       (29)
#define TX_DESCR_3_LD_Pos       (28)
#define TX_DESCR_3_CIC_Pos      (16)
#define TX_DESCR_2_B1L_Pos      (0)
#define TX_DESCR_2_B1L_Msk      (0x3fff << TX_DESCR_2_B1L_Pos)
#elif defined(STM32H7)
#define RX_DESCR_3_OWN_Pos      (31)
#define RX_DESCR_3_IOC_Pos      (30)
#define RX_DESCR_3_BUF1V_Pos    (24)
#define RX_DESCR_3_PL_Msk       (0x7fff)

#define TX_DESCR_3_OWN_Pos      (31)
#define TX_DESCR_3_LD_Pos       (29)
#define TX_DESCR_3_FD_Pos       (28)
#define TX_DESCR_3_CIC_Pos      (16)
#define TX_DESCR_2_B1L_Pos      (0)
#define TX_DESCR_2_B1L_Msk      (0x3fff << TX_DESCR_2_B1L_Pos)
#else
#define RX_DESCR_0_OWN_Pos      (31)
#define RX_DESCR_0_FL_Pos       (16)
#define RX_DESCR_0_FL_Msk       (0x3fff << RX_DESCR_0_FL_Pos)
#define RX_DESCR_1_RER_Pos      (15)
#define RX_DESCR_1_RCH_Pos      (14)
#define RX_DESCR_1_RBS2_Pos     (16)
#define RX_DESCR_1_RBS1_Pos     (0)

#define TX_DESCR_0_OWN_Pos      (31)
#define TX_DESCR_0_LS_Pos       (29)
#define TX_DESCR_0_FS_Pos       (28)
#define TX_DESCR_0_DP_Pos       (26)
#define TX_DESCR_0_CIC_Pos      (22)
#define TX_DESCR_0_TER_Pos      (21)
#define TX_DESCR_0_TCH_Pos      (20)
#define TX_DESCR_1_TBS1_Pos     (0)
#endif

// Configuration values

#define PHY_INIT_TIMEOUT_MS (10000)
#define PHY_AUTONEG_TIMEOUT_MS (5000)

#define RX_BUF_SIZE (1524) // includes 4-byte CRC at end
#define TX_BUF_SIZE (1524)

#define RX_BUF_NUM (5)
#define TX_BUF_NUM (5)

typedef struct _eth_dma_rx_descr_t {
    volatile uint32_t rdes0, rdes1, rdes2, rdes3;
} eth_dma_rx_descr_t;

typedef struct _eth_dma_tx_descr_t {
    volatile uint32_t tdes0, tdes1, tdes2, tdes3;
} eth_dma_tx_descr_t;

typedef struct _eth_dma_t {
    eth_dma_rx_descr_t rx_descr[RX_BUF_NUM];
    eth_dma_tx_descr_t tx_descr[TX_BUF_NUM];
    uint8_t rx_buf[RX_BUF_NUM * RX_BUF_SIZE] __attribute__((aligned(4)));
    uint8_t tx_buf[TX_BUF_NUM * TX_BUF_SIZE] __attribute__((aligned(4)));
    size_t rx_descr_idx;
    size_t tx_descr_idx;
    uint8_t padding[16384 - 15408];
} eth_dma_t;

typedef struct _eth_t {
    uint32_t trace_flags;
    struct netif netif;
    struct dhcp dhcp_struct;
    uint32_t phy_addr;
    int16_t (*phy_get_link_status)(uint32_t phy_addr);
    bool last_link_status;
    bool enabled;
    bool mac_speed_configured;
    uint32_t autoneg_start_ms;
} eth_t;

static eth_dma_t eth_dma MICROPY_HW_ETH_DMA_ATTRIBUTE;

eth_t eth_instance;

static void eth_mac_deinit(eth_t *self);
static void eth_process_frame(eth_t *self, size_t len, const uint8_t *buf);
static void eth_lwip_init(eth_t *self);
static int eth_phy_init(eth_t *self);

void eth_phy_write(uint32_t phy_addr, uint32_t reg, uint32_t val) {
    #if defined(STM32H5) || defined(STM32H7)
    while (ETH->MACMDIOAR & ETH_MACMDIOAR_MB) {
    }
    uint32_t ar = ETH->MACMDIOAR;
    ar &= ~ETH_MACMDIOAR_PA_Msk;
    ar |= (phy_addr << ETH_MACMDIOAR_PA_Pos);
    ar &= ~ETH_MACMDIOAR_RDA_Msk;
    ar |= reg << ETH_MACMDIOAR_RDA_Pos;
    ar &= ~ETH_MACMDIOAR_MOC_Msk;
    ar |= ETH_MACMDIOAR_MOC_WR;
    ar |= ETH_MACMDIOAR_MB;
    ETH->MACMDIODR = val;
    ETH->MACMDIOAR = ar;
    while (ETH->MACMDIOAR & ETH_MACMDIOAR_MB) {
    }
    #else
    while (ETH->MACMIIAR & ETH_MACMIIAR_MB) {
    }
    ETH->MACMIIDR = val;
    uint32_t ar = ETH->MACMIIAR;
    ar = (phy_addr << ETH_MACMIIAR_PA_Pos) | (reg << ETH_MACMIIAR_MR_Pos) | (ar & ETH_MACMIIAR_CR_Msk) | ETH_MACMIIAR_MW | ETH_MACMIIAR_MB;
    ETH->MACMIIAR = ar;
    while (ETH->MACMIIAR & ETH_MACMIIAR_MB) {
    }
    #endif
}

uint32_t eth_phy_read(uint32_t phy_addr, uint32_t reg) {
    #if defined(STM32H5) || defined(STM32H7)
    while (ETH->MACMDIOAR & ETH_MACMDIOAR_MB) {
    }
    uint32_t ar = ETH->MACMDIOAR;
    ar &= ~ETH_MACMDIOAR_PA_Msk;
    ar |= (phy_addr << ETH_MACMDIOAR_PA_Pos);
    ar &= ~ETH_MACMDIOAR_RDA_Msk;
    ar |= reg << ETH_MACMDIOAR_RDA_Pos;
    ar &= ~ETH_MACMDIOAR_MOC_Msk;
    ar |= ETH_MACMDIOAR_MOC_RD;
    ar |= ETH_MACMDIOAR_MB;
    ETH->MACMDIOAR = ar;
    while (ETH->MACMDIOAR & ETH_MACMDIOAR_MB) {
    }
    return ETH->MACMDIODR;
    #else
    while (ETH->MACMIIAR & ETH_MACMIIAR_MB) {
    }
    uint32_t ar = ETH->MACMIIAR;
    ar = (phy_addr << ETH_MACMIIAR_PA_Pos) | (reg << ETH_MACMIIAR_MR_Pos) | (ar & ETH_MACMIIAR_CR_Msk) | ETH_MACMIIAR_MB;
    ETH->MACMIIAR = ar;
    while (ETH->MACMIIAR & ETH_MACMIIAR_MB) {
    }
    return ETH->MACMIIDR;
    #endif
}

int eth_init(eth_t *self, int mac_idx, uint32_t phy_addr, int phy_type) {
    if (self->netif.input != NULL) {
        // Already initialised.
        return 0;
    }
    mp_hal_get_mac(mac_idx, &self->netif.hwaddr[0]);
    self->netif.hwaddr_len = 6;
    self->phy_addr = phy_addr;
    self->last_link_status = false;
    self->enabled = false;
    if (phy_type == ETH_PHY_DP83825 || phy_type == ETH_PHY_DP83848) {
        self->phy_get_link_status = eth_phy_dp838xx_get_link_status;
    } else if (phy_type == ETH_PHY_LAN8720 || phy_type == ETH_PHY_LAN8742) {
        self->phy_get_link_status = eth_phy_lan87xx_get_link_status;
    } else {
        return -1;
    }

    // Configure GPIO
    mp_hal_pin_config_alt_static(MICROPY_HW_ETH_MDC, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, STATIC_AF_ETH_MDC);
    mp_hal_pin_config_alt_static(MICROPY_HW_ETH_MDIO, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, STATIC_AF_ETH_MDIO);
    mp_hal_pin_config_alt_static_speed(MICROPY_HW_ETH_RMII_REF_CLK, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_MEDIUM, STATIC_AF_ETH_RMII_REF_CLK);
    mp_hal_pin_config_alt_static(MICROPY_HW_ETH_RMII_CRS_DV, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, STATIC_AF_ETH_RMII_CRS_DV);
    mp_hal_pin_config_alt_static(MICROPY_HW_ETH_RMII_RXD0, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, STATIC_AF_ETH_RMII_RXD0);
    mp_hal_pin_config_alt_static(MICROPY_HW_ETH_RMII_RXD1, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, STATIC_AF_ETH_RMII_RXD1);
    mp_hal_pin_config_alt_static(MICROPY_HW_ETH_RMII_TX_EN, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, STATIC_AF_ETH_RMII_TX_EN);
    mp_hal_pin_config_alt_static(MICROPY_HW_ETH_RMII_TXD0, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, STATIC_AF_ETH_RMII_TXD0);
    mp_hal_pin_config_alt_static(MICROPY_HW_ETH_RMII_TXD1, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, STATIC_AF_ETH_RMII_TXD1);

    // Enable peripheral clock
    #if defined(STM32H5)
    __HAL_RCC_ETH_CLK_ENABLE();
    __HAL_RCC_ETHTX_CLK_ENABLE();
    __HAL_RCC_ETHRX_CLK_ENABLE();
    #elif defined(STM32H7)
    __HAL_RCC_ETH1MAC_CLK_ENABLE();
    __HAL_RCC_ETH1TX_CLK_ENABLE();
    __HAL_RCC_ETH1RX_CLK_ENABLE();
    #else
    __HAL_RCC_ETH_CLK_ENABLE();
    #endif

    // Initialize netif and register with LWIP
    eth_lwip_init(self);

    return 0;
}

void eth_set_trace(eth_t *self, uint32_t value) {
    self->trace_flags = value;
}

static int eth_mac_init(eth_t *self) {
    // Configure MPU
    uint32_t irq_state = mpu_config_start();
    #if defined(STM32H5)
    mpu_config_region(MPU_REGION_ETH, (uint32_t)&eth_dma, MPU_CONFIG_ETH(16 * 1024));
    #else
    mpu_config_region(MPU_REGION_ETH, (uint32_t)&eth_dma, MPU_CONFIG_ETH(MPU_REGION_SIZE_16KB));
    #endif
    mpu_config_end(irq_state);

    // Set MAC to reset state
    #if defined(STM32H5)
    __HAL_RCC_ETH_FORCE_RESET();
    #elif defined(STM32H7)
    __HAL_RCC_ETH1MAC_FORCE_RESET();
    #else
    __HAL_RCC_ETHMAC_FORCE_RESET();
    #endif

    // Select RMII interface
    #if defined(STM32H5)
    __HAL_RCC_SBS_CLK_ENABLE();
    SBS->PMCR = (SBS->PMCR & ~SBS_PMCR_ETH_SEL_PHY_Msk) | SBS_PMCR_ETH_SEL_PHY_2;
    #elif defined(STM32H7)
    SYSCFG->PMCR = (SYSCFG->PMCR & ~SYSCFG_PMCR_EPIS_SEL_Msk) | SYSCFG_PMCR_EPIS_SEL_2;
    #else
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    SYSCFG->PMC |= SYSCFG_PMC_MII_RMII_SEL;
    #endif

    #if defined(STM32H5)
    __HAL_RCC_ETH_RELEASE_RESET();
    #elif defined(STM32H7)
    __HAL_RCC_ETH1MAC_RELEASE_RESET();
    #else
    __HAL_RCC_ETHMAC_RELEASE_RESET();
    #endif

    // Do a soft reset of the MAC core
    #if defined(STM32H5) || defined(STM32H7)
    #define ETH_SOFT_RESET(eth) do { eth->DMAMR = ETH_DMAMR_SWR; } while (0)
    #define ETH_IS_RESET(eth) (eth->DMAMR & ETH_DMAMR_SWR)
    #else
    #define ETH_SOFT_RESET(eth) do { eth->DMABMR = ETH_DMABMR_SR; } while (0)
    #define ETH_IS_RESET(eth) (eth->DMABMR & ETH_DMABMR_SR)
    #endif

    ETH_SOFT_RESET(ETH);
    mp_hal_delay_ms(2);

    // Wait for soft reset to finish
    uint32_t t0 = mp_hal_ticks_ms();
    while (ETH_IS_RESET(ETH)) {
        if (mp_hal_ticks_ms() - t0 > 1000) {
            return -MP_ETIMEDOUT;
        }
    }

    // Set MII clock range
    uint32_t hclk = HAL_RCC_GetHCLKFreq();
    uint32_t cr_div;
    #if defined(STM32H5)
    cr_div = ETH->MACMDIOAR & ~ETH_MACMDIOAR_CR;
    if (hclk < 35000000) {
        cr_div |= ETH_MACMDIOAR_CR_DIV16;
    } else if (hclk < 60000000) {
        cr_div |= ETH_MACMDIOAR_CR_DIV26;
    } else if (hclk < 100000000) {
        cr_div |= ETH_MACMDIOAR_CR_DIV42;
    } else if (hclk < 150000000) {
        cr_div |= ETH_MACMDIOAR_CR_DIV62;
    } else if (hclk < 250000000) {
        cr_div |= ETH_MACMDIOAR_CR_DIV102;
    } else {
        cr_div |= ETH_MACMDIOAR_CR_DIV124;
    }
    ETH->MACMDIOAR = cr_div;
    #elif defined(STM32H7)
    cr_div = ETH->MACMDIOAR & ~ETH_MACMDIOAR_CR;
    if (hclk < 35000000) {
        cr_div |= ETH_MACMDIOAR_CR_DIV16;
    } else if (hclk < 60000000) {
        cr_div |= ETH_MACMDIOAR_CR_DIV26;
    } else if (hclk < 100000000) {
        cr_div |= ETH_MACMDIOAR_CR_DIV42;
    } else if (hclk < 150000000) {
        cr_div |= ETH_MACMDIOAR_CR_DIV62;
    } else {
        cr_div |= ETH_MACMDIOAR_CR_DIV102;
    }
    ETH->MACMDIOAR = cr_div;
    #else
    if (hclk < 35000000) {
        cr_div = ETH_MACMIIAR_CR_Div16;
    } else if (hclk < 60000000) {
        cr_div = ETH_MACMIIAR_CR_Div26;
    } else if (hclk < 100000000) {
        cr_div = ETH_MACMIIAR_CR_Div42;
    } else if (hclk < 150000000) {
        cr_div = ETH_MACMIIAR_CR_Div62;
    } else {
        cr_div = ETH_MACMIIAR_CR_Div102;
    }
    ETH->MACMIIAR = cr_div;
    #endif

    #if defined(STM32H5) || defined(STM32H7)
    // don't skip 32bit words since our descriptors are continuous in memory
    ETH->DMACCR &= ~(ETH_DMACCR_DSL_Msk);
    #endif

    // Burst mode configuration
    #if defined(STM32H5) || defined(STM32H7)
    ETH->DMASBMR = ETH->DMASBMR & ~ETH_DMASBMR_AAL & ~ETH_DMASBMR_FB;
    #else
    ETH->DMABMR = 0;
    #endif
    mp_hal_delay_ms(2);

    // Select DMA interrupts
    #if defined(STM32H5) || defined(STM32H7)
    ETH->DMACIER = ETH->DMACIER
        | ETH_DMACIER_NIE // enable normal interrupts
        | ETH_DMACIER_RIE // enable RX interrupt
    ;
    #else
    ETH->DMAIER =
        ETH_DMAIER_NISE // enable normal interrupts
        | ETH_DMAIER_RIE // enable RX interrupt
    ;
    #endif

    // Configure RX descriptor lists
    for (size_t i = 0; i < RX_BUF_NUM; ++i) {
        #if defined(STM32H5) || defined(STM32H7)
        eth_dma.rx_descr[i].rdes3 =
            1 << RX_DESCR_3_OWN_Pos
                | (1 << RX_DESCR_3_BUF1V_Pos) // buf1 address valid
                | (1 << RX_DESCR_3_IOC_Pos) // Interrupt Enabled on Completion
        ;
        eth_dma.rx_descr[i].rdes0 = (uint32_t)&eth_dma.rx_buf[i * RX_BUF_SIZE]; // buf 1 address
        #else
        eth_dma.rx_descr[i].rdes0 = 1 << RX_DESCR_0_OWN_Pos;
        eth_dma.rx_descr[i].rdes1 =
            1 << RX_DESCR_1_RCH_Pos // chained
                | RX_BUF_SIZE << RX_DESCR_1_RBS1_Pos
        ;
        eth_dma.rx_descr[i].rdes2 = (uint32_t)&eth_dma.rx_buf[i * RX_BUF_SIZE];
        eth_dma.rx_descr[i].rdes3 = (uint32_t)&eth_dma.rx_descr[(i + 1) % RX_BUF_NUM];
        #endif
    }

    #if defined(STM32H5) || defined(STM32H7)
    ETH->DMACRDLAR = (uint32_t)&eth_dma.rx_descr[0];
    #else
    ETH->DMARDLAR = (uint32_t)&eth_dma.rx_descr[0];
    #endif
    eth_dma.rx_descr_idx = 0;

    // Configure TX descriptor lists
    for (size_t i = 0; i < TX_BUF_NUM; ++i) {
        #if defined(STM32H5) || defined(STM32H7)
        eth_dma.tx_descr[i].tdes0 = 0;
        eth_dma.tx_descr[i].tdes1 = 0;
        eth_dma.tx_descr[i].tdes2 = TX_BUF_SIZE & TX_DESCR_2_B1L_Msk;
        eth_dma.tx_descr[i].tdes3 = 0;
        #else
        eth_dma.tx_descr[i].tdes0 = 1 << TX_DESCR_0_TCH_Pos;
        eth_dma.tx_descr[i].tdes1 = 0;
        eth_dma.tx_descr[i].tdes2 = 0;
        eth_dma.tx_descr[i].tdes3 = (uint32_t)&eth_dma.tx_descr[(i + 1) % TX_BUF_NUM];
        #endif
    }

    #if defined(STM32H5) || defined(STM32H7)
    // set number of descriptors and buffers
    ETH->DMACTDRLR = TX_BUF_NUM - 1;
    ETH->DMACRDRLR = RX_BUF_NUM - 1;

    ETH->DMACTDLAR = (uint32_t)&eth_dma.tx_descr[0];
    #else
    ETH->DMATDLAR = (uint32_t)&eth_dma.tx_descr[0];
    #endif
    eth_dma.tx_descr_idx = 0;

    // Configure DMA
    #if defined(STM32H5) || defined(STM32H7)
    // read from RX FIFO only after a full frame is written
    ETH->MTLRQOMR = ETH_MTLRQOMR_RSF;
    // transmission starts when a full packet resides in the Tx queue
    ETH->MTLTQOMR = ETH_MTLTQOMR_TSF;
    #else
    ETH->DMAOMR =
        ETH_DMAOMR_RSF // read from RX FIFO after a full frame is written
        | ETH_DMAOMR_TSF // transmit when a full frame is in TX FIFO (needed by errata)
    ;
    #endif
    mp_hal_delay_ms(2);

    // Select MAC filtering options
    #if defined(STM32H5) || defined(STM32H7)
    ETH->MACPFR = ETH_MACPFR_RA; // pass all frames up
    #else
    ETH->MACFFR =
        ETH_MACFFR_RA // pass all frames up
    ;
    #endif
    mp_hal_delay_ms(2);

    // Set MAC address
    u8_t *mac = &self->netif.hwaddr[0];
    ETH->MACA0HR = mac[5] << 8 | mac[4];
    mp_hal_delay_ms(2);
    ETH->MACA0LR = mac[3] << 24 | mac[2] << 16 | mac[1] << 8 | mac[0];
    mp_hal_delay_ms(2);

    // Set main MAC control register with default configuration
    // The PHY speed/duplex will be auto-detected and updated via autonegotiation
    ETH->MACCR = ETH_MACCR_FES | ETH_MACCR_DM;  // Default: 100Mbps, Full Duplex
    mp_hal_delay_ms(2);

    // Start MAC layer
    ETH->MACCR |=
        ETH_MACCR_TE // enable TX
        | ETH_MACCR_RE // enable RX
    ;
    mp_hal_delay_ms(2);

    // Start DMA layer
    #if defined(STM32H5) || defined(STM32H7)
    ETH->DMACRCR |= ETH_DMACRCR_SR; // start RX
    ETH->DMACTCR |= ETH_DMACTCR_ST; // start TX
    #else
    ETH->DMAOMR |=
        ETH_DMAOMR_ST // start TX
        | ETH_DMAOMR_SR // start RX
    ;
    #endif
    mp_hal_delay_ms(2);

    // Enable interrupts
    NVIC_SetPriority(ETH_IRQn, IRQ_PRI_PENDSV);
    HAL_NVIC_EnableIRQ(ETH_IRQn);

    return 0;
}

static void eth_mac_deinit(eth_t *self) {
    (void)self;
    HAL_NVIC_DisableIRQ(ETH_IRQn);
    #if defined(STM32H5)
    __HAL_RCC_ETH_FORCE_RESET();
    __HAL_RCC_ETH_RELEASE_RESET();
    __HAL_RCC_ETH_CLK_DISABLE();
    #elif defined(STM32H7)
    __HAL_RCC_ETH1MAC_FORCE_RESET();
    __HAL_RCC_ETH1MAC_RELEASE_RESET();
    __HAL_RCC_ETH1MAC_CLK_DISABLE();
    #else
    __HAL_RCC_ETHMAC_FORCE_RESET();
    __HAL_RCC_ETHMAC_RELEASE_RESET();
    __HAL_RCC_ETH_CLK_DISABLE();
    #endif
}

static int eth_tx_buf_get(size_t len, uint8_t **buf) {
    if (len > TX_BUF_SIZE) {
        return -MP_EINVAL;
    }

    // Wait for DMA to release the current TX descriptor (if it has it)
    eth_dma_tx_descr_t *tx_descr = &eth_dma.tx_descr[eth_dma.tx_descr_idx];
    uint32_t t0 = mp_hal_ticks_ms();
    for (;;) {
        #if defined(STM32H5) || defined(STM32H7)
        if (!(tx_descr->tdes3 & (1 << TX_DESCR_3_OWN_Pos))) {
            break;
        }
        #else
        if (!(tx_descr->tdes0 & (1 << TX_DESCR_0_OWN_Pos))) {
            break;
        }
        #endif
        if (mp_hal_ticks_ms() - t0 > 1000) {
            return -MP_ETIMEDOUT;
        }
    }

    #if defined(STM32H5) || defined(STM32H7)
    // Update TX descriptor with length and buffer pointer
    *buf = &eth_dma.tx_buf[eth_dma.tx_descr_idx * TX_BUF_SIZE];
    tx_descr->tdes2 = len & TX_DESCR_2_B1L_Msk;
    tx_descr->tdes0 = (uint32_t)*buf;
    #else
    // Update TX descriptor with length, buffer pointer and linked list pointer
    *buf = &eth_dma.tx_buf[eth_dma.tx_descr_idx * TX_BUF_SIZE];
    tx_descr->tdes1 = len << TX_DESCR_1_TBS1_Pos;
    tx_descr->tdes2 = (uint32_t)*buf;
    tx_descr->tdes3 = (uint32_t)&eth_dma.tx_descr[(eth_dma.tx_descr_idx + 1) % TX_BUF_NUM];
    #endif

    return 0;
}

static int eth_tx_buf_send(void) {
    // Get TX descriptor and move to next one
    eth_dma_tx_descr_t *tx_descr = &eth_dma.tx_descr[eth_dma.tx_descr_idx];
    eth_dma.tx_descr_idx = (eth_dma.tx_descr_idx + 1) % TX_BUF_NUM;

    // Schedule to send next outgoing frame
    #if defined(STM32H5) || defined(STM32H7)
    tx_descr->tdes3 =
        1 << TX_DESCR_3_OWN_Pos     // owned by DMA
            | 1 << TX_DESCR_3_LD_Pos // last segment
            | 1 << TX_DESCR_3_FD_Pos // first segment
            | 3 << TX_DESCR_3_CIC_Pos // enable all checksums inserted by hardware
    ;
    #else
    tx_descr->tdes0 =
        1 << TX_DESCR_0_OWN_Pos     // owned by DMA
            | 1 << TX_DESCR_0_LS_Pos // last segment
            | 1 << TX_DESCR_0_FS_Pos // first segment
            | 3 << TX_DESCR_0_CIC_Pos // enable all checksums inserted by hardware
            | 1 << TX_DESCR_0_TCH_Pos // TX descriptor is chained
    ;
    #endif

    // Notify ETH DMA that there is a new TX descriptor for sending
    __DMB();
    #if defined(STM32H5) || defined(STM32H7)
    if (ETH->DMACSR & ETH_DMACSR_TBU) {
        ETH->DMACSR = ETH_DMACSR_TBU;
    }
    ETH->DMACTDTPR = (uint32_t)&eth_dma.tx_descr[eth_dma.tx_descr_idx];
    #else
    if (ETH->DMASR & ETH_DMASR_TBUS) {
        ETH->DMASR = ETH_DMASR_TBUS;
        ETH->DMATPDR = 0;
    }
    #endif

    return 0;
}

static void eth_dma_rx_free(void) {
    // Get RX descriptor, RX buffer and move to next one
    eth_dma_rx_descr_t *rx_descr = &eth_dma.rx_descr[eth_dma.rx_descr_idx];
    uint8_t *buf = &eth_dma.rx_buf[eth_dma.rx_descr_idx * RX_BUF_SIZE];
    eth_dma.rx_descr_idx = (eth_dma.rx_descr_idx + 1) % RX_BUF_NUM;

    // Schedule to get next incoming frame
    #if defined(STM32H5) || defined(STM32H7)
    rx_descr->rdes0 = (uint32_t)buf;
    rx_descr->rdes3 = 1 << RX_DESCR_3_OWN_Pos;  // owned by DMA
    rx_descr->rdes3 |= 1 << RX_DESCR_3_BUF1V_Pos; // buf 1 address valid
    rx_descr->rdes3 |= 1 << RX_DESCR_3_IOC_Pos; // Interrupt Enabled on Completion
    #else
    rx_descr->rdes1 =
        1 << RX_DESCR_1_RCH_Pos                 // RX descriptor is chained
            | RX_BUF_SIZE << RX_DESCR_1_RBS1_Pos // maximum buffer length
    ;
    rx_descr->rdes2 = (uint32_t)buf;
    rx_descr->rdes3 = (uint32_t)&eth_dma.rx_descr[eth_dma.rx_descr_idx];
    rx_descr->rdes0 = 1 << RX_DESCR_0_OWN_Pos;  // owned by DMA
    #endif

    // Notify ETH DMA that there is a new RX descriptor available
    __DMB();
    #if defined(STM32H5) || defined(STM32H7)
    ETH->DMACRDTPR = (uint32_t)&rx_descr[eth_dma.rx_descr_idx];
    #else
    ETH->DMARPDR = 0;
    #endif
}

void ETH_IRQHandler(void) {
    #if defined(STM32H5) || defined(STM32H7)
    uint32_t sr = ETH->DMACSR;
    ETH->DMACSR = ETH_DMACSR_NIS;
    uint32_t rx_interrupt = sr & ETH_DMACSR_RI;
    #else
    uint32_t sr = ETH->DMASR;
    ETH->DMASR = ETH_DMASR_NIS;
    uint32_t rx_interrupt = sr & ETH_DMASR_RS;
    #endif
    if (rx_interrupt) {
        #if defined(STM32H5) || defined(STM32H7)
        ETH->DMACSR = ETH_DMACSR_RI;
        #else
        ETH->DMASR = ETH_DMASR_RS;
        #endif
        for (;;) {
            #if defined(STM32H5) || defined(STM32H7)
            eth_dma_rx_descr_t *rx_descr_l = &eth_dma.rx_descr[eth_dma.rx_descr_idx];
            if (rx_descr_l->rdes3 & (1 << RX_DESCR_3_OWN_Pos)) {
                // No more RX descriptors ready to read
                break;
            }
            #else
            eth_dma_rx_descr_t *rx_descr = &eth_dma.rx_descr[eth_dma.rx_descr_idx];
            if (rx_descr->rdes0 & (1 << RX_DESCR_0_OWN_Pos)) {
                // No more RX descriptors ready to read
                break;
            }
            #endif

            // Get RX buffer containing new frame
            #if defined(STM32H5) || defined(STM32H7)
            size_t len = (rx_descr_l->rdes3 & RX_DESCR_3_PL_Msk);
            #else
            size_t len = (rx_descr->rdes0 & RX_DESCR_0_FL_Msk) >> RX_DESCR_0_FL_Pos;
            #endif
            len -= 4; // discard CRC at end
            #if defined(STM32H5) || defined(STM32H7)
            uint8_t *buf = &eth_dma.rx_buf[eth_dma.rx_descr_idx * RX_BUF_SIZE];
            #else
            uint8_t *buf = (uint8_t *)rx_descr->rdes2;
            #endif

            // Process frame
            eth_process_frame(&eth_instance, len, buf);
            eth_dma_rx_free();
        }
    }
}

/*******************************************************************************/
// ETH-LwIP bindings

#define TRACE_ASYNC_EV (0x0001)
#define TRACE_ETH_TX (0x0002)
#define TRACE_ETH_RX (0x0004)
#define TRACE_ETH_FULL (0x0008)

static void eth_trace(eth_t *self, size_t len, const void *data, unsigned int flags) {
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

static err_t eth_netif_output(struct netif *netif, struct pbuf *p) {
    // This function should always be called from a context where PendSV-level IRQs are disabled

    LINK_STATS_INC(link.xmit);
    eth_trace(netif->state, (size_t)-1, p, NETUTILS_TRACE_IS_TX | NETUTILS_TRACE_NEWLINE);

    uint8_t *buf;
    int ret = eth_tx_buf_get(p->tot_len, &buf);
    if (ret == 0) {
        pbuf_copy_partial(p, buf, p->tot_len, 0);
        ret = eth_tx_buf_send();
    }

    return ret ? ERR_BUF : ERR_OK;
}

static err_t eth_netif_init(struct netif *netif) {
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
        | NETIF_CHECKSUM_CHECK_ICMP6);
    return ERR_OK;
}

static void eth_lwip_init(eth_t *self) {
    MICROPY_PY_LWIP_ENTER
    // Initialize netif structure
    struct netif *n = &self->netif;
    n->name[0] = 'e';
    n->name[1] = '0';
    n->state = self;

    // Set default IP configuration (0.0.0.0 = use DHCP)
    ip_addr_t ipconfig[4];
    IP_ADDR4(&ipconfig[0], 0, 0, 0, 0);        // IP: 0.0.0.0 (DHCP)
    IP_ADDR4(&ipconfig[1], 255, 255, 255, 0);  // Netmask
    IP_ADDR4(&ipconfig[2], 192, 168, 0, 1);    // Gateway
    IP_ADDR4(&ipconfig[3], 8, 8, 8, 8);        // DNS

    netif_add(n, ip_2_ip4(&ipconfig[0]), ip_2_ip4(&ipconfig[1]), ip_2_ip4(&ipconfig[2]), self, eth_netif_init, ethernet_input);
    netif_set_hostname(n, mod_network_hostname_data);

    ip_addr_t dns_addr;
    IP_ADDR4(&dns_addr, 8, 8, 8, 8);
    dns_setserver(0, &dns_addr);

    // Initialize DHCP structure
    dhcp_set_struct(n, &self->dhcp_struct);

    #if LWIP_IPV6
    netif_create_ip6_linklocal_address(n, 1);
    #endif
    MICROPY_PY_LWIP_EXIT
}


void eth_phy_link_status_poll() {
    eth_t *self = &eth_instance;
    if (!self->enabled) {
        return;
    }
    // Poll PHY link status and handle state changes
    uint16_t bsr = eth_phy_read(self->phy_addr, PHY_BSR);
    bool current_link_status = (bsr & PHY_BSR_LINK_STATUS) != 0;

    // Check if link status changed
    if (current_link_status != self->last_link_status) {
        self->last_link_status = current_link_status;

        // Update LWIP netif link status to reflect physical cable connection
        struct netif *netif = &self->netif;
        MICROPY_PY_LWIP_ENTER
        if (current_link_status) {
            // Cable is physically connected
            netif_set_link_up(netif);

            // Mark MAC speed as needing configuration (will be done after autoneg completes)
            self->mac_speed_configured = false;
            self->autoneg_start_ms = mp_hal_ticks_ms();

            // Start or restart DHCP if interface is up and no static IP
            if (netif_is_up(netif) && ip4_addr_isany_val(*netif_ip4_addr(netif))) {
                // No static IP configured, ensure DHCP is started fresh
                if (netif_dhcp_data(netif)) {
                    dhcp_stop(netif);  // Clean up any existing DHCP state
                }
                dhcp_start(netif);
            }
        } else {
            // Cable is physically disconnected
            netif_set_link_down(netif);
            self->mac_speed_configured = false;

            // Stop DHCP on cable unplug
            struct dhcp *dhcp = netif_dhcp_data(netif);
            if (dhcp != NULL) {
                dhcp_stop(netif);
            }
        }
        MICROPY_PY_LWIP_EXIT
    }

    // If link is up but MAC speed/duplex not yet configured, check if autoneg is complete
    if (current_link_status && !self->mac_speed_configured) {
        // Check if autonegotiation has completed
        bsr = eth_phy_read(self->phy_addr, PHY_BSR);

        // Check for autoneg timeout - if exceeded, use default speed
        bool autoneg_timeout = (mp_hal_ticks_ms() - self->autoneg_start_ms) > PHY_AUTONEG_TIMEOUT_MS;

        if ((bsr & PHY_BSR_AUTONEG_DONE) || autoneg_timeout) {
            // Autonegotiation complete or timed out - read negotiated speed/duplex
            uint16_t phy_speed = self->phy_get_link_status(self->phy_addr);

            // If autoneg timed out and speed couldn't be read, use safe default (10Mbps Half)
            if (autoneg_timeout && phy_speed == 0) {
                phy_speed = PHY_SPEED_10HALF;
            }

            // Disable ETH interrupts during MAC reconfiguration to prevent race conditions
            HAL_NVIC_DisableIRQ(ETH_IRQn);

            // Update MAC to match PHY negotiated speed/duplex
            uint32_t maccr = ETH->MACCR;

            // Stop TX/RX temporarily to allow safe reconfiguration
            maccr &= ~(ETH_MACCR_TE | ETH_MACCR_RE);
            ETH->MACCR = maccr;
            mp_hal_delay_ms(MAC_RECONFIG_DELAY_MS);

            // Clear speed/duplex bits and set according to PHY
            maccr &= ~(ETH_MACCR_FES | ETH_MACCR_DM);

            if (phy_speed == PHY_SPEED_100FULL) {
                maccr |= ETH_MACCR_FES | ETH_MACCR_DM;
            } else if (phy_speed == PHY_SPEED_100HALF) {
                maccr |= ETH_MACCR_FES;
            } else if (phy_speed == PHY_SPEED_10FULL) {
                maccr |= ETH_MACCR_DM;
            }
            // else: 10HALF = both bits clear

            ETH->MACCR = maccr;
            mp_hal_delay_ms(MAC_RECONFIG_DELAY_MS);

            // Restart TX/RX
            ETH->MACCR |= ETH_MACCR_TE | ETH_MACCR_RE;
            mp_hal_delay_ms(MAC_RECONFIG_DELAY_MS);

            // Re-enable ETH interrupts
            HAL_NVIC_EnableIRQ(ETH_IRQn);

            // Mark as configured
            self->mac_speed_configured = true;

            // Restart DHCP since MAC was reconfigured
            struct netif *netif = &self->netif;
            MICROPY_PY_LWIP_ENTER
            if (netif_is_up(netif) && netif_dhcp_data(netif)) {
                dhcp_renew(netif);
            }
            MICROPY_PY_LWIP_EXIT
        }
    }
}

static void eth_process_frame(eth_t *self, size_t len, const uint8_t *buf) {
    eth_trace(self, len, buf, NETUTILS_TRACE_NEWLINE);

    struct netif *netif = &self->netif;
    if (netif->flags & NETIF_FLAG_LINK_UP) {
        struct pbuf *p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
        if (p != NULL) {
            pbuf_take(p, buf, len);
            if (netif->input(p, netif) != ERR_OK) {
                pbuf_free(p);
            }
        }
    }
}

struct netif *eth_netif(eth_t *self) {
    return &self->netif;
}

int eth_link_status(eth_t *self) {
    // Do a quick poll to ensure link status is current
    eth_phy_link_status_poll();

    struct netif *netif = &self->netif;
    if ((netif->flags & (NETIF_FLAG_UP | NETIF_FLAG_LINK_UP))
        == (NETIF_FLAG_UP | NETIF_FLAG_LINK_UP)) {
        if (!ip4_addr_isany_val(*netif_ip4_addr(netif))) {
            return 3; // link up with IP
        } else {
            return 2; // link up, no IP
        }
    } else {
        // Check physical link status - use direct PHY read if not enabled yet
        bool physical_link_up;
        if (self->enabled) {
            physical_link_up = self->last_link_status;
        } else {
            // Direct PHY read when interface not enabled
            uint16_t bsr = eth_phy_read(self->phy_addr, PHY_BSR);
            physical_link_up = (bsr & PHY_BSR_LINK_STATUS) != 0;
        }

        if (physical_link_up) {
            return 1; // physical link up but interface down
        } else {
            return 0; // link down
        }
    }
}

bool eth_is_enabled(eth_t *self) {
    return self->enabled;
}

int eth_start(eth_t *self) {
    // Make sure Eth is Not in low power mode.
    eth_low_power_mode(self, false);

    int ret = eth_mac_init(self);
    if (ret < 0) {
        return ret;
    }

    // Initialize PHY (reset and configure)
    ret = eth_phy_init(self);
    if (ret < 0) {
        eth_mac_deinit(self);
        return ret;
    }

    MICROPY_PY_LWIP_ENTER
    struct netif *n = &self->netif;

    // Enable the interface in LWIP
    netif_set_default(n);
    netif_set_up(n);

    // Do an initial link status poll after PHY has had time to initialize
    eth_phy_link_status_poll();

    // Start DHCP if no static IP has been configured
    if (ip4_addr_isany_val(*netif_ip4_addr(n))) {
        // No static IP configured, start DHCP regardless of link status
        // DHCP will wait for link to come up before sending packets
        dhcp_start(n);
    }

    MICROPY_PY_LWIP_EXIT

    self->enabled = true;
    return 0;
}

int eth_stop(eth_t *self) {
    // Stop DHCP if running
    if (netif_dhcp_data(&self->netif)) {
        dhcp_stop(&self->netif);
    }
    netif_set_down(&self->netif);

    // Shutdown PHY
    eth_low_power_mode(self, true);

    // Clear link status tracking
    self->last_link_status = false;

    eth_mac_deinit(self);
    self->enabled = false;
    return 0;
}

void eth_low_power_mode(eth_t *self, bool enable) {
    (void)self;

    // Enable eth clock
    #if defined(STM32H7)
    __HAL_RCC_ETH1MAC_CLK_ENABLE();
    #else
    __HAL_RCC_ETH_CLK_ENABLE();
    #endif

    uint16_t bcr = eth_phy_read(self->phy_addr, PHY_BCR);
    if (enable) {
        // Enable low-power mode.
        eth_phy_write(self->phy_addr, PHY_BCR, bcr | PHY_BCR_POWER_DOWN);
        // Disable eth clock.
        #if defined(STM32H7)
        __HAL_RCC_ETH1MAC_CLK_DISABLE();
        #else
        __HAL_RCC_ETH_CLK_DISABLE();
        #endif
    } else {
        // Disable low-power mode.
        eth_phy_write(self->phy_addr, PHY_BCR, bcr & (~PHY_BCR_POWER_DOWN));
    }
}

static int eth_phy_init(eth_t *self) {
    // Reset the PHY and wait for reset to complete
    eth_phy_write(self->phy_addr, PHY_BCR, PHY_BCR_SOFT_RESET);
    mp_hal_delay_ms(10);

    // Wait for PHY reset to complete (but don't wait for link)
    uint32_t t0 = mp_hal_ticks_ms();
    while (eth_phy_read(self->phy_addr, PHY_BCR) & PHY_BCR_SOFT_RESET) {
        if (mp_hal_ticks_ms() - t0 > 1000) {  // 1 second timeout for reset
            return -MP_ETIMEDOUT;
        }
        mp_hal_delay_ms(2);
    }

    // Enable autonegotiation for all speed/duplex modes
    // This starts the autonegotiation process in the background
    eth_phy_write(self->phy_addr, PHY_ANAR,
        PHY_ANAR_SPEED_10HALF |
        PHY_ANAR_SPEED_10FULL |
        PHY_ANAR_SPEED_100HALF |
        PHY_ANAR_SPEED_100FULL |
        PHY_ANAR_IEEE802_3);
    eth_phy_write(self->phy_addr, PHY_BCR, PHY_BCR_AUTONEG_EN | PHY_BCR_AUTONEG_RESTART);

    // Initialize link status tracking (current state, whatever it is)
    self->last_link_status = false;
    self->mac_speed_configured = false;
    eth_phy_link_status_poll();

    return 0;
}

#endif // defined(MICROPY_HW_ETH_MDC)
