/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#include "rtl8195a.h"


VOID
En32KCalibration(
    VOID
)
{
    u32 Rtemp;
    u32 Ttemp = 0;

    //DiagPrintf("32K clock source calibration\n");

    //set parameter
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, 0);
    //offset 1 = 0x1500
    Rtemp = 0x811500;
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, Rtemp);
    HalDelayUs(40);
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, 0);

    //offset 2 = 0x01c0
    Rtemp = 0x8201c0;
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, Rtemp);
    HalDelayUs(40);
        HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, 0);
    
    //offset 4 = 0x0100
    Rtemp = 0x840100;
        HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, Rtemp);
    HalDelayUs(40);
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, 0);

    //offset 0 = 0xf980
    Rtemp = 0x80f980;
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, Rtemp);
    HalDelayUs(40);
    HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, 0);

    while(1) {
        //Polling LOCK
        Rtemp = 0x110000;
        HAL_WRITE32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL0, Rtemp);
        //DiagPrintf("Polling lock\n");
        HalDelayUs(40);

        Rtemp = HAL_READ32(SYSTEM_CTRL_BASE,REG_OSC32K_REG_CTRL1);
        if ((Rtemp & 0x3000) != 0x0){
            //DiagPrintf("32.768 Calibration Success\n", Ttemp);
            break;
        }
        else {
            Ttemp++;
            HalDelayUs(30);
            //DiagPrintf("Check lock: %d\n", Ttemp);
            //DiagPrintf("0x278: %x\n", Rtemp);
            if (Ttemp > 100000) { /*Delay 100ms*/            
                DiagPrintf("32K Calibration Fail!!\n", Ttemp);
                break;
            }
        }
    }
}

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

#define WDGTIMERELY  (10*1024)  //us

WDG_ADAPTER          WDGAdapter;
extern HAL_TIMER_OP HalTimerOp;

VOID
WDGIrqHandle
(
    IN  VOID        *Data
)
{
}


VOID
WDGGtimerHandle
(
    IN  VOID        *Data
)
{
    u32 *Temp = (u32*)&(WDGAdapter.Ctrl);
    WDGAdapter.Ctrl.WdgClear    = 1;
    if (HAL_READ32(SYSTEM_CTRL_BASE,REG_SYS_DSTBY_INFO2) == 0) {
        HAL_WRITE32(VENDOR_REG_BASE, 0, ((*Temp)));
    }
}


VOID
InitWDGIRQ(VOID)
{
    u32 *Temp = (u32*)&(WDGAdapter.Ctrl);
    
    WDGAdapter.Ctrl.WdgScalar   = 0x96;
    WDGAdapter.Ctrl.WdgEnByte   = 0xA5;
    WDGAdapter.Ctrl.WdgClear    = 1;
    WDGAdapter.Ctrl.WdgCunLimit = CNTFFFH;
    WDGAdapter.Ctrl.WdgMode     = RESET_MODE;
    WDGAdapter.Ctrl.WdgToISR    = 0;

    if (WDGAdapter.Ctrl.WdgMode == INT_MODE) {
        
        WDGAdapter.IrqHandle.Data = NULL;
        WDGAdapter.IrqHandle.IrqFun = (IRQ_FUN)WDGIrqHandle;
        WDGAdapter.IrqHandle.IrqNum = WDG_IRQ;
        WDGAdapter.IrqHandle.Priority = 0;
             
        InterruptRegister(&(WDGAdapter.IrqHandle));
        InterruptEn(&(WDGAdapter.IrqHandle));
    }
    else {
        
        WDGAdapter.WdgGTimer.TimerIrqPriority   = 0;
        WDGAdapter.WdgGTimer.TimerMode          = USER_DEFINED;
        WDGAdapter.WdgGTimer.IrqDis             = OFF;
        WDGAdapter.WdgGTimer.TimerId            = 2;//
        WDGAdapter.WdgGTimer.IrqHandle.IrqFun   = (IRQ_FUN)WDGGtimerHandle;
        WDGAdapter.WdgGTimer.IrqHandle.IrqNum   = TIMER2_7_IRQ;
        WDGAdapter.WdgGTimer.IrqHandle.Priority = 0;
        WDGAdapter.WdgGTimer.IrqHandle.Data     = NULL;
        
        if ((WDGAdapter.Ctrl.WdgCunLimit == CNTFFFH)&&(WDGAdapter.Ctrl.WdgScalar >= 0x8429)){
            WDGAdapter.WdgGTimer.TimerLoadValueUs = 0xFFFFFFFF - WDGTIMERELY;
        }
        else {
            WDGAdapter.WdgGTimer.TimerLoadValueUs   = (BIT0 << (WDGAdapter.Ctrl.WdgCunLimit+1))
                *WDGAdapter.Ctrl.WdgScalar*TIMER_TICK_US - WDGTIMERELY;
        }

        HalTimerOpInit((VOID*)(&HalTimerOp));
        HalTimerOp.HalTimerInit((VOID*) &(WDGAdapter.WdgGTimer));
    }
    //fill reg
    HAL_WRITE32(VENDOR_REG_BASE, 0, ((*Temp)));
}


//WDG
VOID HalWdgInit(
    VOID
)
{
        
}
