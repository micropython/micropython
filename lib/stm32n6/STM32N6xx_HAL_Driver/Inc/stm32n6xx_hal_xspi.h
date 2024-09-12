/**
  ******************************************************************************
  * @file    stm32n6xx_hal_xspi.h
  * @author  MCD Application Team
  * @brief   Header file of XSPI HAL module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32N6xx_HAL_XSPI_H
#define STM32N6xx_HAL_XSPI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

#if defined(XSPI) || defined(XSPI1) || defined(XSPI2) || defined(XSPI3)

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @addtogroup XSPI
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup XSPI_Exported_Types XSPI Exported Types
  * @{
  */

/**
  * @brief XSPI Init structure definition
  */
typedef struct
{
  uint32_t FifoThresholdByte;         /*!< This is the threshold used by the Peripheral to generate the interrupt
                                           indicating that data are available in reception or free place
                                           is available in transmission.
                                           For XSPI, this parameter can be a value between 1 and 64 */
  uint32_t MemoryMode;                /*!< It Specifies the memory mode.
                                           This parameter can be a value of @ref XSPI_MemoryMode */
  uint32_t MemoryType;                /*!< It indicates the external device type connected to the XSPI.
                                           This parameter can be a value of @ref XSPI_MemoryType */
  uint32_t MemorySize;                /*!< It defines the size of the external device connected to the XSPI,
                                           it corresponds to the number of address bits required to access
                                           the external device.
                                           This parameter can be a value of @ref XSPI_MemorySize*/
  uint32_t ChipSelectHighTimeCycle;   /*!< It defines the minimum number of clocks which the chip select
                                           must remain high between commands.
                                           This parameter can be a value between 1 and 64U */
  uint32_t FreeRunningClock;          /*!< It enables or not the free running clock.
                                           This parameter can be a value of @ref XSPI_FreeRunningClock */
  uint32_t ClockMode;                 /*!< It indicates the level of clock when the chip select is released.
                                           This parameter can be a value of @ref XSPI_ClockMode */
  uint32_t WrapSize;                  /*!< It indicates the wrap-size corresponding the external device configuration.
                                           This parameter can be a value of @ref XSPI_WrapSize */
  uint32_t ClockPrescaler;            /*!< It specifies the prescaler factor used for generating
                                           the external clock based on the AHB clock.
                                           This parameter can be a value between 0 and 255U */
  uint32_t SampleShifting;            /*!< It allows to delay to 1/2 cycle the data sampling in order
                                           to take in account external signal delays.
                                           This parameter can be a value of @ref XSPI_SampleShifting */
  uint32_t DelayHoldQuarterCycle;     /*!< It allows to hold to 1/4 cycle the data.
                                           This parameter can be a value of @ref XSPI_DelayHoldQuarterCycle */
  uint32_t ChipSelectBoundary;        /*!< It enables the transaction boundary feature and
                                           defines the boundary of bytes to release the chip select.
                                           This parameter can be a value of @ref XSPI_ChipSelectBoundary  */
  uint32_t MaxTran;                   /*!< It enables the communication regulation feature. The chip select is
                                           released every MaxTran+1 bytes when the other XSPI request the access
                                           to the bus.
                                           This parameter can be a value between 0 and 255U */
  uint32_t Refresh;                   /*!< It enables the refresh rate feature. The chip select is released every
                                           Refresh+1 clock cycles.
                                           This parameter can be a value between 0 and 0xFFFFFFFF */
  uint32_t MemorySelect;              /*!< It indicates if the output of nCS.
                                           This parameter can be a value of @ref XSPI_MemorySelect  */
  uint32_t MemoryExtended;            /*!< If available, It indicates if NCS1 and NCS2 are software or hardware controlled when one
                                           XSPI drives two same size external memories located in contiguous places
                                           in the memory map.
                                           This parameter can be a value of @ref XSPI_MemoryExtended */
} XSPI_InitTypeDef;

/**
  * @brief  HAL XSPI Handle Structure definition
  */
#if defined(USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
typedef struct __XSPI_HandleTypeDef
#else
typedef struct
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
{
  XSPI_TypeDef               *Instance;     /*!< XSPI registers base address                           */
  XSPI_InitTypeDef           Init;          /*!< XSPI initialization parameters                        */
  uint8_t                    *pBuffPtr;     /*!< Address of the XSPI buffer for transfer               */
  __IO uint32_t              XferSize;      /*!< Number of data to transfer                            */
  __IO uint32_t              XferCount;     /*!< Counter of data transferred                           */
  DMA_HandleTypeDef          *hdmatx;       /*!< Handle of the DMA channel used for transmit           */
  DMA_HandleTypeDef          *hdmarx;       /*!< Handle of the DMA channel used for receive            */
  __IO uint32_t              State;         /*!< Internal state of the XSPI HAL driver                 */
  __IO uint32_t              ErrorCode;     /*!< Error code in case of HAL driver internal error       */
  uint32_t                   Timeout;       /*!< Timeout used for the XSPI external device access      */
#if defined(USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
  void (* ErrorCallback)(struct __XSPI_HandleTypeDef *hxspi);
  void (* AbortCpltCallback)(struct __XSPI_HandleTypeDef *hxspi);
  void (* FifoThresholdCallback)(struct __XSPI_HandleTypeDef *hxspi);
  void (* CmdCpltCallback)(struct __XSPI_HandleTypeDef *hxspi);
  void (* RxCpltCallback)(struct __XSPI_HandleTypeDef *hxspi);
  void (* TxCpltCallback)(struct __XSPI_HandleTypeDef *hxspi);
  void (* RxHalfCpltCallback)(struct __XSPI_HandleTypeDef *hxspi);
  void (* TxHalfCpltCallback)(struct __XSPI_HandleTypeDef *hxspi);
  void (* StatusMatchCallback)(struct __XSPI_HandleTypeDef *hxspi);
  void (* TimeOutCallback)(struct __XSPI_HandleTypeDef *hxspi);

  void (* MspInitCallback)(struct __XSPI_HandleTypeDef *hxspi);
  void (* MspDeInitCallback)(struct __XSPI_HandleTypeDef *hxspi);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
} XSPI_HandleTypeDef;

/**
  * @brief  HAL XSPI Regular Command Structure definition
  */
typedef struct
{
  uint32_t OperationType;             /*!< It indicates if the configuration applies to the common registers or
                                           to the registers for the write operation (these registers are only
                                           used for memory-mapped mode).
                                           This parameter can be a value of @ref XSPI_OperationType */
  uint32_t IOSelect;                  /*!< It indicates the IOs used to exchange data with external memory.
                                           This parameter can be a value of @ref XSPI_IOSelect */
  uint32_t Instruction;               /*!< It contains the instruction to be sent to the device.
                                           This parameter can be a value between 0 and 0xFFFFFFFFU */
  uint32_t InstructionMode;           /*!< It indicates the mode of the instruction.
                                           This parameter can be a value of @ref XSPI_InstructionMode */
  uint32_t InstructionWidth;          /*!< It indicates the width of the instruction.
                                           This parameter can be a value of @ref XSPI_InstructionWidth */
  uint32_t InstructionDTRMode;        /*!< It enables or not the DTR mode for the instruction phase.
                                           This parameter can be a value of @ref XSPI_InstructionDTRMode */
  uint32_t Address;                   /*!< It contains the address to be sent to the device.
                                           This parameter can be a value between 0 and 0xFFFFFFFFU */
  uint32_t AddressMode;               /*!< It indicates the address mode. Address mode precises number of lines
                                           for address (except no address).
                                           This parameter can be a value of @ref XSPI_AddressMode */
  uint32_t AddressWidth;              /*!< It indicates the width of the address.
                                           This parameter can be a value of @ref XSPI_AddressWidth */
  uint32_t AddressDTRMode;            /*!< It enables or not the DTR mode for the address phase.
                                           This parameter can be a value of @ref XSPI_AddressDTRMode */
  uint32_t AlternateBytes;            /*!< It contains the alternate bytes to be sent to the device.
                                           This parameter can be a value between 0 and 0xFFFFFFFFU */
  uint32_t AlternateBytesMode;        /*!< It indicates the mode of the alternate bytes.
                                           This parameter can be a value of @ref XSPI_AlternateBytesMode */
  uint32_t AlternateBytesWidth;       /*!< It indicates the width of the alternate bytes.
                                           This parameter can be a value of @ref XSPI_AlternateBytesWidth */
  uint32_t AlternateBytesDTRMode;     /*!< It enables or not the DTR mode for the alternate bytes phase.
                                           This parameter can be a value of @ref XSPI_AlternateBytesDTRMode */
  uint32_t DataMode;                  /*!< It indicates the data mode. Data mode precises number of lines
                                           for data exchange (except no data).
                                           This parameter can be a value of @ref XSPI_DataMode */
  uint32_t DataLength;                /*!< It indicates the number of data transferred with this command.
                                           This field is only used for indirect mode.
                                           This parameter can be a value between 1 and 0xFFFFFFFFU */
  uint32_t DataDTRMode;               /*!< It enables or not the DTR mode for the data phase.
                                           This parameter can be a value of @ref XSPI_DataDTRMode */
  uint32_t DummyCycles;               /*!< It indicates the number of dummy cycles inserted before data phase.
                                           This parameter can be a value between 0 and 31U */
  uint32_t DQSMode;                   /*!< It enables or not the data strobe management.
                                           This parameter can be a value of @ref XSPI_DQSMode */
} XSPI_RegularCmdTypeDef;
/**
  * @brief  HAL XSPI Hyperbus Configuration Structure definition
  */
typedef struct
{
  uint32_t RWRecoveryTimeCycle;       /*!< It indicates the number of cycles for the device read write recovery time.
                                           This parameter can be a value between 0 and 255U */
  uint32_t AccessTimeCycle;           /*!< It indicates the number of cycles for the device access time.
                                           This parameter can be a value between 0 and 255U */
  uint32_t WriteZeroLatency;          /*!< It enables or not the latency for the write access.
                                           This parameter can be a value of @ref XSPI_WriteZeroLatency */
  uint32_t LatencyMode;               /*!< It configures the latency mode.
                                           This parameter can be a value of @ref XSPI_LatencyMode */
} XSPI_HyperbusCfgTypeDef;

/**
  * @brief  HAL XSPI Hyperbus Command Structure definition
  */
typedef struct
{
  uint32_t AddressSpace;              /*!< It indicates the address space accessed by the command.
                                           This parameter can be a value of @ref XSPI_AddressSpace */
  uint32_t Address;                   /*!< It contains the address to be sent to the device.
                                           This parameter can be a value between 0 and 0xFFFFFFFFU */
  uint32_t AddressWidth;              /*!< It indicates the width of the address.
                                           This parameter can be a value of @ref XSPI_AddressWidth */
  uint32_t DataLength;                /*!< It indicates the number of data transferred with this command.
                                           This field is only used for indirect mode.
                                           This parameter can be a value between 1 and 0xFFFFFFFF
                                           In case of autopolling mode, this parameter can be
                                           any value between 1 and 4 */
  uint32_t DQSMode;                   /*!< It enables or not the data strobe management.
                                           This parameter can be a value of @ref XSPI_DQSMode */
  uint32_t DataMode;                  /*!< It indicates the data mode. Data mode precises number of lines
                                           for data exchange (except no data).
                                           This parameter can be a value of @ref XSPI_DataMode */
} XSPI_HyperbusCmdTypeDef;

/**
  * @brief  HAL XSPI Auto Polling mode configuration structure definition
  */
typedef struct
{
  uint32_t MatchValue;                /*!< Specifies the value to be compared with the masked status register to get
                                           a match.
                                           This parameter can be any value between 0 and 0xFFFFFFFFU */
  uint32_t MatchMask;                 /*!< Specifies the mask to be applied to the status bytes received.
                                           This parameter can be any value between 0 and 0xFFFFFFFFU */
  uint32_t MatchMode;                 /*!< Specifies the method used for determining a match.
                                           This parameter can be a value of @ref XSPI_MatchMode */
  uint32_t AutomaticStop;             /*!< Specifies if automatic polling is stopped after a match.
                                           This parameter can be a value of @ref XSPI_AutomaticStop */
  uint32_t IntervalTime;              /*!< Specifies the number of clock cycles between two read during automatic
                                           polling phases.
                                           This parameter can be any value between 0 and 0xFFFFU */
} XSPI_AutoPollingTypeDef;

/**
  * @brief  HAL XSPI Memory Mapped mode configuration structure definition
  */
typedef struct
{
  uint32_t TimeOutActivation;         /*!< Specifies if the timeout counter is enabled to release the chip select.
                                           This parameter can be a value of @ref XSPI_TimeOutActivation */
  uint32_t TimeoutPeriodClock;        /*!< Specifies the number of clock to wait when the FIFO is full before to
                                           release the chip select.
                                           This parameter can be any value between 0 and 0xFFFFU */
  uint32_t NoPrefetchData;            /*!< Specifies if the automatic prefetch in the external memory is enabled or not.
                                           This parameter can be a value of @ref XSPI_NoPrefetchData */
  uint32_t NoPrefetchAXI;             /*!< Specifies if the automatic prefetch in the external memory when the corresponding AXI
                                           transaction is signaled as not-prefetchable, is enabled or not.
                                           This parameter can be a value of @ref XSPI_NoPrefetchAXI */
} XSPI_MemoryMappedTypeDef;

/**
  * @brief HAL XSPI IO Manager Configuration structure definition
  */
typedef struct
{
  uint32_t nCSOverride;               /*!< It indicates Chip select selector override setting for XSPI.
                                           This parameter can be a value @ref XSPIM_MemorySelect_Override */
  uint32_t IOPort;                    /*!< It indicates which port of the XSPI IO Manager is used for the instance.
                                           This parameter can be a value of @ref XSPI_IO_Manger_IOPort */
  uint32_t Req2AckTime;               /*!< It indicates the minimum switching duration (in number of clock cycles)
                                           expected if some signals are multiplexed in the XSPI IO Manager with the
                                           other XSPI.
                                           This parameter can be a value between 1 and 256 */
} XSPIM_CfgTypeDef;

#if defined(USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
/**
  * @brief  HAL XSPI Callback ID enumeration definition
  */
typedef enum
{
  HAL_XSPI_ERROR_CB_ID          = 0x00U,  /*!< XSPI Error Callback ID            */
  HAL_XSPI_ABORT_CB_ID          = 0x01U,  /*!< XSPI Abort Callback ID            */
  HAL_XSPI_FIFO_THRESHOLD_CB_ID = 0x02U,  /*!< XSPI FIFO Threshold Callback ID   */
  HAL_XSPI_CMD_CPLT_CB_ID       = 0x03U,  /*!< XSPI Command Complete Callback ID */
  HAL_XSPI_RX_CPLT_CB_ID        = 0x04U,  /*!< XSPI Rx Complete Callback ID      */
  HAL_XSPI_TX_CPLT_CB_ID        = 0x05U,  /*!< XSPI Tx Complete Callback ID      */
  HAL_XSPI_RX_HALF_CPLT_CB_ID   = 0x06U,  /*!< XSPI Rx Half Complete Callback ID */
  HAL_XSPI_TX_HALF_CPLT_CB_ID   = 0x07U,  /*!< XSPI Tx Half Complete Callback ID */
  HAL_XSPI_STATUS_MATCH_CB_ID   = 0x08U,  /*!< XSPI Status Match Callback ID     */
  HAL_XSPI_TIMEOUT_CB_ID        = 0x09U,  /*!< XSPI Timeout Callback ID          */
  HAL_XSPI_MSP_INIT_CB_ID       = 0x0AU,  /*!< XSPI MspInit Callback ID          */
  HAL_XSPI_MSP_DEINIT_CB_ID     = 0x0BU   /*!< XSPI MspDeInit Callback ID        */
} HAL_XSPI_CallbackIDTypeDef;

/**
  * @brief  HAL XSPI Callback pointer definition
  */
typedef void (*pXSPI_CallbackTypeDef)(XSPI_HandleTypeDef *hxspi);

#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
/**
  * @brief  HAL XSPI High-speed interface calibration structure definition
  */
typedef struct
{
  uint32_t DelayValueType;            /*!< It indicates which calibration is concerned by the configuration.
                                           This parameter can be a value of @ref XSPI_DelayType */
  uint32_t FineCalibrationUnit;       /*!< It indicates the fine calibration value of the delay.
                                           This parameter can be a value between 0 and 0x7FU */
  uint32_t CoarseCalibrationUnit;     /*!< It indicates the coarse calibration value of the delay.
                                           This parameter can be a value between 0 and 0x1FU */
  uint32_t MaxCalibration;            /*!< It indicates that the calibration is outside the range of DLL master.
                                           It applies only when the DelayValueType is HAL_XSPI_CAL_FULL_CYCLE_DELAY.
                                           This parameter can be a value of @ref XSPI_MaxCal */
} XSPI_HSCalTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup XSPI_Exported_Constants XSPI Exported Constants
  * @{
  */

/** @defgroup XSPI_State XSPI State
  * @{
  */
#define HAL_XSPI_STATE_RESET                 (0x00000000U)  /*!< Initial state                                                          */
#define HAL_XSPI_STATE_READY                 (0x00000002U)  /*!< Driver ready to be used                                                */
#define HAL_XSPI_STATE_HYPERBUS_INIT         (0x00000001U)  /*!< Initialization done in hyperbus mode but timing configuration not done */
#define HAL_XSPI_STATE_CMD_CFG               (0x00000004U)  /*!< Command (regular or hyperbus) configured, ready for an action          */
#define HAL_XSPI_STATE_READ_CMD_CFG          (0x00000014U)  /*!< Read command configuration done, not the write command configuration   */
#define HAL_XSPI_STATE_WRITE_CMD_CFG         (0x00000024U)  /*!< Write command configuration done, not the read command configuration   */
#define HAL_XSPI_STATE_BUSY_CMD              (0x00000008U)  /*!< Command without data on-going                                          */
#define HAL_XSPI_STATE_BUSY_TX               (0x00000018U)  /*!< Indirect Tx on-going                                                   */
#define HAL_XSPI_STATE_BUSY_RX               (0x00000028U)  /*!< Indirect Rx on-going                                                   */
#define HAL_XSPI_STATE_BUSY_AUTO_POLLING     (0x00000048U)  /*!< Auto-polling on-going                                                  */
#define HAL_XSPI_STATE_BUSY_MEM_MAPPED       (0x00000088U)  /*!< Memory-mapped on-going                                                 */
#define HAL_XSPI_STATE_ABORT                 (0x00000100U)  /*!< Abort on-going                                                         */
#define HAL_XSPI_STATE_ERROR                 (0x00000200U)  /*!< Blocking error, driver should be re-initialized                        */
/**
  * @}
  */

/** @defgroup XSPI_ErrorCode XSPI Error Code
  * @{
  */
#define HAL_XSPI_ERROR_NONE                  (0x00000000U)  /*!< No error                         */
#define HAL_XSPI_ERROR_TIMEOUT               (0x00000001U)  /*!< Timeout error                    */
#define HAL_XSPI_ERROR_TRANSFER              (0x00000002U)  /*!< Transfer error                   */
#define HAL_XSPI_ERROR_DMA                   (0x00000004U)  /*!< DMA transfer error               */
#define HAL_XSPI_ERROR_INVALID_PARAM         (0x00000008U)  /*!< Invalid parameters error         */
#define HAL_XSPI_ERROR_INVALID_SEQUENCE      (0x00000010U)  /*!< Sequence is incorrect            */
#if defined(USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
#define HAL_XSPI_ERROR_INVALID_CALLBACK      (0x00000020U)  /*!< Invalid callback error           */
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
/**
  * @}
  */

/** @defgroup XSPI_MemoryMode XSPI Memory Mode
  * @{
  */
#define HAL_XSPI_SINGLE_MEM                  (0x00000000U)  /*!< Dual-memory mode disabled        */
#define HAL_XSPI_DUAL_MEM                    (XSPI_CR_DMM)  /*!< Dual mode enabled                */

/**
  * @}
  */

/** @defgroup XSPI_MemoryType XSPI Memory Type
  * @{
  */
#define HAL_XSPI_MEMTYPE_MICRON              (0x00000000U)                           /*!< Micron mode      */
#define HAL_XSPI_MEMTYPE_MACRONIX            (XSPI_DCR1_MTYP_0)                      /*!< Macronix mode    */
#define HAL_XSPI_MEMTYPE_APMEM               (XSPI_DCR1_MTYP_1)                      /*!< AP Memory mode   */
#define HAL_XSPI_MEMTYPE_MACRONIX_RAM        ((XSPI_DCR1_MTYP_1 | XSPI_DCR1_MTYP_0)) /*!< Macronix RAM mode*/
#define HAL_XSPI_MEMTYPE_HYPERBUS            (XSPI_DCR1_MTYP_2)                      /*!< Hyperbus mode    */
#define HAL_XSPI_MEMTYPE_APMEM_16BITS        ((XSPI_DCR1_MTYP_2 | XSPI_DCR1_MTYP_1)) /*!< AP Memory mode   */

/**
  * @}
  */

/** @defgroup XSPI_MemorySize XSPI Memory Size
  * @{
  */
#define HAL_XSPI_SIZE_16B                    (0x00000000U)  /*!<  16 bits  (  2  Bytes = 2^( 0+1)) */
#define HAL_XSPI_SIZE_32B                    (0x00000001U)  /*!<  32 bits  (  4  Bytes = 2^( 1+1)) */
#define HAL_XSPI_SIZE_64B                    (0x00000002U)  /*!<  64 bits  (  8  Bytes = 2^( 2+1)) */
#define HAL_XSPI_SIZE_128B                   (0x00000003U)  /*!< 128 bits  ( 16  Bytes = 2^( 3+1)) */
#define HAL_XSPI_SIZE_256B                   (0x00000004U)  /*!< 256 bits  ( 32  Bytes = 2^( 4+1)) */
#define HAL_XSPI_SIZE_512B                   (0x00000005U)  /*!< 512 bits  ( 64  Bytes = 2^( 5+1)) */
#define HAL_XSPI_SIZE_1KB                    (0x00000006U)  /*!<   1 Kbits (128  Bytes = 2^( 6+1)) */
#define HAL_XSPI_SIZE_2KB                    (0x00000007U)  /*!<   2 Kbits (256  Bytes = 2^( 7+1)) */
#define HAL_XSPI_SIZE_4KB                    (0x00000008U)  /*!<   4 Kbits (512  Bytes = 2^( 8+1)) */
#define HAL_XSPI_SIZE_8KB                    (0x00000009U)  /*!<   8 Kbits (  1 KBytes = 2^( 9+1)) */
#define HAL_XSPI_SIZE_16KB                   (0x0000000AU)  /*!<  16 Kbits (  2 KBytes = 2^(10+1)) */
#define HAL_XSPI_SIZE_32KB                   (0x0000000BU)  /*!<  32 Kbits (  4 KBytes = 2^(11+1)) */
#define HAL_XSPI_SIZE_64KB                   (0x0000000CU)  /*!<  64 Kbits (  8 KBytes = 2^(12+1)) */
#define HAL_XSPI_SIZE_128KB                  (0x0000000DU)  /*!< 128 Kbits ( 16 KBytes = 2^(13+1)) */
#define HAL_XSPI_SIZE_256KB                  (0x0000000EU)  /*!< 256 Kbits ( 32 KBytes = 2^(14+1)) */
#define HAL_XSPI_SIZE_512KB                  (0x0000000FU)  /*!< 512 Kbits ( 64 KBytes = 2^(15+1)) */
#define HAL_XSPI_SIZE_1MB                    (0x00000010U)  /*!<   1 Mbits (128 KBytes = 2^(16+1)) */
#define HAL_XSPI_SIZE_2MB                    (0x00000011U)  /*!<   2 Mbits (256 KBytes = 2^(17+1)) */
#define HAL_XSPI_SIZE_4MB                    (0x00000012U)  /*!<   4 Mbits (512 KBytes = 2^(18+1)) */
#define HAL_XSPI_SIZE_8MB                    (0x00000013U)  /*!<   8 Mbits (  1 MBytes = 2^(19+1)) */
#define HAL_XSPI_SIZE_16MB                   (0x00000014U)  /*!<  16 Mbits (  2 MBytes = 2^(20+1)) */
#define HAL_XSPI_SIZE_32MB                   (0x00000015U)  /*!<  32 Mbits (  4 MBytes = 2^(21+1)) */
#define HAL_XSPI_SIZE_64MB                   (0x00000016U)  /*!<  64 Mbits (  8 MBytes = 2^(22+1)) */
#define HAL_XSPI_SIZE_128MB                  (0x00000017U)  /*!< 128 Mbits ( 16 MBytes = 2^(23+1)) */
#define HAL_XSPI_SIZE_256MB                  (0x00000018U)  /*!< 256 Mbits ( 32 MBytes = 2^(24+1)) */
#define HAL_XSPI_SIZE_512MB                  (0x00000019U)  /*!< 512 Mbits ( 64 MBytes = 2^(25+1)) */
#define HAL_XSPI_SIZE_1GB                    (0x0000001AU)  /*!<   1 Gbits (128 MBytes = 2^(26+1)) */
#define HAL_XSPI_SIZE_2GB                    (0x0000001BU)  /*!<   2 Gbits (256 MBytes = 2^(27+1)) */
#define HAL_XSPI_SIZE_4GB                    (0x0000001CU)  /*!<   4 Gbits (512 MBytes = 2^(28+1)) */
#define HAL_XSPI_SIZE_8GB                    (0x0000001DU)  /*!<   8 Gbits (  1 GBytes = 2^(29+1)) */
#define HAL_XSPI_SIZE_16GB                   (0x0000001EU)  /*!<  16 Gbits (  2 GBytes = 2^(30+1)) */
#define HAL_XSPI_SIZE_32GB                   (0x0000001FU)  /*!<  32 Gbits (  4 GBytes = 2^(31+1)) */
/**
  * @}
  */

/** @defgroup XSPI_FreeRunningClock XSPI Free Running Clock
  * @{
  */
#define HAL_XSPI_FREERUNCLK_DISABLE          (0x00000000U)               /*!< CLK is not free running            */
#define HAL_XSPI_FREERUNCLK_ENABLE           ((uint32_t)XSPI_DCR1_FRCK)  /*!< CLK is always provided (running)   */
/**
  * @}
  */

/** @defgroup XSPI_ClockMode XSPI Clock Mode
  * @{
  */
#define HAL_XSPI_CLOCK_MODE_0                (0x00000000U)                 /*!< CLK must stay low while nCS is high  */
#define HAL_XSPI_CLOCK_MODE_3                ((uint32_t)XSPI_DCR1_CKMODE)  /*!< CLK must stay high while nCS is high */
/**
  * @}
  */

/** @defgroup XSPI_WrapSize XSPI Wrap-Size
  * @{
  */
#define HAL_XSPI_WRAP_NOT_SUPPORTED          (0x00000000U)                                             /*!< wrapped reads are not supported by the memory   */
#define HAL_XSPI_WRAP_16_BYTES               ((uint32_t)XSPI_DCR2_WRAPSIZE_1)                          /*!< external memory supports wrap size of 16 bytes  */
#define HAL_XSPI_WRAP_32_BYTES               ((uint32_t)(XSPI_DCR2_WRAPSIZE_0 | XSPI_DCR2_WRAPSIZE_1)) /*!< external memory supports wrap size of 32 bytes  */
#define HAL_XSPI_WRAP_64_BYTES               ((uint32_t)XSPI_DCR2_WRAPSIZE_2)                          /*!< external memory supports wrap size of 64 bytes  */
#define HAL_XSPI_WRAP_128_BYTES              ((uint32_t)(XSPI_DCR2_WRAPSIZE_0 | XSPI_DCR2_WRAPSIZE_2)) /*!< external memory supports wrap size of 128 bytes */
/**
  * @}
  */

/** @defgroup XSPI_SampleShifting XSPI Sample Shifting
  * @{
  */
#define HAL_XSPI_SAMPLE_SHIFT_NONE           (0x00000000U)                /*!< No shift                         */
#define HAL_XSPI_SAMPLE_SHIFT_HALFCYCLE      ((uint32_t)XSPI_TCR_SSHIFT)  /*!< 1/2 cycle shift                  */
/**
  * @}
  */

/** @defgroup XSPI_DelayHoldQuarterCycle XSPI Delay Hold Quarter Cycle
  * @{
  */
#define HAL_XSPI_DHQC_DISABLE                (0x00000000U)              /*!< No Delay                         */
#define HAL_XSPI_DHQC_ENABLE                 ((uint32_t)XSPI_TCR_DHQC)  /*!< Delay Hold 1/4 cycle             */
/**
  * @}
  */

/** @defgroup XSPI_ChipSelectBoundary XSPI Chip Select Boundary
  * @{
  */
#define HAL_XSPI_BONDARYOF_NONE              (0x00000000U)  /*!     CS boundary disabled         */
#define HAL_XSPI_BONDARYOF_16B               (0x00000001U)  /*!<  16 bits  (  2  Bytes = 2^(1))  */
#define HAL_XSPI_BONDARYOF_32B               (0x00000002U)  /*!<  32 bits  (  4  Bytes = 2^(2))  */
#define HAL_XSPI_BONDARYOF_64B               (0x00000003U)  /*!<  64 bits  (  8  Bytes = 2^(3))  */
#define HAL_XSPI_BONDARYOF_128B              (0x00000004U)  /*!< 128 bits  ( 16  Bytes = 2^(4))  */
#define HAL_XSPI_BONDARYOF_256B              (0x00000005U)  /*!< 256 bits  ( 32  Bytes = 2^(5))  */
#define HAL_XSPI_BONDARYOF_512B              (0x00000006U)  /*!< 512 bits  ( 64  Bytes = 2^(6))  */
#define HAL_XSPI_BONDARYOF_1KB               (0x00000007U)  /*!<   1 Kbits (128  Bytes = 2^(7))  */
#define HAL_XSPI_BONDARYOF_2KB               (0x00000008U)  /*!<   2 Kbits (256  Bytes = 2^(8))  */
#define HAL_XSPI_BONDARYOF_4KB               (0x00000009U)  /*!<   4 Kbits (512  Bytes = 2^(9))  */
#define HAL_XSPI_BONDARYOF_8KB               (0x0000000AU)  /*!<   8 Kbits (  1 KBytes = 2^(10)) */
#define HAL_XSPI_BONDARYOF_16KB              (0x0000000BU)  /*!<  16 Kbits (  2 KBytes = 2^(11)) */
#define HAL_XSPI_BONDARYOF_32KB              (0x0000000CU)  /*!<  32 Kbits (  4 KBytes = 2^(12)) */
#define HAL_XSPI_BONDARYOF_64KB              (0x0000000DU)  /*!<  64 Kbits (  8 KBytes = 2^(13)) */
#define HAL_XSPI_BONDARYOF_128KB             (0x0000000EU)  /*!< 128 Kbits ( 16 KBytes = 2^(14)) */
#define HAL_XSPI_BONDARYOF_256KB             (0x0000000FU)  /*!< 256 Kbits ( 32 KBytes = 2^(15)) */
#define HAL_XSPI_BONDARYOF_512KB             (0x00000010U)  /*!< 512 Kbits ( 64 KBytes = 2^(16)) */
#define HAL_XSPI_BONDARYOF_1MB               (0x00000011U)  /*!<   1 Mbits (128 KBytes = 2^(17)) */
#define HAL_XSPI_BONDARYOF_2MB               (0x00000012U)  /*!<   2 Mbits (256 KBytes = 2^(18)) */
#define HAL_XSPI_BONDARYOF_4MB               (0x00000013U)  /*!<   4 Mbits (512 KBytes = 2^(19)) */
#define HAL_XSPI_BONDARYOF_8MB               (0x00000014U)  /*!<   8 Mbits (  1 MBytes = 2^(20)) */
#define HAL_XSPI_BONDARYOF_16MB              (0x00000015U)  /*!<  16 Mbits (  2 MBytes = 2^(21)) */
#define HAL_XSPI_BONDARYOF_32MB              (0x00000016U)  /*!<  32 Mbits (  4 MBytes = 2^(22)) */
#define HAL_XSPI_BONDARYOF_64MB              (0x00000017U)  /*!<  64 Mbits (  8 MBytes = 2^(23)) */
#define HAL_XSPI_BONDARYOF_128MB             (0x00000018U)  /*!< 128 Mbits ( 16 MBytes = 2^(24)) */
#define HAL_XSPI_BONDARYOF_256MB             (0x00000019U)  /*!< 256 Mbits ( 32 MBytes = 2^(25)) */
#define HAL_XSPI_BONDARYOF_512MB             (0x0000001AU)  /*!< 512 Mbits ( 64 MBytes = 2^(26)) */
#define HAL_XSPI_BONDARYOF_1GB               (0x0000001BU)  /*!<   1 Gbits (128 MBytes = 2^(27)) */
#define HAL_XSPI_BONDARYOF_2GB               (0x0000001CU)  /*!<   2 Gbits (256 MBytes = 2^(28)) */
#define HAL_XSPI_BONDARYOF_4GB               (0x0000001DU)  /*!<   4 Gbits (512 MBytes = 2^(29)) */
#define HAL_XSPI_BONDARYOF_8GB               (0x0000001EU)  /*!<   8 Gbits (  1 GBytes = 2^(30)) */
#define HAL_XSPI_BONDARYOF_16GB              (0x0000001FU)  /*!<  16 Gbits (  2 GBytes = 2^(31)) */
/**
  * @}
  */

/** @defgroup XSPI_MemorySelect XSPI Memory Select
  * @{
  */
#define HAL_XSPI_CSSEL_NCS1                  (0x00000000U)             /*!<  The output of nCS is nCS1       */
#define HAL_XSPI_CSSEL_NCS2                  ((uint32_t)XSPI_CR_CSSEL) /*!<  The output of nCS is nCS2       */
/**
  * @}
  */

/** @defgroup XSPI_MemoryExtended XSPI Memory Extended
  * @{
  */
#define HAL_XSPI_CSSEL_SW                  (0x00000000U)             /*!<  NCS1 and NCS2 are software controlled.      */
#define HAL_XSPI_CSSEL_HW                  ((uint32_t)XSPI_DCR1_EXTENDMEM) /*!<  NCS1 and NCS2 are hardware controlled.      */
/**
  * @}
  */

/** @defgroup XSPI_OperationType XSPI Operation Type
  * @{
  */
#define HAL_XSPI_OPTYPE_COMMON_CFG           (0x00000000U)  /*!< Common configuration (indirect or auto-polling mode) */
#define HAL_XSPI_OPTYPE_READ_CFG             (0x00000001U)  /*!< Read configuration (memory-mapped mode)              */
#define HAL_XSPI_OPTYPE_WRITE_CFG            (0x00000002U)  /*!< Write configuration (memory-mapped mode)             */
#define HAL_XSPI_OPTYPE_WRAP_CFG             (0x00000003U)  /*!< Wrap configuration (memory-mapped mode)              */

/**
  * @}
  */

/** @defgroup XSPI_IOSelect XSPI IO Select
  * @{
  */
#define HAL_XSPI_SELECT_IO_3_0               (0x00000000U)                                /*!< Data exchanged over IO[3:0]   */
#define HAL_XSPI_SELECT_IO_7_4               ((uint32_t)XSPI_CR_MSEL_0)                               /*!< Data exchanged over IO[7:4]   */
#define HAL_XSPI_SELECT_IO_11_8              ((uint32_t)XSPI_CR_MSEL_1)                               /*!< Data exchanged over IO[11:8]  */
#define HAL_XSPI_SELECT_IO_15_12             ((uint32_t)XSPI_CR_MSEL )                                /*!< Data exchanged over IO[15:12] */
#define HAL_XSPI_SELECT_IO_7_0               (0x00000000U)                                                            /*!< Data exchanged over IO[7:0]   */
#define HAL_XSPI_SELECT_IO_15_8              ((uint32_t)XSPI_CR_MSEL_1)                               /*!< Data exchanged over IO[15:8]  */
/**
  * @}
  */

/** @defgroup XSPI_InstructionMode XSPI Instruction Mode
  * @{
  */
#define HAL_XSPI_INSTRUCTION_NONE            (0x00000000U)                                     /*!< No instruction               */
#define HAL_XSPI_INSTRUCTION_1_LINE          ((uint32_t)XSPI_CCR_IMODE_0)                      /*!< Instruction on a single line */
#define HAL_XSPI_INSTRUCTION_2_LINES         ((uint32_t)XSPI_CCR_IMODE_1)                      /*!< Instruction on two lines     */
#define HAL_XSPI_INSTRUCTION_4_LINES         ((uint32_t)(XSPI_CCR_IMODE_0 | XSPI_CCR_IMODE_1)) /*!< Instruction on four lines    */
#define HAL_XSPI_INSTRUCTION_8_LINES         ((uint32_t)XSPI_CCR_IMODE_2)                      /*!< Instruction on eight lines   */
/**
  * @}
  */

/** @defgroup XSPI_InstructionWidth XSPI Instruction Width
  * @{
  */
#define HAL_XSPI_INSTRUCTION_8_BITS          (0x00000000U)                 /*!< 8-bit instruction  */
#define HAL_XSPI_INSTRUCTION_16_BITS         ((uint32_t)XSPI_CCR_ISIZE_0)  /*!< 16-bit instruction */
#define HAL_XSPI_INSTRUCTION_24_BITS         ((uint32_t)XSPI_CCR_ISIZE_1)  /*!< 24-bit instruction */
#define HAL_XSPI_INSTRUCTION_32_BITS         ((uint32_t)XSPI_CCR_ISIZE)    /*!< 32-bit instruction */
/**
  * @}
  */

/** @defgroup XSPI_InstructionDTRMode XSPI Instruction DTR Mode
  * @{
  */
#define HAL_XSPI_INSTRUCTION_DTR_DISABLE     (0x00000000U)              /*!< DTR mode disabled for instruction phase */
#define HAL_XSPI_INSTRUCTION_DTR_ENABLE      ((uint32_t)XSPI_CCR_IDTR)  /*!< DTR mode enabled for instruction phase  */
/**
  * @}
  */

/** @defgroup XSPI_AddressMode XSPI Address Mode
  * @{
  */
#define HAL_XSPI_ADDRESS_NONE                (0x00000000U)                                        /*!< No address               */
#define HAL_XSPI_ADDRESS_1_LINE              ((uint32_t)XSPI_CCR_ADMODE_0)                        /*!< Address on a single line */
#define HAL_XSPI_ADDRESS_2_LINES             ((uint32_t)XSPI_CCR_ADMODE_1)                        /*!< Address on two lines     */
#define HAL_XSPI_ADDRESS_4_LINES             ((uint32_t)(XSPI_CCR_ADMODE_0 | XSPI_CCR_ADMODE_1))  /*!< Address on four lines    */
#define HAL_XSPI_ADDRESS_8_LINES             ((uint32_t)XSPI_CCR_ADMODE_2)                        /*!< Address on eight lines   */
/**
  * @}
  */

/** @defgroup XSPI_AddressWidth XSPI Address width
  * @{
  */
#define HAL_XSPI_ADDRESS_8_BITS              (0x00000000U)                  /*!< 8-bit address  */
#define HAL_XSPI_ADDRESS_16_BITS             ((uint32_t)XSPI_CCR_ADSIZE_0)  /*!< 16-bit address */
#define HAL_XSPI_ADDRESS_24_BITS             ((uint32_t)XSPI_CCR_ADSIZE_1)  /*!< 24-bit address */
#define HAL_XSPI_ADDRESS_32_BITS             ((uint32_t)XSPI_CCR_ADSIZE)    /*!< 32-bit address */
/**
  * @}
  */

/** @defgroup XSPI_AddressDTRMode XSPI Address DTR Mode
  * @{
  */
#define HAL_XSPI_ADDRESS_DTR_DISABLE         (0x00000000U)               /*!< DTR mode disabled for address phase */
#define HAL_XSPI_ADDRESS_DTR_ENABLE          ((uint32_t)XSPI_CCR_ADDTR)  /*!< DTR mode enabled for address phase  */
/**
  * @}
  */

/** @defgroup XSPI_AlternateBytesMode XSPI Alternate Bytes Mode
  * @{
  */
#define HAL_XSPI_ALT_BYTES_NONE              (0x00000000U)                                        /*!< No alternate bytes               */
#define HAL_XSPI_ALT_BYTES_1_LINE            ((uint32_t)XSPI_CCR_ABMODE_0)                        /*!< Alternate bytes on a single line */
#define HAL_XSPI_ALT_BYTES_2_LINES           ((uint32_t)XSPI_CCR_ABMODE_1)                        /*!< Alternate bytes on two lines     */
#define HAL_XSPI_ALT_BYTES_4_LINES           ((uint32_t)(XSPI_CCR_ABMODE_0 | XSPI_CCR_ABMODE_1))  /*!< Alternate bytes on four lines    */
#define HAL_XSPI_ALT_BYTES_8_LINES           ((uint32_t)XSPI_CCR_ABMODE_2)                        /*!< Alternate bytes on eight lines   */
/**
  * @}
  */

/** @defgroup XSPI_AlternateBytesWidth XSPI Alternate Bytes Width
  * @{
  */
#define HAL_XSPI_ALT_BYTES_8_BITS            (0x00000000U)                  /*!< 8-bit alternate bytes  */
#define HAL_XSPI_ALT_BYTES_16_BITS           ((uint32_t)XSPI_CCR_ABSIZE_0)  /*!< 16-bit alternate bytes */
#define HAL_XSPI_ALT_BYTES_24_BITS           ((uint32_t)XSPI_CCR_ABSIZE_1)  /*!< 24-bit alternate bytes */
#define HAL_XSPI_ALT_BYTES_32_BITS           ((uint32_t)XSPI_CCR_ABSIZE)    /*!< 32-bit alternate bytes */
/**
  * @}
  */

/** @defgroup XSPI_AlternateBytesDTRMode XSPI Alternate Bytes DTR Mode
  * @{
  */
#define HAL_XSPI_ALT_BYTES_DTR_DISABLE       (0x00000000U)               /*!< DTR mode disabled for alternate bytes phase */
#define HAL_XSPI_ALT_BYTES_DTR_ENABLE        ((uint32_t)XSPI_CCR_ABDTR)  /*!< DTR mode enabled for alternate bytes phase  */
/**
  * @}
  */

/** @defgroup XSPI_DataMode XSPI Data Mode
  * @{
  */
#define HAL_XSPI_DATA_NONE                   (0x00000000U)                                      /*!< No data                                   */
#define HAL_XSPI_DATA_1_LINE                 ((uint32_t)XSPI_CCR_DMODE_0)                       /*!< Data on a single line                     */
#define HAL_XSPI_DATA_2_LINES                ((uint32_t)XSPI_CCR_DMODE_1)                       /*!< Data on two lines                         */
#define HAL_XSPI_DATA_4_LINES                ((uint32_t)(XSPI_CCR_DMODE_0 | XSPI_CCR_DMODE_1))  /*!< Data on four lines                        */
#define HAL_XSPI_DATA_8_LINES                ((uint32_t)XSPI_CCR_DMODE_2)                       /*!< Data on eight lines                       */
#define HAL_XSPI_DATA_16_LINES               ((uint32_t)(XSPI_CCR_DMODE_0 | XSPI_CCR_DMODE_2))  /*!< Data on sixteen lines valid for HSPI only */
/**
  * @}
  */

/** @defgroup XSPI_DataDTRMode XSPI Data DTR Mode
  * @{
  */
#define HAL_XSPI_DATA_DTR_DISABLE            (0x00000000U)              /*!< DTR mode disabled for data phase */
#define HAL_XSPI_DATA_DTR_ENABLE             ((uint32_t)XSPI_CCR_DDTR)  /*!< DTR mode enabled for data phase  */
/**
  * @}
  */

/** @defgroup XSPI_DQSMode XSPI DQS Mode
  * @{
  */
#define HAL_XSPI_DQS_DISABLE                 (0x00000000U)              /*!< DQS disabled */
#define HAL_XSPI_DQS_ENABLE                  ((uint32_t)XSPI_CCR_DQSE)  /*!< DQS enabled  */
/**
  * @}
  */

/** @defgroup XSPI_WriteZeroLatency XSPI Hyperbus Write Zero Latency Activation
  * @{
  */
#define HAL_XSPI_LATENCY_ON_WRITE            (0x00000000U)              /*!< Latency on write accesses    */
#define HAL_XSPI_NO_LATENCY_ON_WRITE         ((uint32_t)XSPI_HLCR_WZL)  /*!< No latency on write accesses */
/**
  * @}
  */

/** @defgroup XSPI_LatencyMode XSPI Hyperbus Latency Mode
  * @{
  */
#define HAL_XSPI_VARIABLE_LATENCY            (0x00000000U)             /*!< Variable initial latency */
#define HAL_XSPI_FIXED_LATENCY               ((uint32_t)XSPI_HLCR_LM)  /*!< Fixed latency            */
/**
  * @}
  */

/** @defgroup XSPI_AddressSpace XSPI Hyperbus Address Space
  * @{
  */
#define HAL_XSPI_MEMORY_ADDRESS_SPACE        (0x00000000U)                 /*!< HyperBus memory mode   */
#define HAL_XSPI_REGISTER_ADDRESS_SPACE      ((uint32_t)XSPI_DCR1_MTYP_0)  /*!< HyperBus register mode */
/**
  * @}
  */

/** @defgroup XSPI_MatchMode XSPI Match Mode
  * @{
  */
#define HAL_XSPI_MATCH_MODE_AND              (0x00000000U)            /*!< AND match mode between unmasked bits */
#define HAL_XSPI_MATCH_MODE_OR               ((uint32_t)XSPI_CR_PMM)  /*!< OR match mode between unmasked bits  */
/**
  * @}
  */

/** @defgroup XSPI_AutomaticStop XSPI Automatic Stop
  * @{
  */
#define HAL_XSPI_AUTOMATIC_STOP_DISABLE      (0x00000000U)             /*!< AutoPolling stops only with abort or XSPI disabling */
#define HAL_XSPI_AUTOMATIC_STOP_ENABLE       ((uint32_t)XSPI_CR_APMS)  /*!< AutoPolling stops as soon as there is a match       */
/**
  * @}
  */

/** @defgroup XSPI_TimeOutActivation XSPI Timeout Activation
  * @{
  */
#define HAL_XSPI_TIMEOUT_COUNTER_DISABLE     (0x00000000U)             /*!< Timeout counter disabled, nCS remains active               */
#define HAL_XSPI_TIMEOUT_COUNTER_ENABLE      ((uint32_t)XSPI_CR_TCEN)  /*!< Timeout counter enabled, nCS released when timeout expires */
/**
  * @}
  */

/** @defgroup XSPI_NoPrefetchAXI XSPI No Prefetch AXI
  * @{
  */
#define HAL_XSPI_AXI_PREFETCH_ENABLE     (0x00000000U)             /*!< Prefetch is enabled for AXI signaled transactions               */
#define HAL_XSPI_AXI_PREFETCH_DISABLE    ((uint32_t)XSPI_CR_NOPREF_AXI)  /*!< Prefetch is disable for AXI signaled transactions */
/**
  * @}
  */

/** @defgroup XSPI_NoPrefetchData XSPI No Prefetch Data
  * @{
  */
#define HAL_XSPI_AUTOMATIC_PREFETCH_ENABLE     (0x00000000U)           /*!< Automatic prefetch enabled                               */
#define HAL_XSPI_AUTOMATIC_PREFETCH_DISABLE    ((uint32_t)XSPI_CR_NOPREF)  /*!< Automatic prefetch disabled             */
/**
  * @}
  */

/** @defgroup XSPI_Flags XSPI Flags
  * @{
  */
#define HAL_XSPI_FLAG_BUSY                   XSPI_SR_BUSY  /*!< Busy flag: operation is ongoing                                                                          */
#define HAL_XSPI_FLAG_TO                     XSPI_SR_TOF   /*!< Timeout flag: timeout occurs in memory-mapped mode                                                       */
#define HAL_XSPI_FLAG_SM                     XSPI_SR_SMF   /*!< Status match flag: received data matches in autopolling mode                                             */
#define HAL_XSPI_FLAG_FT                     XSPI_SR_FTF   /*!< Fifo threshold flag: Fifo threshold reached or data left after read from memory is complete              */
#define HAL_XSPI_FLAG_TC                     XSPI_SR_TCF   /*!< Transfer complete flag: programmed number of data have been transferred or the transfer has been aborted */
#define HAL_XSPI_FLAG_TE                     XSPI_SR_TEF   /*!< Transfer error flag: invalid address is being accessed                                                   */
/**
  * @}
  */

/** @defgroup XSPI_Interrupts XSPI Interrupts
  * @{
  */
#define HAL_XSPI_IT_TO                       XSPI_CR_TOIE  /*!< Interrupt on the timeout flag           */
#define HAL_XSPI_IT_SM                       XSPI_CR_SMIE  /*!< Interrupt on the status match flag      */
#define HAL_XSPI_IT_FT                       XSPI_CR_FTIE  /*!< Interrupt on the fifo threshold flag    */
#define HAL_XSPI_IT_TC                       XSPI_CR_TCIE  /*!< Interrupt on the transfer complete flag */
#define HAL_XSPI_IT_TE                       XSPI_CR_TEIE  /*!< Interrupt on the transfer error flag    */
/**
  * @}
  */

/** @defgroup XSPI_Timeout_definition XSPI Timeout definition
  * @{
  */
#define HAL_XSPI_TIMEOUT_DEFAULT_VALUE       (5000U)  /* 5 s */
/**
  * @}
  */

/** @defgroup XSPI_IO_Manger_IOPort XSPI IO Port
  * @{
  */
#define HAL_XSPIM_IOPORT_1                 (0x00000000U)   /*!< Port 1 */
#define HAL_XSPIM_IOPORT_2                 (0x00000001U)   /*!< Port 2 */
/**
  * @}
  */


/** @defgroup XSPI_DelayType XSPI Calibration Delay Type
  * @{
  */
#define HAL_XSPI_CAL_FULL_CYCLE_DELAY      (0x00000000U)  /*!< Delay value equivalent to full memory-clock cycle                */
#define HAL_XSPI_CAL_FEEDBACK_CLK_DELAY    (0x00000001U)  /*!< Delay value for the feedback clock when reading without DQS      */
#define HAL_XSPI_CAL_DATA_OUTPUT_DELAY     (0x00000002U)  /*!< Delay value for output data in DDR mode for write operations     */
#define HAL_XSPI_CAL_DQS_INPUT_DELAY       (0x00000003U)  /*!< Delay value for DQS input when sampling data for read operations */
/**
  * @}
  */

/** @defgroup XSPIM_MemorySelect_Override XSPIM Memory Select Override
  * @{
  */
#define HAL_XSPI_CSSEL_OVR_DISABLED         (0x00000000U)
#define HAL_XSPI_CSSEL_OVR_NCS1             (0x00000010U)             /*!< The chip select signal from XSPI is sent to NCS1 */
#define HAL_XSPI_CSSEL_OVR_NCS2             (0x00000070U)             /*!< The chip select signal from XSPI is sent to NCS2 */
/**
  * @}
  */

/** @defgroup XSPI_MaxCal XSPI Calibration Maximal Value
  * @{
  */
#define HAL_XSPI_MAXCAL_NOT_REACHED        (0x00000000U)                   /*!< Memory-clock perido inside the range of DLL master                          */
#define HAL_XSPI_MAXCAL_REACHED            ((uint32_t)XSPI_CALFCR_CALMAX)  /*!< Memory-clock period outside the range of DLL master (max delay values used) */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/** @defgroup XSPI_Exported_Macros XSPI Exported Macros
  * @{
  */
/** @brief Reset XSPI handle state.
  * @param  __HANDLE__ specifies the XSPI Handle.
  * @retval None
  */
#if defined(USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
#define HAL_XSPI_RESET_HANDLE_STATE(__HANDLE__)           do {                                              \
                                                                  (__HANDLE__)->State = HAL_XSPI_STATE_RESET; \
                                                                  (__HANDLE__)->MspInitCallback = NULL;       \
                                                                  (__HANDLE__)->MspDeInitCallback = NULL;     \
                                                               } while(0)
#else
#define HAL_XSPI_RESET_HANDLE_STATE(__HANDLE__)           ((__HANDLE__)->State = HAL_XSPI_STATE_RESET)
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */

/** @brief  Enable the XSPI peripheral.
  * @param  __HANDLE__ specifies the XSPI Handle.
  * @retval None
  */
#define HAL_XSPI_ENABLE(__HANDLE__)                       SET_BIT((__HANDLE__)->Instance->CR, XSPI_CR_EN)

/** @brief  Disable the XSPI peripheral.
  * @param  __HANDLE__ specifies the XSPI Handle.
  * @retval None
  */
#define HAL_XSPI_DISABLE(__HANDLE__)                      CLEAR_BIT((__HANDLE__)->Instance->CR, XSPI_CR_EN)

/** @brief  Enable the specified XSPI interrupt.
  * @param  __HANDLE__ specifies the XSPI Handle.
  * @param  __INTERRUPT__ specifies the XSPI interrupt source to enable.
  *          This parameter can be one of the following values:
  *            @arg HAL_XSPI_IT_TO: XSPI Timeout interrupt
  *            @arg HAL_XSPI_IT_SM: XSPI Status match interrupt
  *            @arg HAL_XSPI_IT_FT: XSPI FIFO threshold interrupt
  *            @arg HAL_XSPI_IT_TC: XSPI Transfer complete interrupt
  *            @arg HAL_XSPI_IT_TE: XSPI Transfer error interrupt
  * @retval None
  */
#define HAL_XSPI_ENABLE_IT(__HANDLE__, __INTERRUPT__)     SET_BIT((__HANDLE__)->Instance->CR, (__INTERRUPT__))

/** @brief  Disable the specified XSPI interrupt.
  * @param  __HANDLE__ specifies the XSPI Handle.
  * @param  __INTERRUPT__ specifies the XSPI interrupt source to disable.
  *          This parameter can be one of the following values:
  *            @arg HAL_XSPI_IT_TO: XSPI Timeout interrupt
  *            @arg HAL_XSPI_IT_SM: XSPI Status match interrupt
  *            @arg HAL_XSPI_IT_FT: XSPI FIFO threshold interrupt
  *            @arg HAL_XSPI_IT_TC: XSPI Transfer complete interrupt
  *            @arg HAL_XSPI_IT_TE: XSPI Transfer error interrupt
  * @retval None
  */
#define HAL_XSPI_DISABLE_IT(__HANDLE__, __INTERRUPT__)    CLEAR_BIT((__HANDLE__)->Instance->CR, (__INTERRUPT__))

/** @brief  Check whether the specified XSPI interrupt source is enabled or not.
  * @param  __HANDLE__ specifies the XSPI Handle.
  * @param  __INTERRUPT__ specifies the XSPI interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg HAL_XSPI_IT_TO: XSPI Timeout interrupt
  *            @arg HAL_XSPI_IT_SM: XSPI Status match interrupt
  *            @arg HAL_XSPI_IT_FT: XSPI FIFO threshold interrupt
  *            @arg HAL_XSPI_IT_TC: XSPI Transfer complete interrupt
  *            @arg HAL_XSPI_IT_TE: XSPI Transfer error interrupt
  * @retval The new state of __INTERRUPT__ (TRUE or FALSE).
  */
#define HAL_XSPI_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) (READ_BIT((__HANDLE__)->Instance->CR, (__INTERRUPT__))\
                                                           == (__INTERRUPT__))

/**
  * @brief  Check whether the selected XSPI flag is set or not.
  * @param  __HANDLE__ specifies the XSPI Handle.
  * @param  __FLAG__ specifies the XSPI flag to check.
  *          This parameter can be one of the following values:
  *            @arg HAL_XSPI_FLAG_BUSY: XSPI Busy flag
  *            @arg HAL_XSPI_FLAG_TO:   XSPI Timeout flag
  *            @arg HAL_XSPI_FLAG_SM:   XSPI Status match flag
  *            @arg HAL_XSPI_FLAG_FT:   XSPI FIFO threshold flag
  *            @arg HAL_XSPI_FLAG_TC:   XSPI Transfer complete flag
  *            @arg HAL_XSPI_FLAG_TE:   XSPI Transfer error flag
  * @retval None
  */
#define HAL_XSPI_GET_FLAG(__HANDLE__, __FLAG__)           ((READ_BIT((__HANDLE__)->Instance->SR, (__FLAG__)) \
                                                            != 0U) ? SET : RESET)

/** @brief  Clears the specified XSPI's flag status.
  * @param  __HANDLE__ specifies the XSPI Handle.
  * @param  __FLAG__ specifies the XSPI clear register flag that needs to be set
  *          This parameter can be one of the following values:
  *            @arg HAL_XSPI_FLAG_TO:   XSPI Timeout flag
  *            @arg HAL_XSPI_FLAG_SM:   XSPI Status match flag
  *            @arg HAL_XSPI_FLAG_TC:   XSPI Transfer complete flag
  *            @arg HAL_XSPI_FLAG_TE:   XSPI Transfer error flag
  * @retval None
  */
#define HAL_XSPI_CLEAR_FLAG(__HANDLE__, __FLAG__)         WRITE_REG((__HANDLE__)->Instance->FCR, (__FLAG__))

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup XSPI_Exported_Functions XSPI Exported Functions
  * @{
  */

/* Initialization/de-initialization functions  ********************************/
/** @addtogroup XSPI_Exported_Functions_Group1 Initialization/de-initialization functions
  * @{
  */
HAL_StatusTypeDef     HAL_XSPI_Init(XSPI_HandleTypeDef *hxspi);
void                  HAL_XSPI_MspInit(XSPI_HandleTypeDef *hxspi);
HAL_StatusTypeDef     HAL_XSPI_DeInit(XSPI_HandleTypeDef *hxspi);
void                  HAL_XSPI_MspDeInit(XSPI_HandleTypeDef *hxspi);

/**
  * @}
  */

/* IO operation functions *****************************************************/
/** @addtogroup XSPI_Exported_Functions_Group2 Input and Output operation functions
  * @{
  */
/* XSPI IRQ handler function */
void                  HAL_XSPI_IRQHandler(XSPI_HandleTypeDef *hxspi);

/* XSPI command configuration functions */
HAL_StatusTypeDef     HAL_XSPI_Command(XSPI_HandleTypeDef *hxspi, const XSPI_RegularCmdTypeDef *pCmd,
                                       uint32_t Timeout);
HAL_StatusTypeDef     HAL_XSPI_Command_IT(XSPI_HandleTypeDef *hxspi, const XSPI_RegularCmdTypeDef *pCmd);
HAL_StatusTypeDef     HAL_XSPI_HyperbusCfg(XSPI_HandleTypeDef *hxspi, const XSPI_HyperbusCfgTypeDef *pCfg,
                                           uint32_t Timeout);
HAL_StatusTypeDef     HAL_XSPI_HyperbusCmd(XSPI_HandleTypeDef *hxspi, const XSPI_HyperbusCmdTypeDef *pCmd,
                                           uint32_t Timeout);

/* XSPI indirect mode functions */
HAL_StatusTypeDef     HAL_XSPI_Transmit(XSPI_HandleTypeDef *hxspi, const uint8_t *pData, uint32_t Timeout);
HAL_StatusTypeDef     HAL_XSPI_Receive(XSPI_HandleTypeDef *hxspi, uint8_t *pData, uint32_t Timeout);
HAL_StatusTypeDef     HAL_XSPI_Transmit_IT(XSPI_HandleTypeDef *hxspi, const uint8_t *pData);
HAL_StatusTypeDef     HAL_XSPI_Receive_IT(XSPI_HandleTypeDef *hxspi, uint8_t *pData);
HAL_StatusTypeDef     HAL_XSPI_Transmit_DMA(XSPI_HandleTypeDef *hxspi, const uint8_t *pData);
HAL_StatusTypeDef     HAL_XSPI_Receive_DMA(XSPI_HandleTypeDef *hxspi, uint8_t *pData);

/* XSPI status flag polling mode functions */
HAL_StatusTypeDef     HAL_XSPI_AutoPolling(XSPI_HandleTypeDef *hxspi, const XSPI_AutoPollingTypeDef *pCfg,
                                           uint32_t Timeout);
HAL_StatusTypeDef     HAL_XSPI_AutoPolling_IT(XSPI_HandleTypeDef *hxspi, const XSPI_AutoPollingTypeDef *pCfg);

/* XSPI memory-mapped mode functions */
HAL_StatusTypeDef     HAL_XSPI_MemoryMapped(XSPI_HandleTypeDef *hxspi,  const XSPI_MemoryMappedTypeDef *pCfg);

/* Callback functions in non-blocking modes ***********************************/
void                  HAL_XSPI_ErrorCallback(XSPI_HandleTypeDef *hxspi);
void                  HAL_XSPI_AbortCpltCallback(XSPI_HandleTypeDef *hxspi);
void                  HAL_XSPI_FifoThresholdCallback(XSPI_HandleTypeDef *hxspi);

/* XSPI indirect mode Callback functions */
void                  HAL_XSPI_CmdCpltCallback(XSPI_HandleTypeDef *hxspi);
void                  HAL_XSPI_RxCpltCallback(XSPI_HandleTypeDef *hxspi);
void                  HAL_XSPI_TxCpltCallback(XSPI_HandleTypeDef *hxspi);
void                  HAL_XSPI_RxHalfCpltCallback(XSPI_HandleTypeDef *hxspi);
void                  HAL_XSPI_TxHalfCpltCallback(XSPI_HandleTypeDef *hxspi);

/* XSPI status flag polling mode functions */
void                  HAL_XSPI_StatusMatchCallback(XSPI_HandleTypeDef *hxspi);

/* XSPI memory-mapped mode functions */
void                  HAL_XSPI_TimeOutCallback(XSPI_HandleTypeDef *hxspi);

#if defined(USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
/* XSPI callback registering/unregistering */
HAL_StatusTypeDef     HAL_XSPI_RegisterCallback(XSPI_HandleTypeDef *hxspi, HAL_XSPI_CallbackIDTypeDef CallbackID,
                                                pXSPI_CallbackTypeDef pCallback);
HAL_StatusTypeDef     HAL_XSPI_UnRegisterCallback(XSPI_HandleTypeDef *hxspi, HAL_XSPI_CallbackIDTypeDef CallbackID);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */

/**
  * @}
  */

/* Peripheral Control and State functions  ************************************/
/** @addtogroup XSPI_Exported_Functions_Group3 Peripheral Control and State functions
  * @{
  */
HAL_StatusTypeDef     HAL_XSPI_Abort(XSPI_HandleTypeDef *hxspi);
HAL_StatusTypeDef     HAL_XSPI_Abort_IT(XSPI_HandleTypeDef *hxspi);
HAL_StatusTypeDef     HAL_XSPI_SetFifoThreshold(XSPI_HandleTypeDef *hxspi, uint32_t Threshold);
uint32_t              HAL_XSPI_GetFifoThreshold(const XSPI_HandleTypeDef *hxspi);
HAL_StatusTypeDef     HAL_XSPI_SetMemoryType(XSPI_HandleTypeDef *hxspi, uint32_t Type);
HAL_StatusTypeDef     HAL_XSPI_SetDeviceSize(XSPI_HandleTypeDef *hxspi, uint32_t Size);
HAL_StatusTypeDef     HAL_XSPI_SetClockPrescaler(XSPI_HandleTypeDef *hxspi, uint32_t Prescaler);
HAL_StatusTypeDef     HAL_XSPI_SetTimeout(XSPI_HandleTypeDef *hxspi, uint32_t Timeout);
uint32_t              HAL_XSPI_GetError(const XSPI_HandleTypeDef *hxspi);
uint32_t              HAL_XSPI_GetState(const XSPI_HandleTypeDef *hxspi);

/**
  * @}
  */

/* XSPI IO Manager configuration function  ************************************/
/** @addtogroup XSPI_Exported_Functions_Group4  IO Manager configuration function
  * @{
  */
HAL_StatusTypeDef     HAL_XSPIM_Config(XSPI_HandleTypeDef *hxspi, const XSPIM_CfgTypeDef *pCfg, uint32_t Timeout);

/**
  * @}
  */

/* XSPI high-speed interface and calibration functions  ***********************/
/** @addtogroup XSPI_Exported_Functions_Group6 High-speed interface and calibration functions
  * @{
  */
HAL_StatusTypeDef     HAL_XSPI_GetDelayValue(XSPI_HandleTypeDef *hxspi, XSPI_HSCalTypeDef *pCfg);
HAL_StatusTypeDef     HAL_XSPI_SetDelayValue(XSPI_HandleTypeDef *hxspi, const XSPI_HSCalTypeDef *pCfg);

/**
  * @}
  */

/**
  * @}
  */
/* End of exported functions -------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
/**
  @cond 0
  */
#define IS_XSPI_FIFO_THRESHOLD_BYTE(THRESHOLD)    (((THRESHOLD) >= 1U) &&\
                                                   ((THRESHOLD) <= ((XSPI_CR_FTHRES >> XSPI_CR_FTHRES_Pos)+1U)))
#define IS_XSPI_MEMORY_MODE(MODE)                 (((MODE) == HAL_XSPI_SINGLE_MEM) || \
                                                   ((MODE) == HAL_XSPI_DUAL_MEM))

#define IS_XSPI_MEMORY_TYPE(TYPE)                 (((TYPE) == HAL_XSPI_MEMTYPE_MICRON)       || \
                                                   ((TYPE) == HAL_XSPI_MEMTYPE_MACRONIX)     || \
                                                   ((TYPE) == HAL_XSPI_MEMTYPE_APMEM)     || \
                                                   ((TYPE) == HAL_XSPI_MEMTYPE_MACRONIX_RAM) || \
                                                   ((TYPE) == HAL_XSPI_MEMTYPE_HYPERBUS)     || \
                                                   ((TYPE) == HAL_XSPI_MEMTYPE_APMEM_16BITS))

#define IS_XSPI_MEMORY_SIZE(SIZE)                 (((SIZE) == HAL_XSPI_SIZE_16B)      || \
                                                   ((SIZE) == HAL_XSPI_SIZE_32B)      || \
                                                   ((SIZE) == HAL_XSPI_SIZE_64B)      || \
                                                   ((SIZE) == HAL_XSPI_SIZE_128B)     || \
                                                   ((SIZE) == HAL_XSPI_SIZE_256B)     || \
                                                   ((SIZE) == HAL_XSPI_SIZE_512B)     || \
                                                   ((SIZE) == HAL_XSPI_SIZE_1KB)      || \
                                                   ((SIZE) == HAL_XSPI_SIZE_2KB)      || \
                                                   ((SIZE) == HAL_XSPI_SIZE_4KB)      || \
                                                   ((SIZE) == HAL_XSPI_SIZE_8KB)      || \
                                                   ((SIZE) == HAL_XSPI_SIZE_16KB)     || \
                                                   ((SIZE) == HAL_XSPI_SIZE_32KB)     || \
                                                   ((SIZE) == HAL_XSPI_SIZE_64KB)     || \
                                                   ((SIZE) == HAL_XSPI_SIZE_128KB)    || \
                                                   ((SIZE) == HAL_XSPI_SIZE_256KB)    || \
                                                   ((SIZE) == HAL_XSPI_SIZE_512KB)    || \
                                                   ((SIZE) == HAL_XSPI_SIZE_1MB)      || \
                                                   ((SIZE) == HAL_XSPI_SIZE_2MB)      || \
                                                   ((SIZE) == HAL_XSPI_SIZE_4MB)      || \
                                                   ((SIZE) == HAL_XSPI_SIZE_8MB)      || \
                                                   ((SIZE) == HAL_XSPI_SIZE_16MB)     || \
                                                   ((SIZE) == HAL_XSPI_SIZE_32MB)     || \
                                                   ((SIZE) == HAL_XSPI_SIZE_64MB)     || \
                                                   ((SIZE) == HAL_XSPI_SIZE_128MB)    || \
                                                   ((SIZE) == HAL_XSPI_SIZE_256MB)    || \
                                                   ((SIZE) == HAL_XSPI_SIZE_512MB)    || \
                                                   ((SIZE) == HAL_XSPI_SIZE_1GB)      || \
                                                   ((SIZE) == HAL_XSPI_SIZE_2GB)      || \
                                                   ((SIZE) == HAL_XSPI_SIZE_4GB)      || \
                                                   ((SIZE) == HAL_XSPI_SIZE_8GB)      || \
                                                   ((SIZE) == HAL_XSPI_SIZE_16GB)     || \
                                                   ((SIZE) == HAL_XSPI_SIZE_32GB))

#define IS_XSPI_CS_HIGH_TIME_CYCLE(TIME)          (((TIME) >= 1U) && ((TIME) <= 64U))

#define IS_XSPI_FREE_RUN_CLK(CLK)                 (((CLK) == HAL_XSPI_FREERUNCLK_DISABLE) || \
                                                   ((CLK) == HAL_XSPI_FREERUNCLK_ENABLE))

#define IS_XSPI_CLOCK_MODE(MODE)                  (((MODE) == HAL_XSPI_CLOCK_MODE_0) || \
                                                   ((MODE) == HAL_XSPI_CLOCK_MODE_3))

#define IS_XSPI_WRAP_SIZE(SIZE)                   (((SIZE) == HAL_XSPI_WRAP_NOT_SUPPORTED) || \
                                                   ((SIZE) == HAL_XSPI_WRAP_16_BYTES)      || \
                                                   ((SIZE) == HAL_XSPI_WRAP_32_BYTES)      || \
                                                   ((SIZE) == HAL_XSPI_WRAP_64_BYTES)      || \
                                                   ((SIZE) == HAL_XSPI_WRAP_128_BYTES))

#define IS_XSPI_CLK_PRESCALER(PRESCALER)          ((PRESCALER) <= 255U)

#define IS_XSPI_SAMPLE_SHIFTING(CYCLE)            (((CYCLE) == HAL_XSPI_SAMPLE_SHIFT_NONE)      || \
                                                   ((CYCLE) == HAL_XSPI_SAMPLE_SHIFT_HALFCYCLE))

#define IS_XSPI_DHQC(CYCLE)                       (((CYCLE) == HAL_XSPI_DHQC_DISABLE) || \
                                                   ((CYCLE) == HAL_XSPI_DHQC_ENABLE))

#define IS_XSPI_CS_BOUND(SIZE)                    (((SIZE) == HAL_XSPI_BONDARYOF_NONE)     || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_16B)      || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_32B)      || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_64B)      || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_128B)     || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_256B)     || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_512B)     || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_1KB)      || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_2KB)      || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_4KB)      || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_8KB)      || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_16KB)     || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_32KB)     || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_64KB)     || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_128KB)    || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_256KB)    || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_512KB)    || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_1MB)      || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_2MB)      || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_4MB)      || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_8MB)      || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_16MB)     || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_32MB)     || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_64MB)     || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_128MB)    || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_256MB)    || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_512MB)    || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_1GB)      || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_2GB)      || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_4GB)      || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_8GB)      || \
                                                   ((SIZE) == HAL_XSPI_BONDARYOF_16GB))


#define IS_XSPI_MAXTRAN(NB_BYTES)                 ((NB_BYTES) <= 255U)

#define IS_XSPI_CSSEL(CSSEL)                      (((CSSEL) == HAL_XSPI_CSSEL_NCS1) || \
                                                   ((CSSEL) == HAL_XSPI_CSSEL_NCS2))

#define IS_XSPI_EXTENDMEM(EXTENDMEM)              (((EXTENDMEM) == HAL_XSPI_CSSEL_SW) || \
                                                   ((EXTENDMEM) == HAL_XSPI_CSSEL_HW))

#define IS_XSPI_OPERATION_TYPE(TYPE)              (((TYPE) == HAL_XSPI_OPTYPE_COMMON_CFG) || \
                                                   ((TYPE) == HAL_XSPI_OPTYPE_READ_CFG)   || \
                                                   ((TYPE) == HAL_XSPI_OPTYPE_WRITE_CFG)  || \
                                                   ((TYPE) == HAL_XSPI_OPTYPE_WRAP_CFG))

#define IS_XSPI_IO_SELECT(MEMSEL)                 (((MEMSEL) == HAL_XSPI_SELECT_IO_3_0)   || \
                                                   ((MEMSEL) == HAL_XSPI_SELECT_IO_7_4)   || \
                                                   ((MEMSEL) == HAL_XSPI_SELECT_IO_11_8)  || \
                                                   ((MEMSEL) == HAL_XSPI_SELECT_IO_15_12) || \
                                                   ((MEMSEL) == HAL_XSPI_SELECT_IO_7_0)   || \
                                                   ((MEMSEL) == HAL_XSPI_SELECT_IO_15_8))

#define IS_XSPI_INSTRUCTION(OPCODE)               ((OPCODE) <= 0xFFFFFFFFU)

#define IS_XSPI_INSTRUCTION_MODE(MODE)            (((MODE) == HAL_XSPI_INSTRUCTION_NONE)    || \
                                                   ((MODE) == HAL_XSPI_INSTRUCTION_1_LINE)  || \
                                                   ((MODE) == HAL_XSPI_INSTRUCTION_2_LINES) || \
                                                   ((MODE) == HAL_XSPI_INSTRUCTION_4_LINES) || \
                                                   ((MODE) == HAL_XSPI_INSTRUCTION_8_LINES))

#define IS_XSPI_INSTRUCTION_WIDTH(WIDTH)          (((WIDTH) == HAL_XSPI_INSTRUCTION_8_BITS)  || \
                                                   ((WIDTH) == HAL_XSPI_INSTRUCTION_16_BITS) || \
                                                   ((WIDTH) == HAL_XSPI_INSTRUCTION_24_BITS) || \
                                                   ((WIDTH) == HAL_XSPI_INSTRUCTION_32_BITS))

#define IS_XSPI_INSTRUCTION_DTR_MODE(MODE)        (((MODE) == HAL_XSPI_INSTRUCTION_DTR_DISABLE) || \
                                                   ((MODE) == HAL_XSPI_INSTRUCTION_DTR_ENABLE))

#define IS_XSPI_ADDRESS_MODE(MODE)                (((MODE) == HAL_XSPI_ADDRESS_NONE)    || \
                                                   ((MODE) == HAL_XSPI_ADDRESS_1_LINE)  || \
                                                   ((MODE) == HAL_XSPI_ADDRESS_2_LINES) || \
                                                   ((MODE) == HAL_XSPI_ADDRESS_4_LINES) || \
                                                   ((MODE) == HAL_XSPI_ADDRESS_8_LINES))

#define IS_XSPI_ADDRESS_WIDTH(WIDTH)              (((WIDTH) == HAL_XSPI_ADDRESS_8_BITS)  || \
                                                   ((WIDTH) == HAL_XSPI_ADDRESS_16_BITS) || \
                                                   ((WIDTH) == HAL_XSPI_ADDRESS_24_BITS) || \
                                                   ((WIDTH) == HAL_XSPI_ADDRESS_32_BITS))

#define IS_XSPI_ADDRESS_DTR_MODE(MODE)            (((MODE) == HAL_XSPI_ADDRESS_DTR_DISABLE) || \
                                                   ((MODE) == HAL_XSPI_ADDRESS_DTR_ENABLE))

#define IS_XSPI_ALT_BYTES_MODE(MODE)              (((MODE) == HAL_XSPI_ALT_BYTES_NONE)    || \
                                                   ((MODE) == HAL_XSPI_ALT_BYTES_1_LINE)  || \
                                                   ((MODE) == HAL_XSPI_ALT_BYTES_2_LINES) || \
                                                   ((MODE) == HAL_XSPI_ALT_BYTES_4_LINES) || \
                                                   ((MODE) == HAL_XSPI_ALT_BYTES_8_LINES))

#define IS_XSPI_ALT_BYTES_WIDTH(WIDTH)            (((WIDTH) == HAL_XSPI_ALT_BYTES_8_BITS)  || \
                                                   ((WIDTH) == HAL_XSPI_ALT_BYTES_16_BITS) || \
                                                   ((WIDTH) == HAL_XSPI_ALT_BYTES_24_BITS) || \
                                                   ((WIDTH) == HAL_XSPI_ALT_BYTES_32_BITS))

#define IS_XSPI_ALT_BYTES_DTR_MODE(MODE)          (((MODE) == HAL_XSPI_ALT_BYTES_DTR_DISABLE) || \
                                                   ((MODE) == HAL_XSPI_ALT_BYTES_DTR_ENABLE))

#define IS_XSPI_DATA_MODE(TYPE,MODE)              (((TYPE) == (HAL_XSPI_MEMTYPE_HYPERBUS)) ? \
                                                   (((MODE) == HAL_XSPI_DATA_NONE)    || \
                                                    ((MODE) == HAL_XSPI_DATA_8_LINES) || \
                                                    ((MODE) == HAL_XSPI_DATA_16_LINES)): \
                                                   (((MODE) == HAL_XSPI_DATA_NONE)    || \
                                                    ((MODE) == HAL_XSPI_DATA_1_LINE)  || \
                                                    ((MODE) == HAL_XSPI_DATA_2_LINES) || \
                                                    ((MODE) == HAL_XSPI_DATA_4_LINES) || \
                                                    ((MODE) == HAL_XSPI_DATA_8_LINES) || \
                                                    ((MODE) == HAL_XSPI_DATA_16_LINES)))
#define IS_XSPI_DATA_LENGTH(NUMBER)               ((NUMBER) >= 1U)

#define IS_XSPI_DATA_DTR_MODE(MODE)               (((MODE) == HAL_XSPI_DATA_DTR_DISABLE) || \
                                                   ((MODE) == HAL_XSPI_DATA_DTR_ENABLE))

#define IS_XSPI_DUMMY_CYCLES(NUMBER)              ((NUMBER) <= 31U)

#define IS_XSPI_DQS_MODE(MODE)                    (((MODE) == HAL_XSPI_DQS_DISABLE) || \
                                                   ((MODE) == HAL_XSPI_DQS_ENABLE))

#define IS_XSPI_RW_RECOVERY_TIME_CYCLE(CYCLE)     ((CYCLE) <= 255U)

#define IS_XSPI_ACCESS_TIME_CYCLE(CYCLE)          ((CYCLE) <= 255U)

#define IS_XSPI_WRITE_ZERO_LATENCY(MODE)          (((MODE) == HAL_XSPI_LATENCY_ON_WRITE) || \
                                                   ((MODE) == HAL_XSPI_NO_LATENCY_ON_WRITE))

#define IS_XSPI_LATENCY_MODE(MODE)                (((MODE) == HAL_XSPI_VARIABLE_LATENCY) || \
                                                   ((MODE) == HAL_XSPI_FIXED_LATENCY))

#define IS_XSPI_ADDRESS_SPACE(SPACE)              (((SPACE) == HAL_XSPI_MEMORY_ADDRESS_SPACE) || \
                                                   ((SPACE) == HAL_XSPI_REGISTER_ADDRESS_SPACE))

#define IS_XSPI_MATCH_MODE(MODE)                  (((MODE) == HAL_XSPI_MATCH_MODE_AND) || \
                                                   ((MODE) == HAL_XSPI_MATCH_MODE_OR))

#define IS_XSPI_AUTOMATIC_STOP(MODE)              (((MODE) == HAL_XSPI_AUTOMATIC_STOP_ENABLE) || \
                                                   ((MODE) == HAL_XSPI_AUTOMATIC_STOP_DISABLE))

#define IS_XSPI_INTERVAL(INTERVAL)                ((INTERVAL) <= 0xFFFFU)

#define IS_XSPI_STATUS_BYTES_SIZE(SIZE)           (((SIZE) >= 1U) && ((SIZE) <= 4U))

#define IS_XSPI_TIMEOUT_ACTIVATION(MODE)          (((MODE) == HAL_XSPI_TIMEOUT_COUNTER_DISABLE) || \
                                                   ((MODE) == HAL_XSPI_TIMEOUT_COUNTER_ENABLE))
#define IS_XSPI_NO_PREFETCH_DATA(MODE)            (((MODE) == HAL_XSPI_AUTOMATIC_PREFETCH_ENABLE) || \
                                                   ((MODE) == HAL_XSPI_AUTOMATIC_PREFETCH_DISABLE))

#define IS_XSPI_NO_PREFETCH_AXI(MODE)             (((MODE) == HAL_XSPI_AXI_PREFETCH_ENABLE) || \
                                                   ((MODE) == HAL_XSPI_AXI_PREFETCH_DISABLE))

#define IS_XSPI_TIMEOUT_PERIOD(PERIOD)            ((PERIOD) <= 0xFFFFU)

#define IS_XSPIM_IO_PORT(PORT)                    (((PORT) == HAL_XSPIM_IOPORT_1)   || \
                                                   ((PORT) == HAL_XSPIM_IOPORT_2))

#define IS_XSPIM_NCS_OVR(PORT)                    (((PORT) == HAL_XSPI_CSSEL_OVR_DISABLED) || \
                                                   ((PORT) == HAL_XSPI_CSSEL_OVR_NCS1)     || \
                                                   ((PORT) == HAL_XSPI_CSSEL_OVR_NCS2))

#define IS_XSPIM_REQ2ACKTIME(TIME)                (((TIME) >= 1U) && ((TIME) <= 256U))

#define IS_XSPI_DELAY_TYPE(TYPE)                  (((TYPE) == HAL_XSPI_CAL_FULL_CYCLE_DELAY)   || \
                                                   ((TYPE) == HAL_XSPI_CAL_FEEDBACK_CLK_DELAY) || \
                                                   ((TYPE) == HAL_XSPI_CAL_DATA_OUTPUT_DELAY)  || \
                                                   ((TYPE) == HAL_XSPI_CAL_DQS_INPUT_DELAY))

#define IS_XSPI_FINECAL_VALUE(VALUE)              ((VALUE) <= 0x7FU)

#define IS_XSPI_COARSECAL_VALUE(VALUE)            ((VALUE) <= 0x1FU)

/**
  @endcond
  */

/* End of private macros -----------------------------------------------------*/

/**
  * @}
  */

/**
  * @}
  */

#endif /* XSPI || XSPI1 || XSPI2 || XSPI3 */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_HAL_XSPI_H */
