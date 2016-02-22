/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTL8195A_WDT_H_
#define _RTL8195A_WDT_H_

#define WDGTIMERELY  (10*1024)  //us

typedef struct _WDG_REG_ {
    u16     WdgScalar;
    u8      WdgEnByte;
    u8      WdgClear:1;
    u8      WdgCunLimit:4;
    u8      Rsvd:1;
    u8      WdgMode:1;
    u8      WdgToISR:1;
}WDG_REG, *PWDG_REG;

typedef struct _WDG_ADAPTER_ {

    WDG_REG             Ctrl;
    IRQ_HANDLE          IrqHandle;
    TIMER_ADAPTER       WdgGTimer;
    VOID (*UserCallback)(u32 callback_id);    // User callback function
    u32                 callback_id;
}WDG_ADAPTER, *PWDG_ADAPTER;

typedef enum _WDG_CNTLMT_ {
    CNT1H    = 0,
    CNT3H    = 1,
    CNT7H    = 2,
    CNTFH    = 3,
    CNT1FH   = 4,
    CNT3FH   = 5,
    CNT7FH   = 6,
    CNTFFH   = 7,
    CNT1FFH  = 8,
    CNT3FFH  = 9,
    CNT7FFH  = 10,
    CNTFFFH  = 11
}WDG_CNTLMT, *PWDG_CNTLMT;


typedef enum _WDG_MODE_ {
    INT_MODE    = 0,
    RESET_MODE  = 1
}WDG_MODE, *PWDG_MODE;

extern VOID
WDGInitial(
    IN  u32 Period
);

extern VOID
WDGIrqInitial(
    VOID
);

extern VOID
WDGIrqInitial(
    VOID
);

extern VOID
WDGStop(
    VOID
);

extern VOID
WDGRefresh(
    VOID
);

extern VOID
WDGIrqCallBackReg(
    IN VOID *CallBack,
    IN u32   Id
);

#endif //_RTL8195A_WDT_H_
