/**
  ******************************************************************************
  * @file    stm32f7xx_hal_i2c.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    25-June-2015
  * @brief   Header file of I2C HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
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
#ifndef __STM32F7xx_HAL_I2C_H
#define __STM32F7xx_HAL_I2C_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal_def.h"

/** @addtogroup STM32F7xx_HAL_Driver
  * @{
  */

/** @addtogroup I2C
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup I2C_Exported_Types I2C Exported Types
  * @{
  */

/** @defgroup I2C_Configuration_Structure_definition I2C Configuration Structure definition
  * @brief  I2C Configuration Structure definition
  * @{
  */
typedef struct
{
  uint32_t Timing;              /*!< Specifies the I2C_TIMINGR_register value.
                                  This parameter calculated by referring to I2C initialization
                                         section in Reference manual */

  uint32_t OwnAddress1;         /*!< Specifies the first device own address.
                                  This parameter can be a 7-bit or 10-bit address. */

  uint32_t AddressingMode;      /*!< Specifies if 7-bit or 10-bit addressing mode is selected.
                                  This parameter can be a value of @ref I2C_addressing_mode */

  uint32_t DualAddressMode;     /*!< Specifies if dual addressing mode is selected.
                                  This parameter can be a value of @ref I2C_dual_addressing_mode */

  uint32_t OwnAddress2;         /*!< Specifies the second device own address if dual addressing mode is selected
                                  This parameter can be a 7-bit address. */

  uint32_t OwnAddress2Masks;    /*!< Specifies the acknoledge mask address second device own address if dual addressing mode is selected
                                  This parameter can be a value of @ref I2C_own_address2_masks */

  uint32_t GeneralCallMode;     /*!< Specifies if general call mode is selected.
                                  This parameter can be a value of @ref I2C_general_call_addressing_mode */

  uint32_t NoStretchMode;       /*!< Specifies if nostretch mode is selected.
                                  This parameter can be a value of @ref I2C_nostretch_mode */

}I2C_InitTypeDef;

/**
  * @}
  */

/** @defgroup HAL_state_structure_definition HAL state structure definition
  * @brief  HAL State structure definition
  * @{
  */

typedef enum
{
  HAL_I2C_STATE_RESET           = 0x00,  /*!< I2C not yet initialized or disabled         */
  HAL_I2C_STATE_READY           = 0x01,  /*!< I2C initialized and ready for use           */
  HAL_I2C_STATE_BUSY            = 0x02,  /*!< I2C internal process is ongoing             */
  HAL_I2C_STATE_MASTER_BUSY_TX  = 0x12,  /*!< Master Data Transmission process is ongoing */
  HAL_I2C_STATE_MASTER_BUSY_RX  = 0x22,  /*!< Master Data Reception process is ongoing    */
  HAL_I2C_STATE_SLAVE_BUSY_TX   = 0x32,  /*!< Slave Data Transmission process is ongoing  */
  HAL_I2C_STATE_SLAVE_BUSY_RX   = 0x42,  /*!< Slave Data Reception process is ongoing     */
  HAL_I2C_STATE_MEM_BUSY_TX     = 0x52,  /*!< Memory Data Transmission process is ongoing */
  HAL_I2C_STATE_MEM_BUSY_RX     = 0x62,  /*!< Memory Data Reception process is ongoing    */
  HAL_I2C_STATE_TIMEOUT         = 0x03,  /*!< Timeout state                               */
  HAL_I2C_STATE_ERROR           = 0x04   /*!< Reception process is ongoing                */
}HAL_I2C_StateTypeDef;

/**
  * @}
  */

/** @defgroup I2C_Error_Code_definition I2C Error Code definition
  * @brief  I2C Error Code definition
  * @{
  */
#define HAL_I2C_ERROR_NONE      ((uint32_t)0x00000000)    /*!< No error              */
#define HAL_I2C_ERROR_BERR      ((uint32_t)0x00000001)    /*!< BERR error            */
#define HAL_I2C_ERROR_ARLO      ((uint32_t)0x00000002)    /*!< ARLO error            */
#define HAL_I2C_ERROR_AF        ((uint32_t)0x00000004)    /*!< ACKF error            */
#define HAL_I2C_ERROR_OVR       ((uint32_t)0x00000008)    /*!< OVR error             */
#define HAL_I2C_ERROR_DMA       ((uint32_t)0x00000010)    /*!< DMA transfer error    */
#define HAL_I2C_ERROR_TIMEOUT   ((uint32_t)0x00000020)    /*!< Timeout error         */
#define HAL_I2C_ERROR_SIZE      ((uint32_t)0x00000040)    /*!< Size Management error */
/**
  * @}
  */

/** @defgroup I2C_handle_Structure_definition I2C handle Structure definition
  * @brief  I2C handle Structure definition
  * @{
  */
typedef struct
{
  I2C_TypeDef                *Instance;  /*!< I2C registers base address     */

  I2C_InitTypeDef            Init;       /*!< I2C communication parameters   */

  uint8_t                    *pBuffPtr;  /*!< Pointer to I2C transfer buffer */

  uint16_t                   XferSize;   /*!< I2C transfer size              */

  __IO uint16_t              XferCount;  /*!< I2C transfer counter           */

  DMA_HandleTypeDef          *hdmatx;    /*!< I2C Tx DMA handle parameters   */

  DMA_HandleTypeDef          *hdmarx;    /*!< I2C Rx DMA handle parameters   */

  HAL_LockTypeDef            Lock;       /*!< I2C locking object             */

  __IO HAL_I2C_StateTypeDef  State;      /*!< I2C communication state        */

  __IO uint32_t              ErrorCode;  /*!< I2C Error code                   */

}I2C_HandleTypeDef;
/**
  * @}
  */

/**
  * @}
  */
/* Exported constants --------------------------------------------------------*/

/** @defgroup I2C_Exported_Constants I2C Exported Constants
  * @{
  */

/** @defgroup I2C_addressing_mode I2C addressing mode
  * @{
  */
#define I2C_ADDRESSINGMODE_7BIT         ((uint32_t)0x00000001)
#define I2C_ADDRESSINGMODE_10BIT        ((uint32_t)0x00000002)
/**
  * @}
  */

/** @defgroup I2C_dual_addressing_mode I2C dual addressing mode
  * @{
  */
#define I2C_DUALADDRESS_DISABLE         ((uint32_t)0x00000000)
#define I2C_DUALADDRESS_ENABLE          I2C_OAR2_OA2EN
/**
  * @}
  */

/** @defgroup I2C_own_address2_masks I2C own address2 masks
  * @{
  */
#define I2C_OA2_NOMASK                  ((uint8_t)0x00)
#define I2C_OA2_MASK01                  ((uint8_t)0x01)
#define I2C_OA2_MASK02                  ((uint8_t)0x02)
#define I2C_OA2_MASK03                  ((uint8_t)0x03)
#define I2C_OA2_MASK04                  ((uint8_t)0x04)
#define I2C_OA2_MASK05                  ((uint8_t)0x05)
#define I2C_OA2_MASK06                  ((uint8_t)0x06)
#define I2C_OA2_MASK07                  ((uint8_t)0x07)
/**
  * @}
  */

/** @defgroup I2C_general_call_addressing_mode I2C general call addressing mode
  * @{
  */
#define I2C_GENERALCALL_DISABLE         ((uint32_t)0x00000000)
#define I2C_GENERALCALL_ENABLE          I2C_CR1_GCEN
/**
  * @}
  */

/** @defgroup I2C_nostretch_mode I2C nostretch mode
  * @{
  */
#define I2C_NOSTRETCH_DISABLE           ((uint32_t)0x00000000)
#define I2C_NOSTRETCH_ENABLE            I2C_CR1_NOSTRETCH
/**
  * @}
  */

/** @defgroup I2C_Memory_Address_Size I2C Memory Address Size
  * @{
  */
#define I2C_MEMADD_SIZE_8BIT            ((uint32_t)0x00000001)
#define I2C_MEMADD_SIZE_16BIT           ((uint32_t)0x00000002)
/**
  * @}
  */

/** @defgroup I2C_ReloadEndMode_definition I2C ReloadEndMode definition
  * @{
  */
#define  I2C_RELOAD_MODE                I2C_CR2_RELOAD
#define  I2C_AUTOEND_MODE               I2C_CR2_AUTOEND
#define  I2C_SOFTEND_MODE               ((uint32_t)0x00000000)
/**
  * @}
  */

/** @defgroup I2C_StartStopMode_definition I2C StartStopMode definition
  * @{
  */
#define  I2C_NO_STARTSTOP               ((uint32_t)0x00000000)
#define  I2C_GENERATE_STOP              I2C_CR2_STOP
#define  I2C_GENERATE_START_READ        (uint32_t)(I2C_CR2_START | I2C_CR2_RD_WRN)
#define  I2C_GENERATE_START_WRITE       I2C_CR2_START
/**
  * @}
  */

/** @defgroup I2C_Interrupt_configuration_definition I2C Interrupt configuration definition
  * @brief I2C Interrupt definition
  *        Elements values convention: 0xXXXXXXXX
  *           - XXXXXXXX  : Interrupt control mask
  * @{
  */
#define I2C_IT_ERRI                     I2C_CR1_ERRIE
#define I2C_IT_TCI                      I2C_CR1_TCIE
#define I2C_IT_STOPI                    I2C_CR1_STOPIE
#define I2C_IT_NACKI                    I2C_CR1_NACKIE
#define I2C_IT_ADDRI                    I2C_CR1_ADDRIE
#define I2C_IT_RXI                      I2C_CR1_RXIE
#define I2C_IT_TXI                      I2C_CR1_TXIE

/**
  * @}
  */

/** @defgroup I2C_Flag_definition I2C Flag definition
  * @{
  */
#define I2C_FLAG_TXE                    I2C_ISR_TXE
#define I2C_FLAG_TXIS                   I2C_ISR_TXIS
#define I2C_FLAG_RXNE                   I2C_ISR_RXNE
#define I2C_FLAG_ADDR                   I2C_ISR_ADDR
#define I2C_FLAG_AF                     I2C_ISR_NACKF
#define I2C_FLAG_STOPF                  I2C_ISR_STOPF
#define I2C_FLAG_TC                     I2C_ISR_TC
#define I2C_FLAG_TCR                    I2C_ISR_TCR
#define I2C_FLAG_BERR                   I2C_ISR_BERR
#define I2C_FLAG_ARLO                   I2C_ISR_ARLO
#define I2C_FLAG_OVR                    I2C_ISR_OVR
#define I2C_FLAG_PECERR                 I2C_ISR_PECERR
#define I2C_FLAG_TIMEOUT                I2C_ISR_TIMEOUT
#define I2C_FLAG_ALERT                  I2C_ISR_ALERT
#define I2C_FLAG_BUSY                   I2C_ISR_BUSY
#define I2C_FLAG_DIR                    I2C_ISR_DIR
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/

/** @defgroup I2C_Exported_Macros I2C Exported Macros
  * @{
  */

/** @brief Reset I2C handle state
  * @param  __HANDLE__: specifies the I2C Handle.
  * @retval None
  */
#define __HAL_I2C_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_I2C_STATE_RESET)

/** @brief  Enable the specified I2C interrupts.
  * @param  __HANDLE__: specifies the I2C Handle.
  * @param  __INTERRUPT__: specifies the interrupt source to enable.
  *        This parameter can be one of the following values:
  *            @arg I2C_IT_ERRI: Errors interrupt enable
  *            @arg I2C_IT_TCI: Transfer complete interrupt enable
  *            @arg I2C_IT_STOPI: STOP detection interrupt enable
  *            @arg I2C_IT_NACKI: NACK received interrupt enable
  *            @arg I2C_IT_ADDRI: Address match interrupt enable
  *            @arg I2C_IT_RXI: RX interrupt enable
  *            @arg I2C_IT_TXI: TX interrupt enable
  *
  * @retval None
  */

#define __HAL_I2C_ENABLE_IT(__HANDLE__, __INTERRUPT__)   ((__HANDLE__)->Instance->CR1 |= (__INTERRUPT__))

/** @brief  Disable the specified I2C interrupts.
  * @param  __HANDLE__: specifies the I2C Handle.
  * @param  __INTERRUPT__: specifies the interrupt source to disable.
  *        This parameter can be one of the following values:
  *            @arg I2C_IT_ERRI: Errors interrupt enable
  *            @arg I2C_IT_TCI: Transfer complete interrupt enable
  *            @arg I2C_IT_STOPI: STOP detection interrupt enable
  *            @arg I2C_IT_NACKI: NACK received interrupt enable
  *            @arg I2C_IT_ADDRI: Address match interrupt enable
  *            @arg I2C_IT_RXI: RX interrupt enable
  *            @arg I2C_IT_TXI: TX interrupt enable
  *
  * @retval None
  */
#define __HAL_I2C_DISABLE_IT(__HANDLE__, __INTERRUPT__)  ((__HANDLE__)->Instance->CR1 &= (~(__INTERRUPT__)))

/** @brief  Checks if the specified I2C interrupt source is enabled or disabled.
  * @param  __HANDLE__: specifies the I2C Handle.
  * @param  __INTERRUPT__: specifies the I2C interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg I2C_IT_ERRI: Errors interrupt enable
  *            @arg I2C_IT_TCI: Transfer complete interrupt enable
  *            @arg I2C_IT_STOPI: STOP detection interrupt enable
  *            @arg I2C_IT_NACKI: NACK received interrupt enable
  *            @arg I2C_IT_ADDRI: Address match interrupt enable
  *            @arg I2C_IT_RXI: RX interrupt enable
  *            @arg I2C_IT_TXI: TX interrupt enable
  *
  * @retval The new state of __INTERRUPT__ (TRUE or FALSE).
  */
#define __HAL_I2C_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((((__HANDLE__)->Instance->CR1 & (__INTERRUPT__)) == (__INTERRUPT__)) ? SET : RESET)

/** @brief  Checks whether the specified I2C flag is set or not.
  * @param  __HANDLE__: specifies the I2C Handle.
  * @param  __FLAG__: specifies the flag to check.
  *        This parameter can be one of the following values:
  *            @arg I2C_FLAG_TXE:      Transmit data register empty
  *            @arg I2C_FLAG_TXIS:     Transmit interrupt status
  *            @arg I2C_FLAG_RXNE:     Receive data register not empty
  *            @arg I2C_FLAG_ADDR:     Address matched (slave mode)
  *            @arg I2C_FLAG_AF:       Acknowledge failure received flag
  *            @arg I2C_FLAG_STOPF:    STOP detection flag
  *            @arg I2C_FLAG_TC:       Transfer complete (master mode)
  *            @arg I2C_FLAG_TCR:      Transfer complete reload
  *            @arg I2C_FLAG_BERR:     Bus error
  *            @arg I2C_FLAG_ARLO:     Arbitration lost
  *            @arg I2C_FLAG_OVR:      Overrun/Underrun
  *            @arg I2C_FLAG_PECERR:   PEC error in reception
  *            @arg I2C_FLAG_TIMEOUT:  Timeout or Tlow detection flag
  *            @arg I2C_FLAG_ALERT:    SMBus alert
  *            @arg I2C_FLAG_BUSY:     Bus busy
  *            @arg I2C_FLAG_DIR:      Transfer direction (slave mode)
  *
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define I2C_FLAG_MASK  ((uint32_t)0x0001FFFF)
#define __HAL_I2C_GET_FLAG(__HANDLE__, __FLAG__) (((((__HANDLE__)->Instance->ISR) & ((__FLAG__) & I2C_FLAG_MASK)) == ((__FLAG__) & I2C_FLAG_MASK)))

/** @brief  Clears the I2C pending flags which are cleared by writing 1 in a specific bit.
  * @param  __HANDLE__: specifies the I2C Handle.
  * @param  __FLAG__: specifies the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg I2C_FLAG_ADDR:    Address matched (slave mode)
  *            @arg I2C_FLAG_AF:      Acknowledge failure received flag
  *            @arg I2C_FLAG_STOPF:   STOP detection flag
  *            @arg I2C_FLAG_BERR:    Bus error
  *            @arg I2C_FLAG_ARLO:    Arbitration lost
  *            @arg I2C_FLAG_OVR:     Overrun/Underrun
  *            @arg I2C_FLAG_PECERR:  PEC error in reception
  *            @arg I2C_FLAG_TIMEOUT: Timeout or Tlow detection flag
  *            @arg I2C_FLAG_ALERT:   SMBus alert
  *
  * @retval None
  */
#define __HAL_I2C_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->ICR = ((__FLAG__) & I2C_FLAG_MASK))

/** @brief  Enable the specified I2C peripheral.
  * @param  __HANDLE__: specifies the I2C Handle.
  * @retval None
  */
#define __HAL_I2C_ENABLE(__HANDLE__)                            (SET_BIT((__HANDLE__)->Instance->CR1,  I2C_CR1_PE))

/** @brief  Disable the specified I2C peripheral.
  * @param  __HANDLE__: specifies the I2C Handle.
  * @retval None
  */
#define __HAL_I2C_DISABLE(__HANDLE__)                           (CLEAR_BIT((__HANDLE__)->Instance->CR1, I2C_CR1_PE))

/**
  * @}
  */

/* Include I2C HAL Extension module */
#include "stm32f7xx_hal_i2c_ex.h"

/* Exported functions --------------------------------------------------------*/
/** @addtogroup I2C_Exported_Functions
  * @{
  */

/** @addtogroup I2C_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */
/* Initialization and de-initialization functions******************************/
HAL_StatusTypeDef HAL_I2C_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef HAL_I2C_DeInit (I2C_HandleTypeDef *hi2c);
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c);
/**
  * @}
  */

/** @addtogroup I2C_Exported_Functions_Group2 Input and Output operation functions
  * @{
  */
/* IO operation functions  ****************************************************/
 /******* Blocking mode: Polling */
HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Slave_Transmit(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Slave_Receive(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_IsDeviceReady(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Trials, uint32_t Timeout);

 /******* Non-Blocking mode: Interrupt */
HAL_StatusTypeDef HAL_I2C_Master_Transmit_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Master_Receive_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Slave_Transmit_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Slave_Receive_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Mem_Write_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Mem_Read_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);

 /******* Non-Blocking mode: DMA */
HAL_StatusTypeDef HAL_I2C_Master_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Master_Receive_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Slave_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Slave_Receive_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Mem_Write_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Mem_Read_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
/**
  * @}
  */

/** @addtogroup IRQ_Handler_and_Callbacks IRQ Handler and Callbacks
 * @{
 */
/******* I2C IRQHandler and Callbacks used in non blocking modes (Interrupt and DMA) */
void HAL_I2C_EV_IRQHandler(I2C_HandleTypeDef *hi2c);
void HAL_I2C_ER_IRQHandler(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c);
/**
  * @}
  */

/** @addtogroup I2C_Exported_Functions_Group3 Peripheral State and Errors functions
  * @{
  */
/* Peripheral State and Errors functions  *************************************/
HAL_I2C_StateTypeDef HAL_I2C_GetState(I2C_HandleTypeDef *hi2c);
uint32_t             HAL_I2C_GetError(I2C_HandleTypeDef *hi2c);

/**
  * @}
  */

/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/
/** @defgroup I2C_Private_Constants I2C Private Constants
  * @{
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup I2C_Private_Macro I2C Private Macros
  * @{
  */

#define IS_I2C_ADDRESSING_MODE(MODE)    (((MODE) == I2C_ADDRESSINGMODE_7BIT) || \
                                          ((MODE) == I2C_ADDRESSINGMODE_10BIT))

#define IS_I2C_DUAL_ADDRESS(ADDRESS)    (((ADDRESS) == I2C_DUALADDRESS_DISABLE) || \
                                          ((ADDRESS) == I2C_DUALADDRESS_ENABLE))

#define IS_I2C_OWN_ADDRESS2_MASK(MASK)  (((MASK) == I2C_OA2_NOMASK)  || \
                                          ((MASK) == I2C_OA2_MASK01) || \
                                          ((MASK) == I2C_OA2_MASK02) || \
                                          ((MASK) == I2C_OA2_MASK03) || \
                                          ((MASK) == I2C_OA2_MASK04) || \
                                          ((MASK) == I2C_OA2_MASK05) || \
                                          ((MASK) == I2C_OA2_MASK06) || \
                                          ((MASK) == I2C_OA2_MASK07))

#define IS_I2C_GENERAL_CALL(CALL)       (((CALL) == I2C_GENERALCALL_DISABLE) || \
                                          ((CALL) == I2C_GENERALCALL_ENABLE))

#define IS_I2C_NO_STRETCH(STRETCH)      (((STRETCH) == I2C_NOSTRETCH_DISABLE) || \
                                          ((STRETCH) == I2C_NOSTRETCH_ENABLE))

#define IS_I2C_MEMADD_SIZE(SIZE)        (((SIZE) == I2C_MEMADD_SIZE_8BIT) || \
                                          ((SIZE) == I2C_MEMADD_SIZE_16BIT))


#define IS_TRANSFER_MODE(MODE)          (((MODE) == I2C_RELOAD_MODE)   || \
                                          ((MODE) == I2C_AUTOEND_MODE) || \
                                          ((MODE) == I2C_SOFTEND_MODE))

#define IS_TRANSFER_REQUEST(REQUEST)    (((REQUEST) == I2C_GENERATE_STOP)         || \
                                          ((REQUEST) == I2C_GENERATE_START_READ)  || \
                                          ((REQUEST) == I2C_GENERATE_START_WRITE) || \
                                          ((REQUEST) == I2C_NO_STARTSTOP))


#define I2C_RESET_CR2(__HANDLE__)       ((__HANDLE__)->Instance->CR2 &= (uint32_t)~((uint32_t)(I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN)))

#define IS_I2C_OWN_ADDRESS1(ADDRESS1)   ((ADDRESS1) <= (uint32_t)0x000003FF)
#define IS_I2C_OWN_ADDRESS2(ADDRESS2)   ((ADDRESS2) <= (uint16_t)0x00FF)

#define I2C_MEM_ADD_MSB(__ADDRESS__)    ((uint8_t)((uint16_t)(((uint16_t)((__ADDRESS__) & (uint16_t)(0xFF00))) >> 8)))
#define I2C_MEM_ADD_LSB(__ADDRESS__)    ((uint8_t)((uint16_t)((__ADDRESS__) & (uint16_t)(0x00FF))))

#define I2C_GENERATE_START(__ADDMODE__,__ADDRESS__)   (((__ADDMODE__) == I2C_ADDRESSINGMODE_7BIT) ? (uint32_t)((((uint32_t)(__ADDRESS__) & (I2C_CR2_SADD)) | (I2C_CR2_START) | (I2C_CR2_AUTOEND)) & (~I2C_CR2_RD_WRN)) : \
                                                          (uint32_t)((((uint32_t)(__ADDRESS__) & (I2C_CR2_SADD)) | (I2C_CR2_ADD10) | (I2C_CR2_START)) & (~I2C_CR2_RD_WRN)))
/**
  * @}
  */

/* Private Functions ---------------------------------------------------------*/
/** @defgroup I2C_Private_Functions I2C Private Functions
  * @{
  */
/* Private functions are defined in stm32f7xx_hal_i2c.c file */
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


#endif /* __STM32F7xx_HAL_I2C_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

