/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h"
#include "rtl8195a_uart.h"
#include "hal_uart.h"
#include "hal_gdma.h"

extern u32 _UartIrqHandle(VOID *Data);

HAL_Status 
HalRuartInitRtl8195a_Patch(
        IN VOID *Data  ///< RUART Adapter
        );

VOID
HalRuartOpInit(
        IN VOID *Data
)
{
    PHAL_RUART_OP pHalRuartOp = (PHAL_RUART_OP) Data;

    pHalRuartOp->HalRuartAdapterLoadDef     = HalRuartAdapterLoadDefRtl8195a;
    pHalRuartOp->HalRuartTxGdmaLoadDef      = HalRuartTxGdmaLoadDefRtl8195a;
    pHalRuartOp->HalRuartRxGdmaLoadDef      = HalRuartRxGdmaLoadDefRtl8195a;
    pHalRuartOp->HalRuartResetRxFifo        = HalRuartResetRxFifoRtl8195a;
//    pHalRuartOp->HalRuartInit               = HalRuartInitRtl8195a;         // Hardware Init
    pHalRuartOp->HalRuartInit               = HalRuartInitRtl8195a_Patch;         // Hardware Init ROM code patch 
    pHalRuartOp->HalRuartDeInit             = HalRuartDeInitRtl8195a;         // Hardware Init
    pHalRuartOp->HalRuartPutC               = HalRuartPutCRtl8195a;         // Send a byte
    pHalRuartOp->HalRuartSend               = HalRuartSendRtl8195a;         // Polling mode Tx
    pHalRuartOp->HalRuartIntSend            = HalRuartIntSendRtl8195a;      // Interrupt mode Tx
    pHalRuartOp->HalRuartDmaSend            = HalRuartDmaSendRtl8195a;      // DMA mode Tx
    pHalRuartOp->HalRuartStopSend           = HalRuartStopSendRtl8195a;     // Stop non-blocking TX
    pHalRuartOp->HalRuartGetC               = HalRuartGetCRtl8195a;           // get a byte
    pHalRuartOp->HalRuartRecv               = HalRuartRecvRtl8195a;         // Polling mode Rx
    pHalRuartOp->HalRuartIntRecv            = HalRuartIntRecvRtl8195a;      // Interrupt mode Rx
    pHalRuartOp->HalRuartDmaRecv            = HalRuartDmaRecvRtl8195a;      // DMA mode Rx
    pHalRuartOp->HalRuartStopRecv           = HalRuartStopRecvRtl8195a;     // Stop non-blocking Rx
    pHalRuartOp->HalRuartGetIMR             = HalRuartGetIMRRtl8195a;
    pHalRuartOp->HalRuartSetIMR             = HalRuartSetIMRRtl8195a;
    pHalRuartOp->HalRuartGetDebugValue      = HalRuartGetDebugValueRtl8195a;
    pHalRuartOp->HalRuartDmaInit            = HalRuartDmaInitRtl8195a;
    pHalRuartOp->HalRuartRTSCtrl            = HalRuartRTSCtrlRtl8195a;
    pHalRuartOp->HalRuartRegIrq             = HalRuartRegIrqRtl8195a;
    pHalRuartOp->HalRuartIntEnable          = HalRuartIntEnableRtl8195a;
    pHalRuartOp->HalRuartIntDisable         = HalRuartIntDisableRtl8195a;
}



/**
 * Load UART HAL default setting
 *
 * Call this function to load the default setting for UART HAL adapter
 *
 *
 */
VOID
HalRuartAdapterInit(
    PRUART_ADAPTER pRuartAdapter,
    u8 UartIdx
)
{
    PHAL_RUART_OP pHalRuartOp;
    PHAL_RUART_ADAPTER pHalRuartAdapter;
    
    if (NULL == pRuartAdapter) {
        return;
    }
    
    pHalRuartOp = pRuartAdapter->pHalRuartOp;
    pHalRuartAdapter = pRuartAdapter->pHalRuartAdapter;

    if ((NULL == pHalRuartOp) || (NULL == pHalRuartAdapter)) {
        return;
    }

    // Load default setting
    if (pHalRuartOp->HalRuartAdapterLoadDef != NULL) {
        pHalRuartOp->HalRuartAdapterLoadDef (pHalRuartAdapter, UartIdx);
    }
    else {
        // Initial your UART HAL adapter here
    }

    // Start to modify the defualt setting
    pHalRuartAdapter->PinmuxSelect = RUART0_MUX_TO_GPIOC;
    pHalRuartAdapter->BaudRate = 38400;

//    pHalRuartAdapter->IrqHandle.IrqFun = (IRQ_FUN)_UartIrqHandle;
//    pHalRuartAdapter->IrqHandle.Data = (void *)pHalRuartAdapter;
//    pHalRuartAdapter->IrqHandle.Priority = 0x20;

    // Register IRQ
    InterruptRegister(&pHalRuartAdapter->IrqHandle);
    
}

/**
 * Load UART HAL GDMA default setting
 *
 * Call this function to load the default setting for UART GDMA
 *
 *
 */
VOID
HalRuartTxGdmaInit(
    PRUART_ADAPTER pRuartAdapter
)
{
    PHAL_RUART_OP pHalRuartOp;
    PHAL_RUART_ADAPTER pHalRuartAdapter;
    PUART_DMA_CONFIG pUartGdmaConfig;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;
    
    if (NULL == pRuartAdapter) {
        return;
    }
    
    pHalRuartOp = pRuartAdapter->pHalRuartOp;
    pHalRuartAdapter = pRuartAdapter->pHalRuartAdapter;
    pUartGdmaConfig = pRuartAdapter->pHalRuartDmaCfg;

    if ((NULL == pHalRuartOp) || (NULL == pHalRuartAdapter) || (NULL == pUartGdmaConfig)) {
        return;
    }

    // Load default setting
    if (pHalRuartOp->HalRuartTxGdmaLoadDef != NULL) {
        HalGdmaOpInit((VOID*) (pUartGdmaConfig->pHalGdmaOp));
        pHalRuartOp->HalRuartTxGdmaLoadDef (pHalRuartAdapter, pUartGdmaConfig);
    }
    else {
        // Initial your GDMA setting here
    }

    // Start to patch the default setting
    pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pUartGdmaConfig->pTxHalGdmaAdapter;
    pHalGdmaAdapter->GdmaIndex   = 0;  // GDMA0 
    pHalGdmaAdapter->ChNum       = 4;   // GDMA Channel
    pHalGdmaAdapter->ChEn        = GdmaCh4;     // GDMA Channel Enable

//    pUartGdmaConfig->TxGdmaIrqHandle.Data = pHalRuartAdapter;
    pUartGdmaConfig->TxGdmaIrqHandle.IrqNum = GDMA0_CHANNEL4_IRQ;
//    pUartGdmaConfig->TxGdmaIrqHandle.IrqFun = (IRQ_FUN)_UartTxDmaIrqHandle
    pUartGdmaConfig->TxGdmaIrqHandle.Priority = 0x20;

    pHalRuartOp->HalRuartDmaInit (pHalRuartAdapter);
    InterruptRegister(&pUartGdmaConfig->TxGdmaIrqHandle);
    InterruptEn(&pUartGdmaConfig->TxGdmaIrqHandle);

}

/**
 * Load UART HAL GDMA default setting
 *
 * Call this function to load the default setting for UART GDMA
 *
 *
 */
VOID
HalRuartRxGdmaInit(
    PRUART_ADAPTER pRuartAdapter
)
{
    PHAL_RUART_OP pHalRuartOp;
    PHAL_RUART_ADAPTER pHalRuartAdapter;
    PUART_DMA_CONFIG pUartGdmaConfig;
    PHAL_GDMA_ADAPTER pHalGdmaAdapter;
    
    if (NULL == pRuartAdapter) {
        return;
    }
    
    pHalRuartOp = pRuartAdapter->pHalRuartOp;
    pHalRuartAdapter = pRuartAdapter->pHalRuartAdapter;
    pUartGdmaConfig = pRuartAdapter->pHalRuartDmaCfg;

    if ((NULL == pHalRuartOp) || (NULL == pHalRuartAdapter) || (NULL == pUartGdmaConfig)) {
        return;
    }

    // Load default setting
    if (pHalRuartOp->HalRuartRxGdmaLoadDef != NULL) {
        HalGdmaOpInit((VOID*) (pUartGdmaConfig->pHalGdmaOp));
        pHalRuartOp->HalRuartRxGdmaLoadDef (pHalRuartAdapter, pUartGdmaConfig);
    }
    else {
        // Initial your GDMA setting here
    }

    // Start to patch the default setting
    pHalGdmaAdapter = (PHAL_GDMA_ADAPTER)pUartGdmaConfig->pRxHalGdmaAdapter;
    pHalGdmaAdapter->GdmaIndex   = 0;  // GDMA1
    pHalGdmaAdapter->ChNum       = 5;   // GDMA Channel
    pHalGdmaAdapter->ChEn        = GdmaCh5;     // GDMA Channel Enable

//    pUartGdmaConfig->RxGdmaIrqHandle.Data = pHalRuartAdapter;
    pUartGdmaConfig->RxGdmaIrqHandle.IrqNum = GDMA0_CHANNEL5_IRQ;
//    pUartGdmaConfig->RxGdmaIrqHandle.IrqFun = (IRQ_FUN)_UartTxDmaIrqHandle;
    pUartGdmaConfig->RxGdmaIrqHandle.Priority = 0x20;

    pHalRuartOp->HalRuartDmaInit (pHalRuartAdapter);
    InterruptRegister(&pUartGdmaConfig->RxGdmaIrqHandle);
    InterruptEn(&pUartGdmaConfig->RxGdmaIrqHandle);
}

/**
 * Hook a RX indication callback
 *
 * To hook a callback function which will be called when a got a RX byte
 *
 *
 */
VOID
HalRuartRxIndHook(
    PRUART_ADAPTER pRuartAdapter,
    VOID *pCallback,
    VOID *pPara
)
{
    PHAL_RUART_ADAPTER pHalRuartAdapter = pRuartAdapter->pHalRuartAdapter;

    pHalRuartAdapter->RxDRCallback = (void (*)(void*))pCallback;
    pHalRuartAdapter->RxDRCbPara = pPara;

    // enable RX data ready interrupt
    pHalRuartAdapter->Interrupts |= RUART_IER_ERBI | RUART_IER_ELSI;
    pRuartAdapter->pHalRuartOp->HalRuartSetIMR(pHalRuartAdapter);    
}

extern u32 BAUDRATE[];
#if 0
const u32 OVSR_PATCH[] = {
    11, 13, 19, 10,
    20, 10, 20, 17, 
    11, 10, 17, 10, 
    10, 13, 18, 13, 
    20, 19, 13, 15, 
    13, 13, 15, 13, 
    12, 11, 11, 20, 
    13 
};

const u32 DIV_PATCH[] = {
    6105, 643, 293, 434, 
     142, 217,  71,  62, 
      65,  62,  31,  34, 
      17,  12,   5,   6, 
       3,   3,   4,   3, 
       3,   3,   2,   2, 
       2,   2,   2,   1, 
       1
};

const u32 OVSR_ADJ_PATCH[] = {
    0x24A, 0x555, 0x3BB, 0x000,
    0x24A, 0x000, 0x24A, 0x555,
    0x008, 0x555, 0x555, 0x5AD,
    0x5AD, 0x7EF, 0x020, 0x7EF,
    0x020, 0x444, 0x7EF, 0x080,
    0x7EF, 0x444, 0x080, 0x7EF,
    0x6FB, 0x122, 0x010, 0x5F7,
    0x7EF
};
#else
const u32 OVSR_PATCH[] = {
    10, 15, 10, 13,
    15, 11, 12, 11,
    12, 11, 11, 18,
    11, 18, 11, 10,
    11, 11, 18, 11,
    10, 13, 14, 13,
    12, 11, 10, 10,
    13
};

const u32 DIV_PATCH[] = {
    6516, 543, 543, 311,
     181, 188, 114,  94,
      57,  57,  47,  19,  
      15,   9,   8,	  8,   
       5,   5,   3,   4,
       4,   3,   2,   2,
       2,   2,   2,   2,
       1     
};

const u32 OVSR_ADJ_PATCH[] = {
    0x555, 0x3BB, 0x555, 0x5DD,
    0x3BB, 0x252, 0x555, 0x252,
    0x555, 0x222, 0x252, 0x3BB,
    0x7EF, 0x444, 0x008, 0x222,
    0x7EF, 0x252, 0x444, 0x008,
    0x222, 0x000, 0x5F7, 0x76D,
    0x5AD, 0x010, 0x5FF, 0x222,
    0x76D
};
#endif
static s32
FindElementIndex(
        u32 Element,  ///< RUART Baudrate
        u32* Array    ///< Pre-defined Baudrate Array
        )
{
    /* DBG_ENTRANCE; */
    u32 BaudRateNumber = 29;
    s32 Result = -1;
    u32 Index = 0;

    for (Index = 0; Index < BaudRateNumber && Result == -1; Index++) {
        if (Element == Array[Index])
            Result = Index;
    }
    return Result;  //TODO: Error handling
}

HAL_Status 
HalRuartInitRtl8195a_Patch(
        IN VOID *Data  ///< RUART Adapter
        )
{
    /* DBG_ENTRANCE; */
    u32 RegValue;
    u32 Divisor;
    u32 Dll;
    u32 Dlm;
    u8 UartIndex;
    s32 ElementIndex;
    RUART_SPEED_SETTING RuartSpeedSetting;
    u8 PinmuxSelect;

    PHAL_RUART_ADAPTER pHalRuartAdapter = (PHAL_RUART_ADAPTER) Data;

    UartIndex = pHalRuartAdapter->UartIndex;
    PinmuxSelect = pHalRuartAdapter->PinmuxSelect;

    if (UartIndex > 2) {
        DBG_UART_ERR(ANSI_COLOR_MAGENTA"HalRuartInitRtl8195a: Invalid UART Index\n"ANSI_COLOR_RESET);
        return HAL_ERR_PARA;
    }

    DBG_UART_INFO("%s==>\n", __FUNCTION__);
    DBG_UART_INFO("HalRuartInitRtl8195a: [UART %d] PinSel=%d\n", UartIndex, PinmuxSelect);
    if(( PinmuxSelect == RUART0_MUX_TO_GPIOE ) && ((UartIndex == 0) || (UartIndex == 1))) {
        // confict with J-Teg pin, so we disable the J-Teg
        DBG_UART_WARN(ANSI_COLOR_MAGENTA"Disable JTAG function due to pin conflict\n"ANSI_COLOR_RESET);
        JTAG_PIN_FCTRL(OFF);
    }

    // switch Pin from EEPROM to UART0
    if(( PinmuxSelect == RUART0_MUX_TO_GPIOC ) && (UartIndex == 0)) {
        RegValue = HAL_READ32(SYSTEM_CTRL_BASE, 0xa4);
        if (RegValue & 0x10) {
            DBG_UART_WARN("Disable EEPROM Pin for UART 0\n");
            HAL_WRITE32(SYSTEM_CTRL_BASE, 0xa4, (RegValue & (~0x10)));
        }
    }

    switch (UartIndex) {
        case 0:
            /* UART 0 */
            ACTCK_UART0_CCTRL(ON);
            SLPCK_UART0_CCTRL(ON);
            PinCtrl(UART0, PinmuxSelect, ON);
            UART0_FCTRL(ON);
            UART0_BD_FCTRL(ON);
            break;

        case 1:
            /* UART 1 */
            ACTCK_UART1_CCTRL(ON);
            SLPCK_UART1_CCTRL(ON);
            PinCtrl(UART1, PinmuxSelect, ON);
            UART1_FCTRL(ON);
            UART1_BD_FCTRL(ON);
            break;

        case 2:
            /* UART 1 */
            ACTCK_UART2_CCTRL(ON);
            SLPCK_UART2_CCTRL(ON);
            PinCtrl(UART2, PinmuxSelect, ON);
            UART2_FCTRL(ON);
            UART2_BD_FCTRL(ON);
            break;

        default:
            DBG_UART_ERR("Invalid UART Index(%d)\n", UartIndex);
            return HAL_ERR_PARA;
    }

    /* Reset RX FIFO */
    HalRuartResetRxFifoRtl8195a(Data);
    DBG_UART_INFO(ANSI_COLOR_CYAN"HAL UART Init[UART %d]\n"ANSI_COLOR_RESET, UartIndex);

    /* Disable all interrupts */
    HAL_RUART_WRITE32(UartIndex, RUART_INTERRUPT_EN_REG_OFF, 0x00);

    /* Set DLAB bit to 1 to access DLL/DLM */
    RegValue = HAL_RUART_READ32(UartIndex, RUART_LINE_CTL_REG_OFF);
    RegValue |= RUART_LINE_CTL_REG_DLAB_ENABLE;
    HAL_RUART_WRITE32(UartIndex, RUART_LINE_CTL_REG_OFF, RegValue);
    DBG_UART_INFO("[R] RUART_LINE_CTL_REG_OFF(0x0C) = %x\n", HAL_RUART_READ32(UartIndex, RUART_LINE_CTL_REG_OFF));

    /* Set Baudrate Division */
#if 1
    ElementIndex = FindElementIndex(pHalRuartAdapter->BaudRate, BAUDRATE);
    if (ElementIndex < 0) {
        ElementIndex = 5;
        DBG_UART_ERR("Invalid BaudRate(%d), Force it as default(%d)\n", pHalRuartAdapter->BaudRate, BAUDRATE[ElementIndex]);
    }

    RuartSpeedSetting.BaudRate = BAUDRATE[ElementIndex];
    RuartSpeedSetting.Ovsr     =     OVSR_PATCH[ElementIndex];
    RuartSpeedSetting.Div      =      DIV_PATCH[ElementIndex];
    RuartSpeedSetting.Ovsr_adj = OVSR_ADJ_PATCH[ElementIndex];
#else
    RuartSpeedSetting.BaudRate = 38400;
    RuartSpeedSetting.Ovsr     = 10;
    RuartSpeedSetting.Div      = 217;
    RuartSpeedSetting.Ovsr_adj = 0x0;
#endif

    DBG_UART_INFO("Baud %d, Ovsr %d, Div %d, OvsrAdj 0x%X\n",
            RuartSpeedSetting.BaudRate,
            RuartSpeedSetting.Ovsr,
            RuartSpeedSetting.Div,
            RuartSpeedSetting.Ovsr_adj
            );
    /* Divisor = (SYSTEM_CLK / ((ovsr + 5 + ovsr_adj/11) * (UartAdapter.BaudRate))); */

    /* Set Divisor */
    Divisor = RuartSpeedSetting.Div;

    Dll = Divisor & 0xFF;
    Dlm = (Divisor & 0xFF00) >> 8;

    DBG_UART_INFO("Calculated Dll, Dlm = %02x, %02x\n", Dll, Dlm);
    DBG_UART_INFO("---- Before setting baud rate ----\n");
    DBG_UART_INFO("  [R] RUART_DLL_OFF(0x00) = %x\n", HAL_RUART_READ32(UartIndex, RUART_DLL_OFF));
    DBG_UART_INFO("  [R] RUART_DLM_OFF(0x04) = %x\n", HAL_RUART_READ32(UartIndex, RUART_DLM_OFF));

    HAL_RUART_WRITE32(UartIndex, RUART_DLL_OFF, Dll);
    HAL_RUART_WRITE32(UartIndex, RUART_DLM_OFF, Dlm);

    DBG_UART_INFO("---- After setting baud rate ----\n");
    DBG_UART_INFO("  [R] RUART_DLL_OFF(0x00) = %x\n", HAL_RUART_READ32(UartIndex, RUART_DLL_OFF));
    DBG_UART_INFO("  [R] RUART_DLM_OFF(0x04) = %x\n", HAL_RUART_READ32(UartIndex, RUART_DLM_OFF));

    DBG_UART_INFO(ANSI_COLOR_CYAN"---- Befor OVSR & OVSR_ADJ ----\n"ANSI_COLOR_RESET);
    RegValue = HAL_RUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
    DBG_UART_INFO("UART%d SPR(0x1C) = %X\n", UartIndex, RegValue);
    RegValue = HAL_RUART_READ32(UartIndex, RUART_STS_REG_OFF);
    DBG_UART_INFO("UART%d SIS(0x20) = %X\n", UartIndex, RegValue);

    /**
      * Clean Rx break signal interrupt status at initial stage.
      */
    RegValue = HAL_RUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
    RegValue |= RUART_SP_REG_RXBREAK_INT_STATUS;
    HAL_RUART_WRITE32(UartIndex, RUART_SCRATCH_PAD_REG_OFF, RegValue);

    /* Set OVSR(xfactor) */
    RegValue = HAL_RUART_READ32(UartIndex, RUART_STS_REG_OFF);
    RegValue &= ~(RUART_STS_REG_XFACTOR);
    RegValue |= (((RuartSpeedSetting.Ovsr - 5) << 4) & RUART_STS_REG_XFACTOR);
    HAL_RUART_WRITE32(UartIndex, RUART_STS_REG_OFF, RegValue);

    /* Set OVSR_ADJ[10:0] (xfactor_adj[26:16]) */
    RegValue = HAL_RUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
    RegValue &= ~(RUART_SP_REG_XFACTOR_ADJ);
    RegValue |= ((RuartSpeedSetting.Ovsr_adj << 16) & RUART_SP_REG_XFACTOR_ADJ);
    HAL_RUART_WRITE32(UartIndex, RUART_SCRATCH_PAD_REG_OFF, RegValue);

    DBG_UART_INFO(ANSI_COLOR_CYAN"---- After OVSR & OVSR_ADJ ----\n"ANSI_COLOR_RESET);
    RegValue = HAL_RUART_READ32(UartIndex, RUART_SCRATCH_PAD_REG_OFF);
    DBG_UART_INFO("UART%d SPR(0x1C) = %X\n", UartIndex, RegValue);
    RegValue = HAL_RUART_READ32(UartIndex, RUART_STS_REG_OFF);
    DBG_UART_INFO("UART%d SIS(0x20) = %X\n", UartIndex, RegValue);

    /* clear DLAB bit */
    RegValue = HAL_RUART_READ32(UartIndex, RUART_LINE_CTL_REG_OFF);
    RegValue &= ~(RUART_LINE_CTL_REG_DLAB_ENABLE);
    HAL_RUART_WRITE32(UartIndex, RUART_LINE_CTL_REG_OFF, RegValue);
    DBG_UART_INFO("[R] RUART_LINE_CTL_REG_OFF(0x0C) = %x\n", HAL_RUART_READ32(UartIndex, RUART_LINE_CTL_REG_OFF));
    DBG_UART_INFO("[R] UART%d INT_EN(0x04) = %x\n", UartIndex, pHalRuartAdapter->Interrupts);
    RegValue = ((pHalRuartAdapter->Interrupts) & 0xFF);
    HAL_RUART_WRITE32(UartIndex, RUART_INTERRUPT_EN_REG_OFF, RegValue);
    DBG_UART_INFO("[W] UART%d INT_EN(0x04) = %x\n", UartIndex, RegValue);

    /* Configure FlowControl */
    if (pHalRuartAdapter->FlowControl == AUTOFLOW_ENABLE) {
        RegValue = HAL_RUART_READ32(UartIndex, RUART_MODEM_CTL_REG_OFF);
        RegValue |= RUART_MCL_AUTOFLOW_ENABLE;
        HAL_RUART_WRITE32(UartIndex, RUART_MODEM_CTL_REG_OFF, RegValue);
    }

    /* RUART DMA Initialization */
    HalRuartDmaInitRtl8195a(pHalRuartAdapter);

    DBG_UART_INFO("[R] UART%d LCR(0x%02X): %X\n", UartIndex, RUART_LINE_CTL_REG_OFF, HAL_RUART_READ32(UartIndex, RUART_LINE_CTL_REG_OFF));
    RegValue = HAL_RUART_READ32(UartIndex, RUART_LINE_CTL_REG_OFF);

    /* PARITY CONTROL */
    RegValue &= BIT_CLR_LCR_WLS;
    RegValue |= BIT_LCR_WLS(pHalRuartAdapter->WordLen);

    RegValue &= BIT_INVC_LCR_STB_EN;
    RegValue |= BIT_LCR_STB_EN(pHalRuartAdapter->StopBit);

    RegValue &= BIT_INVC_LCR_PARITY_EN;
    RegValue |= BIT_LCR_PARITY_EN(pHalRuartAdapter->Parity);

    /* PARITY TYPE SELECT */
    RegValue &= BIT_INVC_LCR_PARITY_TYPE;
    RegValue |= BIT_LCR_PARITY_TYPE(pHalRuartAdapter->ParityType);

    /* STICK PARITY CONTROL */
    RegValue &= BIT_INVC_LCR_STICK_PARITY_EN;
    RegValue |= BIT_LCR_STICK_PARITY_EN(pHalRuartAdapter->StickParity);

    HAL_RUART_WRITE32(UartIndex, RUART_LINE_CTL_REG_OFF, RegValue);
    DBG_UART_INFO("[W] UART%d LCR(0x%02X): %X\n", UartIndex, RUART_LINE_CTL_REG_OFF, HAL_RUART_READ32(UartIndex, RUART_LINE_CTL_REG_OFF));

    /* Need to assert RTS during initial stage. */
    if (pHalRuartAdapter->FlowControl == AUTOFLOW_ENABLE) {
        HalRuartRTSCtrlRtl8195a(Data, 1);
    }
    pHalRuartAdapter->State = HAL_UART_STATE_READY;

	return HAL_OK;
}

