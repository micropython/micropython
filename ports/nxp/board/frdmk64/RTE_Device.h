/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __RTE_DEVICE_H
#define __RTE_DEVICE_H

/* ENET configuration. */
#define RTE_ENET              1
#define RTE_ENET_PHY_ADDRESS   0


#define RTE_ENET_MII          0
//   <e> RMII (Reduced Media Independent Interface)
#define   RTE_ENET_RMII                 1

/* UART select, UART0-UART5 */
/* User needs to provide the implementation for XXX_GetFreq/XXX_InitPins/XXX_DeinitPins 
in the application for enabling according instance. */ 
#define RTE_USART0 1
#define RTE_USART0_DMA_EN 0
#define RTE_USART1 1
#define RTE_USART1_DMA_EN 0
#define RTE_USART2 1
#define RTE_USART2_DMA_EN 0
#define RTE_USART3 1
#define RTE_USART3_DMA_EN 0
#define RTE_USART4 1
#define RTE_USART4_DMA_EN 0
#define RTE_USART5 1
#define RTE_USART5_DMA_EN 0

/* UART RX Buffer configuration. */
#define USART_RX_BUFFER_LEN 64
#define USART0_RX_BUFFER_ENABLE 0
#define USART1_RX_BUFFER_ENABLE 0
#define USART2_RX_BUFFER_ENABLE 0
#define USART3_RX_BUFFER_ENABLE 0
#define USART4_RX_BUFFER_ENABLE 0
#define USART5_RX_BUFFER_ENABLE 0

#define RTE_USART0_DMA_TX_CH 0
#define RTE_USART0_DMA_TX_PERI_SEL (uint8_t) kDmaRequestMux0UART0Tx
#define RTE_USART0_DMA_TX_DMAMUX_BASE DMAMUX0
#define RTE_USART0_DMA_TX_DMA_BASE DMA0
#define RTE_USART0_DMA_RX_CH 1
#define RTE_USART0_DMA_RX_PERI_SEL (uint8_t) kDmaRequestMux0UART0Rx
#define RTE_USART0_DMA_RX_DMAMUX_BASE DMAMUX0
#define RTE_USART0_DMA_RX_DMA_BASE DMA0

#define RTE_USART1_DMA_TX_CH 0
#define RTE_USART1_DMA_TX_PERI_SEL (uint8_t) kDmaRequestMux0UART1Tx
#define RTE_USART1_DMA_TX_DMAMUX_BASE DMAMUX0
#define RTE_USART1_DMA_TX_DMA_BASE DMA0
#define RTE_USART1_DMA_RX_CH 1
#define RTE_USART1_DMA_RX_PERI_SEL (uint8_t) kDmaRequestMux0UART1Rx
#define RTE_USART1_DMA_RX_DMAMUX_BASE DMAMUX0
#define RTE_USART1_DMA_RX_DMA_BASE DMA0

#define RTE_USART2_DMA_TX_CH 0
#define RTE_USART2_DMA_TX_PERI_SEL (uint8_t) kDmaRequestMux0UART2Tx
#define RTE_USART2_DMA_TX_DMAMUX_BASE DMAMUX0
#define RTE_USART2_DMA_TX_DMA_BASE DMA0
#define RTE_USART2_DMA_RX_CH 1
#define RTE_USART2_DMA_RX_PERI_SEL (uint8_t) kDmaRequestMux0UART2Rx
#define RTE_USART2_DMA_RX_DMAMUX_BASE DMAMUX0
#define RTE_USART2_DMA_RX_DMA_BASE DMA0

#define RTE_USART3_DMA_TX_CH 0
#define RTE_USART3_DMA_TX_PERI_SEL (uint8_t) kDmaRequestMux0UART3Tx
#define RTE_USART3_DMA_TX_DMAMUX_BASE DMAMUX0
#define RTE_USART3_DMA_TX_DMA_BASE DMA0
#define RTE_USART3_DMA_RX_CH 1
#define RTE_USART3_DMA_RX_PERI_SEL (uint8_t) kDmaRequestMux0UART3Rx
#define RTE_USART3_DMA_RX_DMAMUX_BASE DMAMUX0
#define RTE_USART3_DMA_RX_DMA_BASE DMA0

#define RTE_USART4_DMA_TX_CH 0
#define RTE_USART4_DMA_TX_PERI_SEL (uint8_t) kDmaRequestMux0UART4
#define RTE_USART4_DMA_TX_DMAMUX_BASE DMAMUX0
#define RTE_USART4_DMA_TX_DMA_BASE DMA0
#define RTE_USART4_DMA_RX_CH 1
#define RTE_USART4_DMA_RX_PERI_SEL (uint8_t) kDmaRequestMux0UART4
#define RTE_USART4_DMA_RX_DMAMUX_BASE DMAMUX0
#define RTE_USART4_DMA_RX_DMA_BASE DMA0

#define RTE_USART5_DMA_TX_CH 0
#define RTE_USART5_DMA_TX_PERI_SEL (uint8_t) kDmaRequestMux0UART5
#define RTE_USART5_DMA_TX_DMAMUX_BASE DMAMUX0
#define RTE_USART5_DMA_TX_DMA_BASE DMA0
#define RTE_USART5_DMA_RX_CH 1
#define RTE_USART5_DMA_RX_PERI_SEL (uint8_t) kDmaRequestMux0UART5
#define RTE_USART5_DMA_RX_DMAMUX_BASE DMAMUX0
#define RTE_USART5_DMA_RX_DMA_BASE DMA0


/* I2C select, I2C0 - I2C2. */
/* User needs to provide the implementation for XXX_GetFreq/XXX_InitPins/XXX_DeinitPins 
in the application for enabling according instance. */ 
#define RTE_I2C0 0
#define RTE_I2C0_DMA_EN 0
#define RTE_I2C1 0
#define RTE_I2C1_DMA_EN 0
#define RTE_I2C2 0
#define RTE_I2C2_DMA_EN 0

/*I2C configuration*/
#define RTE_I2C0_Master_DMA_BASE DMA0
#define RTE_I2C0_Master_DMA_CH 0
#define RTE_I2C0_Master_DMAMUX_BASE DMAMUX0
#define RTE_I2C0_Master_PERI_SEL kDmaRequestMux0I2C0

#define RTE_I2C1_Master_DMA_BASE DMA0
#define RTE_I2C1_Master_DMA_CH 1
#define RTE_I2C1_Master_DMAMUX_BASE DMAMUX0
#define RTE_I2C1_Master_PERI_SEL kDmaRequestMux0I2C1

#define RTE_I2C2_Master_DMA_BASE DMA0
#define RTE_I2C2_Master_DMA_CH 2
#define RTE_I2C2_Master_DMAMUX_BASE DMAMUX0
#define RTE_I2C2_Master_PERI_SEL kDmaRequestMux0I2C2

/* SPI select, DSPI0 - DSPI2. */
/* User needs to provide the implementation for XXX_GetFreq/XXX_InitPins/XXX_DeinitPins 
in the application for enabling according instance. */ 
#define RTE_SPI0 1
#define RTE_SPI0_DMA_EN 0
#define RTE_SPI1 1
#define RTE_SPI1_DMA_EN 0
#define RTE_SPI2 1
#define RTE_SPI2_DMA_EN 0

/* DSPI configuration. */
#define RTE_SPI0_PCS_TO_SCK_DELAY 1000
#define RTE_SPI0_SCK_TO_PSC_DELAY 1000
#define RTE_SPI0_BETWEEN_TRANSFER_DELAY 1000
#define RTE_SPI0_MASTER_PCS_PIN_SEL kDSPI_MasterPcs0
#define RTE_SPI0_DMA_TX_CH 0
#define RTE_SPI0_DMA_TX_PERI_SEL (uint8_t) kDmaRequestMux0SPI0Tx
#define RTE_SPI0_DMA_TX_DMAMUX_BASE DMAMUX0
#define RTE_SPI0_DMA_TX_DMA_BASE DMA0
#define RTE_SPI0_DMA_RX_CH 1
#define RTE_SPI0_DMA_RX_PERI_SEL (uint8_t) kDmaRequestMux0SPI0Rx
#define RTE_SPI0_DMA_RX_DMAMUX_BASE DMAMUX0
#define RTE_SPI0_DMA_RX_DMA_BASE DMA0
#define RTE_SPI0_DMA_LINK_DMA_BASE DMA0
#define RTE_SPI0_DMA_LINK_CH 2

#define RTE_SPI1_PCS_TO_SCK_DELAY 1000
#define RTE_SPI1_SCK_TO_PSC_DELAY 1000
#define RTE_SPI1_BETWEEN_TRANSFER_DELAY 1000
#define RTE_SPI1_MASTER_PCS_PIN_SEL kDSPI_MasterPcs0
#define RTE_SPI1_DMA_TX_CH 4
#define RTE_SPI1_DMA_TX_PERI_SEL (uint8_t) kDmaRequestMux0SPI1
#define RTE_SPI1_DMA_TX_DMAMUX_BASE DMAMUX0
#define RTE_SPI1_DMA_TX_DMA_BASE DMA0
#define RTE_SPI1_DMA_RX_CH 3
#define RTE_SPI1_DMA_RX_PERI_SEL (uint8_t) kDmaRequestMux0SPI1
#define RTE_SPI1_DMA_RX_DMAMUX_BASE DMAMUX0
#define RTE_SPI1_DMA_RX_DMA_BASE DMA0
#define RTE_SPI1_DMA_LINK_DMA_BASE DMA0
#define RTE_SPI1_DMA_LINK_CH 2

#define RTE_SPI2_PCS_TO_SCK_DELAY 1000
#define RTE_SPI2_SCK_TO_PSC_DELAY 1000
#define RTE_SPI2_BETWEEN_TRANSFER_DELAY 1000
#define RTE_SPI2_MASTER_PCS_PIN_SEL kDSPI_MasterPcs0
#define RTE_SPI2_DMA_TX_CH 6
#define RTE_SPI2_DMA_TX_PERI_SEL (uint8_t) kDmaRequestMux0SPI2
#define RTE_SPI2_DMA_TX_DMAMUX_BASE DMAMUX0
#define RTE_SPI2_DMA_TX_DMA_BASE DMA0
#define RTE_SPI2_DMA_RX_CH 7
#define RTE_SPI2_DMA_RX_PERI_SEL (uint8_t) kDmaRequestMux0SPI2
#define RTE_SPI2_DMA_RX_DMAMUX_BASE DMAMUX0
#define RTE_SPI2_DMA_RX_DMA_BASE DMA0
#define RTE_SPI2_DMA_LINK_DMA_BASE DMA0
#define RTE_SPI2_DMA_LINK_CH 8

#endif /* __RTE_DEVICE_H */
