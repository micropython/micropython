/**
  ******************************************************************************
  * @file    stm32n6xx_hal_uart_ex.c
  * @author  MCD Application Team
  * @brief   Extended UART HAL module driver.
  *          This file provides firmware functions to manage the following extended
  *          functionalities of the Universal Asynchronous Receiver Transmitter Peripheral (UART).
  *           + Initialization and de-initialization functions
  *           + Peripheral Control functions
  *
  *
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
  @verbatim
  ==============================================================================
               ##### UART peripheral extended features  #####
  ==============================================================================
  [..]
    (#) Declare a UART_HandleTypeDef handle structure.

    (#) For the UART RS485 Driver Enable mode, initialize the UART registers
        by calling the HAL_RS485Ex_Init() API.

    (#) FIFO mode enabling/disabling and RX/TX FIFO threshold programming.

        -@- When UART operates in FIFO mode, FIFO mode must be enabled prior
            starting RX/TX transfers. Also RX/TX FIFO thresholds must be
            configured prior starting RX/TX transfers.

  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup UARTEx UARTEx
  * @brief UART Extended HAL module driver
  * @{
  */

#ifdef HAL_UART_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup UARTEX_Private_Constants UARTEx Private Constants
  * @{
  */
/* UART RX FIFO depth */
#define RX_FIFO_DEPTH 16U

/* UART TX FIFO depth */
#define TX_FIFO_DEPTH 16U
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @defgroup UARTEx_Private_Functions UARTEx Private Functions
  * @{
  */
static void UARTEx_Wakeup_AddressConfig(UART_HandleTypeDef *huart, UART_WakeUpTypeDef WakeUpSelection);
static void UARTEx_SetNbDataToProcess(UART_HandleTypeDef *huart);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @defgroup UARTEx_Exported_Functions  UARTEx Exported Functions
  * @{
  */

/** @defgroup UARTEx_Exported_Functions_Group1 Initialization and de-initialization functions
  * @brief    Extended Initialization and Configuration Functions
  *
@verbatim
===============================================================================
            ##### Initialization and Configuration functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to initialize the USARTx or the UARTy
    in asynchronous mode.
      (+) For the asynchronous mode the parameters below can be configured:
        (++) Baud Rate
        (++) Word Length
        (++) Stop Bit
        (++) Parity: If the parity is enabled, then the MSB bit of the data written
             in the data register is transmitted but is changed by the parity bit.
        (++) Hardware flow control
        (++) Receiver/transmitter modes
        (++) Over Sampling Method
        (++) One-Bit Sampling Method
      (+) For the asynchronous mode, the following advanced features can be configured as well:
        (++) TX and/or RX pin level inversion
        (++) data logical level inversion
        (++) RX and TX pins swap
        (++) RX overrun detection disabling
        (++) DMA disabling on RX error
        (++) MSB first on communication line
        (++) auto Baud rate detection
    [..]
    The HAL_RS485Ex_Init() API follows the UART RS485 mode configuration
     procedures (details for the procedures are available in reference manual).

@endverbatim

  Depending on the frame length defined by the M1 and M0 bits (7-bit,
  8-bit or 9-bit), the possible UART formats are listed in the
  following table.

    Table 1. UART frame format.
    +-----------------------------------------------------------------------+
    |  M1 bit |  M0 bit |  PCE bit  |             UART frame                |
    |---------|---------|-----------|---------------------------------------|
    |    0    |    0    |    0      |    | SB |    8 bit data   | STB |     |
    |---------|---------|-----------|---------------------------------------|
    |    0    |    0    |    1      |    | SB | 7 bit data | PB | STB |     |
    |---------|---------|-----------|---------------------------------------|
    |    0    |    1    |    0      |    | SB |    9 bit data   | STB |     |
    |---------|---------|-----------|---------------------------------------|
    |    0    |    1    |    1      |    | SB | 8 bit data | PB | STB |     |
    |---------|---------|-----------|---------------------------------------|
    |    1    |    0    |    0      |    | SB |    7 bit data   | STB |     |
    |---------|---------|-----------|---------------------------------------|
    |    1    |    0    |    1      |    | SB | 6 bit data | PB | STB |     |
    +-----------------------------------------------------------------------+

  * @{
  */

/**
  * @brief Initialize the RS485 Driver enable feature according to the specified
  *         parameters in the UART_InitTypeDef and creates the associated handle.
  * @param huart            UART handle.
  * @param Polarity         Select the driver enable polarity.
  *          This parameter can be one of the following values:
  *          @arg @ref UART_DE_POLARITY_HIGH DE signal is active high
  *          @arg @ref UART_DE_POLARITY_LOW  DE signal is active low
  * @param AssertionTime    Driver Enable assertion time:
  *       5-bit value defining the time between the activation of the DE (Driver Enable)
  *       signal and the beginning of the start bit. It is expressed in sample time
  *       units (1/8 or 1/16 bit time, depending on the oversampling rate)
  * @param DeassertionTime  Driver Enable deassertion time:
  *       5-bit value defining the time between the end of the last stop bit, in a
  *       transmitted message, and the de-activation of the DE (Driver Enable) signal.
  *       It is expressed in sample time units (1/8 or 1/16 bit time, depending on the
  *       oversampling rate).
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RS485Ex_Init(UART_HandleTypeDef *huart, uint32_t Polarity, uint32_t AssertionTime,
                                   uint32_t DeassertionTime)
{
  uint32_t temp;

  /* Check the UART handle allocation */
  if (huart == NULL)
  {
    return HAL_ERROR;
  }
  /* Check the Driver Enable UART instance */
  assert_param(IS_UART_DRIVER_ENABLE_INSTANCE(huart->Instance));

  /* Check the Driver Enable polarity */
  assert_param(IS_UART_DE_POLARITY(Polarity));

  /* Check the Driver Enable assertion time */
  assert_param(IS_UART_ASSERTIONTIME(AssertionTime));

  /* Check the Driver Enable deassertion time */
  assert_param(IS_UART_DEASSERTIONTIME(DeassertionTime));

  if (huart->gState == HAL_UART_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    huart->Lock = HAL_UNLOCKED;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
    UART_InitCallbacksToDefault(huart);

    if (huart->MspInitCallback == NULL)
    {
      huart->MspInitCallback = HAL_UART_MspInit;
    }

    /* Init the low level hardware */
    huart->MspInitCallback(huart);
#else
    /* Init the low level hardware : GPIO, CLOCK, CORTEX */
    HAL_UART_MspInit(huart);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS) */
  }

  huart->gState = HAL_UART_STATE_BUSY;

  /* Disable the Peripheral */
  __HAL_UART_DISABLE(huart);

  /* Perform advanced settings configuration */
  /* For some items, configuration requires to be done prior TE and RE bits are set */
  if (huart->AdvancedInit.AdvFeatureInit != UART_ADVFEATURE_NO_INIT)
  {
    UART_AdvFeatureConfig(huart);
  }

  /* Set the UART Communication parameters */
  if (UART_SetConfig(huart) == HAL_ERROR)
  {
    return HAL_ERROR;
  }

  /* Enable the Driver Enable mode by setting the DEM bit in the CR3 register */
  SET_BIT(huart->Instance->CR3, USART_CR3_DEM);

  /* Set the Driver Enable polarity */
  MODIFY_REG(huart->Instance->CR3, USART_CR3_DEP, Polarity);

  /* Set the Driver Enable assertion and deassertion times */
  temp = (AssertionTime << UART_CR1_DEAT_ADDRESS_LSB_POS);
  temp |= (DeassertionTime << UART_CR1_DEDT_ADDRESS_LSB_POS);
  MODIFY_REG(huart->Instance->CR1, (USART_CR1_DEDT | USART_CR1_DEAT), temp);

  /* Enable the Peripheral */
  __HAL_UART_ENABLE(huart);

  /* TEACK and/or REACK to check before moving huart->gState and huart->RxState to Ready */
  return (UART_CheckIdleState(huart));
}

/**
  * @}
  */

/** @defgroup UARTEx_Exported_Functions_Group2 IO operation functions
  *  @brief Extended functions
  *
@verbatim
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================
    [..]
    This subsection provides a set of Wakeup and FIFO mode related callback functions.
    (#) Wakeup from Stop mode Callback:
        (++) HAL_UARTEx_WakeupCallback()
    (#) TX/RX Fifos Callbacks:
        (++) HAL_UARTEx_RxFifoFullCallback()
        (++) HAL_UARTEx_TxFifoEmptyCallback()
@endverbatim
  * @{
  */

/**
  * @brief UART wakeup from Stop mode callback.
  * @param huart UART handle.
  * @retval None
  */
__weak void HAL_UARTEx_WakeupCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UARTEx_WakeupCallback can be implemented in the user file.
   */
}

/**
  * @brief  UART RX Fifo full callback.
  * @param  huart UART handle.
  * @retval None
  */
__weak void HAL_UARTEx_RxFifoFullCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UARTEx_RxFifoFullCallback can be implemented in the user file.
   */
}

/**
  * @brief  UART TX Fifo empty callback.
  * @param  huart UART handle.
  * @retval None
  */
__weak void HAL_UARTEx_TxFifoEmptyCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UARTEx_TxFifoEmptyCallback can be implemented in the user file.
   */
}

/**
  * @}
  */

/** @defgroup UARTEx_Exported_Functions_Group3 Peripheral Control functions
  * @brief    Extended Peripheral Control functions
  *
@verbatim
 ===============================================================================
                      ##### Peripheral Control functions #####
 ===============================================================================
    [..] This section provides the following functions:
     (+) HAL_MultiProcessorEx_AddressLength_Set() API optionally sets the UART node address
         detection length to more than 4 bits for multiprocessor address mark wake up.
     (+) HAL_UARTEx_StopModeWakeUpSourceConfig() API defines the wake-up from stop mode
         trigger: address match, Start Bit detection or RXNE bit status.
     (+) HAL_UARTEx_EnableStopMode() API enables the UART to wake up the MCU from stop mode
     (+) HAL_UARTEx_DisableStopMode() API disables the above functionality
     (+) HAL_UARTEx_EnableFifoMode() API enables the FIFO mode
     (+) HAL_UARTEx_DisableFifoMode() API disables the FIFO mode
     (+) HAL_UARTEx_SetTxFifoThreshold() API sets the TX FIFO threshold
     (+) HAL_UARTEx_SetRxFifoThreshold() API sets the RX FIFO threshold

    [..] This subsection also provides a set of additional functions providing enhanced reception
    services to user. (For example, these functions allow application to handle use cases
    where number of data to be received is unknown).

    (#) Compared to standard reception services which only consider number of received
        data elements as reception completion criteria, these functions also consider additional events
        as triggers for updating reception status to caller :
       (++) Detection of inactivity period (RX line has not been active for a given period).
          (+++) RX inactivity detected by IDLE event, i.e. RX line has been in idle state (normally high state)
               for 1 frame time, after last received byte.
          (+++) RX inactivity detected by RTO, i.e. line has been in idle state
               for a programmable time, after last received byte.
       (++) Detection that a specific character has been received.

    (#) There are two modes of transfer:
       (++) Blocking mode: The reception is performed in polling mode, until either expected number of data is received,
           or till IDLE event occurs. Reception is handled only during function execution.
           When function exits, no data reception could occur. HAL status and number of actually received data elements,
           are returned by function after finishing transfer.
       (++) Non-Blocking mode: The reception is performed using Interrupts or DMA.
           These API's return the HAL status.
           The end of the data processing will be indicated through the
           dedicated UART IRQ when using Interrupt mode or the DMA IRQ when using DMA mode.
           The HAL_UARTEx_RxEventCallback() user callback will be executed during Receive process
           The HAL_UART_ErrorCallback()user callback will be executed when a reception error is detected.

    (#) Blocking mode API:
        (++) HAL_UARTEx_ReceiveToIdle()

    (#) Non-Blocking mode API with Interrupt:
        (++) HAL_UARTEx_ReceiveToIdle_IT()

    (#) Non-Blocking mode API with DMA:
        (++) HAL_UARTEx_ReceiveToIdle_DMA()

@endverbatim
  * @{
  */

/**
  * @brief By default in multiprocessor mode, when the wake up method is set
  *        to address mark, the UART handles only 4-bit long addresses detection;
  *        this API allows to enable longer addresses detection (6-, 7- or 8-bit
  *        long).
  * @note  Addresses detection lengths are: 6-bit address detection in 7-bit data mode,
  *        7-bit address detection in 8-bit data mode, 8-bit address detection in 9-bit data mode.
  * @param huart         UART handle.
  * @param AddressLength This parameter can be one of the following values:
  *          @arg @ref UART_ADDRESS_DETECT_4B 4-bit long address
  *          @arg @ref UART_ADDRESS_DETECT_7B 6-, 7- or 8-bit long address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MultiProcessorEx_AddressLength_Set(UART_HandleTypeDef *huart, uint32_t AddressLength)
{
  /* Check the UART handle allocation */
  if (huart == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the address length parameter */
  assert_param(IS_UART_ADDRESSLENGTH_DETECT(AddressLength));

  huart->gState = HAL_UART_STATE_BUSY;

  /* Disable the Peripheral */
  __HAL_UART_DISABLE(huart);

  /* Set the address length */
  MODIFY_REG(huart->Instance->CR2, USART_CR2_ADDM7, AddressLength);

  /* Enable the Peripheral */
  __HAL_UART_ENABLE(huart);

  /* TEACK and/or REACK to check before moving huart->gState to Ready */
  return (UART_CheckIdleState(huart));
}

/**
  * @brief Set Wakeup from Stop mode interrupt flag selection.
  * @note It is the application responsibility to enable the interrupt used as
  *       usart_wkup interrupt source before entering low-power mode.
  * @param huart           UART handle.
  * @param WakeUpSelection Address match, Start Bit detection or RXNE/RXFNE bit status.
  *          This parameter can be one of the following values:
  *          @arg @ref UART_WAKEUP_ON_ADDRESS
  *          @arg @ref UART_WAKEUP_ON_STARTBIT
  *          @arg @ref UART_WAKEUP_ON_READDATA_NONEMPTY
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UARTEx_StopModeWakeUpSourceConfig(UART_HandleTypeDef *huart, UART_WakeUpTypeDef WakeUpSelection)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t tickstart;

  /* check the wake-up from stop mode UART instance */
  assert_param(IS_UART_WAKEUP_FROMSTOP_INSTANCE(huart->Instance));
  /* check the wake-up selection parameter */
  assert_param(IS_UART_WAKEUP_SELECTION(WakeUpSelection.WakeUpEvent));

  /* Process Locked */
  __HAL_LOCK(huart);

  huart->gState = HAL_UART_STATE_BUSY;

  /* Disable the Peripheral */
  __HAL_UART_DISABLE(huart);

  /* Set the wake-up selection scheme */
  MODIFY_REG(huart->Instance->CR3, USART_CR3_WUS, WakeUpSelection.WakeUpEvent);

  if (WakeUpSelection.WakeUpEvent == UART_WAKEUP_ON_ADDRESS)
  {
    UARTEx_Wakeup_AddressConfig(huart, WakeUpSelection);
  }

  /* Enable the Peripheral */
  __HAL_UART_ENABLE(huart);

  /* Init tickstart for timeout management */
  tickstart = HAL_GetTick();

  /* Wait until REACK flag is set */
  if (UART_WaitOnFlagUntilTimeout(huart, USART_ISR_REACK, RESET, tickstart, HAL_UART_TIMEOUT_VALUE) != HAL_OK)
  {
    status = HAL_TIMEOUT;
  }
  else
  {
    /* Initialize the UART State */
    huart->gState = HAL_UART_STATE_READY;
  }

  /* Process Unlocked */
  __HAL_UNLOCK(huart);

  return status;
}

/**
  * @brief Enable UART Stop Mode.
  * @note The UART is able to wake up the MCU from Stop 1 mode as long as UART clock is HSI or LSE.
  * @param huart UART handle.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UARTEx_EnableStopMode(UART_HandleTypeDef *huart)
{
  /* Process Locked */
  __HAL_LOCK(huart);

  /* Set UESM bit */
  ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_UESM);

  /* Process Unlocked */
  __HAL_UNLOCK(huart);

  return HAL_OK;
}

/**
  * @brief Disable UART Stop Mode.
  * @param huart UART handle.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UARTEx_DisableStopMode(UART_HandleTypeDef *huart)
{
  /* Process Locked */
  __HAL_LOCK(huart);

  /* Clear UESM bit */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_UESM);

  /* Process Unlocked */
  __HAL_UNLOCK(huart);

  return HAL_OK;
}

/**
  * @brief  Enable the FIFO mode.
  * @param huart      UART handle.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UARTEx_EnableFifoMode(UART_HandleTypeDef *huart)
{
  uint32_t tmpcr1;

  /* Check parameters */
  assert_param(IS_UART_FIFO_INSTANCE(huart->Instance));

  /* Process Locked */
  __HAL_LOCK(huart);

  huart->gState = HAL_UART_STATE_BUSY;

  /* Save actual UART configuration */
  tmpcr1 = READ_REG(huart->Instance->CR1);

  /* Disable UART */
  __HAL_UART_DISABLE(huart);

  /* Enable FIFO mode */
  SET_BIT(tmpcr1, USART_CR1_FIFOEN);
  huart->FifoMode = UART_FIFOMODE_ENABLE;

  /* Restore UART configuration */
  WRITE_REG(huart->Instance->CR1, tmpcr1);

  /* Determine the number of data to process during RX/TX ISR execution */
  UARTEx_SetNbDataToProcess(huart);

  huart->gState = HAL_UART_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(huart);

  return HAL_OK;
}

/**
  * @brief  Disable the FIFO mode.
  * @param huart      UART handle.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UARTEx_DisableFifoMode(UART_HandleTypeDef *huart)
{
  uint32_t tmpcr1;

  /* Check parameters */
  assert_param(IS_UART_FIFO_INSTANCE(huart->Instance));

  /* Process Locked */
  __HAL_LOCK(huart);

  huart->gState = HAL_UART_STATE_BUSY;

  /* Save actual UART configuration */
  tmpcr1 = READ_REG(huart->Instance->CR1);

  /* Disable UART */
  __HAL_UART_DISABLE(huart);

  /* Disable FIFO mode */
  CLEAR_BIT(tmpcr1, USART_CR1_FIFOEN);
  huart->FifoMode = UART_FIFOMODE_DISABLE;

  /* Restore UART configuration */
  WRITE_REG(huart->Instance->CR1, tmpcr1);

  huart->gState = HAL_UART_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(huart);

  return HAL_OK;
}

/**
  * @brief  Set the TXFIFO threshold.
  * @param huart      UART handle.
  * @param Threshold  TX FIFO threshold value
  *          This parameter can be one of the following values:
  *            @arg @ref UART_TXFIFO_THRESHOLD_1_8
  *            @arg @ref UART_TXFIFO_THRESHOLD_1_4
  *            @arg @ref UART_TXFIFO_THRESHOLD_1_2
  *            @arg @ref UART_TXFIFO_THRESHOLD_3_4
  *            @arg @ref UART_TXFIFO_THRESHOLD_7_8
  *            @arg @ref UART_TXFIFO_THRESHOLD_8_8
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UARTEx_SetTxFifoThreshold(UART_HandleTypeDef *huart, uint32_t Threshold)
{
  uint32_t tmpcr1;

  /* Check parameters */
  assert_param(IS_UART_FIFO_INSTANCE(huart->Instance));
  assert_param(IS_UART_TXFIFO_THRESHOLD(Threshold));

  /* Process Locked */
  __HAL_LOCK(huart);

  huart->gState = HAL_UART_STATE_BUSY;

  /* Save actual UART configuration */
  tmpcr1 = READ_REG(huart->Instance->CR1);

  /* Disable UART */
  __HAL_UART_DISABLE(huart);

  /* Update TX threshold configuration */
  MODIFY_REG(huart->Instance->CR3, USART_CR3_TXFTCFG, Threshold);

  /* Determine the number of data to process during RX/TX ISR execution */
  UARTEx_SetNbDataToProcess(huart);

  /* Restore UART configuration */
  WRITE_REG(huart->Instance->CR1, tmpcr1);

  huart->gState = HAL_UART_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(huart);

  return HAL_OK;
}

/**
  * @brief  Set the RXFIFO threshold.
  * @param huart      UART handle.
  * @param Threshold  RX FIFO threshold value
  *          This parameter can be one of the following values:
  *            @arg @ref UART_RXFIFO_THRESHOLD_1_8
  *            @arg @ref UART_RXFIFO_THRESHOLD_1_4
  *            @arg @ref UART_RXFIFO_THRESHOLD_1_2
  *            @arg @ref UART_RXFIFO_THRESHOLD_3_4
  *            @arg @ref UART_RXFIFO_THRESHOLD_7_8
  *            @arg @ref UART_RXFIFO_THRESHOLD_8_8
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UARTEx_SetRxFifoThreshold(UART_HandleTypeDef *huart, uint32_t Threshold)
{
  uint32_t tmpcr1;

  /* Check the parameters */
  assert_param(IS_UART_FIFO_INSTANCE(huart->Instance));
  assert_param(IS_UART_RXFIFO_THRESHOLD(Threshold));

  /* Process Locked */
  __HAL_LOCK(huart);

  huart->gState = HAL_UART_STATE_BUSY;

  /* Save actual UART configuration */
  tmpcr1 = READ_REG(huart->Instance->CR1);

  /* Disable UART */
  __HAL_UART_DISABLE(huart);

  /* Update RX threshold configuration */
  MODIFY_REG(huart->Instance->CR3, USART_CR3_RXFTCFG, Threshold);

  /* Determine the number of data to process during RX/TX ISR execution */
  UARTEx_SetNbDataToProcess(huart);

  /* Restore UART configuration */
  WRITE_REG(huart->Instance->CR1, tmpcr1);

  huart->gState = HAL_UART_STATE_READY;

  /* Process Unlocked */
  __HAL_UNLOCK(huart);

  return HAL_OK;
}

/**
  * @brief Receive an amount of data in blocking mode till either the expected number of data
  *        is received or an IDLE event occurs.
  * @note  HAL_OK is returned if reception is completed (expected number of data has been received)
  *        or if reception is stopped after IDLE event (less than the expected number of data has been received)
  *        In this case, RxLen output parameter indicates number of data available in reception buffer.
  * @note  When UART parity is not enabled (PCE = 0), and Word Length is configured to 9 bits (M1-M0 = 01),
  *        the received data is handled as a set of uint16_t. In this case, Size must indicate the number
  *        of uint16_t available through pData.
  * @note When FIFO mode is enabled, the RXFNE flag is set as long as the RXFIFO
  *       is not empty. Read operations from the RDR register are performed when
  *       RXFNE flag is set. From hardware perspective, RXFNE flag and
  *       RXNE are mapped on the same bit-field.
  * @param huart   UART handle.
  * @param pData   Pointer to data buffer (uint8_t or uint16_t data elements).
  * @param Size    Amount of data elements (uint8_t or uint16_t) to be received.
  * @param RxLen   Number of data elements finally received
  *                (could be lower than Size, in case reception ends on IDLE event)
  * @param Timeout Timeout duration expressed in ms (covers the whole reception sequence).
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint16_t *RxLen,
                                           uint32_t Timeout)
{
  uint8_t  *pdata8bits;
  uint16_t *pdata16bits;
  uint16_t uhMask;
  uint32_t tickstart;

  /* Check that a Rx process is not already ongoing */
  if (huart->RxState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return  HAL_ERROR;
    }

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->RxState = HAL_UART_STATE_BUSY_RX;
    huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE;
    huart->RxEventType = HAL_UART_RXEVENT_TC;

    /* Init tickstart for timeout management */
    tickstart = HAL_GetTick();

    huart->RxXferSize  = Size;
    huart->RxXferCount = Size;

    /* Computation of UART mask to apply to RDR register */
    UART_MASK_COMPUTATION(huart);
    uhMask = huart->Mask;

    /* In case of 9bits/No Parity transfer, pRxData needs to be handled as a uint16_t pointer */
    if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
    {
      pdata8bits  = NULL;
      pdata16bits = (uint16_t *) pData;
    }
    else
    {
      pdata8bits  = pData;
      pdata16bits = NULL;
    }

    /* Initialize output number of received elements */
    *RxLen = 0U;

    /* as long as data have to be received */
    while (huart->RxXferCount > 0U)
    {
      /* Check if IDLE flag is set */
      if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE))
      {
        /* Clear IDLE flag in ISR */
        __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_IDLEF);

        /* If Set, but no data ever received, clear flag without exiting loop */
        /* If Set, and data has already been received, this means Idle Event is valid : End reception */
        if (*RxLen > 0U)
        {
          huart->RxEventType = HAL_UART_RXEVENT_IDLE;
          huart->RxState = HAL_UART_STATE_READY;

          return HAL_OK;
        }
      }

      /* Check if RXNE flag is set */
      if (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE))
      {
        if (pdata8bits == NULL)
        {
          *pdata16bits = (uint16_t)(huart->Instance->RDR & uhMask);
          pdata16bits++;
        }
        else
        {
          *pdata8bits = (uint8_t)(huart->Instance->RDR & (uint8_t)uhMask);
          pdata8bits++;
        }
        /* Increment number of received elements */
        *RxLen += 1U;
        huart->RxXferCount--;
      }

      /* Check for the Timeout */
      if (Timeout != HAL_MAX_DELAY)
      {
        if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
        {
          huart->RxState = HAL_UART_STATE_READY;

          return HAL_TIMEOUT;
        }
      }
    }

    /* Set number of received elements in output parameter : RxLen */
    *RxLen = huart->RxXferSize - huart->RxXferCount;
    /* At end of Rx process, restore huart->RxState to Ready */
    huart->RxState = HAL_UART_STATE_READY;

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief Receive an amount of data in interrupt mode till either the expected number of data
  *        is received or an IDLE event occurs.
  * @note  Reception is initiated by this function call. Further progress of reception is achieved thanks
  *        to UART interrupts raised by RXNE and IDLE events. Callback is called at end of reception indicating
  *        number of received data elements.
  * @note  When UART parity is not enabled (PCE = 0), and Word Length is configured to 9 bits (M1-M0 = 01),
  *        the received data is handled as a set of uint16_t. In this case, Size must indicate the number
  *        of uint16_t available through pData.
  * @param huart UART handle.
  * @param pData Pointer to data buffer (uint8_t or uint16_t data elements).
  * @param Size  Amount of data elements (uint8_t or uint16_t) to be received.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check that a Rx process is not already ongoing */
  if (huart->RxState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

    /* Set Reception type to reception till IDLE Event*/
    huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE;
    huart->RxEventType = HAL_UART_RXEVENT_TC;

    (void)UART_Start_Receive_IT(huart, pData, Size);

    if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
    {
      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_IDLEF);
      ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);
    }
    else
    {
      /* In case of errors already pending when reception is started,
         Interrupts may have already been raised and lead to reception abortion.
         (Overrun error for instance).
         In such case Reception Type has been reset to HAL_UART_RECEPTION_STANDARD. */
      status = HAL_ERROR;
    }

    return status;
  }
  else
  {
    return HAL_BUSY;
  }
}

#if defined(HAL_DMA_MODULE_ENABLED)
/**
  * @brief Receive an amount of data in DMA mode till either the expected number
  *        of data is received or an IDLE event occurs.
  * @note  Reception is initiated by this function call. Further progress of reception is achieved thanks
  *        to DMA services, transferring automatically received data elements in user reception buffer and
  *        calling registered callbacks at half/end of reception. UART IDLE events are also used to consider
  *        reception phase as ended. In all cases, callback execution will indicate number of received data elements.
  * @note  When the UART parity is enabled (PCE = 1), the received data contain
  *        the parity bit (MSB position).
  * @note  When UART parity is not enabled (PCE = 0), and Word Length is configured to 9 bits (M1-M0 = 01),
  *        the received data is handled as a set of uint16_t. In this case, Size must indicate the number
  *        of uint16_t available through pData.
  * @param huart UART handle.
  * @param pData Pointer to data buffer (uint8_t or uint16_t data elements).
  * @param Size  Amount of data elements (uint8_t or uint16_t) to be received.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef status;

  /* Check that a Rx process is not already ongoing */
  if (huart->RxState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

    /* Set Reception type to reception till IDLE Event*/
    huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE;
    huart->RxEventType = HAL_UART_RXEVENT_TC;

    status =  UART_Start_Receive_DMA(huart, pData, Size);

    /* Check Rx process has been successfully started */
    if (status == HAL_OK)
    {
      if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
      {
        __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_IDLEF);
        ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);
      }
      else
      {
        /* In case of errors already pending when reception is started,
           Interrupts may have already been raised and lead to reception abortion.
           (Overrun error for instance).
           In such case Reception Type has been reset to HAL_UART_RECEPTION_STANDARD. */
        status = HAL_ERROR;
      }
    }

    return status;
  }
  else
  {
    return HAL_BUSY;
  }
}
#endif /* HAL_DMA_MODULE_ENABLED */

/**
  * @brief Provide Rx Event type that has lead to RxEvent callback execution.
  * @note  When HAL_UARTEx_ReceiveToIdle_IT() or HAL_UARTEx_ReceiveToIdle_DMA() API are called, progress
  *        of reception process is provided to application through calls of Rx Event callback (either default one
  *        HAL_UARTEx_RxEventCallback() or user registered one). As several types of events could occur (IDLE event,
  *        Half Transfer, or Transfer Complete), this function allows to retrieve the Rx Event type that has lead
  *        to Rx Event callback execution.
  * @note  This function is expected to be called within the user implementation of Rx Event Callback,
  *        in order to provide the accurate value.
  * @note  In Interrupt Mode:
  *        - HAL_UART_RXEVENT_TC : when Reception has been completed (expected nb of data has been received).
  *        - HAL_UART_RXEVENT_IDLE : when Idle event occurred prior reception has been completed.
  * @note  In DMA Mode:
  *        - HAL_UART_RXEVENT_TC : when Reception has been completed (expected nb of data has been received).
  *        - HAL_UART_RXEVENT_HT : when half of expected nb of data has been received.
  *        - HAL_UART_RXEVENT_IDLE : when Idle event occurred prior reception has been completed.
  * @note  In DMA mode, RxEvent callback could be called several times;
  *        When DMA is configured in Normal Mode, HT event does not stop Reception process;
  *        When DMA is configured in Circular Mode, HT, TC or IDLE events don't stop Reception process;
  * @param  huart UART handle.
  * @retval Rx Event Type (return vale will be a value of @ref UART_RxEvent_Type_Values)
  */
HAL_UART_RxEventTypeTypeDef HAL_UARTEx_GetRxEventType(const UART_HandleTypeDef *huart)
{
  /* Return Rx Event type value, as stored in UART handle */
  return (huart->RxEventType);
}

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup UARTEx_Private_Functions
  * @{
  */

/**
  * @brief Initialize the UART wake-up from stop mode parameters when triggered by address detection.
  * @param huart           UART handle.
  * @param WakeUpSelection UART wake up from stop mode parameters.
  * @retval None
  */
static void UARTEx_Wakeup_AddressConfig(UART_HandleTypeDef *huart, UART_WakeUpTypeDef WakeUpSelection)
{
  assert_param(IS_UART_ADDRESSLENGTH_DETECT(WakeUpSelection.AddressLength));

  /* Set the USART address length */
  MODIFY_REG(huart->Instance->CR2, USART_CR2_ADDM7, WakeUpSelection.AddressLength);

  /* Set the USART address node */
  MODIFY_REG(huart->Instance->CR2, USART_CR2_ADD, ((uint32_t)WakeUpSelection.Address << UART_CR2_ADDRESS_LSB_POS));
}

/**
  * @brief Calculate the number of data to process in RX/TX ISR.
  * @note The RX FIFO depth and the TX FIFO depth is extracted from
  *       the UART configuration registers.
  * @param huart UART handle.
  * @retval None
  */
static void UARTEx_SetNbDataToProcess(UART_HandleTypeDef *huart)
{
  uint8_t rx_fifo_depth;
  uint8_t tx_fifo_depth;
  uint8_t rx_fifo_threshold;
  uint8_t tx_fifo_threshold;
  static const uint8_t numerator[] = {1U, 1U, 1U, 3U, 7U, 1U, 0U, 0U};
  static const uint8_t denominator[] = {8U, 4U, 2U, 4U, 8U, 1U, 1U, 1U};

  if (huart->FifoMode == UART_FIFOMODE_DISABLE)
  {
    huart->NbTxDataToProcess = 1U;
    huart->NbRxDataToProcess = 1U;
  }
  else
  {
    rx_fifo_depth = RX_FIFO_DEPTH;
    tx_fifo_depth = TX_FIFO_DEPTH;
    rx_fifo_threshold = (uint8_t)(READ_BIT(huart->Instance->CR3, USART_CR3_RXFTCFG) >> USART_CR3_RXFTCFG_Pos);
    tx_fifo_threshold = (uint8_t)(READ_BIT(huart->Instance->CR3, USART_CR3_TXFTCFG) >> USART_CR3_TXFTCFG_Pos);
    huart->NbTxDataToProcess = ((uint16_t)tx_fifo_depth * numerator[tx_fifo_threshold]) /
                               (uint16_t)denominator[tx_fifo_threshold];
    huart->NbRxDataToProcess = ((uint16_t)rx_fifo_depth * numerator[rx_fifo_threshold]) /
                               (uint16_t)denominator[rx_fifo_threshold];
  }
}
/**
  * @}
  */

#endif /* HAL_UART_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */

