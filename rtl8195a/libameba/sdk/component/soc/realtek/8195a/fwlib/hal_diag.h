/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _HAL_DIAG_H_
#define _HAL_DIAG_H_


//Register offset
#define UART_REV_BUF_OFF            0x00
#define UART_TRAN_HOLD_OFF          0x00
#define UART_DLH_OFF                0x04
#define UART_DLL_OFF                0x00
#define UART_INTERRUPT_EN_REG_OFF   0x04
#define UART_INTERRUPT_IDEN_REG_OFF 0x08
#define UART_FIFO_CTL_REG_OFF       0x08
#define UART_LINE_CTL_REG_OFF       0x0c
#define UART_MODEM_CTL_REG_OFF      0x10
#define UART_LINE_STATUS_REG_OFF    0x14
#define UART_MODEM_STATUS_REG_OFF   0x18
#define UART_FIFO_ACCESS_REG_OFF    0x70
#define UART_STATUS_REG_OFF         0x7c
#define UART_TFL_OFF                0x80
#define UART_RFL_OFF                0x84


//Buad rate
#define UART_BAUD_RATE_2400         2400
#define UART_BAUD_RATE_4800         4800
#define UART_BAUD_RATE_9600         9600
#define UART_BAUD_RATE_19200        19200
#define UART_BAUD_RATE_38400        38400
#define UART_BAUD_RATE_57600        57600
#define UART_BAUD_RATE_115200       115200
#define UART_BAUD_RATE_921600       921600
#define UART_BAUD_RATE_1152000      1152000

#define UART_PARITY_ENABLE          0x08
#define UART_PARITY_DISABLE         0

#define UART_DATA_LEN_5BIT          0x0
#define UART_DATA_LEN_6BIT          0x1
#define UART_DATA_LEN_7BIT          0x2
#define UART_DATA_LEN_8BIT          0x3

#define UART_STOP_1BIT              0x0
#define UART_STOP_2BIT              0x4


#define HAL_UART_READ32(addr)            HAL_READ32(LOG_UART_REG_BASE, addr)
#define HAL_UART_WRITE32(addr, value)    HAL_WRITE32(LOG_UART_REG_BASE, addr, value)
#define HAL_UART_READ16(addr)            HAL_READ16(LOG_UART_REG_BASE, addr)
#define HAL_UART_WRITE16(addr, value)    HAL_WRITE16(LOG_UART_REG_BASE, addr, value)
#define HAL_UART_READ8(addr)             HAL_READ8(LOG_UART_REG_BASE, addr)
#define HAL_UART_WRITE8(addr, value)     HAL_WRITE8(LOG_UART_REG_BASE, addr, value)

typedef struct _LOG_UART_ADAPTER_ {
    u32     BaudRate;
    u32     FIFOControl; 
    u32     IntEnReg;
    u8      Parity;
    u8      Stop;
    u8      DataLength;
}LOG_UART_ADAPTER, *PLOG_UART_ADAPTER;

typedef struct _COMMAND_TABLE_ {
    const   u8* cmd;
    u16     ArgvCnt;
    u32     (*func)(u16 argc, u8* argv[]);
    const   u8* msg;
}COMMAND_TABLE, *PCOMMAND_TABLE;

//VOID
//HalLogUartHandle(void);


extern _LONG_CALL_ROM_ u32 
HalLogUartInit(
    IN  LOG_UART_ADAPTER    UartAdapter
    );


extern _LONG_CALL_ROM_ VOID 
HalSerialPutcRtl8195a(
    IN  u8 c
    );

extern _LONG_CALL_ROM_ u8 
HalSerialGetcRtl8195a(
    IN  BOOL    PullMode
    );

extern _LONG_CALL_ROM_ u32
HalSerialGetIsrEnRegRtl8195a(VOID);

extern _LONG_CALL_ROM_ VOID
HalSerialSetIrqEnRegRtl8195a (
    IN  u32 SetValue
);


#endif//_HAL_DIAG_H_
