/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _HAL_UART_H_
#define _HAL_UART_H_

#include "rtl8195a_uart.h"

/**
 * RUART Configurations
 */
#define UART_WAIT_FOREVER       0xffffffff

typedef struct _UART_DMA_CONFIG_ {
    u8 TxDmaEnable;
    u8 RxDmaEnable;
    u8 TxDmaBurstSize;
    u8 RxDmaBurstSize;
    VOID *pHalGdmaOp;
    VOID *pTxHalGdmaAdapter;
    VOID *pRxHalGdmaAdapter;
    IRQ_HANDLE TxGdmaIrqHandle;
    IRQ_HANDLE RxGdmaIrqHandle;
}UART_DMA_CONFIG, *PUART_DMA_CONFIG;

typedef struct _HAL_RUART_ADAPTER_ {
    u32 BaudRate;
    u32 FlowControl;
    u32 FifoControl;
    u32 Interrupts;
    u32 TxCount;     // how many byte to TX
    u32 RxCount;     // how many bytes to RX
    u8 *pTxBuf;
    u8 *pRxBuf;
    HAL_UART_State State;       // UART state
    u8 Status;      // Transfer Status
    u8 Locked;      // is UART locked for operation
    u8 UartIndex;
    u8 WordLen;     // word length select: 0 -> 7 bits, 1 -> 8 bits
    u8 StopBit;     // word length select: 0 -> 1 stop bit, 1 -> 2 stop bit
    u8 Parity;      // parity check enable
    u8 ParityType;  // parity check type
    u8 StickParity;
    u8 ModemStatus; // the modem status
    u8 DmaEnable;
    u8 TestCaseNumber;
    u8 PinmuxSelect;
    BOOL PullMode;
    IRQ_HANDLE IrqHandle;
    PUART_DMA_CONFIG DmaConfig;
    VOID (*ModemStatusInd)(VOID *pAdapter);    // modem status indication interrupt handler
    VOID (*TxTDCallback)(VOID *pAdapter);      // User Tx Done callback function
    VOID (*RxDRCallback)(VOID *pAdapter);      // User Rx Data ready callback function
    VOID (*TxCompCallback)(VOID *para);    // User Tx complete callback function
    VOID (*RxCompCallback)(VOID *para);    // User Rx complete callback function
    VOID *TxTDCbPara;   // the pointer agrument for TxTDCallback
    VOID *RxDRCbPara;   // the pointer agrument for RxDRCallback
    VOID *TxCompCbPara; // the pointer argument for TxCompCbPara
    VOID *RxCompCbPara; // the pointer argument for RxCompCallback
    VOID (*EnterCritical)(void);
    VOID (*ExitCritical)(void);

    //1 New member only can be added below: members above must be fixed for ROM code
    u32 *pDefaultBaudRateTbl;      // point to the table of pre-defined baud rate
    u8 *pDefaultOvsrRTbl;         // point to the table of OVSR for pre-defined baud rate
    u16 *pDefaultDivTbl;           // point to the table of DIV for pre-defined baud rate
    u8  *pDefOvsrAdjBitTbl_10;     // point to the table of OVSR-Adj bits for 10 bits
    u8  *pDefOvsrAdjBitTbl_9;     // point to the table of OVSR-Adj bits for 9 bits
    u8  *pDefOvsrAdjBitTbl_8;     // point to the table of OVSR-Adj bits for 8 bits
    u16 *pDefOvsrAdjTbl_10;       // point to the table of OVSR-Adj for pre-defined baud rate
    u16 *pDefOvsrAdjTbl_9;       // point to the table of OVSR-Adj for pre-defined baud rate
    u16 *pDefOvsrAdjTbl_8;       // point to the table of OVSR-Adj for pre-defined baud rate
    u32 BaudRateUsing;             // Current using Baud-Rate    

#if CONFIG_CHIP_E_CUT
    u8  TxState;
    u8  RxState;
    u32 TxInitSize;     // how many byte to TX at atart
    u32 RxInitSize;     // how many bytes to RX at start

    VOID (*RuartEnterCritical)(VOID *para);   // enter critical: disable UART interrupt
    VOID (*RuartExitCritical)(VOID *para);    // exit critical: re-enable UART interrupt
    VOID (*TaskYield)(VOID *para);    // User Task Yield: do a context switch while waitting
    VOID *TaskYieldPara;   // the agrument (pointer) for TaskYield
#endif    // #if CONFIG_CHIP_E_CUT
}HAL_RUART_ADAPTER, *PHAL_RUART_ADAPTER;

typedef struct _HAL_RUART_OP_ {
    VOID (*HalRuartAdapterLoadDef)(VOID *pAdp, u8 UartIdx);    // Load UART adapter default setting
    VOID (*HalRuartTxGdmaLoadDef)(VOID *pAdp, VOID *pCfg);     // Load TX GDMA default setting
    VOID (*HalRuartRxGdmaLoadDef)(VOID *pAdp, VOID *pCfg);     // Load RX GDMA default setting
    HAL_Status (*HalRuartResetRxFifo)(VOID *Data);
    HAL_Status (*HalRuartInit)(VOID *Data);
    VOID (*HalRuartDeInit)(VOID *Data);
    HAL_Status (*HalRuartPutC)(VOID *Data, u8 TxData);
    u32  (*HalRuartSend)(VOID *Data, u8 *pTxData, u32 Length, u32 Timeout);
    HAL_Status  (*HalRuartIntSend)(VOID *Data, u8 *pTxData, u32 Length);
    HAL_Status  (*HalRuartDmaSend)(VOID *Data, u8 *pTxData, u32 Length);
    HAL_Status  (*HalRuartStopSend)(VOID *Data);
    HAL_Status (*HalRuartGetC)(VOID *Data, u8 *pRxByte);
    u32  (*HalRuartRecv)(VOID *Data, u8  *pRxData, u32 Length, u32 Timeout);
    HAL_Status  (*HalRuartIntRecv)(VOID *Data, u8  *pRxData, u32 Length);
    HAL_Status  (*HalRuartDmaRecv)(VOID *Data, u8  *pRxData, u32 Length);
    HAL_Status  (*HalRuartStopRecv)(VOID *Data);
    u8   (*HalRuartGetIMR)(VOID *Data);
    VOID (*HalRuartSetIMR)(VOID *Data);
    u32  (*HalRuartGetDebugValue)(VOID *Data, u32 DbgSel);
    VOID (*HalRuartDmaInit)(VOID *Data);
    VOID (*HalRuartRTSCtrl)(VOID *Data, BOOLEAN RtsCtrl);
    VOID (*HalRuartRegIrq)(VOID *Data);
    VOID (*HalRuartIntEnable)(VOID *Data);
    VOID (*HalRuartIntDisable)(VOID *Data);
}HAL_RUART_OP, *PHAL_RUART_OP;

typedef struct _RUART_DATA_ {
    PHAL_RUART_ADAPTER pHalRuartAdapter;
    BOOL PullMode;
    u8   BinaryData;
    u8   SendBuffer;
    u8   RecvBuffer;
}RUART_DATA, *PRUART_DATA;

typedef struct _RUART_ADAPTER_ {
    PHAL_RUART_OP      pHalRuartOp;
    PHAL_RUART_ADAPTER pHalRuartAdapter;
    PUART_DMA_CONFIG   pHalRuartDmaCfg;
}RUART_ADAPTER, *PRUART_ADAPTER;

extern VOID
HalRuartOpInit(
        IN VOID *Data
);

extern HAL_Status
HalRuartTxGdmaInit(
    PHAL_RUART_OP pHalRuartOp,
    PHAL_RUART_ADAPTER pHalRuartAdapter,
    PUART_DMA_CONFIG pUartGdmaConfig
);

extern VOID
HalRuartTxGdmaDeInit(
    PUART_DMA_CONFIG pUartGdmaConfig
);

extern HAL_Status
HalRuartRxGdmaInit(
    PHAL_RUART_OP pHalRuartOp,
    PHAL_RUART_ADAPTER pHalRuartAdapter,
    PUART_DMA_CONFIG pUartGdmaConfig
);

extern VOID
HalRuartRxGdmaDeInit(
    PUART_DMA_CONFIG pUartGdmaConfig
);

extern HAL_Status
HalRuartResetTxFifo(
    VOID *Data
);

extern HAL_Status 
HalRuartSetBaudRate(
        IN VOID *Data
);

extern HAL_Status 
HalRuartInit(
    IN VOID *Data
);

extern VOID
HalRuartDeInit(
    IN VOID *Data
);

extern HAL_Status 
HalRuartDisable(
    IN VOID *Data
);

extern HAL_Status 
HalRuartEnable(
    IN VOID *Data
);

HAL_Status 
HalRuartFlowCtrl(
    IN VOID *Data
);

extern const HAL_RUART_OP _HalRuartOp;
extern HAL_Status RuartLock (PHAL_RUART_ADAPTER pHalRuartAdapter);
extern VOID RuartUnLock (PHAL_RUART_ADAPTER pHalRuartAdapter);

#endif

