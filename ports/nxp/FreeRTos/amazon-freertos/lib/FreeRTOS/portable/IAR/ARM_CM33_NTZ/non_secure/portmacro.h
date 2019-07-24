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

#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the given hardware
 * and compiler.
 *
 * These settings should not be altered.
 *------------------------------------------------------------------------------
 */

#ifndef configENABLE_FPU
	#error configENABLE_FPU must be defined in FreeRTOSConfig.h.  Set configENABLE_FPU to 1 to enable the FPU or 0 to disable the FPU.
#endif /* configENABLE_FPU */

#ifndef configENABLE_MPU
	#error configENABLE_MPU must be defined in FreeRTOSConfig.h.  Set configENABLE_MPU to 1 to enable the MPU or 0 to disable the MPU.
#endif /* configENABLE_MPU */

#ifndef configENABLE_TRUSTZONE
	#error configENABLE_TRUSTZONE must be defined in FreeRTOSConfig.h.  Set configENABLE_TRUSTZONE to 1 to enable TrustZone or 0 to disable TrustZone.
#endif /* configENABLE_TRUSTZONE */

/*-----------------------------------------------------------*/

/**
 * @brief Type definitions.
 */
#define portCHAR											char
#define portFLOAT											float
#define portDOUBLE											double
#define portLONG											long
#define portSHORT											short
#define portSTACK_TYPE										uint32_t
#define portBASE_TYPE										long

typedef portSTACK_TYPE										StackType_t;
typedef long												BaseType_t;
typedef unsigned long										UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t )					0xffff
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t )					0xffffffffUL

	/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
	 * not need to be guarded with a critical section. */
	#define portTICK_TYPE_IS_ATOMIC							1
#endif
/*-----------------------------------------------------------*/

/**
 * Architecture specifics.
 */
#define portSTACK_GROWTH									( -1 )
#define portTICK_PERIOD_MS									( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT									8
#define portNOP()
#define portINLINE											__inline
#ifndef portFORCE_INLINE
	#define portFORCE_INLINE								inline __attribute__(( always_inline ))
#endif
#define portHAS_STACK_OVERFLOW_CHECKING						1
/*-----------------------------------------------------------*/

/**
 * @brief Extern declarations.
 */
extern void vPortYield( void ) /* PRIVILEGED_FUNCTION */;

extern void vPortEnterCritical( void ) /* PRIVILEGED_FUNCTION */;
extern void vPortExitCritical( void ) /* PRIVILEGED_FUNCTION */;

extern uint32_t ulSetInterruptMaskFromISR( void ) /* __attribute__(( naked )) PRIVILEGED_FUNCTION */;
extern void vClearInterruptMaskFromISR( uint32_t ulMask ) /* __attribute__(( naked )) PRIVILEGED_FUNCTION */;

#if( configENABLE_TRUSTZONE == 1 )
	extern void vPortAllocateSecureContext( uint32_t ulSecureStackSize );
	extern void vPortFreeSecureContext( uint32_t *pulTCB ) /* PRIVILEGED_FUNCTION */;
#endif /* configENABLE_TRUSTZONE */

#if( configENABLE_MPU == 1 )
	extern BaseType_t xIsPrivileged( void ) /* __attribute__ (( naked )) */;
	extern void vResetPrivilege( void ) /* __attribute__ (( naked )) */;
#endif /* configENABLE_MPU */
/*-----------------------------------------------------------*/

/**
 * @brief MPU specific constants.
 */
#if( configENABLE_MPU == 1 )
	#define portUSING_MPU_WRAPPERS							1
	#define portPRIVILEGE_BIT								( 0x80000000UL )
#else
	#define portPRIVILEGE_BIT								( 0x0UL )
#endif /* configENABLE_MPU */


/* MPU regions. */
#define portPRIVILEGED_FLASH_REGION							( 0UL )
#define portUNPRIVILEGED_FLASH_REGION						( 1UL )
#define portPRIVILEGED_RAM_REGION							( 2UL )
#define portUNPRIVILEGED_DEVICE_REGION						( 3UL )
#define portSTACK_REGION									( 4UL )
#define portFIRST_CONFIGURABLE_REGION						( 5UL )
#define portLAST_CONFIGURABLE_REGION						( 7UL )
#define portNUM_CONFIGURABLE_REGIONS						( ( portLAST_CONFIGURABLE_REGION - portFIRST_CONFIGURABLE_REGION ) + 1 )
#define portTOTAL_NUM_REGIONS								( portNUM_CONFIGURABLE_REGIONS + 1 ) /* Plus one to make space for the stack region. */

/* Devices Region. */
#define portDEVICE_REGION_START_ADDRESS						( 0x50000000 )
#define portDEVICE_REGION_END_ADDRESS						( 0x5FFFFFFF )

/* Device memory attributes used in MPU_MAIR registers.
 *
 * 8-bit values encoded as follows:
 *  Bit[7:4] - 0000 - Device Memory
 *  Bit[3:2] - 00 --> Device-nGnRnE
 *				01 --> Device-nGnRE
 *				10 --> Device-nGRE
 *				11 --> Device-GRE
 *  Bit[1:0] - 00, Reserved.
 */
#define portMPU_DEVICE_MEMORY_nGnRnE						( 0x00 ) /* 0000 0000 */
#define portMPU_DEVICE_MEMORY_nGnRE							( 0x04 ) /* 0000 0100 */
#define portMPU_DEVICE_MEMORY_nGRE							( 0x08 ) /* 0000 1000 */
#define portMPU_DEVICE_MEMORY_GRE							( 0x0C ) /* 0000 1100 */

/* Normal memory attributes used in MPU_MAIR registers. */
#define portMPU_NORMAL_MEMORY_NON_CACHEABLE					( 0x44 ) /* Non-cacheable. */
#define portMPU_NORMAL_MEMORY_BUFFERABLE_CACHEABLE			( 0xFF ) /* Non-Transient, Write-back, Read-Allocate and Write-Allocate. */

/* Attributes used in MPU_RBAR registers. */
#define portMPU_REGION_NON_SHAREABLE						( 0UL << 3UL )
#define portMPU_REGION_INNER_SHAREABLE						( 1UL << 3UL )
#define portMPU_REGION_OUTER_SHAREABLE						( 2UL << 3UL )

#define portMPU_REGION_PRIVILEGED_READ_WRITE				( 0UL << 1UL )
#define portMPU_REGION_READ_WRITE							( 1UL << 1UL )
#define portMPU_REGION_PRIVILEGED_READ_ONLY					( 2UL << 1UL )
#define portMPU_REGION_READ_ONLY							( 3UL << 1UL )

#define portMPU_REGION_EXECUTE_NEVER						( 1UL )
/*-----------------------------------------------------------*/

/**
 * @brief Settings to define an MPU region.
 */
typedef struct MPURegionSettings
{
	uint32_t ulRBAR;	/**< RBAR for the region. */
	uint32_t ulRLAR;	/**< RLAR for the region. */
} MPURegionSettings_t;

/**
 * @brief MPU settings as stored in the TCB.
 */
typedef struct MPU_SETTINGS
{
	uint32_t ulMAIR0;	/**< MAIR0 for the task containing attributes for all the 4 per task regions. */
	MPURegionSettings_t xRegionsSettings[ portTOTAL_NUM_REGIONS ]; /**< Settings for 4 per task regions. */
} xMPU_SETTINGS;
/*-----------------------------------------------------------*/

/**
 * @brief SVC numbers.
 */
#define portSVC_ALLOCATE_SECURE_CONTEXT						0
#define portSVC_FREE_SECURE_CONTEXT							1
#define portSVC_START_SCHEDULER								2
#define portSVC_RAISE_PRIVILEGE								3
/*-----------------------------------------------------------*/

/**
 * @brief Scheduler utilities.
 */
#define portYIELD()											vPortYield()
#define portNVIC_INT_CTRL_REG								( * ( ( volatile uint32_t * ) 0xe000ed04 ) )
#define portNVIC_PENDSVSET_BIT								( 1UL << 28UL )
#define portEND_SWITCHING_ISR( xSwitchRequired )			if( xSwitchRequired ) portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT
#define portYIELD_FROM_ISR( x )								portEND_SWITCHING_ISR( x )
/*-----------------------------------------------------------*/

/**
 * @brief Critical section management.
 */
#define portSET_INTERRUPT_MASK_FROM_ISR()					ulSetInterruptMaskFromISR()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)				vClearInterruptMaskFromISR( x )
#define portDISABLE_INTERRUPTS()							__asm volatile ( " cpsid i " ::: "memory" )
#define portENABLE_INTERRUPTS()								__asm volatile ( " cpsie i " ::: "memory" )
#define portENTER_CRITICAL()								vPortEnterCritical()
#define portEXIT_CRITICAL()									vPortExitCritical()
/*-----------------------------------------------------------*/

/**
 * @brief Task function macros as described on the FreeRTOS.org WEB site.
 */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters )	void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters )		void vFunction( void *pvParameters )
/*-----------------------------------------------------------*/

#if( configENABLE_TRUSTZONE == 1 )
	/**
	 * @brief Allocate a secure context for the task.
	 *
	 * Tasks are not created with a secure context. Any task that is going to call
	 * secure functions must call portALLOCATE_SECURE_CONTEXT() to allocate itself a
	 * secure context before it calls any secure function.
	 *
	 * @param[in] ulSecureStackSize The size of the secure stack to be allocated.
	 */
	#define portALLOCATE_SECURE_CONTEXT( ulSecureStackSize )	vPortAllocateSecureContext( ulSecureStackSize )

	/**
	 * @brief Called when a task is deleted to delete the task's secure context,
	 * if it has one.
	 *
	 * @param[in] pxTCB The TCB of the task being deleted.
	 */
	#define portCLEAN_UP_TCB( pxTCB )							vPortFreeSecureContext( ( uint32_t * ) pxTCB )
#else
	#define portALLOCATE_SECURE_CONTEXT( ulSecureStackSize )
	#define portCLEAN_UP_TCB( pxTCB )
#endif /* configENABLE_TRUSTZONE */
/*-----------------------------------------------------------*/

#if( configENABLE_MPU == 1 )
	/**
	 * @brief Checks whether or not the processor is privileged.
	 *
	 * @return 1 if the processor is already privileged, 0 otherwise.
	 */
	#define portIS_PRIVILEGED()									xIsPrivileged()

	/**
	 * @brief Raise an SVC request to raise privilege.
	 *
	 * The SVC handler checks that the SVC was raised from a system call and only
	 * then it raises the privilege. If this is called from any other place,
	 * the privilege is not raised.
	 */
	#define portRAISE_PRIVILEGE()								__asm volatile ( "svc %0 \n" :: "i" ( portSVC_RAISE_PRIVILEGE ) : "memory" );

	/**
	 * @brief Lowers the privilege level by setting the bit 0 of the CONTROL
	 * register.
	 */
	#define portRESET_PRIVILEGE()								vResetPrivilege()
#else
	#define portIS_PRIVILEGED()
	#define portRAISE_PRIVILEGE()
	#define portRESET_PRIVILEGE()
#endif /* configENABLE_MPU */
/*-----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
