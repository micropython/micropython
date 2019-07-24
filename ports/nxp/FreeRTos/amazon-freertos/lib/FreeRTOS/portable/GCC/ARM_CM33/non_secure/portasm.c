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
	"	ldr  r3, [r2]									\n" /* Read pxCurrentTCB. */
	"	ldr  r0, [r3]									\n" /* Read top of stack from TCB - The first item in pxCurrentTCB is the task top of stack. */
	"													\n"
	#if( configENABLE_MPU == 1 )
	"	adds r3, #4										\n" /* r3 = r3 + 4. r3 now points to MAIR0 in TCB. */
	"	ldr  r4, [r3]									\n" /* r4 = *r3 i.e. r4 = MAIR0. */
	"	ldr  r2, xMAIR0Const2							\n" /* r2 = 0xe000edc0 [Location of MAIR0]. */
	"	str  r4, [r2]									\n" /* Program MAIR0. */
	"	ldr  r2, xRNRConst2								\n" /* r2 = 0xe000ed98 [Location of RNR]. */
	"	movs r4, #4										\n" /* r4 = 4. */
	"	str  r4, [r2]									\n" /* Program RNR = 4. */
	"	adds r3, #4										\n" /* r3 = r3 + 4. r3 now points to first RBAR in TCB. */
	"	ldr  r2, xRBARConst2							\n" /* r2 = 0xe000ed9c [Location of RBAR]. */
	"	ldmia r3!, {r4-r11}								\n" /* Read 4 set of RBAR/RLAR registers from TCB. */
	"	stmia r2!, {r4-r11}								\n" /* Write 4 set of RBAR/RLAR registers using alias registers. */
	#endif /* configENABLE_MPU */
	"													\n"
	#if( configENABLE_MPU == 1 )
	"	ldm  r0!, {r1-r4}								\n" /* Read from stack - r1 = xSecureContext, r2 = PSPLIM, r3 = CONTROL and r4 = EXC_RETURN. */
	"	ldr  r5, xSecureContextConst2					\n"
	"	str  r1, [r5]									\n" /* Set xSecureContext to this task's value for the same. */
	"	msr  psplim, r2									\n" /* Set this task's PSPLIM value. */
	"	msr  control, r3								\n" /* Set this task's CONTROL value. */
	"	adds r0, #32									\n" /* Discard everything up to r0. */
	"	msr  psp, r0									\n" /* This is now the new top of stack to use in the task. */
	"	isb												\n"
	"	bx   r4											\n" /* Finally, branch to EXC_RETURN. */
	#else /* configENABLE_MPU */
	"	ldm  r0!, {r1-r3}								\n" /* Read from stack - r1 = xSecureContext, r2 = PSPLIM and r3 = EXC_RETURN. */
	"	ldr  r4, xSecureContextConst2					\n"
	"	str  r1, [r4]									\n" /* Set xSecureContext to this task's value for the same. */
	"	msr  psplim, r2									\n" /* Set this task's PSPLIM value. */
	"	movs r1, #2										\n" /* r1 = 2. */
	"	msr  CONTROL, r1								\n" /* Switch to use PSP in the thread mode. */
	"	adds r0, #32									\n" /* Discard everything up to r0. */
	"	msr  psp, r0									\n" /* This is now the new top of stack to use in the task. */
	"	isb												\n"
	"	bx   r3											\n" /* Finally, branch to EXC_RETURN. */
	#endif /* configENABLE_MPU */
	"													\n"
	"	.align 4										\n"
	"pxCurrentTCBConst2: .word pxCurrentTCB				\n"
	"xSecureContextConst2: .word xSecureContext			\n"
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
	"	mrs r0, control									\n" /* Read the CONTROL register. */
	"	bic r0, #1										\n" /* Clear the bit 0. */
	"	msr control, r0									\n" /* Write back the new CONTROL value. */
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
	"	.extern SecureContext_SaveContext				\n"
	"	.extern SecureContext_LoadContext				\n"
	"													\n"
	"	mrs r1, psp										\n" /* Read PSP in r1. */
	"	ldr r2, xSecureContextConst						\n" /* Read the location of xSecureContext i.e. &( xSecureContext ). */
	"	ldr r0, [r2]									\n" /* Read xSecureContext - Value of xSecureContext must be in r0 as it is used as a parameter later. */
	"													\n"
	"	cbz r0, save_ns_context							\n" /* No secure context to save. */
	"	push {r0-r2, r14}								\n"
	"	bl SecureContext_SaveContext					\n"
	"	pop {r0-r3}										\n" /* LR is now in r3. */
	"	mov lr, r3										\n" /* LR = r3. */
	"	lsls r2, r3, #25								\n" /* r2 = r3 << 25. Bit[6] of EXC_RETURN is 1 if secure stack was used, 0 if non-secure stack was used to store stack frame. */
	"	bpl save_ns_context								\n" /* bpl - branch if positive or zero. If r2 >= 0 ==> Bit[6] in EXC_RETURN is 0 i.e. non-secure stack was used. */
	"	ldr r3, pxCurrentTCBConst						\n" /* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
	"	ldr r2, [r3]									\n" /* Read pxCurrentTCB. */
	#if( configENABLE_MPU == 1 )
	"	subs r1, r1, #16								\n" /* Make space for xSecureContext, PSPLIM, CONTROL and LR on the stack. */
	"	str r1, [r2]									\n" /* Save the new top of stack in TCB. */
	"	mrs r2, psplim									\n" /* r2 = PSPLIM. */
	"	mrs r3, control									\n" /* r3 = CONTROL. */
	"	mov r4, lr										\n" /* r4 = LR/EXC_RETURN. */
	"	stmia r1!, {r0, r2-r4}							\n" /* Store xSecureContext, PSPLIM, CONTROL and LR on the stack. */
	#else /* configENABLE_MPU */
	"	subs r1, r1, #12								\n" /* Make space for xSecureContext, PSPLIM and LR on the stack. */
	"	str r1, [r2]									\n" /* Save the new top of stack in TCB. */
	"	mrs r2, psplim									\n" /* r2 = PSPLIM. */
	"	mov r3, lr										\n" /* r3 = LR/EXC_RETURN. */
	"	stmia r1!, {r0, r2-r3}							\n" /* Store xSecureContext, PSPLIM and LR on the stack. */
	#endif /* configENABLE_MPU */
	"	b select_next_task								\n"
	"													\n"
	" save_ns_context:									\n"
	"	ldr r3, pxCurrentTCBConst						\n" /* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
	"	ldr r2, [r3]									\n" /* Read pxCurrentTCB. */
	#if( configENABLE_FPU == 1 )
	"	tst lr, #0x10									\n" /* Test Bit[4] in LR. Bit[4] of EXC_RETURN is 0 if the FPU is in use. */
	"	it eq											\n"
	"	vstmdbeq r1!, {s16-s31}							\n" /* Store the FPU registers which are not saved automatically. */
	#endif /* configENABLE_FPU */
	#if( configENABLE_MPU == 1 )
	"	subs r1, r1, #48								\n" /* Make space for xSecureContext, PSPLIM, CONTROL, LR and the remaining registers on the stack. */
	"	str r1, [r2]									\n" /* Save the new top of stack in TCB. */
	"	adds r1, r1, #16								\n" /* r1 = r1 + 16. */
	"	stm r1, {r4-r11}								\n" /* Store the registers that are not saved automatically. */
	"	mrs r2, psplim									\n" /* r2 = PSPLIM. */
	"	mrs r3, control									\n" /* r3 = CONTROL. */
	"	mov r4, lr										\n" /* r4 = LR/EXC_RETURN. */
	"	subs r1, r1, #16								\n" /* r1 = r1 - 16. */
	"	stm r1, {r0, r2-r4}								\n" /* Store xSecureContext, PSPLIM, CONTROL and LR on the stack. */
	#else /* configENABLE_MPU */
	"	subs r1, r1, #44								\n" /* Make space for xSecureContext, PSPLIM, LR and the remaining registers on the stack. */
	"	str r1, [r2]									\n" /* Save the new top of stack in TCB. */
	"	adds r1, r1, #12								\n" /* r1 = r1 + 12. */
	"	stm r1, {r4-r11}								\n" /* Store the registers that are not saved automatically. */
	"	mrs r2, psplim									\n" /* r2 = PSPLIM. */
	"	mov r3, lr										\n" /* r3 = LR/EXC_RETURN. */
	"	subs r1, r1, #12								\n" /* r1 = r1 - 12. */
	"	stmia r1!, {r0, r2-r3}							\n" /* Store xSecureContext, PSPLIM and LR on the stack. */
	#endif /* configENABLE_MPU */
	"													\n"
	" select_next_task:									\n"
	"	cpsid i											\n"
	"	bl vTaskSwitchContext							\n"
	"	cpsie i											\n"
	"													\n"
	"	ldr r2, pxCurrentTCBConst						\n" /* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
	"	ldr r3, [r2]									\n" /* Read pxCurrentTCB. */
	"	ldr r1, [r3]									\n" /* The first item in pxCurrentTCB is the task top of stack. r1 now points to the top of stack. */
	"													\n"
	#if( configENABLE_MPU == 1 )
	"	adds r3, #4										\n" /* r3 = r3 + 4. r3 now points to MAIR0 in TCB. */
	"	ldr r4, [r3]									\n" /* r4 = *r3 i.e. r4 = MAIR0. */
	"	ldr r2, xMAIR0Const								\n" /* r2 = 0xe000edc0 [Location of MAIR0]. */
	"	str r4, [r2]									\n" /* Program MAIR0. */
	"	ldr r2, xRNRConst								\n" /* r2 = 0xe000ed98 [Location of RNR]. */
	"	movs r4, #4										\n" /* r4 = 4. */
	"	str r4, [r2]									\n" /* Program RNR = 4. */
	"	adds r3, #4										\n" /* r3 = r3 + 4. r3 now points to first RBAR in TCB. */
	"	ldr  r2, xRBARConst								\n" /* r2 = 0xe000ed9c [Location of RBAR]. */
	"	ldmia r3!, {r4-r11}								\n" /* Read 4 sets of RBAR/RLAR registers from TCB. */
	"	stmia r2!, {r4-r11}								\n" /* Write 4 set of RBAR/RLAR registers using alias registers. */
	#endif /* configENABLE_MPU */
	"													\n"
	#if( configENABLE_MPU == 1 )
	"	ldmia r1!, {r0, r2-r4}							\n" /* Read from stack - r0 = xSecureContext, r2 = PSPLIM, r3 = CONTROL and r4 = LR. */
	"	msr psplim, r2									\n" /* Restore the PSPLIM register value for the task. */
	"	msr control, r3									\n" /* Restore the CONTROL register value for the task. */
	"	mov lr, r4										\n" /* LR = r4. */
	"	ldr r2, xSecureContextConst						\n" /* Read the location of xSecureContext i.e. &( xSecureContext ). */
	"	str r0, [r2]									\n" /* Restore the task's xSecureContext. */
	"	cbz r0, restore_ns_context						\n" /* If there is no secure context for the task, restore the non-secure context. */
	"	push {r1,r4}									\n"
	"	bl SecureContext_LoadContext					\n" /* Restore the secure context. */
	"	pop {r1,r4}										\n"
	"	mov lr, r4										\n" /* LR = r4. */
	"	lsls r2, r4, #25								\n" /* r2 = r4 << 25. Bit[6] of EXC_RETURN is 1 if secure stack was used, 0 if non-secure stack was used to store stack frame. */
	"	bpl restore_ns_context							\n" /* bpl - branch if positive or zero. If r2 >= 0 ==> Bit[6] in EXC_RETURN is 0 i.e. non-secure stack was used. */
	"	msr psp, r1										\n" /* Remember the new top of stack for the task. */
	"	bx lr											\n"
	#else /* configENABLE_MPU */
	"	ldmia r1!, {r0, r2-r3}							\n" /* Read from stack - r0 = xSecureContext, r2 = PSPLIM and r3 = LR. */
	"	msr psplim, r2									\n" /* Restore the PSPLIM register value for the task. */
	"	mov lr, r3										\n" /* LR = r3. */
	"	ldr r2, xSecureContextConst						\n" /* Read the location of xSecureContext i.e. &( xSecureContext ). */
	"	str r0, [r2]									\n" /* Restore the task's xSecureContext. */
	"	cbz r0, restore_ns_context						\n" /* If there is no secure context for the task, restore the non-secure context. */
	"	push {r1,r3}									\n"
	"	bl SecureContext_LoadContext					\n" /* Restore the secure context. */
	"	pop {r1,r3}										\n"
	"	mov lr, r3										\n" /* LR = r3. */
	"	lsls r2, r3, #25								\n" /* r2 = r3 << 25. Bit[6] of EXC_RETURN is 1 if secure stack was used, 0 if non-secure stack was used to store stack frame. */
	"	bpl restore_ns_context							\n" /* bpl - branch if positive or zero. If r2 >= 0 ==> Bit[6] in EXC_RETURN is 0 i.e. non-secure stack was used. */
	"	msr psp, r1										\n" /* Remember the new top of stack for the task. */
	"	bx lr											\n"
	#endif /* configENABLE_MPU */
	"													\n"
	" restore_ns_context:								\n"
	"	ldmia r1!, {r4-r11}								\n" /* Restore the registers that are not automatically restored. */
	#if( configENABLE_FPU == 1 )
	"	tst lr, #0x10									\n" /* Test Bit[4] in LR. Bit[4] of EXC_RETURN is 0 if the FPU is in use. */
	"	it eq											\n"
	"	vldmiaeq r1!, {s16-s31}							\n" /* Restore the FPU registers which are not restored automatically. */
	#endif /* configENABLE_FPU */
	"	msr psp, r1										\n" /* Remember the new top of stack for the task. */
	"	bx lr											\n"
	"													\n"
	"	.align 4										\n"
	"pxCurrentTCBConst: .word pxCurrentTCB				\n"
	"xSecureContextConst: .word xSecureContext			\n"
	#if( configENABLE_MPU == 1 )
	"xMAIR0Const: .word 0xe000edc0						\n"
	"xRNRConst: .word 0xe000ed98						\n"
	"xRBARConst: .word 0xe000ed9c						\n"
	#endif /* configENABLE_MPU */
	);
}
/*-----------------------------------------------------------*/

void SVC_Handler( void ) /* __attribute__ (( naked )) PRIVILEGED_FUNCTION */
{
	__asm volatile
	(
	"	tst lr, #4										\n"
	"	ite eq											\n"
	"	mrseq r0, msp									\n"
	"	mrsne r0, psp									\n"
	"	ldr r1, svchandler_address_const				\n"
	"	bx r1											\n"
	"													\n"
	"	.align 4										\n"
	"svchandler_address_const: .word vPortSVCHandler_C	\n"
	);
}
/*-----------------------------------------------------------*/

void vPortAllocateSecureContext( uint32_t ulSecureStackSize ) /* __attribute__ (( naked )) */
{
	__asm volatile
	(
	"	svc %0											\n" /* Secure context is allocated in the supervisor call. */
	"	bx lr											\n" /* Return. */
	:: "i" ( portSVC_ALLOCATE_SECURE_CONTEXT ) : "memory"
	);
}
/*-----------------------------------------------------------*/

void vPortFreeSecureContext( uint32_t *pulTCB ) /* __attribute__ (( naked )) PRIVILEGED_FUNCTION */
{
	__asm volatile
	(
	"	ldr r1, [r0]									\n" /* The first item in the TCB is the top of the stack. */
	"	ldr r0, [r1]									\n" /* The first item on the stack is the task's xSecureContext. */
	"	cmp r0, #0										\n" /* Raise svc if task's xSecureContext is not NULL. */
	"	it ne											\n"
	"	svcne %0										\n" /* Secure context is freed in the supervisor call. */
	"	bx lr											\n" /* Return. */
	:: "i" ( portSVC_FREE_SECURE_CONTEXT ) : "memory"
	);
}
/*-----------------------------------------------------------*/
