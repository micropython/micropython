/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_LPI2C_H_
#define _FSL_LPI2C_H_

#include <stddef.h>
#include "fsl_device_registers.h"
#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @addtogroup lpi2c
 * @{
 */

/*! @name Driver version */
/*@{*/
/*! @brief LPI2C driver version 2.1.9. */
#define FSL_LPI2C_DRIVER_VERSION (MAKE_VERSION(2, 1, 9))
/*@}*/

/*! @brief Timeout times for waiting flag. */
#ifndef LPI2C_WAIT_TIMEOUT
#define LPI2C_WAIT_TIMEOUT 0U /* Define to zero means keep waiting until the flag is assert/deassert. */
#endif

/*! @brief LPI2C status return codes. */
enum _lpi2c_status
{
    kStatus_LPI2C_Busy = MAKE_STATUS(kStatusGroup_LPI2C, 0), /*!< The master is already performing a transfer. */
    kStatus_LPI2C_Idle = MAKE_STATUS(kStatusGroup_LPI2C, 1), /*!< The slave driver is idle. */
    kStatus_LPI2C_Nak  = MAKE_STATUS(kStatusGroup_LPI2C, 2), /*!< The slave device sent a NAK in response to a byte. */
    kStatus_LPI2C_FifoError       = MAKE_STATUS(kStatusGroup_LPI2C, 3), /*!< FIFO under run or overrun. */
    kStatus_LPI2C_BitError        = MAKE_STATUS(kStatusGroup_LPI2C, 4), /*!< Transferred bit was not seen on the bus. */
    kStatus_LPI2C_ArbitrationLost = MAKE_STATUS(kStatusGroup_LPI2C, 5), /*!< Arbitration lost error. */
    kStatus_LPI2C_PinLowTimeout =
        MAKE_STATUS(kStatusGroup_LPI2C, 6), /*!< SCL or SDA were held low longer than the timeout. */
    kStatus_LPI2C_NoTransferInProgress =
        MAKE_STATUS(kStatusGroup_LPI2C, 7), /*!< Attempt to abort a transfer when one is not in progress. */
    kStatus_LPI2C_DmaRequestFail = MAKE_STATUS(kStatusGroup_LPI2C, 8), /*!< DMA request failed. */
    kStatus_LPI2C_Timeout        = MAKE_STATUS(kStatusGroup_LPI2C, 9), /*!< Timeout poling status flags. */
};

/*! @} */

/*!
 * @addtogroup lpi2c_master_driver
 * @{
 */

/*!
 * @brief LPI2C master peripheral flags.
 *
 * The following status register flags can be cleared:
 * - #kLPI2C_MasterEndOfPacketFlag
 * - #kLPI2C_MasterStopDetectFlag
 * - #kLPI2C_MasterNackDetectFlag
 * - #kLPI2C_MasterArbitrationLostFlag
 * - #kLPI2C_MasterFifoErrFlag
 * - #kLPI2C_MasterPinLowTimeoutFlag
 * - #kLPI2C_MasterDataMatchFlag
 *
 * All flags except #kLPI2C_MasterBusyFlag and #kLPI2C_MasterBusBusyFlag can be enabled as
 * interrupts.
 *
 * @note These enums are meant to be OR'd together to form a bit mask.
 */
enum _lpi2c_master_flags
{
    kLPI2C_MasterTxReadyFlag         = LPI2C_MSR_TDF_MASK,  /*!< Transmit data flag */
    kLPI2C_MasterRxReadyFlag         = LPI2C_MSR_RDF_MASK,  /*!< Receive data flag */
    kLPI2C_MasterEndOfPacketFlag     = LPI2C_MSR_EPF_MASK,  /*!< End Packet flag */
    kLPI2C_MasterStopDetectFlag      = LPI2C_MSR_SDF_MASK,  /*!< Stop detect flag */
    kLPI2C_MasterNackDetectFlag      = LPI2C_MSR_NDF_MASK,  /*!< NACK detect flag */
    kLPI2C_MasterArbitrationLostFlag = LPI2C_MSR_ALF_MASK,  /*!< Arbitration lost flag */
    kLPI2C_MasterFifoErrFlag         = LPI2C_MSR_FEF_MASK,  /*!< FIFO error flag */
    kLPI2C_MasterPinLowTimeoutFlag   = LPI2C_MSR_PLTF_MASK, /*!< Pin low timeout flag */
    kLPI2C_MasterDataMatchFlag       = LPI2C_MSR_DMF_MASK,  /*!< Data match flag */
    kLPI2C_MasterBusyFlag            = LPI2C_MSR_MBF_MASK,  /*!< Master busy flag */
    kLPI2C_MasterBusBusyFlag         = LPI2C_MSR_BBF_MASK   /*!< Bus busy flag */
};

/*! @brief Direction of master and slave transfers. */
typedef enum _lpi2c_direction
{
    kLPI2C_Write = 0U, /*!< Master transmit. */
    kLPI2C_Read  = 1U  /*!< Master receive. */
} lpi2c_direction_t;

/*! @brief LPI2C pin configuration. */
typedef enum _lpi2c_master_pin_config
{
    kLPI2C_2PinOpenDrain  = 0x0U, /*!< LPI2C Configured for 2-pin open drain mode */
    kLPI2C_2PinOutputOnly = 0x1U, /*!< LPI2C Configured for 2-pin output only mode (ultra-fast mode) */
    kLPI2C_2PinPushPull   = 0x2U, /*!< LPI2C Configured for 2-pin push-pull mode */
    kLPI2C_4PinPushPull   = 0x3U, /*!< LPI2C Configured for 4-pin push-pull mode */
    kLPI2C_2PinOpenDrainWithSeparateSlave =
        0x4U, /*!< LPI2C Configured for 2-pin open drain mode with separate LPI2C slave */
    kLPI2C_2PinOutputOnlyWithSeparateSlave =
        0x5U, /*!< LPI2C Configured for 2-pin output only mode(ultra-fast mode) with separate LPI2C slave */
    kLPI2C_2PinPushPullWithSeparateSlave =
        0x6U, /*!< LPI2C Configured for 2-pin push-pull mode with separate LPI2C slave */
    kLPI2C_4PinPushPullWithInvertedOutput = 0x7U /*!< LPI2C Configured for 4-pin push-pull mode(inverted outputs) */
} lpi2c_master_pin_config_t;

/*! @brief LPI2C master host request selection. */
typedef enum _lpi2c_host_request_source
{
    kLPI2C_HostRequestExternalPin  = 0x0U, /*!< Select the LPI2C_HREQ pin as the host request input */
    kLPI2C_HostRequestInputTrigger = 0x1U, /*!< Select the input trigger as the host request input */
} lpi2c_host_request_source_t;

/*! @brief LPI2C master host request pin polarity configuration. */
typedef enum _lpi2c_host_request_polarity
{
    kLPI2C_HostRequestPinActiveLow  = 0x0U, /*!< Configure the LPI2C_HREQ pin active low */
    kLPI2C_HostRequestPinActiveHigh = 0x1U  /*!< Configure the LPI2C_HREQ pin active high */
} lpi2c_host_request_polarity_t;

/*!
 * @brief Structure with settings to initialize the LPI2C master module.
 *
 * This structure holds configuration settings for the LPI2C peripheral. To initialize this
 * structure to reasonable defaults, call the LPI2C_MasterGetDefaultConfig() function and
 * pass a pointer to your configuration structure instance.
 *
 * The configuration structure can be made constant so it resides in flash.
 */
typedef struct _lpi2c_master_config
{
    bool enableMaster;                   /*!< Whether to enable master mode. */
    bool enableDoze;                     /*!< Whether master is enabled in doze mode. */
    bool debugEnable;                    /*!< Enable transfers to continue when halted in debug mode. */
    bool ignoreAck;                      /*!< Whether to ignore ACK/NACK. */
    lpi2c_master_pin_config_t pinConfig; /*!< The pin configuration option. */
    uint32_t baudRate_Hz;                /*!< Desired baud rate in Hertz. */
    uint32_t busIdleTimeout_ns;          /*!< Bus idle timeout in nanoseconds. Set to 0 to disable. */
    uint32_t pinLowTimeout_ns;           /*!< Pin low timeout in nanoseconds. Set to 0 to disable. */
    uint8_t sdaGlitchFilterWidth_ns;     /*!< Width in nanoseconds of glitch filter on SDA pin. Set to 0 to disable. */
    uint8_t sclGlitchFilterWidth_ns;     /*!< Width in nanoseconds of glitch filter on SCL pin. Set to 0 to disable. */
    struct
    {
        bool enable;                            /*!< Enable host request. */
        lpi2c_host_request_source_t source;     /*!< Host request source. */
        lpi2c_host_request_polarity_t polarity; /*!< Host request pin polarity. */
    } hostRequest;                              /*!< Host request options. */
} lpi2c_master_config_t;

/*! @brief LPI2C master data match configuration modes. */
typedef enum _lpi2c_data_match_config_mode
{
    kLPI2C_MatchDisabled       = 0x0U, /*!< LPI2C Match Disabled */
    kLPI2C_1stWordEqualsM0OrM1 = 0x2U, /*!< LPI2C Match Enabled and 1st data word equals MATCH0 OR MATCH1 */
    kLPI2C_AnyWordEqualsM0OrM1 = 0x3U, /*!< LPI2C Match Enabled and any data word equals MATCH0 OR MATCH1 */
    kLPI2C_1stWordEqualsM0And2ndWordEqualsM1 =
        0x4U, /*!< LPI2C Match Enabled and 1st data word equals MATCH0, 2nd data equals MATCH1 */
    kLPI2C_AnyWordEqualsM0AndNextWordEqualsM1 =
        0x5U, /*!< LPI2C Match Enabled and any data word equals MATCH0, next data equals MATCH1 */
    kLPI2C_1stWordAndM1EqualsM0AndM1 =
        0x6U, /*!< LPI2C Match Enabled and 1st data word and MATCH0 equals MATCH0 and MATCH1 */
    kLPI2C_AnyWordAndM1EqualsM0AndM1 =
        0x7U /*!< LPI2C Match Enabled and any data word and MATCH0 equals MATCH0 and MATCH1 */
} lpi2c_data_match_config_mode_t;

/*! @brief LPI2C master data match configuration structure. */
typedef struct _lpi2c_match_config
{
    lpi2c_data_match_config_mode_t matchMode; /*!< Data match configuration setting. */
    bool rxDataMatchOnly; /*!< When set to true, received data is ignored until a successful match. */
    uint32_t match0;      /*!< Match value 0. */
    uint32_t match1;      /*!< Match value 1. */
} lpi2c_data_match_config_t;

/* Forward declaration of the transfer descriptor and handle typedefs. */
typedef struct _lpi2c_master_transfer lpi2c_master_transfer_t;
typedef struct _lpi2c_master_handle lpi2c_master_handle_t;

/*!
 * @brief Master completion callback function pointer type.
 *
 * This callback is used only for the non-blocking master transfer API. Specify the callback you wish to use
 * in the call to LPI2C_MasterTransferCreateHandle().
 *
 * @param base The LPI2C peripheral base address.
 * @param completionStatus Either #kStatus_Success or an error code describing how the transfer completed.
 * @param userData Arbitrary pointer-sized value passed from the application.
 */
typedef void (*lpi2c_master_transfer_callback_t)(LPI2C_Type *base,
                                                 lpi2c_master_handle_t *handle,
                                                 status_t completionStatus,
                                                 void *userData);

/*!
 * @brief Transfer option flags.
 *
 * @note These enumerations are intended to be OR'd together to form a bit mask of options for
 * the #_lpi2c_master_transfer::flags field.
 */
enum _lpi2c_master_transfer_flags
{
    kLPI2C_TransferDefaultFlag       = 0x00U, /*!< Transfer starts with a start signal, stops with a stop signal. */
    kLPI2C_TransferNoStartFlag       = 0x01U, /*!< Don't send a start condition, address, and sub address */
    kLPI2C_TransferRepeatedStartFlag = 0x02U, /*!< Send a repeated start condition */
    kLPI2C_TransferNoStopFlag        = 0x04U, /*!< Don't send a stop condition. */
};

/*!
 * @brief Non-blocking transfer descriptor structure.
 *
 * This structure is used to pass transaction parameters to the LPI2C_MasterTransferNonBlocking() API.
 */
struct _lpi2c_master_transfer
{
    uint32_t flags;        /*!< Bit mask of options for the transfer. See enumeration #_lpi2c_master_transfer_flags for
                              available options. Set to 0 or #kLPI2C_TransferDefaultFlag for normal transfers. */
    uint16_t slaveAddress; /*!< The 7-bit slave address. */
    lpi2c_direction_t direction; /*!< Either #kLPI2C_Read or #kLPI2C_Write. */
    uint32_t subaddress;         /*!< Sub address. Transferred MSB first. */
    size_t subaddressSize;       /*!< Length of sub address to send in bytes. Maximum size is 4 bytes. */
    void *data;                  /*!< Pointer to data to transfer. */
    size_t dataSize;             /*!< Number of bytes to transfer. */
};

/*!
 * @brief Driver handle for master non-blocking APIs.
 * @note The contents of this structure are private and subject to change.
 */
struct _lpi2c_master_handle
{
    uint8_t state;                                       /*!< Transfer state machine current state. */
    uint16_t remainingBytes;                             /*!< Remaining byte count in current state. */
    uint8_t *buf;                                        /*!< Buffer pointer for current state. */
    uint16_t commandBuffer[7];                           /*!< LPI2C command sequence. */
    lpi2c_master_transfer_t transfer;                    /*!< Copy of the current transfer info. */
    lpi2c_master_transfer_callback_t completionCallback; /*!< Callback function pointer. */
    void *userData;                                      /*!< Application data passed to callback. */
};

/*! @} */

/*!
 * @addtogroup lpi2c_slave_driver
 * @{
 */

/*!
 * @brief LPI2C slave peripheral flags.
 *
 * The following status register flags can be cleared:
 * - #kLPI2C_SlaveRepeatedStartDetectFlag
 * - #kLPI2C_SlaveStopDetectFlag
 * - #kLPI2C_SlaveBitErrFlag
 * - #kLPI2C_SlaveFifoErrFlag
 *
 * All flags except #kLPI2C_SlaveBusyFlag and #kLPI2C_SlaveBusBusyFlag can be enabled as
 * interrupts.
 *
 * @note These enumerations are meant to be OR'd together to form a bit mask.
 */
enum _lpi2c_slave_flags
{
    kLPI2C_SlaveTxReadyFlag             = LPI2C_SSR_TDF_MASK,  /*!< Transmit data flag */
    kLPI2C_SlaveRxReadyFlag             = LPI2C_SSR_RDF_MASK,  /*!< Receive data flag */
    kLPI2C_SlaveAddressValidFlag        = LPI2C_SSR_AVF_MASK,  /*!< Address valid flag */
    kLPI2C_SlaveTransmitAckFlag         = LPI2C_SSR_TAF_MASK,  /*!< Transmit ACK flag */
    kLPI2C_SlaveRepeatedStartDetectFlag = LPI2C_SSR_RSF_MASK,  /*!< Repeated start detect flag */
    kLPI2C_SlaveStopDetectFlag          = LPI2C_SSR_SDF_MASK,  /*!< Stop detect flag */
    kLPI2C_SlaveBitErrFlag              = LPI2C_SSR_BEF_MASK,  /*!< Bit error flag */
    kLPI2C_SlaveFifoErrFlag             = LPI2C_SSR_FEF_MASK,  /*!< FIFO error flag */
    kLPI2C_SlaveAddressMatch0Flag       = LPI2C_SSR_AM0F_MASK, /*!< Address match 0 flag */
    kLPI2C_SlaveAddressMatch1Flag       = LPI2C_SSR_AM1F_MASK, /*!< Address match 1 flag */
    kLPI2C_SlaveGeneralCallFlag         = LPI2C_SSR_GCF_MASK,  /*!< General call flag */
    kLPI2C_SlaveBusyFlag                = LPI2C_SSR_SBF_MASK,  /*!< Master busy flag */
    kLPI2C_SlaveBusBusyFlag             = LPI2C_SSR_BBF_MASK,  /*!< Bus busy flag */
};

/*! @brief LPI2C slave address match options. */
typedef enum _lpi2c_slave_address_match
{
    kLPI2C_MatchAddress0                = 0U, /*!< Match only address 0. */
    kLPI2C_MatchAddress0OrAddress1      = 2U, /*!< Match either address 0 or address 1. */
    kLPI2C_MatchAddress0ThroughAddress1 = 6U, /*!< Match a range of slave addresses from address 0 through address 1. */
} lpi2c_slave_address_match_t;

/*!
 * @brief Structure with settings to initialize the LPI2C slave module.
 *
 * This structure holds configuration settings for the LPI2C slave peripheral. To initialize this
 * structure to reasonable defaults, call the LPI2C_SlaveGetDefaultConfig() function and
 * pass a pointer to your configuration structure instance.
 *
 * The configuration structure can be made constant so it resides in flash.
 */
typedef struct _lpi2c_slave_config
{
    bool enableSlave;                             /*!< Enable slave mode. */
    uint8_t address0;                             /*!< Slave's 7-bit address. */
    uint8_t address1;                             /*!< Alternate slave 7-bit address. */
    lpi2c_slave_address_match_t addressMatchMode; /*!< Address matching options. */
    bool filterDozeEnable;                        /*!< Enable digital glitch filter in doze mode. */
    bool filterEnable;                            /*!< Enable digital glitch filter. */
    bool enableGeneralCall;                       /*!< Enable general call address matching. */
    struct
    {
        bool enableAck;     /*!< Enables SCL clock stretching during slave-transmit address byte(s)
                                        and slave-receiver address and data byte(s) to allow software to
                                        write the Transmit ACK Register before the ACK or NACK is transmitted.
                                        Clock stretching occurs when transmitting the 9th bit. When
                                        enableAckSCLStall is enabled, there is no need to set either
                                        enableRxDataSCLStall or enableAddressSCLStall. */
        bool enableTx;      /*!< Enables SCL clock stretching when the transmit data flag is set
                                         during a slave-transmit transfer. */
        bool enableRx;      /*!< Enables SCL clock stretching when receive data flag is set during
                                         a slave-receive transfer. */
        bool enableAddress; /*!< Enables SCL clock stretching when the address valid flag is asserted. */
    } sclStall;
    bool ignoreAck;                   /*!< Continue transfers after a NACK is detected. */
    bool enableReceivedAddressRead;   /*!< Enable reading the address received address as the first byte of data. */
    uint32_t sdaGlitchFilterWidth_ns; /*!< Width in nanoseconds of the digital filter on the SDA signal. */
    uint32_t sclGlitchFilterWidth_ns; /*!< Width in nanoseconds of the digital filter on the SCL signal. */
    uint32_t dataValidDelay_ns;       /*!< Width in nanoseconds of the data valid delay. */
    uint32_t clockHoldTime_ns;        /*!< Width in nanoseconds of the clock hold time. */
} lpi2c_slave_config_t;

/*!
 * @brief Set of events sent to the callback for non blocking slave transfers.
 *
 * These event enumerations are used for two related purposes. First, a bit mask created by OR'ing together
 * events is passed to LPI2C_SlaveTransferNonBlocking() in order to specify which events to enable.
 * Then, when the slave callback is invoked, it is passed the current event through its @a transfer
 * parameter.
 *
 * @note These enumerations are meant to be OR'd together to form a bit mask of events.
 */
typedef enum _lpi2c_slave_transfer_event
{
    kLPI2C_SlaveAddressMatchEvent = 0x01U,  /*!< Received the slave address after a start or repeated start. */
    kLPI2C_SlaveTransmitEvent     = 0x02U,  /*!< Callback is requested to provide data to transmit
                                                 (slave-transmitter role). */
    kLPI2C_SlaveReceiveEvent = 0x04U,       /*!< Callback is requested to provide a buffer in which to place received
                                                  data (slave-receiver role). */
    kLPI2C_SlaveTransmitAckEvent   = 0x08U, /*!< Callback needs to either transmit an ACK or NACK. */
    kLPI2C_SlaveRepeatedStartEvent = 0x10U, /*!< A repeated start was detected. */
    kLPI2C_SlaveCompletionEvent    = 0x20U, /*!< A stop was detected, completing the transfer. */

    /*! Bit mask of all available events. */
    kLPI2C_SlaveAllEvents = kLPI2C_SlaveAddressMatchEvent | kLPI2C_SlaveTransmitEvent | kLPI2C_SlaveReceiveEvent |
                            kLPI2C_SlaveTransmitAckEvent | kLPI2C_SlaveRepeatedStartEvent | kLPI2C_SlaveCompletionEvent,
} lpi2c_slave_transfer_event_t;

/*! @brief LPI2C slave transfer structure */
typedef struct _lpi2c_slave_transfer
{
    lpi2c_slave_transfer_event_t event; /*!< Reason the callback is being invoked. */
    uint8_t receivedAddress;            /*!< Matching address send by master. */
    uint8_t *data;                      /*!< Transfer buffer */
    size_t dataSize;                    /*!< Transfer size */
    status_t completionStatus; /*!< Success or error code describing how the transfer completed. Only applies for
                                  #kLPI2C_SlaveCompletionEvent. */
    size_t transferredCount;   /*!< Number of bytes actually transferred since start or last repeated start. */
} lpi2c_slave_transfer_t;

/* Forward declaration. */
typedef struct _lpi2c_slave_handle lpi2c_slave_handle_t;

/*!
 * @brief Slave event callback function pointer type.
 *
 * This callback is used only for the slave non-blocking transfer API. To install a callback,
 * use the LPI2C_SlaveSetCallback() function after you have created a handle.
 *
 * @param base Base address for the LPI2C instance on which the event occurred.
 * @param transfer Pointer to transfer descriptor containing values passed to and/or from the callback.
 * @param userData Arbitrary pointer-sized value passed from the application.
 */
typedef void (*lpi2c_slave_transfer_callback_t)(LPI2C_Type *base, lpi2c_slave_transfer_t *transfer, void *userData);

/*!
 * @brief LPI2C slave handle structure.
 * @note The contents of this structure are private and subject to change.
 */
struct _lpi2c_slave_handle
{
    lpi2c_slave_transfer_t transfer;          /*!< LPI2C slave transfer copy. */
    bool isBusy;                              /*!< Whether transfer is busy. */
    bool wasTransmit;                         /*!< Whether the last transfer was a transmit. */
    uint32_t eventMask;                       /*!< Mask of enabled events. */
    uint32_t transferredCount;                /*!< Count of bytes transferred. */
    lpi2c_slave_transfer_callback_t callback; /*!< Callback function called at transfer event. */
    void *userData;                           /*!< Callback parameter passed to callback. */
};

/*! @} */

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @addtogroup lpi2c_master_driver
 * @{
 */

/*! @name Initialization and deinitialization */
/*@{*/

/*!
 * @brief Provides a default configuration for the LPI2C master peripheral.
 *
 * This function provides the following default configuration for the LPI2C master peripheral:
 * @code
 *  masterConfig->enableMaster            = true;
 *  masterConfig->debugEnable             = false;
 *  masterConfig->ignoreAck               = false;
 *  masterConfig->pinConfig               = kLPI2C_2PinOpenDrain;
 *  masterConfig->baudRate_Hz             = 100000U;
 *  masterConfig->busIdleTimeout_ns       = 0;
 *  masterConfig->pinLowTimeout_ns        = 0;
 *  masterConfig->sdaGlitchFilterWidth_ns = 0;
 *  masterConfig->sclGlitchFilterWidth_ns = 0;
 *  masterConfig->hostRequest.enable      = false;
 *  masterConfig->hostRequest.source      = kLPI2C_HostRequestExternalPin;
 *  masterConfig->hostRequest.polarity    = kLPI2C_HostRequestPinActiveHigh;
 * @endcode
 *
 * After calling this function, you can override any settings in order to customize the configuration,
 * prior to initializing the master driver with LPI2C_MasterInit().
 *
 * @param[out] masterConfig User provided configuration structure for default values. Refer to #lpi2c_master_config_t.
 */
void LPI2C_MasterGetDefaultConfig(lpi2c_master_config_t *masterConfig);

/*!
 * @brief Initializes the LPI2C master peripheral.
 *
 * This function enables the peripheral clock and initializes the LPI2C master peripheral as described by the user
 * provided configuration. A software reset is performed prior to configuration.
 *
 * @param base The LPI2C peripheral base address.
 * @param masterConfig User provided peripheral configuration. Use LPI2C_MasterGetDefaultConfig() to get a set of
 * defaults
 *      that you can override.
 * @param sourceClock_Hz Frequency in Hertz of the LPI2C functional clock. Used to calculate the baud rate divisors,
 *      filter widths, and timeout periods.
 */
void LPI2C_MasterInit(LPI2C_Type *base, const lpi2c_master_config_t *masterConfig, uint32_t sourceClock_Hz);

/*!
 * @brief Deinitializes the LPI2C master peripheral.
 *
 * This function disables the LPI2C master peripheral and gates the clock. It also performs a software
 * reset to restore the peripheral to reset conditions.
 *
 * @param base The LPI2C peripheral base address.
 */
void LPI2C_MasterDeinit(LPI2C_Type *base);

/*!
 * @brief Configures LPI2C master data match feature.
 *
 * @param base The LPI2C peripheral base address.
 * @param config Settings for the data match feature.
 */
void LPI2C_MasterConfigureDataMatch(LPI2C_Type *base, const lpi2c_data_match_config_t *config);

/* Not static so it can be used from fsl_lpi2c_edma.c. */
status_t LPI2C_MasterCheckAndClearError(LPI2C_Type *base, uint32_t status);

/* Not static so it can be used from fsl_lpi2c_edma.c. */
status_t LPI2C_CheckForBusyBus(LPI2C_Type *base);

/*!
 * @brief Performs a software reset.
 *
 * Restores the LPI2C master peripheral to reset conditions.
 *
 * @param base The LPI2C peripheral base address.
 */
static inline void LPI2C_MasterReset(LPI2C_Type *base)
{
    base->MCR = LPI2C_MCR_RST_MASK;
    base->MCR = 0;
}

/*!
 * @brief Enables or disables the LPI2C module as master.
 *
 * @param base The LPI2C peripheral base address.
 * @param enable Pass true to enable or false to disable the specified LPI2C as master.
 */
static inline void LPI2C_MasterEnable(LPI2C_Type *base, bool enable)
{
    base->MCR = (base->MCR & ~LPI2C_MCR_MEN_MASK) | LPI2C_MCR_MEN(enable);
}

/*@}*/

/*! @name Status */
/*@{*/

/*!
 * @brief Gets the LPI2C master status flags.
 *
 * A bit mask with the state of all LPI2C master status flags is returned. For each flag, the corresponding bit
 * in the return value is set if the flag is asserted.
 *
 * @param base The LPI2C peripheral base address.
 * @return State of the status flags:
 *         - 1: related status flag is set.
 *         - 0: related status flag is not set.
 * @see _lpi2c_master_flags
 */
static inline uint32_t LPI2C_MasterGetStatusFlags(LPI2C_Type *base)
{
    return base->MSR;
}

/*!
 * @brief Clears the LPI2C master status flag state.
 *
 * The following status register flags can be cleared:
 * - #kLPI2C_MasterEndOfPacketFlag
 * - #kLPI2C_MasterStopDetectFlag
 * - #kLPI2C_MasterNackDetectFlag
 * - #kLPI2C_MasterArbitrationLostFlag
 * - #kLPI2C_MasterFifoErrFlag
 * - #kLPI2C_MasterPinLowTimeoutFlag
 * - #kLPI2C_MasterDataMatchFlag
 *
 * Attempts to clear other flags has no effect.
 *
 * @param base The LPI2C peripheral base address.
 * @param statusMask A bitmask of status flags that are to be cleared. The mask is composed of
 *  #_lpi2c_master_flags enumerators OR'd together. You may pass the result of a previous call to
 *  LPI2C_MasterGetStatusFlags().
 * @see _lpi2c_master_flags.
 */
static inline void LPI2C_MasterClearStatusFlags(LPI2C_Type *base, uint32_t statusMask)
{
    base->MSR = statusMask;
}

/*@}*/

/*! @name Interrupts */
/*@{*/

/*!
 * @brief Enables the LPI2C master interrupt requests.
 *
 * All flags except #kLPI2C_MasterBusyFlag and #kLPI2C_MasterBusBusyFlag can be enabled as
 * interrupts.
 *
 * @param base The LPI2C peripheral base address.
 * @param interruptMask Bit mask of interrupts to enable. See #_lpi2c_master_flags for the set
 *      of constants that should be OR'd together to form the bit mask.
 */
static inline void LPI2C_MasterEnableInterrupts(LPI2C_Type *base, uint32_t interruptMask)
{
    base->MIER |= interruptMask;
}

/*!
 * @brief Disables the LPI2C master interrupt requests.
 *
 * All flags except #kLPI2C_MasterBusyFlag and #kLPI2C_MasterBusBusyFlag can be enabled as
 * interrupts.
 *
 * @param base The LPI2C peripheral base address.
 * @param interruptMask Bit mask of interrupts to disable. See #_lpi2c_master_flags for the set
 *      of constants that should be OR'd together to form the bit mask.
 */
static inline void LPI2C_MasterDisableInterrupts(LPI2C_Type *base, uint32_t interruptMask)
{
    base->MIER &= ~interruptMask;
}

/*!
 * @brief Returns the set of currently enabled LPI2C master interrupt requests.
 *
 * @param base The LPI2C peripheral base address.
 * @return A bitmask composed of #_lpi2c_master_flags enumerators OR'd together to indicate the
 *      set of enabled interrupts.
 */
static inline uint32_t LPI2C_MasterGetEnabledInterrupts(LPI2C_Type *base)
{
    return base->MIER;
}

/*@}*/

/*! @name DMA control */
/*@{*/

/*!
 * @brief Enables or disables LPI2C master DMA requests.
 *
 * @param base The LPI2C peripheral base address.
 * @param enableTx Enable flag for transmit DMA request. Pass true for enable, false for disable.
 * @param enableRx Enable flag for receive DMA request. Pass true for enable, false for disable.
 */
static inline void LPI2C_MasterEnableDMA(LPI2C_Type *base, bool enableTx, bool enableRx)
{
    base->MDER = LPI2C_MDER_TDDE(enableTx) | LPI2C_MDER_RDDE(enableRx);
}

/*!
 * @brief Gets LPI2C master transmit data register address for DMA transfer.
 *
 * @param base The LPI2C peripheral base address.
 * @return The LPI2C Master Transmit Data Register address.
 */
static inline uint32_t LPI2C_MasterGetTxFifoAddress(LPI2C_Type *base)
{
    return (uint32_t)&base->MTDR;
}

/*!
 * @brief Gets LPI2C master receive data register address for DMA transfer.
 *
 * @param base The LPI2C peripheral base address.
 * @return The LPI2C Master Receive Data Register address.
 */
static inline uint32_t LPI2C_MasterGetRxFifoAddress(LPI2C_Type *base)
{
    return (uint32_t)&base->MRDR;
}

/*@}*/

/*! @name FIFO control */
/*@{*/

/*!
 * @brief Sets the watermarks for LPI2C master FIFOs.
 *
 * @param base The LPI2C peripheral base address.
 * @param txWords Transmit FIFO watermark value in words. The #kLPI2C_MasterTxReadyFlag flag is set whenever
 *      the number of words in the transmit FIFO is equal or less than @a txWords. Writing a value equal or
 *      greater than the FIFO size is truncated.
 * @param rxWords Receive FIFO watermark value in words. The #kLPI2C_MasterRxReadyFlag flag is set whenever
 *      the number of words in the receive FIFO is greater than @a rxWords. Writing a value equal or greater
 *      than the FIFO size is truncated.
 */
static inline void LPI2C_MasterSetWatermarks(LPI2C_Type *base, size_t txWords, size_t rxWords)
{
    base->MFCR = LPI2C_MFCR_TXWATER(txWords) | LPI2C_MFCR_RXWATER(rxWords);
}

/*!
 * @brief Gets the current number of words in the LPI2C master FIFOs.
 *
 * @param base The LPI2C peripheral base address.
 * @param[out] txCount Pointer through which the current number of words in the transmit FIFO is returned.
 *      Pass NULL if this value is not required.
 * @param[out] rxCount Pointer through which the current number of words in the receive FIFO is returned.
 *      Pass NULL if this value is not required.
 */
static inline void LPI2C_MasterGetFifoCounts(LPI2C_Type *base, size_t *rxCount, size_t *txCount)
{
    if (txCount)
    {
        *txCount = (base->MFSR & LPI2C_MFSR_TXCOUNT_MASK) >> LPI2C_MFSR_TXCOUNT_SHIFT;
    }
    if (rxCount)
    {
        *rxCount = (base->MFSR & LPI2C_MFSR_RXCOUNT_MASK) >> LPI2C_MFSR_RXCOUNT_SHIFT;
    }
}

/*@}*/

/*! @name Bus operations */
/*@{*/

/*!
 * @brief Sets the I2C bus frequency for master transactions.
 *
 * The LPI2C master is automatically disabled and re-enabled as necessary to configure the baud
 * rate. Do not call this function during a transfer, or the transfer is aborted.
 *
 * @note Please note that the second parameter is the clock frequency of LPI2C module, the third
 * parameter means user configured bus baudrate, this implementation is different from other I2C drivers
 * which use baudrate configuration as second parameter and source clock frequency as third parameter.
 *
 * @param base The LPI2C peripheral base address.
 * @param sourceClock_Hz LPI2C functional clock frequency in Hertz.
 * @param baudRate_Hz Requested bus frequency in Hertz.
 */
void LPI2C_MasterSetBaudRate(LPI2C_Type *base, uint32_t sourceClock_Hz, uint32_t baudRate_Hz);

/*!
 * @brief Returns whether the bus is idle.
 *
 * Requires the master mode to be enabled.
 *
 * @param base The LPI2C peripheral base address.
 * @retval true Bus is busy.
 * @retval false Bus is idle.
 */
static inline bool LPI2C_MasterGetBusIdleState(LPI2C_Type *base)
{
    return (base->MSR & LPI2C_MSR_BBF_MASK) >> LPI2C_MSR_BBF_SHIFT;
}

/*!
 * @brief Sends a START signal and slave address on the I2C bus.
 *
 * This function is used to initiate a new master mode transfer. First, the bus state is checked to ensure
 * that another master is not occupying the bus. Then a START signal is transmitted, followed by the
 * 7-bit address specified in the @a address parameter. Note that this function does not actually wait
 * until the START and address are successfully sent on the bus before returning.
 *
 * @param base The LPI2C peripheral base address.
 * @param address 7-bit slave device address, in bits [6:0].
 * @param dir Master transfer direction, either #kLPI2C_Read or #kLPI2C_Write. This parameter is used to set
 *      the R/w bit (bit 0) in the transmitted slave address.
 * @retval #kStatus_Success START signal and address were successfully enqueued in the transmit FIFO.
 * @retval #kStatus_LPI2C_Busy Another master is currently utilizing the bus.
 */
status_t LPI2C_MasterStart(LPI2C_Type *base, uint8_t address, lpi2c_direction_t dir);

/*!
 * @brief Sends a repeated START signal and slave address on the I2C bus.
 *
 * This function is used to send a Repeated START signal when a transfer is already in progress. Like
 * LPI2C_MasterStart(), it also sends the specified 7-bit address.
 *
 * @note This function exists primarily to maintain compatible APIs between LPI2C and I2C drivers,
 *      as well as to better document the intent of code that uses these APIs.
 *
 * @param base The LPI2C peripheral base address.
 * @param address 7-bit slave device address, in bits [6:0].
 * @param dir Master transfer direction, either #kLPI2C_Read or #kLPI2C_Write. This parameter is used to set
 *      the R/w bit (bit 0) in the transmitted slave address.
 * @retval #kStatus_Success Repeated START signal and address were successfully enqueued in the transmit FIFO.
 * @retval #kStatus_LPI2C_Busy Another master is currently utilizing the bus.
 */
static inline status_t LPI2C_MasterRepeatedStart(LPI2C_Type *base, uint8_t address, lpi2c_direction_t dir)
{
    return LPI2C_MasterStart(base, address, dir);
}

/*!
 * @brief Performs a polling send transfer on the I2C bus.
 *
 * Sends up to @a txSize number of bytes to the previously addressed slave device. The slave may
 * reply with a NAK to any byte in order to terminate the transfer early. If this happens, this
 * function returns #kStatus_LPI2C_Nak.
 *
 * @param base  The LPI2C peripheral base address.
 * @param txBuff The pointer to the data to be transferred.
 * @param txSize The length in bytes of the data to be transferred.
 * @retval #kStatus_Success Data was sent successfully.
 * @retval #kStatus_LPI2C_Busy Another master is currently utilizing the bus.
 * @retval #kStatus_LPI2C_Nak The slave device sent a NAK in response to a byte.
 * @retval #kStatus_LPI2C_FifoError FIFO under run or over run.
 * @retval #kStatus_LPI2C_ArbitrationLost Arbitration lost error.
 * @retval #kStatus_LPI2C_PinLowTimeout SCL or SDA were held low longer than the timeout.
 */
status_t LPI2C_MasterSend(LPI2C_Type *base, void *txBuff, size_t txSize);

/*!
 * @brief Performs a polling receive transfer on the I2C bus.
 *
 * @param base  The LPI2C peripheral base address.
 * @param rxBuff The pointer to the data to be transferred.
 * @param rxSize The length in bytes of the data to be transferred.
 * @retval #kStatus_Success Data was received successfully.
 * @retval #kStatus_LPI2C_Busy Another master is currently utilizing the bus.
 * @retval #kStatus_LPI2C_Nak The slave device sent a NAK in response to a byte.
 * @retval #kStatus_LPI2C_FifoError FIFO under run or overrun.
 * @retval #kStatus_LPI2C_ArbitrationLost Arbitration lost error.
 * @retval #kStatus_LPI2C_PinLowTimeout SCL or SDA were held low longer than the timeout.
 */
status_t LPI2C_MasterReceive(LPI2C_Type *base, void *rxBuff, size_t rxSize);

/*!
 * @brief Sends a STOP signal on the I2C bus.
 *
 * This function does not return until the STOP signal is seen on the bus, or an error occurs.
 *
 * @param base The LPI2C peripheral base address.
 * @retval #kStatus_Success The STOP signal was successfully sent on the bus and the transaction terminated.
 * @retval #kStatus_LPI2C_Busy Another master is currently utilizing the bus.
 * @retval #kStatus_LPI2C_Nak The slave device sent a NAK in response to a byte.
 * @retval #kStatus_LPI2C_FifoError FIFO under run or overrun.
 * @retval #kStatus_LPI2C_ArbitrationLost Arbitration lost error.
 * @retval #kStatus_LPI2C_PinLowTimeout SCL or SDA were held low longer than the timeout.
 */
status_t LPI2C_MasterStop(LPI2C_Type *base);

/*!
 * @brief Performs a master polling transfer on the I2C bus.
 *
 * @note The API does not return until the transfer succeeds or fails due
 * to error happens during transfer.
 *
 * @param base The LPI2C peripheral base address.
 * @param transfer Pointer to the transfer structure.
 * @retval #kStatus_Success Data was received successfully.
 * @retval #kStatus_LPI2C_Busy Another master is currently utilizing the bus.
 * @retval #kStatus_LPI2C_Nak The slave device sent a NAK in response to a byte.
 * @retval #kStatus_LPI2C_FifoError FIFO under run or overrun.
 * @retval #kStatus_LPI2C_ArbitrationLost Arbitration lost error.
 * @retval #kStatus_LPI2C_PinLowTimeout SCL or SDA were held low longer than the timeout.
 */
status_t LPI2C_MasterTransferBlocking(LPI2C_Type *base, lpi2c_master_transfer_t *transfer);

/*@}*/

/*! @name Non-blocking */
/*@{*/

/*!
 * @brief Creates a new handle for the LPI2C master non-blocking APIs.
 *
 * The creation of a handle is for use with the non-blocking APIs. Once a handle
 * is created, there is not a corresponding destroy handle. If the user wants to
 * terminate a transfer, the LPI2C_MasterTransferAbort() API shall be called.
 *
 *
 * @note The function also enables the NVIC IRQ for the input LPI2C. Need to notice
 * that on some SoCs the LPI2C IRQ is connected to INTMUX, in this case user needs to
 * enable the associated INTMUX IRQ in application.
 *
 * @param base The LPI2C peripheral base address.
 * @param[out] handle Pointer to the LPI2C master driver handle.
 * @param callback User provided pointer to the asynchronous callback function.
 * @param userData User provided pointer to the application callback data.
 */
void LPI2C_MasterTransferCreateHandle(LPI2C_Type *base,
                                      lpi2c_master_handle_t *handle,
                                      lpi2c_master_transfer_callback_t callback,
                                      void *userData);

/*!
 * @brief Performs a non-blocking transaction on the I2C bus.
 *
 * @param base The LPI2C peripheral base address.
 * @param handle Pointer to the LPI2C master driver handle.
 * @param transfer The pointer to the transfer descriptor.
 * @retval #kStatus_Success The transaction was started successfully.
 * @retval #kStatus_LPI2C_Busy Either another master is currently utilizing the bus, or a non-blocking
 *      transaction is already in progress.
 */
status_t LPI2C_MasterTransferNonBlocking(LPI2C_Type *base,
                                         lpi2c_master_handle_t *handle,
                                         lpi2c_master_transfer_t *transfer);

/*!
 * @brief Returns number of bytes transferred so far.
 * @param base The LPI2C peripheral base address.
 * @param handle Pointer to the LPI2C master driver handle.
 * @param[out] count Number of bytes transferred so far by the non-blocking transaction.
 * @retval #kStatus_Success
 * @retval #kStatus_NoTransferInProgress There is not a non-blocking transaction currently in progress.
 */
status_t LPI2C_MasterTransferGetCount(LPI2C_Type *base, lpi2c_master_handle_t *handle, size_t *count);

/*!
 * @brief Terminates a non-blocking LPI2C master transmission early.
 *
 * @note It is not safe to call this function from an IRQ handler that has a higher priority than the
 *      LPI2C peripheral's IRQ priority.
 *
 * @param base The LPI2C peripheral base address.
 * @param handle Pointer to the LPI2C master driver handle.
 * @retval #kStatus_Success A transaction was successfully aborted.
 * @retval #kStatus_LPI2C_Idle There is not a non-blocking transaction currently in progress.
 */
void LPI2C_MasterTransferAbort(LPI2C_Type *base, lpi2c_master_handle_t *handle);

/*@}*/

/*! @name IRQ handler */
/*@{*/

/*!
 * @brief Reusable routine to handle master interrupts.
 * @note This function does not need to be called unless you are reimplementing the
 *  nonblocking API's interrupt handler routines to add special functionality.
 * @param base The LPI2C peripheral base address.
 * @param handle Pointer to the LPI2C master driver handle.
 */
void LPI2C_MasterTransferHandleIRQ(LPI2C_Type *base, lpi2c_master_handle_t *handle);

/*@}*/

/*! @} */

/*!
 * @addtogroup lpi2c_slave_driver
 * @{
 */

/*! @name Slave initialization and deinitialization */
/*@{*/

/*!
 * @brief Provides a default configuration for the LPI2C slave peripheral.
 *
 * This function provides the following default configuration for the LPI2C slave peripheral:
 * @code
 *  slaveConfig->enableSlave               = true;
 *  slaveConfig->address0                  = 0U;
 *  slaveConfig->address1                  = 0U;
 *  slaveConfig->addressMatchMode          = kLPI2C_MatchAddress0;
 *  slaveConfig->filterDozeEnable          = true;
 *  slaveConfig->filterEnable              = true;
 *  slaveConfig->enableGeneralCall         = false;
 *  slaveConfig->sclStall.enableAck        = false;
 *  slaveConfig->sclStall.enableTx         = true;
 *  slaveConfig->sclStall.enableRx         = true;
 *  slaveConfig->sclStall.enableAddress    = true;
 *  slaveConfig->ignoreAck                 = false;
 *  slaveConfig->enableReceivedAddressRead = false;
 *  slaveConfig->sdaGlitchFilterWidth_ns   = 0;
 *  slaveConfig->sclGlitchFilterWidth_ns   = 0;
 *  slaveConfig->dataValidDelay_ns         = 0;
 *  slaveConfig->clockHoldTime_ns          = 0;
 * @endcode
 *
 * After calling this function, override any settings  to customize the configuration,
 * prior to initializing the master driver with LPI2C_SlaveInit(). Be sure to override at least the @a
 * address0 member of the configuration structure with the desired slave address.
 *
 * @param[out] slaveConfig User provided configuration structure that is set to default values. Refer to
 *      #lpi2c_slave_config_t.
 */
void LPI2C_SlaveGetDefaultConfig(lpi2c_slave_config_t *slaveConfig);

/*!
 * @brief Initializes the LPI2C slave peripheral.
 *
 * This function enables the peripheral clock and initializes the LPI2C slave peripheral as described by the user
 * provided configuration.
 *
 * @param base The LPI2C peripheral base address.
 * @param slaveConfig User provided peripheral configuration. Use LPI2C_SlaveGetDefaultConfig() to get a set of defaults
 *      that you can override.
 * @param sourceClock_Hz Frequency in Hertz of the LPI2C functional clock. Used to calculate the filter widths,
 *      data valid delay, and clock hold time.
 */
void LPI2C_SlaveInit(LPI2C_Type *base, const lpi2c_slave_config_t *slaveConfig, uint32_t sourceClock_Hz);

/*!
 * @brief Deinitializes the LPI2C slave peripheral.
 *
 * This function disables the LPI2C slave peripheral and gates the clock. It also performs a software
 * reset to restore the peripheral to reset conditions.
 *
 * @param base The LPI2C peripheral base address.
 */
void LPI2C_SlaveDeinit(LPI2C_Type *base);

/*!
 * @brief Performs a software reset of the LPI2C slave peripheral.
 *
 * @param base The LPI2C peripheral base address.
 */
static inline void LPI2C_SlaveReset(LPI2C_Type *base)
{
    base->SCR = LPI2C_SCR_RST_MASK;
    base->SCR = 0;
}

/*!
 * @brief Enables or disables the LPI2C module as slave.
 *
 * @param base The LPI2C peripheral base address.
 * @param enable Pass true to enable or false to disable the specified LPI2C as slave.
 */
static inline void LPI2C_SlaveEnable(LPI2C_Type *base, bool enable)
{
    base->SCR = (base->SCR & ~LPI2C_SCR_SEN_MASK) | LPI2C_SCR_SEN(enable);
}

/*@}*/

/*! @name Slave status */
/*@{*/

/*!
 * @brief Gets the LPI2C slave status flags.
 *
 * A bit mask with the state of all LPI2C slave status flags is returned. For each flag, the corresponding bit
 * in the return value is set if the flag is asserted.
 *
 * @param base The LPI2C peripheral base address.
 * @return State of the status flags:
 *         - 1: related status flag is set.
 *         - 0: related status flag is not set.
 * @see _lpi2c_slave_flags
 */
static inline uint32_t LPI2C_SlaveGetStatusFlags(LPI2C_Type *base)
{
    return base->SSR;
}

/*!
 * @brief Clears the LPI2C status flag state.
 *
 * The following status register flags can be cleared:
 * - #kLPI2C_SlaveRepeatedStartDetectFlag
 * - #kLPI2C_SlaveStopDetectFlag
 * - #kLPI2C_SlaveBitErrFlag
 * - #kLPI2C_SlaveFifoErrFlag
 *
 * Attempts to clear other flags has no effect.
 *
 * @param base The LPI2C peripheral base address.
 * @param statusMask A bitmask of status flags that are to be cleared. The mask is composed of
 *  #_lpi2c_slave_flags enumerators OR'd together. You may pass the result of a previous call to
 *  LPI2C_SlaveGetStatusFlags().
 * @see _lpi2c_slave_flags.
 */
static inline void LPI2C_SlaveClearStatusFlags(LPI2C_Type *base, uint32_t statusMask)
{
    base->SSR = statusMask;
}

/*@}*/

/*! @name Slave interrupts */
/*@{*/

/*!
 * @brief Enables the LPI2C slave interrupt requests.
 *
 * All flags except #kLPI2C_SlaveBusyFlag and #kLPI2C_SlaveBusBusyFlag can be enabled as
 * interrupts.
 *
 * @param base The LPI2C peripheral base address.
 * @param interruptMask Bit mask of interrupts to enable. See #_lpi2c_slave_flags for the set
 *      of constants that should be OR'd together to form the bit mask.
 */
static inline void LPI2C_SlaveEnableInterrupts(LPI2C_Type *base, uint32_t interruptMask)
{
    base->SIER |= interruptMask;
}

/*!
 * @brief Disables the LPI2C slave interrupt requests.
 *
 * All flags except #kLPI2C_SlaveBusyFlag and #kLPI2C_SlaveBusBusyFlag can be enabled as
 * interrupts.
 *
 * @param base The LPI2C peripheral base address.
 * @param interruptMask Bit mask of interrupts to disable. See #_lpi2c_slave_flags for the set
 *      of constants that should be OR'd together to form the bit mask.
 */
static inline void LPI2C_SlaveDisableInterrupts(LPI2C_Type *base, uint32_t interruptMask)
{
    base->SIER &= ~interruptMask;
}

/*!
 * @brief Returns the set of currently enabled LPI2C slave interrupt requests.
 * @param base The LPI2C peripheral base address.
 * @return A bitmask composed of #_lpi2c_slave_flags enumerators OR'd together to indicate the
 *      set of enabled interrupts.
 */
static inline uint32_t LPI2C_SlaveGetEnabledInterrupts(LPI2C_Type *base)
{
    return base->SIER;
}

/*@}*/

/*! @name Slave DMA control */
/*@{*/

/*!
 * @brief Enables or disables the LPI2C slave peripheral DMA requests.
 *
 * @param base The LPI2C peripheral base address.
 * @param enableAddressValid Enable flag for the address valid DMA request. Pass true for enable, false for disable.
 *      The address valid DMA request is shared with the receive data DMA request.
 * @param enableRx Enable flag for the receive data DMA request. Pass true for enable, false for disable.
 * @param enableTx Enable flag for the transmit data DMA request. Pass true for enable, false for disable.
 */
static inline void LPI2C_SlaveEnableDMA(LPI2C_Type *base, bool enableAddressValid, bool enableRx, bool enableTx)
{
    base->SDER = (base->SDER & ~(LPI2C_SDER_AVDE_MASK | LPI2C_SDER_RDDE_MASK | LPI2C_SDER_TDDE_MASK)) |
                 LPI2C_SDER_AVDE(enableAddressValid) | LPI2C_SDER_RDDE(enableRx) | LPI2C_SDER_TDDE(enableTx);
}

/*@}*/

/*! @name Slave bus operations */
/*@{*/

/*!
 * @brief Returns whether the bus is idle.
 *
 * Requires the slave mode to be enabled.
 *
 * @param base The LPI2C peripheral base address.
 * @retval true Bus is busy.
 * @retval false Bus is idle.
 */
static inline bool LPI2C_SlaveGetBusIdleState(LPI2C_Type *base)
{
    return (base->SSR & LPI2C_SSR_BBF_MASK) >> LPI2C_SSR_BBF_SHIFT;
}

/*!
 * @brief Transmits either an ACK or NAK on the I2C bus in response to a byte from the master.
 *
 * Use this function to send an ACK or NAK when the #kLPI2C_SlaveTransmitAckFlag is asserted. This
 * only happens if you enable the sclStall.enableAck field of the ::lpi2c_slave_config_t configuration
 * structure used to initialize the slave peripheral.
 *
 * @param base The LPI2C peripheral base address.
 * @param ackOrNack Pass true for an ACK or false for a NAK.
 */
static inline void LPI2C_SlaveTransmitAck(LPI2C_Type *base, bool ackOrNack)
{
    base->STAR = LPI2C_STAR_TXNACK(!ackOrNack);
}

/*!
 * @brief Returns the slave address sent by the I2C master.
 *
 * This function should only be called if the #kLPI2C_SlaveAddressValidFlag is asserted.
 *
 * @param base The LPI2C peripheral base address.
 * @return The 8-bit address matched by the LPI2C slave. Bit 0 contains the R/w direction bit, and
 *      the 7-bit slave address is in the upper 7 bits.
 */
static inline uint32_t LPI2C_SlaveGetReceivedAddress(LPI2C_Type *base)
{
    return base->SASR & LPI2C_SASR_RADDR_MASK;
}

/*!
 * @brief Performs a polling send transfer on the I2C bus.
 *
 * @param base  The LPI2C peripheral base address.
 * @param txBuff The pointer to the data to be transferred.
 * @param txSize The length in bytes of the data to be transferred.
 * @param[out] actualTxSize
 * @return Error or success status returned by API.
 */
status_t LPI2C_SlaveSend(LPI2C_Type *base, void *txBuff, size_t txSize, size_t *actualTxSize);

/*!
 * @brief Performs a polling receive transfer on the I2C bus.
 *
 * @param base  The LPI2C peripheral base address.
 * @param rxBuff The pointer to the data to be transferred.
 * @param rxSize The length in bytes of the data to be transferred.
 * @param[out] actualRxSize
 * @return Error or success status returned by API.
 */
status_t LPI2C_SlaveReceive(LPI2C_Type *base, void *rxBuff, size_t rxSize, size_t *actualRxSize);

/*@}*/

/*! @name Slave non-blocking */
/*@{*/

/*!
 * @brief Creates a new handle for the LPI2C slave non-blocking APIs.
 *
 * The creation of a handle is for use with the non-blocking APIs. Once a handle
 * is created, there is not a corresponding destroy handle. If the user wants to
 * terminate a transfer, the LPI2C_SlaveTransferAbort() API shall be called.
 *
 * @note The function also enables the NVIC IRQ for the input LPI2C. Need to notice
 * that on some SoCs the LPI2C IRQ is connected to INTMUX, in this case user needs to
 * enable the associated INTMUX IRQ in application.

 * @param base The LPI2C peripheral base address.
 * @param[out] handle Pointer to the LPI2C slave driver handle.
 * @param callback User provided pointer to the asynchronous callback function.
 * @param userData User provided pointer to the application callback data.
 */
void LPI2C_SlaveTransferCreateHandle(LPI2C_Type *base,
                                     lpi2c_slave_handle_t *handle,
                                     lpi2c_slave_transfer_callback_t callback,
                                     void *userData);

/*!
 * @brief Starts accepting slave transfers.
 *
 * Call this API after calling I2C_SlaveInit() and LPI2C_SlaveTransferCreateHandle() to start processing
 * transactions driven by an I2C master. The slave monitors the I2C bus and pass events to the
 * callback that was passed into the call to LPI2C_SlaveTransferCreateHandle(). The callback is always invoked
 * from the interrupt context.
 *
 * The set of events received by the callback is customizable. To do so, set the @a eventMask parameter to
 * the OR'd combination of #lpi2c_slave_transfer_event_t enumerators for the events you wish to receive.
 * The #kLPI2C_SlaveTransmitEvent and #kLPI2C_SlaveReceiveEvent events are always enabled and do not need
 * to be included in the mask. Alternatively, you can pass 0 to get a default set of only the transmit and
 * receive events that are always enabled. In addition, the #kLPI2C_SlaveAllEvents constant is provided as
 * a convenient way to enable all events.
 *
 * @param base The LPI2C peripheral base address.
 * @param handle Pointer to #lpi2c_slave_handle_t structure which stores the transfer state.
 * @param eventMask Bit mask formed by OR'ing together #lpi2c_slave_transfer_event_t enumerators to specify
 *      which events to send to the callback. Other accepted values are 0 to get a default set of
 *      only the transmit and receive events, and #kLPI2C_SlaveAllEvents to enable all events.
 *
 * @retval #kStatus_Success Slave transfers were successfully started.
 * @retval #kStatus_LPI2C_Busy Slave transfers have already been started on this handle.
 */
status_t LPI2C_SlaveTransferNonBlocking(LPI2C_Type *base, lpi2c_slave_handle_t *handle, uint32_t eventMask);

/*!
 * @brief Gets the slave transfer status during a non-blocking transfer.
 * @param base The LPI2C peripheral base address.
 * @param handle Pointer to i2c_slave_handle_t structure.
 * @param[out] count Pointer to a value to hold the number of bytes transferred. May be NULL if the count is not
 *      required.
 * @retval #kStatus_Success
 * @retval #kStatus_NoTransferInProgress
 */
status_t LPI2C_SlaveTransferGetCount(LPI2C_Type *base, lpi2c_slave_handle_t *handle, size_t *count);

/*!
 * @brief Aborts the slave non-blocking transfers.
 * @note This API could be called at any time to stop slave for handling the bus events.
 * @param base The LPI2C peripheral base address.
 * @param handle Pointer to #lpi2c_slave_handle_t structure which stores the transfer state.
 * @retval #kStatus_Success
 * @retval #kStatus_LPI2C_Idle
 */
void LPI2C_SlaveTransferAbort(LPI2C_Type *base, lpi2c_slave_handle_t *handle);

/*@}*/

/*! @name Slave IRQ handler */
/*@{*/

/*!
 * @brief Reusable routine to handle slave interrupts.
 * @note This function does not need to be called unless you are reimplementing the
 *  non blocking API's interrupt handler routines to add special functionality.
 * @param base The LPI2C peripheral base address.
 * @param handle Pointer to #lpi2c_slave_handle_t structure which stores the transfer state.
 */
void LPI2C_SlaveTransferHandleIRQ(LPI2C_Type *base, lpi2c_slave_handle_t *handle);

/*@}*/

/*! @} */

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_LPI2C_H_ */
