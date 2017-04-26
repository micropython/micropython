/**
  ******************************************************************************
  * @file    stm32f7xx_ll_sdmmc.h
  * @author  MCD Application Team
  * @version V1.1.2
  * @date    23-September-2016
  * @brief   Header file of SDMMC HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F7xx_LL_SDMMC_H
#define __STM32F7xx_LL_SDMMC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal_def.h"

/** @addtogroup STM32F7xx_Driver
  * @{
  */

/** @addtogroup SDMMC_LL
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup SDMMC_LL_Exported_Types SDMMC_LL Exported Types
  * @{
  */

/**
  * @brief  SDMMC Configuration Structure definition
  */
typedef struct
{
  uint32_t ClockEdge;            /*!< Specifies the clock transition on which the bit capture is made.
                                      This parameter can be a value of @ref SDMMC_LL_Clock_Edge                 */

  uint32_t ClockBypass;          /*!< Specifies whether the SDMMC Clock divider bypass is
                                      enabled or disabled.
                                      This parameter can be a value of @ref SDMMC_LL_Clock_Bypass               */

  uint32_t ClockPowerSave;       /*!< Specifies whether SDMMC Clock output is enabled or
                                      disabled when the bus is idle.
                                      This parameter can be a value of @ref SDMMC_LL_Clock_Power_Save           */

  uint32_t BusWide;              /*!< Specifies the SDMMC bus width.
                                      This parameter can be a value of @ref SDMMC_LL_Bus_Wide                   */

  uint32_t HardwareFlowControl;  /*!< Specifies whether the SDMMC hardware flow control is enabled or disabled.
                                      This parameter can be a value of @ref SDMMC_LL_Hardware_Flow_Control      */

  uint32_t ClockDiv;             /*!< Specifies the clock frequency of the SDMMC controller.
                                      This parameter can be a value between Min_Data = 0 and Max_Data = 255 */

}SDMMC_InitTypeDef;


/**
  * @brief  SDMMC Command Control structure
  */
typedef struct
{
  uint32_t Argument;            /*!< Specifies the SDMMC command argument which is sent
                                     to a card as part of a command message. If a command
                                     contains an argument, it must be loaded into this register
                                     before writing the command to the command register.              */

  uint32_t CmdIndex;            /*!< Specifies the SDMMC command index. It must be Min_Data = 0 and
                                     Max_Data = 64                                                    */

  uint32_t Response;            /*!< Specifies the SDMMC response type.
                                     This parameter can be a value of @ref SDMMC_LL_Response_Type         */

  uint32_t WaitForInterrupt;    /*!< Specifies whether SDMMC wait for interrupt request is
                                     enabled or disabled.
                                     This parameter can be a value of @ref SDMMC_LL_Wait_Interrupt_State  */

  uint32_t CPSM;                /*!< Specifies whether SDMMC Command path state machine (CPSM)
                                     is enabled or disabled.
                                     This parameter can be a value of @ref SDMMC_LL_CPSM_State            */
}SDMMC_CmdInitTypeDef;


/**
  * @brief  SDMMC Data Control structure
  */
typedef struct
{
  uint32_t DataTimeOut;         /*!< Specifies the data timeout period in card bus clock periods.  */

  uint32_t DataLength;          /*!< Specifies the number of data bytes to be transferred.         */

  uint32_t DataBlockSize;       /*!< Specifies the data block size for block transfer.
                                     This parameter can be a value of @ref SDMMC_LL_Data_Block_Size    */

  uint32_t TransferDir;         /*!< Specifies the data transfer direction, whether the transfer
                                     is a read or write.
                                     This parameter can be a value of @ref SDMMC_LL_Transfer_Direction */

  uint32_t TransferMode;        /*!< Specifies whether data transfer is in stream or block mode.
                                     This parameter can be a value of @ref SDMMC_LL_Transfer_Type      */

  uint32_t DPSM;                /*!< Specifies whether SDMMC Data path state machine (DPSM)
                                     is enabled or disabled.
                                     This parameter can be a value of @ref SDMMC_LL_DPSM_State         */
}SDMMC_DataInitTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup SDMMC_LL_Exported_Constants SDMMC_LL Exported Constants
  * @{
  */

/** @defgroup SDMMC_LL_Clock_Edge Clock Edge
  * @{
  */
#define SDMMC_CLOCK_EDGE_RISING               ((uint32_t)0x00000000U)
#define SDMMC_CLOCK_EDGE_FALLING              SDMMC_CLKCR_NEGEDGE

#define IS_SDMMC_CLOCK_EDGE(EDGE) (((EDGE) == SDMMC_CLOCK_EDGE_RISING) || \
                                  ((EDGE) == SDMMC_CLOCK_EDGE_FALLING))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Clock_Bypass Clock Bypass
  * @{
  */
#define SDMMC_CLOCK_BYPASS_DISABLE             ((uint32_t)0x00000000U)
#define SDMMC_CLOCK_BYPASS_ENABLE              SDMMC_CLKCR_BYPASS

#define IS_SDMMC_CLOCK_BYPASS(BYPASS) (((BYPASS) == SDMMC_CLOCK_BYPASS_DISABLE) || \
                                      ((BYPASS) == SDMMC_CLOCK_BYPASS_ENABLE))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Clock_Power_Save Clock Power Saving
  * @{
  */
#define SDMMC_CLOCK_POWER_SAVE_DISABLE         ((uint32_t)0x00000000U)
#define SDMMC_CLOCK_POWER_SAVE_ENABLE          SDMMC_CLKCR_PWRSAV

#define IS_SDMMC_CLOCK_POWER_SAVE(SAVE) (((SAVE) == SDMMC_CLOCK_POWER_SAVE_DISABLE) || \
                                        ((SAVE) == SDMMC_CLOCK_POWER_SAVE_ENABLE))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Bus_Wide Bus Width
  * @{
  */
#define SDMMC_BUS_WIDE_1B                      ((uint32_t)0x00000000U)
#define SDMMC_BUS_WIDE_4B                      SDMMC_CLKCR_WIDBUS_0
#define SDMMC_BUS_WIDE_8B                      SDMMC_CLKCR_WIDBUS_1

#define IS_SDMMC_BUS_WIDE(WIDE) (((WIDE) == SDMMC_BUS_WIDE_1B) || \
                                ((WIDE) == SDMMC_BUS_WIDE_4B) || \
                                ((WIDE) == SDMMC_BUS_WIDE_8B))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Hardware_Flow_Control Hardware Flow Control
  * @{
  */
#define SDMMC_HARDWARE_FLOW_CONTROL_DISABLE    ((uint32_t)0x00000000U)
#define SDMMC_HARDWARE_FLOW_CONTROL_ENABLE     SDMMC_CLKCR_HWFC_EN

#define IS_SDMMC_HARDWARE_FLOW_CONTROL(CONTROL) (((CONTROL) == SDMMC_HARDWARE_FLOW_CONTROL_DISABLE) || \
                                                ((CONTROL) == SDMMC_HARDWARE_FLOW_CONTROL_ENABLE))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Clock_Division Clock Division
  * @{
  */
#define IS_SDMMC_CLKDIV(DIV)   ((DIV) <= 0xFF)
/**
  * @}
  */

/** @defgroup SDMMC_LL_Command_Index Command Index
  * @{
  */
#define IS_SDMMC_CMD_INDEX(INDEX)            ((INDEX) < 0x40)
/**
  * @}
  */

/** @defgroup SDMMC_LL_Response_Type Response Type
  * @{
  */
#define SDMMC_RESPONSE_NO                    ((uint32_t)0x00000000U)
#define SDMMC_RESPONSE_SHORT                 SDMMC_CMD_WAITRESP_0
#define SDMMC_RESPONSE_LONG                  SDMMC_CMD_WAITRESP

#define IS_SDMMC_RESPONSE(RESPONSE) (((RESPONSE) == SDMMC_RESPONSE_NO)    || \
                                    ((RESPONSE) == SDMMC_RESPONSE_SHORT) || \
                                    ((RESPONSE) == SDMMC_RESPONSE_LONG))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Wait_Interrupt_State Wait Interrupt
  * @{
  */
#define SDMMC_WAIT_NO                        ((uint32_t)0x00000000U)
#define SDMMC_WAIT_IT                        SDMMC_CMD_WAITINT
#define SDMMC_WAIT_PEND                      SDMMC_CMD_WAITPEND

#define IS_SDMMC_WAIT(WAIT) (((WAIT) == SDMMC_WAIT_NO) || \
                            ((WAIT) == SDMMC_WAIT_IT) || \
                            ((WAIT) == SDMMC_WAIT_PEND))
/**
  * @}
  */

/** @defgroup SDMMC_LL_CPSM_State CPSM State
  * @{
  */
#define SDMMC_CPSM_DISABLE                   ((uint32_t)0x00000000U)
#define SDMMC_CPSM_ENABLE                    SDMMC_CMD_CPSMEN

#define IS_SDMMC_CPSM(CPSM) (((CPSM) == SDMMC_CPSM_DISABLE) || \
                            ((CPSM) == SDMMC_CPSM_ENABLE))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Response_Registers Response Register
  * @{
  */
#define SDMMC_RESP1                          ((uint32_t)0x00000000U)
#define SDMMC_RESP2                          ((uint32_t)0x00000004U)
#define SDMMC_RESP3                          ((uint32_t)0x00000008U)
#define SDMMC_RESP4                          ((uint32_t)0x0000000C)

#define IS_SDMMC_RESP(RESP) (((RESP) == SDMMC_RESP1) || \
                            ((RESP) == SDMMC_RESP2) || \
                            ((RESP) == SDMMC_RESP3) || \
                            ((RESP) == SDMMC_RESP4))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Data_Length Data Lenght
  * @{
  */
#define IS_SDMMC_DATA_LENGTH(LENGTH) ((LENGTH) <= 0x01FFFFFF)
/**
  * @}
  */

/** @defgroup SDMMC_LL_Data_Block_Size  Data Block Size
  * @{
  */
#define SDMMC_DATABLOCK_SIZE_1B               ((uint32_t)0x00000000U)
#define SDMMC_DATABLOCK_SIZE_2B               SDMMC_DCTRL_DBLOCKSIZE_0
#define SDMMC_DATABLOCK_SIZE_4B               SDMMC_DCTRL_DBLOCKSIZE_1
#define SDMMC_DATABLOCK_SIZE_8B               (SDMMC_DCTRL_DBLOCKSIZE_0|SDMMC_DCTRL_DBLOCKSIZE_1)
#define SDMMC_DATABLOCK_SIZE_16B              SDMMC_DCTRL_DBLOCKSIZE_2
#define SDMMC_DATABLOCK_SIZE_32B              (SDMMC_DCTRL_DBLOCKSIZE_0|SDMMC_DCTRL_DBLOCKSIZE_2)
#define SDMMC_DATABLOCK_SIZE_64B              (SDMMC_DCTRL_DBLOCKSIZE_1|SDMMC_DCTRL_DBLOCKSIZE_2)
#define SDMMC_DATABLOCK_SIZE_128B             (SDMMC_DCTRL_DBLOCKSIZE_0|SDMMC_DCTRL_DBLOCKSIZE_1|SDMMC_DCTRL_DBLOCKSIZE_2)
#define SDMMC_DATABLOCK_SIZE_256B             SDMMC_DCTRL_DBLOCKSIZE_3
#define SDMMC_DATABLOCK_SIZE_512B             (SDMMC_DCTRL_DBLOCKSIZE_0|SDMMC_DCTRL_DBLOCKSIZE_3)
#define SDMMC_DATABLOCK_SIZE_1024B            (SDMMC_DCTRL_DBLOCKSIZE_1|SDMMC_DCTRL_DBLOCKSIZE_3)
#define SDMMC_DATABLOCK_SIZE_2048B            (SDMMC_DCTRL_DBLOCKSIZE_0|SDMMC_DCTRL_DBLOCKSIZE_1|SDMMC_DCTRL_DBLOCKSIZE_3)
#define SDMMC_DATABLOCK_SIZE_4096B            (SDMMC_DCTRL_DBLOCKSIZE_2|SDMMC_DCTRL_DBLOCKSIZE_3)
#define SDMMC_DATABLOCK_SIZE_8192B            (SDMMC_DCTRL_DBLOCKSIZE_0|SDMMC_DCTRL_DBLOCKSIZE_2|SDMMC_DCTRL_DBLOCKSIZE_3)
#define SDMMC_DATABLOCK_SIZE_16384B           (SDMMC_DCTRL_DBLOCKSIZE_1|SDMMC_DCTRL_DBLOCKSIZE_2|SDMMC_DCTRL_DBLOCKSIZE_3)

#define IS_SDMMC_BLOCK_SIZE(SIZE) (((SIZE) == SDMMC_DATABLOCK_SIZE_1B)    || \
                                  ((SIZE) == SDMMC_DATABLOCK_SIZE_2B)    || \
                                  ((SIZE) == SDMMC_DATABLOCK_SIZE_4B)    || \
                                  ((SIZE) == SDMMC_DATABLOCK_SIZE_8B)    || \
                                  ((SIZE) == SDMMC_DATABLOCK_SIZE_16B)   || \
                                  ((SIZE) == SDMMC_DATABLOCK_SIZE_32B)   || \
                                  ((SIZE) == SDMMC_DATABLOCK_SIZE_64B)   || \
                                  ((SIZE) == SDMMC_DATABLOCK_SIZE_128B)  || \
                                  ((SIZE) == SDMMC_DATABLOCK_SIZE_256B)  || \
                                  ((SIZE) == SDMMC_DATABLOCK_SIZE_512B)  || \
                                  ((SIZE) == SDMMC_DATABLOCK_SIZE_1024B) || \
                                  ((SIZE) == SDMMC_DATABLOCK_SIZE_2048B) || \
                                  ((SIZE) == SDMMC_DATABLOCK_SIZE_4096B) || \
                                  ((SIZE) == SDMMC_DATABLOCK_SIZE_8192B) || \
                                  ((SIZE) == SDMMC_DATABLOCK_SIZE_16384B))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Transfer_Direction Transfer Direction
  * @{
  */
#define SDMMC_TRANSFER_DIR_TO_CARD            ((uint32_t)0x00000000U)
#define SDMMC_TRANSFER_DIR_TO_SDMMC            SDMMC_DCTRL_DTDIR

#define IS_SDMMC_TRANSFER_DIR(DIR) (((DIR) == SDMMC_TRANSFER_DIR_TO_CARD) || \
                                   ((DIR) == SDMMC_TRANSFER_DIR_TO_SDMMC))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Transfer_Type Transfer Type
  * @{
  */
#define SDMMC_TRANSFER_MODE_BLOCK             ((uint32_t)0x00000000U)
#define SDMMC_TRANSFER_MODE_STREAM            SDMMC_DCTRL_DTMODE

#define IS_SDMMC_TRANSFER_MODE(MODE) (((MODE) == SDMMC_TRANSFER_MODE_BLOCK) || \
                                     ((MODE) == SDMMC_TRANSFER_MODE_STREAM))
/**
  * @}
  */

/** @defgroup SDMMC_LL_DPSM_State DPSM State
  * @{
  */
#define SDMMC_DPSM_DISABLE                    ((uint32_t)0x00000000U)
#define SDMMC_DPSM_ENABLE                     SDMMC_DCTRL_DTEN

#define IS_SDMMC_DPSM(DPSM) (((DPSM) == SDMMC_DPSM_DISABLE) ||\
                            ((DPSM) == SDMMC_DPSM_ENABLE))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Read_Wait_Mode Read Wait Mode
  * @{
  */
#define SDMMC_READ_WAIT_MODE_DATA2                ((uint32_t)0x00000000U)
#define SDMMC_READ_WAIT_MODE_CLK                  (SDMMC_DCTRL_RWMOD)

#define IS_SDMMC_READWAIT_MODE(MODE) (((MODE) == SDMMC_READ_WAIT_MODE_CLK) || \
                                     ((MODE) == SDMMC_READ_WAIT_MODE_DATA2))
/**
  * @}
  */

/** @defgroup SDMMC_LL_Interrupt_sources Interrupt Sources
  * @{
  */
#define SDMMC_IT_CCRCFAIL                    SDMMC_STA_CCRCFAIL
#define SDMMC_IT_DCRCFAIL                    SDMMC_STA_DCRCFAIL
#define SDMMC_IT_CTIMEOUT                    SDMMC_STA_CTIMEOUT
#define SDMMC_IT_DTIMEOUT                    SDMMC_STA_DTIMEOUT
#define SDMMC_IT_TXUNDERR                    SDMMC_STA_TXUNDERR
#define SDMMC_IT_RXOVERR                     SDMMC_STA_RXOVERR
#define SDMMC_IT_CMDREND                     SDMMC_STA_CMDREND
#define SDMMC_IT_CMDSENT                     SDMMC_STA_CMDSENT
#define SDMMC_IT_DATAEND                     SDMMC_STA_DATAEND
#define SDMMC_IT_DBCKEND                     SDMMC_STA_DBCKEND
#define SDMMC_IT_CMDACT                      SDMMC_STA_CMDACT
#define SDMMC_IT_TXACT                       SDMMC_STA_TXACT
#define SDMMC_IT_RXACT                       SDMMC_STA_RXACT
#define SDMMC_IT_TXFIFOHE                    SDMMC_STA_TXFIFOHE
#define SDMMC_IT_RXFIFOHF                    SDMMC_STA_RXFIFOHF
#define SDMMC_IT_TXFIFOF                     SDMMC_STA_TXFIFOF
#define SDMMC_IT_RXFIFOF                     SDMMC_STA_RXFIFOF
#define SDMMC_IT_TXFIFOE                     SDMMC_STA_TXFIFOE
#define SDMMC_IT_RXFIFOE                     SDMMC_STA_RXFIFOE
#define SDMMC_IT_TXDAVL                      SDMMC_STA_TXDAVL
#define SDMMC_IT_RXDAVL                      SDMMC_STA_RXDAVL
#define SDMMC_IT_SDIOIT                      SDMMC_STA_SDIOIT
/**
  * @}
  */

/** @defgroup SDMMC_LL_Flags Flags
  * @{
  */
#define SDMMC_FLAG_CCRCFAIL                  SDMMC_STA_CCRCFAIL
#define SDMMC_FLAG_DCRCFAIL                  SDMMC_STA_DCRCFAIL
#define SDMMC_FLAG_CTIMEOUT                  SDMMC_STA_CTIMEOUT
#define SDMMC_FLAG_DTIMEOUT                  SDMMC_STA_DTIMEOUT
#define SDMMC_FLAG_TXUNDERR                  SDMMC_STA_TXUNDERR
#define SDMMC_FLAG_RXOVERR                   SDMMC_STA_RXOVERR
#define SDMMC_FLAG_CMDREND                   SDMMC_STA_CMDREND
#define SDMMC_FLAG_CMDSENT                   SDMMC_STA_CMDSENT
#define SDMMC_FLAG_DATAEND                   SDMMC_STA_DATAEND
#define SDMMC_FLAG_DBCKEND                   SDMMC_STA_DBCKEND
#define SDMMC_FLAG_CMDACT                    SDMMC_STA_CMDACT
#define SDMMC_FLAG_TXACT                     SDMMC_STA_TXACT
#define SDMMC_FLAG_RXACT                     SDMMC_STA_RXACT
#define SDMMC_FLAG_TXFIFOHE                  SDMMC_STA_TXFIFOHE
#define SDMMC_FLAG_RXFIFOHF                  SDMMC_STA_RXFIFOHF
#define SDMMC_FLAG_TXFIFOF                   SDMMC_STA_TXFIFOF
#define SDMMC_FLAG_RXFIFOF                   SDMMC_STA_RXFIFOF
#define SDMMC_FLAG_TXFIFOE                   SDMMC_STA_TXFIFOE
#define SDMMC_FLAG_RXFIFOE                   SDMMC_STA_RXFIFOE
#define SDMMC_FLAG_TXDAVL                    SDMMC_STA_TXDAVL
#define SDMMC_FLAG_RXDAVL                    SDMMC_STA_RXDAVL
#define SDMMC_FLAG_SDIOIT                    SDMMC_STA_SDIOIT
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup SDMMC_LL_Exported_macros SDMMC_LL Exported Macros
  * @{
  */

/** @defgroup SDMMC_LL_Register Bits And Addresses Definitions
  * @brief SDMMC_LL registers bit address in the alias region
  * @{
  */
/* ---------------------- SDMMC registers bit mask --------------------------- */
/* --- CLKCR Register ---*/
/* CLKCR register clear mask */
#define CLKCR_CLEAR_MASK         ((uint32_t)(SDMMC_CLKCR_CLKDIV  | SDMMC_CLKCR_PWRSAV |\
                                             SDMMC_CLKCR_BYPASS  | SDMMC_CLKCR_WIDBUS |\
                                             SDMMC_CLKCR_NEGEDGE | SDMMC_CLKCR_HWFC_EN))

/* --- DCTRL Register ---*/
/* SDMMC DCTRL Clear Mask */
#define DCTRL_CLEAR_MASK         ((uint32_t)(SDMMC_DCTRL_DTEN    | SDMMC_DCTRL_DTDIR |\
                                             SDMMC_DCTRL_DTMODE  | SDMMC_DCTRL_DBLOCKSIZE))

/* --- CMD Register ---*/
/* CMD Register clear mask */
#define CMD_CLEAR_MASK           ((uint32_t)(SDMMC_CMD_CMDINDEX | SDMMC_CMD_WAITRESP |\
                                             SDMMC_CMD_WAITINT  | SDMMC_CMD_WAITPEND |\
                                             SDMMC_CMD_CPSMEN   | SDMMC_CMD_SDIOSUSPEND))

/* SDMMC Initialization Frequency (400KHz max) */
#define SDMMC_INIT_CLK_DIV ((uint8_t)0x76)

/* SDMMC Data Transfer Frequency (25MHz max) */
#define SDMMC_TRANSFER_CLK_DIV ((uint8_t)0x0)

/**
  * @}
  */

/** @defgroup SDMMC_LL_Interrupt_Clock Interrupt And Clock Configuration
 *  @brief macros to handle interrupts and specific clock configurations
 * @{
 */

/**
  * @brief  Enable the SDMMC device.
  * @param  __INSTANCE__: SDMMC Instance
  * @retval None
  */
#define __SDMMC_ENABLE(__INSTANCE__)  ((__INSTANCE__)->CLKCR |= SDMMC_CLKCR_CLKEN)

/**
  * @brief  Disable the SDMMC device.
  * @param  __INSTANCE__: SDMMC Instance
  * @retval None
  */
#define __SDMMC_DISABLE(__INSTANCE__)  ((__INSTANCE__)->CLKCR &= ~SDMMC_CLKCR_CLKEN)

/**
  * @brief  Enable the SDMMC DMA transfer.
  * @param  __INSTANCE__: SDMMC Instance
  * @retval None
  */
#define __SDMMC_DMA_ENABLE(__INSTANCE__)  ((__INSTANCE__)->DCTRL |= SDMMC_DCTRL_DMAEN)
/**
  * @brief  Disable the SDMMC DMA transfer.
  * @param  __INSTANCE__: SDMMC Instance
  * @retval None
  */
#define __SDMMC_DMA_DISABLE(__INSTANCE__)  ((__INSTANCE__)->DCTRL &= ~SDMMC_DCTRL_DMAEN)

/**
  * @brief  Enable the SDMMC device interrupt.
  * @param  __INSTANCE__ : Pointer to SDMMC register base
  * @param  __INTERRUPT__ : specifies the SDMMC interrupt sources to be enabled.
  *         This parameter can be one or a combination of the following values:
  *            @arg SDMMC_IT_CCRCFAIL: Command response received (CRC check failed) interrupt
  *            @arg SDMMC_IT_DCRCFAIL: Data block sent/received (CRC check failed) interrupt
  *            @arg SDMMC_IT_CTIMEOUT: Command response timeout interrupt
  *            @arg SDMMC_IT_DTIMEOUT: Data timeout interrupt
  *            @arg SDMMC_IT_TXUNDERR: Transmit FIFO underrun error interrupt
  *            @arg SDMMC_IT_RXOVERR:  Received FIFO overrun error interrupt
  *            @arg SDMMC_IT_CMDREND:  Command response received (CRC check passed) interrupt
  *            @arg SDMMC_IT_CMDSENT:  Command sent (no response required) interrupt
  *            @arg SDMMC_IT_DATAEND:  Data end (data counter, SDIDCOUNT, is zero) interrupt
  *            @arg SDMMC_IT_DBCKEND:  Data block sent/received (CRC check passed) interrupt
  *            @arg SDMMC_IT_CMDACT:   Command transfer in progress interrupt
  *            @arg SDMMC_IT_TXACT:    Data transmit in progress interrupt
  *            @arg SDMMC_IT_RXACT:    Data receive in progress interrupt
  *            @arg SDMMC_IT_TXFIFOHE: Transmit FIFO Half Empty interrupt
  *            @arg SDMMC_IT_RXFIFOHF: Receive FIFO Half Full interrupt
  *            @arg SDMMC_IT_TXFIFOF:  Transmit FIFO full interrupt
  *            @arg SDMMC_IT_RXFIFOF:  Receive FIFO full interrupt
  *            @arg SDMMC_IT_TXFIFOE:  Transmit FIFO empty interrupt
  *            @arg SDMMC_IT_RXFIFOE:  Receive FIFO empty interrupt
  *            @arg SDMMC_IT_TXDAVL:   Data available in transmit FIFO interrupt
  *            @arg SDMMC_IT_RXDAVL:   Data available in receive FIFO interrupt
  *            @arg SDMMC_IT_SDIOIT:   SD I/O interrupt received interrupt
  * @retval None
  */
#define __SDMMC_ENABLE_IT(__INSTANCE__, __INTERRUPT__)  ((__INSTANCE__)->MASK |= (__INTERRUPT__))

/**
  * @brief  Disable the SDMMC device interrupt.
  * @param  __INSTANCE__ : Pointer to SDMMC register base
  * @param  __INTERRUPT__ : specifies the SDMMC interrupt sources to be disabled.
  *          This parameter can be one or a combination of the following values:
  *            @arg SDMMC_IT_CCRCFAIL: Command response received (CRC check failed) interrupt
  *            @arg SDMMC_IT_DCRCFAIL: Data block sent/received (CRC check failed) interrupt
  *            @arg SDMMC_IT_CTIMEOUT: Command response timeout interrupt
  *            @arg SDMMC_IT_DTIMEOUT: Data timeout interrupt
  *            @arg SDMMC_IT_TXUNDERR: Transmit FIFO underrun error interrupt
  *            @arg SDMMC_IT_RXOVERR:  Received FIFO overrun error interrupt
  *            @arg SDMMC_IT_CMDREND:  Command response received (CRC check passed) interrupt
  *            @arg SDMMC_IT_CMDSENT:  Command sent (no response required) interrupt
  *            @arg SDMMC_IT_DATAEND:  Data end (data counter, SDIDCOUNT, is zero) interrupt
  *            @arg SDMMC_IT_DBCKEND:  Data block sent/received (CRC check passed) interrupt
  *            @arg SDMMC_IT_CMDACT:   Command transfer in progress interrupt
  *            @arg SDMMC_IT_TXACT:    Data transmit in progress interrupt
  *            @arg SDMMC_IT_RXACT:    Data receive in progress interrupt
  *            @arg SDMMC_IT_TXFIFOHE: Transmit FIFO Half Empty interrupt
  *            @arg SDMMC_IT_RXFIFOHF: Receive FIFO Half Full interrupt
  *            @arg SDMMC_IT_TXFIFOF:  Transmit FIFO full interrupt
  *            @arg SDMMC_IT_RXFIFOF:  Receive FIFO full interrupt
  *            @arg SDMMC_IT_TXFIFOE:  Transmit FIFO empty interrupt
  *            @arg SDMMC_IT_RXFIFOE:  Receive FIFO empty interrupt
  *            @arg SDMMC_IT_TXDAVL:   Data available in transmit FIFO interrupt
  *            @arg SDMMC_IT_RXDAVL:   Data available in receive FIFO interrupt
  *            @arg SDMMC_IT_SDIOIT:   SD I/O interrupt received interrupt
  * @retval None
  */
#define __SDMMC_DISABLE_IT(__INSTANCE__, __INTERRUPT__)  ((__INSTANCE__)->MASK &= ~(__INTERRUPT__))

/**
  * @brief  Checks whether the specified SDMMC flag is set or not.
  * @param  __INSTANCE__ : Pointer to SDMMC register base
  * @param  __FLAG__: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg SDMMC_FLAG_CCRCFAIL: Command response received (CRC check failed)
  *            @arg SDMMC_FLAG_DCRCFAIL: Data block sent/received (CRC check failed)
  *            @arg SDMMC_FLAG_CTIMEOUT: Command response timeout
  *            @arg SDMMC_FLAG_DTIMEOUT: Data timeout
  *            @arg SDMMC_FLAG_TXUNDERR: Transmit FIFO underrun error
  *            @arg SDMMC_FLAG_RXOVERR:  Received FIFO overrun error
  *            @arg SDMMC_FLAG_CMDREND:  Command response received (CRC check passed)
  *            @arg SDMMC_FLAG_CMDSENT:  Command sent (no response required)
  *            @arg SDMMC_FLAG_DATAEND:  Data end (data counter, SDIDCOUNT, is zero)
  *            @arg SDMMC_FLAG_DBCKEND:  Data block sent/received (CRC check passed)
  *            @arg SDMMC_FLAG_CMDACT:   Command transfer in progress
  *            @arg SDMMC_FLAG_TXACT:    Data transmit in progress
  *            @arg SDMMC_FLAG_RXACT:    Data receive in progress
  *            @arg SDMMC_FLAG_TXFIFOHE: Transmit FIFO Half Empty
  *            @arg SDMMC_FLAG_RXFIFOHF: Receive FIFO Half Full
  *            @arg SDMMC_FLAG_TXFIFOF:  Transmit FIFO full
  *            @arg SDMMC_FLAG_RXFIFOF:  Receive FIFO full
  *            @arg SDMMC_FLAG_TXFIFOE:  Transmit FIFO empty
  *            @arg SDMMC_FLAG_RXFIFOE:  Receive FIFO empty
  *            @arg SDMMC_FLAG_TXDAVL:   Data available in transmit FIFO
  *            @arg SDMMC_FLAG_RXDAVL:   Data available in receive FIFO
  *            @arg SDMMC_FLAG_SDMMCIT:   SD I/O interrupt received
  * @retval The new state of SDMMC_FLAG (SET or RESET).
  */
#define __SDMMC_GET_FLAG(__INSTANCE__, __FLAG__)  (((__INSTANCE__)->STA &(__FLAG__)) != RESET)


/**
  * @brief  Clears the SDMMC pending flags.
  * @param  __INSTANCE__ : Pointer to SDMMC register base
  * @param  __FLAG__: specifies the flag to clear.
  *          This parameter can be one or a combination of the following values:
  *            @arg SDMMC_FLAG_CCRCFAIL: Command response received (CRC check failed)
  *            @arg SDMMC_FLAG_DCRCFAIL: Data block sent/received (CRC check failed)
  *            @arg SDMMC_FLAG_CTIMEOUT: Command response timeout
  *            @arg SDMMC_FLAG_DTIMEOUT: Data timeout
  *            @arg SDMMC_FLAG_TXUNDERR: Transmit FIFO underrun error
  *            @arg SDMMC_FLAG_RXOVERR:  Received FIFO overrun error
  *            @arg SDMMC_FLAG_CMDREND:  Command response received (CRC check passed)
  *            @arg SDMMC_FLAG_CMDSENT:  Command sent (no response required)
  *            @arg SDMMC_FLAG_DATAEND:  Data end (data counter, SDIDCOUNT, is zero)
  *            @arg SDMMC_FLAG_DBCKEND:  Data block sent/received (CRC check passed)
  *            @arg SDMMC_FLAG_SDMMCIT:   SD I/O interrupt received
  * @retval None
  */
#define __SDMMC_CLEAR_FLAG(__INSTANCE__, __FLAG__)  ((__INSTANCE__)->ICR = (__FLAG__))

/**
  * @brief  Checks whether the specified SDMMC interrupt has occurred or not.
  * @param  __INSTANCE__ : Pointer to SDMMC register base
  * @param  __INTERRUPT__: specifies the SDMMC interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg SDMMC_IT_CCRCFAIL: Command response received (CRC check failed) interrupt
  *            @arg SDMMC_IT_DCRCFAIL: Data block sent/received (CRC check failed) interrupt
  *            @arg SDMMC_IT_CTIMEOUT: Command response timeout interrupt
  *            @arg SDMMC_IT_DTIMEOUT: Data timeout interrupt
  *            @arg SDMMC_IT_TXUNDERR: Transmit FIFO underrun error interrupt
  *            @arg SDMMC_IT_RXOVERR:  Received FIFO overrun error interrupt
  *            @arg SDMMC_IT_CMDREND:  Command response received (CRC check passed) interrupt
  *            @arg SDMMC_IT_CMDSENT:  Command sent (no response required) interrupt
  *            @arg SDMMC_IT_DATAEND:  Data end (data counter, SDIDCOUNT, is zero) interrupt
  *            @arg SDMMC_IT_DBCKEND:  Data block sent/received (CRC check passed) interrupt
  *            @arg SDMMC_IT_CMDACT:   Command transfer in progress interrupt
  *            @arg SDMMC_IT_TXACT:    Data transmit in progress interrupt
  *            @arg SDMMC_IT_RXACT:    Data receive in progress interrupt
  *            @arg SDMMC_IT_TXFIFOHE: Transmit FIFO Half Empty interrupt
  *            @arg SDMMC_IT_RXFIFOHF: Receive FIFO Half Full interrupt
  *            @arg SDMMC_IT_TXFIFOF:  Transmit FIFO full interrupt
  *            @arg SDMMC_IT_RXFIFOF:  Receive FIFO full interrupt
  *            @arg SDMMC_IT_TXFIFOE:  Transmit FIFO empty interrupt
  *            @arg SDMMC_IT_RXFIFOE:  Receive FIFO empty interrupt
  *            @arg SDMMC_IT_TXDAVL:   Data available in transmit FIFO interrupt
  *            @arg SDMMC_IT_RXDAVL:   Data available in receive FIFO interrupt
  *            @arg SDMMC_IT_SDIOIT:   SD I/O interrupt received interrupt
  * @retval The new state of SDMMC_IT (SET or RESET).
  */
#define __SDMMC_GET_IT  (__INSTANCE__, __INTERRUPT__)  (((__INSTANCE__)->STA &(__INTERRUPT__)) == (__INTERRUPT__))

/**
  * @brief  Clears the SDMMC's interrupt pending bits.
  * @param  __INSTANCE__ : Pointer to SDMMC register base
  * @param  __INTERRUPT__: specifies the interrupt pending bit to clear.
  *          This parameter can be one or a combination of the following values:
  *            @arg SDMMC_IT_CCRCFAIL: Command response received (CRC check failed) interrupt
  *            @arg SDMMC_IT_DCRCFAIL: Data block sent/received (CRC check failed) interrupt
  *            @arg SDMMC_IT_CTIMEOUT: Command response timeout interrupt
  *            @arg SDMMC_IT_DTIMEOUT: Data timeout interrupt
  *            @arg SDMMC_IT_TXUNDERR: Transmit FIFO underrun error interrupt
  *            @arg SDMMC_IT_RXOVERR:  Received FIFO overrun error interrupt
  *            @arg SDMMC_IT_CMDREND:  Command response received (CRC check passed) interrupt
  *            @arg SDMMC_IT_CMDSENT:  Command sent (no response required) interrupt
  *            @arg SDMMC_IT_DATAEND:  Data end (data counter, SDMMC_DCOUNT, is zero) interrupt
  *            @arg SDMMC_IT_SDIOIT:   SD I/O interrupt received interrupt
  * @retval None
  */
#define __SDMMC_CLEAR_IT(__INSTANCE__, __INTERRUPT__)  ((__INSTANCE__)->ICR = (__INTERRUPT__))

/**
  * @brief  Enable Start the SD I/O Read Wait operation.
  * @param  __INSTANCE__ : Pointer to SDMMC register base
  * @retval None
  */
#define __SDMMC_START_READWAIT_ENABLE(__INSTANCE__)  ((__INSTANCE__)->DCTRL |= SDMMC_DCTRL_RWSTART)

/**
  * @brief  Disable Start the SD I/O Read Wait operations.
  * @param  __INSTANCE__ : Pointer to SDMMC register base
  * @retval None
  */
#define __SDMMC_START_READWAIT_DISABLE(__INSTANCE__)  ((__INSTANCE__)->DCTRL &= ~SDMMC_DCTRL_RWSTART)

/**
  * @brief  Enable Start the SD I/O Read Wait operation.
  * @param  __INSTANCE__ : Pointer to SDMMC register base
  * @retval None
  */
#define __SDMMC_STOP_READWAIT_ENABLE(__INSTANCE__)  ((__INSTANCE__)->DCTRL |= SDMMC_DCTRL_RWSTOP)

/**
  * @brief  Disable Stop the SD I/O Read Wait operations.
  * @param  __INSTANCE__ : Pointer to SDMMC register base
  * @retval None
  */
#define __SDMMC_STOP_READWAIT_DISABLE(__INSTANCE__)  ((__INSTANCE__)->DCTRL &= ~SDMMC_DCTRL_RWSTOP)

/**
  * @brief  Enable the SD I/O Mode Operation.
  * @param  __INSTANCE__ : Pointer to SDMMC register base
  * @retval None
  */
#define __SDMMC_OPERATION_ENABLE(__INSTANCE__)  ((__INSTANCE__)->DCTRL |= SDMMC_DCTRL_SDIOEN)

/**
  * @brief  Disable the SD I/O Mode Operation.
  * @param  __INSTANCE__ : Pointer to SDMMC register base
  * @retval None
  */
#define __SDMMC_OPERATION_DISABLE(__INSTANCE__)  ((__INSTANCE__)->DCTRL &= ~SDMMC_DCTRL_SDIOEN)

/**
  * @brief  Enable the SD I/O Suspend command sending.
  * @param  __INSTANCE__ : Pointer to SDMMC register base
  * @retval None
  */
#define __SDMMC_SUSPEND_CMD_ENABLE(__INSTANCE__)  ((__INSTANCE__)->CMD |= SDMMC_CMD_SDIOSUSPEND)

/**
  * @brief  Disable the SD I/O Suspend command sending.
  * @param  __INSTANCE__ : Pointer to SDMMC register base
  * @retval None
  */
#define __SDMMC_SUSPEND_CMD_DISABLE(__INSTANCE__)  ((__INSTANCE__)->CMD &= ~SDMMC_CMD_SDIOSUSPEND)

/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup SDMMC_LL_Exported_Functions
  * @{
  */

/* Initialization/de-initialization functions  **********************************/
/** @addtogroup HAL_SDMMC_LL_Group1
  * @{
  */
HAL_StatusTypeDef SDMMC_Init(SDMMC_TypeDef *SDMMCx, SDMMC_InitTypeDef Init);
/**
  * @}
  */

/* I/O operation functions  *****************************************************/
/** @addtogroup HAL_SDMMC_LL_Group2
  * @{
  */
/* Blocking mode: Polling */
uint32_t          SDMMC_ReadFIFO(SDMMC_TypeDef *SDMMCx);
HAL_StatusTypeDef SDMMC_WriteFIFO(SDMMC_TypeDef *SDMMCx, uint32_t *pWriteData);
/**
  * @}
  */

/* Peripheral Control functions  ************************************************/
/** @addtogroup HAL_SDMMC_LL_Group3
  * @{
  */
HAL_StatusTypeDef SDMMC_PowerState_ON(SDMMC_TypeDef *SDMMCx);
HAL_StatusTypeDef SDMMC_PowerState_OFF(SDMMC_TypeDef *SDMMCx);
uint32_t          SDMMC_GetPowerState(SDMMC_TypeDef *SDMMCx);

/* Command path state machine (CPSM) management functions */
HAL_StatusTypeDef SDMMC_SendCommand(SDMMC_TypeDef *SDMMCx, SDMMC_CmdInitTypeDef *Command);
uint8_t           SDMMC_GetCommandResponse(SDMMC_TypeDef *SDMMCx);
uint32_t          SDMMC_GetResponse(SDMMC_TypeDef *SDMMCx, uint32_t Response);

/* Data path state machine (DPSM) management functions */
HAL_StatusTypeDef SDMMC_DataConfig(SDMMC_TypeDef *SDMMCx, SDMMC_DataInitTypeDef* Data);
uint32_t          SDMMC_GetDataCounter(SDMMC_TypeDef *SDMMCx);
uint32_t          SDMMC_GetFIFOCount(SDMMC_TypeDef *SDMMCx);

/* SDMMC Cards mode management functions */
HAL_StatusTypeDef SDMMC_SetSDMMCReadWaitMode(SDMMC_TypeDef *SDMMCx, uint32_t SDMMC_ReadWaitMode);

/**
  * @}
  */

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

#endif /* __STM32F7xx_LL_SDMMC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
