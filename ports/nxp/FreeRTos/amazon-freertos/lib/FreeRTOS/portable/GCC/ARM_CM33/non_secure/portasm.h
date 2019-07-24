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

#ifndef __PORT_ASM_H__
#define __PORT_ASM_H__

/* Scheduler includes. */
#include "FreeRTOS.h"

/* MPU wrappers includes. */
#include "mpu_wrappers.h"

/**
 * @brief Restore the context of the first task so that the first task starts
 * executing.
 */
void vRestoreContextOfFirstTask( void ) __attribute__ (( naked )) PRIVILEGED_FUNCTION;

/**
 * @brief Checks whether or not the processor is privileged.
 *
 * @return 1 if the processor is already privileged, 0 otherwise.
 */
BaseType_t xIsPrivileged( void ) __attribute__ (( naked ));

/**
 * @brief Raises the privilege level by clearing the bit 0 of the CONTROL
 * register.
 *
 * @note This is a privileged function and should only be called from the kenrel
 * code.
 *
 * Bit 0 of the CONTROL register defines the privilege level of Thread Mode.
 *  Bit[0] = 0 --> The processor is running privileged
 *  Bit[0] = 1 --> The processor is running unprivileged.
 */
void vRaisePrivilege( void ) __attribute__ (( naked )) PRIVILEGED_FUNCTION;

/**
 * @brief Lowers the privilege level by setting the bit 0 of the CONTROL
 * register.
 *
 * Bit 0 of the CONTROL register defines the privilege level of Thread Mode.
 *  Bit[0] = 0 --> The processor is running privileged
 *  Bit[0] = 1 --> The processor is running unprivileged.
 */
void vResetPrivilege( void ) __attribute__ (( naked ));

/**
 * @brief Starts the first task.
 */
void vStartFirstTask( void ) __attribute__ (( naked )) PRIVILEGED_FUNCTION;

/**
 * @brief Disables interrupts.
 */
uint32_t ulSetInterruptMaskFromISR( void ) __attribute__(( naked )) PRIVILEGED_FUNCTION;

/**
 * @brief Enables interrupts.
 */
void vClearInterruptMaskFromISR( uint32_t ulMask ) __attribute__(( naked )) PRIVILEGED_FUNCTION;

/**
 * @brief PendSV Exception handler.
 */
void PendSV_Handler( void ) __attribute__ (( naked )) PRIVILEGED_FUNCTION;

/**
 * @brief SVC Handler.
 */
void SVC_Handler( void ) __attribute__ (( naked )) PRIVILEGED_FUNCTION;

/**
 * @brief Allocate a Secure context for the calling task.
 *
 * @param[in] ulSecureStackSize The size of the stack to be allocated on the
 * secure side for the calling task.
 */
void vPortAllocateSecureContext( uint32_t ulSecureStackSize ) __attribute__ (( naked ));

/**
 * @brief Free the task's secure context.
 *
 * @param[in] pulTCB Pointer to the Task Control Block (TCB) of the task.
 */
void vPortFreeSecureContext( uint32_t *pulTCB ) __attribute__ (( naked )) PRIVILEGED_FUNCTION;

#endif /* __PORT_ASM_H__ */
