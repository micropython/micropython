/*
 * Copyright (c) 2015-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_LPUART_H_
#define _FSL_LPUART_H_

#include "fsl_common.h"

/*!
 * @addtogroup lpuart_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief LPUART driver version 2.2.7. */
#define FSL_LPUART_DRIVER_VERSION (MAKE_VERSION(2, 2, 7))
/*@}*/

/*! @brief Error codes for the LPUART driver. */
enum _lpuart_status
{
    kStatus_LPUART_TxBusy                  = MAKE_STATUS(kStatusGroup_LPUART, 0), /*!< TX busy */
    kStatus_LPUART_RxBusy                  = MAKE_STATUS(kStatusGroup_LPUART, 1), /*!< RX busy */
    kStatus_LPUART_TxIdle                  = MAKE_STATUS(kStatusGroup_LPUART, 2), /*!< LPUART transmitter is idle. */
    kStatus_LPUART_RxIdle                  = MAKE_STATUS(kStatusGroup_LPUART, 3), /*!< LPUART receiver is idle. */
    kStatus_LPUART_TxWatermarkTooLarge     = MAKE_STATUS(kStatusGroup_LPUART, 4), /*!< TX FIFO watermark too large  */
    kStatus_LPUART_RxWatermarkTooLarge     = MAKE_STATUS(kStatusGroup_LPUART, 5), /*!< RX FIFO watermark too large  */
    kStatus_LPUART_FlagCannotClearManually = MAKE_STATUS(kStatusGroup_LPUART, 6), /*!< Some flag can't manually clear */
    kStatus_LPUART_Error                   = MAKE_STATUS(kStatusGroup_LPUART, 7), /*!< Error happens on LPUART. */
    kStatus_LPUART_RxRingBufferOverrun =
        MAKE_STATUS(kStatusGroup_LPUART, 8), /*!< LPUART RX software ring buffer overrun. */
    kStatus_LPUART_RxHardwareOverrun = MAKE_STATUS(kStatusGroup_LPUART, 9),  /*!< LPUART RX receiver overrun. */
    kStatus_LPUART_NoiseError        = MAKE_STATUS(kStatusGroup_LPUART, 10), /*!< LPUART noise error. */
    kStatus_LPUART_FramingError      = MAKE_STATUS(kStatusGroup_LPUART, 11), /*!< LPUART framing error. */
    kStatus_LPUART_ParityError       = MAKE_STATUS(kStatusGroup_LPUART, 12), /*!< LPUART parity error. */
    kStatus_LPUART_BaudrateNotSupport =
        MAKE_STATUS(kStatusGroup_LPUART, 13), /*!< Baudrate is not support in current clock source */
    kStatus_LPUART_IdleLineDetected = MAKE_STATUS(kStatusGroup_LPUART, 14), /*!< IDLE flag. */
};

/*! @brief LPUART parity mode. */
typedef enum _lpuart_parity_mode
{
    kLPUART_ParityDisabled = 0x0U, /*!< Parity disabled */
    kLPUART_ParityEven     = 0x2U, /*!< Parity enabled, type even, bit setting: PE|PT = 10 */
    kLPUART_ParityOdd      = 0x3U, /*!< Parity enabled, type odd,  bit setting: PE|PT = 11 */
} lpuart_parity_mode_t;

/*! @brief LPUART data bits count. */
typedef enum _lpuart_data_bits
{
    kLPUART_EightDataBits = 0x0U, /*!< Eight data bit */
#if defined(FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT) && FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT
    kLPUART_SevenDataBits = 0x1U, /*!< Seven data bit */
#endif
} lpuart_data_bits_t;

/*! @brief LPUART stop bit count. */
typedef enum _lpuart_stop_bit_count
{
    kLPUART_OneStopBit = 0U, /*!< One stop bit */
    kLPUART_TwoStopBit = 1U, /*!< Two stop bits */
} lpuart_stop_bit_count_t;

#if defined(FSL_FEATURE_LPUART_HAS_MODEM_SUPPORT) && FSL_FEATURE_LPUART_HAS_MODEM_SUPPORT
/*! @brief LPUART transmit CTS source. */
typedef enum _lpuart_transmit_cts_source
{
    kLPUART_CtsSourcePin         = 0U, /*!< CTS resource is the LPUART_CTS pin. */
    kLPUART_CtsSourceMatchResult = 1U, /*!< CTS resource is the match result. */
} lpuart_transmit_cts_source_t;

/*! @brief LPUART transmit CTS configure. */
typedef enum _lpuart_transmit_cts_config
{
    kLPUART_CtsSampleAtStart = 0U, /*!< CTS input is sampled at the start of each character. */
    kLPUART_CtsSampleAtIdle  = 1U, /*!< CTS input is sampled when the transmitter is idle */
} lpuart_transmit_cts_config_t;
#endif

/*! @brief LPUART idle flag type defines when the receiver starts counting. */
typedef enum _lpuart_idle_type_select
{
    kLPUART_IdleTypeStartBit = 0U, /*!< Start counting after a valid start bit. */
    kLPUART_IdleTypeStopBit  = 1U, /*!< Start counting after a stop bit. */
} lpuart_idle_type_select_t;

/*! @brief LPUART idle detected configuration.
 *  This structure defines the number of idle characters that must be received before
 *  the IDLE flag is set.
 */
typedef enum _lpuart_idle_config
{
    kLPUART_IdleCharacter1   = 0U, /*!< the number of idle characters. */
    kLPUART_IdleCharacter2   = 1U, /*!< the number of idle characters. */
    kLPUART_IdleCharacter4   = 2U, /*!< the number of idle characters. */
    kLPUART_IdleCharacter8   = 3U, /*!< the number of idle characters. */
    kLPUART_IdleCharacter16  = 4U, /*!< the number of idle characters. */
    kLPUART_IdleCharacter32  = 5U, /*!< the number of idle characters. */
    kLPUART_IdleCharacter64  = 6U, /*!< the number of idle characters. */
    kLPUART_IdleCharacter128 = 7U, /*!< the number of idle characters. */
} lpuart_idle_config_t;

/*!
 * @brief LPUART interrupt configuration structure, default settings all disabled.
 *
 * This structure contains the settings for all LPUART interrupt configurations.
 */
enum _lpuart_interrupt_enable
{
#if defined(FSL_FEATURE_LPUART_HAS_LIN_BREAK_DETECT) && FSL_FEATURE_LPUART_HAS_LIN_BREAK_DETECT
    kLPUART_LinBreakInterruptEnable = (LPUART_BAUD_LBKDIE_MASK >> 8), /*!< LIN break detect. */
#endif
    kLPUART_RxActiveEdgeInterruptEnable         = (LPUART_BAUD_RXEDGIE_MASK >> 8), /*!< Receive Active Edge. */
    kLPUART_TxDataRegEmptyInterruptEnable       = (LPUART_CTRL_TIE_MASK),          /*!< Transmit data register empty. */
    kLPUART_TransmissionCompleteInterruptEnable = (LPUART_CTRL_TCIE_MASK),         /*!< Transmission complete. */
    kLPUART_RxDataRegFullInterruptEnable        = (LPUART_CTRL_RIE_MASK),          /*!< Receiver data register full. */
    kLPUART_IdleLineInterruptEnable             = (LPUART_CTRL_ILIE_MASK),         /*!< Idle line. */
    kLPUART_RxOverrunInterruptEnable            = (LPUART_CTRL_ORIE_MASK),         /*!< Receiver Overrun. */
    kLPUART_NoiseErrorInterruptEnable           = (LPUART_CTRL_NEIE_MASK),         /*!< Noise error flag. */
    kLPUART_FramingErrorInterruptEnable         = (LPUART_CTRL_FEIE_MASK),         /*!< Framing error flag. */
    kLPUART_ParityErrorInterruptEnable          = (LPUART_CTRL_PEIE_MASK),         /*!< Parity error flag. */
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
    kLPUART_TxFifoOverflowInterruptEnable  = (LPUART_FIFO_TXOFE_MASK >> 8), /*!< Transmit FIFO Overflow. */
    kLPUART_RxFifoUnderflowInterruptEnable = (LPUART_FIFO_RXUFE_MASK >> 8), /*!< Receive FIFO Underflow. */
#endif
};

/*!
 * @brief LPUART status flags.
 *
 * This provides constants for the LPUART status flags for use in the LPUART functions.
 */
enum _lpuart_flags
{
    kLPUART_TxDataRegEmptyFlag =
        (LPUART_STAT_TDRE_MASK), /*!< Transmit data register empty flag, sets when transmit buffer is empty */
    kLPUART_TransmissionCompleteFlag =
        (LPUART_STAT_TC_MASK), /*!< Transmission complete flag, sets when transmission activity complete */
    kLPUART_RxDataRegFullFlag =
        (LPUART_STAT_RDRF_MASK), /*!< Receive data register full flag, sets when the receive data buffer is full */
    kLPUART_IdleLineFlag  = (LPUART_STAT_IDLE_MASK), /*!< Idle line detect flag, sets when idle line detected */
    kLPUART_RxOverrunFlag = (LPUART_STAT_OR_MASK),   /*!< Receive Overrun, sets when new data is received before data is
                                                        read from receive register */
    kLPUART_NoiseErrorFlag = (LPUART_STAT_NF_MASK),  /*!< Receive takes 3 samples of each received bit.  If any of these
                                                        samples differ, noise flag sets */
    kLPUART_FramingErrorFlag =
        (LPUART_STAT_FE_MASK), /*!< Frame error flag, sets if logic 0 was detected where stop bit expected */
    kLPUART_ParityErrorFlag = (LPUART_STAT_PF_MASK), /*!< If parity enabled, sets upon parity error detection */
#if defined(FSL_FEATURE_LPUART_HAS_LIN_BREAK_DETECT) && FSL_FEATURE_LPUART_HAS_LIN_BREAK_DETECT
    kLPUART_LinBreakFlag = (int)(LPUART_STAT_LBKDIF_MASK), /*!< LIN break detect interrupt flag, sets when LIN break
                                                         char detected and LIN circuit enabled */
#endif
    kLPUART_RxActiveEdgeFlag =
        (LPUART_STAT_RXEDGIF_MASK), /*!< Receive pin active edge interrupt flag, sets when active edge detected */
    kLPUART_RxActiveFlag =
        (LPUART_STAT_RAF_MASK), /*!< Receiver Active Flag (RAF), sets at beginning of valid start bit */
#if defined(FSL_FEATURE_LPUART_HAS_ADDRESS_MATCHING) && FSL_FEATURE_LPUART_HAS_ADDRESS_MATCHING
    kLPUART_DataMatch1Flag = LPUART_STAT_MA1F_MASK, /*!< The next character to be read from LPUART_DATA matches MA1*/
    kLPUART_DataMatch2Flag = LPUART_STAT_MA2F_MASK, /*!< The next character to be read from LPUART_DATA matches MA2*/
#endif
#if defined(FSL_FEATURE_LPUART_HAS_EXTENDED_DATA_REGISTER_FLAGS) && FSL_FEATURE_LPUART_HAS_EXTENDED_DATA_REGISTER_FLAGS
    kLPUART_NoiseErrorInRxDataRegFlag =
        (LPUART_DATA_NOISY_MASK >> 10), /*!< NOISY bit, sets if noise detected in current data word */
    kLPUART_ParityErrorInRxDataRegFlag =
        (LPUART_DATA_PARITYE_MASK >> 10), /*!< PARITY bit, sets if noise detected in current data word */
#endif
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
    kLPUART_TxFifoEmptyFlag = (LPUART_FIFO_TXEMPT_MASK >> 16), /*!< TXEMPT bit, sets if transmit buffer is empty */
    kLPUART_RxFifoEmptyFlag = (LPUART_FIFO_RXEMPT_MASK >> 16), /*!< RXEMPT bit, sets if receive buffer is empty */
    kLPUART_TxFifoOverflowFlag =
        (LPUART_FIFO_TXOF_MASK >> 16), /*!< TXOF bit, sets if transmit buffer overflow occurred */
    kLPUART_RxFifoUnderflowFlag =
        (LPUART_FIFO_RXUF_MASK >> 16), /*!< RXUF bit, sets if receive buffer underflow occurred */
#endif
};

/*! @brief LPUART configuration structure. */
typedef struct _lpuart_config
{
    uint32_t baudRate_Bps;            /*!< LPUART baud rate  */
    lpuart_parity_mode_t parityMode;  /*!< Parity mode, disabled (default), even, odd */
    lpuart_data_bits_t dataBitsCount; /*!< Data bits count, eight (default), seven */
    bool isMsb;                       /*!< Data bits order, LSB (default), MSB */
#if defined(FSL_FEATURE_LPUART_HAS_STOP_BIT_CONFIG_SUPPORT) && FSL_FEATURE_LPUART_HAS_STOP_BIT_CONFIG_SUPPORT
    lpuart_stop_bit_count_t stopBitCount; /*!< Number of stop bits, 1 stop bit (default) or 2 stop bits  */
#endif
#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
    uint8_t txFifoWatermark; /*!< TX FIFO watermark */
    uint8_t rxFifoWatermark; /*!< RX FIFO watermark */
#endif
#if defined(FSL_FEATURE_LPUART_HAS_MODEM_SUPPORT) && FSL_FEATURE_LPUART_HAS_MODEM_SUPPORT
    bool enableRxRTS;                         /*!< RX RTS enable */
    bool enableTxCTS;                         /*!< TX CTS enable */
    lpuart_transmit_cts_source_t txCtsSource; /*!< TX CTS source */
    lpuart_transmit_cts_config_t txCtsConfig; /*!< TX CTS configure */
#endif
    lpuart_idle_type_select_t rxIdleType; /*!< RX IDLE type. */
    lpuart_idle_config_t rxIdleConfig;    /*!< RX IDLE configuration. */
    bool enableTx;                        /*!< Enable TX */
    bool enableRx;                        /*!< Enable RX */
} lpuart_config_t;

/*! @brief LPUART transfer structure. */
typedef struct _lpuart_transfer
{
    uint8_t *data;   /*!< The buffer of data to be transfer.*/
    size_t dataSize; /*!< The byte count to be transfer. */
} lpuart_transfer_t;

/* Forward declaration of the handle typedef. */
typedef struct _lpuart_handle lpuart_handle_t;

/*! @brief LPUART transfer callback function. */
typedef void (*lpuart_transfer_callback_t)(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData);

/*! @brief LPUART handle structure. */
struct _lpuart_handle
{
    uint8_t *volatile txData;   /*!< Address of remaining data to send. */
    volatile size_t txDataSize; /*!< Size of the remaining data to send. */
    size_t txDataSizeAll;       /*!< Size of the data to send out. */
    uint8_t *volatile rxData;   /*!< Address of remaining data to receive. */
    volatile size_t rxDataSize; /*!< Size of the remaining data to receive. */
    size_t rxDataSizeAll;       /*!< Size of the data to receive. */

    uint8_t *rxRingBuffer;              /*!< Start address of the receiver ring buffer. */
    size_t rxRingBufferSize;            /*!< Size of the ring buffer. */
    volatile uint16_t rxRingBufferHead; /*!< Index for the driver to store received data into ring buffer. */
    volatile uint16_t rxRingBufferTail; /*!< Index for the user to get data from the ring buffer. */

    lpuart_transfer_callback_t callback; /*!< Callback function. */
    void *userData;                      /*!< LPUART callback function parameter.*/

    volatile uint8_t txState; /*!< TX transfer state. */
    volatile uint8_t rxState; /*!< RX transfer state. */

#if defined(FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT) && FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT
    bool isSevenDataBits; /*!< Seven data bits flag. */
#endif
};

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

#if defined(FSL_FEATURE_LPUART_HAS_GLOBAL) && FSL_FEATURE_LPUART_HAS_GLOBAL

/*!
 * @name Software Reset
 * @{
 */

/*!
 * @brief Resets the LPUART using software.
 *
 * This function resets all internal logic and registers except the Global Register.
 * Remains set until cleared by software.
 *
 * @param base LPUART peripheral base address.
 */
static inline void LPUART_SoftwareReset(LPUART_Type *base)
{
    base->GLOBAL |= LPUART_GLOBAL_RST_MASK;
    base->GLOBAL &= ~LPUART_GLOBAL_RST_MASK;
}
/* @} */
#endif /*FSL_FEATURE_LPUART_HAS_GLOBAL*/

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Initializes an LPUART instance with the user configuration structure and the peripheral clock.
 *
 * This function configures the LPUART module with user-defined settings. Call the LPUART_GetDefaultConfig() function
 * to configure the configuration structure and get the default configuration.
 * The example below shows how to use this API to configure the LPUART.
 * @code
 *  lpuart_config_t lpuartConfig;
 *  lpuartConfig.baudRate_Bps = 115200U;
 *  lpuartConfig.parityMode = kLPUART_ParityDisabled;
 *  lpuartConfig.dataBitsCount = kLPUART_EightDataBits;
 *  lpuartConfig.isMsb = false;
 *  lpuartConfig.stopBitCount = kLPUART_OneStopBit;
 *  lpuartConfig.txFifoWatermark = 0;
 *  lpuartConfig.rxFifoWatermark = 1;
 *  LPUART_Init(LPUART1, &lpuartConfig, 20000000U);
 * @endcode
 *
 * @param base LPUART peripheral base address.
 * @param config Pointer to a user-defined configuration structure.
 * @param srcClock_Hz LPUART clock source frequency in HZ.
 * @retval kStatus_LPUART_BaudrateNotSupport Baudrate is not support in current clock source.
 * @retval kStatus_Success LPUART initialize succeed
 */
status_t LPUART_Init(LPUART_Type *base, const lpuart_config_t *config, uint32_t srcClock_Hz);

/*!
 * @brief Deinitializes a LPUART instance.
 *
 * This function waits for transmit to complete, disables TX and RX, and disables the LPUART clock.
 *
 * @param base LPUART peripheral base address.
 */
void LPUART_Deinit(LPUART_Type *base);

/*!
 * @brief Gets the default configuration structure.
 *
 * This function initializes the LPUART configuration structure to a default value. The default
 * values are:
 *   lpuartConfig->baudRate_Bps = 115200U;
 *   lpuartConfig->parityMode = kLPUART_ParityDisabled;
 *   lpuartConfig->dataBitsCount = kLPUART_EightDataBits;
 *   lpuartConfig->isMsb = false;
 *   lpuartConfig->stopBitCount = kLPUART_OneStopBit;
 *   lpuartConfig->txFifoWatermark = 0;
 *   lpuartConfig->rxFifoWatermark = 1;
 *   lpuartConfig->rxIdleType = kLPUART_IdleTypeStartBit;
 *   lpuartConfig->rxIdleConfig = kLPUART_IdleCharacter1;
 *   lpuartConfig->enableTx = false;
 *   lpuartConfig->enableRx = false;
 *
 * @param config Pointer to a configuration structure.
 */
void LPUART_GetDefaultConfig(lpuart_config_t *config);

/*!
 * @brief Sets the LPUART instance baudrate.
 *
 * This function configures the LPUART module baudrate. This function is used to update
 * the LPUART module baudrate after the LPUART module is initialized by the LPUART_Init.
 * @code
 *  LPUART_SetBaudRate(LPUART1, 115200U, 20000000U);
 * @endcode
 *
 * @param base LPUART peripheral base address.
 * @param baudRate_Bps LPUART baudrate to be set.
 * @param srcClock_Hz LPUART clock source frequency in HZ.
 * @retval kStatus_LPUART_BaudrateNotSupport Baudrate is not supported in the current clock source.
 * @retval kStatus_Success Set baudrate succeeded.
 */
status_t LPUART_SetBaudRate(LPUART_Type *base, uint32_t baudRate_Bps, uint32_t srcClock_Hz);

/* @} */

/*!
 * @name Status
 * @{
 */

/*!
 * @brief Gets LPUART status flags.
 *
 * This function gets all LPUART status flags. The flags are returned as the logical
 * OR value of the enumerators @ref _lpuart_flags. To check for a specific status,
 * compare the return value with enumerators in the @ref _lpuart_flags.
 * For example, to check whether the TX is empty:
 * @code
 *     if (kLPUART_TxDataRegEmptyFlag & LPUART_GetStatusFlags(LPUART1))
 *     {
 *         ...
 *     }
 * @endcode
 *
 * @param base LPUART peripheral base address.
 * @return LPUART status flags which are ORed by the enumerators in the _lpuart_flags.
 */
uint32_t LPUART_GetStatusFlags(LPUART_Type *base);

/*!
 * @brief Clears status flags with a provided mask.
 *
 * This function clears LPUART status flags with a provided mask. Automatically cleared flags
 * can't be cleared by this function.
 * Flags that can only cleared or set by hardware are:
 *    kLPUART_TxDataRegEmptyFlag, kLPUART_TransmissionCompleteFlag, kLPUART_RxDataRegFullFlag,
 *    kLPUART_RxActiveFlag, kLPUART_NoiseErrorInRxDataRegFlag, kLPUART_ParityErrorInRxDataRegFlag,
 *    kLPUART_TxFifoEmptyFlag,kLPUART_RxFifoEmptyFlag
 * Note: This API should be called when the Tx/Rx is idle, otherwise it takes no effects.
 *
 * @param base LPUART peripheral base address.
 * @param mask the status flags to be cleared. The user can use the enumerators in the
 *  _lpuart_status_flag_t to do the OR operation and get the mask.
 * @return 0 succeed, others failed.
 * @retval kStatus_LPUART_FlagCannotClearManually The flag can't be cleared by this function but
 *         it is cleared automatically by hardware.
 * @retval kStatus_Success Status in the mask are cleared.
 */
status_t LPUART_ClearStatusFlags(LPUART_Type *base, uint32_t mask);

/* @} */

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief Enables LPUART interrupts according to a provided mask.
 *
 * This function enables the LPUART interrupts according to a provided mask. The mask
 * is a logical OR of enumeration members. See the @ref _lpuart_interrupt_enable.
 * This examples shows how to enable TX empty interrupt and RX full interrupt:
 * @code
 *     LPUART_EnableInterrupts(LPUART1,kLPUART_TxDataRegEmptyInterruptEnable | kLPUART_RxDataRegFullInterruptEnable);
 * @endcode
 *
 * @param base LPUART peripheral base address.
 * @param mask The interrupts to enable. Logical OR of @ref _uart_interrupt_enable.
 */
void LPUART_EnableInterrupts(LPUART_Type *base, uint32_t mask);

/*!
 * @brief Disables  LPUART interrupts according to a provided mask.
 *
 * This function disables the LPUART interrupts according to a provided mask. The mask
 * is a logical OR of enumeration members. See @ref _lpuart_interrupt_enable.
 * This example shows how to disable the TX empty interrupt and RX full interrupt:
 * @code
 *     LPUART_DisableInterrupts(LPUART1,kLPUART_TxDataRegEmptyInterruptEnable | kLPUART_RxDataRegFullInterruptEnable);
 * @endcode
 *
 * @param base LPUART peripheral base address.
 * @param mask The interrupts to disable. Logical OR of @ref _lpuart_interrupt_enable.
 */
void LPUART_DisableInterrupts(LPUART_Type *base, uint32_t mask);

/*!
 * @brief Gets enabled LPUART interrupts.
 *
 * This function gets the enabled LPUART interrupts. The enabled interrupts are returned
 * as the logical OR value of the enumerators @ref _lpuart_interrupt_enable. To check
 * a specific interrupt enable status, compare the return value with enumerators
 * in @ref _lpuart_interrupt_enable.
 * For example, to check whether the TX empty interrupt is enabled:
 * @code
 *     uint32_t enabledInterrupts = LPUART_GetEnabledInterrupts(LPUART1);
 *
 *     if (kLPUART_TxDataRegEmptyInterruptEnable & enabledInterrupts)
 *     {
 *         ...
 *     }
 * @endcode
 *
 * @param base LPUART peripheral base address.
 * @return LPUART interrupt flags which are logical OR of the enumerators in @ref _lpuart_interrupt_enable.
 */
uint32_t LPUART_GetEnabledInterrupts(LPUART_Type *base);

#if defined(FSL_FEATURE_LPUART_HAS_DMA_ENABLE) && FSL_FEATURE_LPUART_HAS_DMA_ENABLE
/*!
 * @brief Gets the LPUART data register address.
 *
 * This function returns the LPUART data register address, which is mainly used by the DMA/eDMA.
 *
 * @param base LPUART peripheral base address.
 * @return LPUART data register addresses which are used both by the transmitter and receiver.
 */
static inline uint32_t LPUART_GetDataRegisterAddress(LPUART_Type *base)
{
    return (uint32_t) & (base->DATA);
}

/*!
 * @brief Enables or disables the LPUART transmitter DMA request.
 *
 * This function enables or disables the transmit data register empty flag, STAT[TDRE], to generate DMA requests.
 *
 * @param base LPUART peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void LPUART_EnableTxDMA(LPUART_Type *base, bool enable)
{
    if (enable)
    {
        base->BAUD |= LPUART_BAUD_TDMAE_MASK;
    }
    else
    {
        base->BAUD &= ~LPUART_BAUD_TDMAE_MASK;
    }
}

/*!
 * @brief Enables or disables the LPUART receiver DMA.
 *
 * This function enables or disables the receiver data register full flag, STAT[RDRF], to generate DMA requests.
 *
 * @param base LPUART peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void LPUART_EnableRxDMA(LPUART_Type *base, bool enable)
{
    if (enable)
    {
        base->BAUD |= LPUART_BAUD_RDMAE_MASK;
    }
    else
    {
        base->BAUD &= ~LPUART_BAUD_RDMAE_MASK;
    }
}

/* @} */
#endif /* FSL_FEATURE_LPUART_HAS_DMA_ENABLE */

/*!
 * @name Bus Operations
 * @{
 */

/*!
 * @brief Get the LPUART instance from peripheral base address.
 *
 * @param base LPUART peripheral base address.
 * @return LPUART instance.
 */
uint32_t LPUART_GetInstance(LPUART_Type *base);

/*!
 * @brief Enables or disables the LPUART transmitter.
 *
 * This function enables or disables the LPUART transmitter.
 *
 * @param base LPUART peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void LPUART_EnableTx(LPUART_Type *base, bool enable)
{
    if (enable)
    {
        base->CTRL |= LPUART_CTRL_TE_MASK;
    }
    else
    {
        base->CTRL &= ~LPUART_CTRL_TE_MASK;
    }
}

/*!
 * @brief Enables or disables the LPUART receiver.
 *
 * This function enables or disables the LPUART receiver.
 *
 * @param base LPUART peripheral base address.
 * @param enable True to enable, false to disable.
 */
static inline void LPUART_EnableRx(LPUART_Type *base, bool enable)
{
    if (enable)
    {
        base->CTRL |= LPUART_CTRL_RE_MASK;
    }
    else
    {
        base->CTRL &= ~LPUART_CTRL_RE_MASK;
    }
}

/*!
 * @brief Writes to the transmitter register.
 *
 * This function writes data to the transmitter register directly. The upper layer must
 * ensure that the TX register is empty or that the TX FIFO has room before calling this function.
 *
 * @param base LPUART peripheral base address.
 * @param data Data write to the TX register.
 */
static inline void LPUART_WriteByte(LPUART_Type *base, uint8_t data)
{
    base->DATA = data;
}

/*!
 * @brief Reads the receiver register.
 *
 * This function reads data from the receiver register directly. The upper layer must
 * ensure that the receiver register is full or that the RX FIFO has data before calling this function.
 *
 * @param base LPUART peripheral base address.
 * @return Data read from data register.
 */
static inline uint8_t LPUART_ReadByte(LPUART_Type *base)
{
#if defined(FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT) && FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT
    uint32_t ctrl = base->CTRL;
    bool isSevenDataBits =
        ((ctrl & LPUART_CTRL_M7_MASK) ||
         ((!(ctrl & LPUART_CTRL_M7_MASK)) && (!(ctrl & LPUART_CTRL_M_MASK)) && (ctrl & LPUART_CTRL_PE_MASK)));

    if (isSevenDataBits)
    {
        return (base->DATA & 0x7F);
    }
    else
    {
        return base->DATA;
    }
#else
    return (uint8_t)(base->DATA);
#endif
}

/*!
 * @brief Writes to the transmitter register using a blocking method.
 *
 * This function polls the transmitter register, waits for the register to be empty or  for TX FIFO to have
 * room, and writes data to the transmitter buffer.
 *
 * @note This function does not check whether all data has been sent out to the bus.
 * Before disabling the transmitter, check the kLPUART_TransmissionCompleteFlag to ensure that the transmit is
 * finished.
 *
 * @param base LPUART peripheral base address.
 * @param data Start address of the data to write.
 * @param length Size of the data to write.
 */
void LPUART_WriteBlocking(LPUART_Type *base, const uint8_t *data, size_t length);

/*!
 * @brief Reads the receiver data register using a blocking method.
 *
 * This function polls the receiver register, waits for the receiver register full or receiver FIFO
 * has data, and reads data from the TX register.
 *
 * @param base LPUART peripheral base address.
 * @param data Start address of the buffer to store the received data.
 * @param length Size of the buffer.
 * @retval kStatus_LPUART_RxHardwareOverrun Receiver overrun happened while receiving data.
 * @retval kStatus_LPUART_NoiseError Noise error happened while receiving data.
 * @retval kStatus_LPUART_FramingError Framing error happened while receiving data.
 * @retval kStatus_LPUART_ParityError Parity error happened while receiving data.
 * @retval kStatus_Success Successfully received all data.
 */
status_t LPUART_ReadBlocking(LPUART_Type *base, uint8_t *data, size_t length);

/* @} */

/*!
 * @name Transactional
 * @{
 */

/*!
 * @brief Initializes the LPUART handle.
 *
 * This function initializes the LPUART handle, which can be used for other LPUART
 * transactional APIs. Usually, for a specified LPUART instance,
 * call this API once to get the initialized handle.
 *
 * The LPUART driver supports the "background" receiving, which means that user can set up
 * an RX ring buffer optionally. Data received is stored into the ring buffer even when the
 * user doesn't call the LPUART_TransferReceiveNonBlocking() API. If there is already data received
 * in the ring buffer, the user can get the received data from the ring buffer directly.
 * The ring buffer is disabled if passing NULL as @p ringBuffer.
 *
 * @param base LPUART peripheral base address.
 * @param handle LPUART handle pointer.
 * @param callback Callback function.
 * @param userData User data.
 */
void LPUART_TransferCreateHandle(LPUART_Type *base,
                                 lpuart_handle_t *handle,
                                 lpuart_transfer_callback_t callback,
                                 void *userData);
/*!
 * @brief Transmits a buffer of data using the interrupt method.
 *
 * This function send data using an interrupt method. This is a non-blocking function, which
 * returns directly without waiting for all data written to the transmitter register. When
 * all data is written to the TX register in the ISR, the LPUART driver calls the callback
 * function and passes the @ref kStatus_LPUART_TxIdle as status parameter.
 *
 * @note The kStatus_LPUART_TxIdle is passed to the upper layer when all data are written
 * to the TX register. However, there is no check to ensure that all the data sent out. Before disabling the TX,
 * check the kLPUART_TransmissionCompleteFlag to ensure that the transmit is finished.
 *
 * @param base LPUART peripheral base address.
 * @param handle LPUART handle pointer.
 * @param xfer LPUART transfer structure, see #lpuart_transfer_t.
 * @retval kStatus_Success Successfully start the data transmission.
 * @retval kStatus_LPUART_TxBusy Previous transmission still not finished, data not all written to the TX register.
 * @retval kStatus_InvalidArgument Invalid argument.
 */
status_t LPUART_TransferSendNonBlocking(LPUART_Type *base, lpuart_handle_t *handle, lpuart_transfer_t *xfer);

/*!
 * @brief Sets up the RX ring buffer.
 *
 * This function sets up the RX ring buffer to a specific UART handle.
 *
 * When the RX ring buffer is used, data received is stored into the ring buffer even when
 * the user doesn't call the UART_TransferReceiveNonBlocking() API. If there is already data received
 * in the ring buffer, the user can get the received data from the ring buffer directly.
 *
 * @note When using RX ring buffer, one byte is reserved for internal use. In other
 * words, if @p ringBufferSize is 32, then only 31 bytes are used for saving data.
 *
 * @param base LPUART peripheral base address.
 * @param handle LPUART handle pointer.
 * @param ringBuffer Start address of ring buffer for background receiving. Pass NULL to disable the ring buffer.
 * @param ringBufferSize size of the ring buffer.
 */
void LPUART_TransferStartRingBuffer(LPUART_Type *base,
                                    lpuart_handle_t *handle,
                                    uint8_t *ringBuffer,
                                    size_t ringBufferSize);

/*!
 * @brief Aborts the background transfer and uninstalls the ring buffer.
 *
 * This function aborts the background transfer and uninstalls the ring buffer.
 *
 * @param base LPUART peripheral base address.
 * @param handle LPUART handle pointer.
 */
void LPUART_TransferStopRingBuffer(LPUART_Type *base, lpuart_handle_t *handle);

/*!
 * @brief Get the length of received data in RX ring buffer.
 *
 * @param handle LPUART handle pointer.
 * @return Length of received data in RX ring buffer.
 */
size_t LPUART_TransferGetRxRingBufferLength(LPUART_Type *base, lpuart_handle_t *handle);

/*!
 * @brief Aborts the interrupt-driven data transmit.
 *
 * This function aborts the interrupt driven data sending. The user can get the remainBtyes to find out
 * how many bytes are not sent out.
 *
 * @param base LPUART peripheral base address.
 * @param handle LPUART handle pointer.
 */
void LPUART_TransferAbortSend(LPUART_Type *base, lpuart_handle_t *handle);

/*!
 * @brief Gets the number of bytes that have been written to the LPUART transmitter register.
 *
 * This function gets the number of bytes that have been written to LPUART TX
 * register by an interrupt method.
 *
 * @param base LPUART peripheral base address.
 * @param handle LPUART handle pointer.
 * @param count Send bytes count.
 * @retval kStatus_NoTransferInProgress No send in progress.
 * @retval kStatus_InvalidArgument Parameter is invalid.
 * @retval kStatus_Success Get successfully through the parameter \p count;
 */
status_t LPUART_TransferGetSendCount(LPUART_Type *base, lpuart_handle_t *handle, uint32_t *count);

/*!
 * @brief Receives a buffer of data using the interrupt method.
 *
 * This function receives data using an interrupt method. This is a non-blocking function
 * which returns without waiting to ensure that all data are received.
 * If the RX ring buffer is used and not empty, the data in the ring buffer is copied and
 * the parameter @p receivedBytes shows how many bytes are copied from the ring buffer.
 * After copying, if the data in the ring buffer is not enough for read, the receive
 * request is saved by the LPUART driver. When the new data arrives, the receive request
 * is serviced first. When all data is received, the LPUART driver notifies the upper layer
 * through a callback function and passes a status parameter @ref kStatus_UART_RxIdle.
 * For example, the upper layer needs 10 bytes but there are only 5 bytes in ring buffer.
 * The 5 bytes are copied to xfer->data, which returns with the
 * parameter @p receivedBytes set to 5. For the remaining 5 bytes, the newly arrived data is
 * saved from xfer->data[5]. When 5 bytes are received, the LPUART driver notifies the upper layer.
 * If the RX ring buffer is not enabled, this function enables the RX and RX interrupt
 * to receive data to xfer->data. When all data is received, the upper layer is notified.
 *
 * @param base LPUART peripheral base address.
 * @param handle LPUART handle pointer.
 * @param xfer LPUART transfer structure, see #uart_transfer_t.
 * @param receivedBytes Bytes received from the ring buffer directly.
 * @retval kStatus_Success Successfully queue the transfer into the transmit queue.
 * @retval kStatus_LPUART_RxBusy Previous receive request is not finished.
 * @retval kStatus_InvalidArgument Invalid argument.
 */
status_t LPUART_TransferReceiveNonBlocking(LPUART_Type *base,
                                           lpuart_handle_t *handle,
                                           lpuart_transfer_t *xfer,
                                           size_t *receivedBytes);

/*!
 * @brief Aborts the interrupt-driven data receiving.
 *
 * This function aborts the interrupt-driven data receiving. The user can get the remainBytes to find out
 * how many bytes not received yet.
 *
 * @param base LPUART peripheral base address.
 * @param handle LPUART handle pointer.
 */
void LPUART_TransferAbortReceive(LPUART_Type *base, lpuart_handle_t *handle);

/*!
 * @brief Gets the number of bytes that have been received.
 *
 * This function gets the number of bytes that have been received.
 *
 * @param base LPUART peripheral base address.
 * @param handle LPUART handle pointer.
 * @param count Receive bytes count.
 * @retval kStatus_NoTransferInProgress No receive in progress.
 * @retval kStatus_InvalidArgument Parameter is invalid.
 * @retval kStatus_Success Get successfully through the parameter \p count;
 */
status_t LPUART_TransferGetReceiveCount(LPUART_Type *base, lpuart_handle_t *handle, uint32_t *count);

/*!
 * @brief LPUART IRQ handle function.
 *
 * This function handles the LPUART transmit and receive IRQ request.
 *
 * @param base LPUART peripheral base address.
 * @param handle LPUART handle pointer.
 */
void LPUART_TransferHandleIRQ(LPUART_Type *base, lpuart_handle_t *handle);

/*!
 * @brief LPUART Error IRQ handle function.
 *
 * This function handles the LPUART error IRQ request.
 *
 * @param base LPUART peripheral base address.
 * @param handle LPUART handle pointer.
 */
void LPUART_TransferHandleErrorIRQ(LPUART_Type *base, lpuart_handle_t *handle);

/* @} */

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_LPUART_H_ */
