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

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
 * all the API functions to use the MPU wrappers. That should only be done when
 * task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

extern uint32_t SystemCoreClock; /* in Kinetis SDK, this contains the system core clock speed */

/* MPU wrappers includes. */
#include "mpu_wrappers.h"

/* Portasm includes. */
#include "portasm.h"

#if( configENABLE_TRUSTZONE == 1 )
	/* Secure components includes. */
	#include "secure_context.h"
	#include "secure_init.h"
#endif /* configENABLE_TRUSTZONE */

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/**
 * The FreeRTOS Cortex M33 port can be configured to run on the Secure Side only
 * i.e. the processor boots as secure and never jumps to the non-secure side.
 * The Trust Zone support in the port must be disabled in order to run FreeRTOS
 * on the secure side. The following are the valid configuration seetings:
 *
 * 1. Run FreeRTOS on the Secure Side:
 *		configRUN_FREERTOS_SECURE_ONLY = 1 and configENABLE_TRUSTZONE = 0
 *
 * 2. Run FreeRTOS on the Non-Secure Side with Secure Side function call support:
 *		configRUN_FREERTOS_SECURE_ONLY = 0 and configENABLE_TRUSTZONE = 1
 *
 * 3. Run FreeRTOS on the Non-Secure Side only i.e. no Secure Side function call support:
 *		configRUN_FREERTOS_SECURE_ONLY = 0 and configENABLE_TRUSTZONE = 0
 */
#if( ( configRUN_FREERTOS_SECURE_ONLY == 1 ) && ( configENABLE_TRUSTZONE == 1 ) )
	#error TrustZone needs to be disabled in order to run FreeRTOS on the Secure Side.
#endif
/*-----------------------------------------------------------*/

/**
 * @brief Constants required to manipulate the NVIC.
 */
#define portNVIC_SYSTICK_CTRL				( ( volatile uint32_t * ) 0xe000e010 )
#define portNVIC_SYSTICK_LOAD				( ( volatile uint32_t * ) 0xe000e014 )
#define portNVIC_SYSTICK_CURRENT_VALUE		( ( volatile uint32_t * ) 0xe000e018 )
#define portNVIC_INT_CTRL					( ( volatile uint32_t * ) 0xe000ed04 )
#define portNVIC_SYSPRI2					( ( volatile uint32_t * ) 0xe000ed20 )
#define portNVIC_SYSTICK_CLK				( 0x00000004 )
#define portNVIC_SYSTICK_INT				( 0x00000002 )
#define portNVIC_SYSTICK_ENABLE				( 0x00000001 )
#define portNVIC_PENDSVSET					( 0x10000000 )
#define portMIN_INTERRUPT_PRIORITY			( 255UL )
#define portNVIC_PENDSV_PRI					( portMIN_INTERRUPT_PRIORITY << 16UL )
#define portNVIC_SYSTICK_PRI				( portMIN_INTERRUPT_PRIORITY << 24UL )
/*-----------------------------------------------------------*/

/**
 * @brief Constants required to manipulate the SCB.
 */
#define portSCB_SYS_HANDLER_CTRL_STATE_REG	( * ( volatile uint32_t * ) 0xe000ed24 )
#define portSCB_MEM_FAULT_ENABLE			( 1UL << 16UL )
/*-----------------------------------------------------------*/

/**
 * @brief Constants required to manipulate the FPU.
 */
#define portCPACR							( ( volatile uint32_t * ) 0xe000ed88 )	/* Coprocessor Access Control Register. */
#define portCPACR_CP10_VALUE				( 3UL )
#define portCPACR_CP11_VALUE				portCPACR_CP10_VALUE
#define portCPACR_CP10_POS					( 20UL )
#define portCPACR_CP11_POS					( 22UL )

#define portFPCCR							( ( volatile uint32_t * ) 0xe000ef34 )	/* Floating Point Context Control Register. */
#define portFPCCR_ASPEN_POS					( 31UL )
#define portFPCCR_ASPEN_MASK				( 1UL << portFPCCR_ASPEN_POS )
#define portFPCCR_LSPEN_POS					( 30UL )
#define portFPCCR_LSPEN_MASK				( 1UL << portFPCCR_LSPEN_POS )
/*-----------------------------------------------------------*/

/**
 * @brief Constants required to manipulate the MPU.
 */
#define portMPU_TYPE_REG					( * ( ( volatile uint32_t * ) 0xe000ed90 ) )
#define portMPU_CTRL_REG					( * ( ( volatile uint32_t * ) 0xe000ed94 ) )
#define portMPU_RNR_REG						( * ( ( volatile uint32_t * ) 0xe000ed98 ) )

#define portMPU_RBAR_REG					( * ( ( volatile uint32_t * ) 0xe000ed9c ) )
#define portMPU_RLAR_REG					( * ( ( volatile uint32_t * ) 0xe000eda0 ) )

#define portMPU_RBAR_A1_REG					( * ( ( volatile uint32_t * ) 0xe000eda4 ) )
#define portMPU_RLAR_A1_REG					( * ( ( volatile uint32_t * ) 0xe000eda8 ) )

#define portMPU_RBAR_A2_REG					( * ( ( volatile uint32_t * ) 0xe000edac ) )
#define portMPU_RLAR_A2_REG					( * ( ( volatile uint32_t * ) 0xe000edb0 ) )

#define portMPU_RBAR_A3_REG					( * ( ( volatile uint32_t * ) 0xe000edb4 ) )
#define portMPU_RLAR_A3_REG					( * ( ( volatile uint32_t * ) 0xe000edb8 ) )

#define portMPU_MAIR0_REG					( * ( ( volatile uint32_t * ) 0xe000edc0 ) )
#define portMPU_MAIR1_REG					( * ( ( volatile uint32_t * ) 0xe000edc4 ) )

#define portMPU_RBAR_ADDRESS_MASK			( 0xffffffe0 ) /* Must be 32-byte aligned. */
#define portMPU_RLAR_ADDRESS_MASK			( 0xffffffe0 ) /* Must be 32-byte aligned. */

#define portMPU_MAIR_ATTR0_POS				( 0UL )
#define portMPU_MAIR_ATTR0_MASK				( 0x000000ff )

#define portMPU_MAIR_ATTR1_POS				( 8UL )
#define portMPU_MAIR_ATTR1_MASK				( 0x0000ff00 )

#define portMPU_MAIR_ATTR2_POS				( 16UL )
#define portMPU_MAIR_ATTR2_MASK				( 0x00ff0000 )

#define portMPU_MAIR_ATTR3_POS				( 24UL )
#define portMPU_MAIR_ATTR3_MASK				( 0xff000000 )

#define portMPU_MAIR_ATTR4_POS				( 0UL )
#define portMPU_MAIR_ATTR4_MASK				( 0x000000ff )

#define portMPU_MAIR_ATTR5_POS				( 8UL )
#define portMPU_MAIR_ATTR5_MASK				( 0x0000ff00 )

#define portMPU_MAIR_ATTR6_POS				( 16UL )
#define portMPU_MAIR_ATTR6_MASK				( 0x00ff0000 )

#define portMPU_MAIR_ATTR7_POS				( 24UL )
#define portMPU_MAIR_ATTR7_MASK				( 0xff000000 )

#define portMPU_RLAR_ATTR_INDEX0			( 0UL << 1UL )
#define portMPU_RLAR_ATTR_INDEX1			( 1UL << 1UL )
#define portMPU_RLAR_ATTR_INDEX2			( 2UL << 1UL )
#define portMPU_RLAR_ATTR_INDEX3			( 3UL << 1UL )
#define portMPU_RLAR_ATTR_INDEX4			( 4UL << 1UL )
#define portMPU_RLAR_ATTR_INDEX5			( 5UL << 1UL )
#define portMPU_RLAR_ATTR_INDEX6			( 6UL << 1UL )
#define portMPU_RLAR_ATTR_INDEX7			( 7UL << 1UL )

#define portMPU_RLAR_REGION_ENABLE			( 1UL )

/* Enable privileged access to unmapped region. */
#define portMPU_PRIV_BACKGROUND_ENABLE		( 1UL << 2UL )

/* Enable MPU. */
#define portMPU_ENABLE						( 1UL << 0UL )

/* Expected value of the portMPU_TYPE register. */
#define portEXPECTED_MPU_TYPE_VALUE			( 8UL << 8UL ) /* 8 regions, unified. */
/*-----------------------------------------------------------*/

/**
 * @brief Constants required to set up the initial stack.
 */
#define portINITIAL_XPSR					( 0x01000000 )

#if( configRUN_FREERTOS_SECURE_ONLY == 1 )
	/**
	 * @brief Initial EXC_RETURN value.
	 *
	 *     FF         FF         FF         FD
	 * 1111 1111  1111 1111  1111 1111  1111 1101
	 *
	 * Bit[6] - 1 --> The exception was taken from the Secure state.
	 * Bit[5] - 1 --> Do not skip stacking of additional state context.
	 * Bit[4] - 1 --> The PE did not allocate space on the stack for FP context.
	 * Bit[3] - 1 --> Return to the Thread mode.
	 * Bit[2] - 1 --> Restore registers from the process stack.
	 * Bit[1] - 0 --> Reserved, 0.
	 * Bit[0] - 1 --> The exception was taken to the Secure state.
	 */
	#define portINITIAL_EXC_RETURN			( 0xfffffffd )
#else
	/**
	 * @brief Initial EXC_RETURN value.
	 *
	 *     FF         FF         FF         BC
	 * 1111 1111  1111 1111  1111 1111  1011 1100
	 *
	 * Bit[6] - 0 --> The exception was taken from the Non-Secure state.
	 * Bit[5] - 1 --> Do not skip stacking of additional state context.
	 * Bit[4] - 1 --> The PE did not allocate space on the stack for FP context.
	 * Bit[3] - 1 --> Return to the Thread mode.
	 * Bit[2] - 1 --> Restore registers from the process stack.
	 * Bit[1] - 0 --> Reserved, 0.
	 * Bit[0] - 0 --> The exception was taken to the Non-Secure state.
	 */
	#define portINITIAL_EXC_RETURN			( 0xffffffbc )
#endif /* configRUN_FREERTOS_SECURE_ONLY */

/**
 * @brief CONTROL register privileged bit mask.
 *
 * Bit[0] in CONTROL register tells the privilege:
 *  Bit[0] = 0 ==> The task is privileged.
 *  Bit[0] = 1 ==> The task is not privileged.
 */
#define portCONTROL_PRIVILEGED_MASK			( 1UL << 0UL )

/**
 * @brief Initial CONTROL register values.
 */
#define portINITIAL_CONTROL_UNPRIVILEGED	( 0x3 )
#define portINITIAL_CONTROL_PRIVILEGED		( 0x2 )

/**
 * @brief Let the user override the pre-loading of the initial LR with the
 * address of prvTaskExitError() in case it messes up unwinding of the stack
 * in the debugger.
 */
#ifdef configTASK_RETURN_ADDRESS
	#define portTASK_RETURN_ADDRESS			configTASK_RETURN_ADDRESS
#else
	#define portTASK_RETURN_ADDRESS			prvTaskExitError
#endif

/**
 * @brief If portPRELOAD_REGISTERS then registers will be given an initial value
 * when a task is created. This helps in debugging at the cost of code size.
 */
#define portPRELOAD_REGISTERS				1

/**
 * @brief A task is created without a secure context, and must call
 * portALLOCATE_SECURE_CONTEXT() to give itself a secure context before it makes
 * any secure calls.
 */
#define portNO_SECURE_CONTEXT				0
/*-----------------------------------------------------------*/

/**
 * @brief Setup the timer to generate the tick interrupts.
 */
static void prvSetupTimerInterrupt( void ) PRIVILEGED_FUNCTION;

/**
 * @brief Used to catch tasks that attempt to return from their implementing
 * function.
 */
static void prvTaskExitError( void );

#if( configENABLE_MPU == 1 )
	/**
	 * @brief Setup the Memory Protection Unit (MPU).
	 */
	static void prvSetupMPU( void ) PRIVILEGED_FUNCTION;
#endif /* configENABLE_MPU */

#if( configENABLE_FPU == 1 )
	/**
	 * @brief Setup the Floating Point Unit (FPU).
	 */
	static void prvSetupFPU( void ) PRIVILEGED_FUNCTION;
#endif /* configENABLE_FPU */

/**
 * @brief Yield the processor.
 */
void vPortYield( void ) PRIVILEGED_FUNCTION;

/**
 * @brief Enter critical section.
 */
void vPortEnterCritical( void ) PRIVILEGED_FUNCTION;

/**
 * @brief Exit from critical section.
 */
void vPortExitCritical( void ) PRIVILEGED_FUNCTION;

/**
 * @brief SysTick handler.
 */
void SysTick_Handler( void ) PRIVILEGED_FUNCTION;

/**
 * @brief C part of SVC handler.
 */
void vPortSVCHandler_C( uint32_t *pulCallerStackAddress ) PRIVILEGED_FUNCTION;
/*-----------------------------------------------------------*/

/**
 * @brief Each task maintains its own interrupt status in the critical nesting
 * variable.
 */
static volatile uint32_t ulCriticalNesting = 0xaaaaaaaaUL;

#if( configENABLE_TRUSTZONE == 1 )
	/**
	 * @brief Saved as part of the task context to indicate which context the
	 * task is using on the secure side.
	 */
	volatile SecureContextHandle_t xSecureContext = portNO_SECURE_CONTEXT;
#endif /* configENABLE_TRUSTZONE */
/*-----------------------------------------------------------*/

static void prvSetupTimerInterrupt( void ) /* PRIVILEGED_FUNCTION */
{
	/* Stop and reset the SysTick. */
	*( portNVIC_SYSTICK_CTRL ) = 0UL;
	*( portNVIC_SYSTICK_CURRENT_VALUE ) = 0UL;

	/* Configure SysTick to interrupt at the requested rate. */
	*( portNVIC_SYSTICK_LOAD ) = ( configCPU_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
	*( portNVIC_SYSTICK_CTRL ) = portNVIC_SYSTICK_CLK | portNVIC_SYSTICK_INT | portNVIC_SYSTICK_ENABLE;
}
/*-----------------------------------------------------------*/

static void prvTaskExitError( void )
{
volatile uint32_t ulDummy = 0UL;

	/* A function that implements a task must not exit or attempt to return to
	 * its caller as there is nothing to return to. If a task wants to exit it
	 * should instead call vTaskDelete( NULL ). Artificially force an assert()
	 * to be triggered if configASSERT() is defined, then stop here so
	 * application writers can catch the error. */
	configASSERT( ulCriticalNesting == ~0UL );
	portDISABLE_INTERRUPTS();

	while( ulDummy == 0 )
	{
		/* This file calls prvTaskExitError() after the scheduler has been
		 * started to remove a compiler warning about the function being
		 * defined but never called.  ulDummy is used purely to quieten other
		 * warnings about code appearing after this function is called - making
		 * ulDummy volatile makes the compiler think the function could return
		 * and therefore not output an 'unreachable code' warning for code that
		 * appears after it. */
	}
}
/*-----------------------------------------------------------*/

#if( configENABLE_MPU == 1 )
	static void prvSetupMPU( void ) /* PRIVILEGED_FUNCTION */
	{
	#if defined( __ARMCC_VERSION )
		/* Declaration when these variable are defined in code instead of being
		 * exported from linker scripts. */
		extern uint32_t * __privileged_functions_start__;
		extern uint32_t * __privileged_functions_end__;
		extern uint32_t * __syscalls_flash_start__;
		extern uint32_t * __unprivileged_flash_end__;
		extern uint32_t * __privileged_sram_start__;
		extern uint32_t * __privileged_sram_end__;
	#else
		/* Declaration when these variable are exported from linker scripts. */
		extern uint32_t __privileged_functions_start__[];
		extern uint32_t __privileged_functions_end__[];
		extern uint32_t __syscalls_flash_start__[];
		extern uint32_t __unprivileged_flash_end__[];
		extern uint32_t __privileged_sram_start__[];
		extern uint32_t __privileged_sram_end__[];
	#endif /* defined( __ARMCC_VERSION ) */

		/* Check that the MPU is present. */
		if( portMPU_TYPE_REG == portEXPECTED_MPU_TYPE_VALUE )
		{
			/* MAIR0 - Index 0. */
			portMPU_MAIR0_REG |= ( ( portMPU_NORMAL_MEMORY_BUFFERABLE_CACHEABLE << portMPU_MAIR_ATTR0_POS ) & portMPU_MAIR_ATTR0_MASK );
			/* MAIR0 - Index 1. */
			portMPU_MAIR0_REG |= ( ( portMPU_DEVICE_MEMORY_nGnRE << portMPU_MAIR_ATTR1_POS ) & portMPU_MAIR_ATTR1_MASK );

			/* Setup privileged flash as Read Only so that privileged tasks can
			 * read it but not modify. */
			portMPU_RNR_REG = portPRIVILEGED_FLASH_REGION;
			portMPU_RBAR_REG =	( ( ( uint32_t ) __privileged_functions_start__ ) & portMPU_RBAR_ADDRESS_MASK ) |
								( portMPU_REGION_NON_SHAREABLE ) |
								( portMPU_REGION_PRIVILEGED_READ_ONLY );
			portMPU_RLAR_REG =	( ( ( uint32_t ) __privileged_functions_end__ ) & portMPU_RLAR_ADDRESS_MASK ) |
								( portMPU_RLAR_ATTR_INDEX0 ) |
								( portMPU_RLAR_REGION_ENABLE );

			/* Setup unprivileged flash and system calls flash as Read Only by
			 * both privileged and unprivileged tasks. All tasks can read it but
			 * no-one can modify. */
			portMPU_RNR_REG = portUNPRIVILEGED_FLASH_REGION;
			portMPU_RBAR_REG =	( ( ( uint32_t ) __syscalls_flash_start__ ) & portMPU_RBAR_ADDRESS_MASK ) |
								( portMPU_REGION_NON_SHAREABLE ) |
								( portMPU_REGION_READ_ONLY );
			portMPU_RLAR_REG =	( ( ( uint32_t ) __unprivileged_flash_end__ ) & portMPU_RLAR_ADDRESS_MASK ) |
								( portMPU_RLAR_ATTR_INDEX0 ) |
								( portMPU_RLAR_REGION_ENABLE );

			/* Setup RAM containing kernel data for privileged access only. */
			portMPU_RNR_REG = portPRIVILEGED_RAM_REGION;
			portMPU_RBAR_REG =	( ( ( uint32_t ) __privileged_sram_start__ ) & portMPU_RBAR_ADDRESS_MASK ) |
								( portMPU_REGION_NON_SHAREABLE ) |
								( portMPU_REGION_PRIVILEGED_READ_WRITE ) |
								( portMPU_REGION_EXECUTE_NEVER );
			portMPU_RLAR_REG =	( ( ( uint32_t ) __privileged_sram_end__ ) & portMPU_RLAR_ADDRESS_MASK ) |
								( portMPU_RLAR_ATTR_INDEX0 ) |
								( portMPU_RLAR_REGION_ENABLE );

			/* By default allow everything to access the general peripherals.
			 * The system peripherals and registers are protected. */
			portMPU_RNR_REG = portUNPRIVILEGED_DEVICE_REGION;
			portMPU_RBAR_REG =	( ( ( uint32_t ) portDEVICE_REGION_START_ADDRESS ) & portMPU_RBAR_ADDRESS_MASK ) |
								( portMPU_REGION_NON_SHAREABLE ) |
								( portMPU_REGION_READ_WRITE ) |
								( portMPU_REGION_EXECUTE_NEVER );
			portMPU_RLAR_REG =	( ( ( uint32_t ) portDEVICE_REGION_END_ADDRESS ) & portMPU_RLAR_ADDRESS_MASK ) |
								( portMPU_RLAR_ATTR_INDEX1 ) |
								( portMPU_RLAR_REGION_ENABLE );

			/* Enable mem fault. */
			portSCB_SYS_HANDLER_CTRL_STATE_REG |= portSCB_MEM_FAULT_ENABLE;

			/* Enable MPU with privileged background access i.e. unmapped
			 * regions have privileged access. */
			portMPU_CTRL_REG |= ( portMPU_PRIV_BACKGROUND_ENABLE | portMPU_ENABLE );
		}
	}
#endif /* configENABLE_MPU */
/*-----------------------------------------------------------*/

#if( configENABLE_FPU == 1 )
	static void prvSetupFPU( void ) /* PRIVILEGED_FUNCTION */
	{
		#if( configENABLE_TRUSTZONE == 1 )
		{
			/* Enable non-secure access to the FPU. */
			SecureInit_EnableNSFPUAccess();
		}
		#endif /* configENABLE_TRUSTZONE */

		/* CP10 = 11 ==> Full access to FPU i.e. both privileged and
		 * unprivileged code should be able to access FPU. CP11 should be
		 * programmed to the same value as CP10. */
		*( portCPACR ) |=	(	( portCPACR_CP10_VALUE << portCPACR_CP10_POS ) |
								( portCPACR_CP11_VALUE << portCPACR_CP11_POS )
							);

		/* ASPEN = 1 ==> Hardware should automatically preserve floating point
		 * context on exception entry and restore on exception return.
		 * LSPEN = 1 ==> Enable lazy context save of FP state. */
		*( portFPCCR ) |= ( portFPCCR_ASPEN_MASK | portFPCCR_LSPEN_MASK );
	}
#endif /* configENABLE_FPU */
/*-----------------------------------------------------------*/

void vPortYield( void ) /* PRIVILEGED_FUNCTION */
{
	/* Set a PendSV to request a context switch. */
	*( portNVIC_INT_CTRL ) = portNVIC_PENDSVSET;

	/* Barriers are normally not required but do ensure the code is
	 * completely within the specified behaviour for the architecture. */
	__asm volatile( "dsb" ::: "memory" );
	__asm volatile( "isb" );
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void ) /* PRIVILEGED_FUNCTION */
{
	portDISABLE_INTERRUPTS();
	ulCriticalNesting++;

	/* Barriers are normally not required but do ensure the code is
	 * completely within the specified behaviour for the architecture. */
	__asm volatile( "dsb" ::: "memory" );
	__asm volatile( "isb" );
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void ) /* PRIVILEGED_FUNCTION */
{
	configASSERT( ulCriticalNesting );
	ulCriticalNesting--;

	if( ulCriticalNesting == 0 )
	{
		portENABLE_INTERRUPTS();
	}
}
/*-----------------------------------------------------------*/

void SysTick_Handler( void ) /* PRIVILEGED_FUNCTION */
{
uint32_t ulPreviousMask;

	ulPreviousMask = portSET_INTERRUPT_MASK_FROM_ISR();
	{
		/* Increment the RTOS tick. */
		if( xTaskIncrementTick() != pdFALSE )
		{
			/* Pend a context switch. */
			*( portNVIC_INT_CTRL ) = portNVIC_PENDSVSET;
		}
	}
	portCLEAR_INTERRUPT_MASK_FROM_ISR( ulPreviousMask );
}
/*-----------------------------------------------------------*/

void vPortSVCHandler_C( uint32_t *pulCallerStackAddress ) /* PRIVILEGED_FUNCTION */
{
#if( configENABLE_MPU == 1 )
	#if defined( __ARMCC_VERSION )
		/* Declaration when these variable are defined in code instead of being
		 * exported from linker scripts. */
		extern uint32_t * __syscalls_flash_start__;
		extern uint32_t * __syscalls_flash_end__;
	#else
		/* Declaration when these variable are exported from linker scripts. */
		extern uint32_t __syscalls_flash_start__[];
		extern uint32_t __syscalls_flash_end__[];
	#endif /* defined( __ARMCC_VERSION ) */
#endif /* configENABLE_MPU */

uint32_t ulPC;

#if( configENABLE_TRUSTZONE == 1 )
	uint32_t ulR0;
	#if( configENABLE_MPU == 1 )
		uint32_t ulControl, ulIsTaskPrivileged;
	#endif /* configENABLE_MPU */
#endif /* configENABLE_TRUSTZONE */
uint8_t ucSVCNumber;

	/* Register are stored on the stack in the following order - R0, R1, R2, R3,
	 * R12, LR, PC, xPSR. */
	ulPC = pulCallerStackAddress[ 6 ];
	ucSVCNumber = ( ( uint8_t *) ulPC )[ -2 ];

	switch( ucSVCNumber )
	{
		#if( configENABLE_TRUSTZONE == 1 )
			case portSVC_ALLOCATE_SECURE_CONTEXT:
			{
				/* R0 contains the stack size passed as parameter to the
				 * vPortAllocateSecureContext function. */
				ulR0 = pulCallerStackAddress[ 0 ];

				#if( configENABLE_MPU == 1 )
				{
					/* Read the CONTROL register value. */
					__asm volatile ( "mrs %0, control"  : "=r" ( ulControl ) );

					/* The task that raised the SVC is privileged if Bit[0]
					 * in the CONTROL register is 0. */
					ulIsTaskPrivileged = ( ( ulControl & portCONTROL_PRIVILEGED_MASK ) == 0 );

					/* Allocate and load a context for the secure task. */
					xSecureContext = SecureContext_AllocateContext( ulR0, ulIsTaskPrivileged );
				}
				#else
				{
					/* Allocate and load a context for the secure task. */
					xSecureContext = SecureContext_AllocateContext( ulR0 );
				}
				#endif /* configENABLE_MPU */

				configASSERT( xSecureContext != NULL );
				SecureContext_LoadContext( xSecureContext );
			}
			break;

			case portSVC_FREE_SECURE_CONTEXT:
			{
				/* R0 contains the secure context handle to be freed. */
				ulR0 = pulCallerStackAddress[ 0 ];

				/* Free the secure context. */
				SecureContext_FreeContext( ( SecureContextHandle_t ) ulR0 );
			}
			break;
		#endif /* configENABLE_TRUSTZONE */

		case portSVC_START_SCHEDULER:
		{
			#if( configENABLE_TRUSTZONE == 1 )
			{
				/* De-prioritize the non-secure exceptions so that the
				 * non-secure pendSV runs at the lowest priority. */
				SecureInit_DePrioritizeNSExceptions();

				/* Initialize the secure context management system. */
				SecureContext_Init();
			}
			#endif /* configENABLE_TRUSTZONE */

			#if( configENABLE_FPU == 1 )
			{
				/* Setup the Floating Point Unit (FPU). */
				prvSetupFPU();
			}
			#endif /* configENABLE_FPU */

			/* Setup the context of the first task so that the first task starts
			 * executing. */
			vRestoreContextOfFirstTask();
		}
		break;

		#if( configENABLE_MPU == 1 )
			case portSVC_RAISE_PRIVILEGE:
			{
				/* Only raise the privilege, if the svc was raised from any of
				 * the system calls. */
				if( ulPC >= ( uint32_t ) __syscalls_flash_start__ &&
					ulPC <= ( uint32_t ) __syscalls_flash_end__ )
				{
					vRaisePrivilege();
				}
			}
			break;
		#endif /* configENABLE_MPU */

		default:
		{
			/* Incorrect SVC call. */
			configASSERT( pdFALSE );
		}
	}
}
/*-----------------------------------------------------------*/

#if( configENABLE_MPU == 1 )
	StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, StackType_t *pxEndOfStack, TaskFunction_t pxCode, void *pvParameters, BaseType_t xRunPrivileged ) /* PRIVILEGED_FUNCTION */
#else
	StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, StackType_t *pxEndOfStack, TaskFunction_t pxCode, void *pvParameters ) /* PRIVILEGED_FUNCTION */
#endif /* configENABLE_MPU */
{
	/* Simulate the stack frame as it would be created by a context switch
	 * interrupt. */
	#if( portPRELOAD_REGISTERS == 0 )
	{
		pxTopOfStack--; /* Offset added to account for the way the MCU uses the stack on entry/exit of interrupts. */
		*pxTopOfStack = portINITIAL_XPSR;							/* xPSR */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) pxCode;						/* PC */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) portTASK_RETURN_ADDRESS;	/* LR */
		pxTopOfStack -= 5;											/* R12, R3, R2 and R1. */
		*pxTopOfStack = ( StackType_t ) pvParameters;				/* R0 */
		pxTopOfStack -= 9;											/* R11..R4, EXC_RETURN. */
		*pxTopOfStack = portINITIAL_EXC_RETURN;

		#if( configENABLE_MPU == 1 )
		{
			pxTopOfStack--;
			if( xRunPrivileged == pdTRUE )
			{
				*pxTopOfStack = portINITIAL_CONTROL_PRIVILEGED;		/* Slot used to hold this task's CONTROL value. */
			}
			else
			{
				*pxTopOfStack = portINITIAL_CONTROL_UNPRIVILEGED;	/* Slot used to hold this task's CONTROL value. */
			}
		}
		#endif /* configENABLE_MPU */

		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) pxEndOfStack;	/* Slot used to hold this task's PSPLIM value. */

		#if( configENABLE_TRUSTZONE == 1 )
		{
			pxTopOfStack--;
			*pxTopOfStack = portNO_SECURE_CONTEXT;		/* Slot used to hold this task's xSecureContext value. */
		}
		#endif /* configENABLE_TRUSTZONE */
	}
	#else /* portPRELOAD_REGISTERS */
	{
		pxTopOfStack--; /* Offset added to account for the way the MCU uses the stack on entry/exit of interrupts. */
		*pxTopOfStack = portINITIAL_XPSR;							/* xPSR */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) pxCode;						/* PC */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) portTASK_RETURN_ADDRESS;	/* LR */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x12121212UL;				/* R12 */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x03030303UL;				/* R3 */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x02020202UL;				/* R2 */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x01010101UL;				/* R1 */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) pvParameters;				/* R0 */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x11111111UL;				/* R11 */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x10101010UL;				/* R10 */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x09090909UL;				/* R09 */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x08080808UL;				/* R08 */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x07070707UL;				/* R07 */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x06060606UL;				/* R06 */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x05050505UL;				/* R05 */
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x04040404UL;				/* R04 */
		pxTopOfStack--;
		*pxTopOfStack = portINITIAL_EXC_RETURN;						/* EXC_RETURN */

		#if( configENABLE_MPU == 1 )
		{
			pxTopOfStack--;
			if( xRunPrivileged == pdTRUE )
			{
				*pxTopOfStack = portINITIAL_CONTROL_PRIVILEGED;		/* Slot used to hold this task's CONTROL value. */
			}
			else
			{
				*pxTopOfStack = portINITIAL_CONTROL_UNPRIVILEGED;	/* Slot used to hold this task's CONTROL value. */
			}
		}
		#endif /* configENABLE_MPU */

		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) pxEndOfStack;	/* Slot used to hold this task's PSPLIM value. */

		#if( configENABLE_TRUSTZONE == 1 )
		{
			pxTopOfStack--;
			*pxTopOfStack = portNO_SECURE_CONTEXT;		/* Slot used to hold this task's xSecureContext value. */
		}
		#endif /* configENABLE_TRUSTZONE */
	}
	#endif /* portPRELOAD_REGISTERS */

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void ) /* PRIVILEGED_FUNCTION */
{
	/* Make PendSV, CallSV and SysTick the same priority as the kernel. */
	*( portNVIC_SYSPRI2 ) |= portNVIC_PENDSV_PRI;
	*( portNVIC_SYSPRI2 ) |= portNVIC_SYSTICK_PRI;

	#if( configENABLE_MPU == 1 )
	{
		/* Setup the Memory Protection Unit (MPU). */
		prvSetupMPU();
	}
	#endif /* configENABLE_MPU */

	/* Start the timer that generates the tick ISR. Interrupts are disabled
	 * here already. */
	prvSetupTimerInterrupt();

	/* Initialize the critical nesting count ready for the first task. */
	ulCriticalNesting = 0;

	/* Start the first task. */
	vStartFirstTask();

	/* Should never get here as the tasks will now be executing. Call the task
	 * exit error function to prevent compiler warnings about a static function
	 * not being called in the case that the application writer overrides this
	 * functionality by defining configTASK_RETURN_ADDRESS. Call
	 * vTaskSwitchContext() so link time optimization does not remove the
	 * symbol. */
	vTaskSwitchContext();
	prvTaskExitError();

	/* Should not get here. */
	return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void ) /* PRIVILEGED_FUNCTION */
{
	/* Not implemented in ports where there is nothing to return to.
	 * Artificially force an assert. */
	configASSERT( ulCriticalNesting == 1000UL );
}
/*-----------------------------------------------------------*/

#if( configENABLE_MPU == 1 )
	void vPortStoreTaskMPUSettings( xMPU_SETTINGS *xMPUSettings, const struct xMEMORY_REGION * const xRegions, StackType_t *pxBottomOfStack, uint32_t ulStackDepth )
	{
	uint32_t ulRegionStartAddress, ulRegionEndAddress, ulRegionNumber;
	int32_t lIndex = 0;

		/* Setup MAIR0. */
		xMPUSettings->ulMAIR0 = ( ( portMPU_NORMAL_MEMORY_BUFFERABLE_CACHEABLE << portMPU_MAIR_ATTR0_POS ) & portMPU_MAIR_ATTR0_MASK );
		xMPUSettings->ulMAIR0 |= ( ( portMPU_DEVICE_MEMORY_nGnRE << portMPU_MAIR_ATTR1_POS ) & portMPU_MAIR_ATTR1_MASK );

		/* This function is called automatically when the task is created - in
		 * which case the stack region parameters will be valid.  At all other
		 * times the stack parameters will not be valid and it is assumed that
		 * the stack region has already been configured. */
		if( ulStackDepth > 0 )
		{
			/* Define the region that allows access to the stack. */
			ulRegionStartAddress = ( ( uint32_t ) pxBottomOfStack ) & portMPU_RBAR_ADDRESS_MASK;
			ulRegionEndAddress = ( uint32_t ) pxBottomOfStack + ( ulStackDepth * ( uint32_t ) sizeof( StackType_t ) ) - 1;
			ulRegionEndAddress  &= portMPU_RLAR_ADDRESS_MASK;

			xMPUSettings->xRegionsSettings[ 0 ].ulRBAR =	( ulRegionStartAddress ) |
															( portMPU_REGION_NON_SHAREABLE ) |
															( portMPU_REGION_READ_WRITE ) |
															( portMPU_REGION_EXECUTE_NEVER );

			xMPUSettings->xRegionsSettings[ 0 ].ulRLAR =	( ulRegionEndAddress ) |
															( portMPU_RLAR_ATTR_INDEX0 ) |
															( portMPU_RLAR_REGION_ENABLE );
		}

		/* User supplied configurable regions. */
		for( ulRegionNumber = 1; ulRegionNumber <= portNUM_CONFIGURABLE_REGIONS; ulRegionNumber++ )
		{
			/* If xRegions is NULL i.e. the task has not specified any MPU
			 * region, the else part ensures that all the configurable MPU
			 * regions are invalidated. */
			if( ( xRegions != NULL ) && ( xRegions[ lIndex ].ulLengthInBytes > 0UL ) )
			{
				/* Translate the generic region definition contained in xRegions
				 * into the ARMv8 specific MPU settings that are then stored in
				 * xMPUSettings. */
				ulRegionStartAddress = ( ( uint32_t ) xRegions[ lIndex ].pvBaseAddress ) & portMPU_RBAR_ADDRESS_MASK;
				ulRegionEndAddress = ( uint32_t ) xRegions[ lIndex ].pvBaseAddress + xRegions[ lIndex ].ulLengthInBytes - 1;
				ulRegionEndAddress  &= portMPU_RLAR_ADDRESS_MASK;

				/* Start address. */
				xMPUSettings->xRegionsSettings[ ulRegionNumber ].ulRBAR =	( ulRegionStartAddress ) |
																			( portMPU_REGION_NON_SHAREABLE );

				/* RO/RW. */
				if( ( xRegions[ lIndex ].ulParameters & tskMPU_REGION_READ_ONLY ) != 0 )
				{
					xMPUSettings->xRegionsSettings[ ulRegionNumber ].ulRBAR |= ( portMPU_REGION_READ_ONLY );
				}
				else
				{
					xMPUSettings->xRegionsSettings[ ulRegionNumber ].ulRBAR |= ( portMPU_REGION_READ_WRITE );
				}

				/* XN. */
				if( ( xRegions[ lIndex ].ulParameters & tskMPU_REGION_EXECUTE_NEVER ) != 0 )
				{
					xMPUSettings->xRegionsSettings[ ulRegionNumber ].ulRBAR |= ( portMPU_REGION_EXECUTE_NEVER );
				}

				/* End Address. */
				xMPUSettings->xRegionsSettings[ ulRegionNumber ].ulRLAR =	( ulRegionEndAddress ) |
																			( portMPU_RLAR_REGION_ENABLE );

				/* Normal memory/ Device memory. */
				if( ( xRegions[ lIndex ].ulParameters & tskMPU_REGION_DEVICE_MEMORY ) != 0 )
				{
					/* Attr1 in MAIR0 is configured as device memory. */
					xMPUSettings->xRegionsSettings[ ulRegionNumber ].ulRLAR |= portMPU_RLAR_ATTR_INDEX1;
				}
				else
				{
					/* Attr1 in MAIR0 is configured as normal memory. */
					xMPUSettings->xRegionsSettings[ ulRegionNumber ].ulRLAR |= portMPU_RLAR_ATTR_INDEX0;
				}
			}
			else
			{
				/* Invalidate the region. */
				xMPUSettings->xRegionsSettings[ ulRegionNumber ].ulRBAR = 0UL;
				xMPUSettings->xRegionsSettings[ ulRegionNumber ].ulRLAR = 0UL;
			}

			lIndex++;
		}
	}
#endif /* configENABLE_MPU */
/*-----------------------------------------------------------*/
