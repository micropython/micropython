/**
  ******************************************************************************
  * @file    stm32f4xx_ll_sdmmc.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   Header file of SDMMC HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
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
#ifndef __STM32F4xx_LL_SDMMC_H
#define __STM32F4xx_LL_SDMMC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"

/** @addtogroup STM32F4xx_Driver
  * @{
  */

/** @addtogroup SDMMC
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 

/** @defgroup SDIO_Exported_Types SDIO Exported Types
  * @{
  */
  
/** 
  * @brief  SDMMC Configuration Structure definition  
  */
typedef struct
{
  uint32_t ClockEdge;            /*!< Specifies the clock transition on which the bit capture is made.
                                      This parameter can be a value of @ref SDIO_Clock_Edge                 */

  uint32_t ClockBypass;          /*!< Specifies whether the SDIO Clock divider bypass is
                                      enabled or disabled.
                                      This parameter can be a value of @ref SDIO_Clock_Bypass               */

  uint32_t ClockPowerSave;       /*!< Specifies whether SDIO Clock output is enabled or
                                      disabled when the bus is idle.
                                      This parameter can be a value of @ref SDIO_Clock_Power_Save           */

  uint32_t BusWide;              /*!< Specifies the SDIO bus width.
                                      This parameter can be a value of @ref SDIO_Bus_Wide                   */

  uint32_t HardwareFlowControl;  /*!< Specifies whether the SDIO hardware flow control is enabled or disabled.
                                      This parameter can be a value of @ref SDIO_Hardware_Flow_Control      */

  uint32_t ClockDiv;             /*!< Specifies the clock frequency of the SDIO controller.
                                      This parameter can be a value between Min_Data = 0 and Max_Data = 255 */  
  
}SDIO_InitTypeDef;
  

/** 
  * @brief  SDIO Command Control structure 
  */
typedef struct                                                                                            
{
  uint32_t Argument;            /*!< Specifies the SDIO command argument which is sent
                                     to a card as part of a command message. If a command
                                     contains an argument, it must be loaded into this register
                                     before writing the command to the command register.              */

  uint32_t CmdIndex;            /*!< Specifies the SDIO command index. It must be Min_Data = 0 and 
                                     Max_Data = 64                                                    */

  uint32_t Response;            /*!< Specifies the SDIO response type.
                                     This parameter can be a value of @ref SDIO_Response_Type         */

  uint32_t WaitForInterrupt;    /*!< Specifies whether SDIO wait for interrupt request is 
                                     enabled or disabled.
                                     This parameter can be a value of @ref SDIO_Wait_Interrupt_State  */

  uint32_t CPSM;                /*!< Specifies whether SDIO Command path state machine (CPSM)
                                     is enabled or disabled.
                                     This parameter can be a value of @ref SDIO_CPSM_State            */
}SDIO_CmdInitTypeDef;


/** 
  * @brief  SDIO Data Control structure 
  */
typedef struct
{
  uint32_t DataTimeOut;         /*!< Specifies the data timeout period in card bus clock periods.  */

  uint32_t DataLength;          /*!< Specifies the number of data bytes to be transferred.         */
 
  uint32_t DataBlockSize;       /*!< Specifies the data block size for block transfer.
                                     This parameter can be a value of @ref SDIO_Data_Block_Size    */
 
  uint32_t TransferDir;         /*!< Specifies the data transfer direction, whether the transfer
                                     is a read or write.
                                     This parameter can be a value of @ref SDIO_Transfer_Direction */
 
  uint32_t TransferMode;        /*!< Specifies whether data transfer is in stream or block mode.
                                     This parameter can be a value of @ref SDIO_Transfer_Type      */
 
  uint32_t DPSM;                /*!< Specifies whether SDIO Data path state machine (DPSM)
                                     is enabled or disabled.
                                     This parameter can be a value of @ref SDIO_DPSM_State         */
}SDIO_DataInitTypeDef;

/**
  * @}
  */
  
/* Exported constants --------------------------------------------------------*/

/** @defgroup SDIO_Exported_Constants
  * @{
  */

/** @defgroup SDIO_Clock_Edge 
  * @{
  */
#define SDIO_CLOCK_EDGE_RISING               ((uint32_t)0x00000000)
#define SDIO_CLOCK_EDGE_FALLING              SDIO_CLKCR_NEGEDGE

#define IS_SDIO_CLOCK_EDGE(EDGE) (((EDGE) == SDIO_CLOCK_EDGE_RISING) || \
                                  ((EDGE) == SDIO_CLOCK_EDGE_FALLING))
/**
  * @}
  */

/** @defgroup SDIO_Clock_Bypass 
  * @{
  */
#define SDIO_CLOCK_BYPASS_DISABLE             ((uint32_t)0x00000000)
#define SDIO_CLOCK_BYPASS_ENABLE              SDIO_CLKCR_BYPASS   

#define IS_SDIO_CLOCK_BYPASS(BYPASS) (((BYPASS) == SDIO_CLOCK_BYPASS_DISABLE) || \
                                      ((BYPASS) == SDIO_CLOCK_BYPASS_ENABLE))
/**
  * @}
  */ 

/** @defgroup SDIO_Clock_Power_Save 
  * @{
  */
#define SDIO_CLOCK_POWER_SAVE_DISABLE         ((uint32_t)0x00000000)
#define SDIO_CLOCK_POWER_SAVE_ENABLE          SDIO_CLKCR_PWRSAV

#define IS_SDIO_CLOCK_POWER_SAVE(SAVE) (((SAVE) == SDIO_CLOCK_POWER_SAVE_DISABLE) || \
                                        ((SAVE) == SDIO_CLOCK_POWER_SAVE_ENABLE))
/**
  * @}
  */

/** @defgroup SDIO_Bus_Wide 
  * @{
  */
#define SDIO_BUS_WIDE_1B                      ((uint32_t)0x00000000)
#define SDIO_BUS_WIDE_4B                      SDIO_CLKCR_WIDBUS_0
#define SDIO_BUS_WIDE_8B                      SDIO_CLKCR_WIDBUS_1

#define IS_SDIO_BUS_WIDE(WIDE) (((WIDE) == SDIO_BUS_WIDE_1B) || \
                                ((WIDE) == SDIO_BUS_WIDE_4B) || \
                                ((WIDE) == SDIO_BUS_WIDE_8B))
/**
  * @}
  */

/** @defgroup SDIO_Hardware_Flow_Control 
  * @{
  */
#define SDIO_HARDWARE_FLOW_CONTROL_DISABLE    ((uint32_t)0x00000000)
#define SDIO_HARDWARE_FLOW_CONTROL_ENABLE     SDIO_CLKCR_HWFC_EN

#define IS_SDIO_HARDWARE_FLOW_CONTROL(CONTROL) (((CONTROL) == SDIO_HARDWARE_FLOW_CONTROL_DISABLE) || \
                                                ((CONTROL) == SDIO_HARDWARE_FLOW_CONTROL_ENABLE))
/**
  * @}
  */
  
/** @defgroup SDIO_Clock_Division
  * @{
  */
#define IS_SDIO_CLKDIV(DIV)   ((DIV) <= 0xFF)
/**
  * @}
  */  
    
/** @defgroup SDIO_Command_Index
  * @{
  */
#define IS_SDIO_CMD_INDEX(INDEX)            ((INDEX) < 0x40)
/**
  * @}
  */

/** @defgroup SDIO_Response_Type
  * @{
  */
#define SDIO_RESPONSE_NO                    ((uint32_t)0x00000000)
#define SDIO_RESPONSE_SHORT                 SDIO_CMD_WAITRESP_0
#define SDIO_RESPONSE_LONG                  SDIO_CMD_WAITRESP

#define IS_SDIO_RESPONSE(RESPONSE) (((RESPONSE) == SDIO_RESPONSE_NO)    || \
                                    ((RESPONSE) == SDIO_RESPONSE_SHORT) || \
                                    ((RESPONSE) == SDIO_RESPONSE_LONG))
/**
  * @}
  */

/** @defgroup SDIO_Wait_Interrupt_State
  * @{
  */
#define SDIO_WAIT_NO                        ((uint32_t)0x00000000)
#define SDIO_WAIT_IT                        SDIO_CMD_WAITINT 
#define SDIO_WAIT_PEND                      SDIO_CMD_WAITPEND

#define IS_SDIO_WAIT(WAIT) (((WAIT) == SDIO_WAIT_NO) || \
                            ((WAIT) == SDIO_WAIT_IT) || \
                            ((WAIT) == SDIO_WAIT_PEND))
/**
  * @}
  */

/** @defgroup SDIO_CPSM_State
  * @{
  */
#define SDIO_CPSM_DISABLE                   ((uint32_t)0x00000000)
#define SDIO_CPSM_ENABLE                    SDIO_CMD_CPSMEN

#define IS_SDIO_CPSM(CPSM) (((CPSM) == SDIO_CPSM_DISABLE) || \
                            ((CPSM) == SDIO_CPSM_ENABLE))
/**
  * @}
  */  

/** @defgroup SDIO_Response_Registers
  * @{
  */
#define SDIO_RESP1                          ((uint32_t)0x00000000)
#define SDIO_RESP2                          ((uint32_t)0x00000004)
#define SDIO_RESP3                          ((uint32_t)0x00000008)
#define SDIO_RESP4                          ((uint32_t)0x0000000C)

#define IS_SDIO_RESP(RESP) (((RESP) == SDIO_RESP1) || \
                            ((RESP) == SDIO_RESP2) || \
                            ((RESP) == SDIO_RESP3) || \
                            ((RESP) == SDIO_RESP4))
/**
  * @}
  */

/** @defgroup SDIO_Data_Length 
  * @{
  */
#define IS_SDIO_DATA_LENGTH(LENGTH) ((LENGTH) <= 0x01FFFFFF)
/**
  * @}
  */

/** @defgroup SDIO_Data_Block_Size 
  * @{
  */
#define SDIO_DATABLOCK_SIZE_1B               ((uint32_t)0x00000000)
#define SDIO_DATABLOCK_SIZE_2B               SDIO_DCTRL_DBLOCKSIZE_0
#define SDIO_DATABLOCK_SIZE_4B               SDIO_DCTRL_DBLOCKSIZE_1
#define SDIO_DATABLOCK_SIZE_8B               ((uint32_t)0x00000030)
#define SDIO_DATABLOCK_SIZE_16B              SDIO_DCTRL_DBLOCKSIZE_2
#define SDIO_DATABLOCK_SIZE_32B              ((uint32_t)0x00000050)
#define SDIO_DATABLOCK_SIZE_64B              ((uint32_t)0x00000060)
#define SDIO_DATABLOCK_SIZE_128B             ((uint32_t)0x00000070)
#define SDIO_DATABLOCK_SIZE_256B             SDIO_DCTRL_DBLOCKSIZE_3
#define SDIO_DATABLOCK_SIZE_512B             ((uint32_t)0x00000090)
#define SDIO_DATABLOCK_SIZE_1024B            ((uint32_t)0x000000A0)
#define SDIO_DATABLOCK_SIZE_2048B            ((uint32_t)0x000000B0)
#define SDIO_DATABLOCK_SIZE_4096B            ((uint32_t)0x000000C0)
#define SDIO_DATABLOCK_SIZE_8192B            ((uint32_t)0x000000D0)
#define SDIO_DATABLOCK_SIZE_16384B           ((uint32_t)0x000000E0)

#define IS_SDIO_BLOCK_SIZE(SIZE) (((SIZE) == SDIO_DATABLOCK_SIZE_1B)    || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_2B)    || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_4B)    || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_8B)    || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_16B)   || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_32B)   || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_64B)   || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_128B)  || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_256B)  || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_512B)  || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_1024B) || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_2048B) || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_4096B) || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_8192B) || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_16384B)) 
/**
  * @}
  */

/** @defgroup SDIO_Transfer_Direction 
  * @{
  */
#define SDIO_TRANSFER_DIR_TO_CARD            ((uint32_t)0x00000000)
#define SDIO_TRANSFER_DIR_TO_SDIO            SDIO_DCTRL_DTDIR

#define IS_SDIO_TRANSFER_DIR(DIR) (((DIR) == SDIO_TRANSFER_DIR_TO_CARD) || \
                                   ((DIR) == SDIO_TRANSFER_DIR_TO_SDIO))
/**
  * @}
  */

/** @defgroup SDIO_Transfer_Type 
  * @{
  */
#define SDIO_TRANSFER_MODE_BLOCK             ((uint32_t)0x00000000)
#define SDIO_TRANSFER_MODE_STREAM            SDIO_DCTRL_DTMODE

#define IS_SDIO_TRANSFER_MODE(MODE) (((MODE) == SDIO_TRANSFER_MODE_BLOCK) || \
                                     ((MODE) == SDIO_TRANSFER_MODE_STREAM))
/**
  * @}
  */

/** @defgroup SDIO_DPSM_State 
  * @{
  */
#define SDIO_DPSM_DISABLE                    ((uint32_t)0x00000000)
#define SDIO_DPSM_ENABLE                     SDIO_DCTRL_DTEN

#define IS_SDIO_DPSM(DPSM) (((DPSM) == SDIO_DPSM_DISABLE) ||\
                            ((DPSM) == SDIO_DPSM_ENABLE))
/**
  * @}
  */
  
/** @defgroup SDIO_Read_Wait_Mode 
  * @{
  */
#define SDIO_READ_WAIT_MODE_CLK               ((uint32_t)0x00000000)
#define SDIO_READ_WAIT_MODE_DATA2             ((uint32_t)0x00000001)

#define IS_SDIO_READWAIT_MODE(MODE) (((MODE) == SDIO_READ_WAIT_MODE_CLK) || \
                                     ((MODE) == SDIO_READ_WAIT_MODE_DATA2))
/**
  * @}
  */  

/** @defgroup SDIO_Interrupt_sources
  * @{
  */
#define SDIO_IT_CCRCFAIL                    SDIO_STA_CCRCFAIL
#define SDIO_IT_DCRCFAIL                    SDIO_STA_DCRCFAIL
#define SDIO_IT_CTIMEOUT                    SDIO_STA_CTIMEOUT
#define SDIO_IT_DTIMEOUT                    SDIO_STA_DTIMEOUT
#define SDIO_IT_TXUNDERR                    SDIO_STA_TXUNDERR
#define SDIO_IT_RXOVERR                     SDIO_STA_RXOVERR
#define SDIO_IT_CMDREND                     SDIO_STA_CMDREND
#define SDIO_IT_CMDSENT                     SDIO_STA_CMDSENT
#define SDIO_IT_DATAEND                     SDIO_STA_DATAEND
#define SDIO_IT_STBITERR                    SDIO_STA_STBITERR
#define SDIO_IT_DBCKEND                     SDIO_STA_DBCKEND
#define SDIO_IT_CMDACT                      SDIO_STA_CMDACT
#define SDIO_IT_TXACT                       SDIO_STA_TXACT
#define SDIO_IT_RXACT                       SDIO_STA_RXACT
#define SDIO_IT_TXFIFOHE                    SDIO_STA_TXFIFOHE
#define SDIO_IT_RXFIFOHF                    SDIO_STA_RXFIFOHF
#define SDIO_IT_TXFIFOF                     SDIO_STA_TXFIFOF
#define SDIO_IT_RXFIFOF                     SDIO_STA_RXFIFOF
#define SDIO_IT_TXFIFOE                     SDIO_STA_TXFIFOE
#define SDIO_IT_RXFIFOE                     SDIO_STA_RXFIFOE
#define SDIO_IT_TXDAVL                      SDIO_STA_TXDAVL
#define SDIO_IT_RXDAVL                      SDIO_STA_RXDAVL
#define SDIO_IT_SDIOIT                      SDIO_STA_SDIOIT
#define SDIO_IT_CEATAEND                    SDIO_STA_CEATAEND

#define IS_SDIO_IT(IT) ((((IT) & (uint32_t)0xFF000000) == 0x00) && ((IT) != (uint32_t)0x00))
/**
  * @}
  */ 

/** @defgroup SDIO_Flags 
  * @{
  */
#define SDIO_FLAG_CCRCFAIL                  SDIO_STA_CCRCFAIL
#define SDIO_FLAG_DCRCFAIL                  SDIO_STA_DCRCFAIL
#define SDIO_FLAG_CTIMEOUT                  SDIO_STA_CTIMEOUT
#define SDIO_FLAG_DTIMEOUT                  SDIO_STA_DTIMEOUT
#define SDIO_FLAG_TXUNDERR                  SDIO_STA_TXUNDERR
#define SDIO_FLAG_RXOVERR                   SDIO_STA_RXOVERR
#define SDIO_FLAG_CMDREND                   SDIO_STA_CMDREND
#define SDIO_FLAG_CMDSENT                   SDIO_STA_CMDSENT
#define SDIO_FLAG_DATAEND                   SDIO_STA_DATAEND
#define SDIO_FLAG_STBITERR                  SDIO_STA_STBITERR
#define SDIO_FLAG_DBCKEND                   SDIO_STA_DBCKEND
#define SDIO_FLAG_CMDACT                    SDIO_STA_CMDACT
#define SDIO_FLAG_TXACT                     SDIO_STA_TXACT
#define SDIO_FLAG_RXACT                     SDIO_STA_RXACT
#define SDIO_FLAG_TXFIFOHE                  SDIO_STA_TXFIFOHE
#define SDIO_FLAG_RXFIFOHF                  SDIO_STA_RXFIFOHF
#define SDIO_FLAG_TXFIFOF                   SDIO_STA_TXFIFOF
#define SDIO_FLAG_RXFIFOF                   SDIO_STA_RXFIFOF
#define SDIO_FLAG_TXFIFOE                   SDIO_STA_TXFIFOE
#define SDIO_FLAG_RXFIFOE                   SDIO_STA_RXFIFOE
#define SDIO_FLAG_TXDAVL                    SDIO_STA_TXDAVL
#define SDIO_FLAG_RXDAVL                    SDIO_STA_RXDAVL
#define SDIO_FLAG_SDIOIT                    SDIO_STA_SDIOIT
#define SDIO_FLAG_CEATAEND                  SDIO_STA_CEATAEND

#define IS_SDIO_FLAG(FLAG) (((FLAG)  == SDIO_FLAG_CCRCFAIL) || \
                            ((FLAG)  == SDIO_FLAG_DCRCFAIL) || \
                            ((FLAG)  == SDIO_FLAG_CTIMEOUT) || \
                            ((FLAG)  == SDIO_FLAG_DTIMEOUT) || \
                            ((FLAG)  == SDIO_FLAG_TXUNDERR) || \
                            ((FLAG)  == SDIO_FLAG_RXOVERR)  || \
                            ((FLAG)  == SDIO_FLAG_CMDREND)  || \
                            ((FLAG)  == SDIO_FLAG_CMDSENT)  || \
                            ((FLAG)  == SDIO_FLAG_DATAEND)  || \
                            ((FLAG)  == SDIO_FLAG_STBITERR) || \
                            ((FLAG)  == SDIO_FLAG_DBCKEND)  || \
                            ((FLAG)  == SDIO_FLAG_CMDACT)   || \
                            ((FLAG)  == SDIO_FLAG_TXACT)    || \
                            ((FLAG)  == SDIO_FLAG_RXACT)    || \
                            ((FLAG)  == SDIO_FLAG_TXFIFOHE) || \
                            ((FLAG)  == SDIO_FLAG_RXFIFOHF) || \
                            ((FLAG)  == SDIO_FLAG_TXFIFOF)  || \
                            ((FLAG)  == SDIO_FLAG_RXFIFOF)  || \
                            ((FLAG)  == SDIO_FLAG_TXFIFOE)  || \
                            ((FLAG)  == SDIO_FLAG_RXFIFOE)  || \
                            ((FLAG)  == SDIO_FLAG_TXDAVL)   || \
                            ((FLAG)  == SDIO_FLAG_RXDAVL)   || \
                            ((FLAG)  == SDIO_FLAG_SDIOIT)   || \
                            ((FLAG)  == SDIO_FLAG_CEATAEND))

#define IS_SDIO_CLEAR_FLAG(FLAG) ((((FLAG) & (uint32_t)0xFF3FF800) == 0x00) && ((FLAG) != (uint32_t)0x00))

#define IS_SDIO_GET_IT(IT) (((IT)  == SDIO_IT_CCRCFAIL) || \
                            ((IT)  == SDIO_IT_DCRCFAIL) || \
                            ((IT)  == SDIO_IT_CTIMEOUT) || \
                            ((IT)  == SDIO_IT_DTIMEOUT) || \
                            ((IT)  == SDIO_IT_TXUNDERR) || \
                            ((IT)  == SDIO_IT_RXOVERR)  || \
                            ((IT)  == SDIO_IT_CMDREND)  || \
                            ((IT)  == SDIO_IT_CMDSENT)  || \
                            ((IT)  == SDIO_IT_DATAEND)  || \
                            ((IT)  == SDIO_IT_STBITERR) || \
                            ((IT)  == SDIO_IT_DBCKEND)  || \
                            ((IT)  == SDIO_IT_CMDACT)   || \
                            ((IT)  == SDIO_IT_TXACT)    || \
                            ((IT)  == SDIO_IT_RXACT)    || \
                            ((IT)  == SDIO_IT_TXFIFOHE) || \
                            ((IT)  == SDIO_IT_RXFIFOHF) || \
                            ((IT)  == SDIO_IT_TXFIFOF)  || \
                            ((IT)  == SDIO_IT_RXFIFOF)  || \
                            ((IT)  == SDIO_IT_TXFIFOE)  || \
                            ((IT)  == SDIO_IT_RXFIFOE)  || \
                            ((IT)  == SDIO_IT_TXDAVL)   || \
                            ((IT)  == SDIO_IT_RXDAVL)   || \
                            ((IT)  == SDIO_IT_SDIOIT)   || \
                            ((IT)  == SDIO_IT_CEATAEND))

#define IS_SDIO_CLEAR_IT(IT) ((((IT) & (uint32_t)0xFF3FF800) == 0x00) && ((IT) != (uint32_t)0x00))

/**
  * @}
  */


/** @defgroup SDIO_Instance_definition 
  * @{
  */ 
#define IS_SDIO_ALL_INSTANCE(INSTANCE) ((INSTANCE) == SDIO)

/**
  * @}
  */
  
/* Exported macro ------------------------------------------------------------*/
/* ------------ SDIO registers bit address in the alias region -------------- */
#define SDIO_OFFSET               (SDIO_BASE - PERIPH_BASE)

/* --- CLKCR Register ---*/
/* Alias word address of CLKEN bit */
#define CLKCR_OFFSET              (SDIO_OFFSET + 0x04)
#define CLKEN_BitNumber           0x08
#define CLKCR_CLKEN_BB            (PERIPH_BB_BASE + (CLKCR_OFFSET * 32) + (CLKEN_BitNumber * 4))

/* --- CMD Register ---*/
/* Alias word address of SDIOSUSPEND bit */
#define CMD_OFFSET                (SDIO_OFFSET + 0x0C)
#define SDIOSUSPEND_BitNumber     0x0B
#define CMD_SDIOSUSPEND_BB        (PERIPH_BB_BASE + (CMD_OFFSET * 32) + (SDIOSUSPEND_BitNumber * 4))

/* Alias word address of ENCMDCOMPL bit */
#define ENCMDCOMPL_BitNumber      0x0C
#define CMD_ENCMDCOMPL_BB         (PERIPH_BB_BASE + (CMD_OFFSET * 32) + (ENCMDCOMPL_BitNumber * 4))

/* Alias word address of NIEN bit */
#define NIEN_BitNumber            0x0D
#define CMD_NIEN_BB               (PERIPH_BB_BASE + (CMD_OFFSET * 32) + (NIEN_BitNumber * 4))

/* Alias word address of ATACMD bit */
#define ATACMD_BitNumber          0x0E
#define CMD_ATACMD_BB             (PERIPH_BB_BASE + (CMD_OFFSET * 32) + (ATACMD_BitNumber * 4))

/* --- DCTRL Register ---*/
/* Alias word address of DMAEN bit */
#define DCTRL_OFFSET              (SDIO_OFFSET + 0x2C)
#define DMAEN_BitNumber           0x03
#define DCTRL_DMAEN_BB            (PERIPH_BB_BASE + (DCTRL_OFFSET * 32) + (DMAEN_BitNumber * 4))

/* Alias word address of RWSTART bit */
#define RWSTART_BitNumber         0x08
#define DCTRL_RWSTART_BB          (PERIPH_BB_BASE + (DCTRL_OFFSET * 32) + (RWSTART_BitNumber * 4))

/* Alias word address of RWSTOP bit */
#define RWSTOP_BitNumber          0x09
#define DCTRL_RWSTOP_BB           (PERIPH_BB_BASE + (DCTRL_OFFSET * 32) + (RWSTOP_BitNumber * 4))

/* Alias word address of RWMOD bit */
#define RWMOD_BitNumber           0x0A
#define DCTRL_RWMOD_BB            (PERIPH_BB_BASE + (DCTRL_OFFSET * 32) + (RWMOD_BitNumber * 4))

/* Alias word address of SDIOEN bit */
#define SDIOEN_BitNumber          0x0B
#define DCTRL_SDIOEN_BB           (PERIPH_BB_BASE + (DCTRL_OFFSET * 32) + (SDIOEN_BitNumber * 4))

/* ---------------------- SDIO registers bit mask --------------------------- */
/* --- CLKCR Register ---*/
/* CLKCR register clear mask */ 
#define CLKCR_CLEAR_MASK         ((uint32_t)(SDIO_CLKCR_CLKDIV  | SDIO_CLKCR_PWRSAV |\
                                             SDIO_CLKCR_BYPASS  | SDIO_CLKCR_WIDBUS |\
                                             SDIO_CLKCR_NEGEDGE | SDIO_CLKCR_HWFC_EN))

/* --- PWRCTRL Register ---*/
/* --- DCTRL Register ---*/
/* SDIO DCTRL Clear Mask */
#define DCTRL_CLEAR_MASK         ((uint32_t)(SDIO_DCTRL_DTEN    | SDIO_DCTRL_DTDIR |\
                                             SDIO_DCTRL_DTMODE  | SDIO_DCTRL_DBLOCKSIZE))

/* --- CMD Register ---*/
/* CMD Register clear mask */
#define CMD_CLEAR_MASK           ((uint32_t)(SDIO_CMD_CMDINDEX | SDIO_CMD_WAITRESP |\
                                             SDIO_CMD_WAITINT  | SDIO_CMD_WAITPEND |\
                                             SDIO_CMD_CPSMEN   | SDIO_CMD_SDIOSUSPEND))

/* SDIO RESP Registers Address */
#define SDIO_RESP_ADDR           ((uint32_t)(SDIO_BASE + 0x14))

/* SDIO Intialization Frequency (400KHz max) */
#define SDIO_INIT_CLK_DIV ((uint8_t)0x76)

/* SDIO Data Transfer Frequency (25MHz max) */
#define SDIO_TRANSFER_CLK_DIV ((uint8_t)0x0)

/** @defgroup SDIO_Interrupt_Clock
 *  @brief macros to handle interrupts and specific clock configurations
 * @{
 */
 
/**
  * @brief  Enable the SDIO device.
  * @param  None   
  * @retval None
  */ 
#define __SDIO_ENABLE()   (*(__IO uint32_t *)CLKCR_CLKEN_BB = ENABLE)

/**
  * @brief  Disable the SDIO device.
  * @param  None  
  * @retval None
  */
#define __SDIO_DISABLE()   (*(__IO uint32_t *)CLKCR_CLKEN_BB = DISABLE)

/**
  * @brief  Enable the SDIO DMA transfer.
  * @param  None  
  * @retval None
  */ 
#define __SDIO_DMA_ENABLE()   (*(__IO uint32_t *)DCTRL_DMAEN_BB = ENABLE)

/**
  * @brief  Disable the SDIO DMA transfer.
  * @param  None   
  * @retval None
  */
#define __SDIO_DMA_DISABLE()   (*(__IO uint32_t *)DCTRL_DMAEN_BB = DISABLE)
 
/**
  * @brief  Enable the SDIO device interrupt.
  * @param  __INSTANCE__ : Pointer to SDIO register base  
  * @param  __INTERRUPT__ : specifies the SDIO interrupt sources to be enabled.
  *         This parameter can be one or a combination of the following values:
  *            @arg SDIO_IT_CCRCFAIL: Command response received (CRC check failed) interrupt
  *            @arg SDIO_IT_DCRCFAIL: Data block sent/received (CRC check failed) interrupt
  *            @arg SDIO_IT_CTIMEOUT: Command response timeout interrupt
  *            @arg SDIO_IT_DTIMEOUT: Data timeout interrupt
  *            @arg SDIO_IT_TXUNDERR: Transmit FIFO underrun error interrupt
  *            @arg SDIO_IT_RXOVERR:  Received FIFO overrun error interrupt
  *            @arg SDIO_IT_CMDREND:  Command response received (CRC check passed) interrupt
  *            @arg SDIO_IT_CMDSENT:  Command sent (no response required) interrupt
  *            @arg SDIO_IT_DATAEND:  Data end (data counter, SDIDCOUNT, is zero) interrupt
  *            @arg SDIO_IT_STBITERR: Start bit not detected on all data signals in wide 
  *                                   bus mode interrupt
  *            @arg SDIO_IT_DBCKEND:  Data block sent/received (CRC check passed) interrupt
  *            @arg SDIO_IT_CMDACT:   Command transfer in progress interrupt
  *            @arg SDIO_IT_TXACT:    Data transmit in progress interrupt
  *            @arg SDIO_IT_RXACT:    Data receive in progress interrupt
  *            @arg SDIO_IT_TXFIFOHE: Transmit FIFO Half Empty interrupt
  *            @arg SDIO_IT_RXFIFOHF: Receive FIFO Half Full interrupt
  *            @arg SDIO_IT_TXFIFOF:  Transmit FIFO full interrupt
  *            @arg SDIO_IT_RXFIFOF:  Receive FIFO full interrupt
  *            @arg SDIO_IT_TXFIFOE:  Transmit FIFO empty interrupt
  *            @arg SDIO_IT_RXFIFOE:  Receive FIFO empty interrupt
  *            @arg SDIO_IT_TXDAVL:   Data available in transmit FIFO interrupt
  *            @arg SDIO_IT_RXDAVL:   Data available in receive FIFO interrupt
  *            @arg SDIO_IT_SDIOIT:   SD I/O interrupt received interrupt
  *            @arg SDIO_IT_CEATAEND: CE-ATA command completion signal received for CMD61 interrupt     
  * @retval None
  */
#define __SDIO_ENABLE_IT(__INSTANCE__, __INTERRUPT__)  ((__INSTANCE__)->MASK |= (__INTERRUPT__))

/**
  * @brief  Disable the SDIO device interrupt.
  * @param  __INSTANCE__ : Pointer to SDIO register base   
  * @param  __INTERRUPT__ : specifies the SDIO interrupt sources to be disabled.
  *          This parameter can be one or a combination of the following values:
  *            @arg SDIO_IT_CCRCFAIL: Command response received (CRC check failed) interrupt
  *            @arg SDIO_IT_DCRCFAIL: Data block sent/received (CRC check failed) interrupt
  *            @arg SDIO_IT_CTIMEOUT: Command response timeout interrupt
  *            @arg SDIO_IT_DTIMEOUT: Data timeout interrupt
  *            @arg SDIO_IT_TXUNDERR: Transmit FIFO underrun error interrupt
  *            @arg SDIO_IT_RXOVERR:  Received FIFO overrun error interrupt
  *            @arg SDIO_IT_CMDREND:  Command response received (CRC check passed) interrupt
  *            @arg SDIO_IT_CMDSENT:  Command sent (no response required) interrupt
  *            @arg SDIO_IT_DATAEND:  Data end (data counter, SDIDCOUNT, is zero) interrupt
  *            @arg SDIO_IT_STBITERR: Start bit not detected on all data signals in wide 
  *                                   bus mode interrupt
  *            @arg SDIO_IT_DBCKEND:  Data block sent/received (CRC check passed) interrupt
  *            @arg SDIO_IT_CMDACT:   Command transfer in progress interrupt
  *            @arg SDIO_IT_TXACT:    Data transmit in progress interrupt
  *            @arg SDIO_IT_RXACT:    Data receive in progress interrupt
  *            @arg SDIO_IT_TXFIFOHE: Transmit FIFO Half Empty interrupt
  *            @arg SDIO_IT_RXFIFOHF: Receive FIFO Half Full interrupt
  *            @arg SDIO_IT_TXFIFOF:  Transmit FIFO full interrupt
  *            @arg SDIO_IT_RXFIFOF:  Receive FIFO full interrupt
  *            @arg SDIO_IT_TXFIFOE:  Transmit FIFO empty interrupt
  *            @arg SDIO_IT_RXFIFOE:  Receive FIFO empty interrupt
  *            @arg SDIO_IT_TXDAVL:   Data available in transmit FIFO interrupt
  *            @arg SDIO_IT_RXDAVL:   Data available in receive FIFO interrupt
  *            @arg SDIO_IT_SDIOIT:   SD I/O interrupt received interrupt
  *            @arg SDIO_IT_CEATAEND: CE-ATA command completion signal received for CMD61 interrupt     
  * @retval None
  */
#define __SDIO_DISABLE_IT(__INSTANCE__, __INTERRUPT__)  ((__INSTANCE__)->MASK &= ~(__INTERRUPT__))

/**
  * @brief  Checks whether the specified SDIO flag is set or not. 
  * @param  __INSTANCE__ : Pointer to SDIO register base   
  * @param  __FLAG__: specifies the flag to check. 
  *          This parameter can be one of the following values:
  *            @arg SDIO_FLAG_CCRCFAIL: Command response received (CRC check failed)
  *            @arg SDIO_FLAG_DCRCFAIL: Data block sent/received (CRC check failed)
  *            @arg SDIO_FLAG_CTIMEOUT: Command response timeout
  *            @arg SDIO_FLAG_DTIMEOUT: Data timeout
  *            @arg SDIO_FLAG_TXUNDERR: Transmit FIFO underrun error
  *            @arg SDIO_FLAG_RXOVERR:  Received FIFO overrun error
  *            @arg SDIO_FLAG_CMDREND:  Command response received (CRC check passed)
  *            @arg SDIO_FLAG_CMDSENT:  Command sent (no response required)
  *            @arg SDIO_FLAG_DATAEND:  Data end (data counter, SDIDCOUNT, is zero)
  *            @arg SDIO_FLAG_STBITERR: Start bit not detected on all data signals in wide bus mode.
  *            @arg SDIO_FLAG_DBCKEND:  Data block sent/received (CRC check passed)
  *            @arg SDIO_FLAG_CMDACT:   Command transfer in progress
  *            @arg SDIO_FLAG_TXACT:    Data transmit in progress
  *            @arg SDIO_FLAG_RXACT:    Data receive in progress
  *            @arg SDIO_FLAG_TXFIFOHE: Transmit FIFO Half Empty
  *            @arg SDIO_FLAG_RXFIFOHF: Receive FIFO Half Full
  *            @arg SDIO_FLAG_TXFIFOF:  Transmit FIFO full
  *            @arg SDIO_FLAG_RXFIFOF:  Receive FIFO full
  *            @arg SDIO_FLAG_TXFIFOE:  Transmit FIFO empty
  *            @arg SDIO_FLAG_RXFIFOE:  Receive FIFO empty
  *            @arg SDIO_FLAG_TXDAVL:   Data available in transmit FIFO
  *            @arg SDIO_FLAG_RXDAVL:   Data available in receive FIFO
  *            @arg SDIO_FLAG_SDIOIT:   SD I/O interrupt received
  *            @arg SDIO_FLAG_CEATAEND: CE-ATA command completion signal received for CMD61
  * @retval The new state of SDIO_FLAG (SET or RESET).
  */
#define __SDIO_GET_FLAG(__INSTANCE__, __FLAG__)   (((__INSTANCE__)->STA &(__FLAG__)) != RESET)


/**
  * @brief  Clears the SDIO pending flags.
  * @param  __INSTANCE__ : Pointer to SDIO register base  
  * @param  __FLAG__: specifies the flag to clear.  
  *          This parameter can be one or a combination of the following values:
  *            @arg SDIO_FLAG_CCRCFAIL: Command response received (CRC check failed)
  *            @arg SDIO_FLAG_DCRCFAIL: Data block sent/received (CRC check failed)
  *            @arg SDIO_FLAG_CTIMEOUT: Command response timeout
  *            @arg SDIO_FLAG_DTIMEOUT: Data timeout
  *            @arg SDIO_FLAG_TXUNDERR: Transmit FIFO underrun error
  *            @arg SDIO_FLAG_RXOVERR:  Received FIFO overrun error
  *            @arg SDIO_FLAG_CMDREND:  Command response received (CRC check passed)
  *            @arg SDIO_FLAG_CMDSENT:  Command sent (no response required)
  *            @arg SDIO_FLAG_DATAEND:  Data end (data counter, SDIDCOUNT, is zero)
  *            @arg SDIO_FLAG_STBITERR: Start bit not detected on all data signals in wide bus mode
  *            @arg SDIO_FLAG_DBCKEND:  Data block sent/received (CRC check passed)
  *            @arg SDIO_FLAG_SDIOIT:   SD I/O interrupt received
  *            @arg SDIO_FLAG_CEATAEND: CE-ATA command completion signal received for CMD61
  * @retval None
  */
#define __SDIO_CLEAR_FLAG(__INSTANCE__, __FLAG__)   ((__INSTANCE__)->ICR = (__FLAG__))

/**
  * @brief  Checks whether the specified SDIO interrupt has occurred or not.
  * @param  __INSTANCE__ : Pointer to SDIO register base   
  * @param  __INTERRUPT__: specifies the SDIO interrupt source to check. 
  *          This parameter can be one of the following values:
  *            @arg SDIO_IT_CCRCFAIL: Command response received (CRC check failed) interrupt
  *            @arg SDIO_IT_DCRCFAIL: Data block sent/received (CRC check failed) interrupt
  *            @arg SDIO_IT_CTIMEOUT: Command response timeout interrupt
  *            @arg SDIO_IT_DTIMEOUT: Data timeout interrupt
  *            @arg SDIO_IT_TXUNDERR: Transmit FIFO underrun error interrupt
  *            @arg SDIO_IT_RXOVERR:  Received FIFO overrun error interrupt
  *            @arg SDIO_IT_CMDREND:  Command response received (CRC check passed) interrupt
  *            @arg SDIO_IT_CMDSENT:  Command sent (no response required) interrupt
  *            @arg SDIO_IT_DATAEND:  Data end (data counter, SDIDCOUNT, is zero) interrupt
  *            @arg SDIO_IT_STBITERR: Start bit not detected on all data signals in wide 
  *                                   bus mode interrupt
  *            @arg SDIO_IT_DBCKEND:  Data block sent/received (CRC check passed) interrupt
  *            @arg SDIO_IT_CMDACT:   Command transfer in progress interrupt
  *            @arg SDIO_IT_TXACT:    Data transmit in progress interrupt
  *            @arg SDIO_IT_RXACT:    Data receive in progress interrupt
  *            @arg SDIO_IT_TXFIFOHE: Transmit FIFO Half Empty interrupt
  *            @arg SDIO_IT_RXFIFOHF: Receive FIFO Half Full interrupt
  *            @arg SDIO_IT_TXFIFOF:  Transmit FIFO full interrupt
  *            @arg SDIO_IT_RXFIFOF:  Receive FIFO full interrupt
  *            @arg SDIO_IT_TXFIFOE:  Transmit FIFO empty interrupt
  *            @arg SDIO_IT_RXFIFOE:  Receive FIFO empty interrupt
  *            @arg SDIO_IT_TXDAVL:   Data available in transmit FIFO interrupt
  *            @arg SDIO_IT_RXDAVL:   Data available in receive FIFO interrupt
  *            @arg SDIO_IT_SDIOIT:   SD I/O interrupt received interrupt
  *            @arg SDIO_IT_CEATAEND: CE-ATA command completion signal received for CMD61 interrupt
  * @retval The new state of SDIO_IT (SET or RESET).
  */
#define __SDIO_GET_IT  (__INSTANCE__, __INTERRUPT__)   (((__INSTANCE__)->STA &(__INTERRUPT__)) == (__INTERRUPT__))

/**
  * @brief  Clears the SDIO's interrupt pending bits.
  * @param  __INSTANCE__ : Pointer to SDIO register base 
  * @param  __INTERRUPT__: specifies the interrupt pending bit to clear. 
  *          This parameter can be one or a combination of the following values:
  *            @arg SDIO_IT_CCRCFAIL: Command response received (CRC check failed) interrupt
  *            @arg SDIO_IT_DCRCFAIL: Data block sent/received (CRC check failed) interrupt
  *            @arg SDIO_IT_CTIMEOUT: Command response timeout interrupt
  *            @arg SDIO_IT_DTIMEOUT: Data timeout interrupt
  *            @arg SDIO_IT_TXUNDERR: Transmit FIFO underrun error interrupt
  *            @arg SDIO_IT_RXOVERR:  Received FIFO overrun error interrupt
  *            @arg SDIO_IT_CMDREND:  Command response received (CRC check passed) interrupt
  *            @arg SDIO_IT_CMDSENT:  Command sent (no response required) interrupt
  *            @arg SDIO_IT_DATAEND:  Data end (data counter, SDIO_DCOUNT, is zero) interrupt
  *            @arg SDIO_IT_STBITERR: Start bit not detected on all data signals in wide 
  *                                   bus mode interrupt
  *            @arg SDIO_IT_SDIOIT:   SD I/O interrupt received interrupt
  *            @arg SDIO_IT_CEATAEND: CE-ATA command completion signal received for CMD61
  * @retval None
  */
#define __SDIO_CLEAR_IT(__INSTANCE__, __INTERRUPT__)   ((__INSTANCE__)->ICR = (__INTERRUPT__))

/**
  * @brief  Enable Start the SD I/O Read Wait operation.
  * @param  None  
  * @retval None
  */  
#define __SDIO_START_READWAIT_ENABLE()   (*(__IO uint32_t *) DCTRL_RWSTART_BB = ENABLE)

/**
  * @brief  Disable Start the SD I/O Read Wait operations.
  * @param  None  
  * @retval None
  */  
#define __SDIO_START_READWAIT_DISABLE()   (*(__IO uint32_t *) DCTRL_RWSTART_BB = DISABLE)

/**
  * @brief  Enable Start the SD I/O Read Wait operation.
  * @param  None  
  * @retval None
  */  
#define __SDIO_STOP_READWAIT_ENABLE()   (*(__IO uint32_t *) DCTRL_RWSTOP_BB = ENABLE)

/**
  * @brief  Disable Stop the SD I/O Read Wait operations.
  * @param  None  
  * @retval None
  */  
#define __SDIO_STOP_READWAIT_DISABLE()   (*(__IO uint32_t *) DCTRL_RWSTOP_BB = DISABLE)

/**
  * @brief  Enable the SD I/O Mode Operation.
  * @param  None  
  * @retval None
  */  
#define __SDIO_OPERATION_ENABLE()   (*(__IO uint32_t *) DCTRL_SDIOEN_BB = ENABLE)

/**
  * @brief  Disable the SD I/O Mode Operation.
  * @param  None  
  * @retval None
  */  
#define __SDIO_OPERATION_DISABLE()   (*(__IO uint32_t *) DCTRL_SDIOEN_BB = DISABLE)

/**
  * @brief  Enable the SD I/O Suspend command sending.
  * @param  None  
  * @retval None
  */  
#define __SDIO_SUSPEND_CMD_ENABLE()   (*(__IO uint32_t *) CMD_SDIOSUSPEND_BB = ENABLE)

/**
  * @brief  Disable the SD I/O Suspend command sending.
  * @param  None  
  * @retval None
  */  
#define __SDIO_SUSPEND_CMD_DISABLE()   (*(__IO uint32_t *) CMD_SDIOSUSPEND_BB = DISABLE)
    
/**
  * @brief  Enable the command completion signal.
  * @param  None  
  * @retval None
  */    
#define __SDIO_CEATA_CMD_COMPLETION_ENABLE()   (*(__IO uint32_t *) CMD_ENCMDCOMPL_BB = ENABLE)

/**
  * @brief  Disable the command completion signal.
  * @param  None  
  * @retval None
  */  
#define __SDIO_CEATA_CMD_COMPLETION_DISABLE()   (*(__IO uint32_t *) CMD_ENCMDCOMPL_BB = DISABLE)

/**
  * @brief  Enable the CE-ATA interrupt.
  * @param  None  
  * @retval None
  */    
#define __SDIO_CEATA_ENABLE_IT()   (*(__IO uint32_t *) CMD_NIEN_BB = (uint32_t)0)

/**
  * @brief  Disable the CE-ATA interrupt.
  * @param  None  
  * @retval None
  */  
#define __SDIO_CEATA_DISABLE_IT()   (*(__IO uint32_t *) CMD_NIEN_BB = (uint32_t)1)

/**
  * @brief  Enable send CE-ATA command (CMD61).
  * @param  None  
  * @retval None
  */  
#define __SDIO_CEATA_SENDCMD_ENABLE()   (*(__IO uint32_t *) CMD_ATACMD_BB = ENABLE)

/**
  * @brief  Disable send CE-ATA command (CMD61).
  * @param  None  
  * @retval None
  */  
#define __SDIO_CEATA_SENDCMD_DISABLE()   (*(__IO uint32_t *) CMD_ATACMD_BB = DISABLE)
  
/**
  * @}
  */

/**
  * @}
  */  

/* Exported functions --------------------------------------------------------*/
/** @addtogroup SDIO_Exported_Functions
  * @{
  */
  
/* Initialization/de-initialization functions  **********************************/
/** @addtogroup HAL_SDIO_Group1
  * @{
  */
HAL_StatusTypeDef SDIO_Init(SDIO_TypeDef *SDIOx, SDIO_InitTypeDef Init);
/**
  * @}
  */
  
/* I/O operation functions  *****************************************************/
/** @addtogroup HAL_SDIO_Group2
  * @{
  */
/* Blocking mode: Polling */
uint32_t          SDIO_ReadFIFO(SDIO_TypeDef *SDIOx);
HAL_StatusTypeDef SDIO_WriteFIFO(SDIO_TypeDef *SDIOx, uint32_t *pWriteData);
/**
  * @}
  */
  
/* Peripheral Control functions  ************************************************/
/** @addtogroup HAL_SDIO_Group3
  * @{
  */
HAL_StatusTypeDef SDIO_PowerState_ON(SDIO_TypeDef *SDIOx);
HAL_StatusTypeDef SDIO_PowerState_OFF(SDIO_TypeDef *SDIOx);
uint32_t          SDIO_GetPowerState(SDIO_TypeDef *SDIOx);

/* Command path state machine (CPSM) management functions */
HAL_StatusTypeDef SDIO_SendCommand(SDIO_TypeDef *SDIOx, SDIO_CmdInitTypeDef *SDIO_CmdInitStruct);
uint8_t           SDIO_GetCommandResponse(SDIO_TypeDef *SDIOx);
uint32_t          SDIO_GetResponse(uint32_t SDIO_RESP);

/* Data path state machine (DPSM) management functions */
HAL_StatusTypeDef SDIO_DataConfig(SDIO_TypeDef *SDIOx, SDIO_DataInitTypeDef* SDIO_DataInitStruct);
uint32_t          SDIO_GetDataCounter(SDIO_TypeDef *SDIOx);
uint32_t          SDIO_GetFIFOCount(SDIO_TypeDef *SDIOx);

/* SDIO IO Cards mode management functions */
HAL_StatusTypeDef SDIO_SetSDIOReadWaitMode(uint32_t SDIO_ReadWaitMode);

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

#endif /* __STM32F4xx_LL_SDMMC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
