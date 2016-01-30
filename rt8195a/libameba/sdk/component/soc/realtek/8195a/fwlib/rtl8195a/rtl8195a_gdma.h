/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _RTL8195A_GDMA_H_
#define _RTL8195A_GDMA_H_

// Define GDMA Handshake interface with peripheral, 0 -> GDMA0, 1-> GDMA1
// Set this Hnadshake interface map to register REG_PESOC_SOC_CTRL
#define GDMA_HANDSHAKE_UART0_TX             0
#define GDMA_HANDSHAKE_UART0_RX             1
#define GDMA_HANDSHAKE_UART1_TX             2
#define GDMA_HANDSHAKE_UART1_RX             3
#define GDMA_HANDSHAKE_UART2_TX             14      // Only on GDMA 0, hardware fixed
#define GDMA_HANDSHAKE_UART2_RX             14      // Only on GDMA 1, hardware fixed

#define GDMA_HANDSHAKE_SSI0_TX              4
#define GDMA_HANDSHAKE_SSI0_RX              5
#define GDMA_HANDSHAKE_SSI1_TX              6
#define GDMA_HANDSHAKE_SSI1_RX              7
#define GDMA_HANDSHAKE_SSI2_TX              15      // Only on GDMA 0, hardware fixed
#define GDMA_HANDSHAKE_SSI2_RX              15      // Only on GDMA 1, hardware fixed

#define GDMA_HANDSHAKE_I2C0_TX              8
#define GDMA_HANDSHAKE_I2C0_RX              9
#define GDMA_HANDSHAKE_I2C1_TX              10
#define GDMA_HANDSHAKE_I2C1_RX              11

#define GDMA_HANDSHAKE_ADC                  12
#define GDMA_HANDSHAKE_DAC0                 13      // Only on GDMA 0, hardware fixed
#define GDMA_HANDSHAKE_DAC1                 13      // Only on GDMA 1, hardware fixed

#define HAL_GDMAX_READ32(GdmaIndex, addr)     \
                HAL_READ32(GDMA0_REG_BASE+ (GdmaIndex*GDMA1_REG_OFF), addr)
#define HAL_GDMAX_WRITE32(GdmaIndex, addr, value)     \
                HAL_WRITE32((GDMA0_REG_BASE+ (GdmaIndex*GDMA1_REG_OFF)), addr, value)
#define HAL_GDMAX_READ16(GdmaIndex, addr)     \
                HAL_READ16(GDMA0_REG_BASE+ (GdmaIndex*GDMA1_REG_OFF), addr)
#define HAL_GDMAX_WRITE16(GdmaIndex, addr, value)     \
                HAL_WRITE16(GDMA0_REG_BASE+ (GdmaIndex*GDMA1_REG_OFF), addr, value)
#define HAL_GDMAX_READ8(GdmaIndex, addr)     \
                HAL_READ8(GDMA0_REG_BASE+ (GdmaIndex*GDMA1_REG_OFF), addr)
#define HAL_GDMAX_WRITE8(GdmaIndex, addr, value)     \
                HAL_WRITE8(GDMA0_REG_BASE+ (GdmaIndex*GDMA1_REG_OFF), addr, value)


#define GDMA_CH_MAX                 0x06

#define REG_GDMA_CH_OFF                 0x058
#define REG_GDMA_CH_SAR                 0x000
#define REG_GDMA_CH_DAR                 0x008
#define REG_GDMA_CH_LLP                 0x010
#define REG_GDMA_CH_CTL                 0x018
#define REG_GDMA_CH_SSTAT               0x020
#define REG_GDMA_CH_DSTAT               0x028
#define REG_GDMA_CH_SSTATAR             0x030
#define REG_GDMA_CH_DSTATAR             0x038
#define REG_GDMA_CH_CFG                 0x040
#define REG_GDMA_CH_SGR                 0x048
#define REG_GDMA_CH_DSR                 0x050

//3 Interrupt Registers
#define REG_GDMA_RAW_INT_BASE           0x2C0
#define REG_GDMA_RAW_INT_TFR            0x2C0
#define REG_GDMA_RAW_INT_BLOCK          0x2c8
#define REG_GDMA_RAW_INT_SRC_TRAN       0x2D0
#define REG_GDMA_RAW_INT_DST_TRAN       0x2D8
#define REG_GDMA_RAW_INT_ERR            0x2E0

#define REG_GDMA_STATUS_INT_BASE        0x2E8
#define REG_GDMA_STATUS_INT_TFR         0x2E8
#define REG_GDMA_STATUS_INT_BLOCK       0x2F0
#define REG_GDMA_STATUS_INT_SRC_TRAN    0x2F8
#define REG_GDMA_STATUS_INT_DST_TRAN    0x300
#define REG_GDMA_STATUS_INT_ERR         0x308

#define REG_GDMA_MASK_INT_BASE          0x310
#define REG_GDMA_MASK_INT_TFR           0x310
#define REG_GDMA_MASK_INT_BLOCK         0x318
#define REG_GDMA_MASK_INT_SRC_TRAN      0x320
#define REG_GDMA_MASK_INT_DST_TRAN      0x328
#define REG_GDMA_MASK_INT_INT_ERR       0x330

#define REG_GDMA_CLEAR_INT_BASE         0x338
#define REG_GDMA_CLEAR_INT_TFR          0x338
#define REG_GDMA_CLEAR_INT_BLOCK        0x340
#define REG_GDMA_CLEAR_INT_SRC_TRAN     0x348
#define REG_GDMA_CLEAR_INT_DST_TRAN     0x350
#define REG_GDMA_CLEAR_INT_ERR          0x358
#define REG_GDMA_STATUS_INT             0x360

//3 Software handshaking Registers
#define REG_GDMA_REQ_SRC                0x368
#define REG_GDMA_REQ_DST                0x370
#define REG_GDMA_REQ_SGL_REQ            0x378
#define REG_GDMA_REQ_DST_REQ            0x380
#define REG_GDMA_REQ_LST_SRC            0x388
#define REG_GDMA_REQ_LST_DST            0x390

//3 Miscellaneous Registers
#define REG_GDMA_DMAC_CFG               0x398
#define REG_GDMA_CH_EN                  0x3A0
#define REG_GDMA_DMA_ID                 0x3A8
#define REG_GDMA_DMA_TEST               0x3B0
#define REG_GDMA_DMA_COM_PARAMS6        0x3C8
#define REG_GDMA_DMA_COM_PARAMS5        0x3D0
#define REG_GDMA_DMA_COM_PARAMS4        0x3D8
#define REG_GDMA_DMA_COM_PARAMS3        0x3E0
#define REG_GDMA_DMA_COM_PARAMS2        0x3E8
#define REG_GDMA_DMA_COM_PARAMS1        0x3F0
#define REG_GDMA_DMA_COM_PARAMS0        0x3F8

//3 CTL Register Bit Control
#define BIT_SHIFT_CTLX_LO_INT_EN 0
#define BIT_MASK_CTLX_LO_INT_EN 0x1
#define BIT_CTLX_LO_INT_EN(x)(((x) & BIT_MASK_CTLX_LO_INT_EN) << BIT_SHIFT_CTLX_LO_INT_EN)
#define BIT_INVC_CTLX_LO_INT_EN (~(BIT_MASK_CTLX_LO_INT_EN << BIT_SHIFT_CTLX_LO_INT_EN))

#define BIT_SHIFT_CTLX_LO_DST_TR_WIDTH 1
#define BIT_MASK_CTLX_LO_DST_TR_WIDTH 0x7
#define BIT_CTLX_LO_DST_TR_WIDTH(x) (((x) & BIT_MASK_CTLX_LO_DST_TR_WIDTH) << BIT_SHIFT_CTLX_LO_DST_TR_WIDTH)
#define BIT_INVC_CTLX_LO_DST_TR_WIDTH (~(BIT_MASK_CTLX_LO_DST_TR_WIDTH << BIT_SHIFT_CTLX_LO_DST_TR_WIDTH))

#define BIT_SHIFT_CTLX_LO_SRC_TR_WIDTH 4
#define BIT_MASK_CTLX_LO_SRC_TR_WIDTH 0x7
#define BIT_CTLX_LO_SRC_TR_WIDTH(x) (((x) & BIT_MASK_CTLX_LO_SRC_TR_WIDTH) << BIT_SHIFT_CTLX_LO_SRC_TR_WIDTH)
#define BIT_INVC_CTLX_LO_SRC_TR_WIDTH (~(BIT_MASK_CTLX_LO_SRC_TR_WIDTH << BIT_SHIFT_CTLX_LO_SRC_TR_WIDTH))

#define BIT_SHIFT_CTLX_LO_DINC 7
#define BIT_MASK_CTLX_LO_DINC 0x3
#define BIT_CTLX_LO_DINC(x)(((x) & BIT_MASK_CTLX_LO_DINC) << BIT_SHIFT_CTLX_LO_DINC)
#define BIT_INVC_CTLX_LO_DINC (~(BIT_MASK_CTLX_LO_DINC << BIT_SHIFT_CTLX_LO_DINC))

#define BIT_SHIFT_CTLX_LO_SINC 9
#define BIT_MASK_CTLX_LO_SINC 0x3
#define BIT_CTLX_LO_SINC(x)(((x) & BIT_MASK_CTLX_LO_SINC) << BIT_SHIFT_CTLX_LO_SINC)
#define BIT_INVC_CTLX_LO_SINC (~(BIT_MASK_CTLX_LO_SINC << BIT_SHIFT_CTLX_LO_SINC))

#define BIT_SHIFT_CTLX_LO_DEST_MSIZE 11
#define BIT_MASK_CTLX_LO_DEST_MSIZE 0x7
#define BIT_CTLX_LO_DEST_MSIZE(x)(((x) & BIT_MASK_CTLX_LO_DEST_MSIZE) << BIT_SHIFT_CTLX_LO_DEST_MSIZE)
#define BIT_INVC_CTLX_LO_DEST_MSIZE (~(BIT_MASK_CTLX_LO_DEST_MSIZE << BIT_SHIFT_CTLX_LO_DEST_MSIZE))

#define BIT_SHIFT_CTLX_LO_SRC_MSIZE 14
#define BIT_MASK_CTLX_LO_SRC_MSIZE 0x7
#define BIT_CTLX_LO_SRC_MSIZE(x)(((x) & BIT_MASK_CTLX_LO_SRC_MSIZE) << BIT_SHIFT_CTLX_LO_SRC_MSIZE)
#define BIT_INVC_CTLX_LO_SRC_MSIZE (~(BIT_MASK_CTLX_LO_SRC_MSIZE << BIT_SHIFT_CTLX_LO_SRC_MSIZE))


#define BIT_SHIFT_CTLX_LO_SRC_GATHER_EN 17
#define BIT_MASK_CTLX_LO_SRC_GATHER_EN 0x1
#define BIT_CTLX_LO_SRC_GATHER_EN(x)(((x) & BIT_MASK_CTLX_LO_SRC_GATHER_EN) << BIT_SHIFT_CTLX_LO_SRC_GATHER_EN)
#define BIT_INVC_CTLX_LO_SRC_GATHER_EN (~(BIT_MASK_CTLX_LO_SRC_GATHER_EN << BIT_SHIFT_CTLX_LO_SRC_GATHER_EN))


#define BIT_SHIFT_CTLX_LO_DST_SCATTER_EN 18
#define BIT_MASK_CTLX_LO_DST_SCATTER_EN 0x1
#define BIT_CTLX_LO_DST_SCATTER_EN(x)(((x) & BIT_MASK_CTLX_LO_DST_SCATTER_EN) << BIT_SHIFT_CTLX_LO_DST_SCATTER_EN)
#define BIT_INVC_CTLX_LO_DST_SCATTER_EN (~(BIT_MASK_CTLX_LO_DST_SCATTER_EN << BIT_SHIFT_CTLX_LO_DST_SCATTER_EN))


#define BIT_SHIFT_CTLX_LO_TT_FC 20
#define BIT_MASK_CTLX_LO_TT_FC 0x7
#define BIT_CTLX_LO_TT_FC(x)(((x) & BIT_MASK_CTLX_LO_TT_FC) << BIT_SHIFT_CTLX_LO_TT_FC)
#define BIT_INVC_CTLX_LO_TT_FC (~(BIT_MASK_CTLX_LO_TT_FC << BIT_SHIFT_CTLX_LO_TT_FC))


#define BIT_SHIFT_CTLX_LO_DMS 23
#define BIT_MASK_CTLX_LO_DMS 0x3
#define BIT_CTLX_LO_DMS(x)(((x) & BIT_MASK_CTLX_LO_DMS) << BIT_MASK_CTLX_LO_DMS)
#define BIT_INVC_CTLX_LO_DMS (~(BIT_MASK_CTLX_LO_DMS << BIT_SHIFT_CTLX_LO_DMS))


#define BIT_SHIFT_CTLX_LO_SMS 25
#define BIT_MASK_CTLX_LO_SMS 0x3
#define BIT_CTLX_LO_SMS(x)(((x) & BIT_MASK_CTLX_LO_SMS) << BIT_SHIFT_CTLX_LO_SMS)
#define BIT_INVC_CTLX_LO_SMS (~(BIT_MASK_CTLX_LO_SMS << BIT_SHIFT_CTLX_LO_SMS))


#define BIT_SHIFT_CTLX_LO_LLP_DST_EN 27
#define BIT_MASK_CTLX_LO_LLP_DST_EN 0x1
#define BIT_CTLX_LO_LLP_DST_EN(x)(((x) & BIT_MASK_CTLX_LO_LLP_DST_EN) << BIT_SHIFT_CTLX_LO_LLP_DST_EN)
#define BIT_INVC_CTLX_LO_LLP_DST_EN (~(BIT_MASK_CTLX_LO_LLP_DST_EN << BIT_SHIFT_CTLX_LO_LLP_DST_EN))

#define BIT_SHIFT_CTLX_LO_LLP_SRC_EN 28
#define BIT_MASK_CTLX_LO_LLP_SRC_EN 0x1
#define BIT_CTLX_LO_LLP_SRC_EN(x)(((x) & BIT_MASK_CTLX_LO_LLP_SRC_EN) << BIT_SHIFT_CTLX_LO_LLP_SRC_EN)
#define BIT_INVC_CTLX_LO_LLP_SRC_EN (~(BIT_MASK_CTLX_LO_LLP_SRC_EN << BIT_SHIFT_CTLX_LO_LLP_SRC_EN))


#define BIT_SHIFT_CTLX_UP_BLOCK_BS 0
#define BIT_MASK_CTLX_UP_BLOCK_BS 0xFFF
#define BIT_CTLX_UP_BLOCK_BS(x)(((x) & BIT_MASK_CTLX_UP_BLOCK_BS) << BIT_SHIFT_CTLX_UP_BLOCK_BS)
#define BIT_INVC_CTLX_UP_BLOCK_BS (~(BIT_MASK_CTLX_UP_BLOCK_BS << BIT_SHIFT_CTLX_UP_BLOCK_BS))


#define BIT_SHIFT_CTLX_UP_DONE 12
#define BIT_MASK_CTLX_UP_DONE 0x1
#define BIT_CTLX_UP_DONE(x)(((x) & BIT_MASK_CTLX_UP_DONE) << BIT_SHIFT_CTLX_UP_DONE)
#define BIT_INVC_CTLX_UP_DONE (~(BIT_MASK_CTLX_UP_DONE << BIT_SHIFT_CTLX_UP_DONE))


//3 CFG Register Bit Control
#define BIT_SHIFT_CFGX_LO_CH_PRIOR 5
#define BIT_MASK_CFGX_LO_CH_PRIOR 0x7
#define BIT_CFGX_LO_CH_PRIOR(x)(((x) & BIT_MASK_CFGX_LO_CH_PRIOR) << BIT_SHIFT_CFGX_LO_CH_PRIOR)
#define BIT_INVC_CFGX_LO_CH_PRIOR (~(BIT_MASK_CFGX_LO_CH_PRIOR << BIT_SHIFT_CFGX_LO_CH_PRIOR))


#define BIT_SHIFT_CFGX_LO_CH_SUSP 8
#define BIT_MASK_CFGX_LO_CH_SUSP 0x1
#define BIT_CFGX_LO_CH_SUSP(x)(((x) & BIT_MASK_CFGX_LO_CH_SUSP) << BIT_SHIFT_CFGX_LO_CH_SUSP)
#define BIT_INVC_CFGX_LO_CH_SUSP (~(BIT_MASK_CFGX_LO_CH_SUSP << BIT_SHIFT_CFGX_LO_CH_SUSP))


#define BIT_SHIFT_CFGX_LO_FIFO_EMPTY 9
#define BIT_MASK_CFGX_LO_FIFO_EMPTY 0x1
#define BIT_CFGX_LO_FIFO_EMPTY(x)(((x) & BIT_MASK_CFGX_LO_FIFO_EMPTY) << BIT_SHIFT_CFGX_LO_FIFO_EMPTY)
#define BIT_INVC_CFGX_LO_FIFO_EMPTY (~(BIT_MASK_CFGX_LO_FIFO_EMPTY << BIT_SHIFT_CFGX_LO_FIFO_EMPTY))


#define BIT_SHIFT_CFGX_LO_HS_SEL_DST 10
#define BIT_MASK_CFGX_LO_HS_SEL_DST 0x1
#define BIT_CFGX_LO_HS_SEL_DST(x)(((x) & BIT_MASK_CFGX_LO_HS_SEL_DST) << BIT_SHIFT_CFGX_LO_HS_SEL_DST)
#define BIT_INVC_CFGX_LO_HS_SEL_DST (~(BIT_MASK_CFGX_LO_HS_SEL_DST << BIT_SHIFT_CFGX_LO_HS_SEL_DST))

#define BIT_SHIFT_CFGX_LO_HS_SEL_SRC 11
#define BIT_MASK_CFGX_LO_HS_SEL_SRC 0x1
#define BIT_CFGX_LO_HS_SEL_SRC(x)(((x) & BIT_MASK_CFGX_LO_HS_SEL_SRC) << BIT_SHIFT_CFGX_LO_HS_SEL_SRC)
#define BIT_INVC_CFGX_LO_HS_SEL_SRC (~(BIT_MASK_CFGX_LO_HS_SEL_SRC << BIT_SHIFT_CFGX_LO_HS_SEL_SRC))

#define BIT_SHIFT_CFGX_LO_LOCK_CH_L 12
#define BIT_MASK_CFGX_LO_LOCK_CH_L 0x3
#define BIT_CFGX_LO_LOCK_CH_L(x)(((x) & BIT_MASK_CFGX_LO_LOCK_CH_L) << BIT_SHIFT_CFGX_LO_LOCK_CH_L)
#define BIT_INVC_CFGX_LO_LOCK_CH_L (~(BIT_MASK_CFGX_LO_LOCK_CH_L << BIT_SHIFT_CFGX_LO_LOCK_CH_L))

#define BIT_SHIFT_CFGX_LO_LOCK_B_L 14
#define BIT_MASK_CFGX_LO_LOCK_B_L 0x3
#define BIT_CFGX_LO_LOCK_B_L(x)(((x) & BIT_MASK_CFGX_LO_LOCK_B_L) << BIT_SHIFT_CFGX_LO_LOCK_B_L)
#define BIT_INVC_CFGX_LO_LOCK_B_L (~(BIT_MASK_CFGX_LO_LOCK_B_L << BIT_SHIFT_CFGX_LO_LOCK_B_L))

#define BIT_SHIFT_CFGX_LO_LOCK_CH 16
#define BIT_MASK_CFGX_LO_LOCK_CH 0x1
#define BIT_CFGX_LO_LOCK_CH(x)(((x) & BIT_MASK_CFGX_LO_LOCK_CH) << BIT_SHIFT_CFGX_LO_LOCK_CH)
#define BIT_INVC_CFGX_LO_LOCK_CH (~(BIT_MASK_CFGX_LO_LOCK_CH << BIT_SHIFT_CFGX_LO_LOCK_CH))

#define BIT_SHIFT_CFGX_LO_LOCK_B 17
#define BIT_MASK_CFGX_LO_LOCK_B 0x1
#define BIT_CFGX_LO_LOCK_B(x)(((x) & BIT_MASK_CFGX_LO_LOCK_B) << BIT_SHIFT_CFGX_LO_LOCK_B)
#define BIT_INVC_CFGX_LO_LOCK_B (~(BIT_MASK_CFGX_LO_LOCK_B << BIT_SHIFT_CFGX_LO_LOCK_B))

#define BIT_SHIFT_CFGX_LO_DST_HS_POL 18
#define BIT_MASK_CFGX_LO_DST_HS_POL 0x1
#define BIT_CFGX_LO_DST_HS_POL(x)(((x) & BIT_MASK_CFGX_LO_DST_HS_POL) << BIT_SHIFT_CFGX_LO_DST_HS_POL)
#define BIT_INVC_CFGX_LO_DST_HS_POL (~(BIT_MASK_CFGX_LO_DST_HS_POL << BIT_SHIFT_CFGX_LO_DST_HS_POL))

#define BIT_SHIFT_CFGX_LO_SRC_HS_POL 19
#define BIT_MASK_CFGX_LO_SRC_HS_POL 0x1
#define BIT_CFGX_LO_SRC_HS_POL(x)(((x) & BIT_MASK_CFGX_LO_SRC_HS_POL) << BIT_SHIFT_CFGX_LO_SRC_HS_POL)
#define BIT_INVC_CFGX_LO_SRC_HS_POL (~(BIT_MASK_CFGX_LO_SRC_HS_POL << BIT_SHIFT_CFGX_LO_SRC_HS_POL))

#define BIT_SHIFT_CFGX_LO_MAX_ABRST 20
#define BIT_MASK_CFGX_LO_MAX_ABRST 0x3FF
#define BIT_CFGX_LO_MAX_ABRST(x)(((x) & BIT_MASK_CFGX_LO_MAX_ABRST) << BIT_SHIFT_CFGX_LO_MAX_ABRST)
#define BIT_INVC_CFGX_LO_MAX_ABRST (~(BIT_MASK_CFGX_LO_MAX_ABRST << BIT_SHIFT_CFGX_LO_MAX_ABRST))

#define BIT_SHIFT_CFGX_LO_RELOAD_SRC 30
#define BIT_MASK_CFGX_LO_RELOAD_SRC 0x1
#define BIT_CFGX_LO_RELOAD_SRC(x)(((x) & BIT_MASK_CFGX_LO_RELOAD_SRC) << BIT_SHIFT_CFGX_LO_RELOAD_SRC)
#define BIT_INVC_CFGX_LO_RELOAD_SRC (~(BIT_MASK_CFGX_LO_RELOAD_SRC << BIT_SHIFT_CFGX_LO_RELOAD_SRC))

#define BIT_SHIFT_CFGX_LO_RELOAD_DST 31
#define BIT_MASK_CFGX_LO_RELOAD_DST 0x1
#define BIT_CFGX_LO_RELOAD_DST(x)(((x) & BIT_MASK_CFGX_LO_RELOAD_DST) << BIT_SHIFT_CFGX_LO_RELOAD_DST)
#define BIT_INVC_CFGX_LO_RELOAD_DST (~(BIT_MASK_CFGX_LO_RELOAD_DST << BIT_SHIFT_CFGX_LO_RELOAD_DST))

#define BIT_SHIFT_CFGX_UP_FCMODE 0
#define BIT_MASK_CFGX_UP_FCMODE 0x1
#define BIT_CFGX_UP_FCMODE(x)(((x) & BIT_MASK_CFGX_UP_FCMODE) << BIT_SHIFT_CFGX_UP_FCMODE)
#define BIT_INVC_CFGX_UP_FCMODE (~(BIT_MASK_CFGX_UP_FCMODE << BIT_SHIFT_CFGX_UP_FCMODE))

#define BIT_SHIFT_CFGX_UP_FIFO_MODE 1
#define BIT_MASK_CFGX_UP_FIFO_MODE 0x1
#define BIT_CFGX_UP_FIFO_MODE(x)(((x) & BIT_MASK_CFGX_UP_FIFO_MODE) << BIT_SHIFT_CFGX_UP_FIFO_MODE)
#define BIT_INVC_CFGX_UP_FIFO_MODE (~(BIT_MASK_CFGX_UP_FIFO_MODE << BIT_SHIFT_CFGX_UP_FIFO_MODE))

#define BIT_SHIFT_CFGX_UP_PROTCTL 2
#define BIT_MASK_CFGX_UP_PROTCTL 0x7
#define BIT_CFGX_UP_PROTCTL(x)(((x) & BIT_MASK_CFGX_UP_PROTCTL) << BIT_SHIFT_CFGX_UP_PROTCTL)
#define BIT_INVC_CFGX_UP_PROTCTL (~(BIT_MASK_CFGX_UP_PROTCTL << BIT_SHIFT_CFGX_UP_PROTCTL))

#define BIT_SHIFT_CFGX_UP_DS_UPD_EN 5
#define BIT_MASK_CFGX_UP_DS_UPD_EN 0x1
#define BIT_CFGX_UP_DS_UPD_EN(x)(((x) & BIT_MASK_CFGX_UP_DS_UPD_EN) << BIT_SHIFT_CFGX_UP_DS_UPD_EN)
#define BIT_INVC_CFGX_UP_DS_UPD_EN (~(BIT_MASK_CFGX_UP_DS_UPD_EN << BIT_SHIFT_CFGX_UP_DS_UPD_EN))

#define BIT_SHIFT_CFGX_UP_SS_UPD_EN 6
#define BIT_MASK_CFGX_UP_SS_UPD_EN 0x1
#define BIT_CFGX_UP_SS_UPD_EN(x)(((x) & BIT_MASK_CFGX_UP_SS_UPD_EN) << BIT_SHIFT_CFGX_UP_SS_UPD_EN)
#define BIT_INVC_CFGX_UP_SS_UPD_EN (~(BIT_MASK_CFGX_UP_SS_UPD_EN << BIT_SHIFT_CFGX_UP_SS_UPD_EN))

#define BIT_SHIFT_CFGX_UP_SRC_PER 7
#define BIT_MASK_CFGX_UP_SRC_PER 0xF
#define BIT_CFGX_UP_SRC_PER(x)(((x) & BIT_MASK_CFGX_UP_SRC_PER) << BIT_SHIFT_CFGX_UP_SRC_PER)
#define BIT_INVC_CFGX_UP_SRC_PER (~(BIT_MASK_CFGX_UP_SRC_PER << BIT_SHIFT_CFGX_UP_SRC_PER))

#define BIT_SHIFT_CFGX_UP_DEST_PER 11
#define BIT_MASK_CFGX_UP_DEST_PER 0xF
#define BIT_CFGX_UP_DEST_PER(x)(((x) & BIT_MASK_CFGX_UP_DEST_PER) << BIT_SHIFT_CFGX_UP_DEST_PER)
#define BIT_INVC_CFGX_UP_DEST_PER (~(BIT_MASK_CFGX_UP_DEST_PER << BIT_SHIFT_CFGX_UP_DEST_PER))

typedef enum _GDMA_CHANNEL_NUM_ {
        GdmaNoCh    = 0x0000,
        GdmaCh0     = 0x0101,
        GdmaCh1     = 0x0202,
        GdmaCh2     = 0x0404,
        GdmaCh3     = 0x0808,
        GdmaCh4     = 0x1010,
        GdmaCh5     = 0x2020,
        GdmaCh6     = 0x4040,
        GdmaCh7     = 0x8080,
        GdmaAllCh   = 0xffff
}GDMA_CHANNEL_NUM, *PGDMA_CHANNEL_NUM;


//3 CTL register struct

typedef enum _GDMA_CTL_TT_FC_TYPE_ {
        TTFCMemToMem    = 0x00,
        TTFCMemToPeri   = 0x01,
        TTFCPeriToMem   = 0x02
}GDMA_CTL_TT_FC_TYPE, *PGDMA_CTL_TT_FC_TYPE;

//Max type = Bus Width
typedef enum _GDMA_CTL_TR_WIDTH_ {
        TrWidthOneByte    = 0x00,
        TrWidthTwoBytes   = 0x01,
        TrWidthFourBytes  = 0x02
}GDMA_CTL_TR_WIDTH, *PGDMA_CTL_TR_WIDTH;

typedef enum _GDMA_CTL_MSIZE_ {
        MsizeOne        = 0x00,
        MsizeFour       = 0x01,
        MsizeEight      = 0x02
}GDMA_CTL_MSIZE, *PGDMA_CTL_MSIZE;

typedef enum _GDMA_INC_TYPE_ {
        IncType         = 0x00,
        DecType         = 0x01,
        NoChange        = 0x02
}GDMA_INC_TYPE, *PGDMA_INC_TYPE;


typedef struct _GDMA_CTL_REG_ {
        GDMA_CTL_TT_FC_TYPE     TtFc;
        GDMA_CTL_TR_WIDTH       DstTrWidth;
        GDMA_CTL_TR_WIDTH       SrcTrWidth;
        GDMA_INC_TYPE           Dinc;
        GDMA_INC_TYPE           Sinc;
        GDMA_CTL_MSIZE          DestMsize;
        GDMA_CTL_MSIZE          SrcMsize;

        u8                      IntEn           :1; // Bit 0
        u8                      SrcGatherEn     :1; // Bit 1
        u8                      DstScatterEn    :1; // Bit 2
        u8                      LlpDstEn        :1; // Bit 3
        u8                      LlpSrcEn        :1; // Bit 4
        u8                      Done            :1; // Bit 5
        u8                      Rsvd6To7        :2; //Bit 6 -7
        u16                     BlockSize;
        
}GDMA_CTL_REG, *PGDMA_CTL_REG;


//3 CFG Register Structure

typedef enum _GDMA_CH_PRIORITY_ {
    Prior0          = 0,
    Prior1          = 1,
    Prior2          = 2,
    Prior3          = 3,
    Prior4          = 4,
    Prior5          = 5,
    Prior6          = 6,
    Prior7          = 7
}GDMA_CH_PRIORITY, *PGDMA_CH_PRIORITY;

typedef enum _GDMA_LOCK_LEVEL_ {
    OverComplDmaTransfer        = 0x00,
    OverComplDmaBlockTransfer   = 0x01,
    OverComplDmaTransation      = 0x02
}GDMA_LOCK_LEVEL, *PGDMA_LOCK_LEVEL;


typedef struct _GDMA_CFG_REG_ {
    GDMA_CH_PRIORITY        ChPrior;
    GDMA_LOCK_LEVEL         LockBL;     
    GDMA_LOCK_LEVEL         LockChL;     
    u16                     MaxAbrst;
    u8                      SrcPer;
    u8                      DestPer;
    u16                     ChSusp      :1; //Bit 0
    u16                     FifoEmpty   :1; //Bit 1
    u16                     HsSelDst    :1; //Bit 2
    u16                     HsSelSrc    :1; //Bit 3
    u16                     LockCh      :1; //Bit 4
    u16                     LockB       :1; //Bit 5
    u16                     DstHsPol    :1; //Bit 6
    u16                     SrcHsPol    :1; //Bit 7
    u16                     ReloadSrc   :1; //Bit 8
    u16                     ReloadDst   :1; //Bit 9
    u16                     FifoMode    :1; //Bit 10
    u16                     DsUpdEn     :1; //Bit 11
    u16                     SsUpdEn     :1; //Bit 12
    u16                     Rsvd13To15  :3;
}GDMA_CFG_REG, *PGDMA_CFG_REG;

typedef enum _GDMA_ISR_TYPE_ {
    TransferType        = 0x1,
    BlockType           = 0x2,
    SrcTransferType     = 0x4,
    DstTransferType     = 0x8,
    ErrType             = 0x10
}GDMA_ISR_TYPE, *PGDMA_ISR_TYPE;


VOID
HalGdmaOnOffRtl8195a (
    IN  VOID              *Data
);

BOOL
HalGdamChInitRtl8195a(
    IN  VOID     *Data
);

BOOL
HalGdmaChSetingRtl8195a(
    IN  VOID     *Data
);

BOOL
HalGdmaChBlockSetingRtl8195a(
    IN  VOID     *Data
);


VOID
HalGdmaChDisRtl8195a (
    IN  VOID              *Data
);

VOID
HalGdmaChEnRtl8195a (
    IN  VOID              *Data
);

VOID
HalGdmaChIsrEnAndDisRtl8195a (
    IN  VOID              *Data
);

u8
HalGdmaChIsrCleanRtl8195a (
    IN  VOID              *Data
);

VOID
HalGdmaChCleanAutoSrcRtl8195a (
    IN  VOID              *Data
);

VOID
HalGdmaChCleanAutoDstRtl8195a (
    IN  VOID              *Data
); 

u32
HalGdmaQueryDArRtl8195a(
    IN  VOID     *Data
);

u32
HalGdmaQuerySArRtl8195a(
    IN  VOID     *Data
);

BOOL
HalGdmaQueryChEnRtl8195a (
    IN  VOID              *Data
);

#ifdef CONFIG_CHIP_E_CUT
_LONG_CALL_ BOOL
HalGdmaChBlockSetingRtl8195a_V04(
    IN  VOID     *Data
);

_LONG_CALL_ u32
HalGdmaQueryDArRtl8195a_V04(
    IN  VOID     *Data
);

_LONG_CALL_ u32
HalGdmaQuerySArRtl8195a_V04(
    IN  VOID     *Data
);

_LONG_CALL_ BOOL
HalGdmaQueryChEnRtl8195a_V04 (
    IN  VOID      *Data
);

#endif  // #ifdef CONFIG_CHIP_E_CUT

#endif
