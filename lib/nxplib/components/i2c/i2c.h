/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __HAL_I2C_ADAPTER_H__
#define __HAL_I2C_ADAPTER_H__

/*!
 * @addtogroup hal_i2c_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define HAL_I2C_MASTER_HANDLE_SIZE (80U)
#define HAL_I2C_SLAVE_HANDLE_SIZE (80U)

typedef enum _hal_i2c_status
{
    kStatus_HAL_I2cSuccess         = kStatus_Success,                      /*!< Successfully */
    kStatus_HAL_I2cError           = MAKE_STATUS(kStatusGroup_HAL_I2C, 1), /*!< Error occurs on HAL I2C */
    kStatus_HAL_I2cBusy            = MAKE_STATUS(kStatusGroup_HAL_I2C, 2), /*!< HAL I2C is busy with current transfer */
    kStatus_HAL_I2cIdle            = MAKE_STATUS(kStatusGroup_HAL_I2C, 3), /*!< HAL I2C transmitter is idle */
    kStatus_HAL_I2cNak             = MAKE_STATUS(kStatusGroup_HAL_I2C, 4), /*!< NAK received during transfer */
    kStatus_HAL_I2cArbitrationLost = MAKE_STATUS(kStatusGroup_HAL_I2C, 5), /*!< Arbitration lost during transfer */
    kStatus_HAL_I2cTimeout         = MAKE_STATUS(kStatusGroup_HAL_I2C, 6), /*!< Timeout */
    kStatus_HAL_I2cAddrressNak     = MAKE_STATUS(kStatusGroup_HAL_I2C, 7), /*!< NAK received during the address probe */
} hal_i2c_status_t;

/*! @brief HAL I2C master user configuration. */
typedef struct _hal_i2c_master_config
{
    uint32_t srcClock_Hz;  /*!< Clock source for I2C in Hz */
    uint32_t baudRate_Bps; /*!< Baud rate configuration of HAL I2C peripheral. */
    bool enableMaster;     /*!< Enables the HAL I2C peripheral at initialization time. */
    uint8_t instance;      /*!< Instance of the i2c */
} hal_i2c_master_config_t;

/*! @brief HAL I2C slave user configuration. */
typedef struct _hal_i2c_slave_config
{
    uint32_t srcClock_Hz;  /*!< Clock source for I2C in Hz */
    uint16_t slaveAddress; /*!< A slave address configuration. */
    bool enableSlave;      /*!< Enables the HAL I2C peripheral at initialization time. */
    uint8_t instance;      /*!< Instance of the i2c */
} hal_i2c_slave_config_t;

/*! @brief Direction of master and slave transfers. */
typedef enum _hal_i2c_direction
{
    kHAL_I2cWrite = 0U, /*!< Master transmit. */
    kHAL_I2cRead  = 1U  /*!< Master receive. */
} hal_i2c_direction_t;

/*! @brief I2C transfer control flag. */
typedef enum _hal_i2c_master_transfer_flag
{
    kHAL_I2cTransferDefaultFlag = 0x0U,       /*!< A transfer starts with a start signal, stops with a stop signal. */
    kHAL_I2cTransferNoStartFlag = 0x1U,       /*!< A transfer starts without a start signal, only support write only or
                                        write+read with no start flag, do not support read only with no start flag. */
    kHAL_I2cTransferRepeatedStartFlag = 0x2U, /*!< A transfer starts with a repeated start signal. */
    kHAL_I2cTransferNoStopFlag        = 0x4U, /*!< A transfer ends without a stop signal. */
} hal_i2c_master_transfer_flag_t;

/*!
 * @brief Set of events sent to the callback for nonblocking slave transfers.
 *
 * These event enumerations are used for two related purposes. First, a bit mask created by OR'ing together
 * events is passed to I2C_SlaveTransferNonBlocking() to specify which events to enable.
 * Then, when the slave callback is invoked, it is passed the current event through its @a transfer
 * parameter.
 *
 * @note These enumerations are meant to be OR'd together to form a bit mask of events.
 */
typedef enum _hal_i2c_slave_transfer_event
{
    kHAL_I2cSlaveAddressMatchEvent = 0x01U, /*!< Received the slave address after a start or repeated start. */
    kHAL_I2cSlaveTransmitEvent     = 0x02U, /*!< A callback is requested to provide data to transmit
                                                (slave-transmitter role). */
    kHAL_I2cSlaveReceiveEvent = 0x04U,      /*!< A callback is requested to provide a buffer in which to place received
                                                 data (slave-receiver role). */
    kHAL_I2cSlaveTransmitAckEvent = 0x08U,  /*!< A callback needs to either transmit an ACK or NACK. */
    kHAL_I2cSlaveCompletionEvent  = 0x20U,  /*!< A stop was detected or finished transfer, completing the transfer. */
    kHAL_I2cSlaveStartEvent       = 0x10U,  /*!< A start/repeated start was detected. */
    kHAL_I2cSlaveGenaralcallEvent = 0x40U,  /*!< Received the general call address after a start or repeated start. */
    /*! A bit mask of all available events. */
    kHAL_I2cSlaveAllEvents = kHAL_I2cSlaveAddressMatchEvent | kHAL_I2cSlaveTransmitEvent | kHAL_I2cSlaveReceiveEvent |
                             kHAL_I2cSlaveTransmitAckEvent | kHAL_I2cSlaveCompletionEvent | kHAL_I2cSlaveStartEvent |
                             kHAL_I2cSlaveGenaralcallEvent,
} hal_i2c_slave_transfer_event_t;

/*! @brief HAL I2C master transfer structure. */
typedef struct _hal_i2c_master_transfer
{
    uint8_t *volatile data;        /*!< A transfer buffer. */
    volatile size_t dataSize;      /*!< A transfer size. */
    uint32_t flags;                /*!< A transfer flag which controls the transfer. */
    uint32_t subaddress;           /*!< A sub address. Transferred MSB first. */
    uint8_t subaddressSize;        /*!< A size of the command buffer. */
    uint8_t slaveAddress;          /*!< 7-bit slave address. */
    hal_i2c_direction_t direction; /*!< A transfer direction, read or write. */
} hal_i2c_master_transfer_t;

/*! @brief HAL I2C slave transfer structure. */
typedef struct _hal_i2c_slave_transfer
{
    hal_i2c_slave_transfer_event_t event; /*!< A reason that the callback is invoked. */
    uint8_t *volatile data;               /*!< A transfer buffer. */
    volatile size_t dataSize;             /*!< A transfer size. */
    hal_i2c_status_t completionStatus;    /*!< Success or error code describing how the transfer completed. Only applies
                                     for    #kHAL_I2cSlaveCompletionEvent. */
    size_t transferredCount; /*!< A number of bytes actually transferred since the start or since the last repeated
                                start. */
} hal_i2c_slave_transfer_t;

typedef void *hal_i2c_master_handle_t;
typedef void *hal_i2c_slave_handle_t;

/*!
 * @brief Master completion callback function pointer type.
 *
 * This callback is used only for the non-blocking master transfer API. Specify the callback you wish to use
 * in the call to HAL_I2cMasterTransferInstallCallback().
 *
 * @param handle i2c master handle pointer, this should be a static variable.
 * @param completionStatus Either #kStatus_HAL_I2cSuccess or an error code describing how the transfer completed.
 * @param callbackParam Arbitrary pointer-sized value passed from the application.
 */
typedef void (*hal_i2c_master_transfer_callback_t)(hal_i2c_master_handle_t handle,
                                                   hal_i2c_status_t completionStatus,
                                                   void *callbackParam);

/*!
 * @brief Slave event callback function pointer type.
 *
 * This callback is used only for the slave non-blocking transfer API. Specify the callback you wish to use
 * in the call to HAL_I2cSlaveTransferInstallCallback().
 *
 * @param handle i2c slave master handle pointer, this should be a static variable.
 * @param transfer Pointer to transfer descriptor containing values passed to and/or from the callback.
 * @param callbackParam Arbitrary pointer-sized value passed from the application.
 */
typedef void (*hal_i2c_slave_transfer_callback_t)(hal_i2c_slave_handle_t handle,
                                                  hal_i2c_slave_transfer_t *transfer,
                                                  void *callbackParam);

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus. */

/*!
 * @name Initialization and de-initialization
 * @{
 */

/*!
 * @brief Initializes the HAL I2C master peripheral.
 *
 * @note This API should be called at the beginning of the application.
 * Otherwise, any operation to the HAL I2C module can cause a hard fault
 * because the clock is not enabled. This function configures the i2c master
 * with user-defined settings. The user can configure the configuration
 * structure. The parameter handle is a pointer to point to a memory space
 * of size #HAL_I2C_MASTER_HANDLE_SIZE allocated by the caller.
 *
 * This is an example.
 * @code
 *  uint8_t g_i2cMasterHandleBuffer[HAL_I2C_MASTER_HANDLE_SIZE];
 *  hal_i2c_master_handle_t g_i2cMasterHandle = (hal_i2c_master_handle_t)&g_i2cMasterHandleBuffer[0];
 *  hal_i2c_master_config_t masterConfig;
 *  masterConfig.enableMaster = true;
 *  masterConfig.baudRate_Bps = 100000U;
 *  masterConfig.srcClock_Hz = 12000000U;
 *  masterConfig.instance = 0;
 *  HAL_I2cMasterInit(g_i2cMasterHandle, &masterConfig);
 * @endcode
 *
 * @param handle Pointer to point to a memory space of size #HAL_I2C_MASTER_HANDLE_SIZE allocated by the caller.
 * @param config A pointer to the master configuration structure
 * @retval kStatus_HAL_I2cError An error occurred.
 * @retval kStatus_HAL_I2cSuccess i2c master initialization succeed
 */
hal_i2c_status_t HAL_I2cMasterInit(hal_i2c_master_handle_t handle, const hal_i2c_master_config_t *config);

/*!
 * @brief Initializes the HAL I2C peripheral.
 *
 * @note This API should be called at the beginning of the application.
 * Otherwise, any operation to the HAL I2C module can cause a hard fault
 * because the clock is not enabled. This function configures the i2c slave
 * with user-defined settings. The user can configure the configuration
 * structure. The parameter handle is a pointer to point to a memory space
 * of size #HAL_I2C_SLAVE_HANDLE_SIZE allocated by the caller.
 *
 * This is an example.
 * @code
 *  uint8_t g_i2cSlaveHandleBuffer[HAL_I2C_SLAVE_HANDLE_SIZE];
 *  hal_i2c_slave_handle_t g_i2cSlaveHandle = (hal_i2c_slave_handle_t)&g_i2cSlaveHandleBuffer[0];
 *  hal_i2c_slave_config_t slaveConfig;
 *  slaveConfig.enableSlave = true;
 *  slaveConfig.slaveAddress = 0x01U;
 *  slaveConfig.srcClock_Hz = 12000000U;
 *  slaveConfig.instance = 0;
 *  HAL_I2cSlaveInit(g_i2cSlaveHandle, &slaveConfig);
 * @endcode
 *
 * @param handle Pointer to point to a memory space of size #HAL_I2C_SLAVE_HANDLE_SIZE allocated by the caller.
 * @param config A pointer to the slave configuration structure
 * @retval kStatus_HAL_I2cError An error occurred.
 * @retval kStatus_HAL_I2cSuccess i2c slave initialization succeed
 */
hal_i2c_status_t HAL_I2cSlaveInit(hal_i2c_slave_handle_t handle, const hal_i2c_slave_config_t *config);

/*!
 * @brief De-initializes the HAL I2C master peripheral. Call this API to gate the HAL I2C clock.
 * The HAL I2C master module can't work unless the HAL_I2cMasterInit is called.
 *
 * @param handle i2c master handle pointer, this should be a static variable.
 * @retval kStatus_HAL_I2cSuccess i2c master de-initialization succeed */
hal_i2c_status_t HAL_I2cMasterDeinit(hal_i2c_master_handle_t handle);

/*!
 * @brief De-initializes the HAL I2C slave peripheral. Calling this API gates the HAL I2C clock.
 * The HAL I2C slave module can't work unless the HAL_I2cSlaveInit is called to enable the clock.
 *
 * @param handle i2c slave handle pointer, this should be a static variable.
 * @retval kStatus_HAL_I2cSuccess i2c slave de-initialization succeed
 */
hal_i2c_status_t HAL_I2cSlaveDeinit(hal_i2c_slave_handle_t handle);

/*! @} */

/*!
 * @name Bus Operations
 * @{
 */

/*!
 * @brief Performs a polling send transaction on the HAL I2C bus.
 *
 * @param handle i2c master handle pointer, this should be a static variable.
 * @param txBuff The pointer to the data to be transferred.
 * @param txSize The length in bytes of the data to be transferred.
 * @param flags Transfer control flag to decide whether need to send a stop, use kHAL_I2cTransferDefaultFlag
 *  to issue a stop and kHAL_I2cTransferNoStopFlag to not send a stop.
 * @retval kStatus_HAL_I2cSuccess Successfully complete the data transmission.
 * @retval kStatus_HAL_I2cArbitrationLost Transfer error, arbitration lost.
 * @retval kStatus_HAL_I2cNak Transfer error, receive NAK during transfer.
 */
hal_i2c_status_t HAL_I2cMasterWriteBlocking(hal_i2c_master_handle_t handle,
                                            const uint8_t *txBuff,
                                            size_t txSize,
                                            uint32_t flags);

/*!
 * @brief Performs a polling receive transaction on the HAL I2C bus.
 *
 * @note The HAL_I2cMasterReadBlocking function stops the bus before reading the final byte.
 * Without stopping the bus prior for the final read, the bus issues another read, resulting
 * in garbage data being read into the data register.
 *
 * @param handle i2c master handle pointer, this should be a static variable.
 * @param rxBuff The pointer to the data to store the received data.
 * @param rxSize The length in bytes of the data to be received.
 * @param flags Transfer control flag to decide whether need to send a stop, use kHAL_I2cTransferDefaultFlag
 *  to issue a stop and kHAL_I2cTransferNoStopFlag to not send a stop.
 * @retval kStatus_HAL_I2cSuccess Successfully complete the data transmission.
 * @retval kStatus_HAL_I2cTimeout Send stop signal failed, timeout.
 */
hal_i2c_status_t HAL_I2cMasterReadBlocking(hal_i2c_master_handle_t handle,
                                           uint8_t *rxBuff,
                                           size_t rxSize,
                                           uint32_t flags);

/*!
 * @brief Performs a polling send transaction on the HAL I2C bus.
 *
 * @param handle i2c slave handle pointer, this should be a static variable.
 * @param txBuff The pointer to the data to be transferred.
 * @param txSize The length in bytes of the data to be transferred.
 * @retval kStatus_HAL_I2cSuccess Successfully complete the data transmission.
 * @retval kStatus_HAL_I2cArbitrationLost Transfer error, arbitration lost.
 * @retval kStatus_HAL_I2cNak Transfer error, receive NAK during transfer.
 */
hal_i2c_status_t HAL_I2cSlaveWriteBlocking(hal_i2c_slave_handle_t handle, const uint8_t *txBuff, size_t txSize);

/*!
 * @brief Performs a polling receive transaction on the HAL I2C bus.
 *
 * @param handle i2c slave handle pointer, this should be a static variable.
 * @param rxBuff The pointer to the data to store the received data.
 * @param rxSize The length in bytes of the data to be received.
 * @retval kStatus_HAL_I2cSuccess Successfully complete data receive.
 * @retval kStatus_HAL_I2cTimeout Wait status flag timeout.
 */
hal_i2c_status_t HAL_I2cSlaveReadBlocking(hal_i2c_slave_handle_t handle, uint8_t *rxBuff, size_t rxSize);

/*!
 * @brief Performs a master polling transfer on the HAL I2C bus.
 *
 * @note The API does not return until the transfer succeeds or fails due
 * to arbitration lost or receiving a NAK.
 *
 * @param handle i2c master handle pointer, this should be a static variable.
 * @param xfer Pointer to the transfer structure.
 * @retval kStatus_HAL_I2cSuccess Successfully complete the data transmission.
 * @retval kStatus_HAL_I2cBusy Previous transmission still not finished.
 * @retval kStatus_HAL_I2cTimeout Transfer error, wait signal timeout.
 * @retval kStatus_HAL_I2cArbitrationLost Transfer error, arbitration lost.
 * @retval kStatus_HAL_I2cNak Transfer error, receive NAK during transfer.
 */
hal_i2c_status_t HAL_I2cMasterTransferBlocking(hal_i2c_master_handle_t handle, hal_i2c_master_transfer_t *xfer);

/*! @} */

/*!
 * @name Transactional
 * @{
 */

/*!
 * @brief Installs a callback and callback parameter.
 *
 * This function is used to install the callback and callback parameter for i2c master module.
 * When any status of the i2c master changed, the driver will notify the upper layer by the installed callback
 * function. And the status is also passed as status parameter when the callback is called.
 *
 * @param handle i2c master handle pointer, this should be a static variable.
 * @param callback pointer to user callback function.
 * @param callbackParam user parameter passed to the callback function.
 * @retval kStatus_HAL_I2cSuccess i2c master handle created
 */
hal_i2c_status_t HAL_I2cMasterTransferInstallCallback(hal_i2c_master_handle_t handle,
                                                      hal_i2c_master_transfer_callback_t callback,
                                                      void *callbackParam);

/*!
 * @brief Performs a master interrupt non-blocking transfer on the HAL I2C bus.
 *
 * @note Calling the API returns immediately after transfer initiates. The user needs
 * to call HAL_I2cMasterGetTransferCount to poll the transfer status to check whether
 * the transfer is finished. If the return status is not kStatus_HAL_I2cBusy, the transfer
 * is finished.
 *
 * @param handle i2c master handle pointer, this should be a static variable.
 * @param xfer pointer to hal_i2c_master_transfer_t structure.
 * @retval kStatus_HAL_I2cSuccess Successfully start the data transmission.
 * @retval kStatus_HAL_I2cBusy Previous transmission still not finished.
 * @retval kStatus_HAL_I2cTimeout Transfer error, wait signal timeout.
 */
hal_i2c_status_t HAL_I2cMasterTransferNonBlocking(hal_i2c_master_handle_t handle, hal_i2c_master_transfer_t *xfer);

/*!
 * @brief Gets the master transfer status during a interrupt non-blocking transfer.
 *
 * @param handle i2c master handle pointer, this should be a static variable.
 * @param count Number of bytes transferred so far by the non-blocking transaction.
 * @retval kStatus_HAL_I2cError An error occurred.
 * @retval kStatus_HAL_I2cSuccess Successfully return the count.
 */
hal_i2c_status_t HAL_I2cMasterTransferGetCount(hal_i2c_master_handle_t handle, size_t *count);

/*!
 * @brief Aborts an interrupt non-blocking transfer early.
 *
 * @note This API can be called at any time when an interrupt non-blocking transfer initiates
 * to abort the transfer early.
 *
 * @param handle i2c master handle pointer, this should be a static variable.
 * @retval kStatus_HAL_I2cTimeout Timeout during polling flag.
 * @retval kStatus_HAL_I2cSuccess Successfully abort the transfer.
 */
hal_i2c_status_t HAL_I2cMasterTransferAbort(hal_i2c_master_handle_t handle);

/*!
 * @brief Installs a callback and callback parameter.
 *
 * This function is used to install the callback and callback parameter for i2c slave module.
 * When any status of the i2c slave changed, the driver will notify the upper layer by the installed callback
 * function. And the status is also passed as status parameter when the callback is called.
 *
 * @param handle i2c slave handle pointer, this should be a static variable.
 * @param callback pointer to user callback function.
 * @param callbackParam user parameter passed to the callback function.
 * @retval kStatus_HAL_I2cSuccess i2c slave handle created
 */
hal_i2c_status_t HAL_I2cSlaveTransferInstallCallback(hal_i2c_slave_handle_t handle,
                                                     hal_i2c_slave_transfer_callback_t callback,
                                                     void *callbackParam);

/*!
 * @brief Starts accepting slave transfers.
 *
 * Call this API after calling the HAL_I2cSlaveInit() and HAL_I2cSlaveTransferInstallCallback() to start processing
 * transactions driven by an HAL I2C slave. The slave monitors the HAL I2C bus and passes events to the
 * callback that was passed into the call to HAL_I2cSlaveTransferInstallCallback(). The callback is always invoked
 * from the interrupt context.
 *
 * The set of events received by the callback is customizable. To do so, set the @a eventMask parameter to
 * the OR'd combination of #hal_i2c_slave_transfer_event_t enumerators for the events you wish to receive.
 * The #kHAL_I2cSlaveTransmitEvent and #kLPHAL_I2cSlaveReceiveEvent events are always enabled and do not need
 * to be included in the mask. Alternatively, pass 0 to get a default set of only the transmit and
 * receive events that are always enabled. In addition, the #kHAL_I2cSlaveAllEvents constant is provided as
 * a convenient way to enable all events.
 *
 * @param handle i2c slave handle pointer, this should be a static variable.
 * @param eventMask Bit mask formed by OR'ing together #hal_i2c_slave_transfer_event_t enumerators to specify
 *      which events to send to the callback. Other accepted values are 0 to get a default set of
 *      only the transmit and receive events, and #kHAL_I2cSlaveAllEvents to enable all events.
 *
 * @retval #kStatus_HAL_I2cSuccess Slave transfers were successfully started.
 * @retval #kStatus_HAL_I2cBusy Slave transfers have already been started on this handle.
 */
hal_i2c_status_t HAL_I2cSlaveTransferNonBlocking(hal_i2c_slave_handle_t handle, uint32_t eventMask);

/*!
 * @brief Aborts the slave transfer.
 *
 * @note This API can be called at any time to stop slave for handling the bus events.
 *
 * @param handle i2c slave handle pointer, this should be a static variable.
 * @retval kStatus_HAL_I2cSuccess Successfully return the count.
 */
hal_i2c_status_t HAL_I2cSlaveTransferAbort(hal_i2c_slave_handle_t handle);

/*!
 * @brief Gets the slave transfer remaining bytes during a interrupt non-blocking transfer.
 *
 * @param handle i2c slave handle pointer, this should be a static variable.
 * @param count Number of bytes transferred so far by the non-blocking transaction.
 * @retval kStatus_HAL_I2cError An error occurred.
 * @retval kStatus_HAL_I2cSuccess Successfully return the count.
 */
hal_i2c_status_t HAL_I2cSlaveTransferGetCount(hal_i2c_slave_handle_t handle, size_t *count);

/*! @} */
#if defined(__cplusplus)
}
#endif /*_cplusplus. */
/*! @} */

#endif /* __HAL_I2C_ADAPTER_H__*/
