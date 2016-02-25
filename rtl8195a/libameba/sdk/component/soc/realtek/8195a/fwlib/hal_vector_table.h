/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _HAL_VECTOR_TABLE_H_
#define _HAL_VECTOR_TABLE_H_




extern _LONG_CALL_ROM_ VOID
VectorTableInitRtl8195A(
    IN  u32 StackP
);

extern _LONG_CALL_ROM_ VOID
VectorTableInitForOSRtl8195A(
    IN  VOID *PortSVC,
    IN  VOID *PortPendSVH,
    IN  VOID *PortSysTick    
);

extern _LONG_CALL_ROM_ BOOL
VectorIrqRegisterRtl8195A(
    IN  PIRQ_HANDLE pIrqHandle
);

extern _LONG_CALL_ROM_ BOOL
VectorIrqUnRegisterRtl8195A(
    IN  PIRQ_HANDLE pIrqHandle
);


extern _LONG_CALL_ROM_ VOID
VectorIrqEnRtl8195A(
    IN  PIRQ_HANDLE pIrqHandle
);

extern _LONG_CALL_ROM_ VOID
VectorIrqDisRtl8195A(
    IN  PIRQ_HANDLE pIrqHandle
);

 
extern _LONG_CALL_ROM_ VOID
HalPeripheralIntrHandle(VOID);
#endif //_HAL_VECTOR_TABLE_H_
