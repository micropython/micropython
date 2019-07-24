/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* Standard includes. */
#include <stdint.h>

/* Portasm includes. */
#include "portasm.h"

void vRestoreContextOfFirstTask( void ) /* __attribute__ (( naked )) PRIVILEGED_FUNCTION */
{
	__asm volatile
	(
	"	.syntax unified									\n"
	"													\n"
	"	ldr  r2, pxCurrentTCBConst2						\n" /* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
	"	ldr  r1, [r2]									\n" /* Read pxCurrentTCB. */
	"	ldr  r0, [r1]									\n" /* Read top of stack from TCB - The first item in pxCurrentTCB is the task top of stack. */
	"													\n"
	#if( configENABLE_MPU == 1 )
	"	adds r1, #4										\n" /* r1 = r1 + 4. r1 now points to MAIR0 in TCB. */
	"	ldr  r3, [r1]									\n" /* r3 = *r1 i.e. r3 = MAIR0. */
	"	ldr  r2, xMAIR0Const2							\n" /* r2 = 0xe000edc0 [Location of MAIR0]. */
	"	str  r3, [r2]									\n" /* Program MAIR0. */
	"	ldr  r2, xRNRConst2								\n" /* r2 = 0xe000ed98 [Location of RNR]. */
	"	movs r3, #4										\n" /* r3 = 4. */
	"	str  r3, [r2]									\n" /* Program RNR = 4. */
	"	adds r1, #4										\n" /* r1 = r1 + 4. r1 now points to first RBAR in TCB. */
	"	ldr  r2, xRBARConst2							\n" /* r2 = 0xe000ed9c [Location of RBAR]. */
	"	ldmia r1!, {r4-r11}								\n" /* Read 4 set of RBAR/RLAR registers from TCB. */
	"	stmia r2!, {r4-r11}								\n" /* Write 4 set of RBAR/RLAR registers using alias registers. */
	#endif /* configENABLE_MPU */
	"													\n"
	#if( configENABLE_MPU == 1 )
	"	ldm  r0!, {r1-r3}								\n" /* Read from stack - r1 = PSPLIM, r2 = CONTROL and r3 = EXC_RETURN. */
	"	msr  psplim, r1									\n" /* Set this task's PSPLIM value. */
	"	msr  control, r2								\n" /* Set this task's CONTROL value. */
	"	adds r0, #32									\n" /* Discard everything up to r0. */
	"	msr  psp, r0									\n" /* This is now the new top of stack to use in the task. */
	"	isb												\n"
	"	bx   r3											\n" /* Finally, branch to EXC_RETURN. */
	#else /* configENABLE_MPU */
	"	ldm  r0!, {r1-r2}								\n" /* Read from stack - r1 = PSPLIM and r2 = EXC_RETURN. */
	"	msr  psplim, r1									\n" /* Set this task's PSPLIM value. */
	"	movs r1, #2										\n" /* r1 = 2. */
	"	msr  CONTROL, r1								\n" /* Switch to use PSP in the thread mode. */
	"	adds r0, #32									\n" /* Discard everything up to r0. */
	"	msr  psp, r0									\n" /* This is now the new top of stack to use in the task. */
	"	isb												\n"
	"	bx   r2											\n" /* Finally, branch to EXC_RETURN. */
	#endif /* configENABLE_MPU */
	"													\n"
	"	.align 4										\n"
	"pxCurrentTCBConst2: .word pxCurrentTCB				\n"
	#if( configENABLE_MPU == 1 )
	"xMAIR0Const2: .word 0xe000edc0						\n"
	"xRNRConst2: .word 0xe000ed98						\n"
	"xRBARConst2: .word 0xe000ed9c						\n"
	#endif /* configENABLE_MPU */
	);
}
/*-----------------------------------------------------------*/

BaseType_t xIsPrivileged( void ) /* __attribute__ (( naked )) */
{
	__asm volatile
	(
	"	mrs r0, control									\n" /* r0 = CONTROL. */
	"	tst r0, #1										\n" /* Perform r0 & 1 (bitwise AND) and update the conditions flag. */
	"	ite ne											\n"
	"	movne r0, #0									\n" /* CONTROL[0]!=0. Return false to indicate that the processor is not privileged. */
	"	moveq r0, #1									\n" /* CONTROL[0]==0. Return true to indicate that the processor is privileged. */
	"	bx lr											\n" /* Return. */
	"													\n"
	"	.align 4										\n"
	::: "r0", "memory"
	);
}
/*-----------------------------------------------------------*/

void vRaisePrivilege( void ) /* __attribute__ (( naked )) PRIVILEGED_FUNCTION */
{
	 __asm volatile
	(
	"	mrs  r0, control								\n" /* Read the CONTROL register. */
	"	bic r0, #1										\n" /* Clear the bit 0. */
	"	msr  control, r0								\n" /* Write back the new CONTROL value. */
	"	bx lr											\n" /* Return to the caller. */
	::: "r0", "memory"
	);
}
/*-----------------------------------------------------------*/

void vResetPrivilege( void ) /* __attribute__ (( naked )) */
{
	__asm volatile
	(
	"	mrs r0, control									\n" /* r0 = CONTROL. */
	"	orr r0, #1										\n" /* r0 = r0 | 1. */
	"	msr control, r0									\n" /* CONTROL = r0. */
	"	bx lr											\n" /* Return to the caller. */
	:::"r0", "memory"
	);
}
/*-----------------------------------------------------------*/

void vStartFirstTask( void ) /* __attribute__ (( naked )) PRIVILEGED_FUNCTION */
{
	__asm volatile
	(
	"	ldr r0, xVTORConst								\n" /* Use the NVIC offset register to locate the stack. */
	"	ldr r0, [r0]									\n" /* Read the VTOR register which gives the address of vector table. */
	"	ldr r0, [r0]									\n" /* The first entry in vector table is stack pointer. */
	"	msr msp, r0										\n" /* Set the MSP back to the start of the stack. */
	"	cpsie i											\n" /* Globally enable interrupts. */
	"	cpsie f											\n"
	"	dsb												\n"
	"	isb												\n"
	"	svc %0											\n" /* System call to start the first task. */
	"	nop												\n"
	"													\n"
	"   .align 4										\n"
	"xVTORConst: .word 0xe000ed08						\n"
	:: "i" ( portSVC_START_SCHEDULER ) : "memory"
	);
}
/*-----------------------------------------------------------*/

uint32_t ulSetInterruptMaskFromISR( void ) /* __attribute__(( naked )) PRIVILEGED_FUNCTION */
{
	__asm volatile
	(
	"	mrs r0, PRIMASK									\n"
	"	cpsid i											\n"
	"	bx lr											\n"
	::: "memory"
	);

#if !defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
	/* To avoid compiler warnings.  The return statement will never be reached,
	 * but some compilers warn if it is not included, while others won't compile
	 * if it is. */
	return 0;
#endif
}
/*-----------------------------------------------------------*/

void vClearInterruptMaskFromISR( __attribute__( ( unused ) ) uint32_t ulMask ) /* __attribute__(( naked )) PRIVILEGED_FUNCTION */
{
	__asm volatile
	(
	"	msr PRIMASK, r0									\n"
	"	bx lr											\n"
	::: "memory"
	);

#if !defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
	/* Just to avoid compiler warning.  ulMask is used from the asm code but
	 * the compiler can't see that.  Some compilers generate warnings without
	 * the following line, while others generate warnings if the line is
	 * included. */
	( void ) ulMask;
#endif
}
/*-----------------------------------------------------------*/

void PendSV_Handler( void ) /* __attribute__ (( naked )) PRIVILEGED_FUNCTION */
{
	__asm volatile
	(
	"	.syntax unified									\n"
	"													\n"
	"	mrs r0, psp										\n" /* Read PSP in r0. */
	#if( configENABLE_FPU == 1 )
	"	tst lr, #0x10									\n" /* Test Bit[4] in LR. Bit[4] of EXC_RETURN is 0 if the FPU is in use. */
	"	it eq											\n"
	"	vstmdbeq r0!, {s16-s31}							\n" /* Store the FPU registers which are not saved automatically. */
	#endif /* configENABLE_FPU */
	#if( configENABLE_MPU == 1 )
	"	mrs r1, psplim									\n" /* r1 = PSPLIM. */
	"	mrs r2, control									\n" /* r2 = CONTROL. */
	"	mov r3, lr										\n" /* r3 = LR/EXC_RETURN. */
	"	stmdb r0!, {r1-r11}								\n" /* Store on the stack - PSPLIM, CONTROL, LR and registers that are not automatically saved. */
	#else /* configENABLE_MPU */
	"	mrs r2, psplim									\n" /* r2 = PSPLIM. */
	"	mov r3, lr										\n" /* r3 = LR/EXC_RETURN. */
	"	stmdb r0!, {r2-r11}								\n" /* Store on the stack - PSPLIM, LR and registers that are not automatically saved. */
	#endif /* configENABLE_MPU */
	"													\n"
	"	ldr r2, pxCurrentTCBConst						\n" /* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
	"	ldr r1, [r2]									\n" /* Read pxCurrentTCB. */
	"	str r0, [r1]									\n" /* Save the new top of stack in TCB. */
	"													\n"
	"	cpsid i											\n"
	"	bl vTaskSwitchContext							\n"
	"	cpsie i											\n"
	"													\n"
	"	ldr r2, pxCurrentTCBConst						\n" /* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
	"	ldr r1, [r2]									\n" /* Read pxCurrentTCB. */
	"	ldr r0, [r1]									\n" /* The first item in pxCurrentTCB is the task top of stack. r0 now points to the top of stack. */
	"													\n"
	#if( configENABLE_MPU == 1 )
	"	adds r1, #4										\n" /* r1 = r1 + 4. r1 now points to MAIR0 in TCB. */
	"	ldr r3, [r1]									\n" /* r3 = *r1 i.e. r3 = MAIR0. */
	"	ldr r2, xMAIR0Const								\n" /* r2 = 0xe000edc0 [Location of MAIR0]. */
	"	str r3, [r2]									\n" /* Program MAIR0. */
	"	ldr r2, xRNRConst								\n" /* r2 = 0xe000ed98 [Location of RNR]. */
	"	movs r3, #4										\n" /* r3 = 4. */
	"	str r3, [r2]									\n" /* Program RNR = 4. */
	"	adds r1, #4										\n" /* r1 = r1 + 4. r1 now points to first RBAR in TCB. */
	"	ldr r2, xRBARConst								\n" /* r2 = 0xe000ed9c [Location of RBAR]. */
	"	ldmia r1!, {r4-r11}								\n" /* Read 4 sets of RBAR/RLAR registers from TCB. */
	"	stmia r2!, {r4-r11}								\n" /* Write 4 set of RBAR/RLAR registers using alias registers. */
	#endif /* configENABLE_MPU */
	"													\n"
	#if( configENABLE_MPU == 1 )
	"	ldmia r0!, {r1-r11}								\n" /* Read from stack - r1 = PSPLIM, r2 = CONTROL, r3 = LR and r4-r11 restored. */
	#else /* configENABLE_MPU */
	"	ldmia r0!, {r2-r11}								\n" /* Read from stack - r2 = PSPLIM, r3 = LR and r4-r11 restored. */
	#endif /* configENABLE_MPU */
	"													\n"
	#if( configENABLE_FPU == 1 )
	"	tst r3, #0x10									\n" /* Test Bit[4] in LR. Bit[4] of EXC_RETURN is 0 if the FPU is in use. */
	"	it eq											\n"
	"	vldmiaeq r0!, {s16-s31}							\n" /* Restore the FPU registers which are not restored automatically. */
	#endif /* configENABLE_FPU */
	"													\n"
	#if( configENABLE_MPU == 1 )
	"	msr psplim, r1									\n" /* Restore the PSPLIM register value for the task. */
	"	msr control, r2									\n" /* Restore the CONTROL register value for the task. */
	#else /* configENABLE_MPU */
	"	msr psplim, r2									\n" /* Restore the PSPLIM register value for the task. */
	#endif /* configENABLE_MPU */
	"	msr psp, r0										\n" /* Remember the new top of stack for the task. */
	"	bx r3											\n"
	"													\n"
	"	.align 4										\n"
	"pxCurrentTCBConst: .word pxCurrentTCB				\n"
	"xMAIR0Const: .word 0xe000edc0						\n"
	"xRNRConst: .word 0xe000ed98						\n"
	"xRBARConst: .word 0xe000ed9c						\n"
	);
}
/*-----------------------------------------------------------*/

void SVC_Handler( void ) /* __attribute__ (( naked )) PRIVILEGED_FUNCTION */
{
	__asm volatile
	(
	"	 tst lr, #4										\n"
	"	 ite eq											\n"
	"	 mrseq r0, msp									\n"
	"	 mrsne r0, psp									\n"
	"	 ldr r1, svchandler_address_const				\n"
	"	 bx r1											\n"
	"													\n"
	"	 .align 4										\n"
	"svchandler_address_const: .word vPortSVCHandler_C	\n"
	);
}
/*-----------------------------------------------------------*/
