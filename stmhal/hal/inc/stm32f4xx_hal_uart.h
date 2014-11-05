/**
  ******************************************************************************
  * @file    stm32f4xx_hal_uart.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   Header file of UART HAL module.
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
#ifndef __STM32F4xx_HAL_UART_H
#define __STM32F4xx_HAL_UART_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup UART
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 

/** 
  * @brief UART Init Structure definition  
  */ 
typedef struct
{
  uint32_t BaudRate;                  /*!< This member configures the UART communication baud rate.
                                           The baud rate is computed using the following formula:
                                           - IntegerDivider = ((PCLKx) / (8 * (OVR8+1) * (huart->Init.BaudRate)))
                                           - FractionalDivider = ((IntegerDivider - ((uint32_t) IntegerDivider)) * 8 * (OVR8+1)) + 0.5 
                                           Where OVR8 is the "oversampling by 8 mode" configuration bit in the CR1 register. */

  uint32_t WordLength;                /*!< Specifies the number of data bits transmitted or received in a frame.
                                           This parameter can be a value of @ref UART_Word_Length */

  uint32_t StopBits;                  /*!< Specifies the number of stop bits transmitted.
                                           This parameter can be a value of @ref UART_Stop_Bits */

  uint32_t Parity;                    /*!< Specifies the parity mode.
                                           This parameter can be a value of @ref UART_Parity
                                           @note When parity is enabled, the computed parity is inserted
                                                 at the MSB position of the transmitted data (9th bit when
                                                 the word length is set to 9 data bits; 8th bit when the
                                                 word length is set to 8 data bits). */
 
  uint32_t Mode;                      /*!< Specifies wether the Receive or Transmit mode is enabled or disabled.
                                           This parameter can be a value of @ref UART_Mode */

  uint32_t HwFlowCtl;                 /*!< Specifies wether the hardware flow control mode is enabled
                                           or disabled.
                                           This parameter can be a value of @ref UART_Hardware_Flow_Control */
  
  uint32_t OverSampling;              /*!< Specifies wether the Over sampling 8 is enabled or disabled, to achieve higher speed (up to fPCLK/8).
                                           This parameter can be a value of @ref UART_Over_Sampling */ 
}UART_InitTypeDef;

/** 
  * @brief HAL UART State structures definition  
  */ 
typedef enum
{
  HAL_UART_STATE_RESET             = 0x00,    /*!< Peripheral is not yet Initialized                  */
  HAL_UART_STATE_READY             = 0x01,    /*!< Peripheral Initialized and ready for use           */
  HAL_UART_STATE_BUSY              = 0x02,    /*!< an internal process is ongoing                     */   
  HAL_UART_STATE_BUSY_TX           = 0x12,    /*!< Data Transmission process is ongoing               */ 
  HAL_UART_STATE_BUSY_RX           = 0x22,    /*!< Data Reception process is ongoing                  */
  HAL_UART_STATE_BUSY_TX_RX        = 0x32,    /*!< Data Transmission and Reception process is ongoing */  
  HAL_UART_STATE_TIMEOUT           = 0x03,    /*!< Timeout state                                      */
  HAL_UART_STATE_ERROR             = 0x04     /*!< Error                                              */      
}HAL_UART_StateTypeDef;

/** 
  * @brief  HAL UART Error Code structure definition  
  */ 
typedef enum
{
  HAL_UART_ERROR_NONE      = 0x00,    /*!< No error            */
  HAL_UART_ERROR_PE        = 0x01,    /*!< Parity error        */
  HAL_UART_ERROR_NE        = 0x02,    /*!< Noise error         */
  HAL_UART_ERROR_FE        = 0x04,    /*!< frame error         */
  HAL_UART_ERROR_ORE       = 0x08,    /*!< Overrun error       */
  HAL_UART_ERROR_DMA       = 0x10     /*!< DMA transfer error  */
}HAL_UART_ErrorTypeDef;

/** 
  * @brief  UART handle Structure definition  
  */  
typedef struct
{
  USART_TypeDef                 *Instance;        /* UART registers base address        */
  
  UART_InitTypeDef              Init;             /* UART communication parameters      */
  
  uint8_t                       *pTxBuffPtr;      /* Pointer to UART Tx transfer Buffer */
  
  uint16_t                      TxXferSize;       /* UART Tx Transfer size              */
  
  uint16_t                      TxXferCount;      /* UART Tx Transfer Counter           */
  
  uint8_t                       *pRxBuffPtr;      /* Pointer to UART Rx transfer Buffer */
  
  uint16_t                      RxXferSize;       /* UART Rx Transfer size              */
  
  uint16_t                      RxXferCount;      /* UART Rx Transfer Counter           */  
  
  DMA_HandleTypeDef             *hdmatx;          /* UART Tx DMA Handle parameters      */
    
  DMA_HandleTypeDef             *hdmarx;          /* UART Rx DMA Handle parameters      */
  
  HAL_LockTypeDef               Lock;            /* Locking object                     */

  __IO HAL_UART_StateTypeDef    State;            /* UART communication state           */
  
  __IO HAL_UART_ErrorTypeDef    ErrorCode;        /* UART Error code                    */
  
}UART_HandleTypeDef;

/* Exported constants --------------------------------------------------------*/
/** @defgroup UART_Exported_Constants
  * @{
  */
  
/** @defgroup UART_Word_Length 
  * @{
  */
#define UART_WORDLENGTH_8B                  ((uint32_t)0x00000000)
#define UART_WORDLENGTH_9B                  ((uint32_t)USART_CR1_M)
#define IS_UART_WORD_LENGTH(LENGTH) (((LENGTH) == UART_WORDLENGTH_8B) || \
                                     ((LENGTH) == UART_WORDLENGTH_9B))
/**
  * @}
  */

/** @defgroup UART_Stop_Bits 
  * @{
  */
#define UART_STOPBITS_1                     ((uint32_t)0x00000000)
#define UART_STOPBITS_2                     ((uint32_t)USART_CR2_STOP_1)
#define IS_UART_STOPBITS(STOPBITS) (((STOPBITS) == UART_STOPBITS_1) || \
                                    ((STOPBITS) == UART_STOPBITS_2))
/**
  * @}
  */ 

/** @defgroup UART_Parity 
  * @{
  */ 
#define UART_PARITY_NONE                    ((uint32_t)0x00000000)
#define UART_PARITY_EVEN                    ((uint32_t)USART_CR1_PCE)
#define UART_PARITY_ODD                     ((uint32_t)(USART_CR1_PCE | USART_CR1_PS)) 
#define IS_UART_PARITY(PARITY) (((PARITY) == UART_PARITY_NONE) || \
                                ((PARITY) == UART_PARITY_EVEN) || \
                                ((PARITY) == UART_PARITY_ODD))
/**
  * @}
  */ 

/** @defgroup UART_Hardware_Flow_Control 
  * @{
  */ 
#define UART_HWCONTROL_NONE                  ((uint32_t)0x00000000)
#define UART_HWCONTROL_RTS                   ((uint32_t)USART_CR3_RTSE)
#define UART_HWCONTROL_CTS                   ((uint32_t)USART_CR3_CTSE)
#define UART_HWCONTROL_RTS_CTS               ((uint32_t)(USART_CR3_RTSE | USART_CR3_CTSE))
#define IS_UART_HARDWARE_FLOW_CONTROL(CONTROL)\
                              (((CONTROL) == UART_HWCONTROL_NONE) || \
                               ((CONTROL) == UART_HWCONTROL_RTS) || \
                               ((CONTROL) == UART_HWCONTROL_CTS) || \
                               ((CONTROL) == UART_HWCONTROL_RTS_CTS))
/**
  * @}
  */

/** @defgroup UART_Mode 
  * @{
  */ 
#define UART_MODE_RX                        ((uint32_t)USART_CR1_RE)
#define UART_MODE_TX                        ((uint32_t)USART_CR1_TE)
#define UART_MODE_TX_RX                     ((uint32_t)(USART_CR1_TE |USART_CR1_RE))
#define IS_UART_MODE(MODE) ((((MODE) & (uint32_t)0x0000FFF3) == 0x00) && ((MODE) != (uint32_t)0x000000))
/**
  * @}
  */
    
 /** @defgroup UART_State 
  * @{
  */ 
#define UART_STATE_DISABLE                  ((uint32_t)0x00000000)
#define UART_STATE_ENABLE                   ((uint32_t)USART_CR1_UE)
#define IS_UART_STATE(STATE) (((STATE) == UART_STATE_DISABLE) || \
                              ((STATE) == UART_STATE_ENABLE))
/**
  * @}
  */

/** @defgroup UART_Over_Sampling 
  * @{
  */
#define UART_OVERSAMPLING_16                    ((uint32_t)0x00000000)
#define UART_OVERSAMPLING_8                     ((uint32_t)USART_CR1_OVER8)
#define IS_UART_OVERSAMPLING(SAMPLING) (((SAMPLING) == UART_OVERSAMPLING_16) || \
                                        ((SAMPLING) == UART_OVERSAMPLING_8))
/**
  * @}
  */

/** @defgroup UART_LIN_Break_Detection_Length 
  * @{
  */  
#define UART_LINBREAKDETECTLENGTH_10B      ((uint32_t)0x00000000)
#define UART_LINBREAKDETECTLENGTH_11B      ((uint32_t)0x00000020)
#define IS_UART_LIN_BREAK_DETECT_LENGTH(LENGTH) (((LENGTH) == UART_LINBREAKDETECTLENGTH_10B) || \
                                                 ((LENGTH) == UART_LINBREAKDETECTLENGTH_11B))
/**
  * @}
  */
                                         
/** @defgroup UART_WakeUp_functions
  * @{
  */
#define UART_WAKEUPMETHODE_IDLELINE                ((uint32_t)0x00000000)
#define UART_WAKEUPMETHODE_ADDRESSMARK             ((uint32_t)0x00000800)
#define IS_UART_WAKEUPMETHODE(WAKEUP) (((WAKEUP) == UART_WAKEUPMETHODE_IDLELINE) || \
                                       ((WAKEUP) == UART_WAKEUPMETHODE_ADDRESSMARK))
/**
  * @}
  */

/** @defgroup UART_Flags 
  *        Elements values convention: 0xXXXX
  *           - 0xXXXX  : Flag mask in the SR register
  * @{
  */
#define UART_FLAG_CTS                       ((uint32_t)0x00000200)
#define UART_FLAG_LBD                       ((uint32_t)0x00000100)
#define UART_FLAG_TXE                       ((uint32_t)0x00000080)
#define UART_FLAG_TC                        ((uint32_t)0x00000040)
#define UART_FLAG_RXNE                      ((uint32_t)0x00000020)
#define UART_FLAG_IDLE                      ((uint32_t)0x00000010)
#define UART_FLAG_ORE                       ((uint32_t)0x00000008)
#define UART_FLAG_NE                        ((uint32_t)0x00000004)
#define UART_FLAG_FE                        ((uint32_t)0x00000002)
#define UART_FLAG_PE                        ((uint32_t)0x00000001)
/**
  * @}
  */

/** @defgroup UART_Interrupt_definition 
  *        Elements values convention: 0xY000XXXX
  *           - XXXX  : Interrupt mask in the XX register
  *           - Y  : Interrupt source register (2bits)
  *                 - 01: CR1 register
  *                 - 10: CR2 register
  *                 - 11: CR3 register
  *
  * @{
  */  
#define UART_IT_PE                          ((uint32_t)0x10000100)
#define UART_IT_TXE                         ((uint32_t)0x10000080)
#define UART_IT_TC                          ((uint32_t)0x10000040)
#define UART_IT_RXNE                        ((uint32_t)0x10000020)
#define UART_IT_IDLE                        ((uint32_t)0x10000010)

#define UART_IT_LBD                         ((uint32_t)0x20000040)
#define UART_IT_CTS                         ((uint32_t)0x30000400)

#define UART_IT_ERR                         ((uint32_t)0x30000001)

/**
  * @}
  */

/**
  * @}
  */
  
/* Exported macro ------------------------------------------------------------*/

/** @brief Reset UART handle state
  * @param  __HANDLE__: specifies the UART Handle.
  *         This parameter can be UARTx where x: 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or 
  *         UART peripheral.
  * @retval None
  */
#define __HAL_UART_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = HAL_UART_STATE_RESET)

/** @brief  Flushs the UART DR register 
  * @param  __HANDLE__: specifies the UART Handle.
  */
#define __HAL_UART_FLUSH_DRREGISTER(__HANDLE__) ((__HANDLE__)->Instance->DR)

/** @brief  Checks whether the specified UART flag is set or not.
  * @param  __HANDLE__: specifies the UART Handle.
  *         This parameter can be UARTx where x: 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or 
  *         UART peripheral.
  * @param  __FLAG__: specifies the flag to check.
  *        This parameter can be one of the following values:
  *            @arg UART_FLAG_CTS:  CTS Change flag (not available for UART4 and UART5)
  *            @arg UART_FLAG_LBD:  LIN Break detection flag
  *            @arg UART_FLAG_TXE:  Transmit data register empty flag
  *            @arg UART_FLAG_TC:   Transmission Complete flag
  *            @arg UART_FLAG_RXNE: Receive data register not empty flag
  *            @arg UART_FLAG_IDLE: Idle Line detection flag
  *            @arg UART_FLAG_ORE:  OverRun Error flag
  *            @arg UART_FLAG_NE:   Noise Error flag
  *            @arg UART_FLAG_FE:   Framing Error flag
  *            @arg UART_FLAG_PE:   Parity Error flag
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */

#define __HAL_UART_GET_FLAG(__HANDLE__, __FLAG__) (((__HANDLE__)->Instance->SR & (__FLAG__)) == (__FLAG__))   
     
/** @brief  Clears the specified UART pending flag.
  * @param  __HANDLE__: specifies the UART Handle.
  *         This parameter can be UARTx where x: 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or 
  *         UART peripheral.
  * @param  __FLAG__: specifies the flag to check.
  *          This parameter can be any combination of the following values:
  *            @arg UART_FLAG_CTS:  CTS Change flag (not available for UART4 and UART5).
  *            @arg UART_FLAG_LBD:  LIN Break detection flag.
  *            @arg UART_FLAG_TC:   Transmission Complete flag.
  *            @arg UART_FLAG_RXNE: Receive data register not empty flag.
  *   
  * @note   PE (Parity error), FE (Framing error), NE (Noise error), ORE (OverRun 
  *          error) and IDLE (Idle line detected) flags are cleared by software 
  *          sequence: a read operation to USART_SR register followed by a read
  *          operation to USART_DR register.
  * @note   RXNE flag can be also cleared by a read to the USART_DR register.
  * @note   TC flag can be also cleared by software sequence: a read operation to 
  *          USART_SR register followed by a write operation to USART_DR register.
  * @note   TXE flag is cleared only by a write to the USART_DR register.
  *   
  * @retval None
  */
#define __HAL_UART_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->SR = ~(__FLAG__))

/** @brief  Clear the UART PE pending flag.
  * @param  __HANDLE__: specifies the UART Handle.
  *         This parameter can be UARTx where x: 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or 
  *         UART peripheral.
  * @retval None
  */
#define __HAL_UART_CLEAR_PEFLAG(__HANDLE__) do{(__HANDLE__)->Instance->SR;\
                                               (__HANDLE__)->Instance->DR;}while(0)
/** @brief  Clear the UART FE pending flag.
  * @param  __HANDLE__: specifies the UART Handle.
  *         This parameter can be UARTx where x: 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or 
  *         UART peripheral.
  * @retval None
  */
#define __HAL_UART_CLEAR_FEFLAG(__HANDLE__) __HAL_UART_CLEAR_PEFLAG(__HANDLE__)

/** @brief  Clear the UART NE pending flag.
  * @param  __HANDLE__: specifies the UART Handle.
  *         This parameter can be UARTx where x: 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or 
  *         UART peripheral.
  * @retval None
  */
#define __HAL_UART_CLEAR_NEFLAG(__HANDLE__) __HAL_UART_CLEAR_PEFLAG(__HANDLE__)

/** @brief  Clear the UART ORE pending flag.
  * @param  __HANDLE__: specifies the UART Handle.
  *         This parameter can be UARTx where x: 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or 
  *         UART peripheral.
  * @retval None
  */
#define __HAL_UART_CLEAR_OREFLAG(__HANDLE__) __HAL_UART_CLEAR_PEFLAG(__HANDLE__)

/** @brief  Clear the UART IDLE pending flag.
  * @param  __HANDLE__: specifies the UART Handle.
  *         This parameter can be UARTx where x: 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or 
  *         UART peripheral.
  * @retval None
  */
#define __HAL_UART_CLEAR_IDLEFLAG(__HANDLE__) __HAL_UART_CLEAR_PEFLAG(__HANDLE__)
                                                 
/** @brief  Enables or disables the specified UART interrupt.
  * @param  __HANDLE__: specifies the UART Handle.
  *         This parameter can be UARTx where x: 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or 
  *         UART peripheral.
  * @param  __INTERRUPT__: specifies the UART interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg UART_IT_CTS:  CTS change interrupt
  *            @arg UART_IT_LBD:  LIN Break detection interrupt
  *            @arg UART_IT_TXE:  Transmit Data Register empty interrupt
  *            @arg UART_IT_TC:   Transmission complete interrupt
  *            @arg UART_IT_RXNE: Receive Data register not empty interrupt
  *            @arg UART_IT_IDLE: Idle line detection interrupt
  *            @arg UART_IT_PE:   Parity Error interrupt
  *            @arg UART_IT_ERR:  Error interrupt(Frame error, noise error, overrun error)
  * @param  NewState: new state of the specified UART interrupt.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
#define UART_IT_MASK  ((uint32_t)0x0000FFFF)
#define __HAL_UART_ENABLE_IT(__HANDLE__, __INTERRUPT__)   ((((__INTERRUPT__) >> 28) == 1)? ((__HANDLE__)->Instance->CR1 |= ((__INTERRUPT__) & UART_IT_MASK)): \
                                                           (((__INTERRUPT__) >> 28) == 2)? ((__HANDLE__)->Instance->CR2 |=  ((__INTERRUPT__) & UART_IT_MASK)): \
                                                        ((__HANDLE__)->Instance->CR3 |= ((__INTERRUPT__) & UART_IT_MASK)))
#define __HAL_UART_DISABLE_IT(__HANDLE__, __INTERRUPT__)  ((((__INTERRUPT__) >> 28) == 1)? ((__HANDLE__)->Instance->CR1 &= ~((__INTERRUPT__) & UART_IT_MASK)): \
                                                           (((__INTERRUPT__) >> 28) == 2)? ((__HANDLE__)->Instance->CR2 &= ~((__INTERRUPT__) & UART_IT_MASK)): \
                                                        ((__HANDLE__)->Instance->CR3 &= ~ ((__INTERRUPT__) & UART_IT_MASK)))
    
/** @brief  Checks whether the specified UART interrupt has occurred or not.
  * @param  __HANDLE__: specifies the UART Handle.
  *         This parameter can be UARTx where x: 1, 2, 3, 4, 5, 6, 7 or 8 to select the USART or 
  *         UART peripheral.
  * @param  __IT__: specifies the UART interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg UART_IT_CTS: CTS change interrupt (not available for UART4 and UART5)
  *            @arg UART_IT_LBD: LIN Break detection interrupt
  *            @arg UART_IT_TXE: Transmit Data Register empty interrupt
  *            @arg UART_IT_TC:  Transmission complete interrupt
  *            @arg UART_IT_RXNE: Receive Data register not empty interrupt
  *            @arg UART_IT_IDLE: Idle line detection interrupt
  *            @arg USART_IT_ERR: Error interrupt
  * @retval The new state of __IT__ (TRUE or FALSE).
  */
#define __HAL_UART_GET_IT_SOURCE(__HANDLE__, __IT__) (((((__IT__) >> 28) == 1)? (__HANDLE__)->Instance->CR1:(((((uint32_t)(__IT__)) >> 28) == 2)? \
                                                      (__HANDLE__)->Instance->CR2 : (__HANDLE__)->Instance->CR3)) & (((uint32_t)(__IT__)) & UART_IT_MASK))

/** @brief  Enable CTS flow control 
  *         This macro allows to enable CTS hardware flow control for a given UART instance, 
  *         without need to call HAL_UART_Init() function.
  *         As involving direct access to UART registers, usage of this macro should be fully endorsed by user.
  * @note   As macro is expected to be used for modifying CTS Hw flow control feature activation, without need
  *         for USART instance Deinit/Init, following conditions for macro call should be fulfilled :
  *           - UART instance should have already been initialised (through call of HAL_UART_Init() )
  *           - macro could only be called when corresponding UART instance is disabled (i.e __HAL_UART_DISABLE(__HANDLE__))
  *             and should be followed by an Enable macro (i.e __HAL_UART_ENABLE(__HANDLE__)).                                                                                                                  
  * @param  __HANDLE__: specifies the UART Handle.
  *         The Handle Instance can be USART1, USART2 or LPUART.
  * @retval None
  */
#define __HAL_UART_HWCONTROL_CTS_ENABLE(__HANDLE__)        \
  do{                                                      \
    SET_BIT((__HANDLE__)->Instance->CR3, USART_CR3_CTSE);  \
    (__HANDLE__)->Init.HwFlowCtl |= USART_CR3_CTSE;        \
  } while(0)

/** @brief  Disable CTS flow control 
  *         This macro allows to disable CTS hardware flow control for a given UART instance, 
  *         without need to call HAL_UART_Init() function.
  *         As involving direct access to UART registers, usage of this macro should be fully endorsed by user.
  * @note   As macro is expected to be used for modifying CTS Hw flow control feature activation, without need
  *         for USART instance Deinit/Init, following conditions for macro call should be fulfilled :
  *           - UART instance should have already been initialised (through call of HAL_UART_Init() )
  *           - macro could only be called when corresponding UART instance is disabled (i.e __HAL_UART_DISABLE(__HANDLE__))
  *             and should be followed by an Enable macro (i.e __HAL_UART_ENABLE(__HANDLE__)). 
  * @param  __HANDLE__: specifies the UART Handle.
  *         The Handle Instance can be USART1, USART2 or LPUART.
  * @retval None
  */
#define __HAL_UART_HWCONTROL_CTS_DISABLE(__HANDLE__)        \
  do{                                                       \
    CLEAR_BIT((__HANDLE__)->Instance->CR3, USART_CR3_CTSE); \
    (__HANDLE__)->Init.HwFlowCtl &= ~(USART_CR3_CTSE);      \
  } while(0)

/** @brief  Enable RTS flow control 
  *         This macro allows to enable RTS hardware flow control for a given UART instance, 
  *         without need to call HAL_UART_Init() function.
  *         As involving direct access to UART registers, usage of this macro should be fully endorsed by user.
  * @note   As macro is expected to be used for modifying RTS Hw flow control feature activation, without need
  *         for USART instance Deinit/Init, following conditions for macro call should be fulfilled :
  *           - UART instance should have already been initialised (through call of HAL_UART_Init() )
  *           - macro could only be called when corresponding UART instance is disabled (i.e __HAL_UART_DISABLE(__HANDLE__))
  *             and should be followed by an Enable macro (i.e __HAL_UART_ENABLE(__HANDLE__)). 
  * @param  __HANDLE__: specifies the UART Handle.
  *         The Handle Instance can be USART1, USART2 or LPUART.
  * @retval None
  */
#define __HAL_UART_HWCONTROL_RTS_ENABLE(__HANDLE__)       \
  do{                                                     \
    SET_BIT((__HANDLE__)->Instance->CR3, USART_CR3_RTSE); \
    (__HANDLE__)->Init.HwFlowCtl |= USART_CR3_RTSE;       \
  } while(0)

/** @brief  Disable RTS flow control 
  *         This macro allows to disable RTS hardware flow control for a given UART instance, 
  *         without need to call HAL_UART_Init() function.
  *         As involving direct access to UART registers, usage of this macro should be fully endorsed by user.
  * @note   As macro is expected to be used for modifying RTS Hw flow control feature activation, without need
  *         for USART instance Deinit/Init, following conditions for macro call should be fulfilled :
  *           - UART instance should have already been initialised (through call of HAL_UART_Init() )
  *           - macro could only be called when corresponding UART instance is disabled (i.e __HAL_UART_DISABLE(__HANDLE__))
  *             and should be followed by an Enable macro (i.e __HAL_UART_ENABLE(__HANDLE__)). 
  * @param  __HANDLE__: specifies the UART Handle.
  *         The Handle Instance can be USART1, USART2 or LPUART.
  * @retval None
  */
#define __HAL_UART_HWCONTROL_RTS_DISABLE(__HANDLE__)       \
  do{                                                      \
    CLEAR_BIT((__HANDLE__)->Instance->CR3, USART_CR3_RTSE);\
    (__HANDLE__)->Init.HwFlowCtl &= ~(USART_CR3_RTSE);     \
  } while(0)

/** @brief  macros to enables or disables the UART's one bit sampling method
  * @param  __HANDLE__: specifies the UART Handle.  
  * @retval None
  */     
#define __HAL_UART_ONEBIT_ENABLE(__HANDLE__) ((__HANDLE__)->Instance->CR3|= USART_CR3_ONEBIT)
#define __HAL_UART_ONEBIT_DISABLE(__HANDLE__) ((__HANDLE__)->Instance->CR3 &= (uint16_t)~((uint16_t)USART_CR3_ONEBIT))

#define __HAL_UART_ENABLE(__HANDLE__)               ((__HANDLE__)->Instance->CR1 |=  USART_CR1_UE)
#define __HAL_UART_DISABLE(__HANDLE__)              ((__HANDLE__)->Instance->CR1 &=  ~USART_CR1_UE)
    
#define __DIV_SAMPLING16(_PCLK_, _BAUD_)            (((_PCLK_)*25)/(4*(_BAUD_)))
#define __DIVMANT_SAMPLING16(_PCLK_, _BAUD_)        (__DIV_SAMPLING16((_PCLK_), (_BAUD_))/100)
#define __DIVFRAQ_SAMPLING16(_PCLK_, _BAUD_)        (((__DIV_SAMPLING16((_PCLK_), (_BAUD_)) - (__DIVMANT_SAMPLING16((_PCLK_), (_BAUD_)) * 100)) * 16 + 50) / 100)
#define __UART_BRR_SAMPLING16(_PCLK_, _BAUD_)       ((__DIVMANT_SAMPLING16((_PCLK_), (_BAUD_)) << 4)|(__DIVFRAQ_SAMPLING16((_PCLK_), (_BAUD_)) & 0x0F))

#define __DIV_SAMPLING8(_PCLK_, _BAUD_)             (((_PCLK_)*25)/(2*(_BAUD_)))
#define __DIVMANT_SAMPLING8(_PCLK_, _BAUD_)         (__DIV_SAMPLING8((_PCLK_), (_BAUD_))/100)
#define __DIVFRAQ_SAMPLING8(_PCLK_, _BAUD_)         (((__DIV_SAMPLING8((_PCLK_), (_BAUD_)) - (__DIVMANT_SAMPLING8((_PCLK_), (_BAUD_)) * 100)) * 16 + 50) / 100)
#define __UART_BRR_SAMPLING8(_PCLK_, _BAUD_)        ((__DIVMANT_SAMPLING8((_PCLK_), (_BAUD_)) << 4)|(__DIVFRAQ_SAMPLING8((_PCLK_), (_BAUD_)) & 0x0F))

#define IS_UART_BAUDRATE(BAUDRATE) ((BAUDRATE) < 10500001)
#define IS_UART_ADDRESS(ADDRESS) ((ADDRESS) <= 0xF)                             

/* Exported functions --------------------------------------------------------*/
/* Initialization/de-initialization functions  **********************************/
HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_HalfDuplex_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_LIN_Init(UART_HandleTypeDef *huart, uint32_t BreakDetectLength);
HAL_StatusTypeDef HAL_MultiProcessor_Init(UART_HandleTypeDef *huart, uint8_t Address, uint32_t WakeUpMethode);
HAL_StatusTypeDef HAL_UART_DeInit (UART_HandleTypeDef *huart);
void HAL_UART_MspInit(UART_HandleTypeDef *huart);
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart);

/* IO operation functions *******************************************************/
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_DMAPause(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_DMAResume(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_DMAStop(UART_HandleTypeDef *huart);
void HAL_UART_IRQHandler(UART_HandleTypeDef *huart);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);

/* Peripheral Control functions  ************************************************/
HAL_StatusTypeDef HAL_LIN_SendBreak(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_MultiProcessor_EnterMuteMode(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_MultiProcessor_ExitMuteMode(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_HalfDuplex_EnableTransmitter(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_HalfDuplex_EnableReceiver(UART_HandleTypeDef *huart);

/* Peripheral State functions  **************************************************/
HAL_UART_StateTypeDef HAL_UART_GetState(UART_HandleTypeDef *huart);
uint32_t              HAL_UART_GetError(UART_HandleTypeDef *huart);

/**
  * @}
  */ 

/**
  * @}
  */ 
  
#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_UART_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
