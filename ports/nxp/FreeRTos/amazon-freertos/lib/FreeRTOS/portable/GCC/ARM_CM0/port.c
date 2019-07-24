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

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARM CM0 port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "fsl_tickless_generic.h"

extern uint32_t SystemCoreClock; /* in Kinetis SDK, this contains the system core clock speed */

/* Constants required to set up the initial stack. */
#define portINITIAL_XPSR			( 0x01000000 )

/* For backward compatibility, ensure configKERNEL_INTERRUPT_PRIORITY is
defined.  The value 255 should also ensure backward compatibility.
FreeRTOS.org versions prior to V4.3.0 did not include this definition. */
#ifndef configKERNEL_INTERRUPT_PRIORITY
	#define configKERNEL_INTERRUPT_PRIORITY 0
#endif

/* Let the user override the pre-loading of the initial LR with the address of
prvTaskExitError() in case it messes up unwinding of the stack in the
debugger. */
#ifdef configTASK_RETURN_ADDRESS
	#define portTASK_RETURN_ADDRESS	configTASK_RETURN_ADDRESS
#else
	#define portTASK_RETURN_ADDRESS	prvTaskExitError
#endif


/*
 * Exception handlers.
 */
void xPortPendSVHandler( void ) __attribute__ (( naked ));
void xPortSysTickHandler( void );
void vPortSVCHandler( void );

/*
 * Start first task is a separate function so it can be tested in isolation.
 */
static void vPortStartFirstTask( void ) __attribute__ (( naked ));

/*
 * Used to catch tasks that attempt to return from their implementing function.
 */
static void prvTaskExitError( void );

/*-----------------------------------------------------------*/

/* Each task maintains its own interrupt status in the critical nesting
variable. */
static UBaseType_t uxCriticalNesting = 0xaaaaaaaa;

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	/* Simulate the stack frame as it would be created by a context switch
	interrupt. */
	pxTopOfStack--; /* Offset added to account for the way the MCU uses the stack on entry/exit of interrupts. */
	*pxTopOfStack = portINITIAL_XPSR;	/* xPSR */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) pxCode;	/* PC */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) portTASK_RETURN_ADDRESS;	/* LR */
	pxTopOfStack -= 5;	/* R12, R3, R2 and R1. */
	*pxTopOfStack = ( StackType_t ) pvParameters;	/* R0 */
	pxTopOfStack -= 8; /* R11..R4. */

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

#if INCLUDE_vTaskEndScheduler
#include <setjmp.h>
static jmp_buf xJumpBuf; /* Used to restore the original context when the scheduler is ended. */
#endif

/*-----------------------------------------------------------*/
static void prvTaskExitError( void )
{
volatile uint32_t ulDummy = 0UL;

	/* A function that implements a task must not exit or attempt to return to
	its caller as there is nothing to return to.  If a task wants to exit it
	should instead call vTaskDelete( NULL ).

	Artificially force an assert() to be triggered if configASSERT() is
	defined, then stop here so application writers can catch the error. */
	configASSERT( uxCriticalNesting == ~0UL );
	portDISABLE_INTERRUPTS();
	while( ulDummy == 0 )
	{
		/* This file calls prvTaskExitError() after the scheduler has been
		started to remove a compiler warning about the function being defined
		but never called.  ulDummy is used purely to quieten other warnings
		about code appearing after this function is called - making ulDummy
		volatile makes the compiler think the function could return and
		therefore not output an 'unreachable code' warning for code that appears
		after it. */
	}
}
/*-----------------------------------------------------------*/

void vPortSVCHandler( void )
{
	/* This function is no longer used, but retained for backward
	compatibility. */
}
/*-----------------------------------------------------------*/

void vPortStartFirstTask( void )
{
	/* The MSP stack is not reset as, unlike on M3/4 parts, there is no vector
	table offset register that can be used to locate the initial stack value.
	Not all M0 parts have the application vector table at address 0. */
	__asm volatile(
	"	.syntax unified				\n"
	"	ldr  r2, pxCurrentTCBConst2	\n" /* Obtain location of pxCurrentTCB. */
	"	ldr  r3, [r2]				\n"
	"	ldr  r0, [r3]				\n" /* The first item in pxCurrentTCB is the task top of stack. */
	"	adds r0, #32					\n" /* Discard everything up to r0. */
	"	msr  psp, r0					\n" /* This is now the new top of stack to use in the task. */
	"	movs r0, #2					\n" /* Switch to the psp stack. */
	"	msr  CONTROL, r0				\n"
	"	isb							\n"
	"	pop  {r0-r5}					\n" /* Pop the registers that are saved automatically. */
	"	mov  lr, r5					\n" /* lr is now in r5. */
	"	pop  {r3}					\n" /* Return address is now in r3. */
	"	pop  {r2}					\n" /* Pop and discard XPSR. */
	"	cpsie i						\n" /* The first task has its context and interrupts can be enabled. */
	"	bx   r3						\n" /* Finally, jump to the user defined task code. */
	"								\n"
	"	.align 4					\n"
	"pxCurrentTCBConst2: .word pxCurrentTCB	  "
				  );
}
/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
BaseType_t xPortStartScheduler( void )
{
	/* Make PendSV, CallSV and SysTick the same priroity as the kernel. */
	*(portNVIC_SYSPRI2) |= portNVIC_PENDSV_PRI;
	*(portNVIC_SYSPRI2) |= portNVIC_SYSTICK_PRI;

	/* Start the timer that generates the tick ISR.  Interrupts are disabled
	here already. */
	prvSetupTimerInterrupt();

	/* Initialise the critical nesting count ready for the first task. */
	uxCriticalNesting = 0;

#if INCLUDE_vTaskEndScheduler
    if(setjmp(xJumpBuf) != 0 ) {
      /* here we will get in case of call to vTaskEndScheduler() */
      __asm volatile(
        " movs r0, #1         \n" /* Switch back to the MSP stack. */
        " msr CONTROL, r0     \n"
      );
      __asm volatile("dsb");
      __asm volatile("isb");
      return pdFALSE;
    }
#endif

	/* Start the first task. */
	vPortStartFirstTask();

	/* Should never get here as the tasks will now be executing!  Call the task
	exit error function to prevent compiler warnings about a static function
	not being called in the case that the application writer overrides this
	functionality by defining configTASK_RETURN_ADDRESS.  Call
	vTaskSwitchContext() so link time optimisation does not remove the
	symbol. */
	vTaskSwitchContext();
	prvTaskExitError();

	/* Should not get here! */
	return 0;
}

/*-----------------------------------------------------------*/
void vPortEndScheduler(void) {
    /* stop tick timer */
    portNVIC_SYSTICK_CTRL_REG = portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT;
    /* Jump back to the processor state prior to starting the
     scheduler.  This means we are not going to be using a
     task stack frame so the task can be deleted. */
#if INCLUDE_vTaskEndScheduler
    longjmp(xJumpBuf, 1);
#else
  for(;;){} /* wait here */
#endif
}
/*-----------------------------------------------------------*/

void vPortYield( void )
{
	/* Set a PendSV to request a context switch. */
	*( portNVIC_INT_CTRL ) = portNVIC_PENDSVSET;

	/* Barriers are normally not required but do ensure the code is completely
	within the specified behaviour for the architecture. */
	__asm volatile( "dsb" ::: "memory" );
	__asm volatile( "isb" );
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
    portDISABLE_INTERRUPTS();
    uxCriticalNesting++;
	__asm volatile( "dsb" ::: "memory" );
	__asm volatile( "isb" );
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
	configASSERT( uxCriticalNesting );
    uxCriticalNesting--;
    if( uxCriticalNesting == 0 )
    {
        portENABLE_INTERRUPTS();
    }
}
/*-----------------------------------------------------------*/

uint32_t ulSetInterruptMaskFromISR( void )
{
	__asm volatile(
					" mrs r0, PRIMASK	\n"
					" cpsid i			\n"
					" bx lr				  "
					::: "memory"
				  );

#if !defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
	/* To avoid compiler warnings.  The return statement will nevere be reached,
	but some compilers warn if it is not included, while others won't compile if
	it is. */
	return 0;
#endif
}
/*-----------------------------------------------------------*/

void vClearInterruptMaskFromISR( __attribute__( ( unused ) ) uint32_t ulMask )
{
	__asm volatile(
					" msr PRIMASK, r0	\n"
					" bx lr				  "
					::: "memory"
				  );

#if !defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
	/* Just to avoid compiler warning.  ulMask is used from the asm code but
	the compiler can't see that.  Some compilers generate warnings without the
	following line, while others generate warnings if the line is included. */
	( void ) ulMask;
#endif
}
/*-----------------------------------------------------------*/

void xPortPendSVHandler( void )
{
	/* This is a naked function. */

	__asm volatile
	(
	"	.syntax unified						\n"
	"	mrs r0, psp							\n"
	"										\n"
	"	ldr	r3, pxCurrentTCBConst			\n" /* Get the location of the current TCB. */
	"	ldr	r2, [r3]						\n"
	"										\n"
	"	subs r0, r0, #32					\n" /* Make space for the remaining low registers. */
	"	str r0, [r2]						\n" /* Save the new top of stack. */
	"	stmia r0!, {r4-r7}					\n" /* Store the low registers that are not saved automatically. */
	" 	mov r4, r8							\n" /* Store the high registers. */
	" 	mov r5, r9							\n"
	" 	mov r6, r10							\n"
	" 	mov r7, r11							\n"
	" 	stmia r0!, {r4-r7}					\n"
	"										\n"
	"	push {r3, r14}						\n"
	"	cpsid i								\n"
	"	bl vTaskSwitchContext				\n"
	"	cpsie i								\n"
	"	pop {r2, r3}						\n" /* lr goes in r3. r2 now holds tcb pointer. */
	"										\n"
	"	ldr r1, [r2]						\n"
	"	ldr r0, [r1]						\n" /* The first item in pxCurrentTCB is the task top of stack. */
	"	adds r0, r0, #16					\n" /* Move to the high registers. */
	"	ldmia r0!, {r4-r7}					\n" /* Pop the high registers. */
	" 	mov r8, r4							\n"
	" 	mov r9, r5							\n"
	" 	mov r10, r6							\n"
	" 	mov r11, r7							\n"
	"										\n"
	"	msr psp, r0							\n" /* Remember the new top of stack for the task. */
	"										\n"
	"	subs r0, r0, #32					\n" /* Go back for the low registers that are not automatically restored. */
	" 	ldmia r0!, {r4-r7}					\n" /* Pop low registers.  */
	"										\n"
	"	bx r3								\n"
	"										\n"
	"	.align 4							\n"
	"pxCurrentTCBConst: .word pxCurrentTCB	  "
	);
}
/*-----------------------------------------------------------*/

void xPortSysTickHandler( void )
{
uint32_t ulPreviousMask;

	ulPreviousMask = portSET_INTERRUPT_MASK_FROM_ISR();
	{
		/* Increment the RTOS tick. */
		if( xTaskIncrementTick() != pdFALSE )
		{
			/* Pend a context switch. */
			*(portNVIC_INT_CTRL) = portNVIC_PENDSVSET;
		}
	}
	portCLEAR_INTERRUPT_MASK_FROM_ISR( ulPreviousMask );
}
/*-----------------------------------------------------------*/


