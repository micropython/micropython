/**
  ******************************************************************************
  * @file    stm32n6xx_hal_spi.h
  * @author  MCD Application Team
  * @brief   Header file of SPI HAL module.
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
#ifndef STM32N6xx_HAL_SPI_H
#define STM32N6xx_HAL_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @addtogroup SPI
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup SPI_Exported_Types SPI Exported Types
  * @{
  */

/**
  * @brief  SPI Configuration Structure definition
  */
typedef struct
{
  uint32_t Mode;                              /*!< Specifies the SPI operating mode.
                                                     This parameter can be a value of @ref SPI_Mode */

  uint32_t Direction;                         /*!< Specifies the SPI bidirectional mode state.
                                                     This parameter can be a value of @ref SPI_Direction */

  uint32_t DataSize;                          /*!< Specifies the SPI data size.
                                                     This parameter can be a value of @ref SPI_Data_Size */

  uint32_t CLKPolarity;                       /*!< Specifies the serial clock steady state.
                                                     This parameter can be a value of @ref SPI_Clock_Polarity */

  uint32_t CLKPhase;                          /*!< Specifies the clock active edge for the bit capture.
                                                     This parameter can be a value of @ref SPI_Clock_Phase */

  uint32_t NSS;                               /*!< Specifies whether the NSS signal is managed by
                                                     hardware (NSS pin) or by software using the SSI bit.
                                                     This parameter can be a value of
                                                     @ref SPI_Slave_Select_Management */

  uint32_t BaudRatePrescaler;                 /*!< Specifies the Baud Rate prescaler value which will be
                                                     used to configure the transmit and receive SCK clock.
                                                     This parameter can be a value of @ref SPI_BaudRate_Prescaler
                                                     @note The communication clock is derived from the master
                                                     clock. The slave clock does not need to be set. */

  uint32_t FirstBit;                          /*!< Specifies whether data transfers start from MSB or LSB bit.
                                                     This parameter can be a value of @ref SPI_MSB_LSB_Transmission */

  uint32_t TIMode;                            /*!< Specifies if the TI mode is enabled or not.
                                                     This parameter can be a value of @ref SPI_TI_Mode */

  uint32_t CRCCalculation;                    /*!< Specifies if the CRC calculation is enabled or not.
                                                     This parameter can be a value of @ref SPI_CRC_Calculation */

  uint32_t CRCPolynomial;                     /*!< Specifies the polynomial used for the CRC calculation.
                                                     This parameter must be an odd number between
                                                     Min_Data = 0 and Max_Data = 65535 */

  uint32_t CRCLength;                         /*!< Specifies the CRC Length used for the CRC calculation.
                                                     This parameter can be a value of @ref SPI_CRC_length */

  uint32_t NSSPMode;                          /*!< Specifies whether the NSSP signal is enabled or not .
                                                     This parameter can be a value of @ref SPI_NSSP_Mode
                                                     This mode is activated by the SSOM bit in the SPIx_CR2 register
                                                     and it takes effect only if the SPI interface is configured
                                                     as Motorola SPI master (FRF=0). */

  uint32_t NSSPolarity;                       /*!< Specifies which level of SS input/output external signal
                                                     (present on SS pin) is considered as active one.
                                                     This parameter can be a value of @ref SPI_NSS_Polarity */

  uint32_t FifoThreshold;                     /*!< Specifies the FIFO threshold level.
                                                     This parameter can be a value of @ref SPI_Fifo_Threshold */

  uint32_t TxCRCInitializationPattern;        /*!< Specifies the transmitter CRC initialization Pattern used for
                                                     the CRC calculation. This parameter can be a value of
                                                     @ref SPI_CRC_Calculation_Initialization_Pattern */

  uint32_t RxCRCInitializationPattern;        /*!< Specifies the receiver CRC initialization Pattern used for
                                                     the CRC calculation. This parameter can be a value of
                                                     @ref SPI_CRC_Calculation_Initialization_Pattern */

  uint32_t MasterSSIdleness;                  /*!< Specifies an extra delay, expressed in number of SPI clock cycle
                                                     periods, inserted additionally between active edge of SS
                                                     and first data transaction start in master mode.
                                                     This parameter can be a value of @ref SPI_Master_SS_Idleness */

  uint32_t MasterInterDataIdleness;           /*!< Specifies minimum time delay (expressed in SPI clock cycles periods)
                                                     inserted between two consecutive data frames in master mode.
                                                     This parameter can be a value of
                                                     @ref SPI_Master_InterData_Idleness */

  uint32_t MasterReceiverAutoSusp;            /*!< Control continuous SPI transfer in master receiver mode
                                                     and automatic management in order to avoid overrun condition.
                                                     This parameter can be a value of @ref SPI_Master_RX_AutoSuspend*/

  uint32_t MasterKeepIOState;                 /*!< Control of Alternate function GPIOs state
                                                     This parameter can be a value of @ref SPI_Master_Keep_IO_State */

  uint32_t IOSwap;                            /*!< Invert MISO/MOSI alternate functions
                                                     This parameter can be a value of @ref SPI_IO_Swap */

  uint32_t ReadyMasterManagement;             /*!< Specifies if RDY Signal is managed internally or not.
                                                     This parameter can be a value of @ref SPI_RDY_Master_Management */

  uint32_t ReadyPolarity;                     /*!< Specifies which level of RDY Signal input (present on RDY pin)
                                                     is considered as active one.
                                                     This parameter can be a value of @ref SPI_RDY_Polarity */
} SPI_InitTypeDef;

/**
  * @brief  HAL SPI State structure definition
  */
typedef enum
{
  HAL_SPI_STATE_RESET      = 0x00UL,    /*!< Peripheral not Initialized                         */
  HAL_SPI_STATE_READY      = 0x01UL,    /*!< Peripheral Initialized and ready for use           */
  HAL_SPI_STATE_BUSY       = 0x02UL,    /*!< an internal process is ongoing                     */
  HAL_SPI_STATE_BUSY_TX    = 0x03UL,    /*!< Data Transmission process is ongoing               */
  HAL_SPI_STATE_BUSY_RX    = 0x04UL,    /*!< Data Reception process is ongoing                  */
  HAL_SPI_STATE_BUSY_TX_RX = 0x05UL,    /*!< Data Transmission and Reception process is ongoing */
  HAL_SPI_STATE_ERROR      = 0x06UL,    /*!< SPI error state                                    */
  HAL_SPI_STATE_ABORT      = 0x07UL     /*!< SPI abort is ongoing                               */
} HAL_SPI_StateTypeDef;


/**
  * @brief  SPI handle Structure definition
  */
typedef struct __SPI_HandleTypeDef
{
  SPI_TypeDef                *Instance;                    /*!< SPI registers base address               */

  SPI_InitTypeDef            Init;                         /*!< SPI communication parameters             */

  const uint8_t              *pTxBuffPtr;                  /*!< Pointer to SPI Tx transfer Buffer        */

  uint16_t                   TxXferSize;                   /*!< SPI Tx Transfer size                     */

  __IO uint16_t              TxXferCount;                  /*!< SPI Tx Transfer Counter                  */

  uint8_t                    *pRxBuffPtr;                  /*!< Pointer to SPI Rx transfer Buffer        */

  uint16_t                   RxXferSize;                   /*!< SPI Rx Transfer size                     */

  __IO uint16_t              RxXferCount;                  /*!< SPI Rx Transfer Counter                  */

  uint32_t                   CRCSize;                      /*!< SPI CRC size used for the transfer       */

  void (*RxISR)(struct __SPI_HandleTypeDef *hspi);         /*!< function pointer on Rx ISR               */

  void (*TxISR)(struct __SPI_HandleTypeDef *hspi);         /*!< function pointer on Tx ISR               */

  DMA_HandleTypeDef          *hdmatx;                      /*!< SPI Tx DMA Handle parameters             */

  DMA_HandleTypeDef          *hdmarx;                      /*!< SPI Rx DMA Handle parameters             */

  HAL_LockTypeDef            Lock;                         /*!< Locking object                           */

  __IO HAL_SPI_StateTypeDef  State;                        /*!< SPI communication state                  */

  __IO uint32_t              ErrorCode;                    /*!< SPI Error code                           */


#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
  void (* TxCpltCallback)(struct __SPI_HandleTypeDef *hspi);       /*!< SPI Tx Completed callback          */
  void (* RxCpltCallback)(struct __SPI_HandleTypeDef *hspi);       /*!< SPI Rx Completed callback          */
  void (* TxRxCpltCallback)(struct __SPI_HandleTypeDef *hspi);     /*!< SPI TxRx Completed callback        */
  void (* TxHalfCpltCallback)(struct __SPI_HandleTypeDef *hspi);   /*!< SPI Tx Half Completed callback     */
  void (* RxHalfCpltCallback)(struct __SPI_HandleTypeDef *hspi);   /*!< SPI Rx Half Completed callback     */
  void (* TxRxHalfCpltCallback)(struct __SPI_HandleTypeDef *hspi); /*!< SPI TxRx Half Completed callback   */
  void (* ErrorCallback)(struct __SPI_HandleTypeDef *hspi);        /*!< SPI Error callback                 */
  void (* AbortCpltCallback)(struct __SPI_HandleTypeDef *hspi);    /*!< SPI Abort callback                 */
  void (* SuspendCallback)(struct __SPI_HandleTypeDef *hspi);      /*!< SPI Suspend callback               */
  void (* MspInitCallback)(struct __SPI_HandleTypeDef *hspi);      /*!< SPI Msp Init callback              */
  void (* MspDeInitCallback)(struct __SPI_HandleTypeDef *hspi);    /*!< SPI Msp DeInit callback            */

#endif  /* USE_HAL_SPI_REGISTER_CALLBACKS */
} SPI_HandleTypeDef;

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
/**
  * @brief  HAL SPI Callback ID enumeration definition
  */
typedef enum
{
  HAL_SPI_TX_COMPLETE_CB_ID             = 0x00UL,    /*!< SPI Tx Completed callback ID         */
  HAL_SPI_RX_COMPLETE_CB_ID             = 0x01UL,    /*!< SPI Rx Completed callback ID         */
  HAL_SPI_TX_RX_COMPLETE_CB_ID          = 0x02UL,    /*!< SPI TxRx Completed callback ID       */
  HAL_SPI_TX_HALF_COMPLETE_CB_ID        = 0x03UL,    /*!< SPI Tx Half Completed callback ID    */
  HAL_SPI_RX_HALF_COMPLETE_CB_ID        = 0x04UL,    /*!< SPI Rx Half Completed callback ID    */
  HAL_SPI_TX_RX_HALF_COMPLETE_CB_ID     = 0x05UL,    /*!< SPI TxRx Half Completed callback ID  */
  HAL_SPI_ERROR_CB_ID                   = 0x06UL,    /*!< SPI Error callback ID                */
  HAL_SPI_ABORT_CB_ID                   = 0x07UL,    /*!< SPI Abort callback ID                */
  HAL_SPI_SUSPEND_CB_ID                 = 0x08UL,    /*!< SPI Suspend callback ID              */
  HAL_SPI_MSPINIT_CB_ID                 = 0x09UL,    /*!< SPI Msp Init callback ID             */
  HAL_SPI_MSPDEINIT_CB_ID               = 0x0AUL     /*!< SPI Msp DeInit callback ID           */

} HAL_SPI_CallbackIDTypeDef;

/**
  * @brief  HAL SPI Callback pointer definition
  */
typedef  void (*pSPI_CallbackTypeDef)(SPI_HandleTypeDef *hspi); /*!< pointer to an SPI callback function */

#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup SPI_Exported_Constants SPI Exported Constants
  * @{
  */

/** @defgroup SPI_FIFO_Type SPI FIFO Type
  * @{
  */
#define SPI_LOWEND_FIFO_SIZE                          8UL
#define SPI_HIGHEND_FIFO_SIZE                         16UL
/**
  * @}
  */

/** @defgroup SPI_Error_Code SPI Error Codes
  * @{
  */
#define HAL_SPI_ERROR_NONE                            (0x00000000UL)   /*!< No error                               */
#define HAL_SPI_ERROR_MODF                            (0x00000001UL)   /*!< MODF error                             */
#define HAL_SPI_ERROR_CRC                             (0x00000002UL)   /*!< CRC error                              */
#define HAL_SPI_ERROR_OVR                             (0x00000004UL)   /*!< OVR error                              */
#define HAL_SPI_ERROR_FRE                             (0x00000008UL)   /*!< FRE error                              */
#define HAL_SPI_ERROR_DMA                             (0x00000010UL)   /*!< DMA transfer error                     */
#define HAL_SPI_ERROR_FLAG                            (0x00000020UL)   /*!< Error on RXP/TXP/DXP/FTLVL/FRLVL Flag  */
#define HAL_SPI_ERROR_ABORT                           (0x00000040UL)   /*!< Error during SPI Abort procedure       */
#define HAL_SPI_ERROR_UDR                             (0x00000080UL)   /*!< Underrun error                         */
#define HAL_SPI_ERROR_TIMEOUT                         (0x00000100UL)   /*!< Timeout error                          */
#define HAL_SPI_ERROR_UNKNOW                          (0x00000200UL)   /*!< Unknown error                          */
#define HAL_SPI_ERROR_NOT_SUPPORTED                   (0x00000400UL)   /*!< Requested operation not supported      */
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
#define HAL_SPI_ERROR_INVALID_CALLBACK                (0x00001000UL)   /*!< Invalid Callback error                 */
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
/**
  * @}
  */

/** @defgroup SPI_Mode SPI Mode
  * @{
  */
#define SPI_MODE_SLAVE                                (0x00000000UL)
#define SPI_MODE_MASTER                               SPI_CFG2_MASTER
/**
  * @}
  */

/** @defgroup SPI_Direction SPI Direction Mode
  * @{
  */
#define SPI_DIRECTION_2LINES                          (0x00000000UL)
#define SPI_DIRECTION_2LINES_TXONLY                   SPI_CFG2_COMM_0
#define SPI_DIRECTION_2LINES_RXONLY                   SPI_CFG2_COMM_1
#define SPI_DIRECTION_1LINE                           SPI_CFG2_COMM
/**
  * @}
  */

/** @defgroup SPI_Data_Size SPI Data Size
  * @{
  */
#define SPI_DATASIZE_4BIT                             (0x00000003UL)
#define SPI_DATASIZE_5BIT                             (0x00000004UL)
#define SPI_DATASIZE_6BIT                             (0x00000005UL)
#define SPI_DATASIZE_7BIT                             (0x00000006UL)
#define SPI_DATASIZE_8BIT                             (0x00000007UL)
#define SPI_DATASIZE_9BIT                             (0x00000008UL)
#define SPI_DATASIZE_10BIT                            (0x00000009UL)
#define SPI_DATASIZE_11BIT                            (0x0000000AUL)
#define SPI_DATASIZE_12BIT                            (0x0000000BUL)
#define SPI_DATASIZE_13BIT                            (0x0000000CUL)
#define SPI_DATASIZE_14BIT                            (0x0000000DUL)
#define SPI_DATASIZE_15BIT                            (0x0000000EUL)
#define SPI_DATASIZE_16BIT                            (0x0000000FUL)
#define SPI_DATASIZE_17BIT                            (0x00000010UL)
#define SPI_DATASIZE_18BIT                            (0x00000011UL)
#define SPI_DATASIZE_19BIT                            (0x00000012UL)
#define SPI_DATASIZE_20BIT                            (0x00000013UL)
#define SPI_DATASIZE_21BIT                            (0x00000014UL)
#define SPI_DATASIZE_22BIT                            (0x00000015UL)
#define SPI_DATASIZE_23BIT                            (0x00000016UL)
#define SPI_DATASIZE_24BIT                            (0x00000017UL)
#define SPI_DATASIZE_25BIT                            (0x00000018UL)
#define SPI_DATASIZE_26BIT                            (0x00000019UL)
#define SPI_DATASIZE_27BIT                            (0x0000001AUL)
#define SPI_DATASIZE_28BIT                            (0x0000001BUL)
#define SPI_DATASIZE_29BIT                            (0x0000001CUL)
#define SPI_DATASIZE_30BIT                            (0x0000001DUL)
#define SPI_DATASIZE_31BIT                            (0x0000001EUL)
#define SPI_DATASIZE_32BIT                            (0x0000001FUL)
/**
  * @}
  */

/** @defgroup SPI_Clock_Polarity SPI Clock Polarity
  * @{
  */
#define SPI_POLARITY_LOW                              (0x00000000UL)
#define SPI_POLARITY_HIGH                             SPI_CFG2_CPOL
/**
  * @}
  */

/** @defgroup SPI_Clock_Phase SPI Clock Phase
  * @{
  */
#define SPI_PHASE_1EDGE                               (0x00000000UL)
#define SPI_PHASE_2EDGE                               SPI_CFG2_CPHA
/**
  * @}
  */

/** @defgroup SPI_Slave_Select_Management SPI Slave Select Management
  * @{
  */
#define SPI_NSS_SOFT                                  SPI_CFG2_SSM
#define SPI_NSS_HARD_INPUT                            (0x00000000UL)
#define SPI_NSS_HARD_OUTPUT                           SPI_CFG2_SSOE
/**
  * @}
  */

/** @defgroup SPI_NSSP_Mode SPI NSS Pulse Mode
  * @{
  */
#define SPI_NSS_PULSE_DISABLE                         (0x00000000UL)
#define SPI_NSS_PULSE_ENABLE                          SPI_CFG2_SSOM
/**
  * @}
  */

/** @defgroup SPI_BaudRate_Prescaler SPI BaudRate Prescaler
  * @{
  */
#define SPI_BAUDRATEPRESCALER_BYPASS                  (0x80000000UL)
#define SPI_BAUDRATEPRESCALER_2                       (0x00000000UL)
#define SPI_BAUDRATEPRESCALER_4                       (0x10000000UL)
#define SPI_BAUDRATEPRESCALER_8                       (0x20000000UL)
#define SPI_BAUDRATEPRESCALER_16                      (0x30000000UL)
#define SPI_BAUDRATEPRESCALER_32                      (0x40000000UL)
#define SPI_BAUDRATEPRESCALER_64                      (0x50000000UL)
#define SPI_BAUDRATEPRESCALER_128                     (0x60000000UL)
#define SPI_BAUDRATEPRESCALER_256                     (0x70000000UL)
/**
  * @}
  */

/** @defgroup SPI_MSB_LSB_Transmission SPI MSB LSB Transmission
  * @{
  */
#define SPI_FIRSTBIT_MSB                              (0x00000000UL)
#define SPI_FIRSTBIT_LSB                              SPI_CFG2_LSBFRST
/**
  * @}
  */

/** @defgroup SPI_TI_Mode SPI TI Mode
  * @{
  */
#define SPI_TIMODE_DISABLE                            (0x00000000UL)
#define SPI_TIMODE_ENABLE                             SPI_CFG2_SP_0
/**
  * @}
  */

/** @defgroup SPI_CRC_Calculation SPI CRC Calculation
  * @{
  */
#define SPI_CRCCALCULATION_DISABLE                    (0x00000000UL)
#define SPI_CRCCALCULATION_ENABLE                     SPI_CFG1_CRCEN
/**
  * @}
  */

/** @defgroup SPI_CRC_length SPI CRC Length
  * @{
  */
#define SPI_CRC_LENGTH_DATASIZE                       (0x00000000UL)
#define SPI_CRC_LENGTH_4BIT                           (0x00030000UL)
#define SPI_CRC_LENGTH_5BIT                           (0x00040000UL)
#define SPI_CRC_LENGTH_6BIT                           (0x00050000UL)
#define SPI_CRC_LENGTH_7BIT                           (0x00060000UL)
#define SPI_CRC_LENGTH_8BIT                           (0x00070000UL)
#define SPI_CRC_LENGTH_9BIT                           (0x00080000UL)
#define SPI_CRC_LENGTH_10BIT                          (0x00090000UL)
#define SPI_CRC_LENGTH_11BIT                          (0x000A0000UL)
#define SPI_CRC_LENGTH_12BIT                          (0x000B0000UL)
#define SPI_CRC_LENGTH_13BIT                          (0x000C0000UL)
#define SPI_CRC_LENGTH_14BIT                          (0x000D0000UL)
#define SPI_CRC_LENGTH_15BIT                          (0x000E0000UL)
#define SPI_CRC_LENGTH_16BIT                          (0x000F0000UL)
#define SPI_CRC_LENGTH_17BIT                          (0x00100000UL)
#define SPI_CRC_LENGTH_18BIT                          (0x00110000UL)
#define SPI_CRC_LENGTH_19BIT                          (0x00120000UL)
#define SPI_CRC_LENGTH_20BIT                          (0x00130000UL)
#define SPI_CRC_LENGTH_21BIT                          (0x00140000UL)
#define SPI_CRC_LENGTH_22BIT                          (0x00150000UL)
#define SPI_CRC_LENGTH_23BIT                          (0x00160000UL)
#define SPI_CRC_LENGTH_24BIT                          (0x00170000UL)
#define SPI_CRC_LENGTH_25BIT                          (0x00180000UL)
#define SPI_CRC_LENGTH_26BIT                          (0x00190000UL)
#define SPI_CRC_LENGTH_27BIT                          (0x001A0000UL)
#define SPI_CRC_LENGTH_28BIT                          (0x001B0000UL)
#define SPI_CRC_LENGTH_29BIT                          (0x001C0000UL)
#define SPI_CRC_LENGTH_30BIT                          (0x001D0000UL)
#define SPI_CRC_LENGTH_31BIT                          (0x001E0000UL)
#define SPI_CRC_LENGTH_32BIT                          (0x001F0000UL)
/**
  * @}
  */

/** @defgroup SPI_Fifo_Threshold SPI Fifo Threshold
  * @{
  */
#define SPI_FIFO_THRESHOLD_01DATA                     (0x00000000UL)
#define SPI_FIFO_THRESHOLD_02DATA                     (0x00000020UL)
#define SPI_FIFO_THRESHOLD_03DATA                     (0x00000040UL)
#define SPI_FIFO_THRESHOLD_04DATA                     (0x00000060UL)
#define SPI_FIFO_THRESHOLD_05DATA                     (0x00000080UL)
#define SPI_FIFO_THRESHOLD_06DATA                     (0x000000A0UL)
#define SPI_FIFO_THRESHOLD_07DATA                     (0x000000C0UL)
#define SPI_FIFO_THRESHOLD_08DATA                     (0x000000E0UL)
#define SPI_FIFO_THRESHOLD_09DATA                     (0x00000100UL)
#define SPI_FIFO_THRESHOLD_10DATA                     (0x00000120UL)
#define SPI_FIFO_THRESHOLD_11DATA                     (0x00000140UL)
#define SPI_FIFO_THRESHOLD_12DATA                     (0x00000160UL)
#define SPI_FIFO_THRESHOLD_13DATA                     (0x00000180UL)
#define SPI_FIFO_THRESHOLD_14DATA                     (0x000001A0UL)
#define SPI_FIFO_THRESHOLD_15DATA                     (0x000001C0UL)
#define SPI_FIFO_THRESHOLD_16DATA                     (0x000001E0UL)
/**
  * @}
  */

/** @defgroup SPI_CRC_Calculation_Initialization_Pattern SPI CRC Calculation Initialization Pattern
  * @{
  */
#define SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN       (0x00000000UL)
#define SPI_CRC_INITIALIZATION_ALL_ONE_PATTERN        (0x00000001UL)
/**
  * @}
  */

/** @defgroup SPI_NSS_Polarity SPI NSS Polarity
  * @{
  */
#define SPI_NSS_POLARITY_LOW                          (0x00000000UL)
#define SPI_NSS_POLARITY_HIGH                          SPI_CFG2_SSIOP
/**
  * @}
  */

/** @defgroup SPI_Master_Keep_IO_State Keep IO State
  * @{
  */
#define SPI_MASTER_KEEP_IO_STATE_DISABLE              (0x00000000UL)
#define SPI_MASTER_KEEP_IO_STATE_ENABLE               SPI_CFG2_AFCNTR
/**
  * @}
  */

/** @defgroup SPI_IO_Swap Control SPI IO Swap
  * @{
  */
#define SPI_IO_SWAP_DISABLE                           (0x00000000UL)
#define SPI_IO_SWAP_ENABLE                            SPI_CFG2_IOSWP
/**
  * @}
  */

/** @defgroup SPI_Master_SS_Idleness SPI Master SS Idleness
  * @{
  */
#define SPI_MASTER_SS_IDLENESS_00CYCLE                (0x00000000UL)
#define SPI_MASTER_SS_IDLENESS_01CYCLE                (0x00000001UL)
#define SPI_MASTER_SS_IDLENESS_02CYCLE                (0x00000002UL)
#define SPI_MASTER_SS_IDLENESS_03CYCLE                (0x00000003UL)
#define SPI_MASTER_SS_IDLENESS_04CYCLE                (0x00000004UL)
#define SPI_MASTER_SS_IDLENESS_05CYCLE                (0x00000005UL)
#define SPI_MASTER_SS_IDLENESS_06CYCLE                (0x00000006UL)
#define SPI_MASTER_SS_IDLENESS_07CYCLE                (0x00000007UL)
#define SPI_MASTER_SS_IDLENESS_08CYCLE                (0x00000008UL)
#define SPI_MASTER_SS_IDLENESS_09CYCLE                (0x00000009UL)
#define SPI_MASTER_SS_IDLENESS_10CYCLE                (0x0000000AUL)
#define SPI_MASTER_SS_IDLENESS_11CYCLE                (0x0000000BUL)
#define SPI_MASTER_SS_IDLENESS_12CYCLE                (0x0000000CUL)
#define SPI_MASTER_SS_IDLENESS_13CYCLE                (0x0000000DUL)
#define SPI_MASTER_SS_IDLENESS_14CYCLE                (0x0000000EUL)
#define SPI_MASTER_SS_IDLENESS_15CYCLE                (0x0000000FUL)
/**
  * @}
  */

/** @defgroup SPI_Master_InterData_Idleness SPI Master Inter-Data Idleness
  * @{
  */
#define SPI_MASTER_INTERDATA_IDLENESS_00CYCLE         (0x00000000UL)
#define SPI_MASTER_INTERDATA_IDLENESS_01CYCLE         (0x00000010UL)
#define SPI_MASTER_INTERDATA_IDLENESS_02CYCLE         (0x00000020UL)
#define SPI_MASTER_INTERDATA_IDLENESS_03CYCLE         (0x00000030UL)
#define SPI_MASTER_INTERDATA_IDLENESS_04CYCLE         (0x00000040UL)
#define SPI_MASTER_INTERDATA_IDLENESS_05CYCLE         (0x00000050UL)
#define SPI_MASTER_INTERDATA_IDLENESS_06CYCLE         (0x00000060UL)
#define SPI_MASTER_INTERDATA_IDLENESS_07CYCLE         (0x00000070UL)
#define SPI_MASTER_INTERDATA_IDLENESS_08CYCLE         (0x00000080UL)
#define SPI_MASTER_INTERDATA_IDLENESS_09CYCLE         (0x00000090UL)
#define SPI_MASTER_INTERDATA_IDLENESS_10CYCLE         (0x000000A0UL)
#define SPI_MASTER_INTERDATA_IDLENESS_11CYCLE         (0x000000B0UL)
#define SPI_MASTER_INTERDATA_IDLENESS_12CYCLE         (0x000000C0UL)
#define SPI_MASTER_INTERDATA_IDLENESS_13CYCLE         (0x000000D0UL)
#define SPI_MASTER_INTERDATA_IDLENESS_14CYCLE         (0x000000E0UL)
#define SPI_MASTER_INTERDATA_IDLENESS_15CYCLE         (0x000000F0UL)
/**
  * @}
  */

/** @defgroup SPI_Master_RX_AutoSuspend SPI Master Receiver AutoSuspend
  * @{
  */
#define SPI_MASTER_RX_AUTOSUSP_DISABLE                (0x00000000UL)
#define SPI_MASTER_RX_AUTOSUSP_ENABLE                 SPI_CR1_MASRX
/**
  * @}
  */

/** @defgroup SPI_Underrun_Behaviour SPI Underrun Behavior
  * @{
  */
#define SPI_UNDERRUN_BEHAV_REGISTER_PATTERN           (0x00000000UL)
#define SPI_UNDERRUN_BEHAV_LAST_RECEIVED              SPI_CFG1_UDRCFG
/**
  * @}
  */

/** @defgroup SPI_RDY_Master_Management SPI RDY Signal Input Master Management
  * @{
  */
#define SPI_RDY_MASTER_MANAGEMENT_INTERNALLY          (0x00000000UL)
#define SPI_RDY_MASTER_MANAGEMENT_EXTERNALLY          SPI_CFG2_RDIOM
/**
  * @}
  */

/** @defgroup SPI_RDY_Polarity SPI RDY Signal Input/Output Polarity
  * @{
  */
#define SPI_RDY_POLARITY_HIGH                         (0x00000000UL)
#define SPI_RDY_POLARITY_LOW                          SPI_CFG2_RDIOP
/**
  * @}
  */

/** @defgroup SPI_Interrupt_definition SPI Interrupt Definition
  * @{
  */
#define SPI_IT_RXP                      SPI_IER_RXPIE
#define SPI_IT_TXP                      SPI_IER_TXPIE
#define SPI_IT_DXP                      SPI_IER_DXPIE
#define SPI_IT_EOT                      SPI_IER_EOTIE
#define SPI_IT_TXTF                     SPI_IER_TXTFIE
#define SPI_IT_UDR                      SPI_IER_UDRIE
#define SPI_IT_OVR                      SPI_IER_OVRIE
#define SPI_IT_CRCERR                   SPI_IER_CRCEIE
#define SPI_IT_FRE                      SPI_IER_TIFREIE
#define SPI_IT_MODF                     SPI_IER_MODFIE
#define SPI_IT_ERR                      (SPI_IT_UDR | SPI_IT_OVR | SPI_IT_FRE | SPI_IT_MODF | SPI_IT_CRCERR)
/**
  * @}
  */

/** @defgroup SPI_Flags_definition SPI Flags Definition
  * @{
  */
#define SPI_FLAG_RXP                    SPI_SR_RXP     /* SPI status flag : Rx-Packet available flag                 */
#define SPI_FLAG_TXP                    SPI_SR_TXP     /* SPI status flag : Tx-Packet space available flag           */
#define SPI_FLAG_DXP                    SPI_SR_DXP     /* SPI status flag : Duplex Packet flag                       */
#define SPI_FLAG_EOT                    SPI_SR_EOT     /* SPI status flag : End of transfer flag                     */
#define SPI_FLAG_TXTF                   SPI_SR_TXTF    /* SPI status flag : Transmission Transfer Filled flag        */
#define SPI_FLAG_UDR                    SPI_SR_UDR     /* SPI Error flag  : Underrun flag                            */
#define SPI_FLAG_OVR                    SPI_SR_OVR     /* SPI Error flag  : Overrun flag                             */
#define SPI_FLAG_CRCERR                 SPI_SR_CRCE    /* SPI Error flag  : CRC error flag                           */
#define SPI_FLAG_FRE                    SPI_SR_TIFRE   /* SPI Error flag  : TI mode frame format error flag          */
#define SPI_FLAG_MODF                   SPI_SR_MODF    /* SPI Error flag  : Mode fault flag                          */
#define SPI_FLAG_SUSP                   SPI_SR_SUSP    /* SPI status flag : Transfer suspend complete flag           */
#define SPI_FLAG_TXC                    SPI_SR_TXC     /* SPI status flag : TxFIFO transmission complete flag        */
#define SPI_FLAG_FRLVL                  SPI_SR_RXPLVL  /* SPI status flag : Fifo reception level flag                */
#define SPI_FLAG_RXWNE                  SPI_SR_RXWNE   /* SPI status flag : RxFIFO word not empty flag               */
/**
  * @}
  */

/** @defgroup SPI_reception_fifo_status_level SPI Reception FIFO Status Level
  * @{
  */
#define SPI_RX_FIFO_0PACKET             (0x00000000UL)         /* 0 or multiple of 4 packets available in the RxFIFO */
#define SPI_RX_FIFO_1PACKET             (SPI_SR_RXPLVL_0)
#define SPI_RX_FIFO_2PACKET             (SPI_SR_RXPLVL_1)
#define SPI_RX_FIFO_3PACKET             (SPI_SR_RXPLVL_1 | SPI_SR_RXPLVL_0)
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/** @defgroup SPI_Exported_Macros SPI Exported Macros
  * @{
  */

/** @brief  Reset SPI handle state.
  * @param  __HANDLE__: specifies the SPI Handle.
  *         This parameter can be SPI where x: 1, 2, 3, 4, 5 or 6 to select the SPI peripheral.
  * @retval None
  */
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
#define __HAL_SPI_RESET_HANDLE_STATE(__HANDLE__)   do{                                                  \
                                                       (__HANDLE__)->State = HAL_SPI_STATE_RESET;       \
                                                       (__HANDLE__)->MspInitCallback = NULL;            \
                                                       (__HANDLE__)->MspDeInitCallback = NULL;          \
                                                     } while(0)
#else
#define __HAL_SPI_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_SPI_STATE_RESET)
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */

/** @brief  Enable the specified SPI interrupts.
  * @param  __HANDLE__: specifies the SPI Handle.
  *         This parameter can be SPI where x: 1, 2, 3, 4, 5 or 6 to select the SPI peripheral.
  * @param  __INTERRUPT__: specifies the interrupt source to enable or disable.
  *         This parameter can be one of the following values:
  *            @arg SPI_IT_RXP    : Rx-Packet available interrupt
  *            @arg SPI_IT_TXP    : Tx-Packet space available interrupt
  *            @arg SPI_IT_DXP    : Duplex Packet interrupt
  *            @arg SPI_IT_EOT    : End of transfer interrupt
  *            @arg SPI_IT_TXTF   : Transmission Transfer Filled interrupt
  *            @arg SPI_IT_UDR    : Underrun interrupt
  *            @arg SPI_IT_OVR    : Overrun  interrupt
  *            @arg SPI_IT_CRCERR : CRC error interrupt
  *            @arg SPI_IT_FRE    : TI mode frame format error interrupt
  *            @arg SPI_IT_MODF   : Mode fault interrupt
  *            @arg SPI_IT_ERR    : Error interrupt
  * @retval None
  */
#define __HAL_SPI_ENABLE_IT(__HANDLE__, __INTERRUPT__)   ((__HANDLE__)->Instance->IER |= (__INTERRUPT__))

/** @brief  Disable the specified SPI interrupts.
  * @param  __HANDLE__: specifies the SPI Handle.
  *         This parameter can be SPI where x: 1, 2, 3, 4, 5 or 6 to select the SPI peripheral.
  * @param  __INTERRUPT__: specifies the interrupt source to enable or disable.
  *         This parameter can be one of the following values:
  *            @arg SPI_IT_RXP    : Rx-Packet available interrupt
  *            @arg SPI_IT_TXP    : Tx-Packet space available interrupt
  *            @arg SPI_IT_DXP    : Duplex Packet interrupt
  *            @arg SPI_IT_EOT    : End of transfer interrupt
  *            @arg SPI_IT_TXTF   : Transmission Transfer Filled interrupt
  *            @arg SPI_IT_UDR    : Underrun interrupt
  *            @arg SPI_IT_OVR    : Overrun  interrupt
  *            @arg SPI_IT_CRCERR : CRC error interrupt
  *            @arg SPI_IT_FRE    : TI mode frame format error interrupt
  *            @arg SPI_IT_MODF   : Mode fault interrupt
  *            @arg SPI_IT_ERR    : Error interrupt
  * @retval None
  */
#define __HAL_SPI_DISABLE_IT(__HANDLE__, __INTERRUPT__)  ((__HANDLE__)->Instance->IER &= (~(__INTERRUPT__)))

/** @brief  Check whether the specified SPI interrupt source is enabled or not.
  * @param  __HANDLE__: specifies the SPI Handle.
  *         This parameter can be SPI where x: 1, 2, 3, 4, 5 or 6 to select the SPI peripheral.
  * @param  __INTERRUPT__: specifies the SPI interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg SPI_IT_RXP    : Rx-Packet available interrupt
  *            @arg SPI_IT_TXP    : Tx-Packet space available interrupt
  *            @arg SPI_IT_DXP    : Duplex Packet interrupt
  *            @arg SPI_IT_EOT    : End of transfer interrupt
  *            @arg SPI_IT_TXTF   : Transmission Transfer Filled interrupt
  *            @arg SPI_IT_UDR    : Underrun interrupt
  *            @arg SPI_IT_OVR    : Overrun  interrupt
  *            @arg SPI_IT_CRCERR : CRC error interrupt
  *            @arg SPI_IT_FRE    : TI mode frame format error interrupt
  *            @arg SPI_IT_MODF   : Mode fault interrupt
  *            @arg SPI_IT_ERR    : Error interrupt
  * @retval The new state of __IT__ (TRUE or FALSE).
  */
#define __HAL_SPI_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->IER & \
                                                              (__INTERRUPT__)) == (__INTERRUPT__)) ? SET : RESET)

/** @brief  Check whether the specified SPI flag is set or not.
  * @param  __HANDLE__: specifies the SPI Handle.
  *         This parameter can be SPI where x: 1, 2, 3, 4, 5 or 6 to select the SPI peripheral.
  * @param  __FLAG__: specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg SPI_FLAG_RXP    : Rx-Packet available flag
  *            @arg SPI_FLAG_TXP    : Tx-Packet space available flag
  *            @arg SPI_FLAG_DXP    : Duplex Packet flag
  *            @arg SPI_FLAG_EOT    : End of transfer flag
  *            @arg SPI_FLAG_TXTF   : Transmission Transfer Filled flag
  *            @arg SPI_FLAG_UDR    : Underrun flag
  *            @arg SPI_FLAG_OVR    : Overrun flag
  *            @arg SPI_FLAG_CRCERR : CRC error flag
  *            @arg SPI_FLAG_FRE    : TI mode frame format error flag
  *            @arg SPI_FLAG_MODF   : Mode fault flag
  *            @arg SPI_FLAG_SUSP   : Transfer suspend complete flag
  *            @arg SPI_FLAG_TXC    : TxFIFO transmission complete flag
  *            @arg SPI_FLAG_FRLVL  : Fifo reception level flag
  *            @arg SPI_FLAG_RXWNE  : RxFIFO word not empty flag
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_SPI_GET_FLAG(__HANDLE__, __FLAG__) ((((__HANDLE__)->Instance->SR) & (__FLAG__)) == (__FLAG__))

/** @brief  Clear the SPI CRCERR pending flag.
  * @param  __HANDLE__: specifies the SPI Handle.
  * @retval None
  */
#define __HAL_SPI_CLEAR_CRCERRFLAG(__HANDLE__) SET_BIT((__HANDLE__)->Instance->IFCR , SPI_IFCR_CRCEC)

/** @brief  Clear the SPI MODF pending flag.
  * @param  __HANDLE__: specifies the SPI Handle.
  * @retval None
  */
#define __HAL_SPI_CLEAR_MODFFLAG(__HANDLE__)  SET_BIT((__HANDLE__)->Instance->IFCR , (uint32_t)(SPI_IFCR_MODFC));

/** @brief  Clear the SPI OVR pending flag.
  * @param  __HANDLE__: specifies the SPI Handle.
  * @retval None
  */
#define __HAL_SPI_CLEAR_OVRFLAG(__HANDLE__) SET_BIT((__HANDLE__)->Instance->IFCR , SPI_IFCR_OVRC)

/** @brief  Clear the SPI FRE pending flag.
  * @param  __HANDLE__: specifies the SPI Handle.
  * @retval None
  */
#define __HAL_SPI_CLEAR_FREFLAG(__HANDLE__) SET_BIT((__HANDLE__)->Instance->IFCR , SPI_IFCR_TIFREC)

/** @brief  Clear the SPI UDR pending flag.
  * @param  __HANDLE__: specifies the SPI Handle.
  * @retval None
  */
#define __HAL_SPI_CLEAR_UDRFLAG(__HANDLE__) SET_BIT((__HANDLE__)->Instance->IFCR , SPI_IFCR_UDRC)

/** @brief  Clear the SPI EOT pending flag.
  * @param  __HANDLE__: specifies the SPI Handle.
  * @retval None
  */
#define __HAL_SPI_CLEAR_EOTFLAG(__HANDLE__) SET_BIT((__HANDLE__)->Instance->IFCR , SPI_IFCR_EOTC)

/** @brief  Clear the SPI UDR pending flag.
  * @param  __HANDLE__: specifies the SPI Handle.
  * @retval None
  */
#define __HAL_SPI_CLEAR_TXTFFLAG(__HANDLE__) SET_BIT((__HANDLE__)->Instance->IFCR , SPI_IFCR_TXTFC)

/** @brief  Clear the SPI SUSP pending flag.
  * @param  __HANDLE__: specifies the SPI Handle.
  * @retval None
  */
#define __HAL_SPI_CLEAR_SUSPFLAG(__HANDLE__) SET_BIT((__HANDLE__)->Instance->IFCR , SPI_IFCR_SUSPC)

/** @brief  Enable the SPI peripheral.
  * @param  __HANDLE__: specifies the SPI Handle.
  * @retval None
  */
#define __HAL_SPI_ENABLE(__HANDLE__) SET_BIT((__HANDLE__)->Instance->CR1 , SPI_CR1_SPE)

/** @brief  Disable the SPI peripheral.
  * @param  __HANDLE__: specifies the SPI Handle.
  * @retval None
  */
#define __HAL_SPI_DISABLE(__HANDLE__) CLEAR_BIT((__HANDLE__)->Instance->CR1 , SPI_CR1_SPE)
/**
  * @}
  */


/* Include SPI HAL Extension module */
#include "stm32n6xx_hal_spi_ex.h"


/* Exported functions --------------------------------------------------------*/
/** @addtogroup SPI_Exported_Functions
  * @{
  */

/** @addtogroup SPI_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */
/* Initialization/de-initialization functions  ********************************/
HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_DeInit(SPI_HandleTypeDef *hspi);
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi);
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi);

/* Callbacks Register/UnRegister functions  ***********************************/
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
HAL_StatusTypeDef HAL_SPI_RegisterCallback(SPI_HandleTypeDef *hspi, HAL_SPI_CallbackIDTypeDef CallbackID,
                                           pSPI_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_SPI_UnRegisterCallback(SPI_HandleTypeDef *hspi, HAL_SPI_CallbackIDTypeDef CallbackID);
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
/**
  * @}
  */

/** @addtogroup SPI_Exported_Functions_Group2 IO operation functions
  * @{
  */
/* I/O operation functions  ***************************************************/
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, const uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, const uint8_t *pTxData, uint8_t *pRxData,
                                          uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SPI_Transmit_IT(SPI_HandleTypeDef *hspi, const uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_Receive_IT(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_TransmitReceive_IT(SPI_HandleTypeDef *hspi, const uint8_t *pTxData, uint8_t *pRxData,
                                             uint16_t Size);

HAL_StatusTypeDef HAL_SPI_Transmit_DMA(SPI_HandleTypeDef *hspi, const uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_Receive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_TransmitReceive_DMA(SPI_HandleTypeDef *hspi, const uint8_t *pTxData, uint8_t *pRxData,
                                              uint16_t Size);


HAL_StatusTypeDef HAL_SPI_DMAPause(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_DMAResume(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_DMAStop(SPI_HandleTypeDef *hspi);

/* Transfer Abort functions */
HAL_StatusTypeDef HAL_SPI_Abort(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_Abort_IT(SPI_HandleTypeDef *hspi);

void HAL_SPI_IRQHandler(SPI_HandleTypeDef *hspi);
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_SuspendCallback(SPI_HandleTypeDef *hspi);
/**
  * @}
  */

/** @addtogroup SPI_Exported_Functions_Group3 Peripheral State and Errors functions
  * @{
  */

/* Peripheral State and Error functions ***************************************/
HAL_SPI_StateTypeDef HAL_SPI_GetState(const SPI_HandleTypeDef *hspi);
uint32_t             HAL_SPI_GetError(const SPI_HandleTypeDef *hspi);
/**
  * @}
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup SPI_Private_Macros SPI Private Macros
  * @{
  */

/** @brief  Set the SPI transmit-only mode in 1Line configuration.
  * @param  __HANDLE__: specifies the SPI Handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @retval None
  */
#define SPI_1LINE_TX(__HANDLE__) SET_BIT((__HANDLE__)->Instance->CR1, SPI_CR1_HDDIR)

/** @brief  Set the SPI receive-only mode in 1Line configuration.
  * @param  __HANDLE__: specifies the SPI Handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @retval None
  */
#define SPI_1LINE_RX(__HANDLE__) CLEAR_BIT((__HANDLE__)->Instance->CR1, SPI_CR1_HDDIR)

/** @brief  Set the SPI transmit-only mode in 2Lines configuration.
  * @param  __HANDLE__: specifies the SPI Handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @retval None
  */
#define SPI_2LINES_TX(__HANDLE__) MODIFY_REG((__HANDLE__)->Instance->CFG2, SPI_CFG2_COMM, SPI_CFG2_COMM_0)

/** @brief  Set the SPI receive-only mode in 2Lines configuration.
  * @param  __HANDLE__: specifies the SPI Handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @retval None
  */
#define SPI_2LINES_RX(__HANDLE__) MODIFY_REG((__HANDLE__)->Instance->CFG2, SPI_CFG2_COMM, SPI_CFG2_COMM_1)

/** @brief  Set the SPI Transmit-Receive mode in 2Lines configuration.
  * @param  __HANDLE__: specifies the SPI Handle.
  *         This parameter can be SPI where x: 1, 2, or 3 to select the SPI peripheral.
  * @retval None
  */
#define SPI_2LINES(__HANDLE__) MODIFY_REG((__HANDLE__)->Instance->CFG2, SPI_CFG2_COMM, 0x00000000UL)

#define IS_SPI_MODE(MODE)                          (((MODE) == SPI_MODE_SLAVE) || \
                                                    ((MODE) == SPI_MODE_MASTER))

#define IS_SPI_DIRECTION(MODE)                     (((MODE) == SPI_DIRECTION_2LINES)        || \
                                                    ((MODE) == SPI_DIRECTION_2LINES_RXONLY) || \
                                                    ((MODE) == SPI_DIRECTION_1LINE)         || \
                                                    ((MODE) == SPI_DIRECTION_2LINES_TXONLY))

#define IS_SPI_DIRECTION_2LINES(MODE) ((MODE) == SPI_DIRECTION_2LINES)

#define IS_SPI_DIRECTION_2LINES_OR_1LINE_2LINES_TXONLY(MODE) (((MODE) == SPI_DIRECTION_2LINES)|| \
                                                              ((MODE) == SPI_DIRECTION_1LINE) || \
                                                              ((MODE) == SPI_DIRECTION_2LINES_TXONLY))

#define IS_SPI_DIRECTION_2LINES_OR_1LINE_2LINES_RXONLY(MODE) (((MODE) == SPI_DIRECTION_2LINES)|| \
                                                              ((MODE) == SPI_DIRECTION_1LINE) || \
                                                              ((MODE) == SPI_DIRECTION_2LINES_RXONLY))

#define IS_SPI_DATASIZE(DATASIZE)                  (((DATASIZE) == SPI_DATASIZE_32BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_31BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_30BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_29BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_28BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_27BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_26BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_25BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_24BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_23BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_22BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_21BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_20BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_22BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_19BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_18BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_17BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_16BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_15BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_14BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_13BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_12BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_11BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_10BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_9BIT)  || \
                                                    ((DATASIZE) == SPI_DATASIZE_8BIT)  || \
                                                    ((DATASIZE) == SPI_DATASIZE_7BIT)  || \
                                                    ((DATASIZE) == SPI_DATASIZE_6BIT)  || \
                                                    ((DATASIZE) == SPI_DATASIZE_5BIT)  || \
                                                    ((DATASIZE) == SPI_DATASIZE_4BIT))

/**
  * @brief  DataSize for limited instance
  */
#define IS_SPI_LIMITED_DATASIZE(DATASIZE)          (((DATASIZE) == SPI_DATASIZE_16BIT) || \
                                                    ((DATASIZE) == SPI_DATASIZE_8BIT))

#define IS_SPI_FIFOTHRESHOLD(THRESHOLD)            (((THRESHOLD) == SPI_FIFO_THRESHOLD_01DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_02DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_03DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_04DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_05DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_06DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_07DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_08DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_09DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_10DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_11DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_12DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_13DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_14DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_15DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_16DATA))

/**
  * @brief  FifoThreshold for limited instance
  */
#define IS_SPI_LIMITED_FIFOTHRESHOLD(THRESHOLD)    (((THRESHOLD) == SPI_FIFO_THRESHOLD_01DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_02DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_03DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_04DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_05DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_06DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_07DATA) || \
                                                    ((THRESHOLD) == SPI_FIFO_THRESHOLD_08DATA))

#define IS_SPI_CPOL(CPOL)                          (((CPOL) == SPI_POLARITY_LOW) || \
                                                    ((CPOL) == SPI_POLARITY_HIGH))

#define IS_SPI_CPHA(CPHA)                          (((CPHA) == SPI_PHASE_1EDGE) || \
                                                    ((CPHA) == SPI_PHASE_2EDGE))

#define IS_SPI_NSS(NSS)                            (((NSS) == SPI_NSS_SOFT)       || \
                                                    ((NSS) == SPI_NSS_HARD_INPUT) || \
                                                    ((NSS) == SPI_NSS_HARD_OUTPUT))

#define IS_SPI_NSSP(NSSP)                          (((NSSP) == SPI_NSS_PULSE_ENABLE) || \
                                                    ((NSSP) == SPI_NSS_PULSE_DISABLE))

#define IS_SPI_BAUDRATE_PRESCALER(PRESCALER)       (((PRESCALER) == SPI_BAUDRATEPRESCALER_BYPASS) || \
                                                    ((PRESCALER) == SPI_BAUDRATEPRESCALER_2)      || \
                                                    ((PRESCALER) == SPI_BAUDRATEPRESCALER_4)      || \
                                                    ((PRESCALER) == SPI_BAUDRATEPRESCALER_8)      || \
                                                    ((PRESCALER) == SPI_BAUDRATEPRESCALER_16)     || \
                                                    ((PRESCALER) == SPI_BAUDRATEPRESCALER_32)     || \
                                                    ((PRESCALER) == SPI_BAUDRATEPRESCALER_64)     || \
                                                    ((PRESCALER) == SPI_BAUDRATEPRESCALER_128)    || \
                                                    ((PRESCALER) == SPI_BAUDRATEPRESCALER_256))

#define IS_SPI_FIRST_BIT(BIT)                      (((BIT) == SPI_FIRSTBIT_MSB) || \
                                                    ((BIT) == SPI_FIRSTBIT_LSB))

#define IS_SPI_TIMODE(MODE)                        (((MODE) == SPI_TIMODE_DISABLE) || \
                                                    ((MODE) == SPI_TIMODE_ENABLE))

#define IS_SPI_CRC_CALCULATION(CALCULATION)        (((CALCULATION) == SPI_CRCCALCULATION_DISABLE) || \
                                                    ((CALCULATION) == SPI_CRCCALCULATION_ENABLE))

#define IS_SPI_CRC_INITIALIZATION_PATTERN(PATTERN) (((PATTERN) == SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN) || \
                                                    ((PATTERN) == SPI_CRC_INITIALIZATION_ALL_ONE_PATTERN))

#define IS_SPI_CRC_LENGTH(LENGTH)                  (((LENGTH) == SPI_CRC_LENGTH_DATASIZE) || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_32BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_31BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_30BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_29BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_28BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_27BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_26BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_25BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_24BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_23BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_22BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_21BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_20BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_19BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_18BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_17BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_16BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_15BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_14BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_13BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_12BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_11BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_10BIT)    || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_9BIT)     || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_8BIT)     || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_7BIT)     || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_6BIT)     || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_5BIT)     || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_4BIT))


/**
  * @brief  CRC Length for limited instance
  */
#define IS_SPI_LIMITED_CRC_LENGTH(LENGTH)          (((LENGTH) == SPI_CRC_LENGTH_8BIT)     || \
                                                    ((LENGTH) == SPI_CRC_LENGTH_16BIT))


#define IS_SPI_CRC_POLYNOMIAL(POLYNOMIAL)          ((POLYNOMIAL) > 0x0UL)

#define IS_SPI_CRC_POLYNOMIAL_SIZE(POLYNOM, LENGTH) (((POLYNOM) >> (((LENGTH) >> SPI_CFG1_CRCSIZE_Pos) + 1UL)) == 0UL)



#define IS_SPI_UNDERRUN_BEHAVIOUR(MODE)            (((MODE) == SPI_UNDERRUN_BEHAV_REGISTER_PATTERN) || \
                                                    ((MODE) == SPI_UNDERRUN_BEHAV_LAST_RECEIVED))

#define IS_SPI_RDY_MASTER_MANAGEMENT(MANAGEMENT)   (((MANAGEMENT) == SPI_RDY_MASTER_MANAGEMENT_INTERNALLY) || \
                                                    ((MANAGEMENT) == SPI_RDY_MASTER_MANAGEMENT_EXTERNALLY))

#define IS_SPI_RDY_POLARITY(POLARITY)              (((POLARITY) == SPI_RDY_POLARITY_HIGH) || \
                                                    ((POLARITY) == SPI_RDY_POLARITY_LOW))

#define IS_SPI_MASTER_RX_AUTOSUSP(MODE)            (((MODE) == SPI_MASTER_RX_AUTOSUSP_DISABLE) || \
                                                    ((MODE) == SPI_MASTER_RX_AUTOSUSP_ENABLE))
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_HAL_SPI_H */

/**
  * @}
  */
