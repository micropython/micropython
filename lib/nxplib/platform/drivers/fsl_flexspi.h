/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __FSL_FLEXSPI_H_
#define __FSL_FLEXSPI_H_

#include <stddef.h>
#include "fsl_device_registers.h"
#include "fsl_common.h"

/*!
 * @addtogroup flexspi
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief FLEXSPI driver version 2.1.1. */
#define FSL_FLEXSPI_DRIVER_VERSION (MAKE_VERSION(2, 1, 1))
/*@}*/

#define FSL_FEATURE_FLEXSPI_AHB_BUFFER_COUNT FSL_FEATURE_FLEXSPI_AHB_BUFFER_COUNTn(0)

/*! @breif Formula to form FLEXSPI instructions in LUT table. */
#define FLEXSPI_LUT_SEQ(cmd0, pad0, op0, cmd1, pad1, op1)                                                              \
    (FLEXSPI_LUT_OPERAND0(op0) | FLEXSPI_LUT_NUM_PADS0(pad0) | FLEXSPI_LUT_OPCODE0(cmd0) | FLEXSPI_LUT_OPERAND1(op1) | \
     FLEXSPI_LUT_NUM_PADS1(pad1) | FLEXSPI_LUT_OPCODE1(cmd1))

/*! @brief Status structure of FLEXSPI.*/
enum _flexspi_status
{
    kStatus_FLEXSPI_Busy                     = MAKE_STATUS(kStatusGroup_FLEXSPI, 0), /*!< FLEXSPI is busy */
    kStatus_FLEXSPI_SequenceExecutionTimeout = MAKE_STATUS(kStatusGroup_FLEXSPI, 1), /*!< Sequence execution timeout
                                                                            error occurred during FLEXSPI transfer. */
    kStatus_FLEXSPI_IpCommandSequenceError = MAKE_STATUS(kStatusGroup_FLEXSPI, 2),   /*!< IP command Sequence execution
                                                                     timeout error occurred during FLEXSPI transfer. */
    kStatus_FLEXSPI_IpCommandGrantTimeout = MAKE_STATUS(kStatusGroup_FLEXSPI, 3),    /*!< IP command grant timeout error
                                                                                    occurred during FLEXSPI transfer. */
};

/*! @brief CMD definition of FLEXSPI, use to form LUT instruction. */
enum _flexspi_command
{
    kFLEXSPI_Command_STOP           = 0x00U, /*!< Stop execution, deassert CS. */
    kFLEXSPI_Command_SDR            = 0x01U, /*!< Transmit Command code to Flash, using SDR mode. */
    kFLEXSPI_Command_RADDR_SDR      = 0x02U, /*!< Transmit Row Address to Flash, using SDR mode. */
    kFLEXSPI_Command_CADDR_SDR      = 0x03U, /*!< Transmit Column Address to Flash, using SDR mode. */
    kFLEXSPI_Command_MODE1_SDR      = 0x04U, /*!< Transmit 1-bit Mode bits to Flash, using SDR mode. */
    kFLEXSPI_Command_MODE2_SDR      = 0x05U, /*!< Transmit 2-bit Mode bits to Flash, using SDR mode. */
    kFLEXSPI_Command_MODE4_SDR      = 0x06U, /*!< Transmit 4-bit Mode bits to Flash, using SDR mode. */
    kFLEXSPI_Command_MODE8_SDR      = 0x07U, /*!< Transmit 8-bit Mode bits to Flash, using SDR mode. */
    kFLEXSPI_Command_WRITE_SDR      = 0x08U, /*!< Transmit Programming Data to Flash, using SDR mode. */
    kFLEXSPI_Command_READ_SDR       = 0x09U, /*!< Receive Read Data from Flash, using SDR mode. */
    kFLEXSPI_Command_LEARN_SDR      = 0x0AU, /*!< Receive Read Data or Preamble bit from Flash, SDR mode. */
    kFLEXSPI_Command_DATSZ_SDR      = 0x0BU, /*!< Transmit Read/Program Data size (byte) to Flash, SDR mode. */
    kFLEXSPI_Command_DUMMY_SDR      = 0x0CU, /*!< Leave data lines undriven by FlexSPI controller.*/
    kFLEXSPI_Command_DUMMY_RWDS_SDR = 0x0DU, /*!< Leave data lines undriven by FlexSPI controller,
                                                  dummy cycles decided by RWDS. */
    kFLEXSPI_Command_DDR            = 0x21U, /*!< Transmit Command code to Flash, using DDR mode. */
    kFLEXSPI_Command_RADDR_DDR      = 0x22U, /*!< Transmit Row Address to Flash, using DDR mode. */
    kFLEXSPI_Command_CADDR_DDR      = 0x23U, /*!< Transmit Column Address to Flash, using DDR mode. */
    kFLEXSPI_Command_MODE1_DDR      = 0x24U, /*!< Transmit 1-bit Mode bits to Flash, using DDR mode. */
    kFLEXSPI_Command_MODE2_DDR      = 0x25U, /*!< Transmit 2-bit Mode bits to Flash, using DDR mode. */
    kFLEXSPI_Command_MODE4_DDR      = 0x26U, /*!< Transmit 4-bit Mode bits to Flash, using DDR mode. */
    kFLEXSPI_Command_MODE8_DDR      = 0x27U, /*!< Transmit 8-bit Mode bits to Flash, using DDR mode. */
    kFLEXSPI_Command_WRITE_DDR      = 0x28U, /*!< Transmit Programming Data to Flash, using DDR mode. */
    kFLEXSPI_Command_READ_DDR       = 0x29U, /*!< Receive Read Data from Flash, using DDR mode. */
    kFLEXSPI_Command_LEARN_DDR      = 0x2AU, /*!< Receive Read Data or Preamble bit from Flash, DDR mode. */
    kFLEXSPI_Command_DATSZ_DDR      = 0x2BU, /*!< Transmit Read/Program Data size (byte) to Flash, DDR mode. */
    kFLEXSPI_Command_DUMMY_DDR      = 0x2CU, /*!< Leave data lines undriven by FlexSPI controller.*/
    kFLEXSPI_Command_DUMMY_RWDS_DDR = 0x2DU, /*!< Leave data lines undriven by FlexSPI controller,
                                               dummy cycles decided by RWDS. */
    kFLEXSPI_Command_JUMP_ON_CS = 0x1FU,     /*!< Stop execution, deassert CS and save operand[7:0] as the
                                               instruction start pointer for next sequence */
};

/*! @brief pad definition of FLEXSPI, use to form LUT instruction. */
typedef enum _flexspi_pad
{
    kFLEXSPI_1PAD = 0x00U, /*!< Transmit command/address and transmit/receive data only through DATA0/DATA1. */
    kFLEXSPI_2PAD = 0x01U, /*!< Transmit command/address and transmit/receive data only through DATA[1:0]. */
    kFLEXSPI_4PAD = 0x02U, /*!< Transmit command/address and transmit/receive data only through DATA[3:0]. */
    kFLEXSPI_8PAD = 0x03U, /*!< Transmit command/address and transmit/receive data only through DATA[7:0]. */
} flexspi_pad_t;

/*! @brief FLEXSPI interrupt status flags.*/
typedef enum _flexspi_flags
{
    kFLEXSPI_SequenceExecutionTimeoutFlag = FLEXSPI_INTEN_SEQTIMEOUTEN_MASK,    /*!< Sequence execution timeout. */
    kFLEXSPI_AhbBusTimeoutFlag            = FLEXSPI_INTEN_AHBBUSTIMEOUTEN_MASK, /*!< AHB Bus timeout. */
    kFLEXSPI_SckStoppedBecauseTxEmptyFlag =
        FLEXSPI_INTEN_SCKSTOPBYWREN_MASK, /*!< SCK is stopped during command
                                               sequence because Async TX FIFO empty. */
    kFLEXSPI_SckStoppedBecauseRxFullFlag =
        FLEXSPI_INTEN_SCKSTOPBYRDEN_MASK, /*!< SCK is stopped during command
                                               sequence because Async RX FIFO full. */
#if !((defined(FSL_FEATURE_FLEXSPI_HAS_NO_DATA_LEARN)) && (FSL_FEATURE_FLEXSPI_HAS_NO_DATA_LEARN))
    kFLEXSPI_DataLearningFailedFlag = FLEXSPI_INTEN_DATALEARNFAILEN_MASK, /*!< Data learning failed. */
#endif
    kFLEXSPI_IpTxFifoWatermarkEmpltyFlag    = FLEXSPI_INTEN_IPTXWEEN_MASK, /*!< IP TX FIFO WaterMark empty. */
    kFLEXSPI_IpRxFifoWatermarkAvailableFlag = FLEXSPI_INTEN_IPRXWAEN_MASK, /*!< IP RX FIFO WaterMark available. */
    kFLEXSPI_AhbCommandSequenceErrorFlag =
        FLEXSPI_INTEN_AHBCMDERREN_MASK,                                  /*!< AHB triggered Command Sequences Error. */
    kFLEXSPI_IpCommandSequenceErrorFlag = FLEXSPI_INTEN_IPCMDERREN_MASK, /*!< IP triggered Command Sequences Error. */
    kFLEXSPI_AhbCommandGrantTimeoutFlag =
        FLEXSPI_INTEN_AHBCMDGEEN_MASK, /*!< AHB triggered Command Sequences Grant Timeout. */
    kFLEXSPI_IpCommandGrantTimeoutFlag =
        FLEXSPI_INTEN_IPCMDGEEN_MASK, /*!< IP triggered Command Sequences Grant Timeout. */
    kFLEXSPI_IpCommandExcutionDoneFlag =
        FLEXSPI_INTEN_IPCMDDONEEN_MASK,  /*!< IP triggered Command Sequences Execution finished. */
    kFLEXSPI_AllInterruptFlags = 0xFFFU, /*!< All flags. */
} flexspi_flags_t;

/*! @brief FLEXSPI sample clock source selection for Flash Reading.*/
typedef enum _flexspi_read_sample_clock
{
    kFLEXSPI_ReadSampleClkLoopbackInternally = 0x0U,      /*!< Dummy Read strobe generated by FlexSPI Controller
                                                               and loopback internally. */
    kFLEXSPI_ReadSampleClkLoopbackFromDqsPad = 0x1U,      /*!< Dummy Read strobe generated by FlexSPI Controller
                                                               and loopback from DQS pad. */
    kFLEXSPI_ReadSampleClkLoopbackFromSckPad      = 0x2U, /*!< SCK output clock and loopback from SCK pad. */
    kFLEXSPI_ReadSampleClkExternalInputFromDqsPad = 0x3U, /*!< Flash provided Read strobe and input from DQS pad. */
} flexspi_read_sample_clock_t;

/*! @brief FLEXSPI interval unit for flash device select.*/
typedef enum _flexspi_cs_interval_cycle_unit
{
    kFLEXSPI_CsIntervalUnit1SckCycle   = 0x0U, /*!< Chip selection interval: CSINTERVAL * 1 serial clock cycle. */
    kFLEXSPI_CsIntervalUnit256SckCycle = 0x1U, /*!< Chip selection interval: CSINTERVAL * 256 serial clock cycle. */
} flexspi_cs_interval_cycle_unit_t;

/*! @brief FLEXSPI AHB wait interval unit for writting.*/
typedef enum _flexspi_ahb_write_wait_unit
{
    kFLEXSPI_AhbWriteWaitUnit2AhbCycle     = 0x0U, /*!< AWRWAIT unit is 2 ahb clock cycle. */
    kFLEXSPI_AhbWriteWaitUnit8AhbCycle     = 0x1U, /*!< AWRWAIT unit is 8 ahb clock cycle. */
    kFLEXSPI_AhbWriteWaitUnit32AhbCycle    = 0x2U, /*!< AWRWAIT unit is 32 ahb clock cycle. */
    kFLEXSPI_AhbWriteWaitUnit128AhbCycle   = 0x3U, /*!< AWRWAIT unit is 128 ahb clock cycle. */
    kFLEXSPI_AhbWriteWaitUnit512AhbCycle   = 0x4U, /*!< AWRWAIT unit is 512 ahb clock cycle. */
    kFLEXSPI_AhbWriteWaitUnit2048AhbCycle  = 0x5U, /*!< AWRWAIT unit is 2048 ahb clock cycle. */
    kFLEXSPI_AhbWriteWaitUnit8192AhbCycle  = 0x6U, /*!< AWRWAIT unit is 8192 ahb clock cycle. */
    kFLEXSPI_AhbWriteWaitUnit32768AhbCycle = 0x7U, /*!< AWRWAIT unit is 32768 ahb clock cycle. */
} flexspi_ahb_write_wait_unit_t;

/*! @brief Error Code when IP command Error detected.*/
typedef enum _flexspi_ip_error_code
{
    kFLEXSPI_IpCmdErrorNoError               = 0x0U,    /*!< No error. */
    kFLEXSPI_IpCmdErrorJumpOnCsInIpCmd       = 0x2U,    /*!< IP command with JMP_ON_CS instruction used. */
    kFLEXSPI_IpCmdErrorUnknownOpCode         = 0x3U,    /*!< Unknown instruction opcode in the sequence. */
    kFLEXSPI_IpCmdErrorSdrDummyInDdrSequence = 0x4U,    /*!< Instruction DUMMY_SDR/DUMMY_RWDS_SDR
                                                             used in DDR sequence. */
    kFLEXSPI_IpCmdErrorDdrDummyInSdrSequence = 0x5U,    /*!< Instruction DUMMY_DDR/DUMMY_RWDS_DDR
                                                             used in SDR sequence. */
    kFLEXSPI_IpCmdErrorInvalidAddress = 0x6U,           /*!< Flash access start address exceed the whole
                                                            flash address range (A1/A2/B1/B2). */
    kFLEXSPI_IpCmdErrorSequenceExecutionTimeout = 0xEU, /*!< Sequence execution timeout. */
    kFLEXSPI_IpCmdErrorFlashBoundaryAcrosss     = 0xFU, /*!< Flash boundary crossed. */
} flexspi_ip_error_code_t;

/*! @brief Error Code when AHB command Error detected.*/
typedef enum _flexspi_ahb_error_code
{
    kFLEXSPI_AhbCmdErrorNoError            = 0x0U,    /*!< No error. */
    kFLEXSPI_AhbCmdErrorJumpOnCsInWriteCmd = 0x2U,    /*!< AHB Write command with JMP_ON_CS instruction
                                                           used in the sequence. */
    kFLEXSPI_AhbCmdErrorUnknownOpCode         = 0x3U, /*!< Unknown instruction opcode in the sequence. */
    kFLEXSPI_AhbCmdErrorSdrDummyInDdrSequence = 0x4U, /*!< Instruction DUMMY_SDR/DUMMY_RWDS_SDR used
                                                           in DDR sequence. */
    kFLEXSPI_AhbCmdErrorDdrDummyInSdrSequence = 0x5U, /*!< Instruction DUMMY_DDR/DUMMY_RWDS_DDR
                                                           used in SDR sequence. */
    kFLEXSPI_AhbCmdSequenceExecutionTimeout = 0x6U,   /*!< Sequence execution timeout. */
} flexspi_ahb_error_code_t;

/*! @brief FLEXSPI operation port select.*/
typedef enum _flexspi_port
{
    kFLEXSPI_PortA1 = 0x0U, /*!< Access flash on A1 port. */
    kFLEXSPI_PortA2,        /*!< Access flash on A2 port. */
    kFLEXSPI_PortB1,        /*!< Access flash on B1 port. */
    kFLEXSPI_PortB2,        /*!< Access flash on B2 port. */
    kFLEXSPI_PortCount
} flexspi_port_t;

/*! @brief Trigger source of current command sequence granted by arbitrator.*/
typedef enum _flexspi_arb_command_source
{
    kFLEXSPI_AhbReadCommand   = 0x0U,
    kFLEXSPI_AhbWriteCommand  = 0x1U,
    kFLEXSPI_IpCommand        = 0x2U,
    kFLEXSPI_SuspendedCommand = 0x3U,
} flexspi_arb_command_source_t;

typedef enum _flexspi_command_type
{
    kFLEXSPI_Command, /*!< FlexSPI operation: Only command, both TX and Rx buffer are ignored. */
    kFLEXSPI_Config,  /*!< FlexSPI operation: Configure device mode, the TX fifo size is fixed in LUT. */
    kFLEXSPI_Read,    /* /!< FlexSPI operation: Read, only Rx Buffer is effective. */
    kFLEXSPI_Write,   /* /!< FlexSPI operation: Read, only Tx Buffer is effective. */
} flexspi_command_type_t;

typedef struct _flexspi_ahbBuffer_config
{
    uint8_t priority;    /*!< This priority for AHB Master Read which this AHB RX Buffer is assigned. */
    uint8_t masterIndex; /*!< AHB Master ID the AHB RX Buffer is assigned. */
    uint16_t bufferSize; /*!< AHB buffer size in byte. */
    bool enablePrefetch; /*!< AHB Read Prefetch Enable for current AHB RX Buffer corresponding Master, allows
                          prefetch disable/enable seperately for each master. */
} flexspi_ahbBuffer_config_t;

/*! @brief FLEXSPI configuration structure. */
typedef struct _flexspi_config
{
    flexspi_read_sample_clock_t rxSampleClock; /*!< Sample Clock source selection for Flash Reading. */
    bool enableSckFreeRunning;                 /*!< Enable/disable SCK output free-running. */
    bool enableCombination;                    /*!< Enable/disable combining PORT A and B Data Pins
                                               (SIOA[3:0] and SIOB[3:0]) to support Flash Octal mode. */
    bool enableDoze;                           /*!< Enable/disable doze mode support. */
    bool enableHalfSpeedAccess;                /*!< Enable/disable divide by 2 of the clock for half
                                                speed commands. */
    bool enableSckBDiffOpt;                    /*!< Enable/disable SCKB pad use as SCKA differential clock
                                                output, when enable, Port B flash access is not available. */
    bool enableSameConfigForAll;               /*!< Enable/disable same configuration for all connected devices
                                                when enabled, same configuration in FLASHA1CRx is applied to all. */
    uint16_t seqTimeoutCycle;                  /*!< Timeout wait cycle for command sequence execution,
                                               timeout after ahbGrantTimeoutCyle*1024 serial root clock cycles. */
    uint8_t ipGrantTimeoutCycle;               /*!< Timeout wait cycle for IP command grant, timeout after
                                                ipGrantTimeoutCycle*1024 AHB clock cycles. */
    uint8_t txWatermark;                       /*!< FLEXSPI IP transmit watermark value. */
    uint8_t rxWatermark;                       /*!< FLEXSPI receive watermark value. */
    struct
    {
        bool enableAHBWriteIpTxFifo;  /*!< Enable AHB bus write access to IP TX FIFO. */
        bool enableAHBWriteIpRxFifo;  /*!< Enable AHB bus write access to IP RX FIFO. */
        uint8_t ahbGrantTimeoutCycle; /*!< Timeout wait cycle for AHB command grant,
                                       timeout after ahbGrantTimeoutCyle*1024 AHB clock cycles. */
        uint16_t ahbBusTimeoutCycle;  /*!< Timeout wait cycle for AHB read/write access,
                                      timeout after ahbBusTimeoutCycle*1024 AHB clock cycles. */
        uint8_t resumeWaitCycle;      /*!< Wait cycle for idle state before suspended command sequence
                                       resume, timeout after ahbBusTimeoutCycle AHB clock cycles. */
        flexspi_ahbBuffer_config_t buffer[FSL_FEATURE_FLEXSPI_AHB_BUFFER_COUNT]; /*!< AHB buffer size. */
        bool enableClearAHBBufferOpt; /*!< Enable/disable automatically clean AHB RX Buffer and TX Buffer
                                       when FLEXSPI returns STOP mode ACK. */
        bool enableReadAddressOpt;    /*!< Enable/disable remove AHB read burst start address alignment limitation.
                                       when eanble, there is no AHB read burst start address alignment limitation. */
        bool enableAHBPrefetch;       /*!< Enable/disable AHB read prefetch feature, when enabled, FLEXSPI
                                       will fetch more data than current AHB burst. */
        bool enableAHBBufferable;     /*!< Enable/disable AHB bufferable write access support, when enabled,
                                       FLEXSPI return before waiting for command excution finished. */
        bool enableAHBCachable;       /*!< Enable AHB bus cachable read access support. */
    } ahbConfig;
} flexspi_config_t;

/*! @brief External device configuration items. */
typedef struct _flexspi_device_config
{
    uint32_t flexspiRootClk;                         /*!< FLEXSPI serial root clock. */
    bool isSck2Enabled;                              /*!< FLEXSPI use SCK2. */
    uint32_t flashSize;                              /*!< Flash size in KByte. */
    flexspi_cs_interval_cycle_unit_t CSIntervalUnit; /*!< CS interval unit, 1 or 256 cycle. */
    uint16_t CSInterval;                             /*!< CS line assert interval, mutiply CS interval unit to
                                                      get the CS line assert interval cycles. */
    uint8_t CSHoldTime;                              /*!< CS line hold time. */
    uint8_t CSSetupTime;                             /*!< CS line setup time. */
    uint8_t dataValidTime;                           /*!< Data valid time for external device. */
    uint8_t columnspace;                             /*!< Column space size. */
    bool enableWordAddress;                          /*!< If enable word address.*/
    uint8_t AWRSeqIndex;                             /*!< Sequence ID for AHB write command. */
    uint8_t AWRSeqNumber;                            /*!< Sequence number for AHB write command. */
    uint8_t ARDSeqIndex;                             /*!< Sequence ID for AHB read command. */
    uint8_t ARDSeqNumber;                            /*!< Sequence number for AHB read command. */
    flexspi_ahb_write_wait_unit_t AHBWriteWaitUnit;  /*!< AHB write wait unit. */
    uint16_t AHBWriteWaitInterval;                   /*!< AHB write wait interval, mutiply AHB write interval
                                                      unit to get the AHB write wait cycles. */
    bool enableWriteMask;                            /*!< Enable/Disable FLEXSPI drive DQS pin as write mask
                                                      when writing to external device. */
} flexspi_device_config_t;

/*! @brief Transfer structure for FLEXSPI. */
typedef struct _flexspi_transfer
{
    uint32_t deviceAddress;         /*!< Operation device address. */
    flexspi_port_t port;            /*!< Operation port. */
    flexspi_command_type_t cmdType; /*!< Execution command type. */
    uint8_t seqIndex;               /*!< Sequence ID for command. */
    uint8_t SeqNumber;              /*!< Sequence number for command. */
    uint32_t *data;                 /*!< Data buffer. */
    size_t dataSize;                /*!< Data size in bytes. */
} flexspi_transfer_t;

/* Forward declaration of the handle typedef. */
typedef struct _flexspi_handle flexspi_handle_t;

/*! @brief FLEXSPI transfer callback function. */
typedef void (*flexspi_transfer_callback_t)(FLEXSPI_Type *base,
                                            flexspi_handle_t *handle,
                                            status_t status,
                                            void *userData);

/*! @brief Transfer handle structure for FLEXSPI. */
struct _flexspi_handle
{
    uint32_t state;                                 /*!< Internal state for FLEXSPI transfer */
    uint32_t *data;                                 /*!< Data buffer. */
    size_t dataSize;                                /*!< Remaining Data size in bytes. */
    size_t transferTotalSize;                       /*!< Total Data size in bytes. */
    flexspi_transfer_callback_t completionCallback; /*!< Callback for users while transfer finish or error occurred */
    void *userData;                                 /*!< FLEXSPI callback function parameter.*/
};

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus. */

/*!
 * @name Initialization and deinitialization
 * @{
 */
/*!
 * @brief Initializes the FLEXSPI module and internal state.
 *
 * This function enables the clock for FLEXSPI and also configures the FLEXSPI with the
 * input configure parameters. Users should call this function before any FLEXSPI operations.
 *
 * @param base FLEXSPI peripheral base address.
 * @param config FLEXSPI configure structure.
 */
void FLEXSPI_Init(FLEXSPI_Type *base, const flexspi_config_t *config);

/*!
 * @brief Gets default settings for FLEXSPI.
 *
 * @param config FLEXSPI configuration structure.
 */
void FLEXSPI_GetDefaultConfig(flexspi_config_t *config);

/*!
 * @brief Deinitializes the FLEXSPI module.
 *
 * Clears the FLEXSPI state and  FLEXSPI module registers.
 * @param base FLEXSPI peripheral base address.
 */
void FLEXSPI_Deinit(FLEXSPI_Type *base);

/*!
 * @brief Configures the connected device parameter.
 *
 * This function configures the connected device relevant parameters, such as the size, command, and so on.
 * The flash configuration value cannot have a default value. The user needs to configure it according to the
 * connected device.
 *
 * @param base FLEXSPI peripheral base address.
 * @param config Flash configuration parameters.
 * @param port FLEXSPI Operation port.
 */
void FLEXSPI_SetFlashConfig(FLEXSPI_Type *base, flexspi_device_config_t *config, flexspi_port_t port);

/*!
 * @brief Software reset for the FLEXSPI logic.
 *
 * This function sets the software reset flags for both AHB and buffer domain and
 * resets both AHB buffer and also IP FIFOs.
 *
 * @param base FLEXSPI peripheral base address.
 */
static inline void FLEXSPI_SoftwareReset(FLEXSPI_Type *base)
{
    base->MCR0 |= FLEXSPI_MCR0_SWRESET_MASK;
    while (base->MCR0 & FLEXSPI_MCR0_SWRESET_MASK)
    {
    }
}

/*!
 * @brief Enables or disables the FLEXSPI module.
 *
 * @param base FLEXSPI peripheral base address.
 * @param enable True means enable FLEXSPI, false means disable.
 */
static inline void FLEXSPI_Enable(FLEXSPI_Type *base, bool enable)
{
    if (enable)
    {
        base->MCR0 &= ~FLEXSPI_MCR0_MDIS_MASK;
    }
    else
    {
        base->MCR0 |= FLEXSPI_MCR0_MDIS_MASK;
    }
}

/* @} */

/*!
 * @name Interrupts
 * @{
 */
/*!
 * @brief Enables the FLEXSPI interrupts.
 *
 * @param base FLEXSPI peripheral base address.
 * @param mask FLEXSPI interrupt source.
 */
static inline void FLEXSPI_EnableInterrupts(FLEXSPI_Type *base, uint32_t mask)
{
    base->INTEN |= mask;
}

/*!
 * @brief Disable the FLEXSPI interrupts.
 *
 * @param base FLEXSPI peripheral base address.
 * @param mask FLEXSPI interrupt source.
 */
static inline void FLEXSPI_DisableInterrupts(FLEXSPI_Type *base, uint32_t mask)
{
    base->INTEN &= ~mask;
}

/* @} */

/*! @name DMA control */
/*@{*/

/*!
 * @brief Enables or disables FLEXSPI IP Tx FIFO DMA requests.
 *
 * @param base FLEXSPI peripheral base address.
 * @param enable Enable flag for transmit DMA request. Pass true for enable, false for disable.
 */
static inline void FLEXSPI_EnableTxDMA(FLEXSPI_Type *base, bool enable)
{
    if (enable)
    {
        base->IPTXFCR |= FLEXSPI_IPTXFCR_TXDMAEN_MASK;
    }
    else
    {
        base->IPTXFCR &= ~FLEXSPI_IPTXFCR_TXDMAEN_MASK;
    }
}

/*!
 * @brief Enables or disables FLEXSPI IP Rx FIFO DMA requests.
 *
 * @param base FLEXSPI peripheral base address.
 * @param enable Enable flag for receive DMA request. Pass true for enable, false for disable.
 */
static inline void FLEXSPI_EnableRxDMA(FLEXSPI_Type *base, bool enable)
{
    if (enable)
    {
        base->IPRXFCR |= FLEXSPI_IPRXFCR_RXDMAEN_MASK;
    }
    else
    {
        base->IPRXFCR &= ~FLEXSPI_IPRXFCR_RXDMAEN_MASK;
    }
}

/*!
 * @brief Gets FLEXSPI IP tx fifo address for DMA transfer.
 *
 * @param base FLEXSPI peripheral base address.
 * @retval The tx fifo address.
 */
static inline uint32_t FLEXSPI_GetTxFifoAddress(FLEXSPI_Type *base)
{
    return (uint32_t)&base->TFDR[0];
}

/*!
 * @brief Gets FLEXSPI IP rx fifo address for DMA transfer.
 *
 * @param base FLEXSPI peripheral base address.
 * @retval The rx fifo address.
 */
static inline uint32_t FLEXSPI_GetRxFifoAddress(FLEXSPI_Type *base)
{
    return (uint32_t)&base->RFDR[0];
}

/*@}*/

/*! @name FIFO control */
/*@{*/

/*! @brief Clears the FLEXSPI IP FIFO logic.
 *
 * @param base FLEXSPI peripheral base address.
 * @param txFifo Pass true to reset TX FIFO.
 * @param rxFifo Pass true to reset RX FIFO.
 */
static inline void FLEXSPI_ResetFifos(FLEXSPI_Type *base, bool txFifo, bool rxFifo)
{
    if (txFifo)
    {
        base->IPTXFCR |= FLEXSPI_IPTXFCR_CLRIPTXF_MASK;
    }
    if (rxFifo)
    {
        base->IPRXFCR |= FLEXSPI_IPRXFCR_CLRIPRXF_MASK;
    }
}

/*!
 * @brief Gets the valid data entries in the FLEXSPI FIFOs.
 *
 * @param base FLEXSPI peripheral base address.
 * @param[out] txCount Pointer through which the current number of bytes in the transmit FIFO is returned.
 *      Pass NULL if this value is not required.
 * @param[out] rxCount Pointer through which the current number of bytes in the receive FIFO is returned.
 *      Pass NULL if this value is not required.
 */
static inline void FLEXSPI_GetFifoCounts(FLEXSPI_Type *base, size_t *txCount, size_t *rxCount)
{
    if (txCount)
    {
        *txCount = (((base->IPTXFSTS) & FLEXSPI_IPTXFSTS_FILL_MASK) >> FLEXSPI_IPTXFSTS_FILL_SHIFT) * 8U;
    }
    if (rxCount)
    {
        *rxCount = (((base->IPRXFSTS) & FLEXSPI_IPRXFSTS_FILL_MASK) >> FLEXSPI_IPRXFSTS_FILL_SHIFT) * 8U;
    }
}

/*@}*/

/*!
 * @name Status
 * @{
 */
/*!
 * @brief Get the FLEXSPI interrupt status flags.
 *
 * @param base FLEXSPI peripheral base address.
 * @retval interrupt status flag, use status flag to AND #flexspi_flags_t could get the related status.
 */
static inline uint32_t FLEXSPI_GetInterruptStatusFlags(FLEXSPI_Type *base)
{
    return base->INTR;
}

/*!
 * @brief Get the FLEXSPI interrupt status flags.
 *
 * @param base FLEXSPI peripheral base address.
 * @param interrupt status flag.
 */
static inline void FLEXSPI_ClearInterruptStatusFlags(FLEXSPI_Type *base, uint32_t mask)
{
    base->INTR |= mask;
}

#if !((defined(FSL_FEATURE_FLEXSPI_HAS_NO_DATA_LEARN)) && (FSL_FEATURE_FLEXSPI_HAS_NO_DATA_LEARN))
/*! @brief Gets the sampling clock phase selection after Data Learning.
 *
 * @param base FLEXSPI peripheral base address.
 * @param portAPhase Pointer to a uint8_t type variable to receive the selected clock phase on PORTA.
 * @param portBPhase Pointer to a uint8_t type variable to receive the selected clock phase on PORTB.
 */
static inline void FLEXSPI_GetDataLearningPhase(FLEXSPI_Type *base, uint8_t *portAPhase, uint8_t *portBPhase)
{
    if (portAPhase)
    {
        *portAPhase = (base->STS0 & FLEXSPI_STS0_DATALEARNPHASEA_MASK) >> FLEXSPI_STS0_DATALEARNPHASEA_SHIFT;
    }

    if (portBPhase)
    {
        *portBPhase = (base->STS0 & FLEXSPI_STS0_DATALEARNPHASEB_MASK) >> FLEXSPI_STS0_DATALEARNPHASEB_SHIFT;
    }
}
#endif

/*! @brief Gets the trigger source of current command sequence granted by arbitrator.
 *
 * @param base FLEXSPI peripheral base address.
 * @retval trigger source of current command sequence.
 */
static inline flexspi_arb_command_source_t FLEXSPI_GetArbitratorCommandSource(FLEXSPI_Type *base)
{
    return (flexspi_arb_command_source_t)((base->STS0 & FLEXSPI_STS0_ARBCMDSRC_MASK) >> FLEXSPI_STS0_ARBCMDSRC_SHIFT);
}

/*! @brief Gets the error code when IP command error detected.
 *
 * @param base FLEXSPI peripheral base address.
 * @param index Pointer to a uint8_t type variable to receive the sequence index when error detected.
 * @retval error code when IP command error detected.
 */
static inline flexspi_ip_error_code_t FLEXSPI_GetIPCommandErrorCode(FLEXSPI_Type *base, uint8_t *index)
{
    *index = (base->STS1 & FLEXSPI_STS1_IPCMDERRID_MASK) >> FLEXSPI_STS1_IPCMDERRID_SHIFT;
    return (flexspi_ip_error_code_t)((base->STS1 & FLEXSPI_STS1_IPCMDERRCODE_MASK) >> FLEXSPI_STS1_IPCMDERRCODE_SHIFT);
}

/*! @brief Gets the error code when AHB command error detected.
 *
 * @param base FLEXSPI peripheral base address.
 * @param index Pointer to a uint8_t type variable to receive the sequence index when error detected.
 * @retval error code when AHB command error detected.
 */
static inline flexspi_ahb_error_code_t FLEXSPI_GetAHBCommandErrorCode(FLEXSPI_Type *base, uint8_t *index)
{
    *index = (base->STS1 & FLEXSPI_STS1_AHBCMDERRID_MASK) >> FLEXSPI_STS1_AHBCMDERRID_SHIFT;
    return (flexspi_ahb_error_code_t)((base->STS1 & FLEXSPI_STS1_AHBCMDERRCODE_MASK) >>
                                      FLEXSPI_STS1_AHBCMDERRCODE_SHIFT);
}

/*! @brief Returns whether the bus is idle.
 *
 * @param base FLEXSPI peripheral base address.
 * @retval true Bus is idle.
 * @retval false Bus is busy.
 */
static inline bool FLEXSPI_GetBusIdleStatus(FLEXSPI_Type *base)
{
    return (base->STS0 & FLEXSPI_STS0_ARBIDLE_MASK) && (base->STS0 & FLEXSPI_STS0_SEQIDLE_MASK);
}
/*@}*/

/*!
 * @name Bus Operations
 * @{
 */

/*! @brief Update read sample clock source
 *
 * @param base FLEXSPI peripheral base address.
 * @param clockSource clockSource of type #flexspi_read_sample_clock_t
 */
void FLEXSPI_UpdateRxSampleClock(FLEXSPI_Type *base, flexspi_read_sample_clock_t clockSource);

/*! @brief Enables/disables the FLEXSPI IP command parallel mode.
 *
 * @param base FLEXSPI peripheral base address.
 * @param enable True means enable parallel mode, false means disable parallel mode.
 */
static inline void FLEXSPI_EnableIPParallelMode(FLEXSPI_Type *base, bool enable)
{
    if (enable)
    {
        base->IPCR1 |= FLEXSPI_IPCR1_IPAREN_MASK;
    }
    else
    {
        base->IPCR1 &= ~FLEXSPI_IPCR1_IPAREN_MASK;
    }
}

/*! @brief Enables/disables the FLEXSPI AHB command parallel mode.
 *
 * @param base FLEXSPI peripheral base address.
 * @param enable True means enable parallel mode, false means disable parallel mode.
 */
static inline void FLEXSPI_EnableAHBParallelMode(FLEXSPI_Type *base, bool enable)
{
    if (enable)
    {
        base->AHBCR |= FLEXSPI_AHBCR_APAREN_MASK;
    }
    else
    {
        base->AHBCR &= ~FLEXSPI_AHBCR_APAREN_MASK;
    }
}

/*! @brief Updates the LUT table.
 *
 * @param base FLEXSPI peripheral base address.
 * @param index From which index start to update. It could be any index of the LUT table, which
 * also allows user to update command content inside a command. Each command consists of up to
 * 8 instructions and occupy 4*32-bit memory.
 * @param cmd Command sequence array.
 * @param count Number of sequences.
 */
void FLEXSPI_UpdateLUT(FLEXSPI_Type *base, uint32_t index, const uint32_t *cmd, uint32_t count);

/*!
 * @brief Writes data into FIFO.
 *
 * @param base FLEXSPI peripheral base address
 * @param data The data bytes to send
 * @param fifoIndex Destination fifo index.
 */
static inline void FLEXSPI_WriteData(FLEXSPI_Type *base, uint32_t data, uint8_t fifoIndex)
{
    base->TFDR[fifoIndex] = data;
}

/*!
 * @brief Receives data from data FIFO.
 *
 * @param base FLEXSPI peripheral base address
 * @param fifoIndex Source fifo index.
 * @return The data in the FIFO.
 */
static inline uint32_t FLEXSPI_ReadData(FLEXSPI_Type *base, uint8_t fifoIndex)
{
    return base->RFDR[fifoIndex];
}

/*!
 * @brief Sends a buffer of data bytes using blocking method.
 * @note This function blocks via polling until all bytes have been sent.
 * @param base FLEXSPI peripheral base address
 * @param buffer The data bytes to send
 * @param size The number of data bytes to send
 * @retval kStatus_Success write success without error
 * @retval kStatus_FLEXSPI_SequenceExecutionTimeout sequence execution timeout
 * @retval kStatus_FLEXSPI_IpCommandSequenceError IP command sequencen error detected
 * @retval kStatus_FLEXSPI_IpCommandGrantTimeout IP command grant timeout detected
 */
status_t FLEXSPI_WriteBlocking(FLEXSPI_Type *base, uint32_t *buffer, size_t size);

/*!
 * @brief Receives a buffer of data bytes using a blocking method.
 * @note This function blocks via polling until all bytes have been sent.
 * @param base FLEXSPI peripheral base address
 * @param buffer The data bytes to send
 * @param size The number of data bytes to receive
 * @retval kStatus_Success read success without error
 * @retval kStatus_FLEXSPI_SequenceExecutionTimeout sequence execution timeout
 * @retval kStatus_FLEXSPI_IpCommandSequenceError IP command sequencen error detected
 * @retval kStatus_FLEXSPI_IpCommandGrantTimeout IP command grant timeout detected
 */
status_t FLEXSPI_ReadBlocking(FLEXSPI_Type *base, uint32_t *buffer, size_t size);

/*!
 * @brief Execute command to transfer a buffer data bytes using a blocking method.
 * @param base FLEXSPI peripheral base address
 * @param xfer pointer to the transfer structure.
 * @retval kStatus_Success command transfer success without error
 * @retval kStatus_FLEXSPI_SequenceExecutionTimeout sequence execution timeout
 * @retval kStatus_FLEXSPI_IpCommandSequenceError IP command sequencen error detected
 * @retval kStatus_FLEXSPI_IpCommandGrantTimeout IP command grant timeout detected
 */
status_t FLEXSPI_TransferBlocking(FLEXSPI_Type *base, flexspi_transfer_t *xfer);
/*! @} */

/*!
 * @name Transactional
 * @{
 */

/*!
 * @brief Initializes the FLEXSPI handle which is used in transactional functions.
 *
 * @param base FLEXSPI peripheral base address.
 * @param handle pointer to flexspi_handle_t structure to store the transfer state.
 * @param callback pointer to user callback function.
 * @param userData user parameter passed to the callback function.
 */
void FLEXSPI_TransferCreateHandle(FLEXSPI_Type *base,
                                  flexspi_handle_t *handle,
                                  flexspi_transfer_callback_t callback,
                                  void *userData);

/*!
 * @brief Performs a interrupt non-blocking transfer on the FLEXSPI bus.
 *
 * @note Calling the API returns immediately after transfer initiates. The user needs
 * to call FLEXSPI_GetTransferCount to poll the transfer status to check whether
 * the transfer is finished. If the return status is not kStatus_FLEXSPI_Busy, the transfer
 * is finished. For FLEXSPI_Read, the dataSize should be multiple of rx watermark levle, or
 * FLEXSPI could not read data properly.
 *
 * @param base FLEXSPI peripheral base address.
 * @param handle pointer to flexspi_handle_t structure which stores the transfer state.
 * @param xfer pointer to flexspi_transfer_t structure.
 * @retval kStatus_Success Successfully start the data transmission.
 * @retval kStatus_FLEXSPI_Busy Previous transmission still not finished.
 */
status_t FLEXSPI_TransferNonBlocking(FLEXSPI_Type *base, flexspi_handle_t *handle, flexspi_transfer_t *xfer);

/*!
 * @brief Gets the master transfer status during a interrupt non-blocking transfer.
 *
 * @param base FLEXSPI peripheral base address.
 * @param handle pointer to flexspi_handle_t structure which stores the transfer state.
 * @param count Number of bytes transferred so far by the non-blocking transaction.
 * @retval kStatus_InvalidArgument count is Invalid.
 * @retval kStatus_Success Successfully return the count.
 */
status_t FLEXSPI_TransferGetCount(FLEXSPI_Type *base, flexspi_handle_t *handle, size_t *count);

/*!
 * @brief Aborts an interrupt non-blocking transfer early.
 *
 * @note This API can be called at any time when an interrupt non-blocking transfer initiates
 * to abort the transfer early.
 *
 * @param base FLEXSPI peripheral base address.
 * @param handle pointer to flexspi_handle_t structure which stores the transfer state
 */
void FLEXSPI_TransferAbort(FLEXSPI_Type *base, flexspi_handle_t *handle);

/*!
 * @brief Master interrupt handler.
 *
 * @param base FLEXSPI peripheral base address.
 * @param handle pointer to flexspi_handle_t structure.
 */
void FLEXSPI_TransferHandleIRQ(FLEXSPI_Type *base, flexspi_handle_t *handle);
/*! @} */

#if defined(__cplusplus)
}
#endif /*_cplusplus. */
/*@}*/

#endif /* __FSL_FLEXSPI_H_ */
