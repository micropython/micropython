/*!
    \file    gd32f4xx_dma.h
    \brief   definitions for the DMA
    \version 2026-02-05, V3.3.3, firmware for GD32F4xx
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#ifndef GD32F4XX_DMA_H
#define GD32F4XX_DMA_H

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */   
#include "gd32f4xx.h"


/* DMA definitions */
#define DMA0                              (DMA_BASE)                          /*!< DMA0 base address */
#define DMA1                              (DMA_BASE + 0x00000400U)            /*!< DMA1 base address */

/* registers definitions */
#define DMA_INTF0(dmax)                    REG32((dmax) + 0x00000000U)        /*!< DMA interrupt flag register 0 */
#define DMA_INTF1(dmax)                    REG32((dmax) + 0x00000004U)        /*!< DMA interrupt flag register 1 */
#define DMA_INTC0(dmax)                    REG32((dmax) + 0x00000008U)        /*!< DMA interrupt flag clear register 0 */
#define DMA_INTC1(dmax)                    REG32((dmax) + 0x0000000CU)        /*!< DMA interrupt flag clear register 1 */

#define DMA_CH0CTL(dmax)                   REG32((dmax) + 0x00000010U)        /*!< DMA channel 0 control register */
#define DMA_CH0CNT(dmax)                   REG32((dmax) + 0x00000014U)        /*!< DMA channel 0 counter register */
#define DMA_CH0PADDR(dmax)                 REG32((dmax) + 0x00000018U)        /*!< DMA channel 0 peripheral base address register */
#define DMA_CH0M0ADDR(dmax)                REG32((dmax) + 0x0000001CU)        /*!< DMA channel 0 memory 0 base address register */
#define DMA_CH0M1ADDR(dmax)                REG32((dmax) + 0x00000020U)        /*!< DMA channel 0 memory 1 base address register */
#define DMA_CH0FCTL(dmax)                  REG32((dmax) + 0x00000024U)        /*!< DMA channel 0 FIFO control register */

#define DMA_CH1CTL(dmax)                   REG32((dmax) + 0x00000028U)        /*!< DMA channel 1 control register */
#define DMA_CH1CNT(dmax)                   REG32((dmax) + 0x0000002CU)        /*!< DMA channel 1 counter register */
#define DMA_CH1PADDR(dmax)                 REG32((dmax) + 0x00000030U)        /*!< DMA channel 1 peripheral base address register */
#define DMA_CH1M0ADDR(dmax)                REG32((dmax) + 0x00000034U)        /*!< DMA channel 1 memory 0 base address register */
#define DMA_CH1M1ADDR(dmax)                REG32((dmax) + 0x00000038U)        /*!< DMA channel 1 memory 1 base address register */
#define DMA_CH1FCTL(dmax)                  REG32((dmax) + 0x0000003CU)        /*!< DMA channel 1 FIFO control register */

#define DMA_CH2CTL(dmax)                   REG32((dmax) + 0x00000040U)        /*!< DMA channel 2 control register */
#define DMA_CH2CNT(dmax)                   REG32((dmax) + 0x00000044U)        /*!< DMA channel 2 counter register */
#define DMA_CH2PADDR(dmax)                 REG32((dmax) + 0x00000048U)        /*!< DMA channel 2 peripheral base address register */
#define DMA_CH2M0ADDR(dmax)                REG32((dmax) + 0x0000004CU)        /*!< DMA channel 2 memory 0 base address register */
#define DMA_CH2M1ADDR(dmax)                REG32((dmax) + 0x00000050U)        /*!< DMA channel 2 memory 1 base address register */
#define DMA_CH2FCTL(dmax)                  REG32((dmax) + 0x00000054U)        /*!< DMA channel 2 FIFO control register */

#define DMA_CH3CTL(dmax)                   REG32((dmax) + 0x00000058U)        /*!< DMA channel 3 control register */
#define DMA_CH3CNT(dmax)                   REG32((dmax) + 0x0000005CU)        /*!< DMA channel 3 counter register */
#define DMA_CH3PADDR(dmax)                 REG32((dmax) + 0x00000060U)        /*!< DMA channel 3 peripheral base address register */
#define DMA_CH3M0ADDR(dmax)                REG32((dmax) + 0x00000064U)        /*!< DMA channel 3 memory 0 base address register */
#define DMA_CH3M1ADDR(dmax)                REG32((dmax) + 0x00000068U)        /*!< DMA channel 3 memory 1 base address register */
#define DMA_CH3FCTL(dmax)                  REG32((dmax) + 0x0000006CU)        /*!< DMA channel 3 FIFO control register */

#define DMA_CH4CTL(dmax)                   REG32((dmax) + 0x00000070U)        /*!< DMA channel 4 control register */
#define DMA_CH4CNT(dmax)                   REG32((dmax) + 0x00000074U)        /*!< DMA channel 4 counter register */
#define DMA_CH4PADDR(dmax)                 REG32((dmax) + 0x00000078U)        /*!< DMA channel 4 peripheral base address register */
#define DMA_CH4M0ADDR(dmax)                REG32((dmax) + 0x0000007CU)        /*!< DMA channel 4 memory 0 base address register */
#define DMA_CH4M1ADDR(dmax)                REG32((dmax) + 0x00000080U)        /*!< DMA channel 4 memory 1 base address register */
#define DMA_CH4FCTL(dmax)                  REG32((dmax) + 0x00000084U)        /*!< DMA channel 4 FIFO control register */

#define DMA_CH5CTL(dmax)                   REG32((dmax) + 0x00000088U)        /*!< DMA channel 5 control register */
#define DMA_CH5CNT(dmax)                   REG32((dmax) + 0x0000008CU)        /*!< DMA channel 5 counter register */
#define DMA_CH5PADDR(dmax)                 REG32((dmax) + 0x00000090U)        /*!< DMA channel 5 peripheral base address register */
#define DMA_CH5M0ADDR(dmax)                REG32((dmax) + 0x00000094U)        /*!< DMA channel 5 memory 0 base address register */
#define DMA_CH5M1ADDR(dmax)                REG32((dmax) + 0x00000098U)        /*!< DMA channel 5 memory 1 base address register */
#define DMA_CH5FCTL(dmax)                  REG32((dmax) + 0x0000009CU)        /*!< DMA channel 5 FIFO control register */

#define DMA_CH6CTL(dmax)                   REG32((dmax) + 0x000000A0U)        /*!< DMA channel 6 control register */
#define DMA_CH6CNT(dmax)                   REG32((dmax) + 0x000000A4U)        /*!< DMA channel 6 counter register */
#define DMA_CH6PADDR(dmax)                 REG32((dmax) + 0x000000A8U)        /*!< DMA channel 6 peripheral base address register */
#define DMA_CH6M0ADDR(dmax)                REG32((dmax) + 0x000000ACU)        /*!< DMA channel 6 memory 0 base address register */
#define DMA_CH6M1ADDR(dmax)                REG32((dmax) + 0x000000B0U)        /*!< DMA channel 6 memory 1 base address register */
#define DMA_CH6FCTL(dmax)                  REG32((dmax) + 0x000000B4U)        /*!< DMA channel 6 FIFO control register */

#define DMA_CH7CTL(dmax)                   REG32((dmax) + 0x000000B8U)        /*!< DMA channel 7 control register */
#define DMA_CH7CNT(dmax)                   REG32((dmax) + 0x000000BCU)        /*!< DMA channel 7 counter register */
#define DMA_CH7PADDR(dmax)                 REG32((dmax) + 0x000000C0U)        /*!< DMA channel 7 peripheral base address register */
#define DMA_CH7M0ADDR(dmax)                REG32((dmax) + 0x000000C4U)        /*!< DMA channel 7 memory 0 base address register */
#define DMA_CH7M1ADDR(dmax)                REG32((dmax) + 0x000000C8U)        /*!< DMA channel 7 memory 1 base address register */
#define DMA_CH7FCTL(dmax)                  REG32((dmax) + 0x000000CCU)        /*!< DMA channel 7 FIFO control register */

/* bits definitions */
/* DMA_INTF */
#define DMA_INTF_FEEIF                    BIT(0)                        /*!< FIFO error and exception flag */
#define DMA_INTF_SDEIF                    BIT(2)                        /*!< single data mode exception flag */
#define DMA_INTF_TAEIF                    BIT(3)                        /*!< transfer access error flag */
#define DMA_INTF_HTFIF                    BIT(4)                        /*!< half transfer finish flag */
#define DMA_INTF_FTFIF                    BIT(5)                        /*!< full transger finish flag */

/* DMA_INTC */
#define DMA_INTC_FEEIFC                   BIT(0)                        /*!< clear FIFO error and exception flag */
#define DMA_INTC_SDEIFC                   BIT(2)                        /*!< clear single data mode exception flag */
#define DMA_INTC_TAEIFC                   BIT(3)                        /*!< clear single data mode exception flag */
#define DMA_INTC_HTFIFC                   BIT(4)                        /*!< clear half transfer finish flag */
#define DMA_INTC_FTFIFC                   BIT(5)                        /*!< clear full transger finish flag */

/* DMA_CHxCTL,x=0..7 */
#define DMA_CHXCTL_CHEN                   BIT(0)                        /*!< channel x enable */
#define DMA_CHXCTL_SDEIE                  BIT(1)                        /*!< enable bit for channel x single data mode exception interrupt */
#define DMA_CHXCTL_TAEIE                  BIT(2)                        /*!< enable bit for channel x tranfer access error interrupt */
#define DMA_CHXCTL_HTFIE                  BIT(3)                        /*!< enable bit for channel x half transfer finish interrupt */
#define DMA_CHXCTL_FTFIE                  BIT(4)                        /*!< enable bit for channel x full transfer finish interrupt */
#define DMA_CHXCTL_TFCS                   BIT(5)                        /*!< transfer flow controller select */
#define DMA_CHXCTL_TM                     BITS(6,7)                     /*!< transfer mode */
#define DMA_CHXCTL_CMEN                   BIT(8)                        /*!< circulation mode */
#define DMA_CHXCTL_PNAGA                  BIT(9)                        /*!< next address generation algorithm of peripheral */
#define DMA_CHXCTL_MNAGA                  BIT(10)                       /*!< next address generation algorithm of memory */
#define DMA_CHXCTL_PWIDTH                 BITS(11,12)                   /*!< transfer width of peipheral */
#define DMA_CHXCTL_MWIDTH                 BITS(13,14)                   /*!< transfer width of memory */
#define DMA_CHXCTL_PAIF                   BIT(15)                       /*!< peripheral address increment fixed */
#define DMA_CHXCTL_PRIO                   BITS(16,17)                   /*!< priority level */
#define DMA_CHXCTL_SBMEN                  BIT(18)                       /*!< switch-buffer mode enable */
#define DMA_CHXCTL_MBS                    BIT(19)                       /*!< memory buffer select */
#define DMA_CHXCTL_PBURST                 BITS(21,22)                   /*!< transfer burst type of peripheral */
#define DMA_CHXCTL_MBURST                 BITS(23,24)                   /*!< transfer burst type of memory */
#define DMA_CHXCTL_PERIEN                 BITS(25,27)                   /*!< peripheral enable */

/* DMA_CHxCNT,x=0..7 */
#define DMA_CHXCNT_CNT                    BITS(0,15)                    /*!< transfer counter */

/* DMA_CHxPADDR,x=0..7 */
#define DMA_CHXPADDR_PADDR                BITS(0,31)                    /*!< peripheral base address */

/* DMA_CHxM0ADDR,x=0..7 */
#define DMA_CHXM0ADDR_M0ADDR              BITS(0,31)                    /*!< memory 0 base address */

/* DMA_CHxM1ADDR,x=0..7 */
#define DMA_CHXM1ADDR_M0ADDR              BITS(0,31)                    /*!< memory 1 base address */

/* DMA_CHxFCTL,x=0..7 */
#define DMA_CHXFCTL_FCCV                  BITS(0,1)                     /*!< FIFO counter critical value */
#define DMA_CHXFCTL_MDMEN                 BIT(2)                        /*!< multi-data mode enable */
#define DMA_CHXFCTL_FCNT                  BITS(3,5)                     /*!< FIFO counter */
#define DMA_CHXFCTL_FEEIE                 BIT(7)                        /*!< FIFO exception interrupt enable */

/* constants definitions */
/* DMA channel select */
typedef enum 
{
    DMA_CH0 = 0,                                    /*!< DMA Channel 0 */
    DMA_CH1,                                        /*!< DMA Channel 1 */
    DMA_CH2,                                        /*!< DMA Channel 2 */
    DMA_CH3,                                        /*!< DMA Channel 3 */
    DMA_CH4,                                        /*!< DMA Channel 4 */
    DMA_CH5,                                        /*!< DMA Channel 5 */
    DMA_CH6,                                        /*!< DMA Channel 6 */
    DMA_CH7                                         /*!< DMA Channel 7 */
} dma_channel_enum;

/* DMA peripheral select */
typedef enum 
{
    DMA_SUBPERI0 = 0,                               /*!< DMA Peripheral 0 */
    DMA_SUBPERI1,                                   /*!< DMA Peripheral 1 */
    DMA_SUBPERI2,                                   /*!< DMA Peripheral 2 */
    DMA_SUBPERI3,                                   /*!< DMA Peripheral 3 */
    DMA_SUBPERI4,                                   /*!< DMA Peripheral 4 */
    DMA_SUBPERI5,                                   /*!< DMA Peripheral 5 */
    DMA_SUBPERI6,                                   /*!< DMA Peripheral 6 */
    DMA_SUBPERI7                                    /*!< DMA Peripheral 7 */
} dma_subperipheral_enum;

/* DMA multidata mode initialize struct */
typedef struct
{
    uint32_t periph_addr;                           /*!< peripheral base address */
    uint32_t periph_width;                          /*!< transfer data size of peripheral */
    uint32_t periph_inc;                            /*!< peripheral increasing mode */  

    uint32_t memory0_addr;                          /*!< memory 0 base address */
    uint32_t memory_width;                          /*!< transfer data size of memory */
    uint32_t memory_inc;                            /*!< memory increasing mode */

    uint32_t memory_burst_width;                    /*!< multi data mode enable */
    uint32_t periph_burst_width;                    /*!< multi data mode enable */
    uint32_t critical_value;                        /*!< FIFO critical */

    uint32_t circular_mode;                         /*!< DMA circular mode */
    uint32_t direction;                             /*!< channel data transfer direction */
    uint32_t number;                                /*!< channel transfer number */
    uint32_t priority;                              /*!< channel priority level */
}dma_multi_data_parameter_struct;

/* DMA singledata mode initialize struct */
typedef struct
{
    uint32_t periph_addr;                           /*!< peripheral base address */
    uint32_t periph_inc;                            /*!< peripheral increasing mode */  

    uint32_t memory0_addr;                          /*!< memory 0 base address */
    uint32_t memory_inc;                            /*!< memory increasing mode */

    uint32_t periph_memory_width;                   /*!< transfer data size of peripheral */

    uint32_t circular_mode;                         /*!< DMA circular mode */
    uint32_t direction;                             /*!< channel data transfer direction */
    uint32_t number;                                /*!< channel transfer number */
    uint32_t priority;                              /*!< channel priority level */
} dma_single_data_parameter_struct;

#define DMA_FLAG_ADD(flag,channel)        ((uint32_t)((flag)<<((((uint32_t)(channel)*6U))+((uint32_t)(((uint32_t)(channel)) >> 1U)&0x01U)*4U)))   /*!< DMA channel flag shift */

/* DMA_register address */
#define DMA_CHCTL(dma,channel)            REG32(((dma) + 0x10U) + 0x18U*(channel))  /*!< the address of DMA channel CHXCTL register  */
#define DMA_CHCNT(dma,channel)            REG32(((dma) + 0x14U) + 0x18U*(channel))  /*!< the address of DMA channel CHXCNT register */
#define DMA_CHPADDR(dma,channel)          REG32(((dma) + 0x18U) + 0x18U*(channel))  /*!< the address of DMA channel CHXPADDR register */
#define DMA_CHM0ADDR(dma,channel)         REG32(((dma) + 0x1CU) + 0x18U*(channel))  /*!< the address of DMA channel CHXM0ADDR register */
#define DMA_CHM1ADDR(dma,channel)         REG32(((dma) + 0x20U) + 0x18U*(channel))  /*!< the address of DMA channel CHXM1ADDR register */
#define DMA_CHFCTL(dma,channel)           REG32(((dma) + 0x24U) + 0x18U*(channel))  /*!< the address of DMA channel CHXMADDR register */

/* peripheral select */
#define CHCTL_PERIEN(regval)              (BITS(25,27) & ((uint32_t)(regval) << 25))
#define DMA_PERIPH_0_SELECT               CHCTL_PERIEN(0)                           /*!< peripheral 0 select */
#define DMA_PERIPH_1_SELECT               CHCTL_PERIEN(1)                           /*!< peripheral 1 select */
#define DMA_PERIPH_2_SELECT               CHCTL_PERIEN(2)                           /*!< peripheral 2 select */
#define DMA_PERIPH_3_SELECT               CHCTL_PERIEN(3)                           /*!< peripheral 3 select */
#define DMA_PERIPH_4_SELECT               CHCTL_PERIEN(4)                           /*!< peripheral 4 select */
#define DMA_PERIPH_5_SELECT               CHCTL_PERIEN(5)                           /*!< peripheral 5 select */
#define DMA_PERIPH_6_SELECT               CHCTL_PERIEN(6)                           /*!< peripheral 6 select */
#define DMA_PERIPH_7_SELECT               CHCTL_PERIEN(7)                           /*!< peripheral 7 select */

/* burst type of memory */
#define CHCTL_MBURST(regval)              (BITS(23,24) & ((uint32_t)(regval) << 23))
#define DMA_MEMORY_BURST_SINGLE           CHCTL_MBURST(0)                           /*!< single burst */
#define DMA_MEMORY_BURST_4_BEAT           CHCTL_MBURST(1)                           /*!< 4-beat burst */
#define DMA_MEMORY_BURST_8_BEAT           CHCTL_MBURST(2)                           /*!< 8-beat burst */
#define DMA_MEMORY_BURST_16_BEAT          CHCTL_MBURST(3)                           /*!< 16-beat burst */

/* burst type of peripheral */
#define CHCTL_PBURST(regval)              (BITS(21,22) & ((uint32_t)(regval) << 21))
#define DMA_PERIPH_BURST_SINGLE           CHCTL_PBURST(0)                           /*!< single burst */
#define DMA_PERIPH_BURST_4_BEAT           CHCTL_PBURST(1)                           /*!< 4-beat burst */
#define DMA_PERIPH_BURST_8_BEAT           CHCTL_PBURST(2)                           /*!< 8-beat burst */
#define DMA_PERIPH_BURST_16_BEAT          CHCTL_PBURST(3)                           /*!< 16-beat burst */

/* channel priority level */
#define CHCTL_PRIO(regval)                (BITS(16,17) & ((uint32_t)(regval) << 16))
#define DMA_PRIORITY_LOW                  CHCTL_PRIO(0)                             /*!< low priority */
#define DMA_PRIORITY_MEDIUM               CHCTL_PRIO(1)                             /*!< medium priority */
#define DMA_PRIORITY_HIGH                 CHCTL_PRIO(2)                             /*!< high priority */
#define DMA_PRIORITY_ULTRA_HIGH           CHCTL_PRIO(3)                             /*!< ultra high priority */

/* transfer data width of memory */
#define CHCTL_MWIDTH(regval)              (BITS(13,14) & ((uint32_t)(regval) << 13))
#define DMA_MEMORY_WIDTH_8BIT             CHCTL_MWIDTH(0)                           /*!< transfer data width of memory is 8-bit */
#define DMA_MEMORY_WIDTH_16BIT            CHCTL_MWIDTH(1)                           /*!< transfer data width of memory is 16-bit */
#define DMA_MEMORY_WIDTH_32BIT            CHCTL_MWIDTH(2)                           /*!< transfer data width of memory is 32-bit */

/* transfer data width of peripheral */
#define CHCTL_PWIDTH(regval)              (BITS(11,12) & ((uint32_t)(regval) << 11))
#define DMA_PERIPH_WIDTH_8BIT             CHCTL_PWIDTH(0)                           /*!< transfer data width of peripheral is 8-bit */
#define DMA_PERIPH_WIDTH_16BIT            CHCTL_PWIDTH(1)                           /*!< transfer data width of peripheral is 16-bit */
#define DMA_PERIPH_WIDTH_32BIT            CHCTL_PWIDTH(2)                           /*!< transfer data width of peripheral is 32-bit */

/* channel transfer mode */
#define CHCTL_TM(regval)                  (BITS(6,7) & ((uint32_t)(regval) << 6))
#define DMA_PERIPH_TO_MEMORY              CHCTL_TM(0)                               /*!< read from peripheral and write to memory */
#define DMA_MEMORY_TO_PERIPH              CHCTL_TM(1)                               /*!< read from memory and write to peripheral */
#define DMA_MEMORY_TO_MEMORY              CHCTL_TM(2)                               /*!< read from memory and write to memory */

/* FIFO counter critical value */
#define CHFCTL_FCCV(regval)               (BITS(0,1) & ((uint32_t)(regval) << 0))
#define DMA_FIFO_1_WORD                   CHFCTL_FCCV(0)                            /*!< critical value 1 word */
#define DMA_FIFO_2_WORD                   CHFCTL_FCCV(1)                            /*!< critical value 2 word */
#define DMA_FIFO_3_WORD                   CHFCTL_FCCV(2)                            /*!< critical value 3 word */
#define DMA_FIFO_4_WORD                   CHFCTL_FCCV(3)                            /*!< critical value 4 word */

/* memory select */
#define DMA_MEMORY_0                      ((uint32_t)0x00000000U)                   /*!< select memory 0 */
#define DMA_MEMORY_1                      ((uint32_t)0x00000001U)                   /*!< select memory 1 */

/* DMA circular mode */
#define DMA_CIRCULAR_MODE_ENABLE          ((uint32_t)0x00000000U)                   /*!< circular mode enable */
#define DMA_CIRCULAR_MODE_DISABLE         ((uint32_t)0x00000001U)                   /*!< circular mode disable */

/* DMA flow controller select */
#define DMA_FLOW_CONTROLLER_DMA           ((uint32_t)0x00000000U)                   /*!< DMA is the flow controler */
#define DMA_FLOW_CONTROLLER_PERI          ((uint32_t)0x00000001U)                   /*!< peripheral is the flow controler */

/* peripheral increasing mode */
#define DMA_PERIPH_INCREASE_ENABLE        ((uint32_t)0x00000000U)                   /*!< next address of peripheral is increasing address mode */
#define DMA_PERIPH_INCREASE_DISABLE       ((uint32_t)0x00000001U)                   /*!< next address of peripheral is fixed address mode */
#define DMA_PERIPH_INCREASE_FIX           ((uint32_t)0x00000002U)                   /*!< next address of peripheral is increasing fixed */

/* memory increasing mode */
#define DMA_MEMORY_INCREASE_ENABLE        ((uint32_t)0x00000000U)                   /*!< next address of memory is increasing address mode */
#define DMA_MEMORY_INCREASE_DISABLE       ((uint32_t)0x00000001U)                   /*!< next address of memory is fixed address mode */

/* FIFO status */
#define DMA_FIFO_STATUS_NODATA            ((uint32_t)0x00000000U)                   /*!< the data in the FIFO less than 1 word */
#define DMA_FIFO_STATUS_1_WORD            ((uint32_t)0x00000001U)                   /*!< the data in the FIFO more than 1 word, less than 2 words */
#define DMA_FIFO_STATUS_2_WORD            ((uint32_t)0x00000002U)                   /*!< the data in the FIFO more than 2 word, less than 3 words */
#define DMA_FIFO_STATUS_3_WORD            ((uint32_t)0x00000003U)                   /*!< the data in the FIFO more than 3 word, less than 4 words */
#define DMA_FIFO_STATUS_EMPTY             ((uint32_t)0x00000004U)                   /*!< the data in the FIFO is empty */
#define DMA_FIFO_STATUS_FULL              ((uint32_t)0x00000005U)                   /*!< the data in the FIFO is full */

/* DMA reset value */
#define DMA_CHCTL_RESET_VALUE             ((uint32_t)0x00000000U)                   /*!< the reset value of DMA channel CHXCTL register */
#define DMA_CHCNT_RESET_VALUE             ((uint32_t)0x00000000U)                   /*!< the reset value of DMA channel CHXCNT register */
#define DMA_CHPADDR_RESET_VALUE           ((uint32_t)0x00000000U)                   /*!< the reset value of DMA channel CHXPADDR register */
#define DMA_CHMADDR_RESET_VALUE           ((uint32_t)0x00000000U)                   /*!< the reset value of DMA channel CHXMADDR register */
#define DMA_CHINTF_RESET_VALUE            ((uint32_t)0x0000003DU)                   /*!< clear DMA channel CHXINTFS register */
#define DMA_CHFCTL_RESET_VALUE            ((uint32_t)0x00000021U)                   /*!< the reset value of DMA channel CHXFCTL register */

/* DMA_INTF register */
/* interrupt flag bits */
#define DMA_INT_FLAG_FEE                  DMA_INTF_FEEIF                            /*!< FIFO error and exception flag */
#define DMA_INT_FLAG_SDE                  DMA_INTF_SDEIF                            /*!< single data mode exception flag */
#define DMA_INT_FLAG_TAE                  DMA_INTF_TAEIF                            /*!< transfer access error flag */
#define DMA_INT_FLAG_HTF                  DMA_INTF_HTFIF                            /*!< half transfer finish flag */
#define DMA_INT_FLAG_FTF                  DMA_INTF_FTFIF                            /*!< full transfer finish flag */

/* flag bits */
#define DMA_FLAG_FEE                      DMA_INTF_FEEIF                            /*!< FIFO error and exception flag */
#define DMA_FLAG_SDE                      DMA_INTF_SDEIF                            /*!< single data mode exception flag */
#define DMA_FLAG_TAE                      DMA_INTF_TAEIF                            /*!< transfer access error flag */
#define DMA_FLAG_HTF                      DMA_INTF_HTFIF                            /*!< half transfer finish flag */
#define DMA_FLAG_FTF                      DMA_INTF_FTFIF                            /*!< full transfer finish flag */

/* DMA_CHxCTL register */
/* interrupt enable bits */
#define DMA_INT_SDE                       DMA_CHXCTL_SDEIE                          /*!< enable bit for channel x single data mode exception interrupt */
#define DMA_INT_TAE                       DMA_CHXCTL_TAEIE                          /*!< enable bit for channel x tranfer access error interrupt */
#define DMA_INT_HTF                       DMA_CHXCTL_HTFIE                          /*!< enable bit for channel x half transfer finish interrupt */
#define DMA_INT_FTF                       DMA_CHXCTL_FTFIE                          /*!< enable bit for channel x full transfer finish interrupt */    
#define DMA_INT_FEE                       DMA_CHXFCTL_FEEIE                         /*!< FIFO exception interrupt enable */

/* function declarations */
/* DMA deinitialization and initialization functions */
/* deinitialize DMA a channel registers */
void dma_deinit(uint32_t dma_periph, dma_channel_enum channelx);
/* initialize the DMA single data mode parameters struct with the default values */
void dma_single_data_para_struct_init(dma_single_data_parameter_struct* init_struct);
/* initialize the DMA multi data mode parameters struct with the default values */
void dma_multi_data_para_struct_init(dma_multi_data_parameter_struct* init_struct);
/* DMA single data mode initialize */
void dma_single_data_mode_init(uint32_t dma_periph, dma_channel_enum channelx, dma_single_data_parameter_struct* init_struct);
/* DMA multi data mode initialize */
void dma_multi_data_mode_init(uint32_t dma_periph, dma_channel_enum channelx, dma_multi_data_parameter_struct* init_struct);

/* DMA configuration functions */
/* set DMA peripheral base address */
void dma_periph_address_config(uint32_t dma_periph, dma_channel_enum channelx, uint32_t address);
/* set DMA Memory base address */
void dma_memory_address_config(uint32_t dma_periph, dma_channel_enum channelx, uint8_t memory_flag, uint32_t address);

/* set the number of remaining data to be transferred by the DMA */
void dma_transfer_number_config(uint32_t dma_periph,dma_channel_enum channelx, uint32_t number);
/* get the number of remaining data to be transferred by the DMA */
uint32_t dma_transfer_number_get(uint32_t dma_periph, dma_channel_enum channelx);

/* configure priority level of DMA channel */
void dma_priority_config(uint32_t dma_periph, dma_channel_enum channelx, uint32_t priority);

/* configure transfer burst beats of memory */
void dma_memory_burst_beats_config (uint32_t dma_periph, dma_channel_enum channelx, uint32_t mbeat);
/* configure transfer burst beats of peripheral */
void dma_periph_burst_beats_config (uint32_t dma_periph, dma_channel_enum channelx, uint32_t pbeat);
/* configure transfer data size of memory */
void dma_memory_width_config (uint32_t dma_periph, dma_channel_enum channelx, uint32_t msize);
/* configure transfer data size of peripheral */
void dma_periph_width_config (uint32_t dma_periph, dma_channel_enum channelx, uint32_t psize);

/* configure next address increasement algorithm of memory */
void dma_memory_address_generation_config(uint32_t dma_periph, dma_channel_enum channelx, uint8_t generation_algorithm);
/* configure next address increasement algorithm of peripheral */
void dma_peripheral_address_generation_config(uint32_t dma_periph, dma_channel_enum channelx, uint8_t generation_algorithm);

/* enable DMA circulation mode */
void dma_circulation_enable(uint32_t dma_periph, dma_channel_enum channelx);
/* disable DMA circulation mode */
void dma_circulation_disable(uint32_t dma_periph, dma_channel_enum channelx);
/* enable DMA channel */
void dma_channel_enable(uint32_t dma_periph, dma_channel_enum channelx);
/* disable DMA channel */
void dma_channel_disable(uint32_t dma_periph, dma_channel_enum channelx);

/* configure the direction of data transfer on the channel */
void dma_transfer_direction_config(uint32_t dma_periph, dma_channel_enum channelx, uint8_t direction);

/* DMA switch buffer mode config */
void dma_switch_buffer_mode_config(uint32_t dma_periph, dma_channel_enum channelx, uint32_t memory1_addr, uint32_t memory_select);
/* DMA using memory get */
uint32_t dma_using_memory_get(uint32_t dma_periph, dma_channel_enum channelx);

/* DMA channel peripheral select */
void dma_channel_subperipheral_select(uint32_t dma_periph, dma_channel_enum channelx, dma_subperipheral_enum sub_periph);
/* DMA flow controller configure */
void dma_flow_controller_config(uint32_t dma_periph, dma_channel_enum channelx, uint32_t controller);
/* DMA flow controller enable */
void dma_switch_buffer_mode_enable(uint32_t dma_periph, dma_channel_enum channelx, ControlStatus newvalue);
/* DMA FIFO status get */
uint32_t dma_fifo_status_get(uint32_t dma_periph, dma_channel_enum channelx);

/* flag and interrupt functions */
/* check DMA flag is set or not */
FlagStatus dma_flag_get(uint32_t dma_periph, dma_channel_enum channelx, uint32_t flag);
/* clear DMA a channel flag */
void dma_flag_clear(uint32_t dma_periph, dma_channel_enum channelx, uint32_t flag);
/* enable DMA interrupt */
void dma_interrupt_enable(uint32_t dma_periph, dma_channel_enum channelx, uint32_t source);
/* disable DMA interrupt */
void dma_interrupt_disable(uint32_t dma_periph, dma_channel_enum channelx, uint32_t source);
/* check DMA flag is set or not */
FlagStatus dma_interrupt_flag_get(uint32_t dma_periph, dma_channel_enum channelx, uint32_t interrupt);
/* clear DMA a channel flag */
void dma_interrupt_flag_clear(uint32_t dma_periph, dma_channel_enum channelx, uint32_t interrupt);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* GD32F4XX_DMA_H */
