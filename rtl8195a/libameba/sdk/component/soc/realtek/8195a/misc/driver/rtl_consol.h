/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTK_CONSOL_H_
#define _RTK_CONSOL_H_
/*
 * Include user defined options first. Anything not defined in these files
 * will be set to standard values. Override anything you dont like!
 */
 #if defined(CONFIG_PLATFORM_8195A) || defined(CONFIG_PLATFORM_8711B)
#include "platform_opts.h"
#endif

#include "osdep_api.h"
#include "hal_diag.h"

#define CONSOLE_PREFIX	"#"


//Log UART
//UART_LOG_CMD_BUFLEN: only 126 bytes could be used for keeping input
//                                     cmd, the last byte is for string end ('\0').
#define UART_LOG_CMD_BUFLEN     127     
#define MAX_ARGV                10



typedef u32 (*ECHOFUNC)(IN u8*,...);    //UART LOG echo-function type.

typedef struct _UART_LOG_BUF_ {
        u8  BufCount;                           //record the input cmd char number.
        u8  UARTLogBuf[UART_LOG_CMD_BUFLEN];   //record the input command.
} UART_LOG_BUF, *PUART_LOG_BUF;



typedef struct _UART_LOG_CTL_ {
        u8  NewIdx;
        u8  SeeIdx;
        u8  RevdNo;
        u8  EscSTS;
        u8  ExecuteCmd;
        u8  ExecuteEsc;
        u8  BootRdy;
        u8  Resvd;
        PUART_LOG_BUF   pTmpLogBuf;        
        VOID *pfINPUT;
        PCOMMAND_TABLE  pCmdTbl;
        u32 CmdTblSz;
#ifdef CONFIG_UART_LOG_HISTORY        
        u32  CRSTS;
#endif        
#ifdef CONFIG_UART_LOG_HISTORY
        u8  (*pHistoryBuf)[UART_LOG_CMD_BUFLEN];
#endif
#ifdef CONFIG_KERNEL
		u32		TaskRdy;
		_Sema	Sema;
#else
        // Since ROM code will reference this typedef, so keep the typedef same size
        u32     TaskRdy;
        void    *Sema;
#endif
} UART_LOG_CTL, *PUART_LOG_CTL;


#define KB_ASCII_NUL    0x00
#define KB_ASCII_BS     0x08
#define KB_ASCII_TAB    0x09
#define KB_ASCII_LF     0x0A
#define KB_ASCII_CR     0x0D
#define KB_ASCII_ESC    0x1B
#define KB_ASCII_SP     0x20
#define KB_ASCII_BS_7F  0x7F
#define KB_ASCII_LBRKT  0x5B         //[

#define KB_SPACENO_TAB  1

#ifdef CONFIG_UART_LOG_HISTORY
#define UART_LOG_HISTORY_LEN    5
#endif

#ifdef CONFIG_DEBUG_LOG
#define _ConsolePrint  DiagPrintf
#else
#define _ConsolePrint
#endif

#ifndef CONSOLE_PREFIX
#define CONSOLE_PREFIX	"<RTL8195A>"
#endif

#define CONSOLE_8195A(...)     do {\
    _ConsolePrint("\r"CONSOLE_PREFIX __VA_ARGS__);\
}while(0)


_LONG_CALL_ VOID
RtlConsolInit(
    IN  u32     Boot,
    IN  u32     TBLSz,
    IN  VOID    *pTBL    
);

#if defined(CONFIG_KERNEL)
_LONG_CALL_ VOID
RtlConsolTaskRam(
    VOID *Data
);
#endif

_LONG_CALL_ VOID
RtlConsolTaskRom(
    VOID *Data
);


_LONG_CALL_ u32 
Strtoul(
    IN  const u8 *nptr,
    IN  u8 **endptr,
    IN  u32 base
);

void console_init(void);

#endif //_RTK_CONSOL_H_
