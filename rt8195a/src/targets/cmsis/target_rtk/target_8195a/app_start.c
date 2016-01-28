/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "rtl8195a.h"
#include "build_info.h"




extern int main( void );

#ifdef PLATFORM_RTX
extern  void SVC_Handler (void);
extern  void PendSV_Handler (void);
extern  void SysTick_Handler (void);
#endif




// The Main App entry point
void _AppStart(void)
{
#ifdef PLATFORM_RTX
    InterruptForOSInit((VOID*)SVC_Handler,
                       (VOID*)PendSV_Handler,
                       (VOID*)SysTick_Handler);
    __asm (
        "ldr   r0, =SystemInit\n"
        "blx   r0\n"
        "ldr   r0, =_start\n"
        "bx    r0\n"
    );

    DiagPrintf("OS system finished\n");
#else

    main();
    
#endif  // end of else of "#ifdef CONFIG_MBED_ENABLED"

}
