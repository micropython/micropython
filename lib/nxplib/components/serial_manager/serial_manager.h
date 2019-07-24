/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __SERIAL_MANAGER_H__
#define __SERIAL_MANAGER_H__

/*!
 * @addtogroup serialmanager
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifdef DEBUG_CONSOLE_TRANSFER_NON_BLOCKING
/*! @brief Enable or disable serial manager non-blocking mode (1 - enable, 0 - disable) */
#define SERIAL_MANAGER_NON_BLOCKING_MODE (1U)
#else
#ifndef SERIAL_MANAGER_NON_BLOCKING_MODE
#define SERIAL_MANAGER_NON_BLOCKING_MODE (0U)
#endif
#endif

/*! @brief Enable or disable uart port (1 - enable, 0 - disable) */
#ifndef SERIAL_PORT_TYPE_UART
#define SERIAL_PORT_TYPE_UART (1U)
#endif

/*! @brief Enable or disable USB CDC port (1 - enable, 0 - disable) */
#ifndef SERIAL_PORT_TYPE_USBCDC
#define SERIAL_PORT_TYPE_USBCDC (0U)
#endif

/*! @brief Enable or disable SWO port (1 - enable, 0 - disable) */
#ifndef SERIAL_PORT_TYPE_SWO
#define SERIAL_PORT_TYPE_SWO (0U)
#endif

/*! @brief Enable or disable USB CDC virtual port (1 - enable, 0 - disable) */
#ifndef SERIAL_PORT_TYPE_USBCDC_VIRTUAL
#define SERIAL_PORT_TYPE_USBCDC_VIRTUAL (0U)
#endif

/*! @brief Set serial manager write handle size */
#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
#define SERIAL_MANAGER_WRITE_HANDLE_SIZE (44U)
#define SERIAL_MANAGER_READ_HANDLE_SIZE (44U)
#else
#define SERIAL_MANAGER_WRITE_HANDLE_SIZE (4U)
#define SERIAL_MANAGER_READ_HANDLE_SIZE (4U)
#endif

#if (defined(SERIAL_PORT_TYPE_UART) && (SERIAL_PORT_TYPE_UART > 0U))
#include "serial_port_uart.h"
#endif

#if (defined(SERIAL_PORT_TYPE_USBCDC) && (SERIAL_PORT_TYPE_USBCDC > 0U))

#if !(defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
#error The serial manager blocking mode cannot be supported for USB CDC.
#endif

#include "serial_port_usb.h"
#endif

#if (defined(SERIAL_PORT_TYPE_SWO) && (SERIAL_PORT_TYPE_SWO > 0U))
#include "serial_port_swo.h"
#endif

#if (defined(SERIAL_PORT_TYPE_USBCDC_VIRTUAL) && (SERIAL_PORT_TYPE_USBCDC_VIRTUAL > 0U))

#if !(defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
#error The serial manager blocking mode cannot be supported for USB CDC.
#endif

#include "serial_port_usb_virtual.h"
#endif

#define SERIAL_MANAGER_HANDLE_SIZE_TEMP 0U
#if (defined(SERIAL_PORT_TYPE_UART) && (SERIAL_PORT_TYPE_UART > 0U))

#if (SERIAL_PORT_UART_HANDLE_SIZE > SERIAL_MANAGER_HANDLE_SIZE_TEMP)
#undef SERIAL_MANAGER_HANDLE_SIZE_TEMP
#define SERIAL_MANAGER_HANDLE_SIZE_TEMP SERIAL_PORT_UART_HANDLE_SIZE
#endif

#endif

#if (defined(SERIAL_PORT_TYPE_USBCDC) && (SERIAL_PORT_TYPE_USBCDC > 0U))

#if (SERIAL_PORT_USB_CDC_HANDLE_SIZE > SERIAL_MANAGER_HANDLE_SIZE_TEMP)
#undef SERIAL_MANAGER_HANDLE_SIZE_TEMP
#define SERIAL_MANAGER_HANDLE_SIZE_TEMP SERIAL_PORT_USB_CDC_HANDLE_SIZE
#endif

#endif

#if (defined(SERIAL_PORT_TYPE_SWO) && (SERIAL_PORT_TYPE_SWO > 0U))

#if (SERIAL_PORT_SWO_HANDLE_SIZE > SERIAL_MANAGER_HANDLE_SIZE_TEMP)
#undef SERIAL_MANAGER_HANDLE_SIZE_TEMP
#define SERIAL_MANAGER_HANDLE_SIZE_TEMP SERIAL_PORT_SWO_HANDLE_SIZE
#endif

#endif

#if (defined(SERIAL_PORT_TYPE_USBCDC_VIRTUAL) && (SERIAL_PORT_TYPE_USBCDC_VIRTUAL > 0U))

#if (SERIAL_PORT_USB_VIRTUAL_HANDLE_SIZE > SERIAL_MANAGER_HANDLE_SIZE_TEMP)
#undef SERIAL_MANAGER_HANDLE_SIZE_TEMP
#define SERIAL_MANAGER_HANDLE_SIZE_TEMP SERIAL_PORT_USB_VIRTUAL_HANDLE_SIZE
#endif

#endif

/*! @brief SERIAL_PORT_UART_HANDLE_SIZE/SERIAL_PORT_USB_CDC_HANDLE_SIZE + serial manager dedicated size */
#if ((defined(SERIAL_MANAGER_HANDLE_SIZE_TEMP) && (SERIAL_MANAGER_HANDLE_SIZE_TEMP > 0U)))
#else
#error SERIAL_PORT_TYPE_UART, SERIAL_PORT_TYPE_USBCDC, SERIAL_PORT_TYPE_SWO and SERIAL_PORT_TYPE_USBCDC_VIRTUAL should not be cleared at same time.
#endif

#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
#define SERIAL_MANAGER_HANDLE_SIZE (SERIAL_MANAGER_HANDLE_SIZE_TEMP + 120U)
#else
#define SERIAL_MANAGER_HANDLE_SIZE (SERIAL_MANAGER_HANDLE_SIZE_TEMP + 12U)
#endif

#define SERIAL_MANAGER_USE_COMMON_TASK (1U)
#define SERIAL_MANAGER_TASK_PRIORITY (2U)
#define SERIAL_MANAGER_TASK_STACK_SIZE (1000U)

typedef void *serial_handle_t;
typedef void *serial_write_handle_t;
typedef void *serial_read_handle_t;

/*! @brief serial port type*/
typedef enum _serial_port_type
{
    kSerialPort_Uart = 1U,     /*!< Serial port UART */
    kSerialPort_UsbCdc,        /*!< Serial port USB CDC */
    kSerialPort_Swo,           /*!< Serial port SWO */
    kSerialPort_UsbCdcVirtual, /*!< Serial port USB CDC Virtual */
} serial_port_type_t;

/*! @brief serial manager config structure*/
typedef struct _serial_manager_config
{
    uint8_t *ringBuffer;     /*!< Ring buffer address, it is used to buffer data received by the hardware.
                                  Besides, the memory space cannot be free during the lifetime of the serial
                                  manager module. */
    uint32_t ringBufferSize; /*!< The size of the ring buffer */
    serial_port_type_t type; /*!< Serial port type */
    void *portConfig;        /*!< Serial port configuration */
} serial_manager_config_t;

/*! @brief serial manager error code*/
typedef enum _serial_manager_status
{
    kStatus_SerialManager_Success = kStatus_Success,                            /*!< Success */
    kStatus_SerialManager_Error   = MAKE_STATUS(kStatusGroup_SERIALMANAGER, 1), /*!< Failed */
    kStatus_SerialManager_Busy    = MAKE_STATUS(kStatusGroup_SERIALMANAGER, 2), /*!< Busy */
    kStatus_SerialManager_Notify  = MAKE_STATUS(kStatusGroup_SERIALMANAGER, 3), /*!< Ring buffer is not empty */
    kStatus_SerialManager_Canceled =
        MAKE_STATUS(kStatusGroup_SERIALMANAGER, 4), /*!< the non-blocking request is canceled */
    kStatus_SerialManager_HandleConflict = MAKE_STATUS(kStatusGroup_SERIALMANAGER, 5), /*!< The handle is opened */
    kStatus_SerialManager_RingBufferOverflow =
        MAKE_STATUS(kStatusGroup_SERIALMANAGER, 6), /*!< The ring buffer is overflowed */
} serial_manager_status_t;

/*! @brief Callback message structure */
typedef struct _serial_manager_callback_message
{
    uint8_t *buffer; /*!< Transferred buffer */
    uint32_t length; /*!< Transferred data length */
} serial_manager_callback_message_t;

/*! @brief callback function */
typedef void (*serial_manager_callback_t)(void *callbackParam,
                                          serial_manager_callback_message_t *message,
                                          serial_manager_status_t status);

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/*!
 * @brief Initializes a serial manager module with the serial manager handle and the user configuration structure.
 *
 * This function configures the Serial Manager module with user-defined settings. The user can configure the
 * configuration
 * structure. The parameter serialHandle is a pointer to point to a memory space of size #SERIAL_MANAGER_HANDLE_SIZE
 * allocated by the caller.
 * The Serial Manager module supports two types of serial port, UART (includes UART, USART, LPSCI, LPUART, etc) and USB
 * CDC.
 * Please refer to #serial_port_type_t for serial port setting. These two types can be set by using
 * #serial_manager_config_t.
 *
 * Example below shows how to use this API to configure the Serial Manager.
 * For UART,
 *  @code
 *   #define SERIAL_MANAGER_RING_BUFFER_SIZE          (256U)
 *   static uint8_t s_serialHandleBuffer[SERIAL_MANAGER_HANDLE_SIZE];
 *   static serial_handle_t s_serialHandle = &s_serialHandleBuffer[0];
 *   static uint8_t s_ringBuffer[SERIAL_MANAGER_RING_BUFFER_SIZE];
 *
 *   serial_manager_config_t config;
 *   serial_port_uart_config_t uartConfig;
 *   config.type = kSerialPort_Uart;
 *   config.ringBuffer = &s_ringBuffer[0];
 *   config.ringBufferSize = SERIAL_MANAGER_RING_BUFFER_SIZE;
 *   uartConfig.instance = 0;
 *   uartConfig.clockRate = 24000000;
 *   uartConfig.baudRate = 115200;
 *   uartConfig.parityMode = kSerialManager_UartParityDisabled;
 *   uartConfig.stopBitCount = kSerialManager_UartOneStopBit;
 *   uartConfig.enableRx = 1;
 *   uartConfig.enableTx = 1;
 *   config.portConfig = &uartConfig;
 *   SerialManager_Init(s_serialHandle, &config);
 *  @endcode
 * For USB CDC,
 *  @code
 *   #define SERIAL_MANAGER_RING_BUFFER_SIZE          (256U)
 *   static uint8_t s_serialHandleBuffer[SERIAL_MANAGER_HANDLE_SIZE];
 *   static serial_handle_t s_serialHandle = &s_serialHandleBuffer[0];
 *   static uint8_t s_ringBuffer[SERIAL_MANAGER_RING_BUFFER_SIZE];
 *
 *   serial_manager_config_t config;
 *   serial_port_usb_cdc_config_t usbCdcConfig;
 *   config.type = kSerialPort_UsbCdc;
 *   config.ringBuffer = &s_ringBuffer[0];
 *   config.ringBufferSize = SERIAL_MANAGER_RING_BUFFER_SIZE;
 *   usbCdcConfig.controllerIndex = kSerialManager_UsbControllerKhci0;
 *   config.portConfig = &usbCdcConfig;
 *   SerialManager_Init(s_serialHandle, &config);
 *  @endcode
 *
 * @param serialHandle Pointer to point to a memory space of size #SERIAL_MANAGER_HANDLE_SIZE allocated by the caller.
 * @param config Pointer to user-defined configuration structure.
 * @retval kStatus_SerialManager_Error An error occurred.
 * @retval kStatus_SerialManager_Success The Serial Manager module initialization succeed.
 */
serial_manager_status_t SerialManager_Init(serial_handle_t serialHandle, serial_manager_config_t *config);

/*!
 * @brief De-initializes the serial manager module instance.
 *
 * This function de-initializes the serial manager module instance. If the opened writing or
 * reading handle is not closed, the function will return kStatus_SerialManager_Busy.
 *
 * @param serialHandle The serial manager module handle pointer.
 * @retval kStatus_SerialManager_Success The serial manager de-initialization succeed.
 * @retval kStatus_SerialManager_Busy Opened reading or writing handle is not closed.
 */
serial_manager_status_t SerialManager_Deinit(serial_handle_t serialHandle);

/*!
 * @brief Opens a writing handle for the serial manager module.
 *
 * This function Opens a writing handle for the serial manager module. If the serial manager needs to
 * be used in different tasks, the task should open a dedicated write handle for itself by calling
 * #SerialManager_OpenWriteHandle. Since there can only one buffer for transmission for the writing
 * handle at the same time, multiple writing handles need to be opened when the multiple transmission
 * is needed for a task.
 *
 * @param serialHandle The serial manager module handle pointer.
 * @param writeHandle The serial manager module writing handle pointer.
 * @retval kStatus_SerialManager_Error An error occurred.
 * @retval kStatus_SerialManager_HandleConflict The writing handle was opened.
 * @retval kStatus_SerialManager_Success The writing handle is opened.
 *
 * Example below shows how to use this API to write data.
 * For task 1,
 *  @code
 *   static uint8_t s_serialWriteHandleBuffer1[SERIAL_MANAGER_WRITE_HANDLE_SIZE];
 *   static serial_write_handle_t s_serialWriteHandle1 = &s_serialWriteHandleBuffer1[0];
 *   static uint8_t s_nonBlockingWelcome1[] = "This is non-blocking writing log for task1!\r\n";
 *   SerialManager_OpenWriteHandle(serialHandle, s_serialWriteHandle1);
 *   SerialManager_InstallTxCallback(s_serialWriteHandle1, Task1_SerialManagerTxCallback, s_serialWriteHandle1);
 *   SerialManager_WriteNonBlocking(s_serialWriteHandle1, s_nonBlockingWelcome1, sizeof(s_nonBlockingWelcome1) - 1);
 *  @endcode
 * For task 2,
 *  @code
 *   static uint8_t s_serialWriteHandleBuffer2[SERIAL_MANAGER_WRITE_HANDLE_SIZE];
 *   static serial_write_handle_t s_serialWriteHandle2 = &s_serialWriteHandleBuffer2[0];
 *   static uint8_t s_nonBlockingWelcome2[] = "This is non-blocking writing log for task2!\r\n";
 *   SerialManager_OpenWriteHandle(serialHandle, s_serialWriteHandle2);
 *   SerialManager_InstallTxCallback(s_serialWriteHandle2, Task2_SerialManagerTxCallback, s_serialWriteHandle2);
 *   SerialManager_WriteNonBlocking(s_serialWriteHandle2, s_nonBlockingWelcome2, sizeof(s_nonBlockingWelcome2) - 1);
 *  @endcode
 */
serial_manager_status_t SerialManager_OpenWriteHandle(serial_handle_t serialHandle, serial_write_handle_t writeHandle);

/*!
 * @brief Closes a writing handle for the serial manager module.
 *
 * This function Closes a writing handle for the serial manager module.
 *
 * @param writeHandle The serial manager module writing handle pointer.
 * @retval kStatus_SerialManager_Success The writing handle is closed.
 */
serial_manager_status_t SerialManager_CloseWriteHandle(serial_write_handle_t writeHandle);

/*!
 * @brief Opens a reading handle for the serial manager module.
 *
 * This function Opens a reading handle for the serial manager module. The reading handle can not be
 * opened multiple at the same time. The error code kStatus_SerialManager_Busy would be returned when
 * the previous reading handle is not closed. And There can only be one buffer for receiving for the
 * reading handle at the same time.
 *
 * @param serialHandle The serial manager module handle pointer.
 * @param readHandle The serial manager module reading handle pointer.
 * @retval kStatus_SerialManager_Error An error occurred.
 * @retval kStatus_SerialManager_Success The reading handle is opened.
 * @retval kStatus_SerialManager_Busy Previous reading handle is not closed.
 *
 * Example below shows how to use this API to read data.
 *  @code
 *   static uint8_t s_serialReadHandleBuffer[SERIAL_MANAGER_READ_HANDLE_SIZE];
 *   static serial_read_handle_t s_serialReadHandle = &s_serialReadHandleBuffer[0];
 *   SerialManager_OpenReadHandle(serialHandle, s_serialReadHandle);
 *   static uint8_t s_nonBlockingBuffer[64];
 *   SerialManager_InstallRxCallback(s_serialReadHandle, APP_SerialManagerRxCallback, s_serialReadHandle);
 *   SerialManager_ReadNonBlocking(s_serialReadHandle, s_nonBlockingBuffer, sizeof(s_nonBlockingBuffer));
 *  @endcode
 */
serial_manager_status_t SerialManager_OpenReadHandle(serial_handle_t serialHandle, serial_read_handle_t readHandle);

/*!
 * @brief Closes a reading for the serial manager module.
 *
 * This function Closes a reading for the serial manager module.
 *
 * @param readHandle The serial manager module reading handle pointer.
 * @retval kStatus_SerialManager_Success The reading handle is closed.
 */
serial_manager_status_t SerialManager_CloseReadHandle(serial_read_handle_t readHandle);

/*!
 * @brief Transmits data with the blocking mode.
 *
 * This is a blocking function, which polls the sending queue, waits for the sending queue to be empty.
 * This function sends data using an interrupt method. The interrupt of the hardware could not be disabled.
 * And There can only one buffer for transmission for the writing handle at the same time.
 *
 * @note The function #SerialManager_WriteBlocking and the function #SerialManager_WriteNonBlocking
 * cannot be used at the same time.
 * And, the function #SerialManager_CancelWriting cannot be used to abort the transmission of this function.
 *
 * @param writeHandle The serial manager module handle pointer.
 * @param buffer Start address of the data to write.
 * @param length Length of the data to write.
 * @retval kStatus_SerialManager_Success Successfully sent all data.
 * @retval kStatus_SerialManager_Busy Previous transmission still not finished; data not all sent yet.
 * @retval kStatus_SerialManager_Error An error occurred.
 */
serial_manager_status_t SerialManager_WriteBlocking(serial_write_handle_t writeHandle,
                                                    uint8_t *buffer,
                                                    uint32_t length);

/*!
 * @brief Reads data with the blocking mode.
 *
 * This is a blocking function, which polls the receiving buffer, waits for the receiving buffer to be full.
 * This function receives data using an interrupt method. The interrupt of the hardware could not be disabled.
 * And There can only one buffer for receiving for the reading handle at the same time.
 *
 * @note The function #SerialManager_ReadBlocking and the function #SerialManager_ReadNonBlocking
 * cannot be used at the same time.
 * And, the function #SerialManager_CancelReading cannot be used to abort the transmission of this function.
 *
 * @param readHandle The serial manager module handle pointer.
 * @param buffer Start address of the data to store the received data.
 * @param length The length of the data to be received.
 * @retval kStatus_SerialManager_Success Successfully received all data.
 * @retval kStatus_SerialManager_Busy Previous transmission still not finished; data not all received yet.
 * @retval kStatus_SerialManager_Error An error occurred.
 */
serial_manager_status_t SerialManager_ReadBlocking(serial_read_handle_t readHandle, uint8_t *buffer, uint32_t length);

#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
/*!
 * @brief Transmits data with the non-blocking mode.
 *
 * This is a non-blocking function, which returns directly without waiting for all data to be sent.
 * When all data is sent, the module notifies the upper layer through a TX callback function and passes
 * the status parameter @ref kStatus_SerialManager_Success.
 * This function sends data using an interrupt method. The interrupt of the hardware could not be disabled.
 * And There can only one buffer for transmission for the writing handle at the same time.
 *
 * @note The function #SerialManager_WriteBlocking and the function #SerialManager_WriteNonBlocking
 * cannot be used at the same time. And, the TX callback is mandatory before the function could be used.
 *
 * @param writeHandle The serial manager module handle pointer.
 * @param buffer Start address of the data to write.
 * @param length Length of the data to write.
 * @retval kStatus_SerialManager_Success Successfully sent all data.
 * @retval kStatus_SerialManager_Busy Previous transmission still not finished; data not all sent yet.
 * @retval kStatus_SerialManager_Error An error occurred.
 */
serial_manager_status_t SerialManager_WriteNonBlocking(serial_write_handle_t writeHandle,
                                                       uint8_t *buffer,
                                                       uint32_t length);

/*!
 * @brief Reads data with the non-blocking mode.
 *
 * This is a non-blocking function, which returns directly without waiting for all data to be received.
 * When all data is received, the module driver notifies the upper layer
 * through a RX callback function and passes the status parameter @ref kStatus_SerialManager_Success.
 * This function receives data using an interrupt method. The interrupt of the hardware could not be disabled.
 * And There can only one buffer for receiving for the reading handle at the same time.
 *
 * @note The function #SerialManager_ReadBlocking and the function #SerialManager_ReadNonBlocking
 * cannot be used at the same time. And, the RX callback is mandatory before the function could be used.
 *
 * @param readHandle The serial manager module handle pointer.
 * @param buffer Start address of the data to store the received data.
 * @param length The length of the data to be received.
 * @retval kStatus_SerialManager_Success Successfully received all data.
 * @retval kStatus_SerialManager_Busy Previous transmission still not finished; data not all received yet.
 * @retval kStatus_SerialManager_Error An error occurred.
 */
serial_manager_status_t SerialManager_ReadNonBlocking(serial_read_handle_t readHandle,
                                                      uint8_t *buffer,
                                                      uint32_t length);

/*!
 * @brief Tries to read data.
 *
 * The function tries to read data from internal ring buffer. If the ring buffer is not empty, the data will be
 * copied from ring buffer to up layer buffer. The copied length is the minimum of the ring buffer and up layer length.
 * After the data is copied, the actual data length is passed by the parameter length.
 * And There can only one buffer for receiving for the reading handle at the same time.
 *
 * @param readHandle The serial manager module handle pointer.
 * @param buffer Start address of the data to store the received data.
 * @param length The length of the data to be received.
 * @param receivedLength Length received from the ring buffer directly.
 * @retval kStatus_SerialManager_Success Successfully received all data.
 * @retval kStatus_SerialManager_Busy Previous transmission still not finished; data not all received yet.
 * @retval kStatus_SerialManager_Error An error occurred.
 */
serial_manager_status_t SerialManager_TryRead(serial_read_handle_t readHandle,
                                              uint8_t *buffer,
                                              uint32_t length,
                                              uint32_t *receivedLength);

/*!
 * @brief Cancels unfinished send transmission.
 *
 * The function cancels unfinished send transmission. When the transfer is canceled, the module notifies the upper layer
 * through a TX callback function and passes the status parameter @ref kStatus_SerialManager_Canceled.
 *
 * @note The function #SerialManager_CancelWriting cannot be used to abort the transmission of
 * the function #SerialManager_WriteBlocking.
 *
 * @param writeHandle The serial manager module handle pointer.
 * @retval kStatus_SerialManager_Success Get successfully abort the sending.
 * @retval kStatus_SerialManager_Error An error occurred.
 */
serial_manager_status_t SerialManager_CancelWriting(serial_write_handle_t writeHandle);

/*!
 * @brief Cancels unfinished receive transmission.
 *
 * The function cancels unfinished receive transmission. When the transfer is canceled, the module notifies the upper
 * layer
 * through a RX callback function and passes the status parameter @ref kStatus_SerialManager_Canceled.
 *
 * @note The function #SerialManager_CancelReading cannot be used to abort the transmission of
 * the function #SerialManager_ReadBlocking.
 *
 * @param readHandle The serial manager module handle pointer.
 * @retval kStatus_SerialManager_Success Get successfully abort the receiving.
 * @retval kStatus_SerialManager_Error An error occurred.
 */
serial_manager_status_t SerialManager_CancelReading(serial_read_handle_t readHandle);

/*!
 * @brief Installs a TX callback and callback parameter.
 *
 * This function is used to install the TX callback and callback parameter for the serial manager module.
 * When any status of TX transmission changed, the driver will notify the upper layer by the installed callback
 * function. And the status is also passed as status parameter when the callback is called.
 *
 * @param writeHandle The serial manager module handle pointer.
 * @param callback The callback function.
 * @param callbackParam The parameter of the callback function.
 * @retval kStatus_SerialManager_Success Successfully install the callback.
 */
serial_manager_status_t SerialManager_InstallTxCallback(serial_write_handle_t writeHandle,
                                                        serial_manager_callback_t callback,
                                                        void *callbackParam);

/*!
 * @brief Installs a RX callback and callback parameter.
 *
 * This function is used to install the RX callback and callback parameter for the serial manager module.
 * When any status of RX transmission changed, the driver will notify the upper layer by the installed callback
 * function. And the status is also passed as status parameter when the callback is called.
 *
 * @param readHandle The serial manager module handle pointer.
 * @param callback The callback function.
 * @param callbackParam The parameter of the callback function.
 * @retval kStatus_SerialManager_Success Successfully install the callback.
 */
serial_manager_status_t SerialManager_InstallRxCallback(serial_read_handle_t readHandle,
                                                        serial_manager_callback_t callback,
                                                        void *callbackParam);

#endif

/*!
 * @brief Prepares to enter low power consumption.
 *
 * This function is used to prepare to enter low power consumption.
 *
 * @param serialHandle The serial manager module handle pointer.
 * @retval kStatus_SerialManager_Success Successful operation.
 */
serial_manager_status_t SerialManager_EnterLowpower(serial_handle_t serialHandle);

/*!
 * @brief Restores from low power consumption.
 *
 * This function is used to restore from low power consumption.
 *
 * @param serialHandle The serial manager module handle pointer.
 * @retval kStatus_SerialManager_Success Successful operation.
 */
serial_manager_status_t SerialManager_ExitLowpower(serial_handle_t serialHandle);

#if defined(__cplusplus)
}
#endif
/*! @} */
#endif /* __SERIAL_MANAGER_H__ */
