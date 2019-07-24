/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __SERIAL_PORT_INTERNAL_H__
#define __SERIAL_PORT_INTERNAL_H__

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

#if (defined(SERIAL_PORT_TYPE_UART) && (SERIAL_PORT_TYPE_UART > 0U))
serial_manager_status_t Serial_UartInit(serial_handle_t serialHandle, void *serialConfig);
serial_manager_status_t Serial_UartDeinit(serial_handle_t serialHandle);
serial_manager_status_t Serial_UartWrite(serial_handle_t serialHandle, uint8_t *buffer, uint32_t length);
#if !(defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
serial_manager_status_t Serial_UartRead(serial_handle_t serialHandle, uint8_t *buffer, uint32_t length);
#endif

#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
serial_manager_status_t Serial_UartCancelWrite(serial_handle_t serialHandle);
serial_manager_status_t Serial_UartInstallTxCallback(serial_handle_t serialHandle,
                                                     serial_manager_callback_t callback,
                                                     void *callbackParam);
serial_manager_status_t Serial_UartInstallRxCallback(serial_handle_t serialHandle,
                                                     serial_manager_callback_t callback,
                                                     void *callbackParam);
void Serial_UartIsrFunction(serial_handle_t serialHandle);
#endif

#endif

#if (defined(SERIAL_PORT_TYPE_USBCDC) && (SERIAL_PORT_TYPE_USBCDC > 0U))
serial_manager_status_t Serial_UsbCdcInit(serial_handle_t serialHandle, void *config);
serial_manager_status_t Serial_UsbCdcDeinit(serial_handle_t serialHandle);
serial_manager_status_t Serial_UsbCdcWrite(serial_handle_t serialHandle, uint8_t *buffer, uint32_t length);
serial_manager_status_t Serial_UsbCdcRead(serial_handle_t serialHandle, uint8_t *buffer, uint32_t length);
serial_manager_status_t Serial_UsbCdcCancelWrite(serial_handle_t serialHandle);
serial_manager_status_t Serial_UsbCdcInstallTxCallback(serial_handle_t serialHandle,
                                                       serial_manager_callback_t callback,
                                                       void *callbackParam);
serial_manager_status_t Serial_UsbCdcInstallRxCallback(serial_handle_t serialHandle,
                                                       serial_manager_callback_t callback,
                                                       void *callbackParam);
void Serial_UsbCdcIsrFunction(serial_handle_t serialHandle);
#endif

#if (defined(SERIAL_PORT_TYPE_SWO) && (SERIAL_PORT_TYPE_SWO > 0U))
serial_manager_status_t Serial_SwoInit(serial_handle_t serialHandle, void *config);
serial_manager_status_t Serial_SwoDeinit(serial_handle_t serialHandle);
serial_manager_status_t Serial_SwoWrite(serial_handle_t serialHandle, uint8_t *buffer, uint32_t length);
#if !(defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
serial_manager_status_t Serial_SwoRead(serial_handle_t serialHandle, uint8_t *buffer, uint32_t length);
#endif
#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
serial_manager_status_t Serial_SwoCancelWrite(serial_handle_t serialHandle);
serial_manager_status_t Serial_SwoInstallTxCallback(serial_handle_t serialHandle,
                                                    serial_manager_callback_t callback,
                                                    void *callbackParam);
serial_manager_status_t Serial_SwoInstallRxCallback(serial_handle_t serialHandle,
                                                    serial_manager_callback_t callback,
                                                    void *callbackParam);
void Serial_SwoIsrFunction(serial_handle_t serialHandle);
#endif
#endif

#if (defined(SERIAL_PORT_TYPE_USBCDC_VIRTUAL) && (SERIAL_PORT_TYPE_USBCDC_VIRTUAL > 0U))
serial_manager_status_t Serial_UsbCdcVirtualInit(serial_handle_t serialHandle, void *config);
serial_manager_status_t Serial_UsbCdcVirtualDeinit(serial_handle_t serialHandle);
serial_manager_status_t Serial_UsbCdcVirtualWrite(serial_handle_t serialHandle, uint8_t *buffer, uint32_t length);
serial_manager_status_t Serial_UsbCdcVirtualRead(serial_handle_t serialHandle, uint8_t *buffer, uint32_t length);
serial_manager_status_t Serial_UsbCdcVirtualCancelWrite(serial_handle_t serialHandle);
serial_manager_status_t Serial_UsbCdcVirtualInstallTxCallback(serial_handle_t serialHandle,
                                                              serial_manager_callback_t callback,
                                                              void *callbackParam);
serial_manager_status_t Serial_UsbCdcVirtualInstallRxCallback(serial_handle_t serialHandle,
                                                              serial_manager_callback_t callback,
                                                              void *callbackParam);
void Serial_UsbCdcVirtualIsrFunction(serial_handle_t serialHandle);
#endif

#if defined(__cplusplus)
}
#endif

#endif /* __SERIAL_PORT_INTERNAL_H__ */
