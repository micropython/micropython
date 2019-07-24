/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_LPSPI_H_
#define _FSL_LPSPI_H_

#include "fsl_common.h"

/*!
 * @addtogroup lpspi_driver
 * @{
 */

/**********************************************************************************************************************
 * Definitions
 *********************************************************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief LPSPI driver version 2.0.3. */
#define FSL_LPSPI_DRIVER_VERSION (MAKE_VERSION(2, 0, 3))
/*@}*/

#ifndef LPSPI_DUMMY_DATA
/*! @brief LPSPI dummy data if no Tx data.*/
#define LPSPI_DUMMY_DATA (0x00U) /*!< Dummy data used for tx if there is not txData. */
#endif

/*! @brief Global variable for dummy data value setting. */
extern volatile uint8_t g_lpspiDummyData[];

/*! @brief Status for the LPSPI driver.*/
enum _lpspi_status
{
    kStatus_LPSPI_Busy       = MAKE_STATUS(kStatusGroup_LPSPI, 0), /*!< LPSPI transfer is busy.*/
    kStatus_LPSPI_Error      = MAKE_STATUS(kStatusGroup_LPSPI, 1), /*!< LPSPI driver error. */
    kStatus_LPSPI_Idle       = MAKE_STATUS(kStatusGroup_LPSPI, 2), /*!< LPSPI is idle.*/
    kStatus_LPSPI_OutOfRange = MAKE_STATUS(kStatusGroup_LPSPI, 3)  /*!< LPSPI transfer out Of range. */
};

/*! @brief LPSPI status flags in SPIx_SR register.*/
enum _lpspi_flags
{
    kLPSPI_TxDataRequestFlag    = LPSPI_SR_TDF_MASK, /*!< Transmit data flag */
    kLPSPI_RxDataReadyFlag      = LPSPI_SR_RDF_MASK, /*!< Receive data flag */
    kLPSPI_WordCompleteFlag     = LPSPI_SR_WCF_MASK, /*!< Word Complete flag */
    kLPSPI_FrameCompleteFlag    = LPSPI_SR_FCF_MASK, /*!< Frame Complete flag */
    kLPSPI_TransferCompleteFlag = LPSPI_SR_TCF_MASK, /*!< Transfer Complete flag */
    kLPSPI_TransmitErrorFlag    = LPSPI_SR_TEF_MASK, /*!< Transmit Error flag (FIFO underrun) */
    kLPSPI_ReceiveErrorFlag     = LPSPI_SR_REF_MASK, /*!< Receive Error flag (FIFO overrun) */
    kLPSPI_DataMatchFlag        = LPSPI_SR_DMF_MASK, /*!< Data Match flag */
    kLPSPI_ModuleBusyFlag       = LPSPI_SR_MBF_MASK, /*!< Module Busy flag */
    kLPSPI_AllStatusFlag        = (LPSPI_SR_TDF_MASK | LPSPI_SR_RDF_MASK | LPSPI_SR_WCF_MASK | LPSPI_SR_FCF_MASK |
                            LPSPI_SR_TCF_MASK | LPSPI_SR_TEF_MASK | LPSPI_SR_REF_MASK | LPSPI_SR_DMF_MASK |
                            LPSPI_SR_MBF_MASK) /*!< Used for clearing all w1c status flags */
};

/*! @brief LPSPI interrupt source.*/
enum _lpspi_interrupt_enable
{
    kLPSPI_TxInterruptEnable               = LPSPI_IER_TDIE_MASK, /*!< Transmit data interrupt enable */
    kLPSPI_RxInterruptEnable               = LPSPI_IER_RDIE_MASK, /*!< Receive data interrupt enable */
    kLPSPI_WordCompleteInterruptEnable     = LPSPI_IER_WCIE_MASK, /*!< Word complete interrupt enable */
    kLPSPI_FrameCompleteInterruptEnable    = LPSPI_IER_FCIE_MASK, /*!< Frame complete interrupt enable */
    kLPSPI_TransferCompleteInterruptEnable = LPSPI_IER_TCIE_MASK, /*!< Transfer complete interrupt enable */
    kLPSPI_TransmitErrorInterruptEnable    = LPSPI_IER_TEIE_MASK, /*!< Transmit error interrupt enable(FIFO underrun)*/
    kLPSPI_ReceiveErrorInterruptEnable     = LPSPI_IER_REIE_MASK, /*!< Receive Error interrupt enable (FIFO overrun) */
    kLPSPI_DataMatchInterruptEnable        = LPSPI_IER_DMIE_MASK, /*!< Data Match interrupt enable */
    kLPSPI_AllInterruptEnable =
        (LPSPI_IER_TDIE_MASK | LPSPI_IER_RDIE_MASK | LPSPI_IER_WCIE_MASK | LPSPI_IER_FCIE_MASK | LPSPI_IER_TCIE_MASK |
         LPSPI_IER_TEIE_MASK | LPSPI_IER_REIE_MASK | LPSPI_IER_DMIE_MASK) /*!< All above interrupts enable.*/
};

/*! @brief LPSPI DMA source.*/
enum _lpspi_dma_enable
{
    kLPSPI_TxDmaEnable = LPSPI_DER_TDDE_MASK, /*!< Transmit data DMA enable */
    kLPSPI_RxDmaEnable = LPSPI_DER_RDDE_MASK  /*!< Receive data DMA enable */
};

/*! @brief LPSPI master or slave mode configuration.*/
typedef enum _lpspi_master_slave_mode
{
    kLPSPI_Master = 1U, /*!< LPSPI peripheral operates in master mode.*/
    kLPSPI_Slave  = 0U  /*!< LPSPI peripheral operates in slave mode.*/
} lpspi_master_slave_mode_t;

/*! @brief LPSPI Peripheral Chip Select (PCS) configuration (which PCS to configure).*/
typedef enum _lpspi_which_pcs_config
{
    kLPSPI_Pcs0 = 0U, /*!< PCS[0] */
    kLPSPI_Pcs1 = 1U, /*!< PCS[1] */
    kLPSPI_Pcs2 = 2U, /*!< PCS[2] */
    kLPSPI_Pcs3 = 3U  /*!< PCS[3] */
} lpspi_which_pcs_t;

/*! @brief LPSPI Peripheral Chip Select (PCS) Polarity configuration.*/
typedef enum _lpspi_pcs_polarity_config
{
    kLPSPI_PcsActiveHigh = 1U, /*!< PCS Active High (idles low) */
    kLPSPI_PcsActiveLow  = 0U  /*!< PCS Active Low (idles high) */
} lpspi_pcs_polarity_config_t;

/*! @brief LPSPI Peripheral Chip Select (PCS) Polarity.*/
enum _lpspi_pcs_polarity
{
    kLPSPI_Pcs0ActiveLow   = 1U << 0, /*!< Pcs0 Active Low (idles high). */
    kLPSPI_Pcs1ActiveLow   = 1U << 1, /*!< Pcs1 Active Low (idles high). */
    kLPSPI_Pcs2ActiveLow   = 1U << 2, /*!< Pcs2 Active Low (idles high). */
    kLPSPI_Pcs3ActiveLow   = 1U << 3, /*!< Pcs3 Active Low (idles high). */
    kLPSPI_PcsAllActiveLow = 0xFU     /*!< Pcs0 to Pcs5 Active Low (idles high). */
};

/*! @brief LPSPI clock polarity configuration.*/
typedef enum _lpspi_clock_polarity
{
    kLPSPI_ClockPolarityActiveHigh = 0U, /*!< CPOL=0. Active-high LPSPI clock (idles low)*/
    kLPSPI_ClockPolarityActiveLow  = 1U  /*!< CPOL=1. Active-low LPSPI clock (idles high)*/
} lpspi_clock_polarity_t;

/*! @brief LPSPI clock phase configuration.*/
typedef enum _lpspi_clock_phase
{
    kLPSPI_ClockPhaseFirstEdge = 0U, /*!< CPHA=0. Data is captured on the leading edge of the SCK and changed on the
                                         following edge.*/
    kLPSPI_ClockPhaseSecondEdge = 1U /*!< CPHA=1. Data is changed on the leading edge of the SCK and captured on the
                                        following edge.*/
} lpspi_clock_phase_t;

/*! @brief LPSPI data shifter direction options.*/
typedef enum _lpspi_shift_direction
{
    kLPSPI_MsbFirst = 0U, /*!< Data transfers start with most significant bit.*/
    kLPSPI_LsbFirst = 1U  /*!< Data transfers start with least significant bit.*/
} lpspi_shift_direction_t;

/*! @brief LPSPI Host Request select configuration. */
typedef enum _lpspi_host_request_select
{
    kLPSPI_HostReqExtPin          = 0U, /*!< Host Request is an ext pin. */
    kLPSPI_HostReqInternalTrigger = 1U  /*!< Host Request is an internal trigger. */
} lpspi_host_request_select_t;

/*! @brief LPSPI Match configuration options. */
typedef enum _lpspi_match_config
{
    kLPSI_MatchDisabled                     = 0x0U, /*!< LPSPI Match Disabled. */
    kLPSI_1stWordEqualsM0orM1               = 0x2U, /*!< LPSPI Match Enabled. */
    kLPSI_AnyWordEqualsM0orM1               = 0x3U, /*!< LPSPI Match Enabled. */
    kLPSI_1stWordEqualsM0and2ndWordEqualsM1 = 0x4U, /*!< LPSPI Match Enabled. */
    kLPSI_AnyWordEqualsM0andNxtWordEqualsM1 = 0x5U, /*!< LPSPI Match Enabled. */
    kLPSI_1stWordAndM1EqualsM0andM1         = 0x6U, /*!< LPSPI Match Enabled. */
    kLPSI_AnyWordAndM1EqualsM0andM1         = 0x7U, /*!< LPSPI Match Enabled. */
} lpspi_match_config_t;

/*! @brief LPSPI pin (SDO and SDI) configuration. */
typedef enum _lpspi_pin_config
{
    kLPSPI_SdiInSdoOut = 0U, /*!< LPSPI SDI input, SDO output. */
    kLPSPI_SdiInSdiOut = 1U, /*!< LPSPI SDI input, SDI output. */
    kLPSPI_SdoInSdoOut = 2U, /*!< LPSPI SDO input, SDO output. */
    kLPSPI_SdoInSdiOut = 3U  /*!< LPSPI SDO input, SDI output. */
} lpspi_pin_config_t;

/*! @brief LPSPI data output configuration. */
typedef enum _lpspi_data_out_config
{
    kLpspiDataOutRetained = 0U, /*!< Data out retains last value when chip select is de-asserted */
    kLpspiDataOutTristate = 1U  /*!< Data out is tristated when chip select is de-asserted */
} lpspi_data_out_config_t;

/*! @brief LPSPI transfer width configuration. */
typedef enum _lpspi_transfer_width
{
    kLPSPI_SingleBitXfer = 0U, /*!< 1-bit shift at a time, data out on SDO, in on SDI (normal mode) */
    kLPSPI_TwoBitXfer    = 1U, /*!< 2-bits shift out on SDO/SDI and in on SDO/SDI */
    kLPSPI_FourBitXfer   = 2U  /*!< 4-bits shift out on SDO/SDI/PCS[3:2] and in on SDO/SDI/PCS[3:2] */
} lpspi_transfer_width_t;

/*! @brief LPSPI delay type selection.*/
typedef enum _lpspi_delay_type
{
    kLPSPI_PcsToSck = 1U,  /*!< PCS-to-SCK delay. */
    kLPSPI_LastSckToPcs,   /*!< Last SCK edge to PCS delay. */
    kLPSPI_BetweenTransfer /*!< Delay between transfers. */
} lpspi_delay_type_t;

#define LPSPI_MASTER_PCS_SHIFT (4U)   /*!< LPSPI master PCS shift macro , internal used. */
#define LPSPI_MASTER_PCS_MASK (0xF0U) /*!< LPSPI master PCS shift macro , internal used. */

/*! @brief Use this enumeration for LPSPI master transfer configFlags. */
enum _lpspi_transfer_config_flag_for_master
{
    kLPSPI_MasterPcs0 = 0U << LPSPI_MASTER_PCS_SHIFT, /*!< LPSPI master transfer use PCS0 signal */
    kLPSPI_MasterPcs1 = 1U << LPSPI_MASTER_PCS_SHIFT, /*!< LPSPI master transfer use PCS1 signal */
    kLPSPI_MasterPcs2 = 2U << LPSPI_MASTER_PCS_SHIFT, /*!< LPSPI master transfer use PCS2 signal */
    kLPSPI_MasterPcs3 = 3U << LPSPI_MASTER_PCS_SHIFT, /*!< LPSPI master transfer use PCS3 signal */

    kLPSPI_MasterPcsContinuous = 1U << 20, /*!< Is PCS signal continuous */

    kLPSPI_MasterByteSwap =
        1U << 22 /*!< Is master swap the byte.
                  * For example, when want to send data 1 2 3 4 5 6 7 8 (suppose you set
                  * lpspi_shift_direction_t to MSB).
                  * 1. If you set bitPerFrame = 8 , no matter the kLPSPI_MasterByteSwapyou flag is used
                  * or not, the waveform is 1 2 3 4 5 6 7 8.
                  * 2. If you set bitPerFrame = 16 :
                  * (1) the waveform is 2 1 4 3 6 5 8 7 if you do not use the kLPSPI_MasterByteSwap flag.
                  * (2) the waveform is 1 2 3 4 5 6 7 8 if you use the kLPSPI_MasterByteSwap flag.
                  * 3. If you set bitPerFrame = 32 :
                  * (1) the waveform is 4 3 2 1 8 7 6 5 if you do not use the kLPSPI_MasterByteSwap flag.
                  * (2) the waveform is 1 2 3 4 5 6 7 8 if you use the kLPSPI_MasterByteSwap flag.
                  */
};

#define LPSPI_SLAVE_PCS_SHIFT (4U)   /*!< LPSPI slave PCS shift macro , internal used. */
#define LPSPI_SLAVE_PCS_MASK (0xF0U) /*!< LPSPI slave PCS shift macro , internal used. */

/*! @brief Use this enumeration for LPSPI slave transfer configFlags. */
enum _lpspi_transfer_config_flag_for_slave
{
    kLPSPI_SlavePcs0 = 0U << LPSPI_SLAVE_PCS_SHIFT, /*!< LPSPI slave transfer use PCS0 signal */
    kLPSPI_SlavePcs1 = 1U << LPSPI_SLAVE_PCS_SHIFT, /*!< LPSPI slave transfer use PCS1 signal */
    kLPSPI_SlavePcs2 = 2U << LPSPI_SLAVE_PCS_SHIFT, /*!< LPSPI slave transfer use PCS2 signal */
    kLPSPI_SlavePcs3 = 3U << LPSPI_SLAVE_PCS_SHIFT, /*!< LPSPI slave transfer use PCS3 signal */

    kLPSPI_SlaveByteSwap =
        1U << 22 /*!< Is slave swap the byte.
                  * For example, when want to send data 1 2 3 4 5 6 7 8 (suppose you set
                  * lpspi_shift_direction_t to MSB).
                  * 1. If you set bitPerFrame = 8 , no matter the kLPSPI_SlaveByteSwap flag is used
                  * or not, the waveform is 1 2 3 4 5 6 7 8.
                  * 2. If you set bitPerFrame = 16 :
                  * (1) the waveform is 2 1 4 3 6 5 8 7 if you do not use the kLPSPI_SlaveByteSwap flag.
                  * (2) the waveform is 1 2 3 4 5 6 7 8 if you use the kLPSPI_SlaveByteSwap flag.
                  * 3. If you set bitPerFrame = 32 :
                  * (1) the waveform is 4 3 2 1 8 7 6 5 if you do not use the kLPSPI_SlaveByteSwap flag.
                  * (2) the waveform is 1 2 3 4 5 6 7 8 if you use the kLPSPI_SlaveByteSwap flag.
                  */
};

/*! @brief LPSPI transfer state, which is used for LPSPI transactional API state machine. */
enum _lpspi_transfer_state
{
    kLPSPI_Idle = 0x0U, /*!< Nothing in the transmitter/receiver. */
    kLPSPI_Busy,        /*!< Transfer queue is not finished. */
    kLPSPI_Error        /*!< Transfer error. */
};

/*! @brief LPSPI master configuration structure.*/
typedef struct _lpspi_master_config
{
    uint32_t baudRate;                 /*!< Baud Rate for LPSPI. */
    uint32_t bitsPerFrame;             /*!< Bits per frame, minimum 8, maximum 4096.*/
    lpspi_clock_polarity_t cpol;       /*!< Clock polarity. */
    lpspi_clock_phase_t cpha;          /*!< Clock phase. */
    lpspi_shift_direction_t direction; /*!< MSB or LSB data shift direction. */

    uint32_t pcsToSckDelayInNanoSec;     /*!< PCS to SCK delay time in nanoseconds, setting to 0 sets the minimum delay.
                                            It sets the boundary value if out of range.*/
    uint32_t lastSckToPcsDelayInNanoSec; /*!< Last SCK to PCS delay time in nanoseconds, setting to 0 sets the minimum
                                            delay. It sets the boundary value if out of range.*/
    uint32_t betweenTransferDelayInNanoSec; /*!< After the SCK delay time with nanoseconds, setting to 0 sets the
                                             minimum delay. It sets the boundary value if out of range.*/

    lpspi_which_pcs_t whichPcs;                     /*!< Desired Peripheral Chip Select (PCS). */
    lpspi_pcs_polarity_config_t pcsActiveHighOrLow; /*!< Desired PCS active high or low */

    lpspi_pin_config_t pinCfg; /*!< Configures which pins are used for input and output data
                                *during single bit transfers.*/

    lpspi_data_out_config_t dataOutConfig; /*!< Configures if the output data is tristated
                                            * between accesses (LPSPI_PCS is negated). */
} lpspi_master_config_t;

/*! @brief LPSPI slave configuration structure.*/
typedef struct _lpspi_slave_config
{
    uint32_t bitsPerFrame;             /*!< Bits per frame, minimum 8, maximum 4096.*/
    lpspi_clock_polarity_t cpol;       /*!< Clock polarity. */
    lpspi_clock_phase_t cpha;          /*!< Clock phase. */
    lpspi_shift_direction_t direction; /*!< MSB or LSB data shift direction. */

    lpspi_which_pcs_t whichPcs;                     /*!< Desired Peripheral Chip Select (pcs) */
    lpspi_pcs_polarity_config_t pcsActiveHighOrLow; /*!< Desired PCS active high or low */

    lpspi_pin_config_t pinCfg; /*!< Configures which pins are used for input and output data
                                *during single bit transfers.*/

    lpspi_data_out_config_t dataOutConfig; /*!< Configures if the output data is tristated
                                            * between accesses (LPSPI_PCS is negated). */
} lpspi_slave_config_t;

/*!
 * @brief Forward declaration of the _lpspi_master_handle typedefs.
 */
typedef struct _lpspi_master_handle lpspi_master_handle_t;

/*!
 * @brief Forward declaration of the _lpspi_slave_handle typedefs.
 */
typedef struct _lpspi_slave_handle lpspi_slave_handle_t;

/*!
 * @brief Master completion callback function pointer type.
 *
 * @param base LPSPI peripheral address.
 * @param handle Pointer to the handle for the LPSPI master.
 * @param status Success or error code describing whether the transfer is completed.
 * @param userData Arbitrary pointer-dataSized value passed from the application.
 */
typedef void (*lpspi_master_transfer_callback_t)(LPSPI_Type *base,
                                                 lpspi_master_handle_t *handle,
                                                 status_t status,
                                                 void *userData);

/*!
 * @brief Slave completion callback function pointer type.
 *
 * @param base LPSPI peripheral address.
 * @param handle Pointer to the handle for the LPSPI slave.
 * @param status Success or error code describing whether the transfer is completed.
 * @param userData Arbitrary pointer-dataSized value passed from the application.
 */
typedef void (*lpspi_slave_transfer_callback_t)(LPSPI_Type *base,
                                                lpspi_slave_handle_t *handle,
                                                status_t status,
                                                void *userData);

/*! @brief LPSPI master/slave transfer structure.*/
typedef struct _lpspi_transfer
{
    uint8_t *txData;          /*!< Send buffer. */
    uint8_t *rxData;          /*!< Receive buffer. */
    volatile size_t dataSize; /*!< Transfer bytes. */

    uint32_t configFlags; /*!< Transfer transfer configuration flags. Set from _lpspi_transfer_config_flag_for_master if
                             the transfer is used for master or _lpspi_transfer_config_flag_for_slave enumeration if the
                             transfer is used for slave.*/
} lpspi_transfer_t;

/*! @brief LPSPI master transfer handle structure used for transactional API. */
struct _lpspi_master_handle
{
    volatile bool isPcsContinuous; /*!< Is PCS continuous in transfer. */
    volatile bool writeTcrInIsr;   /*!< A flag that whether should write TCR in ISR. */

    volatile bool isByteSwap; /*!< A flag that whether should byte swap. */

    volatile uint8_t fifoSize; /*!< FIFO dataSize. */

    volatile uint8_t rxWatermark; /*!< Rx watermark. */

    volatile uint8_t bytesEachWrite; /*!< Bytes for each write TDR. */
    volatile uint8_t bytesEachRead;  /*!< Bytes for each read RDR. */

    uint8_t *volatile txData;             /*!< Send buffer. */
    uint8_t *volatile rxData;             /*!< Receive buffer. */
    volatile size_t txRemainingByteCount; /*!< Number of bytes remaining to send.*/
    volatile size_t rxRemainingByteCount; /*!< Number of bytes remaining to receive.*/

    volatile uint32_t writeRegRemainingTimes; /*!< Write TDR register remaining times. */
    volatile uint32_t readRegRemainingTimes;  /*!< Read RDR register remaining times. */

    uint32_t totalByteCount; /*!< Number of transfer bytes*/

    uint32_t txBuffIfNull; /*!< Used if the txData is NULL. */

    volatile uint8_t state; /*!< LPSPI transfer state , _lpspi_transfer_state.*/

    lpspi_master_transfer_callback_t callback; /*!< Completion callback. */
    void *userData;                            /*!< Callback user data. */
};

/*! @brief LPSPI slave transfer handle structure used for transactional API. */
struct _lpspi_slave_handle
{
    volatile bool isByteSwap; /*!< A flag that whether should byte swap. */

    volatile uint8_t fifoSize; /*!< FIFO dataSize. */

    volatile uint8_t rxWatermark; /*!< Rx watermark. */

    volatile uint8_t bytesEachWrite; /*!< Bytes for each write TDR. */
    volatile uint8_t bytesEachRead;  /*!< Bytes for each read RDR. */

    uint8_t *volatile txData; /*!< Send buffer. */
    uint8_t *volatile rxData; /*!< Receive buffer. */

    volatile size_t txRemainingByteCount; /*!< Number of bytes remaining to send.*/
    volatile size_t rxRemainingByteCount; /*!< Number of bytes remaining to receive.*/

    volatile uint32_t writeRegRemainingTimes; /*!< Write TDR register remaining times. */
    volatile uint32_t readRegRemainingTimes;  /*!< Read RDR register remaining times. */

    uint32_t totalByteCount; /*!< Number of transfer bytes*/

    volatile uint8_t state; /*!< LPSPI transfer state , _lpspi_transfer_state.*/

    volatile uint32_t errorCount; /*!< Error count for slave transfer.*/

    lpspi_slave_transfer_callback_t callback; /*!< Completion callback. */
    void *userData;                           /*!< Callback user data. */
};

/**********************************************************************************************************************
 * API
 *********************************************************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus*/

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Initializes the LPSPI master.
 *
 * @param base LPSPI peripheral address.
 * @param masterConfig Pointer to structure lpspi_master_config_t.
 * @param srcClock_Hz Module source input clock in Hertz
 */
void LPSPI_MasterInit(LPSPI_Type *base, const lpspi_master_config_t *masterConfig, uint32_t srcClock_Hz);

/*!
 * @brief Sets the lpspi_master_config_t structure to default values.
 *
 * This API initializes the configuration structure  for LPSPI_MasterInit().
 * The initialized structure can remain unchanged in LPSPI_MasterInit(), or can be modified
 * before calling the LPSPI_MasterInit().
 * Example:
 * @code
 *  lpspi_master_config_t  masterConfig;
 *  LPSPI_MasterGetDefaultConfig(&masterConfig);
 * @endcode
 * @param masterConfig pointer to lpspi_master_config_t structure
 */
void LPSPI_MasterGetDefaultConfig(lpspi_master_config_t *masterConfig);

/*!
 * @brief LPSPI slave configuration.
 *
 * @param base LPSPI peripheral address.
 * @param slaveConfig Pointer to a structure lpspi_slave_config_t.
 */
void LPSPI_SlaveInit(LPSPI_Type *base, const lpspi_slave_config_t *slaveConfig);

/*!
 * @brief Sets the lpspi_slave_config_t structure to default values.
 *
 * This API initializes the configuration structure for LPSPI_SlaveInit().
 * The initialized structure can remain unchanged in LPSPI_SlaveInit() or can be modified
 * before calling the LPSPI_SlaveInit().
 * Example:
 * @code
 *  lpspi_slave_config_t  slaveConfig;
 *  LPSPI_SlaveGetDefaultConfig(&slaveConfig);
 * @endcode
 * @param slaveConfig pointer to lpspi_slave_config_t structure.
 */
void LPSPI_SlaveGetDefaultConfig(lpspi_slave_config_t *slaveConfig);

/*!
 * @brief De-initializes the LPSPI peripheral. Call this API to disable the LPSPI clock.
 * @param base LPSPI peripheral address.
 */
void LPSPI_Deinit(LPSPI_Type *base);

/*!
 * @brief Restores the LPSPI peripheral to reset state. Note that this function
 * sets all registers to reset state. As a result, the LPSPI module can't work after calling
 * this API.
 * @param base LPSPI peripheral address.
 */
void LPSPI_Reset(LPSPI_Type *base);

/*!
 * @brief Enables the LPSPI peripheral and sets the MCR MDIS to 0.
 *
 * @param base LPSPI peripheral address.
 * @param enable Pass true to enable module, false to disable module.
 */
static inline void LPSPI_Enable(LPSPI_Type *base, bool enable)
{
    if (enable)
    {
        base->CR |= LPSPI_CR_MEN_MASK;
    }
    else
    {
        base->CR &= ~LPSPI_CR_MEN_MASK;
    }
}

/*!
 *@}
 */

/*!
 * @name Status
 * @{
 */

/*!
 * @brief Gets the LPSPI status flag state.
 * @param base LPSPI peripheral address.
 * @return The LPSPI status(in SR register).
 */
static inline uint32_t LPSPI_GetStatusFlags(LPSPI_Type *base)
{
    return (base->SR);
}

/*!
 * @brief Gets the LPSPI Tx FIFO size.
 * @param base LPSPI peripheral address.
 * @return The LPSPI Tx FIFO size.
 */
static inline uint32_t LPSPI_GetTxFifoSize(LPSPI_Type *base)
{
    return (1U << ((base->PARAM & LPSPI_PARAM_TXFIFO_MASK) >> LPSPI_PARAM_TXFIFO_SHIFT));
}

/*!
 * @brief Gets the LPSPI Rx FIFO size.
 * @param base LPSPI peripheral address.
 * @return The LPSPI Rx FIFO size.
 */
static inline uint32_t LPSPI_GetRxFifoSize(LPSPI_Type *base)
{
    return (1U << ((base->PARAM & LPSPI_PARAM_RXFIFO_MASK) >> LPSPI_PARAM_RXFIFO_SHIFT));
}

/*!
 * @brief Gets the LPSPI Tx FIFO count.
 * @param base LPSPI peripheral address.
 * @return The number of words in the transmit FIFO.
 */
static inline uint32_t LPSPI_GetTxFifoCount(LPSPI_Type *base)
{
    return ((base->FSR & LPSPI_FSR_TXCOUNT_MASK) >> LPSPI_FSR_TXCOUNT_SHIFT);
}

/*!
 * @brief Gets the LPSPI Rx FIFO count.
 * @param base LPSPI peripheral address.
 * @return The number of words in the receive FIFO.
 */
static inline uint32_t LPSPI_GetRxFifoCount(LPSPI_Type *base)
{
    return ((base->FSR & LPSPI_FSR_RXCOUNT_MASK) >> LPSPI_FSR_RXCOUNT_SHIFT);
}

/*!
 * @brief Clears the LPSPI status flag.
 *
 * This function  clears the desired status bit by using a write-1-to-clear. The user passes in the base and the
 * desired status flag bit to clear.  The list of status flags is defined in the _lpspi_flags.
 * Example usage:
 * @code
 *  LPSPI_ClearStatusFlags(base, kLPSPI_TxDataRequestFlag|kLPSPI_RxDataReadyFlag);
 * @endcode
 *
 * @param base LPSPI peripheral address.
 * @param statusFlags The status flag used from type _lpspi_flags.
 */
static inline void LPSPI_ClearStatusFlags(LPSPI_Type *base, uint32_t statusFlags)
{
    base->SR = statusFlags; /*!< The status flags are cleared by writing 1 (w1c).*/
}

/*!
 *@}
 */

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief Enables the LPSPI interrupts.
 *
 * This function configures the various interrupt masks of the LPSPI.  The parameters are base and an interrupt mask.
 * Note that, for Tx fill and Rx FIFO drain requests, enabling the interrupt request disables the DMA request.
 *
 * @code
 *  LPSPI_EnableInterrupts(base, kLPSPI_TxInterruptEnable | kLPSPI_RxInterruptEnable );
 * @endcode
 *
 * @param base LPSPI peripheral address.
 * @param mask The interrupt mask; Use the enum _lpspi_interrupt_enable.
 */
static inline void LPSPI_EnableInterrupts(LPSPI_Type *base, uint32_t mask)
{
    base->IER |= mask;
}

/*!
 * @brief Disables the LPSPI interrupts.
 *
 * @code
 *  LPSPI_DisableInterrupts(base, kLPSPI_TxInterruptEnable | kLPSPI_RxInterruptEnable );
 * @endcode
 *
 * @param base LPSPI peripheral address.
 * @param mask The interrupt mask; Use the enum _lpspi_interrupt_enable.
 */
static inline void LPSPI_DisableInterrupts(LPSPI_Type *base, uint32_t mask)
{
    base->IER &= ~mask;
}

/*!
 *@}
 */

/*!
 * @name DMA Control
 * @{
 */

/*!
 * @brief Enables the LPSPI DMA request.
 *
 * This function configures the Rx and Tx DMA mask of the LPSPI. The parameters are base and a DMA mask.
 * @code
 *  LPSPI_EnableDMA(base, kLPSPI_TxDmaEnable | kLPSPI_RxDmaEnable);
 * @endcode
 *
 * @param base LPSPI peripheral address.
 * @param mask The interrupt mask; Use the enum _lpspi_dma_enable.
 */
static inline void LPSPI_EnableDMA(LPSPI_Type *base, uint32_t mask)
{
    base->DER |= mask;
}

/*!
 * @brief Disables the LPSPI DMA request.
 *
 * This function configures the Rx and Tx DMA mask of the LPSPI.  The parameters are base and a DMA mask.
 * @code
 *  SPI_DisableDMA(base, kLPSPI_TxDmaEnable | kLPSPI_RxDmaEnable);
 * @endcode
 *
 * @param base LPSPI peripheral address.
 * @param mask The interrupt mask; Use the enum _lpspi_dma_enable.
 */
static inline void LPSPI_DisableDMA(LPSPI_Type *base, uint32_t mask)
{
    base->DER &= ~mask;
}

/*!
 * @brief Gets the LPSPI Transmit Data Register address for a DMA operation.
 *
 * This function gets the LPSPI Transmit Data Register address because this value is needed
 * for the DMA operation.
 * This function can be used for either master or slave mode.
 *
 * @param base LPSPI peripheral address.
 * @return The LPSPI Transmit Data Register address.
 */
static inline uint32_t LPSPI_GetTxRegisterAddress(LPSPI_Type *base)
{
    return (uint32_t) & (base->TDR);
}

/*!
 * @brief Gets the LPSPI Receive Data Register address for a DMA operation.
 *
 * This function gets the LPSPI Receive Data Register address because this value is needed
 * for the DMA operation.
 * This function can be used for either master or slave mode.
 *
 * @param base LPSPI peripheral address.
 * @return The LPSPI Receive Data Register address.
 */
static inline uint32_t LPSPI_GetRxRegisterAddress(LPSPI_Type *base)
{
    return (uint32_t) & (base->RDR);
}

/*!
 *@}
 */

/*!
 * @name Bus Operations
 * @{
 */

/*!
 * @brief Check the argument for transfer .
 *
 * @param transfer the transfer struct to be used.
 * @param bitPerFrame The bit size of one frame.
 * @param bytePerFrame The byte size of one frame.
 * @return Return true for right and false for wrong.
 */
bool LPSPI_CheckTransferArgument(lpspi_transfer_t *transfer, uint32_t bitsPerFrame, uint32_t bytesPerFrame);

/*!
 * @brief Configures the LPSPI for either master or slave.
 *
 * Note that the CFGR1 should only be written when the LPSPI is disabled (LPSPIx_CR_MEN = 0).
 *
 * @param base LPSPI peripheral address.
 * @param mode Mode setting (master or slave) of type lpspi_master_slave_mode_t.
 */
static inline void LPSPI_SetMasterSlaveMode(LPSPI_Type *base, lpspi_master_slave_mode_t mode)
{
    base->CFGR1 = (base->CFGR1 & (~LPSPI_CFGR1_MASTER_MASK)) | LPSPI_CFGR1_MASTER(mode);
}

/*!
 * @brief Returns whether the LPSPI module is in master mode.
 *
 * @param base LPSPI peripheral address.
 * @return Returns true if the module is in master mode or false if the module is in slave mode.
 */
static inline bool LPSPI_IsMaster(LPSPI_Type *base)
{
    return (bool)((base->CFGR1) & LPSPI_CFGR1_MASTER_MASK);
}

/*!
 * @brief Flushes the LPSPI FIFOs.
 *
 * @param base LPSPI peripheral address.
 * @param flushTxFifo Flushes (true) the Tx FIFO, else do not flush (false) the Tx FIFO.
 * @param flushRxFifo Flushes (true) the Rx FIFO, else do not flush (false) the Rx FIFO.
 */
static inline void LPSPI_FlushFifo(LPSPI_Type *base, bool flushTxFifo, bool flushRxFifo)
{
    base->CR |= ((uint32_t)flushTxFifo << LPSPI_CR_RTF_SHIFT) | ((uint32_t)flushRxFifo << LPSPI_CR_RRF_SHIFT);
}

/*!
 * @brief Sets the transmit and receive FIFO watermark values.
 *
 * This function allows the user to set the receive and transmit FIFO watermarks. The function
 * does not compare the watermark settings to the FIFO size. The FIFO watermark should not be
 * equal to or greater than the FIFO size.  It is up to the higher level driver to make this check.
 *
 * @param base LPSPI peripheral address.
 * @param txWater The TX FIFO watermark value. Writing a value equal or greater than the FIFO size is truncated.
 * @param rxWater The RX FIFO watermark value. Writing a value equal or greater than the FIFO size is truncated.
 */
static inline void LPSPI_SetFifoWatermarks(LPSPI_Type *base, uint32_t txWater, uint32_t rxWater)
{
    base->FCR = LPSPI_FCR_TXWATER(txWater) | LPSPI_FCR_RXWATER(rxWater);
}

/*!
 * @brief Configures all LPSPI peripheral chip select polarities simultaneously.
 *
 * Note that the CFGR1 should only be written when the LPSPI is disabled (LPSPIx_CR_MEN = 0).
 *
 * This is an example:  PCS0 and PCS1 set to active low and other PCSs set to active high. Note that the number of
 * PCS is device-specific.
 * @code
 *  LPSPI_SetAllPcsPolarity(base, kLPSPI_Pcs0ActiveLow | kLPSPI_Pcs1ActiveLow);
 * @endcode
 *
 * @param base LPSPI peripheral address.
 * @param mask The PCS polarity mask; Use the enum _lpspi_pcs_polarity.
 */
static inline void LPSPI_SetAllPcsPolarity(LPSPI_Type *base, uint32_t mask)
{
    base->CFGR1 = (base->CFGR1 & ~LPSPI_CFGR1_PCSPOL_MASK) | LPSPI_CFGR1_PCSPOL(~mask);
}

/*!
 * @brief Configures the frame size.
 *
 * The minimum frame size is 8-bits and the maximum frame size is 4096-bits. If the frame size is less than or equal
 * to 32-bits, the word size and frame size are identical. If the frame size is greater than 32-bits, the word
 * size is 32-bits for each word except the last (the last word contains the remainder bits if the frame size is not
 * divisible by 32). The minimum word size is 2-bits. A frame size of 33-bits (or similar) is not supported.
 *
 * Note 1: The transmit command register should be initialized before enabling the LPSPI in slave mode, although
 * the command register does not update until after the LPSPI is enabled. After it is enabled, the transmit command
 * register
 * should only be changed if the LPSPI is idle.
 *
 * Note 2: The transmit and command FIFO is a combined FIFO that includes both transmit data and command words. That
 * means the TCR register should be written to when the Tx FIFO is not full.
 *
 * @param base LPSPI peripheral address.
 * @param frameSize The frame size in number of bits.
 */
static inline void LPSPI_SetFrameSize(LPSPI_Type *base, uint32_t frameSize)
{
    base->TCR = (base->TCR & ~LPSPI_TCR_FRAMESZ_MASK) | LPSPI_TCR_FRAMESZ(frameSize - 1);
}

/*!
 * @brief Sets the LPSPI baud rate in bits per second.
 *
 * This function takes in the desired bitsPerSec (baud rate) and calculates the nearest
 * possible baud rate without exceeding the desired baud rate and returns the
 * calculated baud rate in bits-per-second. It requires the caller to provide
 * the frequency of the module source clock (in Hertz). Note that the baud rate
 * does not go into effect until the Transmit Control Register (TCR) is programmed
 * with the prescale value. Hence, this function returns the prescale tcrPrescaleValue
 * parameter for later programming in the TCR.  The higher level
 * peripheral driver should alert the user of an out of range baud rate input.
 *
 * Note that the LPSPI module must first be disabled before configuring this.
 * Note that the LPSPI module must be configured for master mode before configuring this.
 *
 * @param base LPSPI peripheral address.
 * @param baudRate_Bps The desired baud rate in bits per second.
 * @param srcClock_Hz Module source input clock in Hertz.
 * @param tcrPrescaleValue The TCR prescale value needed to program the TCR.
 * @return  The actual calculated baud rate. This function may also return a "0" if the
 *          LPSPI is not configured for master mode or if the LPSPI module is not disabled.
 */

uint32_t LPSPI_MasterSetBaudRate(LPSPI_Type *base,
                                 uint32_t baudRate_Bps,
                                 uint32_t srcClock_Hz,
                                 uint32_t *tcrPrescaleValue);

/*!
 * @brief Manually configures a specific LPSPI delay parameter (module must be disabled to
 *        change the delay values).
 *
 * This function configures the following:
 * SCK to PCS delay, or
 * PCS to SCK delay, or
 * The configurations must occur between the transfer delay.
 *
 * The delay names are available in type lpspi_delay_type_t.
 *
 * The user passes the desired delay along with the delay value.
 * This allows the user to directly set the delay values if they have
 * pre-calculated them or if they simply wish to manually increment the value.
 *
 * Note that the LPSPI module must first be disabled before configuring this.
 * Note that the LPSPI module must be configured for master mode before configuring this.
 *
 * @param base LPSPI peripheral address.
 * @param scaler The 8-bit delay value 0x00 to 0xFF (255).
 * @param whichDelay The desired delay to configure, must be of type lpspi_delay_type_t.
 */
void LPSPI_MasterSetDelayScaler(LPSPI_Type *base, uint32_t scaler, lpspi_delay_type_t whichDelay);

/*!
 * @brief Calculates the delay based on the desired delay input in nanoseconds (module must be
 *        disabled to change the delay values).
 *
 * This function calculates the values for the following:
 * SCK to PCS delay, or
 * PCS to SCK delay, or
 * The configurations must occur between the transfer delay.
 *
 * The delay names are available in type lpspi_delay_type_t.
 *
 * The user passes the desired delay and the desired delay value in
 * nano-seconds.  The function calculates the value needed for the desired delay parameter
 * and returns the actual calculated delay because an exact delay match may not be possible. In this
 * case, the closest match is calculated without going below the desired delay value input.
 * It is possible to input a very large delay value that exceeds the capability of the part, in
 * which case the maximum supported delay is returned. It is up to the higher level
 * peripheral driver to alert the user of an out of range delay input.
 *
 * Note that the LPSPI module must be configured for master mode before configuring this. And note that
 * the delayTime = LPSPI_clockSource / (PRESCALE * Delay_scaler).
 *
 * @param base LPSPI peripheral address.
 * @param delayTimeInNanoSec The desired delay value in nano-seconds.
 * @param whichDelay The desired delay to configuration, which must be of type lpspi_delay_type_t.
 * @param srcClock_Hz  Module source input clock in Hertz.
 * @return actual Calculated delay value in nano-seconds.
 */
uint32_t LPSPI_MasterSetDelayTimes(LPSPI_Type *base,
                                   uint32_t delayTimeInNanoSec,
                                   lpspi_delay_type_t whichDelay,
                                   uint32_t srcClock_Hz);

/*!
 * @brief Writes data into the transmit data buffer.
 *
 * This function writes data passed in by the user to the Transmit Data Register (TDR).
 * The user can pass up to 32-bits of data to load into the TDR. If the frame size exceeds 32-bits,
 * the user has to manage sending the data one 32-bit word at a time.
 * Any writes to the TDR result in an immediate push to the transmit FIFO.
 * This function can be used for either master or slave modes.
 *
 * @param base LPSPI peripheral address.
 * @param data The data word to be sent.
 */
static inline void LPSPI_WriteData(LPSPI_Type *base, uint32_t data)
{
    base->TDR = data;
}

/*!
 * @brief Reads data from the data buffer.
 *
 * This function reads the data from the Receive Data Register (RDR).
 * This function can be used for either master or slave mode.
 *
 * @param base LPSPI peripheral address.
 * @return The data read from the data buffer.
 */
static inline uint32_t LPSPI_ReadData(LPSPI_Type *base)
{
    return (base->RDR);
}

/*!
 * @brief Set up the dummy data.
 *
 * @param base LPSPI peripheral address.
 * @param dummyData Data to be transferred when tx buffer is NULL.
 * Note:
 *      This API has no effect when LPSPI in slave interrupt mode, because driver
 *      will set the TXMSK bit to 1 if txData is NULL, no data is loaded from transmit
 *      FIFO and output pin is tristated.
 */
void LPSPI_SetDummyData(LPSPI_Type *base, uint8_t dummyData);

/*!
 *@}
 */

/*!
 * @name Transactional
 * @{
 */
/*Transactional APIs*/

/*!
 * @brief Initializes the LPSPI master handle.
 *
 * This function initializes the LPSPI handle, which can be used for other LPSPI transactional APIs.  Usually, for a
 * specified LPSPI instance, call this API once to get the initialized handle.

 * @param base LPSPI peripheral address.
 * @param handle LPSPI handle pointer to lpspi_master_handle_t.
 * @param callback DSPI callback.
 * @param userData callback function parameter.
 */
void LPSPI_MasterTransferCreateHandle(LPSPI_Type *base,
                                      lpspi_master_handle_t *handle,
                                      lpspi_master_transfer_callback_t callback,
                                      void *userData);

/*!
 * @brief LPSPI master transfer data using a polling method.
 *
 * This function transfers data using a  polling method. This is a blocking function, which does not return until all
 * transfers have been
 * completed.
 *
 * Note:
 * The transfer data size should be integer multiples of bytesPerFrame if bytesPerFrame is less than or equal to 4.
 * For bytesPerFrame greater than 4:
 * The transfer data size should be equal to bytesPerFrame if the bytesPerFrame is not integer multiples of 4.
 * Otherwise, the transfer data size can be an integer multiple of bytesPerFrame.
 *
 * @param base LPSPI peripheral address.
 * @param transfer pointer to lpspi_transfer_t structure.
 * @return status of status_t.
 */
status_t LPSPI_MasterTransferBlocking(LPSPI_Type *base, lpspi_transfer_t *transfer);

/*!
 * @brief LPSPI master transfer data using an interrupt method.
 *
 * This function transfers data using an interrupt method. This is a non-blocking function, which returns right away.
 * When all data
 * is transferred, the callback function is called.
 *
 * Note:
 * The transfer data size should be integer multiples of bytesPerFrame if bytesPerFrame is less than or equal to 4.
 * For bytesPerFrame greater than 4:
 * The transfer data size should be equal to bytesPerFrame if the bytesPerFrame is not integer multiples of 4.
 * Otherwise, the transfer data size can be an integer multiple of bytesPerFrame.
 *
 * @param base LPSPI peripheral address.
 * @param handle pointer to lpspi_master_handle_t structure which stores the transfer state.
 * @param transfer pointer to lpspi_transfer_t structure.
 * @return status of status_t.
 */
status_t LPSPI_MasterTransferNonBlocking(LPSPI_Type *base, lpspi_master_handle_t *handle, lpspi_transfer_t *transfer);

/*!
 * @brief Gets the master transfer remaining bytes.
 *
 * This function gets the master transfer remaining bytes.
 *
 * @param base LPSPI peripheral address.
 * @param handle pointer to lpspi_master_handle_t structure which stores the transfer state.
 * @param count Number of bytes transferred so far by the non-blocking transaction.
 * @return status of status_t.
 */
status_t LPSPI_MasterTransferGetCount(LPSPI_Type *base, lpspi_master_handle_t *handle, size_t *count);

/*!
 * @brief LPSPI master abort transfer which uses an interrupt method.
 *
 * This function aborts a transfer which uses an interrupt method.
 *
 * @param base LPSPI peripheral address.
 * @param handle pointer to lpspi_master_handle_t structure which stores the transfer state.
 */
void LPSPI_MasterTransferAbort(LPSPI_Type *base, lpspi_master_handle_t *handle);

/*!
 * @brief LPSPI Master IRQ handler function.
 *
 * This function processes the LPSPI transmit and receive IRQ.
 *
 * @param base LPSPI peripheral address.
 * @param handle pointer to lpspi_master_handle_t structure which stores the transfer state.
 */
void LPSPI_MasterTransferHandleIRQ(LPSPI_Type *base, lpspi_master_handle_t *handle);

/*!
 * @brief Initializes the LPSPI slave handle.
 *
 * This function initializes the LPSPI handle, which can be used for other LPSPI transactional APIs.  Usually, for a
 * specified LPSPI instance, call this API once to get the initialized handle.
 *
 * @param base LPSPI peripheral address.
 * @param handle LPSPI handle pointer to lpspi_slave_handle_t.
 * @param callback DSPI callback.
 * @param userData callback function parameter.
 */
void LPSPI_SlaveTransferCreateHandle(LPSPI_Type *base,
                                     lpspi_slave_handle_t *handle,
                                     lpspi_slave_transfer_callback_t callback,
                                     void *userData);

/*!
 * @brief LPSPI slave transfer data using an interrupt method.
 *
 * This function transfer data using an interrupt method. This is a non-blocking function, which returns right away.
 * When all data
 * is transferred, the callback function is called.
 *
 * Note:
 * The transfer data size should be integer multiples of bytesPerFrame if bytesPerFrame is less than or equal to 4.
 * For bytesPerFrame greater than 4:
 * The transfer data size should be equal to bytesPerFrame if the bytesPerFrame is not an integer multiple of 4.
 * Otherwise, the transfer data size can be an integer multiple of bytesPerFrame.
 *
 * @param base LPSPI peripheral address.
 * @param handle pointer to lpspi_slave_handle_t structure which stores the transfer state.
 * @param transfer pointer to lpspi_transfer_t structure.
 * @return status of status_t.
 */
status_t LPSPI_SlaveTransferNonBlocking(LPSPI_Type *base, lpspi_slave_handle_t *handle, lpspi_transfer_t *transfer);

/*!
 * @brief Gets the slave transfer remaining bytes.
 *
 * This function gets the slave transfer remaining bytes.
 *
 * @param base LPSPI peripheral address.
 * @param handle pointer to lpspi_slave_handle_t structure which stores the transfer state.
 * @param count Number of bytes transferred so far by the non-blocking transaction.
 * @return status of status_t.
 */
status_t LPSPI_SlaveTransferGetCount(LPSPI_Type *base, lpspi_slave_handle_t *handle, size_t *count);

/*!
 * @brief LPSPI slave aborts a transfer which uses an interrupt method.
 *
 * This function aborts a transfer which uses an interrupt method.
 *
 * @param base LPSPI peripheral address.
 * @param handle pointer to lpspi_slave_handle_t structure which stores the transfer state.
 */
void LPSPI_SlaveTransferAbort(LPSPI_Type *base, lpspi_slave_handle_t *handle);

/*!
 * @brief LPSPI Slave IRQ handler function.
 *
 * This function processes the LPSPI transmit and receives an IRQ.
 *
 * @param base LPSPI peripheral address.
 * @param handle pointer to lpspi_slave_handle_t structure which stores the transfer state.
 */
void LPSPI_SlaveTransferHandleIRQ(LPSPI_Type *base, lpspi_slave_handle_t *handle);

/*!
 *@}
 */

#if defined(__cplusplus)
}
#endif /*_cplusplus*/
       /*!
        *@}
        */

#endif /*_FSL_LPSPI_H_*/
