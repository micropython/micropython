/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_FLEXIO_MCULCD_H_
#define _FSL_FLEXIO_MCULCD_H_

#include "fsl_common.h"
#include "fsl_flexio.h"

/*!
 * @addtogroup flexio_mculcd
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief FlexIO MCULCD driver version 2.0.2. */
#define FSL_FLEXIO_MCULCD_DRIVER_VERSION (MAKE_VERSION(2, 0, 2))
/*@}*/

#ifndef FLEXIO_MCULCD_WAIT_COMPLETE_TIME
/*!
 * @brief The delay time to wait for FLEXIO transmit complete.
 *
 * Currently there is no method to detect whether the data has been
 * sent out from the shifter, so the driver use a software delay for this. When
 * the data is written to shifter buffer, the driver call the delay
 * function to wait for the data shift out.
 * If this value is too small, then the last few bytes might be lost when writing
 * data using interrupt method or DMA method.
 */
#define FLEXIO_MCULCD_WAIT_COMPLETE_TIME 512
#endif

#ifndef FLEXIO_MCULCD_DATA_BUS_WIDTH
/*!
 * @brief The data bus width, must be 8 or 16.
 */
#define FLEXIO_MCULCD_DATA_BUS_WIDTH 16
#endif

#if (16 != FLEXIO_MCULCD_DATA_BUS_WIDTH) && (8 != FLEXIO_MCULCD_DATA_BUS_WIDTH)
#error Only support data bus 8-bit or 16-bit
#endif

/*! @brief FlexIO LCD transfer status */
enum _flexio_mculcd_status
{
    kStatus_FLEXIO_MCULCD_Idle  = MAKE_STATUS(kStatusGroup_FLEXIO_MCULCD, 0), /*!< FlexIO LCD is idle. */
    kStatus_FLEXIO_MCULCD_Busy  = MAKE_STATUS(kStatusGroup_FLEXIO_MCULCD, 1), /*!< FlexIO LCD is busy */
    kStatus_FLEXIO_MCULCD_Error = MAKE_STATUS(kStatusGroup_FLEXIO_MCULCD, 2), /*!< FlexIO LCD error occurred */
};

/*! @brief Define FlexIO MCULCD pixel format. */
typedef enum _flexio_mculcd_pixel_format
{
    kFLEXIO_MCULCD_RGB565 = 0, /*!< RGB565, 16-bit. */
    kFLEXIO_MCULCD_BGR565,     /*!< BGR565, 16-bit. */
    kFLEXIO_MCULCD_RGB888,     /*!< RGB888, 24-bit. */
    kFLEXIO_MCULCD_BGR888,     /*!< BGR888, 24-bit. */
} flexio_mculcd_pixel_format_t;

/*! @brief Define FlexIO MCULCD bus type. */
typedef enum _flexio_mculcd_bus
{
    kFLEXIO_MCULCD_8080, /*!< Using Intel 8080 bus. */
    kFLEXIO_MCULCD_6800, /*!< Using Motorola 6800 bus. */
} flexio_mculcd_bus_t;

/*! @brief Define FlexIO MCULCD interrupt mask. */
enum _flexio_mculcd_interrupt_enable
{
    kFLEXIO_MCULCD_TxEmptyInterruptEnable = (1U << 0U), /*!< Transmit buffer empty interrupt enable. */
    kFLEXIO_MCULCD_RxFullInterruptEnable  = (1U << 1U), /*!< Receive buffer full interrupt enable. */
};

/*! @brief Define FlexIO MCULCD status mask. */
enum _flexio_mculcd_status_flags
{
    kFLEXIO_MCULCD_TxEmptyFlag = (1U << 0U), /*!< Transmit buffer empty flag. */
    kFLEXIO_MCULCD_RxFullFlag  = (1U << 1U), /*!< Receive buffer full flag. */
};

/*! @brief Define FlexIO MCULCD DMA mask. */
enum _flexio_mculcd_dma_enable
{
    kFLEXIO_MCULCD_TxDmaEnable = 0x1U, /*!< Tx DMA request source */
    kFLEXIO_MCULCD_RxDmaEnable = 0x2U, /*!< Rx DMA request source */
};

/*! @brief Function to set or clear the CS and RS pin. */
typedef void (*flexio_mculcd_pin_func_t)(bool set);

/*! @brief Define FlexIO MCULCD access structure typedef. */
typedef struct _flexio_mculcd_type
{
    FLEXIO_Type *flexioBase;             /*!< FlexIO base pointer. */
    flexio_mculcd_bus_t busType;         /*!< The bus type, 8080 or 6800. */
    uint8_t dataPinStartIndex;           /*!< Start index of the data pin, the FlexIO pin dataPinStartIndex
                                              to (dataPinStartIndex + FLEXIO_MCULCD_DATA_BUS_WIDTH -1)
                                              will be used for data transfer. Only support data bus width 8 and 16. */
    uint8_t ENWRPinIndex;                /*!< Pin select for WR(8080 mode), EN(6800 mode). */
    uint8_t RDPinIndex;                  /*!< Pin select for RD(8080 mode), not used in 6800 mode. */
    uint8_t txShifterStartIndex;         /*!< Start index of shifters used for data write, it must be 0 or 4. */
    uint8_t txShifterEndIndex;           /*!< End index of shifters used for data write. */
    uint8_t rxShifterStartIndex;         /*!< Start index of shifters used for data read. */
    uint8_t rxShifterEndIndex;           /*!< End index of shifters used for data read, it must be 3 or 7.  */
    uint8_t timerIndex;                  /*!< Timer index used in FlexIO MCULCD. */
    flexio_mculcd_pin_func_t setCSPin;   /*!< Function to set or clear the CS pin. */
    flexio_mculcd_pin_func_t setRSPin;   /*!< Function to set or clear the RS pin. */
    flexio_mculcd_pin_func_t setRDWRPin; /*!< Function to set or clear the RD/WR pin, only used in 6800 mode. */
} FLEXIO_MCULCD_Type;

/*! @brief Define FlexIO MCULCD configuration structure. */
typedef struct _flexio_mculcd_config
{
    bool enable;           /*!< Enable/disable FlexIO MCULCD after configuration. */
    bool enableInDoze;     /*!< Enable/disable FlexIO operation in doze mode. */
    bool enableInDebug;    /*!< Enable/disable FlexIO operation in debug mode. */
    bool enableFastAccess; /*!< Enable/disable fast access to FlexIO registers,
                           fast access requires the FlexIO clock to be at least
                           twice the frequency of the bus clock. */
    uint32_t baudRate_Bps; /*!< Baud rate in Bps. */
} flexio_mculcd_config_t;

/*! @brief Transfer mode.*/
typedef enum _flexio_mculcd_transfer_mode
{
    kFLEXIO_MCULCD_ReadArray,      /*!< Read data into an array. */
    kFLEXIO_MCULCD_WriteArray,     /*!< Write data from an array. */
    kFLEXIO_MCULCD_WriteSameValue, /*!< Write the same value many times. */
} flexio_mculcd_transfer_mode_t;

/*! @brief Define FlexIO MCULCD transfer structure. */
typedef struct _flexio_mculcd_transfer
{
    uint32_t command;                   /*!< Command to send. */
    flexio_mculcd_transfer_mode_t mode; /*!< Transfer mode. */
    uint32_t dataAddrOrSameValue;       /*!< When sending the same value for many times,
                                           this is the value to send. When writing or reading array,
                                           this is the address of the data array. */
    size_t dataSize;                    /*!< How many bytes to transfer. */
} flexio_mculcd_transfer_t;

/*! @brief typedef for flexio_mculcd_handle_t in advance. */
typedef struct _flexio_mculcd_handle flexio_mculcd_handle_t;

/*! @brief FlexIO MCULCD callback for finished transfer.
 *
 * When transfer finished, the callback function is called and returns the
 * @p status as kStatus_FLEXIO_MCULCD_Idle.
 */
typedef void (*flexio_mculcd_transfer_callback_t)(FLEXIO_MCULCD_Type *base,
                                                  flexio_mculcd_handle_t *handle,
                                                  status_t status,
                                                  void *userData);

/*! @brief Define FlexIO MCULCD handle structure. */
struct _flexio_mculcd_handle
{
    uint32_t dataAddrOrSameValue;                         /*!< When sending the same value for many times,
                                                             this is the value to send. When writing or reading array,
                                                             this is the address of the data array. */
    size_t dataCount;                                     /*!< Total count to be transferred. */
    volatile size_t remainingCount;                       /*!< Remaining count to transfer. */
    volatile uint32_t state;                              /*!< FlexIO MCULCD internal state. */
    flexio_mculcd_transfer_callback_t completionCallback; /*!< FlexIO MCULCD transfer completed callback. */
    void *userData;                                       /*!< Callback parameter. */
};

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus*/

/*!
 * @name FlexIO MCULCD Configuration
 * @{
 */

/*!
 * @brief Ungates the FlexIO clock, resets the FlexIO module, configures the
 * FlexIO MCULCD hardware, and configures the FlexIO MCULCD with FlexIO MCULCD
 * configuration.
 * The configuration structure can be filled by the user, or be set with default
 * values
 * by the @ref FLEXIO_MCULCD_GetDefaultConfig.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 * @param config Pointer to the flexio_mculcd_config_t structure.
 * @param srcClock_Hz FlexIO source clock in Hz.
 * @retval kStatus_Success Initialization success.
 * @retval kStatus_InvalidArgument Initialization failed because of invalid
 * argument.
 */
status_t FLEXIO_MCULCD_Init(FLEXIO_MCULCD_Type *base, flexio_mculcd_config_t *config, uint32_t srcClock_Hz);

/*!
 * @brief Resets the FLEXIO_MCULCD timer and shifter configuration.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type.
 */
void FLEXIO_MCULCD_Deinit(FLEXIO_MCULCD_Type *base);

/*!
 * @brief Gets the default configuration to configure the FlexIO MCULCD.
 *
 * The default configuration value is:
 * @code
 *  config->enable = true;
 *  config->enableInDoze = false;
 *  config->enableInDebug = true;
 *  config->enableFastAccess = true;
 *  config->baudRate_Bps = 96000000U;
 * @endcode
 * @param Config Pointer to the flexio_mculcd_config_t structure.
 */
void FLEXIO_MCULCD_GetDefaultConfig(flexio_mculcd_config_t *config);

/*@}*/

/*!
 * @name Status
 * @{
 */

/*!
 * @brief Gets FlexIO MCULCD status flags.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 * @return status flag; OR'ed value or the @ref _flexio_mculcd_status_flags.
 *
 * @note Don't use this function with DMA APIs.
 */
uint32_t FLEXIO_MCULCD_GetStatusFlags(FLEXIO_MCULCD_Type *base);

/*!
 * @brief Clears FlexIO MCULCD status flags.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 * @param mask Status to clear, it is the OR'ed value of @ref
 * _flexio_mculcd_status_flags.
 *
 * @note Don't use this function with DMA APIs.
 */
void FLEXIO_MCULCD_ClearStatusFlags(FLEXIO_MCULCD_Type *base, uint32_t mask);

/*@}*/

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief Enables the FlexIO MCULCD interrupt.
 *
 * This function enables the FlexIO MCULCD interrupt.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 * @param mask Interrupts to enable, it is the OR'ed value of @ref
 * _flexio_mculcd_interrupt_enable.
 */
void FLEXIO_MCULCD_EnableInterrupts(FLEXIO_MCULCD_Type *base, uint32_t mask);

/*!
 * @brief Disables the FlexIO MCULCD interrupt.
 *
 * This function disables the FlexIO MCULCD interrupt.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 * @param mask Interrupts to disable, it is the OR'ed value of @ref
 * _flexio_mculcd_interrupt_enable.
 */
void FLEXIO_MCULCD_DisableInterrupts(FLEXIO_MCULCD_Type *base, uint32_t mask);

/*@}*/

/*!
 * @name DMA Control
 * @{
 */

/*!
 * @brief Enables/disables the FlexIO MCULCD transmit DMA.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 * @param mask MCULCD DMA source.
 * @param enable True means enable DMA, false means disable DMA.
 */
static inline void FLEXIO_MCULCD_EnableTxDMA(FLEXIO_MCULCD_Type *base, bool enable)
{
    FLEXIO_EnableShifterStatusDMA(base->flexioBase, (1U << base->txShifterStartIndex), enable);
}

/*!
 * @brief Enables/disables the FlexIO MCULCD receive DMA.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 * @param mask MCULCD DMA source.
 * @param enable True means enable DMA, false means disable DMA.
 */
static inline void FLEXIO_MCULCD_EnableRxDMA(FLEXIO_MCULCD_Type *base, bool enable)
{
    FLEXIO_EnableShifterStatusDMA(base->flexioBase, (1U << base->rxShifterEndIndex), enable);
}

/*!
 * @brief Gets the FlexIO MCULCD transmit data register address.
 *
 * This function returns the MCULCD data register address, which is mainly used
 * by DMA/eDMA.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 * @return FlexIO MCULCD transmit data register address.
 */
static inline uint32_t FLEXIO_MCULCD_GetTxDataRegisterAddress(FLEXIO_MCULCD_Type *base)
{
    return (uint32_t) & (base->flexioBase->SHIFTBUF[base->txShifterStartIndex]);
}

/*!
 * @brief Gets the FlexIO MCULCD receive data register address.
 *
 * This function returns the MCULCD data register address, which is mainly used
 * by DMA/eDMA.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 * @return FlexIO MCULCD receive data register address.
 */
static inline uint32_t FLEXIO_MCULCD_GetRxDataRegisterAddress(FLEXIO_MCULCD_Type *base)
{
    return (uint32_t) & (base->flexioBase->SHIFTBUF[base->rxShifterStartIndex]);
}

/*@}*/

/*!
 * @name Bus Operations
 * @{
 */

/*!
 * @brief Set desired baud rate.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 * @param baudRate_Bps Desired baud rate.
 * @param srcClock_Hz FLEXIO clock frequency in Hz.
 * @retval kStatus_Success Set successfully.
 * @retval kStatus_InvalidArgument Could not set the baud rate.
 */
status_t FLEXIO_MCULCD_SetBaudRate(FLEXIO_MCULCD_Type *base, uint32_t baudRate_Bps, uint32_t srcClock_Hz);

/*!
 * @brief Configures the FLEXIO MCULCD to multiple beats write mode.
 *
 * At the begining multiple beats write operation, the FLEXIO MCULCD is configured to
 * multiple beats write mode using this function. After write operation, the configuration
 * is cleared by @ref FLEXIO_MCULCD_ClearSingleBeatWriteConfig.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type.
 *
 * @note This is an internal used function, upper layer should not use.
 */
void FLEXIO_MCULCD_SetSingleBeatWriteConfig(FLEXIO_MCULCD_Type *base);

/*!
 * @brief Clear the FLEXIO MCULCD multiple beats write mode configuration.
 *
 * Clear the write configuration set by @ref FLEXIO_MCULCD_SetSingleBeatWriteConfig.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type.
 *
 * @note This is an internal used function, upper layer should not use.
 */
void FLEXIO_MCULCD_ClearSingleBeatWriteConfig(FLEXIO_MCULCD_Type *base);

/*!
 * @brief Configures the FLEXIO MCULCD to multiple beats read mode.
 *
 * At the begining or multiple beats read operation, the FLEXIO MCULCD is configured
 * to multiple beats read mode using this function. After read operation, the configuration
 * is cleared by @ref FLEXIO_MCULCD_ClearSingleBeatReadConfig.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type.
 *
 * @note This is an internal used function, upper layer should not use.
 */
void FLEXIO_MCULCD_SetSingleBeatReadConfig(FLEXIO_MCULCD_Type *base);

/*!
 * @brief Clear the FLEXIO MCULCD multiple beats read mode configuration.
 *
 * Clear the read configuration set by @ref FLEXIO_MCULCD_SetSingleBeatReadConfig.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type.
 *
 * @note This is an internal used function, upper layer should not use.
 */
void FLEXIO_MCULCD_ClearSingleBeatReadConfig(FLEXIO_MCULCD_Type *base);

/*!
 * @brief Configures the FLEXIO MCULCD to multiple beats write mode.
 *
 * At the begining multiple beats write operation, the FLEXIO MCULCD is configured to
 * multiple beats write mode using this function. After write operation, the configuration
 * is cleared by @ref FLEXIO_MCULCD_ClearMultBeatsWriteConfig.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type.
 *
 * @note This is an internal used function, upper layer should not use.
 */
void FLEXIO_MCULCD_SetMultiBeatsWriteConfig(FLEXIO_MCULCD_Type *base);

/*!
 * @brief Clear the FLEXIO MCULCD multiple beats write mode configuration.
 *
 * Clear the write configuration set by @ref FLEXIO_MCULCD_SetMultBeatsWriteConfig.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type.
 *
 * @note This is an internal used function, upper layer should not use.
 */
void FLEXIO_MCULCD_ClearMultiBeatsWriteConfig(FLEXIO_MCULCD_Type *base);

/*!
 * @brief Configures the FLEXIO MCULCD to multiple beats read mode.
 *
 * At the begining or multiple beats read operation, the FLEXIO MCULCD is configured
 * to multiple beats read mode using this function. After read operation, the configuration
 * is cleared by @ref FLEXIO_MCULCD_ClearMultBeatsReadConfig.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type.
 *
 * @note This is an internal used function, upper layer should not use.
 */
void FLEXIO_MCULCD_SetMultiBeatsReadConfig(FLEXIO_MCULCD_Type *base);

/*!
 * @brief Clear the FLEXIO MCULCD multiple beats read mode configuration.
 *
 * Clear the read configuration set by @ref FLEXIO_MCULCD_SetMultBeatsReadConfig.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type.
 *
 * @note This is an internal used function, upper layer should not use.
 */
void FLEXIO_MCULCD_ClearMultiBeatsReadConfig(FLEXIO_MCULCD_Type *base);

/*!
 * @brief Enables/disables the FlexIO MCULCD module operation.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type.
 * @param enable True to enable, false does not have any effect.
 */
static inline void FLEXIO_MCULCD_Enable(FLEXIO_MCULCD_Type *base, bool enable)
{
    if (enable)
    {
        FLEXIO_Enable(base->flexioBase, enable);
    }
}

/*!
 * @brief Read data from the FLEXIO MCULCD RX shifter buffer.
 *
 * Read data from the RX shift buffer directly, it does no check whether the
 * buffer is empty or not.
 *
 * If the data bus width is 8-bit:
 * @code
 * uint8_t value;
 * value = (uint8_t)FLEXIO_MCULCD_ReadData(base);
 * @endcode
 *
 * If the data bus width is 16-bit:
 * @code
 * uint16_t value;
 * value = (uint16_t)FLEXIO_MCULCD_ReadData(base);
 * @endcode
 *
 * @note This function returns the RX shifter buffer value (32-bit) directly.
 * The return value should be converted according to data bus width.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 * @return The data read out.
 *
 * @note Don't use this function with DMA APIs.
 */
uint32_t FLEXIO_MCULCD_ReadData(FLEXIO_MCULCD_Type *base);

/*!
 * @brief Write data into the FLEXIO MCULCD TX shifter buffer.
 *
 * Write data into the TX shift buffer directly, it does no check whether the
 * buffer is full or not.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 * @param data The data to write.
 *
 * @note Don't use this function with DMA APIs.
 */
static inline void FLEXIO_MCULCD_WriteData(FLEXIO_MCULCD_Type *base, uint32_t data)
{
    base->flexioBase->SHIFTBUF[base->txShifterStartIndex] = data;
}

/*!
 * @brief Assert the nCS to start transfer.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 */
static inline void FLEXIO_MCULCD_StartTransfer(FLEXIO_MCULCD_Type *base)
{
    base->setCSPin(false);
}

/*!
 * @brief De-assert the nCS to stop transfer.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 */
static inline void FLEXIO_MCULCD_StopTransfer(FLEXIO_MCULCD_Type *base)
{
    base->setCSPin(true);
}

/*!
 * @brief Wait for transmit data send out finished.
 *
 * Currently there is no effective method to wait for the data send out
 * from the shiter, so here use a while loop to wait.
 *
 * @note This is an internal used function.
 */
void FLEXIO_MCULCD_WaitTransmitComplete(void);

/*!
 * @brief Send command in blocking way.
 *
 * This function sends the command and returns when the command has been sent
 * out.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 * @param command The command to send.
 */
void FLEXIO_MCULCD_WriteCommandBlocking(FLEXIO_MCULCD_Type *base, uint32_t command);

/*!
 * @brief Send data array in blocking way.
 *
 * This function sends the data array and returns when the data sent out.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 * @param data The data array to send.
 * @param size How many bytes to write.
 */
void FLEXIO_MCULCD_WriteDataArrayBlocking(FLEXIO_MCULCD_Type *base, void *data, size_t size);

/*!
 * @brief Read data into array in blocking way.
 *
 * This function reads the data into array and returns when the data read
 * finished.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 * @param data The array to save the data.
 * @param size How many bytes to read.
 */
void FLEXIO_MCULCD_ReadDataArrayBlocking(FLEXIO_MCULCD_Type *base, void *data, size_t size);

/*!
 * @brief Send the same value many times in blocking way.
 *
 * This function sends the same value many times. It could be used to clear the
 * LCD screen. If the data bus width is 8, this function will send LSB 8 bits of
 * @p sameValue for @p size times. If the data bus is 16, this function will send
 * LSB 16 bits of @p sameValue for @p size / 2 times.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 * @param sameValue The same value to send.
 * @param size How many bytes to send.
 */
void FLEXIO_MCULCD_WriteSameValueBlocking(FLEXIO_MCULCD_Type *base, uint32_t sameValue, size_t size);

/*!
 * @brief Performs a polling transfer.
 *
 * @note The API does not return until the transfer finished.
 *
 * @param base pointer to FLEXIO_MCULCD_Type structure.
 * @param xfer pointer to flexio_mculcd_transfer_t structure.
 */
void FLEXIO_MCULCD_TransferBlocking(FLEXIO_MCULCD_Type *base, flexio_mculcd_transfer_t *xfer);
/*@}*/

/*!
 * @name Transactional
 * @{
 */

/*!
 * @brief Initializes the FlexIO MCULCD handle, which is used in transactional
 * functions.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 * @param handle Pointer to the flexio_mculcd_handle_t structure to store the
 * transfer state.
 * @param callback The callback function.
 * @param userData The parameter of the callback function.
 * @retval kStatus_Success Successfully create the handle.
 * @retval kStatus_OutOfRange The FlexIO type/handle/ISR table out of range.
 */
status_t FLEXIO_MCULCD_TransferCreateHandle(FLEXIO_MCULCD_Type *base,
                                            flexio_mculcd_handle_t *handle,
                                            flexio_mculcd_transfer_callback_t callback,
                                            void *userData);

/*!
 * @brief Transfer data using IRQ.
 *
 * This function sends data using IRQ. This is a non-blocking function, which
 * returns right away. When all data is sent out/received, the callback
 * function is called.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 * @param handle Pointer to the flexio_mculcd_handle_t structure to store the
 * transfer state.
 * @param xfer FlexIO MCULCD transfer structure. See #flexio_mculcd_transfer_t.
 * @retval kStatus_Success Successfully start a transfer.
 * @retval kStatus_InvalidArgument Input argument is invalid.
 * @retval kStatus_FLEXIO_MCULCD_Busy MCULCD is busy with another transfer.
 */
status_t FLEXIO_MCULCD_TransferNonBlocking(FLEXIO_MCULCD_Type *base,
                                           flexio_mculcd_handle_t *handle,
                                           flexio_mculcd_transfer_t *xfer);

/*!
 * @brief Aborts the data transfer, which used IRQ.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 * @param handle Pointer to the flexio_mculcd_handle_t structure to store the
 * transfer state.
 */
void FLEXIO_MCULCD_TransferAbort(FLEXIO_MCULCD_Type *base, flexio_mculcd_handle_t *handle);

/*!
 * @brief Gets the data transfer status which used IRQ.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 * @param handle Pointer to the flexio_mculcd_handle_t structure to store the
 * transfer state.
 * @param count How many bytes transferred so far by the non-blocking transaction.
 * @retval kStatus_Success Get the transferred count Successfully.
 * @retval kStatus_NoTransferInProgress No transfer in process.
 */
status_t FLEXIO_MCULCD_TransferGetCount(FLEXIO_MCULCD_Type *base, flexio_mculcd_handle_t *handle, size_t *count);

/*!
 * @brief FlexIO MCULCD IRQ handler function.
 *
 * @param base Pointer to the FLEXIO_MCULCD_Type structure.
 * @param handle Pointer to the flexio_mculcd_handle_t structure to store the
 * transfer state.
 */
void FLEXIO_MCULCD_TransferHandleIRQ(void *base, void *handle);

/*@}*/

#if defined(__cplusplus)
}
#endif /*_cplusplus*/
/*@}*/

#endif /*_FSL_FLEXIO_MCULCD_H_*/
