/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_FLEXIO_SPI_H_
#define _FSL_FLEXIO_SPI_H_

#include "fsl_common.h"
#include "fsl_flexio.h"

/*!
 * @addtogroup flexio_spi
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief FlexIO SPI driver version 2.1.3. */
#define FSL_FLEXIO_SPI_DRIVER_VERSION (MAKE_VERSION(2, 1, 3))
/*@}*/

#ifndef FLEXIO_SPI_DUMMYDATA
/*! @brief FlexIO SPI dummy transfer data, the data is sent while txData is NULL. */
#define FLEXIO_SPI_DUMMYDATA (0xFFFFU)
#endif

/*! @brief Error codes for the FlexIO SPI driver. */
enum _flexio_spi_status
{
    kStatus_FLEXIO_SPI_Busy  = MAKE_STATUS(kStatusGroup_FLEXIO_SPI, 1), /*!< FlexIO SPI is busy. */
    kStatus_FLEXIO_SPI_Idle  = MAKE_STATUS(kStatusGroup_FLEXIO_SPI, 2), /*!< SPI is idle */
    kStatus_FLEXIO_SPI_Error = MAKE_STATUS(kStatusGroup_FLEXIO_SPI, 3), /*!< FlexIO SPI error. */
};

/*! @brief FlexIO SPI clock phase configuration. */
typedef enum _flexio_spi_clock_phase
{
    kFLEXIO_SPI_ClockPhaseFirstEdge = 0x0U,  /*!< First edge on SPSCK occurs at the middle of the first
                                              *   cycle of a data transfer. */
    kFLEXIO_SPI_ClockPhaseSecondEdge = 0x1U, /*!< First edge on SPSCK occurs at the start of the
                                              *   first cycle of a data transfer. */
} flexio_spi_clock_phase_t;

/*! @brief FlexIO SPI data shifter direction options. */
typedef enum _flexio_spi_shift_direction
{
    kFLEXIO_SPI_MsbFirst = 0, /*!< Data transfers start with most significant bit. */
    kFLEXIO_SPI_LsbFirst = 1, /*!< Data transfers start with least significant bit. */
} flexio_spi_shift_direction_t;

/*! @brief FlexIO SPI data length mode options. */
typedef enum _flexio_spi_data_bitcount_mode
{
    kFLEXIO_SPI_8BitMode  = 0x08U, /*!< 8-bit data transmission mode. */
    kFLEXIO_SPI_16BitMode = 0x10U, /*!< 16-bit data transmission mode. */
} flexio_spi_data_bitcount_mode_t;

/*! @brief Define FlexIO SPI interrupt mask. */
enum _flexio_spi_interrupt_enable
{
    kFLEXIO_SPI_TxEmptyInterruptEnable = 0x1U, /*!< Transmit buffer empty interrupt enable. */
    kFLEXIO_SPI_RxFullInterruptEnable  = 0x2U, /*!< Receive buffer full interrupt enable. */
};

/*! @brief Define FlexIO SPI status mask. */
enum _flexio_spi_status_flags
{
    kFLEXIO_SPI_TxBufferEmptyFlag = 0x1U, /*!< Transmit buffer empty flag. */
    kFLEXIO_SPI_RxBufferFullFlag  = 0x2U, /*!< Receive buffer full flag. */
};

/*! @brief Define FlexIO SPI DMA mask. */
enum _flexio_spi_dma_enable
{
    kFLEXIO_SPI_TxDmaEnable  = 0x1U, /*!< Tx DMA request source */
    kFLEXIO_SPI_RxDmaEnable  = 0x2U, /*!< Rx DMA request source */
    kFLEXIO_SPI_DmaAllEnable = 0x3U, /*!< All DMA request source*/
};

/*! @brief Define FlexIO SPI transfer flags. */
enum _flexio_spi_transfer_flags
{
    kFLEXIO_SPI_8bitMsb  = 0x1U, /*!< FlexIO SPI 8-bit MSB first */
    kFLEXIO_SPI_8bitLsb  = 0x2U, /*!< FlexIO SPI 8-bit LSB first */
    kFLEXIO_SPI_16bitMsb = 0x9U, /*!< FlexIO SPI 16-bit MSB first */
    kFLEXIO_SPI_16bitLsb = 0xaU, /*!< FlexIO SPI 16-bit LSB first */
};

/*! @brief Define FlexIO SPI access structure typedef. */
typedef struct _flexio_spi_type
{
    FLEXIO_Type *flexioBase; /*!< FlexIO base pointer. */
    uint8_t SDOPinIndex;     /*!< Pin select for data output. */
    uint8_t SDIPinIndex;     /*!< Pin select for data input. */
    uint8_t SCKPinIndex;     /*!< Pin select for clock. */
    uint8_t CSnPinIndex;     /*!< Pin select for enable. */
    uint8_t shifterIndex[2]; /*!< Shifter index used in FlexIO SPI. */
    uint8_t timerIndex[2];   /*!< Timer index used in FlexIO SPI. */
} FLEXIO_SPI_Type;

/*! @brief Define FlexIO SPI master configuration structure. */
typedef struct _flexio_spi_master_config
{
    bool enableMaster;                        /*!< Enable/disable FlexIO SPI master after configuration. */
    bool enableInDoze;                        /*!< Enable/disable FlexIO operation in doze mode. */
    bool enableInDebug;                       /*!< Enable/disable FlexIO operation in debug mode. */
    bool enableFastAccess;                    /*!< Enable/disable fast access to FlexIO registers,
                                              fast access requires the FlexIO clock to be at least
                                              twice the frequency of the bus clock. */
    uint32_t baudRate_Bps;                    /*!< Baud rate in Bps. */
    flexio_spi_clock_phase_t phase;           /*!< Clock phase. */
    flexio_spi_data_bitcount_mode_t dataMode; /*!< 8bit or 16bit mode. */
} flexio_spi_master_config_t;

/*! @brief Define FlexIO SPI slave configuration structure. */
typedef struct _flexio_spi_slave_config
{
    bool enableSlave;                         /*!< Enable/disable FlexIO SPI slave after configuration. */
    bool enableInDoze;                        /*!< Enable/disable FlexIO operation in doze mode. */
    bool enableInDebug;                       /*!< Enable/disable FlexIO operation in debug mode. */
    bool enableFastAccess;                    /*!< Enable/disable fast access to FlexIO registers,
                                              fast access requires the FlexIO clock to be at least
                                              twice the frequency of the bus clock. */
    flexio_spi_clock_phase_t phase;           /*!< Clock phase. */
    flexio_spi_data_bitcount_mode_t dataMode; /*!< 8bit or 16bit mode. */
} flexio_spi_slave_config_t;

/*! @brief Define FlexIO SPI transfer structure. */
typedef struct _flexio_spi_transfer
{
    uint8_t *txData; /*!< Send buffer. */
    uint8_t *rxData; /*!< Receive buffer. */
    size_t dataSize; /*!< Transfer bytes. */
    uint8_t flags;   /*!< FlexIO SPI control flag, MSB first  or LSB first. */
} flexio_spi_transfer_t;

/*! @brief  typedef for flexio_spi_master_handle_t in advance. */
typedef struct _flexio_spi_master_handle flexio_spi_master_handle_t;

/*! @brief  Slave handle is the same with master handle. */
typedef flexio_spi_master_handle_t flexio_spi_slave_handle_t;

/*! @brief FlexIO SPI master callback for finished transmit */
typedef void (*flexio_spi_master_transfer_callback_t)(FLEXIO_SPI_Type *base,
                                                      flexio_spi_master_handle_t *handle,
                                                      status_t status,
                                                      void *userData);

/*! @brief FlexIO SPI slave callback for finished transmit */
typedef void (*flexio_spi_slave_transfer_callback_t)(FLEXIO_SPI_Type *base,
                                                     flexio_spi_slave_handle_t *handle,
                                                     status_t status,
                                                     void *userData);

/*! @brief Define FlexIO SPI handle structure. */
struct _flexio_spi_master_handle
{
    uint8_t *txData;                                /*!< Transfer buffer. */
    uint8_t *rxData;                                /*!< Receive buffer. */
    size_t transferSize;                            /*!< Total bytes to be transferred. */
    volatile size_t txRemainingBytes;               /*!< Send data remaining in bytes. */
    volatile size_t rxRemainingBytes;               /*!< Receive data remaining in bytes. */
    volatile uint32_t state;                        /*!< FlexIO SPI internal state. */
    uint8_t bytePerFrame;                           /*!< SPI mode, 2bytes or 1byte in a frame */
    flexio_spi_shift_direction_t direction;         /*!< Shift direction. */
    flexio_spi_master_transfer_callback_t callback; /*!< FlexIO SPI callback. */
    void *userData;                                 /*!< Callback parameter. */
};

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus*/

/*!
 * @name FlexIO SPI Configuration
 * @{
 */

/*!
 * @brief Ungates the FlexIO clock, resets the FlexIO module, configures the FlexIO SPI master hardware,
 * and configures the FlexIO SPI with FlexIO SPI master configuration. The
 * configuration structure can be filled by the user, or be set with default values
 * by the FLEXIO_SPI_MasterGetDefaultConfig().
 *
 * @note FlexIO SPI master only support CPOL = 0, which means clock inactive low.
 *
 * Example
   @code
   FLEXIO_SPI_Type spiDev = {
   .flexioBase = FLEXIO,
   .SDOPinIndex = 0,
   .SDIPinIndex = 1,
   .SCKPinIndex = 2,
   .CSnPinIndex = 3,
   .shifterIndex = {0,1},
   .timerIndex = {0,1}
   };
   flexio_spi_master_config_t config = {
   .enableMaster = true,
   .enableInDoze = false,
   .enableInDebug = true,
   .enableFastAccess = false,
   .baudRate_Bps = 500000,
   .phase = kFLEXIO_SPI_ClockPhaseFirstEdge,
   .direction = kFLEXIO_SPI_MsbFirst,
   .dataMode = kFLEXIO_SPI_8BitMode
   };
   FLEXIO_SPI_MasterInit(&spiDev, &config, srcClock_Hz);
   @endcode
 *
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @param masterConfig Pointer to the flexio_spi_master_config_t structure.
 * @param srcClock_Hz FlexIO source clock in Hz.
*/
void FLEXIO_SPI_MasterInit(FLEXIO_SPI_Type *base, flexio_spi_master_config_t *masterConfig, uint32_t srcClock_Hz);

/*!
 * @brief Resets the FlexIO SPI timer and shifter config.
 *
 * @param base Pointer to the FLEXIO_SPI_Type.
 */
void FLEXIO_SPI_MasterDeinit(FLEXIO_SPI_Type *base);

/*!
 * @brief Gets the default configuration to configure the FlexIO SPI master. The configuration
 * can be used directly by calling the FLEXIO_SPI_MasterConfigure().
 * Example:
   @code
   flexio_spi_master_config_t masterConfig;
   FLEXIO_SPI_MasterGetDefaultConfig(&masterConfig);
   @endcode
 * @param masterConfig Pointer to the flexio_spi_master_config_t structure.
*/
void FLEXIO_SPI_MasterGetDefaultConfig(flexio_spi_master_config_t *masterConfig);

/*!
 * @brief Ungates the FlexIO clock, resets the FlexIO module, configures the FlexIO SPI slave hardware
 * configuration, and configures the FlexIO SPI with FlexIO SPI slave configuration. The
 * configuration structure can be filled by the user, or be set with default values
 * by the FLEXIO_SPI_SlaveGetDefaultConfig().
 *
 * @note Only one timer is needed in the FlexIO SPI slave. As a result, the second timer index is ignored.
 * FlexIO SPI slave only support CPOL = 0, which means clock inactive low.
 * Example
   @code
   FLEXIO_SPI_Type spiDev = {
   .flexioBase = FLEXIO,
   .SDOPinIndex = 0,
   .SDIPinIndex = 1,
   .SCKPinIndex = 2,
   .CSnPinIndex = 3,
   .shifterIndex = {0,1},
   .timerIndex = {0}
   };
   flexio_spi_slave_config_t config = {
   .enableSlave = true,
   .enableInDoze = false,
   .enableInDebug = true,
   .enableFastAccess = false,
   .phase = kFLEXIO_SPI_ClockPhaseFirstEdge,
   .direction = kFLEXIO_SPI_MsbFirst,
   .dataMode = kFLEXIO_SPI_8BitMode
   };
   FLEXIO_SPI_SlaveInit(&spiDev, &config);
   @endcode
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @param slaveConfig Pointer to the flexio_spi_slave_config_t structure.
*/
void FLEXIO_SPI_SlaveInit(FLEXIO_SPI_Type *base, flexio_spi_slave_config_t *slaveConfig);

/*!
 * @brief Gates the FlexIO clock.
 *
 * @param base Pointer to the FLEXIO_SPI_Type.
 */
void FLEXIO_SPI_SlaveDeinit(FLEXIO_SPI_Type *base);

/*!
 * @brief Gets the default configuration to configure the FlexIO SPI slave. The configuration
 * can be used directly for calling the FLEXIO_SPI_SlaveConfigure().
 * Example:
   @code
   flexio_spi_slave_config_t slaveConfig;
   FLEXIO_SPI_SlaveGetDefaultConfig(&slaveConfig);
   @endcode
 * @param slaveConfig Pointer to the flexio_spi_slave_config_t structure.
*/
void FLEXIO_SPI_SlaveGetDefaultConfig(flexio_spi_slave_config_t *slaveConfig);

/*@}*/

/*!
 * @name Status
 * @{
 */

/*!
 * @brief Gets FlexIO SPI status flags.
 *
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @return status flag; Use the status flag to AND the following flag mask and get the status.
 *          @arg kFLEXIO_SPI_TxEmptyFlag
 *          @arg kFLEXIO_SPI_RxEmptyFlag
 */

uint32_t FLEXIO_SPI_GetStatusFlags(FLEXIO_SPI_Type *base);

/*!
 * @brief Clears FlexIO SPI status flags.
 *
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @param mask status flag
 *      The parameter can be any combination of the following values:
 *          @arg kFLEXIO_SPI_TxEmptyFlag
 *          @arg kFLEXIO_SPI_RxEmptyFlag
 */

void FLEXIO_SPI_ClearStatusFlags(FLEXIO_SPI_Type *base, uint32_t mask);

/*@}*/

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief Enables the FlexIO SPI interrupt.
 *
 * This function enables the FlexIO SPI interrupt.
 *
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @param mask interrupt source. The parameter can be any combination of the following values:
 *        @arg kFLEXIO_SPI_RxFullInterruptEnable
 *        @arg kFLEXIO_SPI_TxEmptyInterruptEnable
 */
void FLEXIO_SPI_EnableInterrupts(FLEXIO_SPI_Type *base, uint32_t mask);

/*!
 * @brief Disables the FlexIO SPI interrupt.
 *
 * This function disables the FlexIO SPI interrupt.
 *
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @param mask interrupt source The parameter can be any combination of the following values:
 *        @arg kFLEXIO_SPI_RxFullInterruptEnable
 *        @arg kFLEXIO_SPI_TxEmptyInterruptEnable
 */
void FLEXIO_SPI_DisableInterrupts(FLEXIO_SPI_Type *base, uint32_t mask);

/*@}*/

/*!
 * @name DMA Control
 * @{
 */

/*!
 * @brief Enables/disables the FlexIO SPI transmit DMA. This function enables/disables the FlexIO SPI Tx DMA,
 * which means that asserting the kFLEXIO_SPI_TxEmptyFlag does/doesn't trigger the DMA request.
 *
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @param mask SPI DMA source.
 * @param enable True means enable DMA, false means disable DMA.
 */
void FLEXIO_SPI_EnableDMA(FLEXIO_SPI_Type *base, uint32_t mask, bool enable);

/*!
 * @brief Gets the FlexIO SPI transmit data register address for MSB first transfer.
 *
 * This function returns the SPI data register address, which is mainly used by DMA/eDMA.
 *
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @param direction Shift direction of MSB first or LSB first.
 * @return FlexIO SPI transmit data register address.
 */
static inline uint32_t FLEXIO_SPI_GetTxDataRegisterAddress(FLEXIO_SPI_Type *base,
                                                           flexio_spi_shift_direction_t direction)
{
    if (direction == kFLEXIO_SPI_MsbFirst)
    {
        return FLEXIO_GetShifterBufferAddress(base->flexioBase, kFLEXIO_ShifterBufferBitSwapped,
                                              base->shifterIndex[0]) +
               3U;
    }
    else
    {
        return FLEXIO_GetShifterBufferAddress(base->flexioBase, kFLEXIO_ShifterBuffer, base->shifterIndex[0]);
    }
}

/*!
 * @brief Gets the FlexIO SPI receive data register address for the MSB first transfer.
 *
 * This function returns the SPI data register address, which is mainly used by DMA/eDMA.
 *
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @param direction Shift direction of MSB first or LSB first.
 * @return FlexIO SPI receive data register address.
 */
static inline uint32_t FLEXIO_SPI_GetRxDataRegisterAddress(FLEXIO_SPI_Type *base,
                                                           flexio_spi_shift_direction_t direction)
{
    if (direction == kFLEXIO_SPI_MsbFirst)
    {
        return FLEXIO_GetShifterBufferAddress(base->flexioBase, kFLEXIO_ShifterBufferBitSwapped, base->shifterIndex[1]);
    }
    else
    {
        return FLEXIO_GetShifterBufferAddress(base->flexioBase, kFLEXIO_ShifterBuffer, base->shifterIndex[1]) + 3U;
    }
}

/*@}*/

/*!
 * @name Bus Operations
 * @{
 */

/*!
 * @brief Enables/disables the FlexIO SPI module operation.
 *
 * @param base Pointer to the FLEXIO_SPI_Type.
 * @param enable True to enable, false does not have any effect.
 */
static inline void FLEXIO_SPI_Enable(FLEXIO_SPI_Type *base, bool enable)
{
    if (enable)
    {
        base->flexioBase->CTRL |= FLEXIO_CTRL_FLEXEN_MASK;
    }
}

/*!
 * @brief Sets baud rate for the FlexIO SPI transfer, which is only used for the master.
 *
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @param baudRate_Bps Baud Rate needed in Hz.
 * @param srcClockHz SPI source clock frequency in Hz.
 */
void FLEXIO_SPI_MasterSetBaudRate(FLEXIO_SPI_Type *base, uint32_t baudRate_Bps, uint32_t srcClockHz);

/*!
 * @brief Writes one byte of data, which is sent using the MSB method.
 *
 * @note This is a non-blocking API, which returns directly after the data is put into the
 * data register but the data transfer is not finished on the bus. Ensure that
 * the TxEmptyFlag is asserted before calling this API.
 *
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @param direction Shift direction of MSB first or LSB first.
 * @param data 8 bit/16 bit data.
 */
static inline void FLEXIO_SPI_WriteData(FLEXIO_SPI_Type *base, flexio_spi_shift_direction_t direction, uint16_t data)
{
    if (direction == kFLEXIO_SPI_MsbFirst)
    {
        base->flexioBase->SHIFTBUFBBS[base->shifterIndex[0]] = data;
    }
    else
    {
        base->flexioBase->SHIFTBUF[base->shifterIndex[0]] = data;
    }
}

/*!
 * @brief Reads 8 bit/16 bit data.
 *
 * @note This is a non-blocking API, which returns directly after the data is read from the
 * data register. Ensure that the RxFullFlag is asserted before calling this API.
 *
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @param direction Shift direction of MSB first or LSB first.
 * @return 8 bit/16 bit data received.
 */
static inline uint16_t FLEXIO_SPI_ReadData(FLEXIO_SPI_Type *base, flexio_spi_shift_direction_t direction)
{
    if (direction == kFLEXIO_SPI_MsbFirst)
    {
        return base->flexioBase->SHIFTBUFBIS[base->shifterIndex[1]];
    }
    else
    {
        return base->flexioBase->SHIFTBUFBYS[base->shifterIndex[1]];
    }
}

/*!
 * @brief Sends a buffer of data bytes.
 *
 * @note This function blocks using the polling method until all bytes have been sent.
 *
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @param direction Shift direction of MSB first or LSB first.
 * @param buffer The data bytes to send.
 * @param size The number of data bytes to send.
 */
void FLEXIO_SPI_WriteBlocking(FLEXIO_SPI_Type *base,
                              flexio_spi_shift_direction_t direction,
                              const uint8_t *buffer,
                              size_t size);

/*!
 * @brief Receives a buffer of bytes.
 *
 * @note This function blocks using the polling method until all bytes have been received.
 *
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @param direction Shift direction of MSB first or LSB first.
 * @param buffer The buffer to store the received bytes.
 * @param size The number of data bytes to be received.
 * @param direction Shift direction of MSB first or LSB first.
 */
void FLEXIO_SPI_ReadBlocking(FLEXIO_SPI_Type *base,
                             flexio_spi_shift_direction_t direction,
                             uint8_t *buffer,
                             size_t size);

/*!
 * @brief Receives a buffer of bytes.
 *
 * @note This function blocks via polling until all bytes have been received.
 *
 * @param base pointer to FLEXIO_SPI_Type structure
 * @param xfer FlexIO SPI transfer structure, see #flexio_spi_transfer_t.
 */
void FLEXIO_SPI_MasterTransferBlocking(FLEXIO_SPI_Type *base, flexio_spi_transfer_t *xfer);

/*Transactional APIs*/

/*!
 * @name Transactional
 * @{
 */

/*!
 * @brief Initializes the FlexIO SPI Master handle, which is used in transactional functions.
 *
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @param handle Pointer to the flexio_spi_master_handle_t structure to store the transfer state.
 * @param callback The callback function.
 * @param userData The parameter of the callback function.
 * @retval kStatus_Success Successfully create the handle.
 * @retval kStatus_OutOfRange The FlexIO type/handle/ISR table out of range.
 */
status_t FLEXIO_SPI_MasterTransferCreateHandle(FLEXIO_SPI_Type *base,
                                               flexio_spi_master_handle_t *handle,
                                               flexio_spi_master_transfer_callback_t callback,
                                               void *userData);

/*!
 * @brief Master transfer data using IRQ.
 *
 * This function sends data using IRQ. This is a non-blocking function, which returns
 * right away. When all data is sent out/received, the callback function is called.
 *
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @param handle Pointer to the flexio_spi_master_handle_t structure to store the transfer state.
 * @param xfer FlexIO SPI transfer structure. See #flexio_spi_transfer_t.
 * @retval kStatus_Success Successfully start a transfer.
 * @retval kStatus_InvalidArgument Input argument is invalid.
 * @retval kStatus_FLEXIO_SPI_Busy SPI is not idle, is running another transfer.
 */
status_t FLEXIO_SPI_MasterTransferNonBlocking(FLEXIO_SPI_Type *base,
                                              flexio_spi_master_handle_t *handle,
                                              flexio_spi_transfer_t *xfer);

/*!
 * @brief Aborts the master data transfer, which used IRQ.
 *
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @param handle Pointer to the flexio_spi_master_handle_t structure to store the transfer state.
 */
void FLEXIO_SPI_MasterTransferAbort(FLEXIO_SPI_Type *base, flexio_spi_master_handle_t *handle);

/*!
 * @brief Gets the data transfer status which used IRQ.
 *
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @param handle Pointer to the flexio_spi_master_handle_t structure to store the transfer state.
 * @param count Number of bytes transferred so far by the non-blocking transaction.
 * @retval kStatus_InvalidArgument count is Invalid.
 * @retval kStatus_Success Successfully return the count.
 */
status_t FLEXIO_SPI_MasterTransferGetCount(FLEXIO_SPI_Type *base, flexio_spi_master_handle_t *handle, size_t *count);

/*!
 * @brief FlexIO SPI master IRQ handler function.
 *
 * @param spiType Pointer to the FLEXIO_SPI_Type structure.
 * @param spiHandle Pointer to the flexio_spi_master_handle_t structure to store the transfer state.
 */
void FLEXIO_SPI_MasterTransferHandleIRQ(void *spiType, void *spiHandle);

/*!
 * @brief Initializes the FlexIO SPI Slave handle, which is used in transactional functions.
 *
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @param handle Pointer to the flexio_spi_slave_handle_t structure to store the transfer state.
 * @param callback The callback function.
 * @param userData The parameter of the callback function.
 * @retval kStatus_Success Successfully create the handle.
 * @retval kStatus_OutOfRange The FlexIO type/handle/ISR table out of range.
 */
status_t FLEXIO_SPI_SlaveTransferCreateHandle(FLEXIO_SPI_Type *base,
                                              flexio_spi_slave_handle_t *handle,
                                              flexio_spi_slave_transfer_callback_t callback,
                                              void *userData);

/*!
 * @brief Slave transfer data using IRQ.
 *
 * This function sends data using IRQ. This is a non-blocking function, which returns
 * right away. When all data is sent out/received, the callback function is called.
 * @param handle Pointer to the flexio_spi_slave_handle_t structure to store the transfer state.
 *
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @param xfer FlexIO SPI transfer structure. See #flexio_spi_transfer_t.
 * @retval kStatus_Success Successfully start a transfer.
 * @retval kStatus_InvalidArgument Input argument is invalid.
 * @retval kStatus_FLEXIO_SPI_Busy SPI is not idle; it is running another transfer.
 */
status_t FLEXIO_SPI_SlaveTransferNonBlocking(FLEXIO_SPI_Type *base,
                                             flexio_spi_slave_handle_t *handle,
                                             flexio_spi_transfer_t *xfer);

/*!
 * @brief Aborts the slave data transfer which used IRQ, share same API with master.
 *
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @param handle Pointer to the flexio_spi_slave_handle_t structure to store the transfer state.
 */
static inline void FLEXIO_SPI_SlaveTransferAbort(FLEXIO_SPI_Type *base, flexio_spi_slave_handle_t *handle)
{
    FLEXIO_SPI_MasterTransferAbort(base, handle);
}
/*!
 * @brief Gets the data transfer status which used IRQ, share same API with master.
 *
 * @param base Pointer to the FLEXIO_SPI_Type structure.
 * @param handle Pointer to the flexio_spi_slave_handle_t structure to store the transfer state.
 * @param count Number of bytes transferred so far by the non-blocking transaction.
 * @retval kStatus_InvalidArgument count is Invalid.
 * @retval kStatus_Success Successfully return the count.
 */
static inline status_t FLEXIO_SPI_SlaveTransferGetCount(FLEXIO_SPI_Type *base,
                                                        flexio_spi_slave_handle_t *handle,
                                                        size_t *count)
{
    return FLEXIO_SPI_MasterTransferGetCount(base, handle, count);
}

/*!
 * @brief FlexIO SPI slave IRQ handler function.
 *
 * @param spiType Pointer to the FLEXIO_SPI_Type structure.
 * @param spiHandle Pointer to the flexio_spi_slave_handle_t structure to store the transfer state.
 */
void FLEXIO_SPI_SlaveTransferHandleIRQ(void *spiType, void *spiHandle);

/*@}*/

#if defined(__cplusplus)
}
#endif /*_cplusplus*/
/*@}*/

#endif /*_FSL_FLEXIO_SPI_H_*/
