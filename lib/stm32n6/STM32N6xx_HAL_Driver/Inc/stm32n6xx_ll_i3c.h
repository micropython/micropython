/**
  ******************************************************************************
  * @file    stm32n6xx_ll_i3c.h
  * @author  MCD Application Team
  * @brief   Header file of I3C LL module.
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
#ifndef STM32N6xx_LL_I3C_H
#define STM32N6xx_LL_I3C_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx.h"

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */

#if defined (I3C1) || defined (I3C2)

/** @defgroup I3C_LL I3C
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
/** @defgroup I3C_LL_Private_Macros I3C Private Macros
  * @{
  */
/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup I3C_LL_ES_CONTROLLER_BUS_CONFIG_STRUCTURE_DEFINITION I3C Controller Bus Configuration Structure definition
  * @brief    I3C LL Controller Bus Configuration Structure definition
  * @{
  */
typedef struct
{
  uint32_t SDAHoldTime;         /*!< Specifies the I3C SDA hold time.
                                     This parameter must be a value of @ref I3C_LL_EC_SDA_HOLD_TIME                   */

  uint32_t WaitTime;            /*!< Specifies the time that the main and the new controllers should wait before
                                     issuing a start.
                                     This parameter must be a value of @ref I3C_LL_EC_OWN_ACTIVITY_STATE              */

  uint8_t SCLPPLowDuration;     /*!< Specifies the I3C SCL low duration in number of kernel clock cycles
                                     in I3C push-pull phases.
                                     This parameter must be a number between Min_Data=0 and Max_Data=0xFF.            */

  uint8_t SCLI3CHighDuration;   /*!< Specifies the I3C SCL high duration in number of kernel clock cycles,
                                     used for I3C messages for I3C open-drain and push pull phases.
                                     This parameter must be a number between Min_Data=0 and Max_Data=0xFF.            */

  uint8_t SCLODLowDuration;     /*!< Specifies the I3C SCL low duration in number of kernel clock cycles in
                                     open-drain phases, used for legacy I2C commands and for I3C open-drain phases.
                                     This parameter must be a number between Min_Data=0 and Max_Data=0xFF.            */

  uint8_t SCLI2CHighDuration;   /*!< Specifies the I3C SCL high duration in number of kernel clock cycles, used
                                     for legacy I2C commands.
                                     This parameter must be a number between Min_Data=0 and Max_Data=0xFF.            */

  uint8_t BusFreeDuration;      /*!< Specifies the I3C controller duration in number of kernel clock cycles, after
                                     a stop and before a start.
                                     This parameter must be a number between Min_Data=0 and Max_Data=0xFF.            */

  uint8_t BusIdleDuration;      /*!< Specifies the I3C controller duration in number of kernel clock cycles to be
                                     elapsed, after that both SDA and SCL are continuously high and stable
                                     before issuing a hot-join event.
                                     This parameter must be a number between Min_Data=0 and Max_Data=0xFF.            */
} LL_I3C_CtrlBusConfTypeDef;
/**
  * @}
  */

/** @defgroup I3C_LL_ES_TARGET_BUS_CONFIG_STRUCTURE_DEFINITION I3C Target Bus Configuration Structure definition
  * @brief    I3C LL Target Bus Configuration Structure definition
  * @{
  */
typedef struct
{
  uint8_t BusAvailableDuration; /*!< Specifies the I3C target duration in number of kernel clock cycles, when
                                     the SDA and the SCL are high for at least taval.
                                     This parameter must be a number between Min_Data=0 and Max_Data=0xFF.            */
} LL_I3C_TgtBusConfTypeDef;
/**
  * @}
  */
#if defined(USE_FULL_LL_DRIVER)

/** @defgroup I3C_LL_ES_INIT I3C Exported Init structure
  * @brief    I3C LL Init Structure definition
  * @{
  */
typedef struct
{
  LL_I3C_CtrlBusConfTypeDef CtrlBusCharacteristic; /*!< Specifies the I3C controller bus characteristic configuration
                                                        when Controller mode                                          */

  LL_I3C_TgtBusConfTypeDef  TgtBusCharacteristic;  /*!< Specifies the I3C target bus characteristic configuration
                                                        when Target mode                                              */

} LL_I3C_InitTypeDef;
/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/* Exported constants --------------------------------------------------------*/
/** @defgroup I3C_LL_Exported_Constants I3C Exported Constants
  * @{
  */

/** @defgroup I3C_LL_EC_GET_FLAG Get Flags Defines
  * @brief    Flags defines which can be used with LL_I3C_ReadReg function
  * @{
  */
#define LL_I3C_EVR_CFEF                    I3C_EVR_CFEF
#define LL_I3C_EVR_TXFEF                   I3C_EVR_TXFEF
#define LL_I3C_EVR_CFNFF                   I3C_EVR_CFNFF
#define LL_I3C_EVR_SFNEF                   I3C_EVR_SFNEF
#define LL_I3C_EVR_TXFNFF                  I3C_EVR_TXFNFF
#define LL_I3C_EVR_RXFNEF                  I3C_EVR_RXFNEF
#define LL_I3C_EVR_RXLASTF                 I3C_EVR_RXLASTF
#define LL_I3C_EVR_TXLASTF                 I3C_EVR_TXLASTF
#define LL_I3C_EVR_FCF                     I3C_EVR_FCF
#define LL_I3C_EVR_RXTGTENDF               I3C_EVR_RXTGTENDF
#define LL_I3C_EVR_ERRF                    I3C_EVR_ERRF
#define LL_I3C_EVR_IBIF                    I3C_EVR_IBIF
#define LL_I3C_EVR_IBIENDF                 I3C_EVR_IBIENDF
#define LL_I3C_EVR_CRF                     I3C_EVR_CRF
#define LL_I3C_EVR_CRUPDF                  I3C_EVR_CRUPDF
#define LL_I3C_EVR_HJF                     I3C_EVR_HJF
#define LL_I3C_EVR_WKPF                    I3C_EVR_WKPF
#define LL_I3C_EVR_GETF                    I3C_EVR_GETF
#define LL_I3C_EVR_STAF                    I3C_EVR_STAF
#define LL_I3C_EVR_DAUPDF                  I3C_EVR_DAUPDF
#define LL_I3C_EVR_MWLUPDF                 I3C_EVR_MWLUPDF
#define LL_I3C_EVR_MRLUPDF                 I3C_EVR_MRLUPDF
#define LL_I3C_EVR_RSTF                    I3C_EVR_RSTF
#define LL_I3C_EVR_ASUPDF                  I3C_EVR_ASUPDF
#define LL_I3C_EVR_INTUPDF                 I3C_EVR_INTUPDF
#define LL_I3C_EVR_DEFF                    I3C_EVR_DEFF
#define LL_I3C_EVR_GRPF                    I3C_EVR_GRPF
#define LL_I3C_SER_PERR                    I3C_SER_PERR
#define LL_I3C_SER_STALL                   I3C_SER_STALL
#define LL_I3C_SER_DOVR                    I3C_SER_DOVR
#define LL_I3C_SER_COVR                    I3C_SER_COVR
#define LL_I3C_SER_ANACK                   I3C_SER_ANACK
#define LL_I3C_SER_DNACK                   I3C_SER_DNACK
#define LL_I3C_SER_DERR                    I3C_SER_DERR
/**
  * @}
  */

/** @defgroup I3C_LL_EC_IT IT Defines
  * @brief    IT defines which can be used with LL_I3C_ReadReg and  LL_I3C_WriteReg functions
  * @{
  */
#define LL_I3C_IER_CFNFIE                  I3C_IER_CFNFIE
#define LL_I3C_IER_SFNEIE                  I3C_IER_SFNEIE
#define LL_I3C_IER_TXFNFIE                 I3C_IER_TXFNFIE
#define LL_I3C_IER_RXFNEIE                 I3C_IER_RXFNEIE
#define LL_I3C_IER_FCIE                    I3C_IER_FCIE
#define LL_I3C_IER_RXTGTENDIE              I3C_IER_RXTGTENDIE
#define LL_I3C_IER_ERRIE                   I3C_IER_ERRIE
#define LL_I3C_IER_IBIIE                   I3C_IER_IBIIE
#define LL_I3C_IER_IBIENDIE                I3C_IER_IBIENDIE
#define LL_I3C_IER_CRIE                    I3C_IER_CRIE
#define LL_I3C_IER_CRUPDIE                 I3C_IER_CRUPDIE
#define LL_I3C_IER_HJIE                    I3C_IER_HJIE
#define LL_I3C_IER_WKPIE                   I3C_IER_WKPIE
#define LL_I3C_IER_GETIE                   I3C_IER_GETIE
#define LL_I3C_IER_STAIE                   I3C_IER_STAIE
#define LL_I3C_IER_DAUPDIE                 I3C_IER_DAUPDIE
#define LL_I3C_IER_MWLUPDIE                I3C_IER_MWLUPDIE
#define LL_I3C_IER_MRLUPDIE                I3C_IER_MRLUPDIE
#define LL_I3C_IER_RSTIE                   I3C_IER_RSTIE
#define LL_I3C_IER_ASUPDIE                 I3C_IER_ASUPDIE
#define LL_I3C_IER_INTUPDIE                I3C_IER_INTUPDIE
#define LL_I3C_IER_DEFIE                   I3C_IER_DEFIE
#define LL_I3C_IER_GRPIE                   I3C_IER_GRPIE
/**
  * @}
  */

/** @defgroup I3C_LL_EC_MODE MODE
  * @{
  */
#define LL_I3C_MODE_CONTROLLER              I3C_CFGR_CRINIT         /*!< I3C Controller mode */
#define LL_I3C_MODE_TARGET                  0x00000000U             /*!< I3C Target (Controller capable) mode */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_DMA_REG_DATA DMA Register Data
  * @{
  */
#define LL_I3C_DMA_REG_DATA_TRANSMIT_BYTE   0x00000000U              /*!< Get address of data register used
                                                                          for transmission in Byte */
#define LL_I3C_DMA_REG_DATA_RECEIVE_BYTE    0x00000001U              /*!< Get address of data register used
                                                                          for reception in Byte */
#define LL_I3C_DMA_REG_DATA_TRANSMIT_WORD   0x00000002U              /*!< Get address of data register used for
                                                                          transmission in Word */
#define LL_I3C_DMA_REG_DATA_RECEIVE_WORD    0x00000003U              /*!< Get address of data register used
                                                                          for reception in Word */
#define LL_I3C_DMA_REG_STATUS               0x00000004U              /*!< Get address of status register used
                                                                          for transfer status in Word */
#define LL_I3C_DMA_REG_CONTROL              0x00000005U              /*!< Get address of control register used
                                                                          for transfer control in Word */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_RX_THRESHOLD RX THRESHOLD
  * @{
  */
#define LL_I3C_RXFIFO_THRESHOLD_1_4         0x00000000U
/*!< Rx Fifo Threshold is 1 byte in a Fifo depth of 4 bytes */
#define LL_I3C_RXFIFO_THRESHOLD_4_4         I3C_CFGR_RXTHRES
/*!< Rx Fifo Threshold is 4 bytes in a Fifo depth of 4 bytes */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_TX_THRESHOLD TX THRESHOLD
  * @{
  */
#define LL_I3C_TXFIFO_THRESHOLD_1_4         0x00000000U
/*!< Tx Fifo Threshold is 1 byte in a Fifo depth of 4 bytes */
#define LL_I3C_TXFIFO_THRESHOLD_4_4         I3C_CFGR_TXTHRES
/*!< Tx Fifo Threshold is 4 bytes in a Fifo depth of 4 bytes */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_PAYLOAD PAYLOAD
  * @{
  */
#define LL_I3C_PAYLOAD_EMPTY               0x00000000U
/*!< Empty payload, no additional data after IBI acknowledge */
#define LL_I3C_PAYLOAD_1_BYTE              I3C_MAXRLR_IBIP_0
/*!< One additional data byte after IBI acknowledge */
#define LL_I3C_PAYLOAD_2_BYTES             I3C_MAXRLR_IBIP_1
/*!< Two additional data bytes after IBI acknowledge */
#define LL_I3C_PAYLOAD_3_BYTES             (I3C_MAXRLR_IBIP_1 | I3C_MAXRLR_IBIP_0)
/*!< Three additional data bytes after IBI acknowledge */
#define LL_I3C_PAYLOAD_4_BYTES             I3C_MAXRLR_IBIP_2
/*!< Four additional data bytes after IBI acknowledge */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_SDA_HOLD_TIME SDA HOLD TIME 0
  * @{
  */
#define LL_I3C_SDA_HOLD_TIME_0_5           0x00000000U               /*!< SDA hold time is 0.5 x ti3cclk */
#define LL_I3C_SDA_HOLD_TIME_1_5           I3C_TIMINGR1_SDA_HD       /*!< SDA hold time is 1.5 x ti3cclk */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_OWN_ACTIVITY_STATE OWN ACTIVITY STATE
  * @{
  */
#define LL_I3C_OWN_ACTIVITY_STATE_0        0x00000000U
/*!< Own Controller Activity state 0 */
#define LL_I3C_OWN_ACTIVITY_STATE_1        I3C_TIMINGR1_ASNCR_0
/*!< Own Controller Activity state 1 */
#define LL_I3C_OWN_ACTIVITY_STATE_2        I3C_TIMINGR1_ASNCR_1
/*!< Own Controller Activity state 2 */
#define LL_I3C_OWN_ACTIVITY_STATE_3        (I3C_TIMINGR1_ASNCR_1 | I3C_TIMINGR1_ASNCR_0)
/*!< Own Controller Activity state 3 */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_DEVICE_ROLE_AS DEVICE ROLE AS
  * @{
  */
#define LL_I3C_DEVICE_ROLE_AS_TARGET        0x00000000U              /*!< I3C Target */
#define LL_I3C_DEVICE_ROLE_AS_CONTROLLER    I3C_BCR_BCR6             /*!< I3C Controller */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_IBI_NO_ADDITIONAL IBI NO ADDITIONAL
  * @{
  */
#define LL_I3C_IBI_NO_ADDITIONAL_DATA      0x00000000U               /*!< No data byte follows the accepted IBI */
#define LL_I3C_IBI_ADDITIONAL_DATA         I3C_BCR_BCR2              /*!< A Mandatory Data Byte (MDB)
                                                                          follows the accepted IBI */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_MAX_DATA_SPEED_LIMITATION MAX DATA SPEED LIMITATION
  * @{
  */
#define LL_I3C_NO_DATA_SPEED_LIMITATION    0x00000000U               /*!< No max data speed limitation */
#define LL_I3C_MAX_DATA_SPEED_LIMITATION   I3C_BCR_BCR0              /*!< Max data speed limitation */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_IBI_MDB_READ_NOTIFICATION IBI MDB READ NOTIFICATION
  * @{
  */
#define LL_I3C_MDB_NO_PENDING_READ_NOTIFICATION  0x00000000U
/*!< No support of pending read notification via the IBI MDB[7:0] value */
#define LL_I3C_MDB_PENDING_READ_NOTIFICATION     I3C_GETCAPR_CAPPEND
/*!< Support of pending read notification via the IBI MDB[7:0] value    */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_HANDOFF_GRP_ADDR_NOT HANDOFF GRP ADDR NOT
  * @{
  */
#define LL_I3C_HANDOFF_GRP_ADDR_NOT_SUPPORTED 0x00000000U            /*!< Group Address Handoff is not supported */
#define LL_I3C_HANDOFF_GRP_ADDR_SUPPORTED     I3C_CRCAPR_CAPGRP      /*!< Group Address Handoff is supported     */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_HANDOFF HANDOFF
  * @{
  */
#define LL_I3C_HANDOFF_NOT_DELAYED         0x00000000U
/*!< Additional time to process controllership handoff is not needed */
#define LL_I3C_HANDOFF_DELAYED             I3C_CRCAPR_CAPDHOFF
/*!< Additional time to process controllership handoff is needed */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_HANDOFF_ACTIVITY_STATE HANDOFF ACTIVITY STATE
  * @{
  */
#define LL_I3C_HANDOFF_ACTIVITY_STATE_0    0x00000000U
/*!< Indicates that will act according to Activity State 0 after controllership handoff */
#define LL_I3C_HANDOFF_ACTIVITY_STATE_1    I3C_GETMXDSR_HOFFAS_0
/*!< Indicates that will act according to Activity State 1 after controllership handoff */
#define LL_I3C_HANDOFF_ACTIVITY_STATE_2    I3C_GETMXDSR_HOFFAS_1
/*!< Indicates that will act according to Activity State 2 after controllership handoff */
#define LL_I3C_HANDOFF_ACTIVITY_STATE_3    (I3C_GETMXDSR_HOFFAS_1 | I3C_GETMXDSR_HOFFAS_0)
/*!< Indicates that will act according to Activity State 3 after controllership handoff */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_GETMXDS_FORMAT GETMXDS FORMAT
  * @{
  */
#define LL_I3C_GETMXDS_FORMAT_1            0x00000000U
/*!< GETMXDS CCC Format 1 is used, no MaxRdTurn field in response */
#define LL_I3C_GETMXDS_FORMAT_2_LSB        I3C_GETMXDSR_FMT_0
/*!< GETMXDS CCC Format 2 is used, MaxRdTurn field in response, LSB = RDTURN[7:0] */
#define LL_I3C_GETMXDS_FORMAT_2_MID        I3C_GETMXDSR_FMT_1
/*!< GETMXDS CCC Format 2 is used, MaxRdTurn field in response, Middle byte = RDTURN[7:0] */
#define LL_I3C_GETMXDS_FORMAT_2_MSB        (I3C_GETMXDSR_FMT_1 | I3C_GETMXDSR_FMT_0)
/*!< GETMXDS CCC Format 2 is used, MaxRdTurn field in response, MSB = RDTURN[7:0] */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_GETMXDS_TSCO GETMXDS TSCO
  * @{
  */
#define LL_I3C_TURNAROUND_TIME_TSCO_LESS_12NS     0x00000000U         /*!< clock-to-data turnaround time tSCO <= 12ns */
#define LL_I3C_TURNAROUND_TIME_TSCO_GREATER_12NS  I3C_GETMXDSR_TSCO   /*!< clock-to-data turnaround time tSCO > 12ns  */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_BUS_ACTIVITY_STATE BUS ACTIVITY STATE
  * @{
  */
#define LL_I3C_BUS_ACTIVITY_STATE_0        0x00000000U
/*!< Controller on the Bus Activity State 0 */
#define LL_I3C_BUS_ACTIVITY_STATE_1        I3C_DEVR0_AS_0
/*!< Controller on the Bus Activity State 1 */
#define LL_I3C_BUS_ACTIVITY_STATE_2        I3C_DEVR0_AS_1
/*!< Controller on the Bus Activity State 2 */
#define LL_I3C_BUS_ACTIVITY_STATE_3        (I3C_DEVR0_AS_1 | I3C_DEVR0_AS_0)
/*!< Controller on the Bus Activity State 3 */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_RESET_ACTION RESET ACTION
  * @{
  */
#define LL_I3C_RESET_ACTION_NONE           0x00000000U
/*!< No Reset Action Required */
#define LL_I3C_RESET_ACTION_PARTIAL        I3C_DEVR0_RSTACT_0
/*!< Reset of some internal registers of the peripheral*/
#define LL_I3C_RESET_ACTION_FULL           I3C_DEVR0_RSTACT_1
/*!< Reset all internal registers of the peripheral */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_DIRECTION DIRECTION
  * @{
  */
#define LL_I3C_DIRECTION_WRITE             0x00000000U               /*!< Write transfer      */
#define LL_I3C_DIRECTION_READ              I3C_CR_RNW                /*!< Read transfer       */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_GENERATE GENERATE
  * @{
  */
#define LL_I3C_GENERATE_STOP               I3C_CR_MEND
/*!< Generate Stop condition after sending a message */
#define LL_I3C_GENERATE_RESTART            0x00000000U
/*!< Generate Restart condition after sending a message */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_CONTROLLER_MTYPE CONTROLLER MTYPE
  * @{
  */
#define LL_I3C_CONTROLLER_MTYPE_RELEASE        0x00000000U
/*!< SCL output clock stops running until next instruction executed       */
#define LL_I3C_CONTROLLER_MTYPE_HEADER         I3C_CR_MTYPE_0
/*!< Header Message */
#define LL_I3C_CONTROLLER_MTYPE_PRIVATE        I3C_CR_MTYPE_1
/*!< Private Message Type */
#define LL_I3C_CONTROLLER_MTYPE_DIRECT         (I3C_CR_MTYPE_1 | I3C_CR_MTYPE_0)
/*!< Direct Message Type */
#define LL_I3C_CONTROLLER_MTYPE_LEGACY_I2C     I3C_CR_MTYPE_2
/*!< Legacy I2C Message Type */
#define LL_I3C_CONTROLLER_MTYPE_CCC            (I3C_CR_MTYPE_2 | I3C_CR_MTYPE_1)
/*!< Common Command Code */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_TARGET_MTYPE_HOT TARGET MTYPE HOT
  * @{
  */
#define LL_I3C_TARGET_MTYPE_HOT_JOIN            I3C_CR_MTYPE_3                     /*!< Hot Join*/
#define LL_I3C_TARGET_MTYPE_CONTROLLER_ROLE_REQ (I3C_CR_MTYPE_3 | I3C_CR_MTYPE_0)  /*!< Controller-role Request */
#define LL_I3C_TARGET_MTYPE_IBI                 (I3C_CR_MTYPE_3 | I3C_CR_MTYPE_1)  /*!< In Band Interrupt (IBI) */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_MESSAGE MESSAGE
  * @{
  */
#define LL_I3C_MESSAGE_ERROR               0x00000000U               /*!< An error has been detected in the message */
#define LL_I3C_MESSAGE_SUCCESS             I3C_SR_OK                 /*!< The message ended with success       */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_MESSAGE_DIRECTION MESSAGE DIRECTION
  * @{
  */
#define LL_I3C_MESSAGE_DIRECTION_WRITE     0x00000000U               /*!< Write data or command      */
#define LL_I3C_MESSAGE_DIRECTION_READ      I3C_SR_DIR                /*!< Read data       */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_CONTROLLER_ERROR CONTROLLER ERROR
  * @{
  */
#define LL_I3C_CONTROLLER_ERROR_CE0        0x00000000U
/*!< Controller detected an illegally formatted CCC    */
#define LL_I3C_CONTROLLER_ERROR_CE1        I3C_SER_CODERR_0
/*!< Controller detected that transmitted data on the bus is different than expected    */
#define LL_I3C_CONTROLLER_ERROR_CE2        I3C_SER_CODERR_1
/*!< Controller detected that broadcast address 7'h7E has been nacked    */
#define LL_I3C_CONTROLLER_ERROR_CE3        (I3C_SER_CODERR_1 | I3C_SER_CODERR_0)
/*!< Controller detected that new Controller did not drive the bus after Controller-role handoff    */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_TARGET_ERROR TARGET ERROR
  * @{
  */
#define LL_I3C_TARGET_ERROR_TE0            I3C_SER_CODERR_3
/*!< Target detected an invalid broadcast address    */
#define LL_I3C_TARGET_ERROR_TE1            (I3C_SER_CODERR_3  | I3C_SER_CODERR_0)
/*!< Target detected an invalid CCC Code    */
#define LL_I3C_TARGET_ERROR_TE2            (I3C_SER_CODERR_3  | I3C_SER_CODERR_1)
/*!< Target detected an invalid write data    */
#define LL_I3C_TARGET_ERROR_TE3            (I3C_SER_CODERR_3  | I3C_SER_CODERR_1 | I3C_SER_CODERR_0)
/*!< Target detected an invalid assigned address during Dynamic Address Assignment procedure    */
#define LL_I3C_TARGET_ERROR_TE4            (I3C_SER_CODERR_3  | I3C_SER_CODERR_2)
/*!< Target detected 7'h7E missing after Restart during Dynamic Address Assignment procedure    */
#define LL_I3C_TARGET_ERROR_TE5            (I3C_SER_CODERR_3  | I3C_SER_CODERR_2 | I3C_SER_CODERR_0)
/*!< Target detected an illegally formatted CCC     */
#define LL_I3C_TARGET_ERROR_TE6            (I3C_SER_CODERR_3  | I3C_SER_CODERR_2 | I3C_SER_CODERR_1)
/*!< Target detected that transmitted data on the bus is different than expected     */
/**
  * @}
  */

/** @defgroup I3C_BCR_IN_PAYLOAD I3C BCR IN PAYLOAD
  * @{
  */
#define LL_I3C_BCR_IN_PAYLOAD_SHIFT        48  /*!< BCR field in target payload */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_IBI_CAPABILITY IBI CAPABILITY
  * @{
  */
#define LL_I3C_IBI_CAPABILITY              I3C_DEVRX_IBIACK
/*!< Controller acknowledge Target In Band Interrupt capable */
#define LL_I3C_IBI_NO_CAPABILITY           0x00000000U
/*!< Controller no acknowledge Target In Band Interrupt capable */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_IBI_ADDITIONAL_DATA IBI ADDITIONAL DATA
  * @{
  */
#define LL_I3C_IBI_DATA_ENABLE             I3C_DEVRX_IBIDEN
/*!< A mandatory data byte follows the IBI acknowledgement */
#define LL_I3C_IBI_DATA_DISABLE            0x00000000U
/*!< No mandatory data byte follows the IBI acknowledgement */
/**
  * @}
  */

/** @defgroup I3C_LL_EC_CR_CAPABILITY CR CAPABILITY
  * @{
  */
#define LL_I3C_CR_CAPABILITY               I3C_DEVRX_CRACK
/*!< Controller acknowledge Target Controller Role capable */
#define LL_I3C_CR_NO_CAPABILITY            0x00000000U
/*!< Controller no acknowledge Target Controller Role capable */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup I3C_LL_Exported_Macros I3C Exported Macros
  * @{
  */

/** @defgroup I3C_LL_EM_WRITE_READ Common Write and read registers Macros
  * @{
  */

/** @brief  Get Bus Characterics in payload (64bits) receive during ENTDAA procedure.
  * @param  __PAYLOAD__ specifies the Bus Characteristics capabilities retrieve during ENTDAA procedure.
  *         This parameter must be a number between Min_Data=0x00(uint64_t) and Max_Data=0xFFFFFFFFFFFFFFFFFF.
  * @retval The value of BCR Return value between Min_Data=0x00 and Max_Data=0xFF.
  */
#define LL_I3C_GET_BCR(__PAYLOAD__) (((uint32_t)((uint64_t)(__PAYLOAD__) >> LL_I3C_BCR_IN_PAYLOAD_SHIFT)) & \
                                     I3C_BCR_BCR)

/** @brief  Check IBI request capabilities.
  * @param  __BCR__ specifies the Bus Characteristics capabilities retrieve during ENTDAA procedure.
  *         This parameter must be a number between Min_Data=0x00 and Max_Data=0xFF.
  * @retval Value of @ref I3C_LL_EC_IBI_CAPABILITY.
  */
#define LL_I3C_GET_IBI_CAPABLE(__BCR__) (((((__BCR__) & I3C_BCR_BCR1_Msk) >> I3C_BCR_BCR1_Pos) == 1U) \
                                         ? LL_I3C_IBI_CAPABILITY : LL_I3C_IBI_NO_CAPABILITY)

/** @brief  Check IBI additional data byte capabilities.
  * @param  __BCR__ specifies the Bus Characteristics capabilities retrieve during ENTDAA procedure.
  *         This parameter must be a number between Min_Data=0x00 and Max_Data=0xFF.
  * @retval Value of @ref I3C_LL_EC_IBI_ADDITIONAL_DATA.
  */
#define LL_I3C_GET_IBI_PAYLOAD(__BCR__) (((((__BCR__) & I3C_BCR_BCR2_Msk) >> I3C_BCR_BCR2_Pos) == 1U) \
                                         ? LL_I3C_IBI_DATA_ENABLE : LL_I3C_IBI_DATA_DISABLE)

/** @brief  Check Controller role request capabilities.
  * @param  __BCR__ specifies the Bus Characteristics capabilities retrieve during ENTDAA procedure.
  *         This parameter must be a number between Min_Data=0x00 and Max_Data=0xFF.
  * @retval Value of @ref I3C_LL_EC_CR_CAPABILITY.
  */
#define LL_I3C_GET_CR_CAPABLE(__BCR__) (((((__BCR__) & I3C_BCR_BCR6_Msk) >> I3C_BCR_BCR6_Pos) == 1U) \
                                        ? LL_I3C_CR_CAPABILITY : LL_I3C_CR_NO_CAPABILITY)

/**
  * @brief  Write a value in I3C register
  * @param  __INSTANCE__ I3C Instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_I3C_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/**
  * @brief  Read a value in I3C register
  * @param  __INSTANCE__ I3C Instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_I3C_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup I3C_LL_Exported_Functions I3C Exported Functions
  * @{
  */

/** @defgroup I3C_LL_EF_Configuration Configuration
  * @{
  */

/**
  * @brief  Enable I3C peripheral (EN = 1).
  * @rmtoll CFGR      EN            LL_I3C_Enable
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_Enable(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->CFGR, I3C_CFGR_EN);
}

/**
  * @brief  Disable I3C peripheral (EN = 0).
  * @note   Controller mode: before clearing EN, all possible target requests must be disabled using DISEC CCC.
  *         Target mode: software is not expected clearing EN unless a partial reset of the IP is needed
  * @rmtoll CFGR      EN            LL_I3C_Disable
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_Disable(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->CFGR, I3C_CFGR_EN);
}

/**
  * @brief  Check if the I3C peripheral is enabled or disabled.
  * @rmtoll CFGR      EN            LL_I3C_IsEnabled
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabled(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->CFGR, I3C_CFGR_EN) == (I3C_CFGR_EN)) ? 1UL : 0UL);
}

/**
  * @brief  Check if Reset action is required or not required.
  * @note   This bit indicates if Reset Action field has been updated by HW upon reception
  *         of RSTACT during current frame.
  * @rmtoll DEVR0        RSTVAL        LL_I3C_IsEnabledReset
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledReset(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->DEVR0, I3C_DEVR0_RSTVAL) == (I3C_DEVR0_RSTVAL)) ? 1UL : 0UL);
}

/**
  * @brief  Configure peripheral mode.
  * @note   This bit can only be programmed when the I3C is disabled (EN = 0).
  * @rmtoll CFGR      CRINIT       LL_I3C_SetMode
  * @param  I3Cx I3C Instance.
  * @param  PeripheralMode This parameter can be one of the following values:
  *         @arg @ref LL_I3C_MODE_CONTROLLER
  *         @arg @ref LL_I3C_MODE_TARGET
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetMode(I3C_TypeDef *I3Cx, uint32_t PeripheralMode)
{
  MODIFY_REG(I3Cx->CFGR, I3C_CFGR_CRINIT, PeripheralMode);
}

/**
  * @brief  Get peripheral mode.
  * @rmtoll CFGR      CRINIT       LL_I3C_GetMode
  * @param  I3Cx I3C Instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_I3C_MODE_CONTROLLER
  *         @arg @ref LL_I3C_MODE_TARGET
  */
__STATIC_INLINE uint32_t LL_I3C_GetMode(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)((READ_BIT(I3Cx->CFGR, I3C_CFGR_CRINIT) == (I3C_CFGR_CRINIT)) ? 1UL : 0UL);
}

/**
  * @brief  An arbitration header (7'h7E) is sent after Start in case of legacy I2C or I3C private transfers.
  * @note   This bit can be modified only when there is no frame ongoing
  * @rmtoll CFGR      NOARBH        LL_I3C_EnableArbitrationHeader
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableArbitrationHeader(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->CFGR, I3C_CFGR_NOARBH);
}

/**
  * @brief  Target address is sent directly after a Start in case of legacy I2C or I3C private transfers.
  * @note   This bit can be modified only when there is no frame ongoing
  * @rmtoll CFGR      NOARBH        LL_I3C_DisableArbitrationHeader
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableArbitrationHeader(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->CFGR, I3C_CFGR_NOARBH);
}

/**
  * @brief  Check if the arbitration header is enabled of disabled.
  * @rmtoll CFGR      NOARBH        LL_I3C_IsEnabledArbitrationHeader
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledArbitrationHeader(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->CFGR, I3C_CFGR_NOARBH) == (I3C_CFGR_NOARBH)) ? 0UL : 1UL);
}

/**
  * @brief  A Reset Pattern is inserted before the STOP at the end of a frame when the last CCC
  *         of the frame was RSTACT CCC.
  * @note   This bit can be modified only when there is no frame ongoing
  * @rmtoll CFGR      RSTPTRN       LL_I3C_EnableResetPattern
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableResetPattern(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->CFGR, I3C_CFGR_RSTPTRN);
}

/**
  * @brief  A single STOP is emitted at the end of a frame.
  * @note   This bit can be modified only when there is no frame ongoing
  * @rmtoll CFGR      RSTPTRN       LL_I3C_DisableResetPattern
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableResetPattern(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->CFGR, I3C_CFGR_RSTPTRN);
}

/**
  * @brief  Check if Reset Pattern is enabled of disabled.
  * @rmtoll CFGR      RSTPTRN       LL_I3C_IsEnabledResetPattern
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledResetPattern(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->CFGR, I3C_CFGR_RSTPTRN) == (I3C_CFGR_RSTPTRN)) ? 1UL : 0UL);
}

/**
  * @brief  An Exit Pattern is sent after header (MTYPE = header) to program an escalation fault.
  * @note   This bit can be modified only when there is no frame ongoing
  * @rmtoll CFGR      EXITPTRN      LL_I3C_EnableExitPattern
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableExitPattern(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->CFGR, I3C_CFGR_EXITPTRN);
}

/**
  * @brief  An Exit Pattern is not sent after header (MTYPE = header).
  * @note   This bit can be modified only when there is no frame ongoing
  * @rmtoll CFGR      EXITPTRN      LL_I3C_DisableExitPattern
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableExitPattern(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->CFGR, I3C_CFGR_EXITPTRN);
}

/**
  * @brief  Check if Exit Pattern is enabled or disabled.
  * @rmtoll CFGR      EXITPTRN      LL_I3C_IsEnabledExitPattern
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledExitPattern(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->CFGR, I3C_CFGR_EXITPTRN) == (I3C_CFGR_EXITPTRN)) ? 1UL : 0UL);
}

/**
  * @brief  High Keeper is enabled and will be used in place of standard Open drain Pull Up device
  *         during handoff procedures.
  * @note   This bit can only be programmed when the I3C is disabled (EN = 0).
  * @rmtoll CFGR      HKSDAEN       LL_I3C_EnableHighKeeperSDA
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableHighKeeperSDA(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->CFGR, I3C_CFGR_HKSDAEN);
}

/**
  * @brief  High Keeper is disabled.
  * @note   This bit can only be programmed when the I3C is disabled (EN = 0).
  * @rmtoll CFGR      HKSDAEN       LL_I3C_DisableHighKeeperSDA
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableHighKeeperSDA(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->CFGR, I3C_CFGR_HKSDAEN);
}

/**
  * @brief  Check if High Keeper is enabled or disabled.
  * @rmtoll CFGR      HKSDAEN       LL_I3C_IsEnabledHighKeeperSDA
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledHighKeeperSDA(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->CFGR, I3C_CFGR_HKSDAEN) == (I3C_CFGR_HKSDAEN)) ? 1UL : 0UL);
}

/**
  * @brief  Hot Join Request is Acked. Current frame on the bus is continued.
  *         An Hot Join interrupt is sent through HJF flag.
  * @note   This bit can be used when I3C is acting as a Controller.
  * @rmtoll CFGR      HJACK         LL_I3C_EnableHJAck
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableHJAck(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->CFGR, I3C_CFGR_HJACK);
}

/**
  * @brief  Hot Join Request is Nacked. Current frame on the bus is continued.
  *         No Hot Join interrupt is generated.
  * @note   This bit can be used when I3C is acting as a Controller.
  * @rmtoll CFGR      HJACK         LL_I3C_DisableHJAck
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableHJAck(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->CFGR, I3C_CFGR_HJACK);
}

/**
  * @brief  Check if Hot Join Request Acknowledgement is enabled or disabled.
  * @rmtoll CFGR      HJACK         LL_I3C_IsEnabledHJAck
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledHJAck(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->CFGR, I3C_CFGR_HJACK) == (I3C_CFGR_HJACK)) ? 1UL : 0UL);
}

/**
  * @brief  Get the data register address used for DMA transfer
  * @rmtoll TDR          TDB0         LL_I3C_DMA_GetRegAddr\n
  *         TDWR         TDWR          LL_I3C_DMA_GetRegAddr\n
  *         RDR          RXRB0         LL_I3C_DMA_GetRegAddr\n
  *         RDWR         RDWR          LL_I3C_DMA_GetRegAddr\n
  *         SR           SR            LL_I3C_DMA_GetRegAddr\n
  *         CR           CR            LL_I3C_DMA_GetRegAddr
  * @param  I3Cx I3C Instance
  * @param  Direction This parameter can be one of the following values:
  *         @arg @ref LL_I3C_DMA_REG_DATA_TRANSMIT_BYTE
  *         @arg @ref LL_I3C_DMA_REG_DATA_RECEIVE_BYTE
  *         @arg @ref LL_I3C_DMA_REG_DATA_TRANSMIT_WORD
  *         @arg @ref LL_I3C_DMA_REG_DATA_RECEIVE_WORD
  *         @arg @ref LL_I3C_DMA_REG_STATUS
  *         @arg @ref LL_I3C_DMA_REG_CONTROL
  * @retval Address of data register
  */
__STATIC_INLINE uint32_t LL_I3C_DMA_GetRegAddr(const I3C_TypeDef *I3Cx, uint32_t Direction)
{
  register uint32_t data_reg_addr;

  if (Direction == LL_I3C_DMA_REG_DATA_TRANSMIT_BYTE)
  {
    /* return address of TDR register */
    data_reg_addr = (uint32_t) &(I3Cx->TDR);
  }
  else if (Direction == LL_I3C_DMA_REG_DATA_RECEIVE_BYTE)
  {
    /* return address of RDR register */
    data_reg_addr = (uint32_t) &(I3Cx->RDR);
  }
  else if (Direction == LL_I3C_DMA_REG_DATA_TRANSMIT_WORD)
  {
    /* return address of TDWR register */
    data_reg_addr = (uint32_t) &(I3Cx->TDWR);
  }
  else if (Direction == LL_I3C_DMA_REG_DATA_RECEIVE_WORD)
  {
    /* return address of RDWR register */
    data_reg_addr = (uint32_t) &(I3Cx->RDWR);
  }
  else if (Direction == LL_I3C_DMA_REG_STATUS)
  {
    /* return address of SR register */
    data_reg_addr = (uint32_t) &(I3Cx->SR);
  }
  else
  {
    /* return address of CR register */
    data_reg_addr = (uint32_t) &(I3Cx->CR);
  }

  return data_reg_addr;
}

/**
  * @brief  Enable DMA FIFO reception requests.
  * @rmtoll CFGR      RXDMAEN       LL_I3C_EnableDMAReq_RX
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableDMAReq_RX(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->CFGR, I3C_CFGR_RXDMAEN);
}

/**
  * @brief  Disable DMA FIFO reception requests.
  * @rmtoll CFGR      RXDMAEN       LL_I3C_DisableDMAReq_RX
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableDMAReq_RX(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->CFGR, I3C_CFGR_RXDMAEN);
}

/**
  * @brief  Check if DMA FIFO reception requests are enabled or disabled.
  * @rmtoll CFGR      RXDMAEN       LL_I3C_IsEnabledDMAReq_RX
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledDMAReq_RX(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->CFGR, I3C_CFGR_RXDMAEN) == (I3C_CFGR_RXDMAEN)) ? 1UL : 0UL);
}

/**
  * @brief  Set the Receive FIFO Threshold level.
  * @rmtoll CFGR      RXTHRES       LL_I3C_SetRxFIFOThreshold
  * @param  I3Cx I3C Instance.
  * @param  RxFIFOThreshold This parameter can be one of the following values:
  *         @arg @ref LL_I3C_RXFIFO_THRESHOLD_1_4
  *         @arg @ref LL_I3C_RXFIFO_THRESHOLD_4_4
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetRxFIFOThreshold(I3C_TypeDef *I3Cx, uint32_t RxFIFOThreshold)
{
  MODIFY_REG(I3Cx->CFGR, I3C_CFGR_RXTHRES, RxFIFOThreshold);
}

/**
  * @brief  Get the Receive FIFO Threshold level.
  * @rmtoll CFGR      RXTHRES       LL_I3C_GetRxFIFOThreshold
  * @param  I3Cx I3C Instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_I3C_RXFIFO_THRESHOLD_1_4
  *         @arg @ref LL_I3C_RXFIFO_THRESHOLD_4_4
  */
__STATIC_INLINE uint32_t LL_I3C_GetRxFIFOThreshold(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->CFGR, I3C_CFGR_RXTHRES));
}

/**
  * @brief  Enable DMA FIFO transmission requests.
  * @rmtoll CFGR      TXDMAEN       LL_I3C_EnableDMAReq_TX
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableDMAReq_TX(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->CFGR, I3C_CFGR_TXDMAEN);
}

/**
  * @brief  Disable DMA FIFO transmission requests.
  * @rmtoll CFGR      TXDMAEN       LL_I3C_DisableDMAReq_TX
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableDMAReq_TX(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->CFGR, I3C_CFGR_TXDMAEN);
}

/**
  * @brief  Check if DMA FIFO transmission requests are enabled or disabled.
  * @rmtoll CFGR      TXDMAEN       LL_I3C_IsEnabledDMAReq_TX
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledDMAReq_TX(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->CFGR, I3C_CFGR_TXDMAEN) == (I3C_CFGR_TXDMAEN)) ? 1UL : 0UL);
}

/**
  * @brief  Set the Transmit FIFO Threshold level.
  * @rmtoll CFGR      TXTHRES       LL_I3C_SetTxFIFOThreshold
  * @param  I3Cx I3C Instance.
  * @param  TxFIFOThreshold This parameter can be one of the following values:
  *         @arg @ref LL_I3C_TXFIFO_THRESHOLD_1_4
  *         @arg @ref LL_I3C_TXFIFO_THRESHOLD_4_4
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetTxFIFOThreshold(I3C_TypeDef *I3Cx, uint32_t TxFIFOThreshold)
{
  MODIFY_REG(I3Cx->CFGR, I3C_CFGR_TXTHRES, TxFIFOThreshold);
}

/**
  * @brief  Get the Transmit FIFO Threshold level.
  * @rmtoll CFGR      TXTHRES       LL_I3C_GetTxFIFOThreshold
  * @param  I3Cx I3C Instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_I3C_TXFIFO_THRESHOLD_1_4
  *         @arg @ref LL_I3C_TXFIFO_THRESHOLD_4_4
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_GetTxFIFOThreshold(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->CFGR, I3C_CFGR_TXTHRES));
}

/**
  * @brief  Enable DMA FIFO Status requests.
  * @rmtoll CFGR      SDMAEN        LL_I3C_EnableDMAReq_Status
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableDMAReq_Status(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->CFGR, I3C_CFGR_SDMAEN);
}

/**
  * @brief  Disable DMA FIFO Status requests.
  * @rmtoll CFGR      SDMAEN        LL_I3C_DisableDMAReq_Status
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableDMAReq_Status(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->CFGR, I3C_CFGR_SDMAEN);
}

/**
  * @brief  Check if DMA FIFO Status requests are enabled or disabled.
  * @rmtoll CFGR      SDMAEN        LL_I3C_IsEnabledDMAReq_Status
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledDMAReq_Status(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->CFGR, I3C_CFGR_SDMAEN) == (I3C_CFGR_SDMAEN)) ? 1UL : 0UL);
}

/**
  * @brief  Enable the Status FIFO.
  * @note   Not applicable in target mode. Status FIFO always disabled in target mode.
  * @rmtoll CFGR      SMODE         LL_I3C_EnableStatusFIFO
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableStatusFIFO(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->CFGR, I3C_CFGR_SMODE);
}

/**
  * @brief  Disable the Status FIFO Threshold.
  * @rmtoll CFGR      SMODE         LL_I3C_DisableStatusFIFO
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableStatusFIFO(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->CFGR, I3C_CFGR_SMODE);
}

/**
  * @brief  Check if the Status FIFO Threshold is enabled or disabled.
  * @rmtoll CFGR      SMODE         LL_I3C_IsEnabledStatusFIFO
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledStatusFIFO(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->CFGR, I3C_CFGR_SMODE) == (I3C_CFGR_SMODE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable the Control and Transmit FIFO preloaded before starting a transfer on I3C bus.
  * @note   Not applicable in target mode. Control FIFO always disabled in target mode.
  * @rmtoll CFGR      TMODE         LL_I3C_EnableControlFIFO
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableControlFIFO(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->CFGR, I3C_CFGR_TMODE);
}

/**
  * @brief  Disable the Control and Transmit FIFO preloaded before starting a transfer on I3C bus.
  * @rmtoll CFGR      TMODE         LL_I3C_DisableControlFIFO
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableControlFIFO(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->CFGR, I3C_CFGR_TMODE);
}

/**
  * @brief  Check if the Control and Transmit FIFO preloaded is enabled or disabled.
  * @rmtoll CFGR      TMODE         LL_I3C_IsEnabledControlFIFO
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledControlFIFO(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->CFGR, I3C_CFGR_TMODE) == (I3C_CFGR_TMODE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable DMA FIFO Control word transfer requests.
  * @rmtoll CFGR      CDMAEN        LL_I3C_EnableDMAReq_Control
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableDMAReq_Control(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->CFGR, I3C_CFGR_CDMAEN);
}

/**
  * @brief  Disable DMA FIFO Control word transfer requests.
  * @rmtoll CFGR      CDMAEN        LL_I3C_DisableDMAReq_Control
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableDMAReq_Control(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->CFGR, I3C_CFGR_CDMAEN);
}

/**
  * @brief  Check if DMA FIFO Control word transfer requests are enabled or disabled.
  * @rmtoll CFGR      CDMAEN        LL_I3C_IsEnabledDMAReq_Control
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledDMAReq_Control(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->CFGR, I3C_CFGR_CDMAEN) == (I3C_CFGR_CDMAEN)) ? 1UL : 0UL);
}

/**
  * @brief  Set Own Dynamic Address as Valid.
  * @rmtoll DEVR0        DAVAL         LL_I3C_EnableOwnDynAddress
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableOwnDynAddress(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->DEVR0, I3C_DEVR0_DAVAL);
}

/**
  * @brief  Set Own Dynamic Address as Not-Valid.
  * @rmtoll DEVR0        DAVAL         LL_I3C_DisableOwnDynAddress
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableOwnDynAddress(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->DEVR0, I3C_DEVR0_DAVAL);
}

/**
  * @brief  Check if Own Dynamic address is Valid or Not-Valid.
  * @rmtoll DEVR0        DAVAL         LL_I3C_IsEnabledOwnDynAddress
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledOwnDynAddress(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->DEVR0, I3C_DEVR0_DAVAL) == (I3C_DEVR0_DAVAL)) ? 1UL : 0UL);
}

/**
  * @brief  Configure Own Dynamic Address.
  * @note   This bit can be programmed in controller mode or during Dynamic Address procedure from current controller.
  * @rmtoll DEVR0        DA            LL_I3C_SetOwnDynamicAddress
  * @param  I3Cx I3C Instance.
  * @param  OwnDynamicAddress This parameter must be a value between Min_Data=0 and Max_Data=0x7F
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetOwnDynamicAddress(I3C_TypeDef *I3Cx, uint32_t OwnDynamicAddress)
{
  MODIFY_REG(I3Cx->DEVR0, I3C_DEVR0_DA, (OwnDynamicAddress << I3C_DEVR0_DA_Pos));
}

/**
  * @brief  Get Own Dynamic Address.
  * @rmtoll DEVR0        DA            LL_I3C_GetOwnDynamicAddress
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 and Max_Data=0x7F
  */
__STATIC_INLINE uint8_t LL_I3C_GetOwnDynamicAddress(const I3C_TypeDef *I3Cx)
{
  return (uint8_t)(READ_BIT(I3Cx->DEVR0, I3C_DEVR0_DA) >> I3C_DEVR0_DA_Pos);
}

/**
  * @brief  Set IBI procedure allowed (when the I3C is acting as target).
  * @note   This bit can be programmed when the I3C is disabled (EN = 0) or updated by HW upon reception of DISEC CCC.
  * @rmtoll DEVR0        IBIEN         LL_I3C_EnableIBI
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIBI(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->DEVR0, I3C_DEVR0_IBIEN);
}

/**
  * @brief  Set IBI procedure not-allowed (when the I3C is acting as target).
  * @note   This bit can be programmed when the I3C is disabled (EN = 0) or updated by HW upon reception of DISEC CCC.
  * @rmtoll DEVR0        IBIEN         LL_I3C_DisableIBI
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIBI(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->DEVR0, I3C_DEVR0_IBIEN);
}

/**
  * @brief  Check if IBI procedure is allowed or not allowed.
  * @rmtoll DEVR0        IBIEN         LL_I3C_IsEnabledIBI
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIBI(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->DEVR0, I3C_DEVR0_IBIEN) == (I3C_DEVR0_IBIEN)) ? 1UL : 0UL);
}

/**
  * @brief  Set Controller-role Request allowed (when the I3C is acting as target).
  * @note   This bit can be programmed when the I3C is disabled (EN = 0) or updated by HW upon reception of DISEC CCC.
  * @rmtoll DEVR0        CREN          LL_I3C_EnableControllerRoleReq
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableControllerRoleReq(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->DEVR0, I3C_DEVR0_CREN);
}

/**
  * @brief  Set Controller-role Request as not-allowed (when the I3C is acting as target).
  * @note   This bit can be programmed when the I3C is disabled (EN = 0) or updated by HW upon reception of DISEC CCC.
  * @rmtoll DEVR0        CREN          LL_I3C_DisableControllerRoleReq
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableControllerRoleReq(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->DEVR0, I3C_DEVR0_CREN);
}

/**
  * @brief  Check if Controller-role Request is allowed or not-allowed.
  * @rmtoll DEVR0        CREN          LL_I3C_IsEnabledControllerRoleReq
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledControllerRoleReq(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->DEVR0, I3C_DEVR0_CREN) == (I3C_DEVR0_CREN)) ? 1UL : 0UL);
}

/**
  * @brief  Set Hot Join allowed (when the I3C is acting as target).
  * @note   This bit can be programmed when the I3C is disabled (EN = 0) or updated by HW upon reception of DISEC CCC.
  * @rmtoll DEVR0        HJEN          LL_I3C_EnableHotJoin
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableHotJoin(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->DEVR0, I3C_DEVR0_HJEN);
}

/**
  * @brief  Set Hot Join as not-allowed (when the I3C is acting as target).
  * @note   This bit can be programmed when the I3C is disabled (EN = 0) or updated by HW upon reception of DISEC CCC.
  * @rmtoll DEVR0        HJEN          LL_I3C_DisableHotJoin
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableHotJoin(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->DEVR0, I3C_DEVR0_HJEN);
}

/**
  * @brief  Check if Hot Join is allowed or not-allowed.
  * @rmtoll DEVR0        HJEN          LL_I3C_IsEnabledHotJoin
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledHotJoin(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->DEVR0, I3C_DEVR0_HJEN) == (I3C_DEVR0_HJEN)) ? 1UL : 0UL);
}

/**
  * @brief  Configure Maximum Read Length (target mode).
  * @note   Those bits can be updated by HW upon reception of GETMRL CCC.
  * @rmtoll MAXRLR       MRL           LL_I3C_SetMaxReadLength
  * @param  I3Cx I3C Instance.
  * @param  MaxReadLength This parameter must be a value between Min_Data=0x0 and Max_Data=0xFFFF
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetMaxReadLength(I3C_TypeDef *I3Cx, uint16_t MaxReadLength)
{
  MODIFY_REG(I3Cx->MAXRLR, I3C_MAXRLR_MRL, MaxReadLength);
}

/**
  * @brief  Return Maximum Read Length (target mode).
  * @rmtoll MAXRLR       MRL           LL_I3C_GetMaxReadLength
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0x0 and Max_Data=0xFFFFF
  */
__STATIC_INLINE uint32_t LL_I3C_GetMaxReadLength(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->MAXRLR, I3C_MAXRLR_MRL));
}

/**
  * @brief  Configure the number of additional Mandatory Data Byte (MDB) sent to the controller
  *         after an acknowledge of the IBI (target mode).
  * @rmtoll MAXRLR       IBIP          LL_I3C_ConfigNbIBIAddData
  * @param  I3Cx I3C Instance.
  * @param  NbIBIAddData This parameter can be one of the following values:
  *         @arg @ref LL_I3C_PAYLOAD_EMPTY
  *         @arg @ref LL_I3C_PAYLOAD_1_BYTE
  *         @arg @ref LL_I3C_PAYLOAD_2_BYTES
  *         @arg @ref LL_I3C_PAYLOAD_3_BYTES
  *         @arg @ref LL_I3C_PAYLOAD_4_BYTES
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ConfigNbIBIAddData(I3C_TypeDef *I3Cx, uint32_t NbIBIAddData)
{
  MODIFY_REG(I3Cx->MAXRLR, I3C_MAXRLR_IBIP, NbIBIAddData);
}

/**
  * @brief  Return the number of additional Mandatory Data Byte (MDB) sent to the controller
  *         after an acknowledge of the IBI (target mode).
  * @rmtoll MAXRLR       IBIP          LL_I3C_GetConfigNbIBIAddData
  * @param  I3Cx I3C Instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_I3C_PAYLOAD_EMPTY
  *         @arg @ref LL_I3C_PAYLOAD_1_BYTE
  *         @arg @ref LL_I3C_PAYLOAD_2_BYTES
  *         @arg @ref LL_I3C_PAYLOAD_3_BYTES
  *         @arg @ref LL_I3C_PAYLOAD_4_BYTES
  */
__STATIC_INLINE uint32_t LL_I3C_GetConfigNbIBIAddData(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->MAXRLR, I3C_MAXRLR_IBIP));
}

/**
  * @brief  Configure Maximum Write Length (target mode).
  * @note   Those bits can be updated by HW upon reception of GETMWL CCC.
  * @rmtoll MAXWLR       MWL           LL_I3C_SetMaxWriteLength
  * @param  I3Cx I3C Instance.
  * @param  MaxWriteLength This parameter must be a value between Min_Data=0x0 and Max_Data=0xFFFF
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetMaxWriteLength(I3C_TypeDef *I3Cx, uint16_t MaxWriteLength)
{
  MODIFY_REG(I3Cx->MAXWLR, I3C_MAXWLR_MWL, MaxWriteLength);
}

/**
  * @brief  Return Maximum Write Length (target mode).
  * @rmtoll MAXWLR       MWL           LL_I3C_GetMaxWriteLength
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0x0 and Max_Data=0xFFFFF
  */
__STATIC_INLINE uint32_t LL_I3C_GetMaxWriteLength(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->MAXWLR, I3C_MAXWLR_MWL));
}

/**
  * @brief  Configure the SCL clock signal waveform.
  * @note   This bit can only be programmed when the I3C is disabled (EN = 0).
  *
  * @note   This parameter is computed with the STM32CubeMX Tool.
  * @rmtoll TIMINGR0     TIMINGR0      LL_I3C_ConfigClockWaveForm
  * @param  I3Cx I3C Instance.
  * @param  ClockWaveForm This parameter must be a value between Min_Data=0 and Max_Data=0xFFFFFFFF.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ConfigClockWaveForm(I3C_TypeDef *I3Cx, uint32_t ClockWaveForm)
{
  WRITE_REG(I3Cx->TIMINGR0, ClockWaveForm);
}

/**
  * @brief  Get the SCL clock signal waveform.
  * @rmtoll TIMINGR0     TIMINGR0      LL_I3C_GetClockWaveForm
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 and Max_Data=0xFFFFFFFF.
  */
__STATIC_INLINE uint32_t LL_I3C_GetClockWaveForm(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_REG(I3Cx->TIMINGR0));
}

/**
  * @brief  Configure the SCL clock low period during I3C push-pull phases.
  * @note   This bit can only be programmed when the I3C is disabled (EN = 0).
  *
  * @note   This parameter is computed with the STM32CubeMX Tool.
  * @rmtoll TIMINGR0     SCLL_PP       LL_I3C_SetPeriodClockLowPP
  * @param  I3Cx I3C Instance.
  * @param  PeriodClockLowPP This parameter must be a value between Min_Data=0 and Max_Data=0xFF.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetPeriodClockLowPP(I3C_TypeDef *I3Cx, uint32_t PeriodClockLowPP)
{
  MODIFY_REG(I3Cx->TIMINGR0, I3C_TIMINGR0_SCLL_PP, (PeriodClockLowPP << I3C_TIMINGR0_SCLL_PP_Pos));
}

/**
  * @brief  Get the SCL clock low period during I3C push-pull phases.
  * @rmtoll TIMINGR0     SCLL_PP       LL_I3C_GetPeriodClockLowPP
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 and Max_Data=0xFF.
  */
__STATIC_INLINE uint32_t LL_I3C_GetPeriodClockLowPP(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->TIMINGR0, I3C_TIMINGR0_SCLL_PP) >> I3C_TIMINGR0_SCLL_PP_Pos);
}

/**
  * @brief  Configure the SCL clock High period during I3C open drain and push-pull phases.
  * @note   This bit can only be programmed when the I3C is disabled (EN = 0).
  *
  * @note   This parameter is computed with the STM32CubeMX Tool.
  * @rmtoll TIMINGR0     SCLH_I3C      LL_I3C_SetPeriodClockHighI3C
  * @param  I3Cx I3C Instance.
  * @param  PeriodClockHighI3C This parameter must be a value between Min_Data=0 and Max_Data=0xFF.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetPeriodClockHighI3C(I3C_TypeDef *I3Cx, uint32_t PeriodClockHighI3C)
{
  MODIFY_REG(I3Cx->TIMINGR0, I3C_TIMINGR0_SCLH_I3C, (PeriodClockHighI3C << I3C_TIMINGR0_SCLH_I3C_Pos));
}

/**
  * @brief  Get the SCL clock high period during I3C open drain and push-pull phases.
  * @rmtoll TIMINGR0     SCLH_I3C      LL_I3C_GetPeriodClockHighI3C
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 and Max_Data=0xFF.
  */
__STATIC_INLINE uint32_t LL_I3C_GetPeriodClockHighI3C(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->TIMINGR0, I3C_TIMINGR0_SCLH_I3C) >> I3C_TIMINGR0_SCLH_I3C_Pos);
}

/**
  * @brief  Configure the SCL clock low period during I3C open drain phases.
  * @note   This bit can only be programmed when the I3C is disabled (EN = 0).
  *
  * @note   This parameter is computed with the STM32CubeMX Tool.
  * @rmtoll TIMINGR0     SCLL_OD       LL_I3C_SetPeriodClockLowOD
  * @param  I3Cx I3C Instance.
  * @param  PeriodClockLowOD This parameter must be a value between Min_Data=0 and Max_Data=0xFF.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetPeriodClockLowOD(I3C_TypeDef *I3Cx, uint32_t PeriodClockLowOD)
{
  MODIFY_REG(I3Cx->TIMINGR0, I3C_TIMINGR0_SCLL_OD, (PeriodClockLowOD << I3C_TIMINGR0_SCLL_OD_Pos));
}

/**
  * @brief  Get the SCL clock low period during I3C open phases.
  * @rmtoll TIMINGR0     SCLL_OD       LL_I3C_GetPeriodClockLowOD
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 and Max_Data=0xFF.
  */
__STATIC_INLINE uint32_t LL_I3C_GetPeriodClockLowOD(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->TIMINGR0, I3C_TIMINGR0_SCLL_OD) >> I3C_TIMINGR0_SCLL_OD_Pos);
}

/**
  * @brief  Configure the SCL clock High period during I2C open drain phases.
  * @note   This bit can only be programmed when the I3C is disabled (EN = 0).
  *
  * @note   This parameter is computed with the STM32CubeMX Tool.
  * @rmtoll TIMINGR0     SCLH_I2C      LL_I3C_SetPeriodClockHighI2C
  * @param  I3Cx I3C Instance.
  * @param  PeriodClockHighI2C This parameter must be a value between Min_Data=0 and Max_Data=0xFF.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetPeriodClockHighI2C(I3C_TypeDef *I3Cx, uint32_t PeriodClockHighI2C)
{
  MODIFY_REG(I3Cx->TIMINGR0, I3C_TIMINGR0_SCLH_I2C, PeriodClockHighI2C << I3C_TIMINGR0_SCLH_I2C_Pos);
}

/**
  * @brief  Get the SCL clock high period during I2C open drain phases.
  * @rmtoll TIMINGR0     SCLH_I2C      LL_I3C_GetPeriodClockHighI2C
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 and Max_Data=0xFF.
  */
__STATIC_INLINE uint32_t LL_I3C_GetPeriodClockHighI2C(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->TIMINGR0, I3C_TIMINGR0_SCLH_I2C) >> I3C_TIMINGR0_SCLH_I2C_Pos);
}

/**
  * @brief  Configure the Controller additional hold time on SDA line.
  * @rmtoll TIMINGR1     SDA_HD        LL_I3C_SetDataHoldTime
  * @param  I3Cx I3C Instance.
  * @param  DataHoldTime This parameter can be one of the following values:
  *         @arg @ref LL_I3C_SDA_HOLD_TIME_0_5
  *         @arg @ref LL_I3C_SDA_HOLD_TIME_1_5
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetDataHoldTime(I3C_TypeDef *I3Cx, uint32_t DataHoldTime)
{
  MODIFY_REG(I3Cx->TIMINGR1, I3C_TIMINGR1_SDA_HD, DataHoldTime);
}

/**
  * @brief  Get the Controller additional hold time on SDA line.
  * @rmtoll TIMINGR1     SDA_HD        LL_I3C_GetDataHoldTime
  * @param  I3Cx I3C Instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_I3C_SDA_HOLD_TIME_0_5
  *         @arg @ref LL_I3C_SDA_HOLD_TIME_1_5
  */
__STATIC_INLINE uint32_t LL_I3C_GetDataHoldTime(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->TIMINGR1, I3C_TIMINGR1_SDA_HD));
}

/**
  * @brief  Configure the Idle, Available state.
  * @note   This bit can only be programmed when the I3C is disabled (EN = 0).
  *
  * @note   This parameter is computed with the STM32CubeMX Tool.
  * @rmtoll TIMINGR1     AVAL          LL_I3C_SetAvalTiming
  * @param  I3Cx I3C Instance.
  * @param  AvalTiming This parameter must be a value between Min_Data=0 and Max_Data=0xFF.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetAvalTiming(I3C_TypeDef *I3Cx, uint32_t AvalTiming)
{
  MODIFY_REG(I3Cx->TIMINGR1, I3C_TIMINGR1_AVAL, (AvalTiming << I3C_TIMINGR1_AVAL_Pos));
}

/**
  * @brief  Get the Idle, Available integer value state.
  * @rmtoll TIMINGR1     AVAL          LL_I3C_GetAvalTiming
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 and Max_Data=0xFF.
  */
__STATIC_INLINE uint32_t LL_I3C_GetAvalTiming(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->TIMINGR1, I3C_TIMINGR1_AVAL) >> I3C_TIMINGR1_AVAL_Pos);
}

/**
  * @brief  Configure the Free state.
  * @note   This bit can only be programmed when the I3C is disabled (EN = 0).
  *
  * @note   This parameter is computed with the STM32CubeMX Tool.
  * @rmtoll TIMINGR1     FREE          LL_I3C_SetFreeTiming
  * @param  I3Cx I3C Instance.
  * @param  FreeTiming This parameter must be a value between Min_Data=0 and Max_Data=0x3F.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetFreeTiming(I3C_TypeDef *I3Cx, uint32_t FreeTiming)
{
  MODIFY_REG(I3Cx->TIMINGR1, I3C_TIMINGR1_FREE, (FreeTiming << I3C_TIMINGR1_FREE_Pos));
}

/**
  * @brief  Get the Free integeter value state.
  * @rmtoll TIMINGR1     FREE          LL_I3C_GetFreeTiming
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 and Max_Data=0x3F.
  */
__STATIC_INLINE uint32_t LL_I3C_GetFreeTiming(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->TIMINGR1, I3C_TIMINGR1_FREE) >> I3C_TIMINGR1_FREE_Pos);
}

/**
  * @brief  Configure the activity state of the new controller.
  * @note   Refer to MIPI I3C specification (https:__www.mipi.org_specifications)
  *         for more details related to Activity State.
  * @rmtoll TIMINGR1     ASNCR            LL_I3C_SetControllerActivityState
  * @param  I3Cx I3C Instance.
  * @param  ControllerActivityState This parameter can be one of the following values:
  *         @arg @ref LL_I3C_OWN_ACTIVITY_STATE_0
  *         @arg @ref LL_I3C_OWN_ACTIVITY_STATE_1
  *         @arg @ref LL_I3C_OWN_ACTIVITY_STATE_2
  *         @arg @ref LL_I3C_OWN_ACTIVITY_STATE_3
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetControllerActivityState(I3C_TypeDef *I3Cx, uint32_t ControllerActivityState)
{
  MODIFY_REG(I3Cx->TIMINGR1, I3C_TIMINGR1_ASNCR, ControllerActivityState);
}

/**
  * @brief  Get the activity state of the new controller.
  * @note   Refer to MIPI I3C specification (https:__www.mipi.org_specifications)
  *         for more details related to Activity State.
  * @rmtoll TIMINGR1     ASNCR            LL_I3C_GetControllerActivityState
  * @param  I3Cx I3C Instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_I3C_OWN_ACTIVITY_STATE_0
  *         @arg @ref LL_I3C_OWN_ACTIVITY_STATE_1
  *         @arg @ref LL_I3C_OWN_ACTIVITY_STATE_2
  *         @arg @ref LL_I3C_OWN_ACTIVITY_STATE_3
  */
__STATIC_INLINE uint32_t LL_I3C_GetControllerActivityState(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->TIMINGR1, I3C_TIMINGR1_ASNCR));
}

/**
  * @brief  Configure the Controller SDA Hold time, Bus Free, Activity state, Idle state.
  * @note   This bit can only be programmed when the I3C is disabled (EN = 0).
  *
  * @note   This parameter is computed with the STM32CubeMX Tool.
  * @rmtoll TIMINGR1     SDA_HD        LL_I3C_SetCtrlBusCharacteristic\n
  *         TIMINGR1     FREE          LL_I3C_SetCtrlBusCharacteristic\n
  *         TIMINGR1     ASNCR         LL_I3C_SetCtrlBusCharacteristic\n
  *         TIMINGR1     IDLE          LL_I3C_SetCtrlBusCharacteristic
  * @param  I3Cx I3C Instance.
  * @param  CtrlBusCharacteristic This parameter must be a value between Min_Data=0 and Max_Data=0x107F03FF.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetCtrlBusCharacteristic(I3C_TypeDef *I3Cx, uint32_t CtrlBusCharacteristic)
{
  WRITE_REG(I3Cx->TIMINGR1, CtrlBusCharacteristic);
}

/**
  * @brief  Get the Controller SDA Hold time, Bus Free, Activity state, Idle state.
  * @rmtoll TIMINGR1     SDA_HD        LL_I3C_GetCtrlBusCharacteristic\n
  *         TIMINGR1     FREE          LL_I3C_GetCtrlBusCharacteristic\n
  *         TIMINGR1     ASNCR         LL_I3C_GetCtrlBusCharacteristic\n
  *         TIMINGR1     IDLE          LL_I3C_GetCtrlBusCharacteristic
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 and Max_Data=0x107F03FF.
  */
__STATIC_INLINE uint32_t LL_I3C_GetCtrlBusCharacteristic(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_REG(I3Cx->TIMINGR1));
}

/**
  * @brief  Configure the Target Available state.
  * @note   This bit can only be programmed when the I3C is disabled (EN = 0).
  *
  * @note   This parameter is computed with the STM32CubeMX Tool.
  * @rmtoll TIMINGR1     IDLE          LL_I3C_SetTgtBusCharacteristic
  * @param  I3Cx I3C Instance.
  * @param  TgtBusCharacteristic This parameter must be a value between Min_Data=0 and Max_Data=0xFF.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetTgtBusCharacteristic(I3C_TypeDef *I3Cx, uint32_t TgtBusCharacteristic)
{
  MODIFY_REG(I3Cx->TIMINGR1, I3C_TIMINGR1_AVAL, (TgtBusCharacteristic & I3C_TIMINGR1_AVAL));
}

/**
  * @brief  Get the Target Available state.
  * @rmtoll TIMINGR1     IDLE          LL_I3C_GetTgtBusCharacteristic
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 and Max_Data=0xFF.
  */
__STATIC_INLINE uint32_t LL_I3C_GetTgtBusCharacteristic(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->TIMINGR1, I3C_TIMINGR1_AVAL));
}

/**
  * @brief  Configure the SCL clock stalling time on I3C Bus (controller mode).
  * @note   This bit can only be programmed when the I3C is disabled (EN = 0).
  *
  * @note   This parameter is computed with the STM32CubeMX Tool.
  * @rmtoll TIMINGR2     STALL        LL_I3C_SetStallTime
  * @param  I3Cx I3C Instance.
  * @param  ControllerStallTime This parameter must be a value between Min_Data=0 and Max_Data=0xFF.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetStallTime(I3C_TypeDef *I3Cx, uint32_t ControllerStallTime)
{
  MODIFY_REG(I3Cx->TIMINGR2, I3C_TIMINGR2_STALL, (ControllerStallTime << I3C_TIMINGR2_STALL_Pos));
}

/**
  * @brief  Get the SCL clock stalling time on I3C Bus (controller mode).
  * @rmtoll TIMINGR2     STALL        LL_I3C_GetStallTime
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 and Max_Data=0xFF.
  */
__STATIC_INLINE uint32_t LL_I3C_GetStallTime(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->TIMINGR2, I3C_TIMINGR2_STALL));
}

/**
  * @brief  Set stall on ACK bit (controller mode).
  * @note   This bit can be programmed when the I3C is disabled (EN = 0).
  * @rmtoll TIMINGR2     STALLA       LL_I3C_EnableStallACK
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableStallACK(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->TIMINGR2, I3C_TIMINGR2_STALLA);
}

/**
  * @brief  Disable stall on ACK bit (controller mode).
  * @note   This bit can be programmed when the I3C is disabled (EN = 0).
  * @rmtoll TIMINGR2     STALLA       LL_I3C_DisableStallACK
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableStallACK(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->TIMINGR2, I3C_TIMINGR2_STALLA);
}

/**
  * @brief  Check if stall on ACK bit is enabled or disabled (controller mode).
  * @rmtoll TIMINGR2     STALLA       LL_I3C_IsEnabledStallACK
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledStallACK(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->TIMINGR2, I3C_TIMINGR2_STALLA) == (I3C_TIMINGR2_STALLA)) ? 1UL : 0UL);
}

/**
  * @brief  Set stall on Parity bit of Command Code byte (controller mode).
  * @note   This bit can be programmed when the I3C is disabled (EN = 0).
  * @rmtoll TIMINGR2     STALLC       LL_I3C_EnableStallParityCCC
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableStallParityCCC(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->TIMINGR2, I3C_TIMINGR2_STALLC);
}

/**
  * @brief  Disable stall on Parity bit of Command Code byte (controller mode).
  * @note   This bit can be programmed when the I3C is disabled (EN = 0).
  * @rmtoll TIMINGR2     STALLC       LL_I3C_DisableStallParityCCC
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableStallParityCCC(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->TIMINGR2, I3C_TIMINGR2_STALLC);
}

/**
  * @brief  Check if stall on Parity bit of Command Code byte is enabled or disabled (controller mode).
  * @rmtoll TIMINGR2     STALLC       LL_I3C_IsEnabledStallParityCCC
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledStallParityCCC(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->TIMINGR2, I3C_TIMINGR2_STALLC) == (I3C_TIMINGR2_STALLC)) ? 1UL : 0UL);
}

/**
  * @brief  Set stall on Parity bit of Data bytes (controller mode).
  * @note   This bit can be programmed when the I3C is disabled (EN = 0).
  * @rmtoll TIMINGR2     STALLD       LL_I3C_EnableStallParityData
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableStallParityData(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->TIMINGR2, I3C_TIMINGR2_STALLD);
}

/**
  * @brief  Disable stall on Parity bit of Data bytes (controller mode).
  * @note   This bit can be programmed when the I3C is disabled (EN = 0).
  * @rmtoll TIMINGR2     STALLD       LL_I3C_DisableStallParityData
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableStallParityData(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->TIMINGR2, I3C_TIMINGR2_STALLD);
}

/**
  * @brief  Check if stall on Parity bit of Data bytes is enabled or disabled (controller mode).
  * @rmtoll TIMINGR2     STALLD       LL_I3C_IsEnabledStallParityData
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledStallParityData(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->TIMINGR2, I3C_TIMINGR2_STALLD) == (I3C_TIMINGR2_STALLD)) ? 1UL : 0UL);
}

/**
  * @brief  Set stall on T bit (controller mode).
  * @note   This bit can be programmed when the I3C is disabled (EN = 0).
  * @rmtoll TIMINGR2     STALLT       LL_I3C_EnableStallTbit
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableStallTbit(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->TIMINGR2, I3C_TIMINGR2_STALLT);
}

/**
  * @brief  Disable stall on T bit (controller mode).
  * @note   This bit can be programmed when the I3C is disabled (EN = 0).
  * @rmtoll TIMINGR2     STALLT       LL_I3C_DisableStallTbit
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableStallTbit(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->TIMINGR2, I3C_TIMINGR2_STALLT);
}

/**
  * @brief  Check if stall on T bit is enabled or disabled (controller mode).
  * @rmtoll TIMINGR2     STALLT       LL_I3C_IsEnabledStallTbit
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledStallTbit(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->TIMINGR2, I3C_TIMINGR2_STALLT) == (I3C_TIMINGR2_STALLT)) ? 1UL : 0UL);
}

/**
  * @brief  Configure the Device Capability on Bus as Target or Controller (MIPI Bus Characteristics Register BCR6).
  * @note   Those bits can be programmed when the I3C is disabled (EN = 0).
  * @rmtoll BCR          BCR6          LL_I3C_SetDeviceCapabilityOnBus
  * @param  I3Cx I3C Instance.
  * @param  DeviceCapabilityOnBus This parameter can be one of the following values:
  *         @arg @ref LL_I3C_DEVICE_ROLE_AS_TARGET
  *         @arg @ref LL_I3C_DEVICE_ROLE_AS_CONTROLLER
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetDeviceCapabilityOnBus(I3C_TypeDef *I3Cx, uint32_t DeviceCapabilityOnBus)
{
  MODIFY_REG(I3Cx->BCR, I3C_BCR_BCR6, DeviceCapabilityOnBus);
}

/**
  * @brief  Get the Device Capability on Bus as Target or Controller (MIPI Bus Characteristics Register BCR6).
  * @rmtoll BCR          BCR6          LL_I3C_GetDeviceCapabilityOnBus
  * @param  I3Cx I3C Instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_I3C_DEVICE_ROLE_AS_TARGET
  *         @arg @ref LL_I3C_DEVICE_ROLE_AS_CONTROLLER
  */
__STATIC_INLINE uint32_t LL_I3C_GetDeviceCapabilityOnBus(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->BCR, I3C_BCR_BCR6));
}

/**
  * @brief  Configure the Device IBI Payload (MIPI Bus Characteristics Register BCR2).
  * @note   Those bits can be programmed when the I3C is disabled (EN = 0).
  * @rmtoll BCR          BCR2          LL_I3C_SetDeviceIBIPayload
  * @param  I3Cx I3C Instance.
  * @param  DeviceIBIPayload This parameter can be one of the following values:
  *         @arg @ref LL_I3C_IBI_NO_ADDITIONAL_DATA
  *         @arg @ref LL_I3C_IBI_ADDITIONAL_DATA
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetDeviceIBIPayload(I3C_TypeDef *I3Cx, uint32_t DeviceIBIPayload)
{
  MODIFY_REG(I3Cx->BCR, I3C_BCR_BCR2, DeviceIBIPayload);
}

/**
  * @brief  Get the Device IBI Payload (MIPI Bus Characteristics Register BCR2).
  * @rmtoll BCR          BCR2          LL_I3C_GetDeviceIBIPayload
  * @param  I3Cx I3C Instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_I3C_IBI_NO_ADDITIONAL_DATA
  *         @arg @ref LL_I3C_IBI_ADDITIONAL_DATA
  */
__STATIC_INLINE uint32_t LL_I3C_GetDeviceIBIPayload(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->BCR, I3C_BCR_BCR2));
}

/**
  * @brief  Configure the Data Speed Limitation (limitation, as described by I3C_GETMXDSR).
  * @note   Those bits can be programmed when the I3C is disabled (EN = 0).
  * @rmtoll BCR          BCR0          LL_I3C_SetDataSpeedLimitation
  * @param  I3Cx I3C Instance.
  * @param  DataSpeedLimitation This parameter can be one of the following values:
  *         @arg @ref LL_I3C_NO_DATA_SPEED_LIMITATION
  *         @arg @ref LL_I3C_MAX_DATA_SPEED_LIMITATION
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetDataSpeedLimitation(I3C_TypeDef *I3Cx, uint32_t DataSpeedLimitation)
{
  MODIFY_REG(I3Cx->BCR, I3C_BCR_BCR0, DataSpeedLimitation);
}

/**
  * @brief  Get  the Data Speed Limitation (limitation, as described by I3C_GETMXDSR).
  * @rmtoll BCR          BCR0          LL_I3C_GetDataSpeedLimitation
  * @param  I3Cx I3C Instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_I3C_NO_DATA_SPEED_LIMITATION
  *         @arg @ref LL_I3C_MAX_DATA_SPEED_LIMITATION
  */
__STATIC_INLINE uint32_t LL_I3C_GetDataSpeedLimitation(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->BCR, I3C_BCR_BCR0));
}

/**
  * @brief  Configure the Device Characteristics Register (DCR).
  * @note   This bit can only be programmed when the I3C is disabled (EN = 0).
  *
  * @note   Refer MIPI web site for the list of device code available.
  * @rmtoll DCR          DC            LL_I3C_SetDeviceCharacteristics
  * @param  I3Cx I3C Instance.
  * @param  DeviceCharacteristics This parameter must be a value between Min_Data=0 and Max_Data=0xFF.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetDeviceCharacteristics(I3C_TypeDef *I3Cx, uint32_t DeviceCharacteristics)
{
  MODIFY_REG(I3Cx->DCR, I3C_DCR_DCR, DeviceCharacteristics);
}

/**
  * @brief  Get the Device Characteristics Register (DCR).
  * @note   Refer MIPI web site to associated value with the list of device code available.
  * @rmtoll DCR          DCR            LL_I3C_GetDeviceCharacteristics
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 and Max_Data=0xFF.
  */
__STATIC_INLINE uint32_t LL_I3C_GetDeviceCharacteristics(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->DCR, I3C_DCR_DCR));
}

/**
  * @brief  Configure IBI MDB support for pending read notification.
  * @note   Those bits can be programmed when the I3C is disabled (EN = 0).
  * @rmtoll GETCAPR     CAPPEND          LL_I3C_SetPendingReadMDB
  * @param  I3Cx I3C Instance.
  * @param  PendingReadMDB This parameter can be one of the following values:
  *         @arg @ref LL_I3C_MDB_NO_PENDING_READ_NOTIFICATION
  *         @arg @ref LL_I3C_MDB_PENDING_READ_NOTIFICATION
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetPendingReadMDB(I3C_TypeDef *I3Cx, uint32_t PendingReadMDB)
{
  MODIFY_REG(I3Cx->GETCAPR, I3C_GETCAPR_CAPPEND, PendingReadMDB);
}

/**
  * @brief  Get IBI MDB support for pending read notification value.
  * @rmtoll GETCAPR     CAPPEND          LL_I3C_GetPendingReadMDB
  * @param  I3Cx I3C Instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_I3C_MDB_NO_PENDING_READ_NOTIFICATION
  *         @arg @ref LL_I3C_MDB_PENDING_READ_NOTIFICATION
  */
__STATIC_INLINE uint32_t LL_I3C_GetPendingReadMDB(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->GETCAPR, I3C_GETCAPR_CAPPEND));
}

/**
  * @brief  Configure the Group Management Support bit of MSTCAP1.
  * @note   Those bits can be programmed when the I3C is disabled (EN = 0).
  * @rmtoll CRCAPR      CAPGRP        LL_I3C_SetGrpAddrHandoffSupport
  * @param  I3Cx I3C Instance.
  * @param  GrpAddrHandoffSupport This parameter can be one of the following values:
  *         @arg @ref LL_I3C_HANDOFF_GRP_ADDR_NOT_SUPPORTED
  *         @arg @ref LL_I3C_HANDOFF_GRP_ADDR_SUPPORTED
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetGrpAddrHandoffSupport(I3C_TypeDef *I3Cx, uint32_t GrpAddrHandoffSupport)
{
  MODIFY_REG(I3Cx->CRCAPR, I3C_CRCAPR_CAPGRP, GrpAddrHandoffSupport);
}

/**
  * @brief  Get the Group Management Support bit of MSTCAP1.
  * @rmtoll CRCAPR      CAPGRP        LL_I3C_GetGrpAddrHandoffSupport
  * @param  I3Cx I3C Instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_I3C_HANDOFF_GRP_ADDR_NOT_SUPPORTED
  *         @arg @ref LL_I3C_HANDOFF_GRP_ADDR_SUPPORTED
  */
__STATIC_INLINE uint32_t LL_I3C_GetGrpAddrHandoffSupport(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->CRCAPR, I3C_CRCAPR_CAPGRP));
}

/**
  * @brief  Configure the Delayed Controller Handoff bit in MSTCAP2.
  * @note   Those bits can be programmed when the I3C is disabled (EN = 0).
  * @rmtoll CRCAPR      CAPDHOFF      LL_I3C_SetControllerHandoffDelayed
  * @param  I3Cx I3C Instance.
  * @param  ControllerHandoffDelayed This parameter can be one of the following values:
  *         @arg @ref LL_I3C_HANDOFF_NOT_DELAYED
  *         @arg @ref LL_I3C_HANDOFF_DELAYED
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetControllerHandoffDelayed(I3C_TypeDef *I3Cx, uint32_t ControllerHandoffDelayed)
{
  MODIFY_REG(I3Cx->CRCAPR, I3C_CRCAPR_CAPDHOFF, ControllerHandoffDelayed);
}

/**
  * @brief  Get the Delayed Controller Handoff bit in MSTCAP2.
  * @rmtoll CRCAPR      CAPDHOFF      LL_I3C_GetControllerHandoffDelayed
  * @param  I3Cx I3C Instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_I3C_HANDOFF_NOT_DELAYED
  *         @arg @ref LL_I3C_HANDOFF_DELAYED
  */
__STATIC_INLINE uint32_t LL_I3C_GetControllerHandoffDelayed(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->CRCAPR, I3C_CRCAPR_CAPDHOFF));
}

/**
  * @brief  Configure the Activity State after controllership handoff.
  * @note   Those bits can be programmed when the I3C is disabled (EN = 0).
  * @rmtoll GETMXDSR     HOFFAS        LL_I3C_SetHandoffActivityState
  * @param  I3Cx I3C Instance.
  * @param  HandoffActivityState This parameter can be one of the following values:
  *         @arg @ref LL_I3C_HANDOFF_ACTIVITY_STATE_0
  *         @arg @ref LL_I3C_HANDOFF_ACTIVITY_STATE_1
  *         @arg @ref LL_I3C_HANDOFF_ACTIVITY_STATE_2
  *         @arg @ref LL_I3C_HANDOFF_ACTIVITY_STATE_3
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetHandoffActivityState(I3C_TypeDef *I3Cx, uint32_t HandoffActivityState)
{
  MODIFY_REG(I3Cx->GETMXDSR, I3C_GETMXDSR_HOFFAS, HandoffActivityState);
}

/**
  * @brief  Get the Activity State after controllership handoff.
  * @rmtoll GETMXDSR     HOFFAS        LL_I3C_GetHandoffActivityState
  * @param  I3Cx I3C Instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_I3C_HANDOFF_ACTIVITY_STATE_0
  *         @arg @ref LL_I3C_HANDOFF_ACTIVITY_STATE_1
  *         @arg @ref LL_I3C_HANDOFF_ACTIVITY_STATE_2
  *         @arg @ref LL_I3C_HANDOFF_ACTIVITY_STATE_3
  */
__STATIC_INLINE uint32_t LL_I3C_GetHandoffActivityState(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->GETMXDSR, I3C_GETMXDSR_HOFFAS));
}

/**
  * @brief  Configure the Max Data Speed Format response for GETMXDS CCC.
  * @note   Those bits can be programmed when the I3C is disabled (EN = 0).
  * @rmtoll GETMXDSR     FMT          LL_I3C_SetMaxDataSpeedFormat
  * @param  I3Cx I3C Instance.
  * @param  MaxDataSpeedFormat This parameter can be one of the following values:
  *         @arg @ref LL_I3C_GETMXDS_FORMAT_1
  *         @arg @ref LL_I3C_GETMXDS_FORMAT_2_LSB
  *         @arg @ref LL_I3C_GETMXDS_FORMAT_2_MID
  *         @arg @ref LL_I3C_GETMXDS_FORMAT_2_MSB
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetMaxDataSpeedFormat(I3C_TypeDef *I3Cx, uint32_t MaxDataSpeedFormat)
{
  MODIFY_REG(I3Cx->GETMXDSR, I3C_GETMXDSR_FMT, MaxDataSpeedFormat);
}

/**
  * @brief  Get the Max Data Speed Format response for GETMXDS CCC.
  * @rmtoll GETMXDSR     FMT          LL_I3C_GetMaxDataSpeedFormat
  * @param  I3Cx I3C Instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_I3C_GETMXDS_FORMAT_1
  *         @arg @ref LL_I3C_GETMXDS_FORMAT_2_LSB
  *         @arg @ref LL_I3C_GETMXDS_FORMAT_2_MID
  *         @arg @ref LL_I3C_GETMXDS_FORMAT_2_MSB
  */
__STATIC_INLINE uint32_t LL_I3C_GetMaxDataSpeedFormat(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->GETMXDSR, I3C_GETMXDSR_FMT));
}

/**
  * @brief  Configure the Middle byte of MaxRdTurn field of GETMXDS CCC Format 2 with turnaround.
  * @note   Those bits can be programmed when the I3C is disabled (EN = 0).
  * @rmtoll GETMXDSR     RDTURN        LL_I3C_SetMiddleByteTurnAround
  * @param  I3Cx I3C Instance.
  * @param  MiddleByteTurnAround This parameter must be a value between Min_Data=0 and Max_Data=0xF.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetMiddleByteTurnAround(I3C_TypeDef *I3Cx, uint32_t MiddleByteTurnAround)
{
  MODIFY_REG(I3Cx->GETMXDSR, I3C_GETMXDSR_RDTURN, (MiddleByteTurnAround << I3C_GETMXDSR_RDTURN_Pos));
}

/**
  * @brief  Get the value of Middle byte of MaxRdTurn field of GETMXDS CCC Format 2 with turnaround.
  * @rmtoll GETMXDSR     RDTURN        LL_I3C_GetMiddleByteTurnAround
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 and Max_Data=0xF.
  */
__STATIC_INLINE uint32_t LL_I3C_GetMiddleByteTurnAround(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->GETMXDSR, I3C_GETMXDSR_RDTURN));
}

/**
  * @brief  Configure clock-to-data turnaround time.
  * @note   Those bits can be programmed when the I3C is disabled (EN = 0).
  * @rmtoll GETMXDSR     TSCO          LL_I3C_SetDataTurnAroundTime
  * @param  I3Cx I3C Instance.
  * @param  DataTurnAroundTime This parameter can be one of the following values:
  *         @arg @ref LL_I3C_TURNAROUND_TIME_TSCO_LESS_12NS
  *         @arg @ref LL_I3C_TURNAROUND_TIME_TSCO_GREATER_12NS
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetDataTurnAroundTime(I3C_TypeDef *I3Cx, uint32_t DataTurnAroundTime)
{
  MODIFY_REG(I3Cx->GETMXDSR, I3C_GETMXDSR_TSCO, DataTurnAroundTime);
}

/**
  * @brief  Get clock-to-data turnaround time.
  * @rmtoll GETMXDSR     TSCO          LL_I3C_GetDataTurnAroundTime
  * @param  I3Cx I3C Instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_I3C_TURNAROUND_TIME_TSCO_LESS_12NS
  *         @arg @ref LL_I3C_TURNAROUND_TIME_TSCO_GREATER_12NS
  */
__STATIC_INLINE uint32_t LL_I3C_GetDataTurnAroundTime(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->GETMXDSR, I3C_GETMXDSR_TSCO));
}

/**
  * @brief  Configure the MIPI Instance ID.
  * @note   Those bits can be programmed when the I3C is disabled (EN = 0).
  * @rmtoll EPIDR        MIPIID       LL_I3C_SetMIPIInstanceID
  * @param  I3Cx I3C Instance.
  * @param  MIPIInstanceID This parameter must be a value between Min_Data=0 and Max_Data=0xF.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetMIPIInstanceID(I3C_TypeDef *I3Cx, uint32_t MIPIInstanceID)
{
  MODIFY_REG(I3Cx->EPIDR, I3C_EPIDR_MIPIID, (MIPIInstanceID << I3C_EPIDR_MIPIID_Pos));
}

/**
  * @brief  Get the MIPI Instance ID.
  * @rmtoll EPIDR        MIPIID       LL_I3C_GetMIPIInstanceID
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 and Max_Data=0xF.
  */
__STATIC_INLINE uint32_t LL_I3C_GetMIPIInstanceID(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->EPIDR, I3C_EPIDR_MIPIID) >> I3C_EPIDR_MIPIID_Pos);
}

/**
  * @brief  Get the ID type selector.
  * @rmtoll EPIDR        IDTSEL        LL_I3C_GetIDTypeSelector
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0x0 and Max_Data=0x1
  */
__STATIC_INLINE uint32_t LL_I3C_GetIDTypeSelector(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->EPIDR, I3C_EPIDR_IDTSEL) >> I3C_EPIDR_IDTSEL_Pos);
}

/**
  * @brief  Get the MIPI Manufacturer ID.
  * @rmtoll EPIDR        MIPIMID       LL_I3C_GetMIPIManufacturerID
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 and Max_Data=0x7FFF.
  */
__STATIC_INLINE uint32_t LL_I3C_GetMIPIManufacturerID(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->EPIDR, I3C_EPIDR_MIPIMID) >> I3C_EPIDR_MIPIMID_Pos);
}

/**
  * @}
  */

/** @defgroup I3C_LL_EF_Data Management
  * @{
  */

/**
  * @brief  Request a reception Data FIFO Flush.
  * @rmtoll CFGR      RXFLUSH       LL_I3C_RequestRxFIFOFlush
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_RequestRxFIFOFlush(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->CFGR, I3C_CFGR_RXFLUSH);
}

/**
  * @brief  Request a transmission Data FIFO Flush.
  * @rmtoll CFGR      TXFLUSH       LL_I3C_RequestTxFIFOFlush
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_RequestTxFIFOFlush(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->CFGR, I3C_CFGR_TXFLUSH);
}

/**
  * @brief  Request a Status Data FIFO Flush.
  * @rmtoll CFGR      SFLUSH        LL_I3C_RequestStatusFIFOFlush
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_RequestStatusFIFOFlush(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->CFGR, I3C_CFGR_SFLUSH);
}

/**
  * @brief  Get Activity state of Controller on the I3C Bus (Target only).
  * @rmtoll DEVR0        AS            LL_I3C_GetActivityState
  * @param  I3Cx I3C Instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_I3C_BUS_ACTIVITY_STATE_0
  *         @arg @ref LL_I3C_BUS_ACTIVITY_STATE_1
  *         @arg @ref LL_I3C_BUS_ACTIVITY_STATE_2
  *         @arg @ref LL_I3C_BUS_ACTIVITY_STATE_3
  */
__STATIC_INLINE uint32_t LL_I3C_GetActivityState(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->DEVR0, I3C_DEVR0_AS));
}

/**
  * @brief  Get Reset Action (Target only).
  * @rmtoll DEVR0        RSTACT        LL_I3C_GetResetAction
  * @param  I3Cx I3C Instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_I3C_RESET_ACTION_NONE
  *         @arg @ref LL_I3C_RESET_ACTION_PARTIAL
  *         @arg @ref LL_I3C_RESET_ACTION_FULL
  */
__STATIC_INLINE uint32_t LL_I3C_GetResetAction(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->DEVR0, I3C_DEVR0_RSTACT));
}

/**
  * @brief  Request a Control word FIFO Flush.
  * @rmtoll CFGR      CFLUSH        LL_I3C_RequestControlFIFOFlush
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_RequestControlFIFOFlush(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->CFGR, I3C_CFGR_CFLUSH);
}

/**
  * @brief  Request a Transfer start.
  * @note   After request, the current instruction in Control Register is executed on I3C Bus.
  * @rmtoll CFGR      TSFSET        LL_I3C_RequestTransfer
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_RequestTransfer(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->CFGR, I3C_CFGR_TSFSET);
}

/**
  * @brief  Handles I3C Message content on the I3C Bus as Controller.
  * @rmtoll CR           ADD           LL_I3C_ControllerHandleMessage\n
  *         CR           DCNT          LL_I3C_ControllerHandleMessage\n
  *         CR           RNW           LL_I3C_ControllerHandleMessage\n
  *         CR           MTYPE         LL_I3C_ControllerHandleMessage\n
  *         CR           MEND          LL_I3C_ControllerHandleMessage
  * @param  I3Cx I3C Instance.
  * @param  TargetAddr Specifies the target address to be programmed.
  *               This parameter must be a value between Min_Data=0 and Max_Data=0xFF.
  * @param  TransferSize Specifies the number of bytes to be programmed.
  *               This parameter must be a value between Min_Data=0 and Max_Data=65535.
  * @param  Direction This parameter can be one of the following values:
  *         @arg @ref LL_I3C_DIRECTION_WRITE
  *         @arg @ref LL_I3C_DIRECTION_READ
  * @param  MessageType This parameter can be one of the following values:
  *         @arg @ref LL_I3C_CONTROLLER_MTYPE_RELEASE
  *         @arg @ref LL_I3C_CONTROLLER_MTYPE_HEADER
  *         @arg @ref LL_I3C_CONTROLLER_MTYPE_PRIVATE
  *         @arg @ref LL_I3C_CONTROLLER_MTYPE_DIRECT
  *         @arg @ref LL_I3C_CONTROLLER_MTYPE_LEGACY_I2C
  * @param  EndMode This parameter can be one of the following values:
  *         @arg @ref LL_I3C_GENERATE_STOP
  *         @arg @ref LL_I3C_GENERATE_RESTART
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ControllerHandleMessage(I3C_TypeDef *I3Cx, uint32_t TargetAddr, uint32_t TransferSize,
                                                    uint32_t Direction, uint32_t MessageType, uint32_t EndMode)
{
  WRITE_REG(I3Cx->CR, ((TargetAddr << I3C_CR_ADD_Pos) | TransferSize | Direction | MessageType | EndMode) \
            & (I3C_CR_ADD | I3C_CR_DCNT | I3C_CR_RNW | I3C_CR_MTYPE | I3C_CR_MEND));
}

/**
  * @brief  Handles I3C Common Command Code content on the I3C Bus as Controller.
  * @rmtoll CR           CCC           LL_I3C_ControllerHandleCCC\n
  *         CR           DCNT          LL_I3C_ControllerHandleCCC\n
  *         CR           MTYPE         LL_I3C_ControllerHandleCCC\n
  *         CR           MEND          LL_I3C_ControllerHandleCCC
  * @param  I3Cx I3C Instance.
  * @param  CCCValue Specifies the Command Code to be programmed.
  *               This parameter must be a value between Min_Data=0 and Max_Data=0x1FF.
  * @param  AddByteSize Specifies the number of CCC additional bytes to be programmed.
  *               This parameter must be a value between Min_Data=0 and Max_Data=65535.
  * @param  EndMode This parameter can be one of the following values:
  *         @arg @ref LL_I3C_GENERATE_STOP
  *         @arg @ref LL_I3C_GENERATE_RESTART
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ControllerHandleCCC(I3C_TypeDef *I3Cx, uint32_t CCCValue,
                                                uint32_t AddByteSize, uint32_t EndMode)
{
  WRITE_REG(I3Cx->CR, ((CCCValue << I3C_CR_CCC_Pos) | AddByteSize | EndMode | LL_I3C_CONTROLLER_MTYPE_CCC) \
            & (I3C_CR_CCC | I3C_CR_DCNT | I3C_CR_MTYPE | I3C_CR_MEND));
}

/**
  * @brief  Handles I3C Message content on the I3C Bus as Target.
  * @rmtoll CR           MTYPE         LL_I3C_TargetHandleMessage\n
  *         CR           DCNT       LL_I3C_TargetHandleMessage
  * @param  I3Cx I3C Instance.
  * @param  MessageType This parameter can be one of the following values:
  *         @arg @ref LL_I3C_TARGET_MTYPE_HOT_JOIN
  *         @arg @ref LL_I3C_TARGET_MTYPE_CONTROLLER_ROLE_REQ
  *         @arg @ref LL_I3C_TARGET_MTYPE_IBI
  * @param  IBISize Specifies the number of IBI bytes.
  *               This parameter must be a value between Min_Data=0 and Max_Data=65535.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_TargetHandleMessage(I3C_TypeDef *I3Cx, uint32_t MessageType, uint32_t IBISize)
{
  WRITE_REG(I3Cx->CR, (MessageType | IBISize) & (I3C_CR_DCNT | I3C_CR_MTYPE));
}

/**
  * @}
  */

/** @defgroup I3C_LL_EF_Data_Management Data_Management
  * @{
  */

/**
  * @brief  Read Receive Data Byte register.
  * @rmtoll RDR          RDB0         LL_I3C_ReceiveData8
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 to Max_Data=0xFF
  */
__STATIC_INLINE uint8_t LL_I3C_ReceiveData8(const I3C_TypeDef *I3Cx)
{
  return (uint8_t)(READ_BIT(I3Cx->RDR, I3C_RDR_RDB0));
}

/**
  * @brief  Write in Transmit Data Byte Register.
  * @rmtoll TDR          TDB0         LL_I3C_TransmitData8
  * @param  I3Cx I3C Instance.
  * @param  Data This parameter must be a value between Min_Data=0 and Max_Data=0xFF.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_TransmitData8(I3C_TypeDef *I3Cx, uint8_t Data)
{
  WRITE_REG(I3Cx->TDR, Data);
}

/**
  * @brief  Read Receive Data Word register.
  * @note   Content of register is filled in Little Endian.
  *         Mean MSB correspond to last data byte received,
  *         LSB correspond to first data byte received.
  * @rmtoll RDWR         RDWR          LL_I3C_ReceiveData32
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 to Max_Data=0xFFFFFFFF
  */
__STATIC_INLINE uint32_t LL_I3C_ReceiveData32(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_REG(I3Cx->RDWR));
}

/**
  * @brief  Write in Transmit Data Word Register.
  * @note   Content of register is filled in Little Endian.
  *         Mean MSB correspond to last data byte transmitted,
  *         LSB correspond to first data byte transmitted.
  * @rmtoll TDWR         TDWR          LL_I3C_TransmitData32
  * @param  I3Cx I3C Instance.
  * @param  Data This parameter must be a value between Min_Data=0 and Max_Data=0xFFFFFFFF.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_TransmitData32(I3C_TypeDef *I3Cx, uint32_t Data)
{
  WRITE_REG(I3Cx->TDWR, Data);
}

/**
  * @brief  Configure the IBI data payload to be sent during IBI (target mode).
  * @note   Content of register is filled in Little Endian.
  *         Mean MSB correspond to last IBI data byte,
  *         LSB correspond to first IBI data byte.
  * @rmtoll IBIDR        IBIDR         LL_I3C_SetIBIPayload
  * @param  I3Cx I3C Instance.
  * @param  OwnIBIPayload This parameter must be a value between Min_Data=0 and Max_Data=0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetIBIPayload(I3C_TypeDef *I3Cx, uint32_t OwnIBIPayload)
{
  WRITE_REG(I3Cx->IBIDR, OwnIBIPayload);
}

/**
  * @brief  Get the own IBI data payload (target mode), or get the Target IBI received (controller mode).
  * @note   Content of register is filled in Little Endian.
  *         Mean MSB correspond to last IBI data byte,
  *         LSB correspond to first IBI data byte.
  * @rmtoll IBIDR        IBIDR         LL_I3C_GetIBIPayload
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 to Max_Data=0xFFFFFFFF
  */
__STATIC_INLINE uint32_t LL_I3C_GetIBIPayload(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_REG(I3Cx->IBIDR));
}

/**
  * @brief  Get the number of data bytes received when reading IBI data (controller mode).
  * @rmtoll RMR         IBIRDCNT     LL_I3C_GetNbIBIAddData
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 to Max_Data=0x7
  */
__STATIC_INLINE uint32_t LL_I3C_GetNbIBIAddData(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->RMR, I3C_RMR_IBIRDCNT));
}

/**
  * @brief  Get the target address received during accepted IBI or Controller-role request.
  * @rmtoll RMR         RADD         LL_I3C_GetIBITargetAddr
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 to Max_Data=0x3F
  */
__STATIC_INLINE uint32_t LL_I3C_GetIBITargetAddr(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->RMR, I3C_RMR_RADD) >> I3C_RMR_RADD_Pos);
}

/**
  * @brief  Set TX FIFO Preload (target mode).
  * @note   Set high by Software, cleared by hardware when all the bytes to transmit have been loaded to TX FIFO.
  * @rmtoll TGTTDR       PRELOAD       LL_I3C_ConfigTxPreload
  * @rmtoll TGTTDR       TDCNT        LL_I3C_ConfigTxPreload
  * @param  I3Cx I3C Instance.
  * @param  TxDataCount This parameter must be a value between Min_Data=0 and Max_Data=0xFFFF
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ConfigTxPreload(I3C_TypeDef *I3Cx, uint16_t TxDataCount)
{
  MODIFY_REG(I3Cx->TGTTDR, (I3C_TGTTDR_PRELOAD | I3C_TGTTDR_TGTTDCNT), (I3C_TGTTDR_PRELOAD | TxDataCount));
}

/**
  * @brief  Indicates the status of TX FIFO preload (target mode).
  *         RESET: No preload of TX FIFO.
  *         SET: Preload of TX FIFO ongoing.
  * @note   Set high by Software, cleared by hardware when all the bytes to transmit have been loaded to TX FIFO.
  * @rmtoll TGTTDR       PRELOAD       LL_I3C_IsActiveTxPreload
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveTxPreload(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->TGTTDR, I3C_TGTTDR_PRELOAD) == (I3C_TGTTDR_PRELOAD)) ? 1UL : 0UL);
}

/**
  * @brief  Get the number of bytes to transmit (target mode).
  * @note   The return value correspond to the remaining number of bytes to load in TX FIFO.
  * @rmtoll TGTTDR       TDCNT        LL_I3C_GetTxPreloadDataCount
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 to Max_Data=0xFFFF
  */
__STATIC_INLINE uint16_t LL_I3C_GetTxPreloadDataCount(const I3C_TypeDef *I3Cx)
{
  return (uint16_t)(READ_BIT(I3Cx->TGTTDR, I3C_TGTTDR_TGTTDCNT));
}

/**
  * @brief  Get the number of data during a Transfer.
  * @note   The return value correspond to number of transmitted bytes reported
  *         during Address Assignment process in Target mode.
  * The return value  correspond to number of target detected
  * during Address Assignment process in Controller mode.
  * The return value  correspond to number of data bytes read from or sent to the I3C bus
  * during the message link to MID current value.
  * @rmtoll SR           XDCNT      LL_I3C_GetXferDataCount
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 to Max_Data=0xFFFF
  */
__STATIC_INLINE uint32_t LL_I3C_GetXferDataCount(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->SR, I3C_SR_XDCNT));
}

/**
  * @brief  Indicates if a Target abort a private read command.
  * @rmtoll SR           ABT           LL_I3C_IsTargetAbortPrivateRead
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsTargetAbortPrivateRead(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->SR, I3C_SR_ABT) == (I3C_SR_ABT)) ? 1UL : 0UL);
}

/**
  * @brief  Get Direction of the Message.
  * @rmtoll SR           DIR           LL_I3C_GetMessageDirection
  * @param  I3Cx I3C Instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_I3C_MESSAGE_DIRECTION_WRITE
  *         @arg @ref LL_I3C_MESSAGE_DIRECTION_READ
  */
__STATIC_INLINE uint32_t LL_I3C_GetMessageDirection(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->SR, I3C_SR_DIR));
}

/**
  * @brief  Get Message identifier.
  * @rmtoll SR           MID           LL_I3C_GetMessageIdentifier
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 to Max_Data=0xFF, representing the internal hardware counter value.
  */
__STATIC_INLINE uint32_t LL_I3C_GetMessageIdentifier(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->SR, I3C_SR_MID));
}

/**
  * @brief  Get Message error code.
  * @rmtoll SER          CODERR        LL_I3C_GetMessageErrorCode
  * @param  I3Cx I3C Instance.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_I3C_CONTROLLER_ERROR_CE0
  *         @arg @ref LL_I3C_CONTROLLER_ERROR_CE1
  *         @arg @ref LL_I3C_CONTROLLER_ERROR_CE2
  *         @arg @ref LL_I3C_CONTROLLER_ERROR_CE3
  *         @arg @ref LL_I3C_TARGET_ERROR_TE0
  *         @arg @ref LL_I3C_TARGET_ERROR_TE1
  *         @arg @ref LL_I3C_TARGET_ERROR_TE2
  *         @arg @ref LL_I3C_TARGET_ERROR_TE3
  *         @arg @ref LL_I3C_TARGET_ERROR_TE4
  *         @arg @ref LL_I3C_TARGET_ERROR_TE5
  *         @arg @ref LL_I3C_TARGET_ERROR_TE6
  */
__STATIC_INLINE uint32_t LL_I3C_GetMessageErrorCode(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->SER, I3C_SER_CODERR));
}

/**
  * @brief  Get CCC code of received command.
  * @rmtoll RMR         RCODE        LL_I3C_GetReceiveCommandCode
  * @param  I3Cx I3C Instance.
  * @retval Value between Min_Data=0 to Max_Data=0xFF.
  */
__STATIC_INLINE uint32_t LL_I3C_GetReceiveCommandCode(const I3C_TypeDef *I3Cx)
{
  return (uint32_t)(READ_BIT(I3Cx->RMR, I3C_RMR_RCODE) >> I3C_RMR_RCODE_Pos);
}

/**
  * @}
  */

/** @defgroup I3C_LL_EF_Target Payload
  * @{
  */

/**
  * @brief  Set Dynamic Address assigned to target x.
  * @rmtoll DEVRX        DA            LL_I3C_SetTargetDynamicAddress
  * @param  I3Cx I3C Instance.
  * @param  TargetId This parameter must be a value between Min_Data=1 and Max_Data=4
  * @param  DynamicAddr Value between Min_Data=0 to Max_Data=0x7F
  * @retval None
  */
__STATIC_INLINE void LL_I3C_SetTargetDynamicAddress(I3C_TypeDef *I3Cx, uint32_t TargetId, uint32_t DynamicAddr)
{
  MODIFY_REG(I3Cx->DEVRX[TargetId - 1U], I3C_DEVRX_DA, (DynamicAddr << I3C_DEVRX_DA_Pos));
}

/**
  * @brief  Get Dynamic Address assigned to target x.
  * @rmtoll DEVRX        DA            LL_I3C_GetTargetDynamicAddress
  * @param  I3Cx I3C Instance.
  * @param  TargetId This parameter must be a value between Min_Data=1 and Max_Data=4
  * @retval Value between Min_Data=0 to Max_Data=0x7F
  */
__STATIC_INLINE uint32_t LL_I3C_GetTargetDynamicAddress(const I3C_TypeDef *I3Cx, uint32_t TargetId)
{
  return (uint32_t)((READ_BIT(I3Cx->DEVRX[TargetId - 1U], I3C_DEVRX_DA)) >> I3C_DEVRX_DA_Pos);
}

/**
  * @brief  Enable IBI Acknowledgement from target x(controller mode).
  * @note   The bit DIS is automatically set when CRACK or IBIACK are set.
  *         This mean DEVRX register access is not allowed.
  *         Reset CRACK and IBIACK will reset DIS bit.
  * @rmtoll DEVRX        IBIACK        LL_I3C_EnableTargetIBIAck
  * @param  I3Cx I3C Instance.
  * @param  TargetId This parameter must be a value between Min_Data=1 and Max_Data=4
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableTargetIBIAck(I3C_TypeDef *I3Cx, uint32_t TargetId)
{
  SET_BIT(I3Cx->DEVRX[TargetId - 1U], I3C_DEVRX_IBIACK);
}

/**
  * @brief  Disable IBI Acknowledgement from target x (controller mode).
  * @rmtoll DEVRX        IBIACK        LL_I3C_DisableTargetIBIAck
  * @param  I3Cx I3C Instance.
  * @param  TargetId This parameter must be a value between Min_Data=1 and Max_Data=4
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableTargetIBIAck(I3C_TypeDef *I3Cx, uint32_t TargetId)
{
  CLEAR_BIT(I3Cx->DEVRX[TargetId - 1U], I3C_DEVRX_IBIACK);
}

/**
  * @brief  Indicates if IBI from target x will be Acknowledged or Not Acknowledged (controller mode).
  *         RESET: IBI Not Acknowledged.
  *         SET: IBI Acknowledged.
  * @rmtoll DEVRX        IBIACK        LL_I3C_IsEnabledTargetIBIAck
  * @param  I3Cx I3C Instance.
  * @param  TargetId This parameter must be a value between Min_Data=1 and Max_Data=4
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledTargetIBIAck(const I3C_TypeDef *I3Cx, uint32_t TargetId)
{
  return ((READ_BIT(I3Cx->DEVRX[TargetId - 1U], I3C_DEVRX_IBIACK) == I3C_DEVRX_IBIACK) ? 1UL : 0UL);
}

/**
  * @brief  Enable Controller-role Request Acknowledgement from target x(controller mode).
  * @note   The bit DIS is automatically set when CRACK or IBIACK are set.
  *         This mean DEVRX register access is not allowed.
  *         Reset CRACK and IBIACK will reset DIS bit.
  * @rmtoll DEVRX        CRACK         LL_I3C_EnableTargetCRAck
  * @param  I3Cx I3C Instance.
  * @param  TargetId This parameter must be a value between Min_Data=1 and Max_Data=4
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableTargetCRAck(I3C_TypeDef *I3Cx, uint32_t TargetId)
{
  SET_BIT(I3Cx->DEVRX[TargetId - 1U], I3C_DEVRX_CRACK);
}

/**
  * @brief  Disable Controller-role Request Acknowledgement from target x (controller mode).
  * @rmtoll DEVRX        CRACK         LL_I3C_DisableTargetCRAck
  * @param  I3Cx I3C Instance.
  * @param  TargetId This parameter must be a value between Min_Data=1 and Max_Data=4
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableTargetCRAck(I3C_TypeDef *I3Cx, uint32_t TargetId)
{
  CLEAR_BIT(I3Cx->DEVRX[TargetId - 1U], I3C_DEVRX_CRACK);
}

/**
  * @brief  Indicates if Controller-role Request from target x will be
  *         Acknowledged or Not Acknowledged (controller mode).
  *         RESET: Controller-role Request Not Acknowledged.
  *         SET: Controller-role Request Acknowledged.
  * @rmtoll DEVRX        CRACK         LL_I3C_IsEnabledTargetCRAck
  * @param  I3Cx I3C Instance.
  * @param  TargetId This parameter must be a value between Min_Data=1 and Max_Data=4
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledTargetCRAck(const I3C_TypeDef *I3Cx, uint32_t TargetId)
{
  return ((READ_BIT(I3Cx->DEVRX[TargetId - 1U], I3C_DEVRX_CRACK) == I3C_DEVRX_CRACK) ? 1UL : 0UL);
}

/**
  * @brief  Enable additional Mandatory Data Byte (MDB) follows the accepted IBI from target x.
  * @rmtoll DEVRX        IBIDEN          LL_I3C_EnableIBIAddData
  * @param  I3Cx I3C Instance.
  * @param  TargetId This parameter must be a value between Min_Data=1 and Max_Data=4
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIBIAddData(I3C_TypeDef *I3Cx, uint32_t TargetId)
{
  SET_BIT(I3Cx->DEVRX[TargetId - 1U], I3C_DEVRX_IBIDEN);
}

/**
  * @brief  Disable additional Mandatory Data Byte (MDB) follows the accepted IBI from target x.
  * @rmtoll DEVRX        IBIDEN          LL_I3C_DisableIBIAddData
  * @param  I3Cx I3C Instance.
  * @param  TargetId This parameter must be a value between Min_Data=1 and Max_Data=4
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIBIAddData(I3C_TypeDef *I3Cx, uint32_t TargetId)
{
  CLEAR_BIT(I3Cx->DEVRX[TargetId - 1U], I3C_DEVRX_IBIDEN);
}

/**
  * @brief  Indicates if additional Mandatory Data Byte (MDB) follows the accepted IBI from target x.
  *         RESET: No Mandatory Data Byte follows IBI.
  *         SET: Mandatory Data Byte follows IBI.
  * @rmtoll DEVRX        IBIDEN          LL_I3C_IsEnabledIBIAddData
  * @param  I3Cx I3C Instance.
  * @param  TargetId This parameter must be a value between Min_Data=1 and Max_Data=4
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIBIAddData(const I3C_TypeDef *I3Cx, uint32_t TargetId)
{
  return ((READ_BIT(I3Cx->DEVRX[TargetId - 1U], I3C_DEVRX_IBIDEN) == I3C_DEVRX_IBIDEN) ? 1UL : 0UL);
}

/**
  * @brief  Enable Suspension of Current transfer during IBI treatment.
  * @note   When set, this feature will allow controller to send
  *         a Stop condition and CR FIFO is flushed after IBI treatment.
  *         Software has to rewrite instructions in Control Register to start a new transfer.
  * @rmtoll DEVRX        SUSP          LL_I3C_EnableFrameSuspend
  * @param  I3Cx I3C Instance.
  * @param  TargetId This parameter must be a value between Min_Data=1 and Max_Data=4
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableFrameSuspend(I3C_TypeDef *I3Cx, uint32_t TargetId)
{
  SET_BIT(I3Cx->DEVRX[TargetId - 1U], I3C_DEVRX_SUSP);
}

/**
  * @brief  Disable Suspension of Current transfer during IBI treatment.
  * @note   When set, this feature will allow controller to continue CR FIFO treatment after IBI treatment.
  * @rmtoll DEVRX        SUSP          LL_I3C_DisableFrameSuspend
  * @param  I3Cx I3C Instance.
  * @param  TargetId This parameter must be a value between Min_Data=1 and Max_Data=4
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableFrameSuspend(I3C_TypeDef *I3Cx, uint32_t TargetId)
{
  CLEAR_BIT(I3Cx->DEVRX[TargetId - 1U], I3C_DEVRX_SUSP);
}

/**
  * @brief  Indicates if I3C transfer must be Suspended or not Suspended during IBI treatment from target x.
  *         RESET: Transfer is not suspended. Instruction in CR FIFO are executed after IBI.
  *         SET: Transfer is suspended (a Stop condition is sent). CR FIFO is flushed.
  * @rmtoll DEVRX        SUSP          LL_I3C_IsFrameMustBeSuspended
  * @param  I3Cx I3C Instance.
  * @param  TargetId This parameter must be a value between Min_Data=1 and Max_Data=4
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsFrameMustBeSuspended(const I3C_TypeDef *I3Cx, uint32_t TargetId)
{
  return ((READ_BIT(I3Cx->DEVRX[TargetId - 1U], I3C_DEVRX_SUSP) == I3C_DEVRX_SUSP) ? 1UL : 0UL);
}

/**
  * @brief  Indicates if update of the Device Characteristics Register is Allowed or Not Allowed.
  *         RESET: Device Characteristics Register update is Not Allowed.
  *         SET: Device Characteristics Register update is Allowed.
  * @note   Used to prevent software writing during reception of an IBI or Controller-role Request from target x.
  * @rmtoll DEVRX        DIS           LL_I3C_IsAllowedPayloadUpdate
  * @param  I3Cx I3C Instance.
  * @param  TargetId This parameter must be a value between Min_Data=1 and Max_Data=4
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsAllowedPayloadUpdate(const I3C_TypeDef *I3Cx, uint32_t TargetId)
{
  return ((READ_BIT(I3Cx->DEVRX[TargetId - 1U], I3C_DEVRX_DIS) != I3C_DEVRX_DIS) ? 1UL : 0UL);
}

/**
  * @brief  Set I3C bus devices configuration.
  * @note   This function is called only when the I3C instance is initialized as controller.
  *         This function can be called by the controller application to help the automatic treatment when target have
  *         capability of IBI and/or Control-Role.
  * @rmtoll DEVRX        DA            LL_I3C_ConfigDeviceCapabilities
  * @rmtoll DEVRX        IBIACK        LL_I3C_ConfigDeviceCapabilities
  * @rmtoll DEVRX        IBIDEN        LL_I3C_ConfigDeviceCapabilities
  * @rmtoll DEVRX        CRACK         LL_I3C_ConfigDeviceCapabilities
  * @param  I3Cx I3C Instance.
  * @param  TargetId This parameter must be a value between Min_Data=1 and Max_Data=4
  * @param  DynamicAddr Value between Min_Data=0 to Max_Data=0x7F
  * @param  IBIAck Value This parameter can be one of the following values:
  *         @arg @ref LL_I3C_IBI_CAPABILITY
  *         @arg @ref LL_I3C_IBI_NO_CAPABILITY
  * @param  IBIAddData This parameter can be one of the following values:
  *         @arg @ref LL_I3C_IBI_DATA_ENABLE
  *         @arg @ref LL_I3C_IBI_DATA_DISABLE
  * @param  CRAck This parameter can be one of the following values:
  *         @arg @ref LL_I3C_CR_CAPABILITY
  *         @arg @ref LL_I3C_CR_NO_CAPABILITY
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ConfigDeviceCapabilities(I3C_TypeDef *I3Cx,
                                                     uint32_t TargetId,
                                                     uint32_t DynamicAddr,
                                                     uint32_t IBIAck,
                                                     uint32_t IBIAddData,
                                                     uint32_t CRAck)
{
  MODIFY_REG(I3Cx->DEVRX[TargetId - 1U], \
             (I3C_DEVRX_DA | I3C_DEVRX_IBIACK | I3C_DEVRX_CRACK | I3C_DEVRX_IBIDEN), \
             ((DynamicAddr << I3C_DEVRX_DA_Pos) | IBIAck | IBIAddData | CRAck));
}
/**
  * @}
  */

/** @defgroup I3C_LL_EF_FLAG_management FLAG_management
  * @{
  */

/**
  * @brief  Indicates the status of Control FIFO Empty flag.
  *         RESET: One or more data are available in Control FIFO.
  *         SET: No more data available in Control FIFO.
  * @rmtoll EVR          CFEF          LL_I3C_IsActiveFlag_CFE
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_CFE(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_CFEF) == (I3C_EVR_CFEF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Transmit FIFO Empty flag.
  *         RESET: One or more data are available in Transmit FIFO.
  *         SET: No more data available in Transmit FIFO.
  * @rmtoll EVR          TXFEF         LL_I3C_IsActiveFlag_TXFE
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_TXFE(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_TXFEF) == (I3C_EVR_TXFEF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Control FIFO Not Full flag.
  *         RESET: One or more free space available in Control FIFO.
  *         SET: No more free space available in Control FIFO.
  * @note   When a transfer is ongoing, the Control FIFO shall not be written unless this flag is set.
  * @rmtoll EVR          CFNFF         LL_I3C_IsActiveFlag_CFNF
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_CFNF(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_CFNFF) == (I3C_EVR_CFNFF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Status FIFO Not Empty flag.
  *         RESET: One or more free space available in Status FIFO.
  *         SET: No more free space available in Status FIFO.
  * @note   This flag is updated only when the FIFO is used, mean SMODE = 1.
  * @rmtoll EVR          SFNEF         LL_I3C_IsActiveFlag_SFNE
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_SFNE(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_SFNEF) == (I3C_EVR_SFNEF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Transmit FIFO Not Full flag.
  *         RESET: One or more free space available in Transmit FIFO.
  *         SET: No more free space available in Transmit FIFO.
  * @note   When a transfer is ongoing, the Transmit FIFO shall not be written unless this flag is set.
  * @rmtoll EVR          TXFNFF        LL_I3C_IsActiveFlag_TXFNF
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_TXFNF(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_TXFNFF) == (I3C_EVR_TXFNFF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Receive FIFO Not Full flag.
  *         RESET: One or more data are available in Receive FIFO.
  *         SET: No more data available in Receive FIFO.
  * @rmtoll EVR          RXFNEF        LL_I3C_IsActiveFlag_RXFNE
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_RXFNE(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_RXFNEF) == (I3C_EVR_RXFNEF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates that the last Receive byte is available.
  *         RESET: Clear default value.
  *         SET: Last Receive byte ready to read from Receive FIFO.
  * @rmtoll EVR          RXLASTF       LL_I3C_IsActiveFlag_RXLAST
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_RXLAST(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_RXLASTF) == (I3C_EVR_RXLASTF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates that the last Transmit byte is written in FIFO.
  *         RESET: Transmission is not finalized.
  *         SET: Last Transmit byte is written in transmit FIFO.
  * @rmtoll EVR          TXLASTF       LL_I3C_IsActiveFlag_TXLAST
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_TXLAST(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_TXLASTF) == (I3C_EVR_TXLASTF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Frame Complete flag (controller and target mode).
  *         RESET: Current Frame transfer is not finalized.
  *         SET: Current Frame transfer is completed.
  * @rmtoll EVR          FCF           LL_I3C_IsActiveFlag_FC
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_FC(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_FCF) == (I3C_EVR_FCF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Reception Target End flag (controller mode).
  *         RESET: Clear default value.
  *         SET: Target prematurely ended a Read Command.
  * @note   This flag is set only when status FIFO is not used, mean SMODE = 0.
  * @rmtoll EVR          RXTGTENDF     LL_I3C_IsActiveFlag_RXTGTEND
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_RXTGTEND(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_RXTGTENDF) == (I3C_EVR_RXTGTENDF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Error flag (controller and target mode).
  *         RESET: Clear default value.
  *         SET: One or more Errors are detected.
  * @rmtoll EVR          ERRF          LL_I3C_IsActiveFlag_ERR
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_ERR(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_ERRF) == (I3C_EVR_ERRF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of IBI flag (controller mode).
  *         RESET: Clear default value.
  *         SET: An IBI have been received.
  * @rmtoll EVR          IBIF          LL_I3C_IsActiveFlag_IBI
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_IBI(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_IBIF) == (I3C_EVR_IBIF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of IBI End flag (target mode).
  *         RESET: Clear default value.
  *         SET: IBI procedure is finished.
  * @rmtoll EVR          IBIENDF       LL_I3C_IsActiveFlag_IBIEND
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_IBIEND(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_IBIENDF) == (I3C_EVR_IBIENDF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Controller-role Request flag (controller mode).
  *         RESET: Clear default value.
  *         SET: A Controller-role request procedure have been received.
  * @rmtoll EVR          CRF           LL_I3C_IsActiveFlag_CR
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_CR(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_CRF) == (I3C_EVR_CRF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Controller-role Request Update flag (target mode).
  *         RESET: Clear default value.
  *         SET: I3C device have gained Controller-role of the I3C Bus.
  * @rmtoll EVR          BCUPDF        LL_I3C_IsActiveFlag_CRUPD
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_CRUPD(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_CRUPDF) == (I3C_EVR_CRUPDF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Hot Join flag (controller mode).
  *         RESET: Clear default value.
  *         SET: A Hot Join request have been received.
  * @rmtoll EVR          HJF           LL_I3C_IsActiveFlag_HJ
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_HJ(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_HJF) == (I3C_EVR_HJF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Wake Up flag (target mode).
  *         RESET: Clear default value.
  *         SET: I3C Internal clock not available on time to treat the falling edge on SCL.
  * @rmtoll EVR          WKPF          LL_I3C_IsActiveFlag_WKP
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_WKP(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_WKPF) == (I3C_EVR_WKPF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Get flag (target mode).
  *         RESET: Clear default value.
  *         SET: A "get" type CCC have been received.
  * @rmtoll EVR          GETF          LL_I3C_IsActiveFlag_GET
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_GET(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_GETF) == (I3C_EVR_GETF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Get Status flag (target mode).
  *         RESET: Clear default value.
  *         SET: A GETSTATUS Command have been received.
  * @rmtoll EVR          STAF          LL_I3C_IsActiveFlag_STA
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_STA(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_STAF) == (I3C_EVR_STAF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Dynamic Address Update flag (target mode).
  *         RESET: Clear default value.
  *         SET: Own Dynamic Address have been updated.
  * @rmtoll EVR          DAUPDF        LL_I3C_IsActiveFlag_DAUPD
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_DAUPD(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_DAUPDF) == (I3C_EVR_DAUPDF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Max Write Length flag (target mode).
  *         RESET: Clear default value.
  *         SET: Max Write Length have been updated.
  * @rmtoll EVR          MWLUPDF       LL_I3C_IsActiveFlag_MWLUPD
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_MWLUPD(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_MWLUPDF) == (I3C_EVR_MWLUPDF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Max Read Length flag (target mode).
  *         RESET: Clear default value.
  *         SET: Max Read Length have been updated.
  * @rmtoll EVR          MRLUPDF       LL_I3C_IsActiveFlag_MRLUPD
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_MRLUPD(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_MRLUPDF) == (I3C_EVR_MRLUPDF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Reset flag (target mode).
  *         RESET: Clear default value.
  *         SET: A Reset Pattern have been received.
  * @rmtoll EVR          RSTF          LL_I3C_IsActiveFlag_RST
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_RST(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_RSTF) == (I3C_EVR_RSTF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Active State flag (target mode).
  *         RESET: Clear default value.
  *         SET: The Activity State have been updated.
  * @rmtoll EVR          ASUPDF        LL_I3C_IsActiveFlag_ASUPD
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_ASUPD(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_ASUPDF) == (I3C_EVR_ASUPDF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Interrupt Update flag (target mode).
  *         RESET: Clear default value.
  *         SET: One or more Interrupt autorized have been updated.
  * @rmtoll EVR          INTUPDF       LL_I3C_IsActiveFlag_INTUPD
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_INTUPD(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_INTUPDF) == (I3C_EVR_INTUPDF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Define List Targets flag (target mode).
  *         RESET: Clear default value.
  *         SET: A Define List Targets Command have been received.
  * @rmtoll EVR          DEFF          LL_I3C_IsActiveFlag_DEF
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_DEF(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_DEFF) == (I3C_EVR_DEFF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Define List Group Addresses flag.
  *         RESET: Clear default value.
  *         SET: A Define List Group Addresses have been received.
  * @rmtoll EVR          GRPF          LL_I3C_IsActiveFlag_GRP
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_GRP(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->EVR, I3C_EVR_GRPF) == (I3C_EVR_GRPF)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Protocol Error flag.
  *         RESET: Clear default value.
  *         SET: Protocol error detected.
  * @rmtoll SER          PERR          LL_I3C_IsActiveFlag_PERR
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_PERR(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->SER, I3C_SER_PERR) == (I3C_SER_PERR)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of SCL Stall Error flag (target mode).
  *         RESET: Clear default value.
  *         SET: Target detected that SCL was stable for more than 125us during I3C SDR read.
  * @rmtoll SER          STALL         LL_I3C_IsActiveFlag_STALL
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_STALL(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->SER, I3C_SER_STALL) == (I3C_SER_STALL)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of RX or TX FIFO Overrun flag.
  *         RESET: Clear default value.
  *         SET: RX FIFO Full or TX FIFO Empty depending of direction of message.
  * @rmtoll SER          DOVR          LL_I3C_IsActiveFlag_DOVR
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_DOVR(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->SER, I3C_SER_DOVR) == (I3C_SER_DOVR)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Control or Status FIFO Overrun flag (controller mode).
  *         RESET: Clear default value.
  *         SET: Status FIFO Full or Control FIFO Empty after Restart.
  * @rmtoll SER          COVR          LL_I3C_IsActiveFlag_COVR
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_COVR(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->SER, I3C_SER_COVR) == (I3C_SER_COVR)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Address not acknowledged flag (controller mode).
  *         RESET: Clear default value.
  *         SET: Controller detected that Target nacked static or dynamic address.
  * @rmtoll SER          ANACK         LL_I3C_IsActiveFlag_ANACK
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_ANACK(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->SER, I3C_SER_ANACK) == (I3C_SER_ANACK)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Data not acknowledged flag (controller mode).
  *         RESET: Clear default value.
  *         SET: Controller detected that Target nacked Data byte.
  * @rmtoll SER          DNACK         LL_I3C_IsActiveFlag_DNACK
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_DNACK(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->SER, I3C_SER_DNACK) == (I3C_SER_DNACK)) ? 1UL : 0UL);
}

/**
  * @brief  Indicates the status of Data error flag (controller mode).
  *         RESET: Clear default value.
  *         SET: Controller detected data error during Controller-role handoff process.
  * @rmtoll SER          DERR          LL_I3C_IsActiveFlag_DERR
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsActiveFlag_DERR(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->SER, I3C_SER_DERR) == (I3C_SER_DERR)) ? 1UL : 0UL);
}

/**
  * @}
  */

/** @defgroup I3C_LL_EF_IT_Management IT_Management
  * @{
  */

/**
  * @brief  Enable Control FIFO Not Full interrupt.
  * @rmtoll IER          CFNFIE        LL_I3C_EnableIT_CFNF
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_CFNF(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_CFNFIE);
}

/**
  * @brief  Disable Control FIFO Not Full interrupt.
  * @rmtoll IER          CFNFIE        LL_I3C_DisableIT_CFNF
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_CFNF(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_CFNFIE);
}

/**
  * @brief  Check if Control FIFO Not Full interrupt is enabled or disabled.
  * @rmtoll IER          CFNFIE        LL_I3C_IsEnabledIT_CFNF
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_CFNF(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_CFNFIE) == (I3C_IER_CFNFIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Status FIFO Not Empty interrupt.
  * @rmtoll IER          SFNEIE        LL_I3C_EnableIT_SFNE
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_SFNE(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_SFNEIE);
}

/**
  * @brief  Disable Status FIFO Not Empty interrupt.
  * @rmtoll IER          SFNEIE        LL_I3C_DisableIT_SFNE
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_SFNE(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_SFNEIE);
}

/**
  * @brief  Check if Status FIFO Not Empty interrupt is enabled or disabled.
  * @rmtoll IER          SFNEIE        LL_I3C_IsEnabledIT_SFNE
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_SFNE(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_SFNEIE) == (I3C_IER_SFNEIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Transmit FIFO Not Full interrupt.
  * @rmtoll IER          TXFNFIE       LL_I3C_EnableIT_TXFNF
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_TXFNF(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_TXFNFIE);
}

/**
  * @brief  Disable Transmit FIFO Not Full interrupt.
  * @rmtoll IER          TXFNFIE       LL_I3C_DisableIT_TXFNF
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_TXFNF(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_TXFNFIE);
}

/**
  * @brief  Check if Transmit FIFO Not Full interrupt is enabled or disabled.
  * @rmtoll IER          TXFNFIE       LL_I3C_IsEnabledIT_TXFNF
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_TXFNF(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_TXFNFIE) == (I3C_IER_TXFNFIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Receive FIFO Not Empty interrupt.
  * @rmtoll IER          RXFNEIE       LL_I3C_EnableIT_RXFNE
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_RXFNE(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_RXFNEIE);
}

/**
  * @brief  Disable Receive FIFO Not Empty interrupt.
  * @rmtoll IER          RXFNEIE       LL_I3C_DisableIT_RXFNE
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_RXFNE(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_RXFNEIE);
}

/**
  * @brief  Check if Receive FIFO Not Empty interrupt is enabled or disabled.
  * @rmtoll IER          RXFNEIE       LL_I3C_IsEnabledIT_RXFNE
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_RXFNE(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_RXFNEIE) == (I3C_IER_RXFNEIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Frame Complete interrupt.
  * @rmtoll IER          FCIE          LL_I3C_EnableIT_FC
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_FC(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_FCIE);
}

/**
  * @brief  Disable Frame Complete interrupt.
  * @rmtoll IER          FCIE          LL_I3C_DisableIT_FC
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_FC(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_FCIE);
}

/**
  * @brief  Check if Frame Complete interrupt is enabled or disabled.
  * @rmtoll IER          FCIE          LL_I3C_IsEnabledIT_FC
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_FC(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_FCIE) == (I3C_IER_FCIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Reception Target End interrupt.
  * @rmtoll IER          RXTGTENDIE    LL_I3C_EnableIT_RXTGTEND
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_RXTGTEND(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_RXTGTENDIE);
}

/**
  * @brief  Disable Reception Target End interrupt.
  * @rmtoll IER          RXTGTENDIE    LL_I3C_DisableIT_RXTGTEND
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_RXTGTEND(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_RXTGTENDIE);
}

/**
  * @brief  Check if Reception Target End interrupt is enabled or disabled.
  * @rmtoll IER          RXTGTENDIE    LL_I3C_IsEnabledIT_RXTGTEND
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_RXTGTEND(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_RXTGTENDIE) == (I3C_IER_RXTGTENDIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Error interrupt.
  * @rmtoll IER          ERRIE         LL_I3C_EnableIT_ERR
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_ERR(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_ERRIE);
}

/**
  * @brief  Disable Error interrupt.
  * @rmtoll IER          ERRIE         LL_I3C_DisableIT_ERR
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_ERR(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_ERRIE);
}

/**
  * @brief  Check if Error interrupt is enabled or disabled.
  * @rmtoll IER          ERRIE         LL_I3C_IsEnabledIT_ERR
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_ERR(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_ERRIE) == (I3C_IER_ERRIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable IBI interrupt.
  * @rmtoll IER          IBIIE         LL_I3C_EnableIT_IBI
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_IBI(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_IBIIE);
}

/**
  * @brief  Disable IBI interrupt.
  * @rmtoll IER          IBIIE         LL_I3C_DisableIT_IBI
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_IBI(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_IBIIE);
}

/**
  * @brief  Check if IBI interrupt is enabled or disabled.
  * @rmtoll IER          IBIIE         LL_I3C_IsEnabledIT_IBI
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_IBI(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_IBIIE) == (I3C_IER_IBIIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable IBI End interrupt.
  * @rmtoll IER          IBIENDIE      LL_I3C_EnableIT_IBIEND
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_IBIEND(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_IBIENDIE);
}

/**
  * @brief  Disable IBI End interrupt.
  * @rmtoll IER          IBIENDIE      LL_I3C_DisableIT_IBIEND
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_IBIEND(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_IBIENDIE);
}

/**
  * @brief  Check if IBI End interrupt is enabled or disabled.
  * @rmtoll IER          IBIENDIE      LL_I3C_IsEnabledIT_IBIEND
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_IBIEND(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_IBIENDIE) == (I3C_IER_IBIENDIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Controller-role interrupt.
  * @rmtoll IER          CRIE          LL_I3C_EnableIT_CR
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_CR(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_CRIE);
}

/**
  * @brief  Disable Controller-role interrupt.
  * @rmtoll IER          CRIE          LL_I3C_DisableIT_CR
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_CR(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_CRIE);
}

/**
  * @brief  Check if Controller-role interrupt is enabled or disabled.
  * @rmtoll IER          CRIE          LL_I3C_IsEnabledIT_CR
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_CR(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_CRIE) == (I3C_IER_CRIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Controller-role Update interrupt.
  * @rmtoll IER          CRUPDIE       LL_I3C_EnableIT_CRUPD
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_CRUPD(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_CRUPDIE);
}

/**
  * @brief  Disable Controller-role Update interrupt.
  * @rmtoll IER          CRUPDIE       LL_I3C_DisableIT_CRUPD
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_CRUPD(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_CRUPDIE);
}

/**
  * @brief  Check if Controller-role Update interrupt is enabled or disabled.
  * @rmtoll IER          CRUPDIE       LL_I3C_IsEnabledIT_CRUPD
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_CRUPD(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_CRUPDIE) == (I3C_IER_CRUPDIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Hot Join interrupt.
  * @rmtoll IER          HJIE          LL_I3C_EnableIT_HJ
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_HJ(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_HJIE);
}

/**
  * @brief  Disable Hot Join interrupt.
  * @rmtoll IER          HJIE          LL_I3C_DisableIT_HJ
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_HJ(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_HJIE);
}

/**
  * @brief  Check if Hot Join interrupt is enabled or disabled.
  * @rmtoll IER          HJIE          LL_I3C_IsEnabledIT_HJ
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_HJ(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_HJIE) == (I3C_IER_HJIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Wake Up interrupt.
  * @rmtoll IER          WKPIE         LL_I3C_EnableIT_WKP
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_WKP(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_WKPIE);
}

/**
  * @brief  Disable Wake Up interrupt.
  * @rmtoll IER          WKPIE         LL_I3C_DisableIT_WKP
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_WKP(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_WKPIE);
}

/**
  * @brief  Check if Wake Up is enabled or disabled.
  * @rmtoll IER          WKPIE         LL_I3C_IsEnabledIT_WKP
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_WKP(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_WKPIE) == (I3C_IER_WKPIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Get Command interrupt.
  * @rmtoll IER          GETIE         LL_I3C_EnableIT_GET
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_GET(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_GETIE);
}

/**
  * @brief  Disable Get Command interrupt.
  * @rmtoll IER          GETIE         LL_I3C_DisableIT_GET
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_GET(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_GETIE);
}

/**
  * @brief  Check if Get Command is enabled or disabled.
  * @rmtoll IER          GETIE         LL_I3C_IsEnabledIT_GET
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_GET(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_GETIE) == (I3C_IER_GETIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Get Status interrupt.
  * @rmtoll IER          STAIE         LL_I3C_EnableIT_STA
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_STA(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_STAIE);
}

/**
  * @brief  Disable Get Status interrupt.
  * @rmtoll IER          STAIE         LL_I3C_DisableIT_STA
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_STA(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_STAIE);
}

/**
  * @brief  Check if Get Status interrupt is enabled or disabled.
  * @rmtoll IER          STAIE         LL_I3C_IsEnabledIT_STA
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_STA(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_STAIE) == (I3C_IER_STAIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Dynamic Address Update interrupt.
  * @rmtoll IER          DAUPDIE       LL_I3C_EnableIT_DAUPD
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_DAUPD(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_DAUPDIE);
}

/**
  * @brief  Disable Dynamic Address Update interrupt.
  * @rmtoll IER          DAUPDIE       LL_I3C_DisableIT_DAUPD
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_DAUPD(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_DAUPDIE);
}

/**
  * @brief  Check if Dynamic Address Update interrupt is enabled or disabled.
  * @rmtoll IER          DAUPDIE       LL_I3C_IsEnabledIT_DAUPD
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_DAUPD(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_DAUPDIE) == (I3C_IER_DAUPDIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Max Write Length Update interrupt.
  * @rmtoll IER          MWLUPDIE      LL_I3C_EnableIT_MWLUPD
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_MWLUPD(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_MWLUPDIE);
}

/**
  * @brief  Disable Max Write Length Update interrupt.
  * @rmtoll IER          MWLUPDIE      LL_I3C_DisableIT_MWLUPD
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_MWLUPD(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_MWLUPDIE);
}

/**
  * @brief  Check if Max Write Length Update interrupt is enabled or disabled.
  * @rmtoll IER          MWLUPDIE      LL_I3C_IsEnabledIT_MWLUPD
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_MWLUPD(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_MWLUPDIE) == (I3C_IER_MWLUPDIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Max Read Length Update interrupt.
  * @rmtoll IER          MRLUPDIE      LL_I3C_EnableIT_MRLUPD
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_MRLUPD(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_MRLUPDIE);
}

/**
  * @brief  Disable Max Read Length Update interrupt.
  * @rmtoll IER          MRLUPDIE      LL_I3C_DisableIT_MRLUPD
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_MRLUPD(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_MRLUPDIE);
}

/**
  * @brief  Check if Max Read Length Update interrupt is enabled or disabled.
  * @rmtoll IER          MRLUPDIE      LL_I3C_IsEnabledIT_MRLUPD
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_MRLUPD(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_MRLUPDIE) == (I3C_IER_MRLUPDIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Reset interrupt.
  * @rmtoll IER          RSTIE         LL_I3C_EnableIT_RST
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_RST(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_RSTIE);
}

/**
  * @brief  Disable Reset interrupt.
  * @rmtoll IER          RSTIE         LL_I3C_DisableIT_RST
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_RST(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_RSTIE);
}

/**
  * @brief  Check if Reset interrupt is enabled or disabled.
  * @rmtoll IER          RSTIE         LL_I3C_IsEnabledIT_RST
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_RST(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_RSTIE) == (I3C_IER_RSTIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Activity State Update interrupt.
  * @rmtoll IER          ASUPDIE       LL_I3C_EnableIT_ASUPD
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_ASUPD(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_ASUPDIE);
}

/**
  * @brief  Disable Activity State Update interrupt.
  * @rmtoll IER          ASUPDIE       LL_I3C_DisableIT_ASUPD
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_ASUPD(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_ASUPDIE);
}

/**
  * @brief  Check if Activity State Update interrupt is enabled or disabled.
  * @rmtoll IER          ASUPDIE       LL_I3C_IsEnabledIT_ASUPD
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_ASUPD(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_ASUPDIE) == (I3C_IER_ASUPDIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Interrupt Update interrupt.
  * @rmtoll IER          INTUPDIE      LL_I3C_EnableIT_INTUPD
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_INTUPD(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_INTUPDIE);
}

/**
  * @brief  Disable Interrupt Update interrupt.
  * @rmtoll IER          INTUPDIE      LL_I3C_DisableIT_INTUPD
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_INTUPD(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_INTUPDIE);
}

/**
  * @brief  Check if Interrupt Update interrupt is enabled or disabled.
  * @rmtoll IER          INTUPDIE      LL_I3C_IsEnabledIT_INTUPD
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_INTUPD(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_INTUPDIE) == (I3C_IER_INTUPDIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Define List Target interrupt.
  * @rmtoll IER          DEFIE         LL_I3C_EnableIT_DEF
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_DEF(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_DEFIE);
}

/**
  * @brief  Disable Define List Target interrupt.
  * @rmtoll IER          DEFIE         LL_I3C_DisableIT_DEF
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_DEF(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_DEFIE);
}

/**
  * @brief  Check if Define List Target interrupt is enabled or disabled.
  * @rmtoll IER          DEFIE         LL_I3C_IsEnabledIT_DEF
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_DEF(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_DEFIE) == (I3C_IER_DEFIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Define List Group Addresses interrupt.
  * @rmtoll IER          GRPIE         LL_I3C_EnableIT_GRP
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_EnableIT_GRP(I3C_TypeDef *I3Cx)
{
  SET_BIT(I3Cx->IER, I3C_IER_GRPIE);
}

/**
  * @brief  Disable Define List Group Addresses interrupt.
  * @rmtoll IER          GRPIE         LL_I3C_DisableIT_GRP
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_DisableIT_GRP(I3C_TypeDef *I3Cx)
{
  CLEAR_BIT(I3Cx->IER, I3C_IER_GRPIE);
}

/**
  * @brief  Check if Define List Group Addresses interrupt is enabled or disabled.
  * @rmtoll IER          GRPIE         LL_I3C_IsEnabledIT_GRP
  * @param  I3Cx I3C Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_I3C_IsEnabledIT_GRP(const I3C_TypeDef *I3Cx)
{
  return ((READ_BIT(I3Cx->IER, I3C_IER_GRPIE) == (I3C_IER_GRPIE)) ? 1UL : 0UL);
}

/**
  * @}
  */

/** @addtogroup I3C_LL_EF_FLAG_management FLAG_management
  * @{
  */

/**
  * @brief  Clear Frame Complete flag (controller and target mode).
  * @rmtoll CEVR         CFCF          LL_I3C_ClearFlag_FC
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ClearFlag_FC(I3C_TypeDef *I3Cx)
{
  WRITE_REG(I3Cx->CEVR, I3C_CEVR_CFCF);
}

/**
  * @brief  Clear Reception Target End flag (controller mode).
  * @rmtoll CEVR         CRXTGTENDF    LL_I3C_ClearFlag_RXTGTEND
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ClearFlag_RXTGTEND(I3C_TypeDef *I3Cx)
{
  WRITE_REG(I3Cx->CEVR, I3C_CEVR_CRXTGTENDF);
}

/**
  * @brief  Clear Error flag (controller and target mode).
  * @rmtoll CEVR         CERRF         LL_I3C_ClearFlag_ERR
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ClearFlag_ERR(I3C_TypeDef *I3Cx)
{
  WRITE_REG(I3Cx->CEVR, I3C_CEVR_CERRF);
}

/**
  * @brief  Clear IBI flag (controller mode).
  * @rmtoll CEVR         CIBIF         LL_I3C_ClearFlag_IBI
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ClearFlag_IBI(I3C_TypeDef *I3Cx)
{
  WRITE_REG(I3Cx->CEVR, I3C_CEVR_CIBIF);
}

/**
  * @brief  Clear IBI End flag (target mode).
  * @rmtoll CEVR         CIBIENDF      LL_I3C_ClearFlag_IBIEND
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ClearFlag_IBIEND(I3C_TypeDef *I3Cx)
{
  WRITE_REG(I3Cx->CEVR, I3C_CEVR_CIBIENDF);
}

/**
  * @brief  Clear Controller-role Request flag (controller mode).
  * @rmtoll CEVR         CCRF          LL_I3C_ClearFlag_CR
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ClearFlag_CR(I3C_TypeDef *I3Cx)
{
  WRITE_REG(I3Cx->CEVR, I3C_CEVR_CCRF);
}

/**
  * @brief  Clear Controller-role Request Update flag (target mode).
  * @rmtoll CEVR         CCRUPDF       LL_I3C_ClearFlag_CRUPD
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ClearFlag_CRUPD(I3C_TypeDef *I3Cx)
{
  WRITE_REG(I3Cx->CEVR, I3C_CEVR_CCRUPDF);
}

/**
  * @brief  Clear Hot Join flag (controller mode).
  * @rmtoll CEVR         CHJF          LL_I3C_ClearFlag_HJ
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ClearFlag_HJ(I3C_TypeDef *I3Cx)
{
  WRITE_REG(I3Cx->CEVR, I3C_CEVR_CHJF);
}

/**
  * @brief  Clear Wake Up flag (target mode).
  * @rmtoll CEVR         CWKPF         LL_I3C_ClearFlag_WKP
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ClearFlag_WKP(I3C_TypeDef *I3Cx)
{
  WRITE_REG(I3Cx->CEVR, I3C_CEVR_CWKPF);
}

/**
  * @brief  Clear Get flag (target mode).
  * @rmtoll CEVR         CGETF         LL_I3C_ClearFlag_GET
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ClearFlag_GET(I3C_TypeDef *I3Cx)
{
  WRITE_REG(I3Cx->CEVR, I3C_CEVR_CGETF);
}

/**
  * @brief  Clear Get Status flag (target mode).
  * @rmtoll CEVR         CSTAF         LL_I3C_ClearFlag_STA
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ClearFlag_STA(I3C_TypeDef *I3Cx)
{
  WRITE_REG(I3Cx->CEVR, I3C_CEVR_CSTAF);
}

/**
  * @brief  Clear Dynamic Address Update flag (target mode).
  * @rmtoll CEVR         CDAUPDF       LL_I3C_ClearFlag_DAUPD
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ClearFlag_DAUPD(I3C_TypeDef *I3Cx)
{
  WRITE_REG(I3Cx->CEVR, I3C_CEVR_CDAUPDF);
}

/**
  * @brief  Clear Max Write Length flag (target mode).
  * @rmtoll CEVR         CMWLUPDF      LL_I3C_ClearFlag_MWLUPD
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ClearFlag_MWLUPD(I3C_TypeDef *I3Cx)
{
  WRITE_REG(I3Cx->CEVR, I3C_CEVR_CMWLUPDF);
}

/**
  * @brief  Clear Max Read Length flag (target mode).
  * @rmtoll CEVR         CMRLUPDF      LL_I3C_ClearFlag_MRLUPD
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ClearFlag_MRLUPD(I3C_TypeDef *I3Cx)
{
  WRITE_REG(I3Cx->CEVR, I3C_CEVR_CMRLUPDF);
}

/**
  * @brief  Clear Reset flag (target mode).
  * @rmtoll CEVR         CRSTF         LL_I3C_ClearFlag_RST
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ClearFlag_RST(I3C_TypeDef *I3Cx)
{
  WRITE_REG(I3Cx->CEVR, I3C_CEVR_CRSTF);
}

/**
  * @brief  Clear Active State flag (target mode).
  * @rmtoll CEVR         CASUPDF       LL_I3C_ClearFlag_ASUPD
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ClearFlag_ASUPD(I3C_TypeDef *I3Cx)
{
  WRITE_REG(I3Cx->CEVR, I3C_CEVR_CASUPDF);
}

/**
  * @brief  Clear Interrupt Update flag (target mode).
  * @rmtoll CEVR         CINTUPDF      LL_I3C_ClearFlag_INTUPD
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ClearFlag_INTUPD(I3C_TypeDef *I3Cx)
{
  WRITE_REG(I3Cx->CEVR, I3C_CEVR_CINTUPDF);
}

/**
  * @brief  Clear Define List Targets flag (target mode).
  * @rmtoll CEVR         CDEFF         LL_I3C_ClearFlag_DEF
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ClearFlag_DEF(I3C_TypeDef *I3Cx)
{
  WRITE_REG(I3Cx->CEVR, I3C_CEVR_CDEFF);
}

/**
  * @brief  Clear Define List Group Addresses flag.
  * @rmtoll CEVR         CGRPF         LL_I3C_ClearFlag_GRP
  * @param  I3Cx I3C Instance.
  * @retval None
  */
__STATIC_INLINE void LL_I3C_ClearFlag_GRP(I3C_TypeDef *I3Cx)
{
  WRITE_REG(I3Cx->CEVR, I3C_CEVR_CGRPF);
}

/**
  * @}
  */

#if defined(USE_FULL_LL_DRIVER)
/** @defgroup I3C_LL_EF_Init Initialization and de-initialization functions
  * @{
  */

ErrorStatus LL_I3C_Init(I3C_TypeDef *I3Cx, LL_I3C_InitTypeDef *I3C_InitStruct, uint32_t Mode);
ErrorStatus LL_I3C_DeInit(const I3C_TypeDef *I3Cx);
void LL_I3C_StructInit(LL_I3C_InitTypeDef *I3C_InitStruct);

/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/**
  * @}
  */

/**
  * @}
  */

#endif /* I3C1 || I3C2 */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32N6xx_LL_I3C_H */
