/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2014, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */

#include "objects.h"
//#include "mbed_assert.h"
#include "serial_api.h"

#if CONFIG_UART_EN

//#include "cmsis.h"
#include "pinmap.h"
#include <string.h>

static const PinMap PinMap_UART_TX[] = {
    {PC_3,  RTL_PIN_PERI(UART0, 0, S0), RTL_PIN_FUNC(UART0, S0)},
    {PE_0,  RTL_PIN_PERI(UART0, 0, S1), RTL_PIN_FUNC(UART0, S1)},
    {PA_7,  RTL_PIN_PERI(UART0, 0, S2), RTL_PIN_FUNC(UART0, S2)},
    {PD_3,  RTL_PIN_PERI(UART1, 1, S0), RTL_PIN_FUNC(UART1, S0)},
    {PE_4,  RTL_PIN_PERI(UART1, 1, S1), RTL_PIN_FUNC(UART1, S1)},
    {PB_5,  RTL_PIN_PERI(UART1, 1, S2), RTL_PIN_FUNC(UART1, S2)},
    {PA_4,  RTL_PIN_PERI(UART2, 2, S0), RTL_PIN_FUNC(UART2, S0)},
    {PC_9,  RTL_PIN_PERI(UART2, 2, S1), RTL_PIN_FUNC(UART2, S1)},
    {PD_7,  RTL_PIN_PERI(UART2, 2, S2), RTL_PIN_FUNC(UART2, S2)},
    {NC,    NC,     0}
};

static const PinMap PinMap_UART_RX[] = {
    {PC_0,  RTL_PIN_PERI(UART0, 0, S0), RTL_PIN_FUNC(UART0, S0)},
    {PE_3,  RTL_PIN_PERI(UART0, 0, S1), RTL_PIN_FUNC(UART0, S1)},
    {PA_6,  RTL_PIN_PERI(UART0, 0, S2), RTL_PIN_FUNC(UART0, S2)},
    {PD_0,  RTL_PIN_PERI(UART1, 1, S0), RTL_PIN_FUNC(UART1, S0)},
    {PE_7,  RTL_PIN_PERI(UART1, 1, S1), RTL_PIN_FUNC(UART1, S1)},
    {PB_4,  RTL_PIN_PERI(UART1, 1, S2), RTL_PIN_FUNC(UART1, S2)},
    {PA_0,  RTL_PIN_PERI(UART2, 2, S0), RTL_PIN_FUNC(UART2, S0)},
    {PC_6,  RTL_PIN_PERI(UART2, 2, S1), RTL_PIN_FUNC(UART2, S1)},
    {PD_4,  RTL_PIN_PERI(UART2, 2, S2), RTL_PIN_FUNC(UART2, S2)},
    {NC,    NC,     0}
};

#define UART_NUM (3)
#define SERIAL_TX_IRQ_EN        0x01
#define SERIAL_RX_IRQ_EN        0x02

static uint32_t serial_irq_ids[UART_NUM] = {0, 0, 0};

static uart_irq_handler irq_handler[UART_NUM];
static uint32_t serial_irq_en=0;

#ifdef CONFIG_MBED_ENABLED
int stdio_uart_inited = 0;
serial_t stdio_uart;
#endif

void serial_init(serial_t *obj, PinName tx, PinName rx) 
{
    uint32_t uart_tx, uart_rx;
    uint32_t uart_sel;
    uint8_t uart_idx;
    PHAL_RUART_OP      pHalRuartOp;
    PHAL_RUART_ADAPTER pHalRuartAdapter;
    
    // Determine the UART to use (UART0, UART1, or UART3)
    uart_tx = pinmap_peripheral(tx, PinMap_UART_TX);
    uart_rx = pinmap_peripheral(rx, PinMap_UART_RX);

    uart_sel = pinmap_merge(uart_tx, uart_rx);
    uart_idx = RTL_GET_PERI_IDX(uart_sel);
    if (unlikely(uart_idx == (uint8_t)NC)) {
        DBG_UART_ERR("%s: Cannot find matched UART\n", __FUNCTION__);
        return;
    }

    pHalRuartOp = &(obj->hal_uart_op);
    pHalRuartAdapter = &(obj->hal_uart_adp);

    if ((NULL == pHalRuartOp) || (NULL == pHalRuartAdapter)) {
        DBG_UART_ERR("%s: Allocate Adapter Failed\n", __FUNCTION__);
        return;
    }
    
    HalRuartOpInit((VOID*)pHalRuartOp);

    pHalRuartOp->HalRuartAdapterLoadDef(pHalRuartAdapter, uart_idx);
    pHalRuartAdapter->PinmuxSelect = RTL_GET_PERI_SEL(uart_sel);
    pHalRuartAdapter->BaudRate = 9600;
    
    // Configure the UART pins
    // TODO:
//    pinmap_pinout(tx, PinMap_UART_TX);
//    pinmap_pinout(rx, PinMap_UART_RX);
//    pin_mode(tx, PullUp);
//    pin_mode(rx, PullUp);
    
    pHalRuartOp->HalRuartInit(pHalRuartAdapter);
    pHalRuartOp->HalRuartRegIrq(pHalRuartAdapter);    
    pHalRuartOp->HalRuartIntEnable(pHalRuartAdapter);

#ifdef CONFIG_MBED_ENABLED
    // For stdio management
    if (uart_idx == STDIO_UART) {
        stdio_uart_inited = 1;
        memcpy(&stdio_uart, obj, sizeof(serial_t));
    }
#endif
}

void serial_free(serial_t *obj) 
{
    PHAL_RUART_ADAPTER pHalRuartAdapter;
    PHAL_RUART_OP      pHalRuartOp;

    pHalRuartAdapter = &(obj->hal_uart_adp);
    pHalRuartOp = &(obj->hal_uart_op);

    pHalRuartOp->HalRuartDeInit(pHalRuartAdapter);
    

    // TODO: recovery Pin Mux

#if 0
    RtlMfree((unsigned char *)(obj->uart_adp.pHalRuartOp), sizeof(HAL_RUART_OP));
    RtlMfree((unsigned char *)(obj->uart_adp.pHalRuartAdapter), sizeof(HAL_RUART_ADAPTER));    
    obj->uart_adp.pHalRuartOp = NULL;
    obj->uart_adp.pHalRuartAdapter = NULL;
#endif    
}

void serial_baud(serial_t *obj, int baudrate) {
    PHAL_RUART_ADAPTER pHalRuartAdapter;
    PHAL_RUART_OP      pHalRuartOp;

    pHalRuartAdapter = &(obj->hal_uart_adp);
    pHalRuartOp = &(obj->hal_uart_op);

    pHalRuartAdapter->BaudRate = baudrate;
    pHalRuartOp->HalRuartInit(pHalRuartAdapter);
}

void serial_format(serial_t *obj, int data_bits, SerialParity parity, int stop_bits) 
{
    PHAL_RUART_ADAPTER pHalRuartAdapter;
    PHAL_RUART_OP      pHalRuartOp;

    pHalRuartAdapter = &(obj->hal_uart_adp);
    pHalRuartOp = &(obj->hal_uart_op);
    
    if (data_bits == 8) {
        pHalRuartAdapter->WordLen = RUART_WLS_8BITS;
    } else {
        pHalRuartAdapter->WordLen = RUART_WLS_7BITS;
    }


    switch (parity) {
        case ParityOdd:
        case ParityForced0:
            pHalRuartAdapter->Parity = RUART_PARITY_ENABLE;
            pHalRuartAdapter->ParityType = RUART_ODD_PARITY;
            break;
        case ParityEven:
        case ParityForced1:
            pHalRuartAdapter->Parity = RUART_PARITY_ENABLE;
            pHalRuartAdapter->ParityType = RUART_EVEN_PARITY;
            break;
        default: // ParityNone
            pHalRuartAdapter->Parity = RUART_PARITY_DISABLE;
            break;
    }

    if (stop_bits == 1) {
        pHalRuartAdapter->StopBit = RUART_1_STOP_BIT;
    } else {
        pHalRuartAdapter->StopBit = RUART_NO_STOP_BIT;
    }

    pHalRuartOp->HalRuartInit(pHalRuartAdapter);
}

/******************************************************************************
 * INTERRUPTS HANDLING
 ******************************************************************************/

static void SerialTxDoneCallBack(VOID *pAdapter)
{
    PHAL_RUART_ADAPTER pHalRuartAdapter = pAdapter;
    u8 uart_idx = pHalRuartAdapter->UartIndex;

    // Mask UART TX FIFI empty
    pHalRuartAdapter->Interrupts &= ~RUART_IER_ETBEI;
    HalRuartSetIMRRtl8195a (pHalRuartAdapter);
    
    if (irq_handler[uart_idx] != NULL) {
        irq_handler[uart_idx](serial_irq_ids[uart_idx], TxIrq);
    }
}

static void SerialRxDoneCallBack(VOID *pAdapter)
{
    PHAL_RUART_ADAPTER pHalRuartAdapter = pAdapter;
    u8 uart_idx = pHalRuartAdapter->UartIndex;

    if (irq_handler[uart_idx] != NULL) {
        irq_handler[uart_idx](serial_irq_ids[uart_idx], RxIrq);
    }
}

void serial_irq_handler(serial_t *obj, uart_irq_handler handler, uint32_t id) 
{
    PHAL_RUART_ADAPTER pHalRuartAdapter;
//    PHAL_RUART_OP pHalRuartOp;
    u8 uart_idx;

    pHalRuartAdapter = &(obj->hal_uart_adp);
//    pHalRuartOp = &(obj->hal_uart_op);
    
    uart_idx = pHalRuartAdapter->UartIndex;
    
    irq_handler[uart_idx] = handler;
    serial_irq_ids[uart_idx] = id;        

    pHalRuartAdapter->TxTDCallback = SerialTxDoneCallBack;
    pHalRuartAdapter->TxTDCbPara = (void*)pHalRuartAdapter;
    pHalRuartAdapter->RxDRCallback = SerialRxDoneCallBack;
    pHalRuartAdapter->RxDRCbPara = (void*)pHalRuartAdapter;    

//    pHalRuartOp->HalRuartRegIrq(pHalRuartAdapter);
//    pHalRuartOp->HalRuartIntEnable(pHalRuartAdapter);
}


void serial_irq_set(serial_t *obj, SerialIrq irq, uint32_t enable) 
{
    PHAL_RUART_ADAPTER pHalRuartAdapter;
    PHAL_RUART_OP pHalRuartOp;

    pHalRuartAdapter = &(obj->hal_uart_adp);
    pHalRuartOp = &(obj->hal_uart_op);
    
    if (enable) {
        if (irq == RxIrq) {
            pHalRuartAdapter->Interrupts |= RUART_IER_ERBI | RUART_IER_ELSI;
            serial_irq_en |= SERIAL_RX_IRQ_EN;
            HalRuartSetIMRRtl8195a (pHalRuartAdapter);
        }
        else {
            serial_irq_en |= SERIAL_TX_IRQ_EN;
        }
        pHalRuartOp->HalRuartRegIrq(pHalRuartAdapter);
        pHalRuartOp->HalRuartIntEnable(pHalRuartAdapter);
    } 
    else { // disable
        if (irq == RxIrq) {
            pHalRuartAdapter->Interrupts &= ~(RUART_IER_ERBI | RUART_IER_ELSI);
            serial_irq_en &= ~SERIAL_RX_IRQ_EN;
        }
        else {
            pHalRuartAdapter->Interrupts &= RUART_IER_ETBEI;
            serial_irq_en &= ~SERIAL_TX_IRQ_EN;
        }
        HalRuartSetIMRRtl8195a (pHalRuartAdapter);
        if (pHalRuartAdapter->Interrupts == 0) {
            InterruptUnRegister(&pHalRuartAdapter->IrqHandle);
            InterruptDis(&pHalRuartAdapter->IrqHandle);
        }
    }
}

/******************************************************************************
 * READ/WRITE
 ******************************************************************************/

int serial_getc(serial_t *obj) 
{
    PHAL_RUART_ADAPTER pHalRuartAdapter=(PHAL_RUART_ADAPTER)&(obj->hal_uart_adp);
    u8  uart_idx = pHalRuartAdapter->UartIndex;

    while (!serial_readable(obj));
    return (int)((HAL_RUART_READ32(uart_idx, RUART_REV_BUF_REG_OFF)) & 0xFF);
}

void serial_putc(serial_t *obj, int c) 
{
    PHAL_RUART_ADAPTER pHalRuartAdapter=(PHAL_RUART_ADAPTER)&(obj->hal_uart_adp);
    u8  uart_idx = pHalRuartAdapter->UartIndex;
    
    while (!serial_writable(obj));
    HAL_RUART_WRITE32(uart_idx, RUART_TRAN_HOLD_REG_OFF, (c & 0xFF));

    if (serial_irq_en & SERIAL_TX_IRQ_EN) {
        // UnMask TX FIFO empty IRQ
        pHalRuartAdapter->Interrupts |= RUART_IER_ETBEI;
        HalRuartSetIMRRtl8195a (pHalRuartAdapter);
    }
}

int serial_readable(serial_t *obj) 
{
    PHAL_RUART_ADAPTER pHalRuartAdapter=(PHAL_RUART_ADAPTER)&(obj->hal_uart_adp);
    u8  uart_idx = pHalRuartAdapter->UartIndex;

    if ((HAL_RUART_READ32(uart_idx, RUART_LINE_STATUS_REG_OFF)) & RUART_LINE_STATUS_REG_DR) {
        return 1;
    }
    else {
        return 0;
    }
}

int serial_writable(serial_t *obj) 
{
    PHAL_RUART_ADAPTER pHalRuartAdapter=(PHAL_RUART_ADAPTER)&(obj->hal_uart_adp);
    u8  uart_idx = pHalRuartAdapter->UartIndex;

    if (HAL_RUART_READ32(uart_idx, RUART_LINE_STATUS_REG_OFF) & 
                        (RUART_LINE_STATUS_REG_THRE)) {
       return 1;
    }
    else {
        return 0;
    }
}

void serial_clear(serial_t *obj) 
{
    PHAL_RUART_ADAPTER pHalRuartAdapter;
    PHAL_RUART_OP pHalRuartOp;

    pHalRuartAdapter = &(obj->hal_uart_adp);
    pHalRuartOp = &(obj->hal_uart_op);

    pHalRuartOp->HalRuartResetRxFifo(pHalRuartAdapter);
}

void serial_pinout_tx(PinName tx) 
{
    pinmap_pinout(tx, PinMap_UART_TX);
}

void serial_break_set(serial_t *obj) 
{
    PHAL_RUART_ADAPTER pHalRuartAdapter=(PHAL_RUART_ADAPTER)&(obj->hal_uart_adp);
    u8  uart_idx = pHalRuartAdapter->UartIndex;
    u32 RegValue;

    RegValue = HAL_RUART_READ32(uart_idx, RUART_LINE_CTL_REG_OFF);
    RegValue |= BIT_UART_LCR_BREAK_CTRL;
    HAL_RUART_WRITE32(uart_idx, RUART_LINE_CTL_REG_OFF, RegValue);
}

void serial_break_clear(serial_t *obj) 
{
    PHAL_RUART_ADAPTER pHalRuartAdapter=(PHAL_RUART_ADAPTER)&(obj->hal_uart_adp);
    u8  uart_idx = pHalRuartAdapter->UartIndex;
    u32 RegValue;

    RegValue = HAL_RUART_READ32(uart_idx, RUART_LINE_CTL_REG_OFF);
    RegValue &= ~(BIT_UART_LCR_BREAK_CTRL);
    HAL_RUART_WRITE32(uart_idx, RUART_LINE_CTL_REG_OFF, RegValue);
}

#endif
