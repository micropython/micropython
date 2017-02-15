/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _HAL_SSI_H_
#define _HAL_SSI_H_

#include "rtl8195a_ssi.h"

/**
 * LOG Configurations
 */

extern u32 SSI_DBG_CONFIG;
extern uint8_t SPI0_IS_AS_SLAVE;


#define SSI_DBG_ENTRANCE(...)  do {\
    if (unlikely(SSI_DBG_CONFIG & DBG_TYPE_ENTRANCE)) \
        DBG_SSI_INFO(IDENT_FOUR_SPACE ANSI_COLOR_GREEN __VA_ARGS__ ANSI_COLOR_RESET); \
}while(0)

#define SSI_DBG_INIT(...)  do {\
    if (unlikely(SSI_DBG_CONFIG & DBG_TYPE_INIT)) \
        DBG_SSI_INFO(IDENT_FOUR_SPACE __VA_ARGS__); \
}while(0)

#define SSI_DBG_INIT_V(...)  do {\
    if (unlikely(SSI_DBG_CONFIG & DBG_TYPE_INIT_V)) \
        DBG_SSI_INFO(IDENT_FOUR_SPACE __VA_ARGS__); \
}while(0)

#define SSI_DBG_INIT_VV(...)  do {\
    if (unlikely(SSI_DBG_CONFIG & DBG_TYPE_INIT_VV)) \
        DBG_SSI_INFO(IDENT_FOUR_SPACE __VA_ARGS__); \
}while(0)

#define SSI_DBG_PINMUX(...)  do {\
    if (unlikely(SSI_DBG_CONFIG & DBG_TYPE_PINMUX)) \
        DBG_SSI_INFO(IDENT_FOUR_SPACE __VA_ARGS__); \
}while(0)

#define SSI_DBG_ENDIS(...)  do {\
    if (unlikely(SSI_DBG_CONFIG & DBG_TYPE_ENDIS)) \
        DBG_SSI_INFO(IDENT_FOUR_SPACE __VA_ARGS__); \
}while(0)

#define SSI_DBG_INT(...)  do {\
    if (unlikely(SSI_DBG_CONFIG & DBG_TYPE_INT)) \
        DBG_SSI_INFO(IDENT_FOUR_SPACE __VA_ARGS__); \
}while(0)

#define SSI_DBG_INT_V(...)  do {\
    if (unlikely(SSI_DBG_CONFIG & DBG_TYPE_INT_V)) \
        DBG_SSI_INFO(IDENT_FOUR_SPACE __VA_ARGS__); \
}while(0)

#define SSI_DBG_INT_HNDLR(...)  do {\
    if (unlikely(SSI_DBG_CONFIG & DBG_TYPE_INT_HNDLR)) \
        DBG_SSI_INFO(IDENT_FOUR_SPACE __VA_ARGS__); \
}while(0)

#define SSI_DBG_INT_READ(...)  do {\
    if (unlikely(SSI_DBG_CONFIG & DBG_TYPE_INT_READ)) \
        DBG_SSI_INFO(IDENT_FOUR_SPACE __VA_ARGS__); \
}while(0)

#define SSI_DBG_INT_WRITE(...)  do {\
    if (unlikely(SSI_DBG_CONFIG & DBG_TYPE_INT_WRITE)) \
        DBG_SSI_INFO(IDENT_FOUR_SPACE __VA_ARGS__); \
}while(0)

#define SSI_DBG_STATUS(...)  do {\
    if (unlikely(SSI_DBG_CONFIG & DBG_TYPE_STATUS)) \
        DBG_SSI_INFO(IDENT_FOUR_SPACE __VA_ARGS__); \
}while(0)

#define SSI_DBG_FIFO(...)  do {\
    if (unlikely(SSI_DBG_CONFIG & DBG_TYPE_FIFO)) \
        DBG_SSI_INFO(IDENT_FOUR_SPACE __VA_ARGS__); \
}while(0)

#define SSI_DBG_READ(...)  do {\
    if (unlikely(SSI_DBG_CONFIG & DBG_TYPE_READ)) \
        DBG_SSI_INFO(IDENT_FOUR_SPACE __VA_ARGS__); \
}while(0)

#define SSI_DBG_WRITE(...)  do {\
    if (unlikely(SSI_DBG_CONFIG & DBG_TYPE_WRITE)) \
        DBG_SSI_INFO(IDENT_FOUR_SPACE __VA_ARGS__); \
}while(0)

#define SSI_DBG_SLV_CTRL(...)  do {\
    if (unlikely(SSI_DBG_CONFIG & DBG_TYPE_SLV_CTRL)) \
        DBG_SSI_INFO(IDENT_FOUR_SPACE __VA_ARGS__); \
}while(0)

typedef enum _SSI_DBG_TYPE_LIST_ {
    DBG_TYPE_ENTRANCE  = 1 << 0,
    DBG_TYPE_INIT      = 1 << 1,
    DBG_TYPE_INIT_V    = 1 << 2,
    DBG_TYPE_INIT_VV   = 1 << 3,
    DBG_TYPE_PINMUX    = 1 << 4,
    DBG_TYPE_ENDIS     = 1 << 5,
    DBG_TYPE_INT       = 1 << 6,
    DBG_TYPE_INT_V     = 1 << 7,
    DBG_TYPE_INT_HNDLR = 1 << 8,
    DBG_TYPE_INT_READ  = 1 << 9,
    DBG_TYPE_INT_WRITE = 1 << 10,
    DBG_TYPE_STATUS    = 1 << 11,
    DBG_TYPE_FIFO      = 1 << 12,
    DBG_TYPE_READ      = 1 << 13,
    DBG_TYPE_WRITE     = 1 << 14,
    DBG_TYPE_SLV_CTRL  = 1 << 15
} SSI_DBG_TYPE_LIST, *PSSI_DBG_TYPE_LIST;

 typedef struct _SSI_DMA_CONFIG_ {
    VOID *pHalGdmaOp;
    VOID *pTxHalGdmaAdapter;
    VOID *pRxHalGdmaAdapter;
    u8    RxDmaBurstSize;
    u8    TxDmaBurstSize;
    u8    RxDmaEnable;
    u8    TxDmaEnable;
    IRQ_HANDLE RxGdmaIrqHandle;
    IRQ_HANDLE TxGdmaIrqHandle;
}SSI_DMA_CONFIG, *PSSI_DMA_CONFIG;

/**
 * DesignWare SSI Configurations
 */
typedef struct _HAL_SSI_ADAPTOR_ {
    SSI_DMA_CONFIG DmaConfig;
    IRQ_HANDLE IrqHandle;
    //
    VOID (*RxCompCallback)(VOID *Para);
    VOID *RxCompCbPara;
    VOID *RxData;
    VOID (*TxCompCallback)(VOID *Para);
    VOID *TxCompCbPara;
    VOID *TxData;
    u32  DmaRxDataLevel;
    u32  DmaTxDataLevel;
    u32  InterruptPriority;
    u32  RxLength;
    u32  RxLengthRemainder;
    u32  RxThresholdLevel;
    u32  TxLength;
    u32  TxThresholdLevel;
    u32  SlaveSelectEnable;
    //
    u16  ClockDivider;
    u16  DataFrameNumber;
    //
    u8   ControlFrameSize;
    u8   DataFrameFormat;
    u8   DataFrameSize;
    u8   DmaControl;
    u8   Index;
    u8   InterruptMask;
    u8   MicrowireDirection;
    u8   MicrowireHandshaking;
    u8   MicrowireTransferMode;
    u8   PinmuxSelect;
    u8   Role;
    u8   SclkPhase;
    u8   SclkPolarity;
    u8   SlaveOutputEnable;
    u8   TransferMode;
    u8   TransferMechanism;

    // Extend
    u32 Reserved1;
    u8 DefaultRxThresholdLevel;
}HAL_SSI_ADAPTOR, *PHAL_SSI_ADAPTOR;

typedef struct _HAL_SSI_OP_{
    HAL_Status (*HalSsiPinmuxEnable)(VOID *Adaptor);
    HAL_Status (*HalSsiPinmuxDisable)(VOID *Adaptor);
    HAL_Status (*HalSsiEnable)(VOID *Adaptor);
    HAL_Status (*HalSsiDisable)(VOID *Adaptor);
    HAL_Status (*HalSsiInit)(VOID *Adaptor);
    HAL_Status (*HalSsiSetSclkPolarity)(VOID *Adaptor);
    HAL_Status (*HalSsiSetSclkPhase)(VOID *Adaptor);
    HAL_Status (*HalSsiWrite)(VOID *Adaptor, u32 value);
    HAL_Status (*HalSsiLoadSetting)(VOID *Adaptor, VOID *Setting);
    HAL_Status (*HalSsiSetInterruptMask)(VOID *Adaptor);
    HAL_Status (*HalSsiSetDeviceRole)(VOID *Adaptor, u32 Role);
    HAL_Status (*HalSsiInterruptEnable)(VOID *Adaptor);
    HAL_Status (*HalSsiInterruptDisable)(VOID *Adaptor);
    HAL_Status (*HalSsiReadInterrupt)(VOID *Adaptor, VOID *RxData, u32 Length);
    HAL_Status (*HalSsiSetRxFifoThresholdLevel)(VOID *Adaptor);
    HAL_Status (*HalSsiSetTxFifoThresholdLevel)(VOID *Adaptor);
    HAL_Status (*HalSsiWriteInterrupt)(VOID *Adaptor, u8 *TxData, u32 Length);
    HAL_Status (*HalSsiSetSlaveEnableRegister)(VOID *Adaptor, u32 SlaveIndex);
    u32  (*HalSsiBusy)(VOID *Adaptor);
    u32  (*HalSsiReadable)(VOID *Adaptor);
    u32  (*HalSsiWriteable)(VOID *Adaptor);
    u32  (*HalSsiGetInterruptMask)(VOID *Adaptor);
    u32  (*HalSsiGetRxFifoLevel)(VOID *Adaptor);
    u32  (*HalSsiGetTxFifoLevel)(VOID *Adaptor);
    u32  (*HalSsiGetStatus)(VOID *Adaptor);
    u32  (*HalSsiGetInterruptStatus)(VOID *Adaptor);
    u32  (*HalSsiRead)(VOID *Adaptor);
    u32  (*HalSsiGetRawInterruptStatus)(VOID *Adaptor);
    u32  (*HalSsiGetSlaveEnableRegister)(VOID *Adaptor);
}HAL_SSI_OP, *PHAL_SSI_OP;

typedef struct _DW_SSI_DEFAULT_SETTING_ {
    VOID (*RxCompCallback)(VOID *Para);
    VOID *RxCompCbPara;
    VOID *RxData;
    VOID (*TxCompCallback)(VOID *Para);
    VOID *TxCompCbPara;
    VOID *TxData;
    u32  DmaRxDataLevel;
    u32  DmaTxDataLevel;
    u32  InterruptPriority;
    u32  RxLength;
    u32  RxLengthRemainder;
    u32  RxThresholdLevel;
    u32  TxLength;
    u32  TxThresholdLevel;
    u32  SlaveSelectEnable;
    //
    u16  ClockDivider;
    u16  DataFrameNumber;
    //
    u8   ControlFrameSize;
    u8   DataFrameFormat;
    u8   DataFrameSize;
    u8   DmaControl;
    //u8   Index;
    u8   InterruptMask;
    u8   MicrowireDirection;
    u8   MicrowireHandshaking;
    u8   MicrowireTransferMode;
    //u8   PinmuxSelect;
    //u8   Role;
    u8   SclkPhase;
    u8   SclkPolarity;
    u8   SlaveOutputEnable;
    u8   TransferMode;
    u8   TransferMechanism;
} DW_SSI_DEFAULT_SETTING, *PDW_SSI_DEFAULT_SETTING;


struct spi_s {
    HAL_SSI_ADAPTOR spi_adp;
    HAL_SSI_OP      spi_op;
    u32 irq_handler;
    u32 irq_id;
    u32 dma_en;
    u32 state;
    u8 sclk;
#ifdef CONFIG_GDMA_EN    
    HAL_GDMA_ADAPTER spi_gdma_adp_tx;
    HAL_GDMA_ADAPTER spi_gdma_adp_rx;
#endif    
};

VOID HalSsiOpInit(VOID *Adaptor);
static __inline__ VOID HalSsiSetSclk(
    IN PHAL_SSI_ADAPTOR pHalSsiAdapter,
    IN u32 ClkRate)
{
    HalSsiSetSclkRtl8195a((VOID*)pHalSsiAdapter, ClkRate);
}

HAL_Status HalSsiInit(VOID * Data);
HAL_Status HalSsiDeInit(VOID * Data);
HAL_Status HalSsiEnable(VOID * Data);
HAL_Status HalSsiDisable(VOID * Data);


#ifdef CONFIG_GDMA_EN    
HAL_Status HalSsiTxGdmaInit(PHAL_SSI_OP pHalSsiOp, PHAL_SSI_ADAPTOR pHalSsiAdapter);
VOID HalSsiTxGdmaDeInit(PHAL_SSI_ADAPTOR pHalSsiAdapter);
HAL_Status HalSsiRxGdmaInit(PHAL_SSI_OP pHalSsiOp, PHAL_SSI_ADAPTOR pHalSsiAdapter);
VOID HalSsiRxGdmaDeInit(PHAL_SSI_ADAPTOR pHalSsiAdapter);

static __inline__ VOID
HalSsiDmaInit(
    IN PHAL_SSI_ADAPTOR pHalSsiAdapter
)
{
    HalSsiDmaInitRtl8195a((void *)pHalSsiAdapter);
}

static __inline__ HAL_Status HalSsiDmaSend(VOID *Adapter, u8 *pTxData, u32 Length)
{
    return (HalSsiDmaSendRtl8195a(Adapter, pTxData, Length));
}

static __inline__ HAL_Status HalSsiDmaRecv(VOID *Adapter, u8  *pRxData, u32 Length)
{
    return (HalSsiDmaRecvRtl8195a(Adapter, pRxData, Length));
}
    

#endif  // end of "#ifdef CONFIG_GDMA_EN"

#endif

