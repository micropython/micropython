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

/* Secure context includes. */
#include "secure_context.h"

/* Secure heap includes. */
#include "secure_heap.h"

/* Secure port macros. */
#include "secure_port_macros.h"

/**
 * @brief CONTROL value for privileged tasks.
 *
 * Bit[0] - 0 --> Thread mode is privileged.
 * Bit[1] - 1 --> Thread mode uses PSP.
 */
#define securecontextCONTROL_VALUE_PRIVILEGED		0x02

/**
 * @brief CONTROL value for un-privileged tasks.
 *
 * Bit[0] - 1 --> Thread mode is un-privileged.
 * Bit[1] - 1 --> Thread mode uses PSP.
 */
#define securecontextCONTROL_VALUE_UNPRIVILEGED		0x03
/*-----------------------------------------------------------*/

/**
 * @brief Structure to represent secure context.
 *
 * @note Since stack grows down, pucStackStart is the highest address while
 * pucStackLimit is the first addess of the allocated memory.
 */
typedef struct SecureContext
{
	uint8_t *pucCurrentStackPointer;	/**< Current value of stack pointer (PSP). */
	uint8_t *pucStackLimit;				/**< Last location of the stack memory (PSPLIM). */
	uint8_t *pucStackStart;				/**< First location of the stack memory. */
} SecureContext_t;
/*-----------------------------------------------------------*/

secureportNON_SECURE_CALLABLE void SecureContext_Init( void )
{
	uint32_t ulIPSR;

	/* Read the Interrupt Program Status Register (IPSR) value. */
	secureportREAD_IPSR( ulIPSR );

	/* Do nothing if the processor is running in the Thread Mode. IPSR is zero
	 * when the processor is running in the Thread Mode. */
	if( ulIPSR != 0 )
	{
		/* No stack for thread mode until a task's context is loaded. */
		secureportSET_PSPLIM( securecontextNO_STACK );
		secureportSET_PSP( securecontextNO_STACK );

		#if( configENABLE_MPU == 1 )
		{
			/* Configure thread mode to use PSP and to be unprivileged. */
			secureportSET_CONTROL( securecontextCONTROL_VALUE_UNPRIVILEGED );
		}
		#else /* configENABLE_MPU */
		{
			/* Configure thread mode to use PSP and to be privileged.. */
			secureportSET_CONTROL( securecontextCONTROL_VALUE_PRIVILEGED );
		}
		#endif /* configENABLE_MPU */
	}
}
/*-----------------------------------------------------------*/

#if( configENABLE_MPU == 1 )
	secureportNON_SECURE_CALLABLE SecureContextHandle_t SecureContext_AllocateContext( uint32_t ulSecureStackSize, uint32_t ulIsTaskPrivileged )
#else /* configENABLE_MPU */
	secureportNON_SECURE_CALLABLE SecureContextHandle_t SecureContext_AllocateContext( uint32_t ulSecureStackSize )
#endif /* configENABLE_MPU */
{
	uint8_t *pucStackMemory = NULL;
	uint32_t ulIPSR;
	SecureContextHandle_t xSecureContextHandle = NULL;
	#if( configENABLE_MPU == 1 )
		uint32_t *pulCurrentStackPointer = NULL;
	#endif /* configENABLE_MPU */

	/* Read the Interrupt Program Status Register (IPSR) value. */
	secureportREAD_IPSR( ulIPSR );

	/* Do nothing if the processor is running in the Thread Mode. IPSR is zero
	 * when the processor is running in the Thread Mode. */
	if( ulIPSR != 0 )
	{
		/* Allocate the context structure. */
		xSecureContextHandle = ( SecureContextHandle_t ) pvPortMalloc( sizeof( SecureContext_t ) );

		if( xSecureContextHandle != NULL )
		{
			/* Allocate the stack space. */
			pucStackMemory = pvPortMalloc( ulSecureStackSize );

			if( pucStackMemory != NULL )
			{
				/* Since stack grows down, the starting point will be the last
				 * location. Note that this location is next to the last
				 * allocated byte because the hardware decrements the stack
				 * pointer before writing i.e. if stack pointer is 0x2, a push
				 * operation will decrement the stack pointer to 0x1 and then
				 * write at 0x1. */
				xSecureContextHandle->pucStackStart = pucStackMemory + ulSecureStackSize;

				/* The stack cannot go beyond this location. This value is
				 * programmed in the PSPLIM register on context switch.*/
				xSecureContextHandle->pucStackLimit = pucStackMemory;

				#if( configENABLE_MPU == 1 )
				{
					/* Store the correct CONTROL value for the task on the stack.
					 * This value is programmed in the CONTROL register on
					 * context switch. */
					pulCurrentStackPointer = ( uint32_t * ) xSecureContextHandle->pucStackStart;
					pulCurrentStackPointer--;
					if( ulIsTaskPrivileged )
					{
						*( pulCurrentStackPointer ) = securecontextCONTROL_VALUE_PRIVILEGED;
					}
					else
					{
						*( pulCurrentStackPointer ) = securecontextCONTROL_VALUE_UNPRIVILEGED;
					}

					/* Store the current stack pointer. This value is programmed in
					 * the PSP register on context switch. */
					xSecureContextHandle->pucCurrentStackPointer = ( uint8_t * ) pulCurrentStackPointer;
				}
				#else /* configENABLE_MPU */
				{
					/* Current SP is set to the starting of the stack. This
					 * value programmed in the PSP register on context switch. */
					xSecureContextHandle->pucCurrentStackPointer = xSecureContextHandle->pucStackStart;

				}
				#endif /* configENABLE_MPU */
			}
			else
			{
				/* Free the context to avoid memory leak and make sure to return
				 * NULL to indicate failure. */
				vPortFree( xSecureContextHandle );
				xSecureContextHandle = NULL;
			}
		}
	}

	return xSecureContextHandle;
}
/*-----------------------------------------------------------*/

secureportNON_SECURE_CALLABLE void SecureContext_FreeContext( SecureContextHandle_t xSecureContextHandle )
{
	uint32_t ulIPSR;

	/* Read the Interrupt Program Status Register (IPSR) value. */
	secureportREAD_IPSR( ulIPSR );

	/* Do nothing if the processor is running in the Thread Mode. IPSR is zero
	 * when the processor is running in the Thread Mode. */
	if( ulIPSR != 0 )
	{
		/* Ensure that valid parameters are passed. */
		secureportASSERT( xSecureContextHandle != NULL );

		/* Free the stack space. */
		vPortFree( xSecureContextHandle->pucStackLimit );

		/* Free the context itself. */
		vPortFree( xSecureContextHandle );
	}
}
/*-----------------------------------------------------------*/
