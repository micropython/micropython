/**
  ******************************************************************************
  * @file    stm32n6xx_hal_xspi.c
  * @author  MCD Application Team
  * @brief   XSPI HAL module driver.
             This file provides firmware functions to manage the following
             functionalities of the XSPI interface (XSPI).
              + Initialization and de-initialization functions
              + Hyperbus configuration
              + Indirect functional mode management
              + Memory-mapped functional mode management
              + Auto-polling functional mode management
              + Interrupts and flags management
              + DMA channel configuration for indirect functional mode
              + Errors management and abort functionality
              + IO manager configuration
              + HIGH-SPEED INTERFACE configuration
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
 ===============================================================================
                        ##### How to use this driver #####
 ===============================================================================
  [..]
    *** Initialization ***
    ======================
    [..]
     As prerequisite, fill in the HAL_XSPI_MspInit() :
     (+) Enable XSPI clocks interface with __HAL_RCC_XSPI_CLK_ENABLE().
     (+) Reset XSPI Peripheral with __HAL_RCC_XSPI_FORCE_RESET() and __HAL_RCC_XSPI_RELEASE_RESET().
     (+) Enable the clocks for the XSPI GPIOS with __HAL_RCC_GPIOx_CLK_ENABLE().
     (+) Configure these XSPI pins in alternate mode using HAL_GPIO_Init().
     (+) If interrupt or DMA mode is used, enable and configure XSPI global
         interrupt with HAL_NVIC_SetPriority() and HAL_NVIC_EnableIRQ().
     (+) If DMA mode is used, enable the clocks for the XSPI DMA channel
         with __HAL_RCC_DMAx_CLK_ENABLE(), configure DMA with HAL_DMA_Init(),
         link it with XSPI handle using __HAL_LINKDMA(), enable and configure
         DMA channel global interrupt with HAL_NVIC_SetPriority() and HAL_NVIC_EnableIRQ().
    [..]
     Configure the fifo threshold, the memory mode, the memory type, the
     device size, the CS high time, the free running clock, the clock mode,
     the wrap size, the clock prescaler, the sample shifting, the hold delay
     and the CS boundary using the HAL_XSPI_Init() function.
    [..]
     When using Hyperbus, configure the RW recovery time, the access time,
     the write latency and the latency mode using the HAL_XSPI_HyperbusCfg()
     function.

    *** Indirect functional mode ***
    ================================
    [..]
     In regular mode, configure the command sequence using the HAL_XSPI_Command()
     or HAL_XSPI_Command_IT() functions :
     (+) Instruction phase : the mode used and if present the size, the instruction
         opcode and the DTR mode.
     (+) Address phase : the mode used and if present the size, the address
         value and the DTR mode.
     (+) Alternate-bytes phase : the mode used and if present the size, the
         alternate bytes values and the DTR mode.
     (+) Dummy-cycles phase : the number of dummy cycles (mode used is same as data phase).
     (+) Data phase : the mode used and if present the number of bytes and the DTR mode.
     (+) Data strobe (DQS) mode : the activation (or not) of this mode
     (+) IO selection : to access external memory.
     (+) Operation type : always common configuration.
    [..]
     In Hyperbus mode, configure the command sequence using the HAL_XSPI_HyperbusCmd()
     function :
     (+) Address space : indicate if the access will be done in register or memory
     (+) Address size
     (+) Number of data
     (+) Data strobe (DQS) mode : the activation (or not) of this mode
    [..]
     If no data is required for the command (only for regular mode, not for
     Hyperbus mode), it is sent directly to the memory :
     (+) In polling mode, the output of the function is done when the transfer is complete.
     (+) In interrupt mode, HAL_XSPI_CmdCpltCallback() will be called when the transfer is complete.
    [..]
     For the indirect write mode, use HAL_XSPI_Transmit(), HAL_XSPI_Transmit_DMA() or
     HAL_XSPI_Transmit_IT() after the command configuration :
     (+) In polling mode, the output of the function is done when the transfer is complete.
     (+) In interrupt mode, HAL_XSPI_FifoThresholdCallback() will be called when the fifo threshold
         is reached and HAL_XSPI_TxCpltCallback() will be called when the transfer is complete.
     (+) In DMA mode, HAL_XSPI_TxHalfCpltCallback() will be called at the half transfer and
         HAL_XSPI_TxCpltCallback() will be called when the transfer is complete.
    [..]
     For the indirect read mode, use HAL_XSPI_Receive(), HAL_XSPI_Receive_DMA() or
     HAL_XSPI_Receive_IT() after the command configuration :
     (+) In polling mode, the output of the function is done when the transfer is complete.
     (+) In interrupt mode, HAL_XSPI_FifoThresholdCallback() will be called when the fifo threshold
         is reached and HAL_XSPI_RxCpltCallback() will be called when the transfer is complete.
     (+) In DMA mode, HAL_XSPI_RxHalfCpltCallback() will be called at the half transfer and
         HAL_XSPI_RxCpltCallback() will be called when the transfer is complete.

    *** Auto-polling functional mode ***
    ====================================
    [..]
     Configure the command sequence by the same way than the indirect mode
    [..]
     Configure the auto-polling functional mode using the HAL_XSPI_AutoPolling()
     or HAL_XSPI_AutoPolling_IT() functions :
     (+) The size of the status bytes, the match value, the mask used, the match mode (OR/AND),
         the polling interval and the automatic stop activation.
    [..]
     After the configuration :
     (+) In polling mode, the output of the function is done when the status match is reached. The
         automatic stop is activated to avoid an infinite loop.
     (+) In interrupt mode, HAL_XSPI_StatusMatchCallback() will be called each time the status match is reached.

    *** Memory-mapped functional mode ***
    =====================================
    [..]
     Configure the command sequence by the same way than the indirect mode except
     for the operation type in regular mode :
     (+) Operation type equals to read configuration : the command configuration
         applies to read access in memory-mapped mode
     (+) Operation type equals to write configuration : the command configuration
         applies to write access in memory-mapped mode
     (+) Both read and write configuration should be performed before activating
         memory-mapped mode
    [..]
     Configure the memory-mapped functional mode using the HAL_XSPI_MemoryMapped()
     functions :
     (+) The timeout activation and the timeout period.
    [..]
     After the configuration, the XSPI will be used as soon as an access on the AHB is done on
     the address range. HAL_XSPI_TimeOutCallback() will be called when the timeout expires.

    *** Errors management and abort functionality ***
    =================================================
    [..]
     HAL_XSPI_GetError() function gives the error raised during the last operation.
    [..]
     HAL_XSPI_Abort() and HAL_XSPI_AbortIT() functions aborts any on-going operation and
     flushes the fifo :
     (+) In polling mode, the output of the function is done when the transfer
         complete bit is set and the busy bit cleared.
     (+) In interrupt mode, HAL_XSPI_AbortCpltCallback() will be called when
         the transfer complete bit is set.

    *** Control functions ***
    =========================
    [..]
     HAL_XSPI_GetState() function gives the current state of the HAL XSPI driver.
    [..]
     HAL_XSPI_SetTimeout() function configures the timeout value used in the driver.
    [..]
     HAL_XSPI_SetFifoThreshold() function configures the threshold on the Fifo of the XSPI Peripheral.
    [..]
     HAL_XSPI_SetMemoryType() function configures the type of the external memory.
    [..]
     HAL_XSPI_SetDeviceSize() function configures the size of the external memory.
    [..]
     HAL_XSPI_SetClockPrescaler() function configures the clock prescaler of the XSPI Peripheral.
    [..]
     HAL_XSPI_GetFifoThreshold() function gives the current of the Fifo's threshold

    *** IO manager configuration functions ***
    ==========================================
    [..]
     HAL_XSPIM_Config() function configures the IO manager for the XSPI instance.

    *** High-speed interface and calibration functions ***
    ==========================================
    [..]
     The  purpose of High-speed interface is primary to shift data or data strobe by one quarter of octal
     bus clock period, with a correct timing accuracy. DLL must be calibrated versus this clock period.
     The calibration process is automatically enabled when one of the three conditions below is met:
     (+) The XSPI exits Reset state.
     (+) The Prescaler is set.
     (+) The configuration of communication is set.
    [..]
     HAL_XSPI_GetDelayValue() function Get the delay values of the high-speed interface DLLs..
    [..]
     HAL_XSPI_SetDelayValue() function Set the delay values of the high-speed interface DLLs..

    *** Callback registration ***
    =============================================
    [..]
     The compilation define  USE_HAL_XSPI_REGISTER_CALLBACKS when set to 1
     allows the user to configure dynamically the driver callbacks.

    [..]
     Use function HAL_XSPI_RegisterCallback() to register a user callback,
     it allows to register following callbacks:
     (+) ErrorCallback : callback when error occurs.
     (+) AbortCpltCallback : callback when abort is completed.
     (+) FifoThresholdCallback : callback when the fifo threshold is reached.
     (+) CmdCpltCallback : callback when a command without data is completed.
     (+) RxCpltCallback : callback when a reception transfer is completed.
     (+) TxCpltCallback : callback when a transmission transfer is completed.
     (+) RxHalfCpltCallback : callback when half of the reception transfer is completed.
     (+) TxHalfCpltCallback : callback when half of the transmission transfer is completed.
     (+) StatusMatchCallback : callback when a status match occurs.
     (+) TimeOutCallback : callback when the timeout perioed expires.
     (+) MspInitCallback    : XSPI MspInit.
     (+) MspDeInitCallback  : XSPI MspDeInit.
    [..]
     This function takes as parameters the HAL peripheral handle, the Callback ID
     and a pointer to the user callback function.

    [..]
     Use function HAL_XSPI_UnRegisterCallback() to reset a callback to the default
     weak (overridden) function. It allows to reset following callbacks:
     (+) ErrorCallback : callback when error occurs.
     (+) AbortCpltCallback : callback when abort is completed.
     (+) FifoThresholdCallback : callback when the fifo threshold is reached.
     (+) CmdCpltCallback : callback when a command without data is completed.
     (+) RxCpltCallback : callback when a reception transfer is completed.
     (+) TxCpltCallback : callback when a transmission transfer is completed.
     (+) RxHalfCpltCallback : callback when half of the reception transfer is completed.
     (+) TxHalfCpltCallback : callback when half of the transmission transfer is completed.
     (+) StatusMatchCallback : callback when a status match occurs.
     (+) TimeOutCallback : callback when the timeout perioed expires.
     (+) MspInitCallback    : XSPI MspInit.
     (+) MspDeInitCallback  : XSPI MspDeInit.
    [..]
     This function) takes as parameters the HAL peripheral handle and the Callback ID.

    [..]
     By default, after the HAL_XSPI_Init() and if the state is HAL_XSPI_STATE_RESET
     all callbacks are reset to the corresponding legacy weak (overridden) functions.
     Exception done for MspInit and MspDeInit callbacks that are respectively
     reset to the legacy weak (overridden) functions in the HAL_XSPI_Init()
     and HAL_XSPI_DeInit() only when these callbacks are null (not registered beforehand).
     If not, MspInit or MspDeInit are not null, the HAL_XSPI_Init() and HAL_XSPI_DeInit()
     keep and use the user MspInit/MspDeInit callbacks (registered beforehand)

    [..]
     Callbacks can be registered/unregistered in READY state only.
     Exception done for MspInit/MspDeInit callbacks that can be registered/unregistered
     in READY or RESET state, thus registered (user) MspInit/DeInit callbacks can be used
     during the Init/DeInit.
     In that case first register the MspInit/MspDeInit user callbacks
     using HAL_XSPI_RegisterCallback() before calling HAL_XSPI_DeInit()
     or HAL_XSPI_Init() function.

    [..]
     When The compilation define USE_HAL_XSPI_REGISTER_CALLBACKS is set to 0 or
     not defined, the callback registering feature is not available
     and weak (overridden) callbacks are used.

  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"
#include "stm32n6xx_hal_rcc.h"

#if defined(XSPI) || defined(XSPI1) || defined(XSPI2) || defined(XSPI3)

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup XSPI XSPI
  * @brief XSPI HAL module driver
  * @{
  */

#ifdef HAL_XSPI_MODULE_ENABLED

/**
  @cond 0
  */
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define XSPI_FUNCTIONAL_MODE_INDIRECT_WRITE ((uint32_t)0x00000000)         /*!< Indirect write mode    */
#define XSPI_FUNCTIONAL_MODE_INDIRECT_READ  ((uint32_t)XSPI_CR_FMODE_0)    /*!< Indirect read mode     */
#define XSPI_FUNCTIONAL_MODE_AUTO_POLLING   ((uint32_t)XSPI_CR_FMODE_1)    /*!< Automatic polling mode */
#define XSPI_FUNCTIONAL_MODE_MEMORY_MAPPED  ((uint32_t)XSPI_CR_FMODE)      /*!< Memory-mapped mode     */

#define XSPI_CFG_STATE_MASK  0x00000004U
#define XSPI_BUSY_STATE_MASK 0x00000008U

#define XSPI_NB_INSTANCE   3U
#define XSPI_IOM_NB_PORTS  2U
#define XSPI_IOM_PORT_MASK 0x1U

/* Private macro -------------------------------------------------------------*/
#define IS_XSPI_FUNCTIONAL_MODE(MODE) (((MODE) == XSPI_FUNCTIONAL_MODE_INDIRECT_WRITE) || \
                                       ((MODE) == XSPI_FUNCTIONAL_MODE_INDIRECT_READ)  || \
                                       ((MODE) == XSPI_FUNCTIONAL_MODE_AUTO_POLLING)   || \
                                       ((MODE) == XSPI_FUNCTIONAL_MODE_MEMORY_MAPPED))

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void              XSPI_DMACplt(DMA_HandleTypeDef *hdma);
static void              XSPI_DMAHalfCplt(DMA_HandleTypeDef *hdma);
static void              XSPI_DMAError(DMA_HandleTypeDef *hdma);
static void              XSPI_DMAAbortCplt(DMA_HandleTypeDef *hdma);
static HAL_StatusTypeDef XSPI_WaitFlagStateUntilTimeout(XSPI_HandleTypeDef *hxspi, uint32_t Flag, FlagStatus State,
                                                        uint32_t Tickstart, uint32_t Timeout);
static HAL_StatusTypeDef XSPI_ConfigCmd(XSPI_HandleTypeDef *hxspi, const XSPI_RegularCmdTypeDef *pCmd);
static void XSPIM_GetConfig(uint8_t instance_nb, XSPIM_CfgTypeDef *pCfg);
/**
  @endcond
  */

/* Exported functions --------------------------------------------------------*/

/** @defgroup XSPI_Exported_Functions XSPI Exported Functions
  * @{
  */

/** @defgroup XSPI_Exported_Functions_Group1 Initialization/de-initialization functions
  * @brief    Initialization and Configuration functions
  *
@verbatim
===============================================================================
            ##### Initialization and Configuration functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to :
      (+) Initialize the XSPI.
      (+) De-initialize the XSPI.

@endverbatim
  * @{
  */

/**
  * @brief  Initialize the XSPI mode according to the specified parameters
  *         in the XSPI_InitTypeDef and initialize the associated handle.
  * @param  hxspi : XSPI handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_Init(XSPI_HandleTypeDef *hxspi)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t tickstart = HAL_GetTick();

  /* Check the XSPI handle allocation */
  if (hxspi == NULL)
  {
    status = HAL_ERROR;
    /* No error code can be set set as the handler is null */
  }
  else
  {
    /* Check the parameters of the initialization structure */
    assert_param(IS_XSPI_MEMORY_MODE(hxspi->Init.MemoryMode));
    assert_param(IS_XSPI_MEMORY_TYPE(hxspi->Init.MemoryType));
    assert_param(IS_XSPI_MEMORY_SIZE(hxspi->Init.MemorySize));
    assert_param(IS_XSPI_CS_HIGH_TIME_CYCLE(hxspi->Init.ChipSelectHighTimeCycle));
    assert_param(IS_XSPI_FREE_RUN_CLK(hxspi->Init.FreeRunningClock));
    assert_param(IS_XSPI_CLOCK_MODE(hxspi->Init.ClockMode));
    assert_param(IS_XSPI_WRAP_SIZE(hxspi->Init.WrapSize));
    assert_param(IS_XSPI_CLK_PRESCALER(hxspi->Init.ClockPrescaler));
    assert_param(IS_XSPI_SAMPLE_SHIFTING(hxspi->Init.SampleShifting));
    assert_param(IS_XSPI_DHQC(hxspi->Init.DelayHoldQuarterCycle));
    assert_param(IS_XSPI_CS_BOUND(hxspi->Init.ChipSelectBoundary));
    assert_param(IS_XSPI_FIFO_THRESHOLD_BYTE(hxspi->Init.FifoThresholdByte));
    assert_param(IS_XSPI_MAXTRAN(hxspi->Init.MaxTran));
    assert_param(IS_XSPI_CSSEL(hxspi->Init.MemorySelect));
    assert_param(IS_XSPI_EXTENDMEM(hxspi->Init.MemoryExtended));
    /* Initialize error code */
    hxspi->ErrorCode = HAL_XSPI_ERROR_NONE;

    /* Check if the state is the reset state */
    if (hxspi->State == HAL_XSPI_STATE_RESET)
    {
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
      /* Reset Callback pointers in HAL_XSPI_STATE_RESET only */
      hxspi->ErrorCallback         = HAL_XSPI_ErrorCallback;
      hxspi->AbortCpltCallback     = HAL_XSPI_AbortCpltCallback;
      hxspi->FifoThresholdCallback = HAL_XSPI_FifoThresholdCallback;
      hxspi->CmdCpltCallback       = HAL_XSPI_CmdCpltCallback;
      hxspi->RxCpltCallback        = HAL_XSPI_RxCpltCallback;
      hxspi->TxCpltCallback        = HAL_XSPI_TxCpltCallback;
      hxspi->RxHalfCpltCallback    = HAL_XSPI_RxHalfCpltCallback;
      hxspi->TxHalfCpltCallback    = HAL_XSPI_TxHalfCpltCallback;
      hxspi->StatusMatchCallback   = HAL_XSPI_StatusMatchCallback;
      hxspi->TimeOutCallback       = HAL_XSPI_TimeOutCallback;

      if (hxspi->MspInitCallback == NULL)
      {
        hxspi->MspInitCallback = HAL_XSPI_MspInit;
      }

      /* Init the low level hardware */
      hxspi->MspInitCallback(hxspi);
#else
      /* Initialization of the low level hardware */
      HAL_XSPI_MspInit(hxspi);
#endif /* defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */

      /* Configure the default timeout for the XSPI memory access */
      (void)HAL_XSPI_SetTimeout(hxspi, HAL_XSPI_TIMEOUT_DEFAULT_VALUE);

      /* Configure memory type, device size, chip select high time, free running clock, clock mode */
      MODIFY_REG(hxspi->Instance->DCR1,
                 (XSPI_DCR1_MTYP | XSPI_DCR1_DEVSIZE | XSPI_DCR1_CSHT | XSPI_DCR1_FRCK | XSPI_DCR1_CKMODE),
                 (hxspi->Init.MemoryType | ((hxspi->Init.MemorySize) << XSPI_DCR1_DEVSIZE_Pos) |
                  ((hxspi->Init.ChipSelectHighTimeCycle - 1U) << XSPI_DCR1_CSHT_Pos) | hxspi->Init.ClockMode));

      /* Configure wrap size */
      MODIFY_REG(hxspi->Instance->DCR2, XSPI_DCR2_WRAPSIZE, hxspi->Init.WrapSize);

      /* Configure chip select boundary */
      MODIFY_REG(hxspi->Instance->DCR3, XSPI_DCR3_CSBOUND, (hxspi->Init.ChipSelectBoundary << XSPI_DCR3_CSBOUND_Pos));

      /* Configure maximum transfer */
      MODIFY_REG(hxspi->Instance->DCR3, XSPI_DCR3_MAXTRAN, \
                 (hxspi->Init.MaxTran << XSPI_DCR3_MAXTRAN_Pos));

      /* Configure refresh */
      hxspi->Instance->DCR4 = hxspi->Init.Refresh;

      /* Configure FIFO threshold */
      MODIFY_REG(hxspi->Instance->CR, XSPI_CR_FTHRES, ((hxspi->Init.FifoThresholdByte - 1U) << XSPI_CR_FTHRES_Pos));

      /* Wait till busy flag is reset */
      status = XSPI_WaitFlagStateUntilTimeout(hxspi, HAL_XSPI_FLAG_BUSY, RESET, tickstart, hxspi->Timeout);

      if (status == HAL_OK)
      {
        /* Configure clock prescaler */
        MODIFY_REG(hxspi->Instance->DCR2, XSPI_DCR2_PRESCALER,
                   ((hxspi->Init.ClockPrescaler) << XSPI_DCR2_PRESCALER_Pos));

        if (IS_XSPI_ALL_INSTANCE(hxspi->Instance))
        {
          /* The configuration of clock prescaler trigger automatically a calibration process.
          So it is necessary to wait the calibration is complete */
          status = XSPI_WaitFlagStateUntilTimeout(hxspi, HAL_XSPI_FLAG_BUSY, RESET, tickstart, hxspi->Timeout);
          if (status != HAL_OK)
          {
            return status;
          }
        }
        /* Configure Dual Memory mode and CS Selection */
        MODIFY_REG(hxspi->Instance->CR, (XSPI_CR_DMM | XSPI_CR_CSSEL),
                   (hxspi->Init.MemoryMode | hxspi->Init.MemorySelect));

        /* Configure sample shifting and delay hold quarter cycle */
        MODIFY_REG(hxspi->Instance->TCR, (XSPI_TCR_SSHIFT | XSPI_TCR_DHQC),
                   (hxspi->Init.SampleShifting | hxspi->Init.DelayHoldQuarterCycle));

        /* Enable XSPI */
        HAL_XSPI_ENABLE(hxspi);

        /* Enable free running clock if needed : must be done after XSPI enable */
        if (hxspi->Init.FreeRunningClock == HAL_XSPI_FREERUNCLK_ENABLE)
        {
          SET_BIT(hxspi->Instance->DCR1, XSPI_DCR1_FRCK);
        }

        if (hxspi->Init.MemoryExtended == HAL_XSPI_CSSEL_HW)
        {
          SET_BIT(hxspi->Instance->DCR1, XSPI_DCR1_EXTENDMEM);
        }

        /* Initialize the XSPI state */
        if (hxspi->Init.MemoryType == HAL_XSPI_MEMTYPE_HYPERBUS)
        {
          hxspi->State = HAL_XSPI_STATE_HYPERBUS_INIT;
        }
        else
        {
          hxspi->State = HAL_XSPI_STATE_READY;
        }
      }
    }
  }
  return status;
}

/**
  * @brief  Initialize the XSPI MSP.
  * @param  hxspi : XSPI handle
  * @retval None
  */
__weak void HAL_XSPI_MspInit(XSPI_HandleTypeDef *hxspi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hxspi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_XSPI_MspInit can be implemented in the user file
   */
}

/**
  * @brief  De-Initialize the XSPI peripheral.
  * @param  hxspi : XSPI handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_DeInit(XSPI_HandleTypeDef *hxspi)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the XSPI handle allocation */
  if (hxspi == NULL)
  {
    status = HAL_ERROR;
    /* No error code can be set as the handler is null */
  }
  else
  {
    /* Disable XSPI */
    HAL_XSPI_DISABLE(hxspi);

    /* Disable free running clock if needed : must be done after XSPI disable */
    CLEAR_BIT(hxspi->Instance->DCR1, XSPI_DCR1_FRCK);

#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
    if (hxspi->MspDeInitCallback == NULL)
    {
      hxspi->MspDeInitCallback = HAL_XSPI_MspDeInit;
    }

    /* De-initialize the low level hardware */
    hxspi->MspDeInitCallback(hxspi);
#else
    /* De-initialize the low-level hardware */
    HAL_XSPI_MspDeInit(hxspi);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */

    /* Reset the driver state */
    hxspi->State = HAL_XSPI_STATE_RESET;
  }

  return status;
}

/**
  * @brief  DeInitialize the XSPI MSP.
  * @param  hxspi : XSPI handle
  * @retval None
  */
__weak void HAL_XSPI_MspDeInit(XSPI_HandleTypeDef *hxspi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hxspi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_XSPI_MspDeInit can be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup XSPI_Exported_Functions_Group2 Input and Output operation functions
  *  @brief XSPI Transmit/Receive functions
  *
@verbatim
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to :
      (+) Handle the interrupts.
      (+) Handle the command sequence (regular and Hyperbus).
      (+) Handle the Hyperbus configuration.
      (+) Transmit data in blocking, interrupt or DMA mode.
      (+) Receive data in blocking, interrupt or DMA mode.
      (+) Manage the auto-polling functional mode.
      (+) Manage the memory-mapped functional mode.

@endverbatim
  * @{
  */

/**
  * @brief  Handle XSPI interrupt request.
  * @param  hxspi : XSPI handle
  * @retval None
  */
void HAL_XSPI_IRQHandler(XSPI_HandleTypeDef *hxspi)
{
  __IO uint32_t *data_reg = &hxspi->Instance->DR;
  uint32_t flag           = hxspi->Instance->SR;
  uint32_t itsource       = hxspi->Instance->CR;
  uint32_t currentstate   = hxspi->State;

  /* XSPI fifo threshold interrupt occurred -------------------------------*/
  if (((flag & HAL_XSPI_FLAG_FT) != 0U) && ((itsource & HAL_XSPI_IT_FT) != 0U))
  {
    if (currentstate == HAL_XSPI_STATE_BUSY_TX)
    {
      /* Write a data in the fifo */
      *((__IO uint8_t *)data_reg) = *hxspi->pBuffPtr;
      hxspi->pBuffPtr++;
      hxspi->XferCount--;
    }
    else if (currentstate == HAL_XSPI_STATE_BUSY_RX)
    {
      /* Read a data from the fifo */
      *hxspi->pBuffPtr = *((__IO uint8_t *)data_reg);
      hxspi->pBuffPtr++;
      hxspi->XferCount--;
    }
    else
    {
      /* Nothing to do */
    }

    if (hxspi->XferCount == 0U)
    {
      /* All data have been received or transmitted for the transfer */
      /* Disable fifo threshold interrupt */
      HAL_XSPI_DISABLE_IT(hxspi, HAL_XSPI_IT_FT);
    }

    /* Fifo threshold callback */
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
    hxspi->FifoThresholdCallback(hxspi);
#else
    HAL_XSPI_FifoThresholdCallback(hxspi);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
  }
  /* XSPI transfer complete interrupt occurred ----------------------------*/
  else if (((flag & HAL_XSPI_FLAG_TC) != 0U) && ((itsource & HAL_XSPI_IT_TC) != 0U))
  {
    if (currentstate == HAL_XSPI_STATE_BUSY_RX)
    {
      if ((hxspi->XferCount > 0U) && ((flag & XSPI_SR_FLEVEL) != 0U))
      {
        /* Read the last data received in the fifo */
        *hxspi->pBuffPtr = *((__IO uint8_t *)data_reg);
        hxspi->pBuffPtr++;
        hxspi->XferCount--;
      }
      else if (hxspi->XferCount == 0U)
      {
        /* Clear flag */
        hxspi->Instance->FCR = HAL_XSPI_FLAG_TC;

        /* Disable the interrupts */
        HAL_XSPI_DISABLE_IT(hxspi, HAL_XSPI_IT_TC | HAL_XSPI_IT_FT | HAL_XSPI_IT_TE);

        hxspi->State = HAL_XSPI_STATE_READY;

        /* RX complete callback */
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
        hxspi->RxCpltCallback(hxspi);
#else
        HAL_XSPI_RxCpltCallback(hxspi);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
      }
      else
      {
        /* Nothing to do */
      }
    }
    else
    {
      /* Clear flag */
      hxspi->Instance->FCR = HAL_XSPI_FLAG_TC;

      /* Disable the interrupts */
      HAL_XSPI_DISABLE_IT(hxspi, HAL_XSPI_IT_TC | HAL_XSPI_IT_FT | HAL_XSPI_IT_TE);

      hxspi->State = HAL_XSPI_STATE_READY;

      if (currentstate == HAL_XSPI_STATE_BUSY_TX)
      {
        /* TX complete callback */
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
        hxspi->TxCpltCallback(hxspi);
#else
        HAL_XSPI_TxCpltCallback(hxspi);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
      }
      else if (currentstate == HAL_XSPI_STATE_BUSY_CMD)
      {
        /* Command complete callback */
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
        hxspi->CmdCpltCallback(hxspi);
#else
        HAL_XSPI_CmdCpltCallback(hxspi);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
      }
      else if (currentstate == HAL_XSPI_STATE_ABORT)
      {
        if (hxspi->ErrorCode == HAL_XSPI_ERROR_NONE)
        {
          /* Abort called by the user */
          /* Abort complete callback */
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
          hxspi->AbortCpltCallback(hxspi);
#else
          HAL_XSPI_AbortCpltCallback(hxspi);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
        }
        else
        {
          /* Abort due to an error (eg : DMA error) */
          /* Error callback */
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
          hxspi->ErrorCallback(hxspi);
#else
          HAL_XSPI_ErrorCallback(hxspi);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
        }
      }
      else
      {
        /* Nothing to do */
      }
    }
  }
  /* XSPI status match interrupt occurred ---------------------------------*/
  else if (((flag & HAL_XSPI_FLAG_SM) != 0U) && ((itsource & HAL_XSPI_IT_SM) != 0U))
  {
    /* Clear flag */
    hxspi->Instance->FCR = HAL_XSPI_FLAG_SM;

    /* Check if automatic poll mode stop is activated */
    if ((hxspi->Instance->CR & XSPI_CR_APMS) != 0U)
    {
      /* Disable the interrupts */
      HAL_XSPI_DISABLE_IT(hxspi, HAL_XSPI_IT_SM | HAL_XSPI_IT_TE);

      hxspi->State = HAL_XSPI_STATE_READY;
    }

    /* Status match callback */
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
    hxspi->StatusMatchCallback(hxspi);
#else
    HAL_XSPI_StatusMatchCallback(hxspi);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
  }
  /* XSPI transfer error interrupt occurred -------------------------------*/
  else if (((flag & HAL_XSPI_FLAG_TE) != 0U) && ((itsource & HAL_XSPI_IT_TE) != 0U))
  {
    /* Clear flag */
    hxspi->Instance->FCR = HAL_XSPI_FLAG_TE;

    /* Disable all interrupts */
    HAL_XSPI_DISABLE_IT(hxspi, (HAL_XSPI_IT_TO | HAL_XSPI_IT_SM | HAL_XSPI_IT_FT | HAL_XSPI_IT_TC | HAL_XSPI_IT_TE));

    /* Set error code */
    hxspi->ErrorCode = HAL_XSPI_ERROR_TRANSFER;

    /* Check if the DMA is enabled */
    if ((hxspi->Instance->CR & XSPI_CR_DMAEN) != 0U)
    {
      /* Disable the DMA transfer on the XSPI side */
      CLEAR_BIT(hxspi->Instance->CR, XSPI_CR_DMAEN);

      /* Disable the DMA transmit on the DMA side */
      hxspi->hdmatx->XferAbortCallback = XSPI_DMAAbortCplt;
      if (HAL_DMA_Abort_IT(hxspi->hdmatx) != HAL_OK)
      {
        hxspi->State = HAL_XSPI_STATE_READY;

        /* Error callback */
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
        hxspi->ErrorCallback(hxspi);
#else
        HAL_XSPI_ErrorCallback(hxspi);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
      }

      /* Disable the DMA receive on the DMA side */
      hxspi->hdmarx->XferAbortCallback = XSPI_DMAAbortCplt;
      if (HAL_DMA_Abort_IT(hxspi->hdmarx) != HAL_OK)
      {
        hxspi->State = HAL_XSPI_STATE_READY;

        /* Error callback */
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
        hxspi->ErrorCallback(hxspi);
#else
        HAL_XSPI_ErrorCallback(hxspi);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
      }
    }
    else
    {
      hxspi->State = HAL_XSPI_STATE_READY;

      /* Error callback */
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
      hxspi->ErrorCallback(hxspi);
#else
      HAL_XSPI_ErrorCallback(hxspi);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
    }
  }
  /* XSPI timeout interrupt occurred --------------------------------------*/
  else if (((flag & HAL_XSPI_FLAG_TO) != 0U) && ((itsource & HAL_XSPI_IT_TO) != 0U))
  {
    /* Clear flag */
    hxspi->Instance->FCR = HAL_XSPI_FLAG_TO;

    /* Timeout callback */
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
    hxspi->TimeOutCallback(hxspi);
#else
    HAL_XSPI_TimeOutCallback(hxspi);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
  }
  else
  {
    /* Nothing to do */
  }
}

/**
  * @brief  Set the command configuration.
  * @param  hxspi   : XSPI handle
  * @param  pCmd     : structure that contains the command configuration information
  * @param  Timeout : Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_Command(XSPI_HandleTypeDef *hxspi, const XSPI_RegularCmdTypeDef *pCmd, uint32_t Timeout)
{
  HAL_StatusTypeDef status;
  uint32_t state;
  uint32_t tickstart = HAL_GetTick();

  /* Check the parameters of the command structure */
  assert_param(IS_XSPI_OPERATION_TYPE(pCmd->OperationType));
  if (hxspi->Init.MemoryMode == HAL_XSPI_SINGLE_MEM)
  {
    assert_param(IS_XSPI_IO_SELECT(pCmd->IOSelect));
  }

  assert_param(IS_XSPI_INSTRUCTION_MODE(pCmd->InstructionMode));
  if (pCmd->InstructionMode != HAL_XSPI_INSTRUCTION_NONE)
  {
    assert_param(IS_XSPI_INSTRUCTION_WIDTH(pCmd->InstructionWidth));
    assert_param(IS_XSPI_INSTRUCTION_DTR_MODE(pCmd->InstructionDTRMode));
  }

  assert_param(IS_XSPI_ADDRESS_MODE(pCmd->AddressMode));
  if (pCmd->AddressMode != HAL_XSPI_ADDRESS_NONE)
  {
    assert_param(IS_XSPI_ADDRESS_WIDTH(pCmd->AddressWidth));
    assert_param(IS_XSPI_ADDRESS_DTR_MODE(pCmd->AddressDTRMode));
  }

  assert_param(IS_XSPI_ALT_BYTES_MODE(pCmd->AlternateBytesMode));
  if (pCmd->AlternateBytesMode != HAL_XSPI_ALT_BYTES_NONE)
  {
    assert_param(IS_XSPI_ALT_BYTES_WIDTH(pCmd->AlternateBytesWidth));
    assert_param(IS_XSPI_ALT_BYTES_DTR_MODE(pCmd->AlternateBytesDTRMode));
  }

  assert_param(IS_XSPI_DATA_MODE(hxspi->Init.MemoryType, pCmd->DataMode));

  if (pCmd->DataMode != HAL_XSPI_DATA_NONE)
  {
    if (pCmd->OperationType == HAL_XSPI_OPTYPE_COMMON_CFG)
    {
      assert_param(IS_XSPI_DATA_LENGTH(pCmd->DataLength));
    }
    assert_param(IS_XSPI_DATA_DTR_MODE(pCmd->DataDTRMode));
    assert_param(IS_XSPI_DUMMY_CYCLES(pCmd->DummyCycles));
  }

  assert_param(IS_XSPI_DQS_MODE(pCmd->DQSMode));

  /* Check the state of the driver */
  state = hxspi->State;
  if (((state == HAL_XSPI_STATE_READY)         && (hxspi->Init.MemoryType != HAL_XSPI_MEMTYPE_HYPERBUS)) ||
      ((state == HAL_XSPI_STATE_READ_CMD_CFG)  && ((pCmd->OperationType == HAL_XSPI_OPTYPE_WRITE_CFG) ||
                                                   (pCmd->OperationType == HAL_XSPI_OPTYPE_WRAP_CFG))) ||
      ((state == HAL_XSPI_STATE_WRITE_CMD_CFG) &&
       ((pCmd->OperationType == HAL_XSPI_OPTYPE_READ_CFG)  ||
        (pCmd->OperationType == HAL_XSPI_OPTYPE_WRAP_CFG))))
  {
    /* Wait till busy flag is reset */
    status = XSPI_WaitFlagStateUntilTimeout(hxspi, HAL_XSPI_FLAG_BUSY, RESET, tickstart, Timeout);

    if (status == HAL_OK)
    {
      /* Initialize error code */
      hxspi->ErrorCode = HAL_XSPI_ERROR_NONE;

      /* Configure the registers */
      status = XSPI_ConfigCmd(hxspi, pCmd);

      if (status == HAL_OK)
      {
        if (pCmd->DataMode == HAL_XSPI_DATA_NONE)
        {
          /* When there is no data phase, the transfer start as soon as the configuration is done
             so wait until BUSY flag is reset to go back in idle state. */
          status = XSPI_WaitFlagStateUntilTimeout(hxspi, HAL_XSPI_FLAG_BUSY, RESET, tickstart, Timeout);

          /* Clear TC flag */
          HAL_XSPI_CLEAR_FLAG(hxspi, HAL_XSPI_FLAG_TC);
        }
        else
        {
          /* Update the state */
          if (pCmd->OperationType == HAL_XSPI_OPTYPE_COMMON_CFG)
          {
            hxspi->State = HAL_XSPI_STATE_CMD_CFG;
          }
          else if (pCmd->OperationType == HAL_XSPI_OPTYPE_READ_CFG)
          {
            if (hxspi->State == HAL_XSPI_STATE_WRITE_CMD_CFG)
            {
              hxspi->State = HAL_XSPI_STATE_CMD_CFG;
            }
            else
            {
              hxspi->State = HAL_XSPI_STATE_READ_CMD_CFG;
            }
          }
          else if (pCmd->OperationType == HAL_XSPI_OPTYPE_WRITE_CFG)
          {
            if (hxspi->State == HAL_XSPI_STATE_READ_CMD_CFG)
            {
              hxspi->State = HAL_XSPI_STATE_CMD_CFG;
            }
            else
            {
              hxspi->State = HAL_XSPI_STATE_WRITE_CMD_CFG;
            }
          }
          else
          {
            /* Wrap configuration, no state change */
          }
        }
      }
    }
    else
    {
      status = HAL_BUSY;
    }
  }
  else
  {
    status = HAL_ERROR;
    hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_SEQUENCE;
  }

  return status;
}

/**
  * @brief  Set the command configuration in interrupt mode.
  * @param  hxspi : XSPI handle
  * @param  pCmd   : structure that contains the command configuration information
  * @note   This function is used only in Indirect Read or Write Modes
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_Command_IT(XSPI_HandleTypeDef *hxspi, const XSPI_RegularCmdTypeDef *pCmd)
{
  HAL_StatusTypeDef status;
  uint32_t tickstart = HAL_GetTick();

  /* Check the parameters of the command structure */
  assert_param(IS_XSPI_OPERATION_TYPE(pCmd->OperationType));

  if (hxspi->Init.MemoryMode == HAL_XSPI_SINGLE_MEM)
  {
    assert_param(IS_XSPI_IO_SELECT(pCmd->IOSelect));
  }

  assert_param(IS_XSPI_INSTRUCTION_MODE(pCmd->InstructionMode));
  if (pCmd->InstructionMode != HAL_XSPI_INSTRUCTION_NONE)
  {
    assert_param(IS_XSPI_INSTRUCTION_WIDTH(pCmd->InstructionWidth));
    assert_param(IS_XSPI_INSTRUCTION_DTR_MODE(pCmd->InstructionDTRMode));
  }

  assert_param(IS_XSPI_ADDRESS_MODE(pCmd->AddressMode));
  if (pCmd->AddressMode != HAL_XSPI_ADDRESS_NONE)
  {
    assert_param(IS_XSPI_ADDRESS_WIDTH(pCmd->AddressWidth));
    assert_param(IS_XSPI_ADDRESS_DTR_MODE(pCmd->AddressDTRMode));
  }

  assert_param(IS_XSPI_ALT_BYTES_MODE(pCmd->AlternateBytesMode));
  if (pCmd->AlternateBytesMode != HAL_XSPI_ALT_BYTES_NONE)
  {
    assert_param(IS_XSPI_ALT_BYTES_WIDTH(pCmd->AlternateBytesWidth));
    assert_param(IS_XSPI_ALT_BYTES_DTR_MODE(pCmd->AlternateBytesDTRMode));
  }

  assert_param(IS_XSPI_DATA_MODE(hxspi->Init.MemoryType, pCmd->DataMode));

  if (pCmd->DataMode != HAL_XSPI_DATA_NONE)
  {
    assert_param(IS_XSPI_DATA_LENGTH(pCmd->DataLength));
    assert_param(IS_XSPI_DATA_DTR_MODE(pCmd->DataDTRMode));
    assert_param(IS_XSPI_DUMMY_CYCLES(pCmd->DummyCycles));
  }

  assert_param(IS_XSPI_DQS_MODE(pCmd->DQSMode));

  /* Check the state of the driver */
  if ((hxspi->State  == HAL_XSPI_STATE_READY) && (pCmd->OperationType     == HAL_XSPI_OPTYPE_COMMON_CFG) &&
      (pCmd->DataMode == HAL_XSPI_DATA_NONE)   && (hxspi->Init.MemoryType != HAL_XSPI_MEMTYPE_HYPERBUS))
  {
    /* Wait till busy flag is reset */
    status = XSPI_WaitFlagStateUntilTimeout(hxspi, HAL_XSPI_FLAG_BUSY, RESET, tickstart, hxspi->Timeout);

    if (status == HAL_OK)
    {
      /* Initialize error code */
      hxspi->ErrorCode = HAL_XSPI_ERROR_NONE;

      /* Clear flags related to interrupt */
      HAL_XSPI_CLEAR_FLAG(hxspi, HAL_XSPI_FLAG_TE | HAL_XSPI_FLAG_TC);

      /* Configure the registers */
      status = XSPI_ConfigCmd(hxspi, pCmd);

      if (status == HAL_OK)
      {
        /* Update the state */
        hxspi->State = HAL_XSPI_STATE_BUSY_CMD;

        /* Enable the transfer complete and transfer error interrupts */
        HAL_XSPI_ENABLE_IT(hxspi, HAL_XSPI_IT_TC | HAL_XSPI_IT_TE);
      }
    }
  }
  else
  {
    status = HAL_ERROR;
    hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_SEQUENCE;
  }

  return status;
}

/**
  * @brief  Configure the Hyperbus parameters.
  * @param  hxspi   : XSPI handle
  * @param  pCfg     : Pointer to Structure containing the Hyperbus configuration
  * @param  Timeout : Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_HyperbusCfg(XSPI_HandleTypeDef *hxspi, const XSPI_HyperbusCfgTypeDef *pCfg,
                                       uint32_t Timeout)
{
  HAL_StatusTypeDef status;
  uint32_t state;
  uint32_t tickstart = HAL_GetTick();

  /* Check the parameters of the hyperbus configuration structure */
  assert_param(IS_XSPI_RW_RECOVERY_TIME_CYCLE(pCfg->RWRecoveryTimeCycle));
  assert_param(IS_XSPI_ACCESS_TIME_CYCLE(pCfg->AccessTimeCycle));
  assert_param(IS_XSPI_WRITE_ZERO_LATENCY(pCfg->WriteZeroLatency));
  assert_param(IS_XSPI_LATENCY_MODE(pCfg->LatencyMode));

  /* Check the state of the driver */
  state = hxspi->State;
  if ((state == HAL_XSPI_STATE_HYPERBUS_INIT) || (state == HAL_XSPI_STATE_READY))
  {
    /* Wait till busy flag is reset */
    status = XSPI_WaitFlagStateUntilTimeout(hxspi, HAL_XSPI_FLAG_BUSY, RESET, tickstart, Timeout);

    if (status == HAL_OK)
    {
      /* Configure Hyperbus configuration Latency register */
      WRITE_REG(hxspi->Instance->HLCR, ((pCfg->RWRecoveryTimeCycle << XSPI_HLCR_TRWR_Pos) |
                                        (pCfg->AccessTimeCycle << XSPI_HLCR_TACC_Pos)     |
                                        pCfg->WriteZeroLatency | pCfg->LatencyMode));

      /* Update the state */
      hxspi->State = HAL_XSPI_STATE_READY;
    }
    else
    {
      status = HAL_BUSY;
    }
  }
  else
  {
    status = HAL_ERROR;
    hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_SEQUENCE;
  }

  return status;
}

/**
  * @brief  Set the Hyperbus command configuration.
  * @param  hxspi   : XSPI handle
  * @param  pCmd     : Structure containing the Hyperbus command
  * @param  Timeout : Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_HyperbusCmd(XSPI_HandleTypeDef *hxspi, const XSPI_HyperbusCmdTypeDef *pCmd,
                                       uint32_t Timeout)
{
  HAL_StatusTypeDef status;
  uint32_t tickstart = HAL_GetTick();

  /* Check the parameters of the hyperbus command structure */
  assert_param(IS_XSPI_ADDRESS_SPACE(pCmd->AddressSpace));
  assert_param(IS_XSPI_ADDRESS_WIDTH(pCmd->AddressWidth));
  assert_param(IS_XSPI_DATA_LENGTH(pCmd->DataLength));
  assert_param(IS_XSPI_DQS_MODE(pCmd->DQSMode));
  assert_param(IS_XSPI_DATA_MODE(hxspi->Init.MemoryType, pCmd->DataMode));

  /* Check the state of the driver */
  if ((hxspi->State == HAL_XSPI_STATE_READY) && (hxspi->Init.MemoryType == HAL_XSPI_MEMTYPE_HYPERBUS))
  {
    /* Wait till busy flag is reset */
    status = XSPI_WaitFlagStateUntilTimeout(hxspi, HAL_XSPI_FLAG_BUSY, RESET, tickstart, Timeout);

    if (status == HAL_OK)
    {
      /* Re-initialize the value of the functional mode */
      MODIFY_REG(hxspi->Instance->CR, XSPI_CR_FMODE, 0U);

      /* Configure the address space in the DCR1 register */
      MODIFY_REG(hxspi->Instance->DCR1, XSPI_DCR1_MTYP_0, pCmd->AddressSpace);

      /* Configure the CCR and WCCR registers with the address size and the following configuration :
         - DQS signal enabled (used as RWDS)
         - DTR mode enabled on address and data */
      /* - address and data on 8 or 16 lines */
      WRITE_REG(hxspi->Instance->CCR, (pCmd->DQSMode | XSPI_CCR_DDTR | pCmd->DataMode |
                                       pCmd->AddressWidth | XSPI_CCR_ADDTR | XSPI_CCR_ADMODE_2));
      WRITE_REG(hxspi->Instance->WCCR, (pCmd->DQSMode | XSPI_WCCR_DDTR | pCmd->DataMode |
                                        pCmd->AddressWidth | XSPI_WCCR_ADDTR | XSPI_WCCR_ADMODE_2));

      /* Configure the DLR register with the number of data */
      WRITE_REG(hxspi->Instance->DLR, (pCmd->DataLength - 1U));

      /* Configure the AR register with the address value */
      WRITE_REG(hxspi->Instance->AR, pCmd->Address);

      /* Update the state */
      hxspi->State = HAL_XSPI_STATE_CMD_CFG;
    }
    else
    {
      status = HAL_BUSY;
    }
  }
  else
  {
    status = HAL_ERROR;
    hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_SEQUENCE;
  }

  return status;
}

/**
  * @brief  Transmit an amount of data in blocking mode.
  * @param  hxspi   : XSPI handle
  * @param  pData   : pointer to data buffer
  * @param  Timeout : Timeout duration
  * @note   This function is used only in Indirect Write Mode
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_Transmit(XSPI_HandleTypeDef *hxspi, const uint8_t *pData, uint32_t Timeout)
{
  HAL_StatusTypeDef status;
  uint32_t tickstart = HAL_GetTick();
  __IO uint32_t *data_reg = &hxspi->Instance->DR;

  /* Check the data pointer allocation */
  if (pData == NULL)
  {
    status = HAL_ERROR;
    hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_PARAM;
  }
  else
  {
    /* Check the state */
    if (hxspi->State == HAL_XSPI_STATE_CMD_CFG)
    {
      /* Configure counters and size */
      hxspi->XferCount = READ_REG(hxspi->Instance->DLR) + 1U;
      hxspi->XferSize  = hxspi->XferCount;
      hxspi->pBuffPtr  = (uint8_t *)pData;

      /* Configure CR register with functional mode as indirect write */
      MODIFY_REG(hxspi->Instance->CR, XSPI_CR_FMODE, XSPI_FUNCTIONAL_MODE_INDIRECT_WRITE);

      do
      {
        /* Wait till fifo threshold flag is set to send data */
        status = XSPI_WaitFlagStateUntilTimeout(hxspi, HAL_XSPI_FLAG_FT, SET, tickstart, Timeout);

        if (status != HAL_OK)
        {
          break;
        }

        *((__IO uint8_t *)data_reg) = *hxspi->pBuffPtr;
        hxspi->pBuffPtr++;
        hxspi->XferCount--;
      } while (hxspi->XferCount > 0U);

      if (status == HAL_OK)
      {
        /* Wait till transfer complete flag is set to go back in idle state */
        status = XSPI_WaitFlagStateUntilTimeout(hxspi, HAL_XSPI_FLAG_TC, SET, tickstart, Timeout);

        if (status == HAL_OK)
        {
          /* Clear transfer complete flag */
          HAL_XSPI_CLEAR_FLAG(hxspi, HAL_XSPI_FLAG_TC);

          hxspi->State = HAL_XSPI_STATE_READY;
        }
      }
    }
    else
    {
      status = HAL_ERROR;
      hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_SEQUENCE;
    }
  }

  return status;
}

/**
  * @brief  Receive an amount of data in blocking mode.
  * @param  hxspi   : XSPI handle
  * @param  pData   : pointer to data buffer
  * @param  Timeout : Timeout duration
  * @note   This function is used only in Indirect Read Mode
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_Receive(XSPI_HandleTypeDef *hxspi, uint8_t *pData, uint32_t Timeout)
{
  HAL_StatusTypeDef status;
  uint32_t tickstart = HAL_GetTick();
  __IO uint32_t *data_reg = &hxspi->Instance->DR;
  uint32_t addr_reg = hxspi->Instance->AR;
  uint32_t ir_reg = hxspi->Instance->IR;

  /* Check the data pointer allocation */
  if (pData == NULL)
  {
    status = HAL_ERROR;
    hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_PARAM;
  }
  else
  {
    /* Check the state */
    if (hxspi->State == HAL_XSPI_STATE_CMD_CFG)
    {
      /* Configure counters and size */
      hxspi->XferCount = READ_REG(hxspi->Instance->DLR) + 1U;
      hxspi->XferSize  = hxspi->XferCount;
      hxspi->pBuffPtr  = pData;

      /* Configure CR register with functional mode as indirect read */
      MODIFY_REG(hxspi->Instance->CR, XSPI_CR_FMODE, XSPI_FUNCTIONAL_MODE_INDIRECT_READ);

      /* Trig the transfer by re-writing address or instruction register */
      if (hxspi->Init.MemoryType == HAL_XSPI_MEMTYPE_HYPERBUS)
      {
        WRITE_REG(hxspi->Instance->AR, addr_reg);
      }
      else
      {
        if (READ_BIT(hxspi->Instance->CCR, XSPI_CCR_ADMODE) != HAL_XSPI_ADDRESS_NONE)
        {
          WRITE_REG(hxspi->Instance->AR, addr_reg);
        }
        else
        {
          WRITE_REG(hxspi->Instance->IR, ir_reg);
        }
      }

      do
      {
        /* Wait till fifo threshold or transfer complete flags are set to read received data */
        status = XSPI_WaitFlagStateUntilTimeout(hxspi, (HAL_XSPI_FLAG_FT | HAL_XSPI_FLAG_TC), SET, tickstart, Timeout);

        if (status != HAL_OK)
        {
          break;
        }

        *hxspi->pBuffPtr = *((__IO uint8_t *)data_reg);
        hxspi->pBuffPtr++;
        hxspi->XferCount--;
      } while (hxspi->XferCount > 0U);

      if (status == HAL_OK)
      {
        /* Wait till transfer complete flag is set to go back in idle state */
        status = XSPI_WaitFlagStateUntilTimeout(hxspi, HAL_XSPI_FLAG_TC, SET, tickstart, Timeout);

        if (status == HAL_OK)
        {
          /* Clear transfer complete flag */
          HAL_XSPI_CLEAR_FLAG(hxspi, HAL_XSPI_FLAG_TC);

          hxspi->State = HAL_XSPI_STATE_READY;
        }
      }
    }
    else
    {
      status = HAL_ERROR;
      hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_SEQUENCE;
    }
  }

  return status;
}

/**
  * @brief  Send an amount of data in non-blocking mode with interrupt.
  * @param  hxspi : XSPI handle
  * @param  pData : pointer to data buffer
  * @note   This function is used only in Indirect Write Mode
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_Transmit_IT(XSPI_HandleTypeDef *hxspi, const uint8_t *pData)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the data pointer allocation */
  if (pData == NULL)
  {
    status = HAL_ERROR;
    hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_PARAM;
  }
  else
  {
    /* Check the state */
    if (hxspi->State == HAL_XSPI_STATE_CMD_CFG)
    {
      /* Configure counters and size */
      hxspi->XferCount = READ_REG(hxspi->Instance->DLR) + 1U;
      hxspi->XferSize  = hxspi->XferCount;
      hxspi->pBuffPtr  = (uint8_t *)pData;

      /* Configure CR register with functional mode as indirect write */
      MODIFY_REG(hxspi->Instance->CR, XSPI_CR_FMODE, XSPI_FUNCTIONAL_MODE_INDIRECT_WRITE);

      /* Clear flags related to interrupt */
      HAL_XSPI_CLEAR_FLAG(hxspi, HAL_XSPI_FLAG_TE | HAL_XSPI_FLAG_TC);

      /* Update the state */
      hxspi->State = HAL_XSPI_STATE_BUSY_TX;

      /* Enable the transfer complete, fifo threshold and transfer error interrupts */
      HAL_XSPI_ENABLE_IT(hxspi, HAL_XSPI_IT_TC | HAL_XSPI_IT_FT | HAL_XSPI_IT_TE);
    }
    else
    {
      status = HAL_ERROR;
      hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_SEQUENCE;
    }
  }

  return status;
}

/**
  * @brief  Receive an amount of data in non-blocking mode with interrupt.
  * @param  hxspi : XSPI handle
  * @param  pData : pointer to data buffer
  * @note   This function is used only in Indirect Read Mode
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_Receive_IT(XSPI_HandleTypeDef *hxspi, uint8_t *pData)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t addr_reg = hxspi->Instance->AR;
  uint32_t ir_reg = hxspi->Instance->IR;

  /* Check the data pointer allocation */
  if (pData == NULL)
  {
    status = HAL_ERROR;
    hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_PARAM;
  }
  else
  {
    /* Check the state */
    if (hxspi->State == HAL_XSPI_STATE_CMD_CFG)
    {
      /* Configure counters and size */
      hxspi->XferCount = READ_REG(hxspi->Instance->DLR) + 1U;
      hxspi->XferSize  = hxspi->XferCount;
      hxspi->pBuffPtr  = pData;

      /* Configure CR register with functional mode as indirect read */
      MODIFY_REG(hxspi->Instance->CR, XSPI_CR_FMODE, XSPI_FUNCTIONAL_MODE_INDIRECT_READ);

      /* Clear flags related to interrupt */
      HAL_XSPI_CLEAR_FLAG(hxspi, HAL_XSPI_FLAG_TE | HAL_XSPI_FLAG_TC);

      /* Update the state */
      hxspi->State = HAL_XSPI_STATE_BUSY_RX;

      /* Enable the transfer complete, fifo threshold and transfer error interrupts */
      HAL_XSPI_ENABLE_IT(hxspi, HAL_XSPI_IT_TC | HAL_XSPI_IT_FT | HAL_XSPI_IT_TE);

      /* Trig the transfer by re-writing address or instruction register */
      if (hxspi->Init.MemoryType == HAL_XSPI_MEMTYPE_HYPERBUS)
      {
        WRITE_REG(hxspi->Instance->AR, addr_reg);
      }
      else
      {
        if (READ_BIT(hxspi->Instance->CCR, XSPI_CCR_ADMODE) != HAL_XSPI_ADDRESS_NONE)
        {
          WRITE_REG(hxspi->Instance->AR, addr_reg);
        }
        else
        {
          WRITE_REG(hxspi->Instance->IR, ir_reg);
        }
      }
    }
    else
    {
      status = HAL_ERROR;
      hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_SEQUENCE;
    }
  }

  return status;
}

/**
  * @brief  Send an amount of data in non-blocking mode with DMA.
  * @param  hxspi : XSPI handle
  * @param  pData : pointer to data buffer
  * @note   This function is used only in Indirect Write Mode
  * @note   If DMA peripheral access is configured as halfword, the number
  *         of data and the fifo threshold should be aligned on halfword
  * @note   If DMA peripheral access is configured as word, the number
  *         of data and the fifo threshold should be aligned on word
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_Transmit_DMA(XSPI_HandleTypeDef *hxspi, const uint8_t *pData)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t data_size = hxspi->Instance->DLR + 1U;
  DMA_QListTypeDef *p_queue = {NULL};
  uint32_t data_width = DMA_DEST_DATAWIDTH_BYTE;

  /* Check the data pointer allocation */
  if (pData == NULL)
  {
    status = HAL_ERROR;
    hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_PARAM;
  }
  else
  {
    /* Check the state */
    if (hxspi->State == HAL_XSPI_STATE_CMD_CFG)
    {
      if ((hxspi->hdmatx->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
      {
        p_queue = hxspi->hdmatx->LinkedListQueue;
        if ((p_queue != NULL) && (p_queue->Head != NULL))
        {
          data_width = p_queue->Head->LinkRegisters[NODE_CTR1_DEFAULT_OFFSET] & DMA_CTR1_DDW_LOG2;
        }
        else
        {
          /* Set Error Code function status  */
          hxspi->ErrorCode = HAL_XSPI_ERROR_DMA;

          /* Return function status */
          status = HAL_ERROR;
        }
      }
      else
      {
        data_width = hxspi->hdmatx->Init.DestDataWidth;
      }
      /* Configure counters and size */
      if (data_width == DMA_DEST_DATAWIDTH_BYTE)
      {
        hxspi->XferCount = data_size;
      }
      else if (data_width == DMA_DEST_DATAWIDTH_HALFWORD)
      {
        if (((data_size % 2U) != 0U) || ((hxspi->Init.FifoThresholdByte % 2U) != 0U))
        {
          /* The number of data or the fifo threshold is not aligned on halfword
          => no transfer possible with DMA peripheral access configured as halfword */
          hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_PARAM;
          status = HAL_ERROR;
        }
        else
        {
          hxspi->XferCount = data_size;
        }
      }
      else if (data_width == DMA_DEST_DATAWIDTH_WORD)
      {
        if (((data_size % 4U) != 0U) || ((hxspi->Init.FifoThresholdByte % 4U) != 0U))
        {
          /* The number of data or the fifo threshold is not aligned on word
          => no transfer possible with DMA peripheral access configured as word */
          hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_PARAM;
          status = HAL_ERROR;
        }
        else
        {
          hxspi->XferCount = data_size;
        }
      }
      else
      {
        /* Nothing to do */
      }

      if (status == HAL_OK)
      {
        hxspi->XferSize = hxspi->XferCount;
        hxspi->pBuffPtr = (uint8_t *)pData;

        /* Configure CR register with functional mode as indirect write */
        MODIFY_REG(hxspi->Instance->CR, XSPI_CR_FMODE, XSPI_FUNCTIONAL_MODE_INDIRECT_WRITE);

        /* Clear flags related to interrupt */
        HAL_XSPI_CLEAR_FLAG(hxspi, HAL_XSPI_FLAG_TE | HAL_XSPI_FLAG_TC);

        /* Update the state */
        hxspi->State = HAL_XSPI_STATE_BUSY_TX;

        /* Set the DMA transfer complete callback */
        hxspi->hdmatx->XferCpltCallback = XSPI_DMACplt;

        /* Set the DMA Half transfer complete callback */
        hxspi->hdmatx->XferHalfCpltCallback = XSPI_DMAHalfCplt;

        /* Set the DMA error callback */
        hxspi->hdmatx->XferErrorCallback = XSPI_DMAError;

        /* Clear the DMA abort callback */
        hxspi->hdmatx->XferAbortCallback = NULL;

        /* Enable the transmit DMA Channel */
        if ((hxspi->hdmatx->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
        {
          if (hxspi->hdmatx->LinkedListQueue != NULL)
          {
            /* Enable the DMA channel */
            MODIFY_REG(p_queue->Head->LinkRegisters[NODE_CTR1_DEFAULT_OFFSET], \
                       (DMA_CTR1_SINC | DMA_CTR1_DINC), (DMA_SINC_INCREMENTED | DMA_DINC_FIXED));
            MODIFY_REG(p_queue->Head->LinkRegisters[NODE_CTR2_DEFAULT_OFFSET], \
                       DMA_CTR2_DREQ, DMA_MEMORY_TO_PERIPH);
            /* Set DMA data size*/
            p_queue->Head->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] = hxspi->XferSize;
            /* Set DMA source address */
            p_queue->Head->LinkRegisters[NODE_CSAR_DEFAULT_OFFSET] = (uint32_t)pData;
            /* Set DMA destination address */
            p_queue->Head->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET] = (uint32_t)&hxspi->Instance->DR;

            status = HAL_DMAEx_List_Start_IT(hxspi->hdmatx);
          }
          else
          {
            /* Set Error Code */
            hxspi->ErrorCode = HAL_XSPI_ERROR_DMA;

            hxspi->State = HAL_XSPI_STATE_READY;

            /* Return function status */
            status = HAL_ERROR;
          }
        }
        else
        {
          if ((hxspi->hdmatx->Init.Direction == DMA_MEMORY_TO_PERIPH) &&
              (hxspi->hdmatx->Init.SrcInc == DMA_SINC_INCREMENTED) && (hxspi->hdmatx->Init.DestInc == DMA_DINC_FIXED))
          {
            status = HAL_DMA_Start_IT(hxspi->hdmatx, (uint32_t)pData, (uint32_t)&hxspi->Instance->DR, hxspi->XferSize);
          }
          else
          {
            /* no transmit possible with DMA peripheral, invalid configuration */
            hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_PARAM;
            status = HAL_ERROR;
          }
        }
        if (status == HAL_OK)
        {
          /* Enable the transfer error interrupt */
          HAL_XSPI_ENABLE_IT(hxspi, HAL_XSPI_IT_TE);

          /* Enable the DMA transfer by setting the DMAEN bit  */
          SET_BIT(hxspi->Instance->CR, XSPI_CR_DMAEN);
        }
        else
        {
          status = HAL_ERROR;
          hxspi->ErrorCode = HAL_XSPI_ERROR_DMA;
          hxspi->State = HAL_XSPI_STATE_READY;
        }
      }
    }
    else
    {
      status = HAL_ERROR;
      hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_SEQUENCE;
    }
  }

  return status;
}

/**
  * @brief  Receive an amount of data in non-blocking mode with DMA.
  * @param  hxspi : XSPI handle
  * @param  pData : pointer to data buffer.
  * @note   This function is used only in Indirect Read Mode
  * @note   If DMA peripheral access is configured as halfword, the number
  *         of data and the fifo threshold should be aligned on halfword
  * @note   If DMA peripheral access is configured as word, the number
  *         of data and the fifo threshold should be aligned on word
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_Receive_DMA(XSPI_HandleTypeDef *hxspi, uint8_t *pData)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t data_size = hxspi->Instance->DLR + 1U;
  uint32_t addr_reg = hxspi->Instance->AR;
  uint32_t ir_reg = hxspi->Instance->IR;
  DMA_QListTypeDef *p_queue = {NULL};
  uint32_t data_width = DMA_DEST_DATAWIDTH_BYTE;

  /* Check the data pointer allocation */
  if (pData == NULL)
  {
    status = HAL_ERROR;
    hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_PARAM;
  }
  else
  {
    /* Check the state */
    if (hxspi->State == HAL_XSPI_STATE_CMD_CFG)
    {
      if ((hxspi->hdmarx->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
      {
        p_queue = hxspi->hdmarx->LinkedListQueue;
        if ((p_queue != NULL) && (p_queue->Head != NULL))
        {
          data_width = p_queue->Head->LinkRegisters[NODE_CTR1_DEFAULT_OFFSET] & DMA_CTR1_DDW_LOG2;
        }
        else
        {
          /* Set Error Code */
          hxspi->ErrorCode = HAL_XSPI_ERROR_DMA;

          /* Return function status */
          status = HAL_ERROR;
        }
      }
      else
      {
        data_width = hxspi->hdmarx->Init.DestDataWidth;
      }

      /* Configure counters and size */
      if (data_width == DMA_DEST_DATAWIDTH_BYTE)
      {
        hxspi->XferCount = data_size;
      }
      else if (data_width == DMA_DEST_DATAWIDTH_HALFWORD)
      {
        if (((data_size % 2U) != 0U) || ((hxspi->Init.FifoThresholdByte % 2U) != 0U))
        {
          /* The number of data or the fifo threshold is not aligned on halfword
          => no transfer possible with DMA peripheral access configured as halfword */
          hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_PARAM;
          status = HAL_ERROR;
        }
        else
        {
          hxspi->XferCount = data_size;
        }
      }
      else if (data_width == DMA_DEST_DATAWIDTH_WORD)
      {
        if (((data_size % 4U) != 0U) || ((hxspi->Init.FifoThresholdByte % 4U) != 0U))
        {
          /* The number of data or the fifo threshold is not aligned on word
          => no transfer possible with DMA peripheral access configured as word */
          hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_PARAM;
          status = HAL_ERROR;
        }
        else
        {
          hxspi->XferCount = data_size;
        }
      }
      else
      {
        /* Nothing to do */
      }

      if (status == HAL_OK)
      {
        hxspi->XferSize  = hxspi->XferCount;
        hxspi->pBuffPtr  = pData;

        /* Configure CR register with functional mode as indirect read */
        MODIFY_REG(hxspi->Instance->CR, XSPI_CR_FMODE, XSPI_FUNCTIONAL_MODE_INDIRECT_READ);

        /* Clear flags related to interrupt */
        HAL_XSPI_CLEAR_FLAG(hxspi, HAL_XSPI_FLAG_TE | HAL_XSPI_FLAG_TC);

        /* Update the state */
        hxspi->State = HAL_XSPI_STATE_BUSY_RX;

        /* Set the DMA transfer complete callback */
        hxspi->hdmarx->XferCpltCallback = XSPI_DMACplt;

        /* Set the DMA Half transfer complete callback */
        hxspi->hdmarx->XferHalfCpltCallback = XSPI_DMAHalfCplt;

        /* Set the DMA error callback */
        hxspi->hdmarx->XferErrorCallback = XSPI_DMAError;

        /* Clear the DMA abort callback */
        hxspi->hdmarx->XferAbortCallback = NULL;

        /* Enable the receive DMA Channel */
        if ((hxspi->hdmarx->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
        {
          if (hxspi->hdmarx->LinkedListQueue != NULL)
          {
            /* Enable the DMA channel */
            MODIFY_REG(p_queue->Head->LinkRegisters[NODE_CTR1_DEFAULT_OFFSET], \
                       (DMA_CTR1_SINC | DMA_CTR1_DINC), (DMA_SINC_FIXED | DMA_DINC_INCREMENTED));
            MODIFY_REG(p_queue->Head->LinkRegisters[NODE_CTR2_DEFAULT_OFFSET], \
                       DMA_CTR2_DREQ, DMA_PERIPH_TO_MEMORY);
            /* Set DMA data size */
            p_queue->Head->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] = hxspi->XferSize;
            /* Set DMA source address */
            p_queue->Head->LinkRegisters[NODE_CSAR_DEFAULT_OFFSET] = (uint32_t)&hxspi->Instance->DR;
            /* Set DMA destination address */
            p_queue->Head->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET] = (uint32_t)pData;

            status = HAL_DMAEx_List_Start_IT(hxspi->hdmarx);
          }
          else
          {
            /* Set Error Code */
            hxspi->ErrorCode = HAL_XSPI_ERROR_DMA;

            hxspi->State = HAL_XSPI_STATE_READY;

            /* Return function status */
            status = HAL_ERROR;
          }
        }
        else
        {
          if ((hxspi->hdmarx->Init.Direction == DMA_PERIPH_TO_MEMORY) && (hxspi->hdmarx->Init.SrcInc == DMA_SINC_FIXED)
              && (hxspi->hdmarx->Init.DestInc == DMA_DINC_INCREMENTED))
          {
            status = HAL_DMA_Start_IT(hxspi->hdmarx, (uint32_t)&hxspi->Instance->DR, (uint32_t)pData, hxspi->XferSize);
          }
          else
          {
            /* no receive possible with DMA peripheral, invalid configuration */
            hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_PARAM;
            status = HAL_ERROR;
          }
        }
        if (status == HAL_OK)
        {
          /* Enable the transfer error interrupt */
          HAL_XSPI_ENABLE_IT(hxspi, HAL_XSPI_IT_TE);

          /* Trig the transfer by re-writing address or instruction register */
          if (hxspi->Init.MemoryType == HAL_XSPI_MEMTYPE_HYPERBUS)
          {
            WRITE_REG(hxspi->Instance->AR, addr_reg);
          }
          else
          {
            if (READ_BIT(hxspi->Instance->CCR, XSPI_CCR_ADMODE) != HAL_XSPI_ADDRESS_NONE)
            {
              WRITE_REG(hxspi->Instance->AR, addr_reg);
            }
            else
            {
              WRITE_REG(hxspi->Instance->IR, ir_reg);
            }
          }

          /* Enable the DMA transfer by setting the DMAEN bit  */
          SET_BIT(hxspi->Instance->CR, XSPI_CR_DMAEN);
        }
        else
        {
          status = HAL_ERROR;
          hxspi->ErrorCode = HAL_XSPI_ERROR_DMA;
          hxspi->State = HAL_XSPI_STATE_READY;
        }
      }
    }
    else
    {
      status = HAL_ERROR;
      hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_SEQUENCE;
    }
  }

  return status;
}

/**
  * @brief  Configure the XSPI Automatic Polling Mode in blocking mode.
  * @param  hxspi   : XSPI handle
  * @param  pCfg     : Pointer to structure that contains the polling configuration information.
  * @param  Timeout : Timeout duration
  * @note   This function is used only in Automatic Polling Mode
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_AutoPolling(XSPI_HandleTypeDef *hxspi, const XSPI_AutoPollingTypeDef *pCfg,
                                       uint32_t Timeout)
{
  HAL_StatusTypeDef status;
  uint32_t tickstart = HAL_GetTick();
  uint32_t addr_reg = hxspi->Instance->AR;
  uint32_t ir_reg = hxspi->Instance->IR;
#ifdef USE_FULL_ASSERT
  uint32_t dlr_reg = hxspi->Instance->DLR;
#endif /* USE_FULL_ASSERT */

  /* Check the parameters of the autopolling configuration structure */
  assert_param(IS_XSPI_MATCH_MODE(pCfg->MatchMode));
  assert_param(IS_XSPI_AUTOMATIC_STOP(pCfg->AutomaticStop));
  assert_param(IS_XSPI_INTERVAL(pCfg->IntervalTime));
  assert_param(IS_XSPI_STATUS_BYTES_SIZE(dlr_reg + 1U));

  /* Check the state */
  if ((hxspi->State == HAL_XSPI_STATE_CMD_CFG) && (pCfg->AutomaticStop == HAL_XSPI_AUTOMATIC_STOP_ENABLE))
  {
    /* Wait till busy flag is reset */
    status = XSPI_WaitFlagStateUntilTimeout(hxspi, HAL_XSPI_FLAG_BUSY, RESET, tickstart, Timeout);

    if (status == HAL_OK)
    {
      /* Configure registers */
      WRITE_REG(hxspi->Instance->PSMAR, pCfg->MatchValue);
      WRITE_REG(hxspi->Instance->PSMKR, pCfg->MatchMask);
      WRITE_REG(hxspi->Instance->PIR,   pCfg->IntervalTime);
      MODIFY_REG(hxspi->Instance->CR, (XSPI_CR_PMM | XSPI_CR_APMS | XSPI_CR_FMODE),
                 (pCfg->MatchMode | pCfg->AutomaticStop | XSPI_FUNCTIONAL_MODE_AUTO_POLLING));

      /* Trig the transfer by re-writing address or instruction register */
      if (hxspi->Init.MemoryType == HAL_XSPI_MEMTYPE_HYPERBUS)
      {
        WRITE_REG(hxspi->Instance->AR, addr_reg);
      }
      else
      {
        if (READ_BIT(hxspi->Instance->CCR, XSPI_CCR_ADMODE) != HAL_XSPI_ADDRESS_NONE)
        {
          WRITE_REG(hxspi->Instance->AR, addr_reg);
        }
        else
        {
          WRITE_REG(hxspi->Instance->IR, ir_reg);
        }
      }

      /* Wait till status match flag is set to go back in idle state */
      status = XSPI_WaitFlagStateUntilTimeout(hxspi, HAL_XSPI_FLAG_SM, SET, tickstart, Timeout);

      if (status == HAL_OK)
      {
        /* Clear status match flag */
        HAL_XSPI_CLEAR_FLAG(hxspi, HAL_XSPI_FLAG_SM);

        hxspi->State = HAL_XSPI_STATE_READY;
      }
    }
    else
    {
      status = HAL_BUSY;
    }
  }
  else
  {
    status = HAL_ERROR;
    hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_SEQUENCE;
  }

  return status;
}

/**
  * @brief  Configure the XSPI Automatic Polling Mode in non-blocking mode.
  * @param  hxspi : XSPI handle
  * @param  pCfg   : Pointer to structure that contains the polling configuration information.
  * @note   This function is used only in Automatic Polling Mode
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_AutoPolling_IT(XSPI_HandleTypeDef *hxspi, const XSPI_AutoPollingTypeDef *pCfg)
{
  HAL_StatusTypeDef status;
  uint32_t tickstart = HAL_GetTick();
  uint32_t addr_reg = hxspi->Instance->AR;
  uint32_t ir_reg = hxspi->Instance->IR;
#ifdef USE_FULL_ASSERT
  uint32_t dlr_reg = hxspi->Instance->DLR;
#endif /* USE_FULL_ASSERT */

  /* Check the parameters of the autopolling configuration structure */
  assert_param(IS_XSPI_MATCH_MODE(pCfg->MatchMode));
  assert_param(IS_XSPI_AUTOMATIC_STOP(pCfg->AutomaticStop));
  assert_param(IS_XSPI_INTERVAL(pCfg->IntervalTime));
  assert_param(IS_XSPI_STATUS_BYTES_SIZE(dlr_reg + 1U));

  /* Check the state */
  if (hxspi->State == HAL_XSPI_STATE_CMD_CFG)
  {
    /* Wait till busy flag is reset */
    status = XSPI_WaitFlagStateUntilTimeout(hxspi, HAL_XSPI_FLAG_BUSY, RESET, tickstart, hxspi->Timeout);

    if (status == HAL_OK)
    {
      /* Configure registers */
      WRITE_REG(hxspi->Instance->PSMAR, pCfg->MatchValue);
      WRITE_REG(hxspi->Instance->PSMKR, pCfg->MatchMask);
      WRITE_REG(hxspi->Instance->PIR,   pCfg->IntervalTime);
      MODIFY_REG(hxspi->Instance->CR, (XSPI_CR_PMM | XSPI_CR_APMS | XSPI_CR_FMODE),
                 (pCfg->MatchMode | pCfg->AutomaticStop | XSPI_FUNCTIONAL_MODE_AUTO_POLLING));

      /* Clear flags related to interrupt */
      HAL_XSPI_CLEAR_FLAG(hxspi, HAL_XSPI_FLAG_TE | HAL_XSPI_FLAG_SM);

      hxspi->State = HAL_XSPI_STATE_BUSY_AUTO_POLLING;

      /* Enable the status match and transfer error interrupts */
      HAL_XSPI_ENABLE_IT(hxspi, HAL_XSPI_IT_SM | HAL_XSPI_IT_TE);

      /* Trig the transfer by re-writing address or instruction register */
      if (hxspi->Init.MemoryType == HAL_XSPI_MEMTYPE_HYPERBUS)
      {
        WRITE_REG(hxspi->Instance->AR, addr_reg);
      }
      else
      {
        if (READ_BIT(hxspi->Instance->CCR, XSPI_CCR_ADMODE) != HAL_XSPI_ADDRESS_NONE)
        {
          WRITE_REG(hxspi->Instance->AR, addr_reg);
        }
        else
        {
          WRITE_REG(hxspi->Instance->IR, ir_reg);
        }
      }
    }
  }
  else
  {
    status = HAL_ERROR;
    hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_SEQUENCE;
  }

  return status;
}

/**
  * @brief  Configure the Memory Mapped mode.
  * @param  hxspi : XSPI handle
  * @param  pCfg   : Pointer to structure that contains the memory mapped configuration information.
  * @note   This function is used only in Memory mapped Mode
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_MemoryMapped(XSPI_HandleTypeDef *hxspi, const XSPI_MemoryMappedTypeDef *pCfg)
{
  HAL_StatusTypeDef status;
  uint32_t tickstart = HAL_GetTick();

  /* Check the parameters of the memory-mapped configuration structure */
  assert_param(IS_XSPI_TIMEOUT_ACTIVATION(pCfg->TimeOutActivation));
  assert_param(IS_XSPI_NO_PREFETCH_DATA(pCfg->NoPrefetchData));

  /* Check the state */
  if (hxspi->State == HAL_XSPI_STATE_CMD_CFG)
  {
    /* Wait till busy flag is reset */
    status = XSPI_WaitFlagStateUntilTimeout(hxspi, HAL_XSPI_FLAG_BUSY, RESET, tickstart, hxspi->Timeout);

    if (status == HAL_OK)
    {
      hxspi->State = HAL_XSPI_STATE_BUSY_MEM_MAPPED;

      if (pCfg->NoPrefetchData == HAL_XSPI_AUTOMATIC_PREFETCH_DISABLE)
      {
        /* Configure register */
        MODIFY_REG(hxspi->Instance->CR, XSPI_CR_NOPREF, pCfg->NoPrefetchData);
      }

      else
      {
        assert_param(IS_XSPI_NO_PREFETCH_AXI(pCfg->NoPrefetchAXI));

        /* Configure register */
        MODIFY_REG(hxspi->Instance->CR, (XSPI_CR_NOPREF | XSPI_CR_NOPREF_AXI),
                   (pCfg->NoPrefetchData | pCfg->NoPrefetchAXI));
      }
      if (pCfg->TimeOutActivation == HAL_XSPI_TIMEOUT_COUNTER_ENABLE)
      {
        assert_param(IS_XSPI_TIMEOUT_PERIOD(pCfg->TimeoutPeriodClock));

        /* Configure register */
        WRITE_REG(hxspi->Instance->LPTR, pCfg->TimeoutPeriodClock);

        /* Clear flags related to interrupt */
        HAL_XSPI_CLEAR_FLAG(hxspi, HAL_XSPI_FLAG_TO);

        /* Enable the timeout interrupt */
        HAL_XSPI_ENABLE_IT(hxspi, HAL_XSPI_IT_TO);
      }

      /* Configure CR register with functional mode as memory-mapped */
      MODIFY_REG(hxspi->Instance->CR, (XSPI_CR_TCEN | XSPI_CR_FMODE),
                 (pCfg->TimeOutActivation | XSPI_FUNCTIONAL_MODE_MEMORY_MAPPED));
    }
  }
  else
  {
    status = HAL_ERROR;
    hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_SEQUENCE;
  }

  return status;
}

/**
  * @brief  Transfer Error callback.
  * @param  hxspi : XSPI handle
  * @retval None
  */
__weak void HAL_XSPI_ErrorCallback(XSPI_HandleTypeDef *hxspi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hxspi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_XSPI_ErrorCallback could be implemented in the user file
   */
}

/**
  * @brief  Abort completed callback.
  * @param  hxspi : XSPI handle
  * @retval None
  */
__weak void HAL_XSPI_AbortCpltCallback(XSPI_HandleTypeDef *hxspi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hxspi);

  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_XSPI_AbortCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  FIFO Threshold callback.
  * @param  hxspi : XSPI handle
  * @retval None
  */
__weak void HAL_XSPI_FifoThresholdCallback(XSPI_HandleTypeDef *hxspi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hxspi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_XSPI_FIFOThresholdCallback could be implemented in the user file
   */
}

/**
  * @brief  Command completed callback.
  * @param  hxspi : XSPI handle
  * @retval None
  */
__weak void HAL_XSPI_CmdCpltCallback(XSPI_HandleTypeDef *hxspi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hxspi);

  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_XSPI_CmdCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Rx Transfer completed callback.
  * @param  hxspi : XSPI handle
  * @retval None
  */
__weak void HAL_XSPI_RxCpltCallback(XSPI_HandleTypeDef *hxspi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hxspi);

  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_XSPI_RxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Tx Transfer completed callback.
  * @param  hxspi : XSPI handle
  * @retval None
  */
__weak void HAL_XSPI_TxCpltCallback(XSPI_HandleTypeDef *hxspi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hxspi);

  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_XSPI_TxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Rx Half Transfer completed callback.
  * @param  hxspi : XSPI handle
  * @retval None
  */
__weak void HAL_XSPI_RxHalfCpltCallback(XSPI_HandleTypeDef *hxspi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hxspi);

  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_XSPI_RxHalfCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Tx Half Transfer completed callback.
  * @param  hxspi : XSPI handle
  * @retval None
  */
__weak void HAL_XSPI_TxHalfCpltCallback(XSPI_HandleTypeDef *hxspi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hxspi);

  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_XSPI_TxHalfCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Status Match callback.
  * @param  hxspi : XSPI handle
  * @retval None
  */
__weak void HAL_XSPI_StatusMatchCallback(XSPI_HandleTypeDef *hxspi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hxspi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_XSPI_StatusMatchCallback could be implemented in the user file
   */
}

/**
  * @brief  Timeout callback.
  * @param  hxspi : XSPI handle
  * @retval None
  */
__weak void HAL_XSPI_TimeOutCallback(XSPI_HandleTypeDef *hxspi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hxspi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_XSPI_TimeOutCallback could be implemented in the user file
   */
}

#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
/**
  * @brief  Register a User XSPI Callback
  *         To be used to override the weak predefined callback
  * @param hxspi : XSPI handle
  * @param CallbackID : ID of the callback to be registered
  *        This parameter can be one of the following values:
  *          @arg @ref HAL_XSPI_ERROR_CB_ID          XSPI Error Callback ID
  *          @arg @ref HAL_XSPI_ABORT_CB_ID          XSPI Abort Callback ID
  *          @arg @ref HAL_XSPI_FIFO_THRESHOLD_CB_ID XSPI FIFO Threshold Callback ID
  *          @arg @ref HAL_XSPI_CMD_CPLT_CB_ID       XSPI Command Complete Callback ID
  *          @arg @ref HAL_XSPI_RX_CPLT_CB_ID        XSPI Rx Complete Callback ID
  *          @arg @ref HAL_XSPI_TX_CPLT_CB_ID        XSPI Tx Complete Callback ID
  *          @arg @ref HAL_XSPI_RX_HALF_CPLT_CB_ID   XSPI Rx Half Complete Callback ID
  *          @arg @ref HAL_XSPI_TX_HALF_CPLT_CB_ID   XSPI Tx Half Complete Callback ID
  *          @arg @ref HAL_XSPI_STATUS_MATCH_CB_ID   XSPI Status Match Callback ID
  *          @arg @ref HAL_XSPI_TIMEOUT_CB_ID        XSPI Timeout Callback ID
  *          @arg @ref HAL_XSPI_MSP_INIT_CB_ID       XSPI MspInit callback ID
  *          @arg @ref HAL_XSPI_MSP_DEINIT_CB_ID     XSPI MspDeInit callback ID
  * @param pCallback : pointer to the Callback function
  * @retval status
  */
HAL_StatusTypeDef HAL_XSPI_RegisterCallback(XSPI_HandleTypeDef *hxspi, HAL_XSPI_CallbackIDTypeDef CallbackID,
                                            pXSPI_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hxspi->ErrorCode |= HAL_XSPI_ERROR_INVALID_CALLBACK;
    return HAL_ERROR;
  }

  if (hxspi->State == HAL_XSPI_STATE_READY)
  {
    switch (CallbackID)
    {
      case  HAL_XSPI_ERROR_CB_ID :
        hxspi->ErrorCallback = pCallback;
        break;
      case HAL_XSPI_ABORT_CB_ID :
        hxspi->AbortCpltCallback = pCallback;
        break;
      case HAL_XSPI_FIFO_THRESHOLD_CB_ID :
        hxspi->FifoThresholdCallback = pCallback;
        break;
      case HAL_XSPI_CMD_CPLT_CB_ID :
        hxspi->CmdCpltCallback = pCallback;
        break;
      case HAL_XSPI_RX_CPLT_CB_ID :
        hxspi->RxCpltCallback = pCallback;
        break;
      case HAL_XSPI_TX_CPLT_CB_ID :
        hxspi->TxCpltCallback = pCallback;
        break;
      case HAL_XSPI_RX_HALF_CPLT_CB_ID :
        hxspi->RxHalfCpltCallback = pCallback;
        break;
      case HAL_XSPI_TX_HALF_CPLT_CB_ID :
        hxspi->TxHalfCpltCallback = pCallback;
        break;
      case HAL_XSPI_STATUS_MATCH_CB_ID :
        hxspi->StatusMatchCallback = pCallback;
        break;
      case HAL_XSPI_TIMEOUT_CB_ID :
        hxspi->TimeOutCallback = pCallback;
        break;
      case HAL_XSPI_MSP_INIT_CB_ID :
        hxspi->MspInitCallback = pCallback;
        break;
      case HAL_XSPI_MSP_DEINIT_CB_ID :
        hxspi->MspDeInitCallback = pCallback;
        break;
      default :
        /* Update the error code */
        hxspi->ErrorCode |= HAL_XSPI_ERROR_INVALID_CALLBACK;
        /* update return status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (hxspi->State == HAL_XSPI_STATE_RESET)
  {
    switch (CallbackID)
    {
      case HAL_XSPI_MSP_INIT_CB_ID :
        hxspi->MspInitCallback = pCallback;
        break;
      case HAL_XSPI_MSP_DEINIT_CB_ID :
        hxspi->MspDeInitCallback = pCallback;
        break;
      default :
        /* Update the error code */
        hxspi->ErrorCode |= HAL_XSPI_ERROR_INVALID_CALLBACK;
        /* update return status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hxspi->ErrorCode |= HAL_XSPI_ERROR_INVALID_CALLBACK;
    /* update return status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Unregister a User XSPI Callback
  *         XSPI Callback is redirected to the weak predefined callback
  * @param hxspi : XSPI handle
  * @param CallbackID : ID of the callback to be unregistered
  *        This parameter can be one of the following values:
  *          @arg @ref HAL_XSPI_ERROR_CB_ID          XSPI Error Callback ID
  *          @arg @ref HAL_XSPI_ABORT_CB_ID          XSPI Abort Callback ID
  *          @arg @ref HAL_XSPI_FIFO_THRESHOLD_CB_ID XSPI FIFO Threshold Callback ID
  *          @arg @ref HAL_XSPI_CMD_CPLT_CB_ID       XSPI Command Complete Callback ID
  *          @arg @ref HAL_XSPI_RX_CPLT_CB_ID        XSPI Rx Complete Callback ID
  *          @arg @ref HAL_XSPI_TX_CPLT_CB_ID        XSPI Tx Complete Callback ID
  *          @arg @ref HAL_XSPI_RX_HALF_CPLT_CB_ID   XSPI Rx Half Complete Callback ID
  *          @arg @ref HAL_XSPI_TX_HALF_CPLT_CB_ID   XSPI Tx Half Complete Callback ID
  *          @arg @ref HAL_XSPI_STATUS_MATCH_CB_ID   XSPI Status Match Callback ID
  *          @arg @ref HAL_XSPI_TIMEOUT_CB_ID        XSPI Timeout Callback ID
  *          @arg @ref HAL_XSPI_MSP_INIT_CB_ID       XSPI MspInit callback ID
  *          @arg @ref HAL_XSPI_MSP_DEINIT_CB_ID     XSPI MspDeInit callback ID
  * @retval status
  */
HAL_StatusTypeDef HAL_XSPI_UnRegisterCallback(XSPI_HandleTypeDef *hxspi, HAL_XSPI_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hxspi->State == HAL_XSPI_STATE_READY)
  {
    switch (CallbackID)
    {
      case  HAL_XSPI_ERROR_CB_ID :
        hxspi->ErrorCallback = HAL_XSPI_ErrorCallback;
        break;
      case HAL_XSPI_ABORT_CB_ID :
        hxspi->AbortCpltCallback = HAL_XSPI_AbortCpltCallback;
        break;
      case HAL_XSPI_FIFO_THRESHOLD_CB_ID :
        hxspi->FifoThresholdCallback = HAL_XSPI_FifoThresholdCallback;
        break;
      case HAL_XSPI_CMD_CPLT_CB_ID :
        hxspi->CmdCpltCallback = HAL_XSPI_CmdCpltCallback;
        break;
      case HAL_XSPI_RX_CPLT_CB_ID :
        hxspi->RxCpltCallback = HAL_XSPI_RxCpltCallback;
        break;
      case HAL_XSPI_TX_CPLT_CB_ID :
        hxspi->TxCpltCallback = HAL_XSPI_TxCpltCallback;
        break;
      case HAL_XSPI_RX_HALF_CPLT_CB_ID :
        hxspi->RxHalfCpltCallback = HAL_XSPI_RxHalfCpltCallback;
        break;
      case HAL_XSPI_TX_HALF_CPLT_CB_ID :
        hxspi->TxHalfCpltCallback = HAL_XSPI_TxHalfCpltCallback;
        break;
      case HAL_XSPI_STATUS_MATCH_CB_ID :
        hxspi->StatusMatchCallback = HAL_XSPI_StatusMatchCallback;
        break;
      case HAL_XSPI_TIMEOUT_CB_ID :
        hxspi->TimeOutCallback = HAL_XSPI_TimeOutCallback;
        break;
      case HAL_XSPI_MSP_INIT_CB_ID :
        hxspi->MspInitCallback = HAL_XSPI_MspInit;
        break;
      case HAL_XSPI_MSP_DEINIT_CB_ID :
        hxspi->MspDeInitCallback = HAL_XSPI_MspDeInit;
        break;
      default :
        /* Update the error code */
        hxspi->ErrorCode |= HAL_XSPI_ERROR_INVALID_CALLBACK;
        /* update return status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (hxspi->State == HAL_XSPI_STATE_RESET)
  {
    switch (CallbackID)
    {
      case HAL_XSPI_MSP_INIT_CB_ID :
        hxspi->MspInitCallback = HAL_XSPI_MspInit;
        break;
      case HAL_XSPI_MSP_DEINIT_CB_ID :
        hxspi->MspDeInitCallback = HAL_XSPI_MspDeInit;
        break;
      default :
        /* Update the error code */
        hxspi->ErrorCode |= HAL_XSPI_ERROR_INVALID_CALLBACK;
        /* update return status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hxspi->ErrorCode |= HAL_XSPI_ERROR_INVALID_CALLBACK;
    /* update return status */
    status =  HAL_ERROR;
  }

  return status;
}
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */

/**
  * @}
  */

/** @defgroup XSPI_Exported_Functions_Group3 Peripheral Control and State functions
  *  @brief   XSPI control and State functions
  *
@verbatim
 ===============================================================================
                  ##### Peripheral Control and State functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to :
      (+) Check in run-time the state of the driver.
      (+) Check the error code set during last operation.
      (+) Abort any operation.
      (+) Manage the Fifo threshold.
      (+) Configure the timeout duration used in the driver.

@endverbatim
  * @{
  */

/**
  * @brief  Abort the current operation, return to the indirect mode.
  * @param  hxspi : XSPI handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_Abort(XSPI_HandleTypeDef *hxspi)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t tickstart = HAL_GetTick();

  /* Check if the state is not in reset state */
  if (hxspi->State != HAL_XSPI_STATE_RESET)
  {
    /* Check if the DMA is enabled */
    if ((hxspi->Instance->CR & XSPI_CR_DMAEN) != 0U)
    {
      /* Disable the DMA transfer on the XSPI side */
      CLEAR_BIT(hxspi->Instance->CR, XSPI_CR_DMAEN);

      /* Disable the DMA transmit on the DMA side */
      status = HAL_DMA_Abort(hxspi->hdmatx);
      if (status != HAL_OK)
      {
        hxspi->ErrorCode = HAL_XSPI_ERROR_DMA;
      }

      /* Disable the DMA receive on the DMA side */
      status = HAL_DMA_Abort(hxspi->hdmarx);
      if (status != HAL_OK)
      {
        hxspi->ErrorCode = HAL_XSPI_ERROR_DMA;
      }
    }

    if (HAL_XSPI_GET_FLAG(hxspi, HAL_XSPI_FLAG_BUSY) != RESET)
    {
      /* Perform an abort of the XSPI */
      SET_BIT(hxspi->Instance->CR, XSPI_CR_ABORT);

      /* Wait until the transfer complete flag is set to go back in idle state */
      status = XSPI_WaitFlagStateUntilTimeout(hxspi, HAL_XSPI_FLAG_TC, SET, tickstart, hxspi->Timeout);

      if (status == HAL_OK)
      {
        /* Clear transfer complete flag */
        HAL_XSPI_CLEAR_FLAG(hxspi, HAL_XSPI_FLAG_TC);

        /* Wait until the busy flag is reset to go back in idle state */
        status = XSPI_WaitFlagStateUntilTimeout(hxspi, HAL_XSPI_FLAG_BUSY, RESET, tickstart, hxspi->Timeout);

        if (status == HAL_OK)
        {
          /* Return to indirect mode */
          CLEAR_BIT(hxspi->Instance->CR, XSPI_CR_FMODE);

          hxspi->State = HAL_XSPI_STATE_READY;
        }
      }
    }
    else
    {
      /* Return to indirect mode */
      CLEAR_BIT(hxspi->Instance->CR, XSPI_CR_FMODE);

      hxspi->State = HAL_XSPI_STATE_READY;
    }
  }
  else
  {
    status = HAL_ERROR;
    hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_SEQUENCE;
  }

  return status;
}

/**
  * @brief  Abort the current operation, return to the indirect mode. (non-blocking function)
  * @param  hxspi : XSPI handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_Abort_IT(XSPI_HandleTypeDef *hxspi)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check if the state is not in reset state */
  if (hxspi->State != HAL_XSPI_STATE_RESET)
  {
    /* Disable all interrupts */
    HAL_XSPI_DISABLE_IT(hxspi, (HAL_XSPI_IT_TO | HAL_XSPI_IT_SM | HAL_XSPI_IT_FT | HAL_XSPI_IT_TC | HAL_XSPI_IT_TE));

    hxspi->State = HAL_XSPI_STATE_ABORT;

    /* Check if the DMA is enabled */
    if ((hxspi->Instance->CR & XSPI_CR_DMAEN) != 0U)
    {
      /* Disable the DMA transfer on the XSPI side */
      CLEAR_BIT(hxspi->Instance->CR, XSPI_CR_DMAEN);

      /* Disable the DMA transmit on the DMA side */
      hxspi->hdmatx->XferAbortCallback = XSPI_DMAAbortCplt;
      if (HAL_DMA_Abort_IT(hxspi->hdmatx) != HAL_OK)
      {
        hxspi->State = HAL_XSPI_STATE_READY;

        /* Abort callback */
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
        hxspi->AbortCpltCallback(hxspi);
#else
        HAL_XSPI_AbortCpltCallback(hxspi);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
      }

      /* Disable the DMA receive on the DMA side */
      hxspi->hdmarx->XferAbortCallback = XSPI_DMAAbortCplt;
      if (HAL_DMA_Abort_IT(hxspi->hdmarx) != HAL_OK)
      {
        hxspi->State = HAL_XSPI_STATE_READY;

        /* Abort callback */
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
        hxspi->AbortCpltCallback(hxspi);
#else
        HAL_XSPI_AbortCpltCallback(hxspi);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
      }
    }
    else
    {
      if (HAL_XSPI_GET_FLAG(hxspi, HAL_XSPI_FLAG_BUSY) != RESET)
      {
        /* Clear transfer complete flag */
        HAL_XSPI_CLEAR_FLAG(hxspi, HAL_XSPI_FLAG_TC);

        /* Enable the transfer complete interrupts */
        HAL_XSPI_ENABLE_IT(hxspi, HAL_XSPI_IT_TC);

        /* Perform an abort of the XSPI */
        SET_BIT(hxspi->Instance->CR, XSPI_CR_ABORT);

        /* Return to indirect mode */
        CLEAR_BIT(hxspi->Instance->CR, XSPI_CR_FMODE);
      }
      else
      {
        /* Return to indirect mode */
        CLEAR_BIT(hxspi->Instance->CR, XSPI_CR_FMODE);

        hxspi->State = HAL_XSPI_STATE_READY;

        /* Abort callback */
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
        hxspi->AbortCpltCallback(hxspi);
#else
        HAL_XSPI_AbortCpltCallback(hxspi);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
      }
    }
  }
  else
  {
    status = HAL_ERROR;
    hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_SEQUENCE;
  }

  return status;
}

/** @brief  Set XSPI Fifo threshold.
  * @param  hxspi     : XSPI handle.
  * @param  Threshold : Threshold of the Fifo.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_SetFifoThreshold(XSPI_HandleTypeDef *hxspi, uint32_t Threshold)
{
  HAL_StatusTypeDef status = HAL_OK;

  assert_param(IS_XSPI_FIFO_THRESHOLD_BYTE(Threshold));

  /* Check the state */
  if ((hxspi->State & XSPI_BUSY_STATE_MASK) == 0U)
  {
    /* Synchronize initialization structure with the new fifo threshold value */
    hxspi->Init.FifoThresholdByte = Threshold;

    /* Configure new fifo threshold */
    MODIFY_REG(hxspi->Instance->CR, XSPI_CR_FTHRES, ((hxspi->Init.FifoThresholdByte - 1U) << XSPI_CR_FTHRES_Pos));

  }
  else
  {
    status = HAL_ERROR;
    hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_SEQUENCE;
  }

  return status;
}

/** @brief  Get XSPI Fifo threshold.
  * @param  hxspi : XSPI handle.
  * @retval Fifo threshold
  */
uint32_t HAL_XSPI_GetFifoThreshold(const XSPI_HandleTypeDef *hxspi)
{
  return ((READ_BIT(hxspi->Instance->CR, XSPI_CR_FTHRES) >> XSPI_CR_FTHRES_Pos) + 1U);
}

/** @brief  Set XSPI Memory Type.
  * @param  hxspi     : XSPI handle.
  * @param  Type : Memory Type.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_SetMemoryType(XSPI_HandleTypeDef *hxspi, uint32_t Type)
{
  HAL_StatusTypeDef status = HAL_OK;

  assert_param(IS_XSPI_MEMORY_TYPE(Type));

  /* Check the state */
  if ((hxspi->State & XSPI_BUSY_STATE_MASK) == 0U)
  {
    /* Synchronize initialization structure with the new memory type value */
    hxspi->Init.MemoryType = Type;

    /* Configure new memory type */
    MODIFY_REG(hxspi->Instance->DCR1, XSPI_DCR1_MTYP, hxspi->Init.MemoryType);
  }
  else
  {
    status = HAL_ERROR;
    hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_SEQUENCE;
  }

  return status;
}

/** @brief  Set XSPI Device Size.
  * @param  hxspi     : XSPI handle.
  * @param  Size : Device Size.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_SetDeviceSize(XSPI_HandleTypeDef *hxspi, uint32_t Size)
{
  HAL_StatusTypeDef status = HAL_OK;

  assert_param(IS_XSPI_MEMORY_SIZE(Size));

  /* Check the state */
  if ((hxspi->State & XSPI_BUSY_STATE_MASK) == 0U)
  {
    /* Synchronize initialization structure with the new device size value */
    hxspi->Init.MemorySize = Size;

    /* Configure new device size */
    MODIFY_REG(hxspi->Instance->DCR1, XSPI_DCR1_DEVSIZE,
               (hxspi->Init.MemorySize << XSPI_DCR1_DEVSIZE_Pos));
  }
  else
  {
    status = HAL_ERROR;
    hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_SEQUENCE;
  }

  return status;
}

/** @brief  Set XSPI Clock prescaler.
  * @param  hxspi     : XSPI handle.
  * @param  Prescaler : Clock prescaler.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_SetClockPrescaler(XSPI_HandleTypeDef *hxspi, uint32_t Prescaler)
{
  HAL_StatusTypeDef status = HAL_OK;
  assert_param(IS_XSPI_CLK_PRESCALER(Prescaler));

  /* Check the state */
  if ((hxspi->State & XSPI_BUSY_STATE_MASK) == 0U)
  {
    /* Synchronize initialization structure with the new clock prescaler value */
    hxspi->Init.ClockPrescaler = Prescaler;

    /* Configure clock prescaler */
    MODIFY_REG(hxspi->Instance->DCR2, XSPI_DCR2_PRESCALER,
               ((hxspi->Init.ClockPrescaler) << XSPI_DCR2_PRESCALER_Pos));
  }
  else
  {
    status = HAL_ERROR;
    hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_SEQUENCE;
  }

  return status;
}

/** @brief Set XSPI timeout.
  * @param  hxspi   : XSPI handle.
  * @param  Timeout : Timeout for the memory access.
  * @retval HAL state
  */
HAL_StatusTypeDef HAL_XSPI_SetTimeout(XSPI_HandleTypeDef *hxspi, uint32_t Timeout)
{
  hxspi->Timeout = Timeout;
  return HAL_OK;
}

/**
  * @brief  Return the XSPI error code.
  * @param  hxspi : XSPI handle
  * @retval XSPI Error Code
  */
uint32_t HAL_XSPI_GetError(const XSPI_HandleTypeDef *hxspi)
{
  return hxspi->ErrorCode;
}

/**
  * @brief  Return the XSPI handle state.
  * @param  hxspi : XSPI handle
  * @retval HAL state
  */
uint32_t HAL_XSPI_GetState(const XSPI_HandleTypeDef *hxspi)
{
  /* Return XSPI handle state */
  return hxspi->State;
}

/**
  * @}
  */

/** @defgroup XSPI_Exported_Functions_Group4 IO Manager configuration function
  *  @brief   XSPI IO Manager configuration function
  *
@verbatim
 ===============================================================================
                  ##### IO Manager configuration function #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to :
      (+) Configure the IO manager.

@endverbatim
  * @{
  */

/**
  * @brief  Configure the XSPI IO manager.
  * @param  hxspi   : XSPI handle
  * @param  pCfg     : Pointer to Configuration of the IO Manager for the instance
  * @param  Timeout : Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPIM_Config(XSPI_HandleTypeDef *hxspi, const XSPIM_CfgTypeDef *pCfg, uint32_t Timeout)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint8_t index;
  uint8_t xspi_enabled = 0U;

  XSPIM_CfgTypeDef IOM_cfg[XSPI_NB_INSTANCE] = {0};

  /* Prevent unused argument(s) compilation warning */
  UNUSED(Timeout);

  /* Check the parameters of the XSPI IO Manager configuration structure */
  assert_param(IS_XSPIM_NCS_OVR(pCfg->nCSOverride));
  assert_param(IS_XSPIM_IO_PORT(pCfg->IOPort));
  assert_param(IS_XSPIM_REQ2ACKTIME(pCfg->Req2AckTime));

  /**************** Get current configuration of the instances ****************/
  for (index = 0U; index < XSPI_NB_INSTANCE; index++)
  {
    XSPIM_GetConfig(index + 1U, &(IOM_cfg[index]));
  }

  /********** Disable all XSPI to configure XSPI IO Manager **********/
  if (__HAL_RCC_XSPI1_IS_CLK_ENABLED() != 0U)
  {
    if ((XSPI1->CR & XSPI_CR_EN) != 0U)
    {
      CLEAR_BIT(XSPI1->CR, XSPI_CR_EN);
      xspi_enabled |= 0x1U;
    }
  }
  if (__HAL_RCC_XSPI2_IS_CLK_ENABLED() != 0U)
  {
    if ((XSPI2->CR & XSPI_CR_EN) != 0U)
    {
      CLEAR_BIT(XSPI2->CR, XSPI_CR_EN);
      xspi_enabled |= 0x2U;
    }
  }
  if (__HAL_RCC_XSPI3_IS_CLK_ENABLED() != 0U)
  {
    if ((XSPI3->CR & XSPI_CR_EN) != 0U)
    {
      CLEAR_BIT(XSPI2->CR, XSPI_CR_EN);
      xspi_enabled |= 0x4U;
    }
  }

  /***************** Deactivation of previous configuration *****************/
  CLEAR_REG(XSPIM->CR);

  /******************** Activation of new configuration *********************/
  MODIFY_REG(XSPIM->CR, XSPIM_CR_REQ2ACK_TIME, ((pCfg->Req2AckTime - 1U) << XSPIM_CR_REQ2ACK_TIME_Pos));

  if (hxspi->Instance == XSPI1)
  {
    IOM_cfg[0].IOPort = pCfg->IOPort ;
    if (pCfg->nCSOverride != HAL_XSPI_CSSEL_OVR_DISABLED)
    {
      MODIFY_REG(XSPIM->CR, (XSPIM_CR_CSSEL_OVR_O1 | XSPIM_CR_CSSEL_OVR_EN), (pCfg->nCSOverride));
    }
    else
    {
      /* Nothing to do */
    }
  }
  else if (hxspi->Instance == XSPI2)
  {
    IOM_cfg[1].IOPort = pCfg->IOPort ;
    if (pCfg->nCSOverride != HAL_XSPI_CSSEL_OVR_DISABLED)
    {
      MODIFY_REG(XSPIM->CR, (XSPIM_CR_CSSEL_OVR_O2 | XSPIM_CR_CSSEL_OVR_EN), (pCfg->nCSOverride));
    }
    else
    {
      /* Nothing to do */
    }
  }
  else if (hxspi->Instance == XSPI3)
  {
    if (pCfg->IOPort == HAL_XSPIM_IOPORT_1)
    {
      IOM_cfg[0].IOPort = HAL_XSPIM_IOPORT_2 ;
      IOM_cfg[1].IOPort = HAL_XSPIM_IOPORT_2 ;
    }
    else if (pCfg->IOPort == HAL_XSPIM_IOPORT_2)
    {
      IOM_cfg[0].IOPort = HAL_XSPIM_IOPORT_1 ;
      IOM_cfg[1].IOPort = HAL_XSPIM_IOPORT_1 ;
    }
    else
    {
      /* Nothing to do */
    }
  }
  else
  {
    hxspi->ErrorCode |= HAL_XSPI_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  for (index = 0U; index < (XSPI_NB_INSTANCE - 2U); index++)
  {
    if (IOM_cfg[index].IOPort == IOM_cfg[index + 1U].IOPort)
    {
      /*Mux*/
      SET_BIT(XSPIM->CR, XSPIM_CR_MUXEN);
    }
    else
    {
      /* Nothing to do */
    }
    if (IOM_cfg[0].IOPort == HAL_XSPIM_IOPORT_2)
    {
      /*Mode*/
      SET_BIT(XSPIM->CR, XSPIM_CR_MODE);
    }
    else
    {
      /* Nothing to do */
    }
  }

  /******* Re-enable both XSPI after configure XSPI IO Manager ********/
  if ((xspi_enabled & 0x1U) != 0U)
  {
    SET_BIT(XSPI1->CR, XSPI_CR_EN);
  }
  if ((xspi_enabled & 0x2U) != 0U)
  {
    SET_BIT(XSPI2->CR, XSPI_CR_EN);
  }

  return status;
}

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup XSPI_Exported_Functions_Group6 High-speed interface and calibration functions
  *  @brief   XSPI high-speed interface and calibration functions
  *
@verbatim
 ===============================================================================
            ##### High-speed interface and calibration functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to :
      (+) Get the delay values of the high-speed interface DLLs.
    (+) Set a delay value for the high-speed interface DLLs.

@endverbatim
  * @{
  */

/**
  * @brief  Get the delay values of the high-speed interface DLLs.
  * @param  hxspi : XSPI handle
  * @param  pCfg   : Current delay values corresponding to the DelayValueType field.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_GetDelayValue(XSPI_HandleTypeDef *hxspi, XSPI_HSCalTypeDef *pCfg)
{
  HAL_StatusTypeDef status = HAL_OK;
  __IO uint32_t reg = 0;

  if (IS_XSPI_ALL_INSTANCE(hxspi->Instance))
  {
    /* Check the parameter specified in the structure */
    assert_param(IS_XSPI_DELAY_TYPE(pCfg->DelayValueType));

    switch (pCfg->DelayValueType)
    {
      case HAL_XSPI_CAL_FULL_CYCLE_DELAY:
        reg = hxspi->Instance->CALFCR;
        pCfg->MaxCalibration = (reg & XSPI_CALFCR_CALMAX);
        break;
      case HAL_XSPI_CAL_FEEDBACK_CLK_DELAY:
        reg = hxspi->Instance->CALMR;
        break;
      case HAL_XSPI_CAL_DATA_OUTPUT_DELAY:
        reg = hxspi->Instance->CALSOR;
        break;
      case HAL_XSPI_CAL_DQS_INPUT_DELAY:
        reg = hxspi->Instance->CALSIR;
        break;
      default:
        status = HAL_ERROR;
        hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_PARAM;
        break;
    }

    if (status == HAL_OK)
    {
      pCfg->FineCalibrationUnit = (reg & XSPI_CALFCR_FINE);
      pCfg->CoarseCalibrationUnit = ((reg & XSPI_CALFCR_COARSE) >> XSPI_CALFCR_COARSE_Pos);
    }
  }
  else
  {
    status = HAL_ERROR;
    hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_PARAM;
  }

  return status;
}

/**
  * @brief  Set a delay value for the high-speed interface DLLs.
  * @param  hxspi : XSPI handle
  * @param  pCfg   : Configuration of delay value specified in DelayValueType field.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_XSPI_SetDelayValue(XSPI_HandleTypeDef *hxspi, const XSPI_HSCalTypeDef *pCfg)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (IS_XSPI_ALL_INSTANCE(hxspi->Instance))
  {
    /* Check the parameter specified in the structure */
    assert_param(IS_XSPI_DELAY_TYPE(pCfg->DelayValueType));
    assert_param(IS_XSPI_FINECAL_VALUE(pCfg->FineCalibrationUnit));
    assert_param(IS_XSPI_COARSECAL_VALUE(pCfg->CoarseCalibrationUnit));

    /* Check if the state isn't in one of the busy states */
    if ((hxspi->State & XSPI_BUSY_STATE_MASK) == 0U)
    {
      switch (pCfg->DelayValueType)
      {
        case HAL_XSPI_CAL_FEEDBACK_CLK_DELAY:
          MODIFY_REG(hxspi->Instance->CALMR, (XSPI_CALMR_COARSE |  XSPI_CALMR_FINE),
                     (pCfg->FineCalibrationUnit | (pCfg->CoarseCalibrationUnit << XSPI_CALMR_COARSE_Pos)));
          break;
        case HAL_XSPI_CAL_DATA_OUTPUT_DELAY:
          MODIFY_REG(hxspi->Instance->CALSOR, (XSPI_CALSOR_COARSE | XSPI_CALSOR_FINE),
                     (pCfg->FineCalibrationUnit | (pCfg->CoarseCalibrationUnit << XSPI_CALSOR_COARSE_Pos)));
          break;
        case HAL_XSPI_CAL_DQS_INPUT_DELAY:
          MODIFY_REG(hxspi->Instance->CALSIR, (XSPI_CALSIR_COARSE | XSPI_CALSIR_FINE),
                     (pCfg->FineCalibrationUnit | (pCfg->CoarseCalibrationUnit << XSPI_CALSIR_COARSE_Pos)));
          break;
        default:
          status = HAL_ERROR;
          hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_PARAM;
          break;
      }
    }
    else
    {
      status = HAL_ERROR;
      hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_SEQUENCE;
    }
  }
  else
  {
    status = HAL_ERROR;
    hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_PARAM;
  }

  return status;
}

/**
  * @}
  */

/**
  @cond 0
  */
/**
  * @brief  DMA XSPI process complete callback.
  * @param  hdma : DMA handle
  * @retval None
  */
static void XSPI_DMACplt(DMA_HandleTypeDef *hdma)
{
  XSPI_HandleTypeDef *hxspi = (XSPI_HandleTypeDef *)(hdma->Parent);
  hxspi->XferCount = 0;

  /* Disable the DMA transfer on the XSPI side */
  CLEAR_BIT(hxspi->Instance->CR, XSPI_CR_DMAEN);

  /* Enable the XSPI transfer complete Interrupt */
  HAL_XSPI_ENABLE_IT(hxspi, HAL_XSPI_IT_TC);
}

/**
  * @brief  DMA XSPI process half complete callback.
  * @param  hdma : DMA handle
  * @retval None
  */
static void XSPI_DMAHalfCplt(DMA_HandleTypeDef *hdma)
{
  XSPI_HandleTypeDef *hxspi = (XSPI_HandleTypeDef *)(hdma->Parent);
  hxspi->XferCount = (hxspi->XferCount >> 1);

  if (hxspi->State == HAL_XSPI_STATE_BUSY_RX)
  {
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
    hxspi->RxHalfCpltCallback(hxspi);
#else
    HAL_XSPI_RxHalfCpltCallback(hxspi);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
  }
  else
  {
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
    hxspi->TxHalfCpltCallback(hxspi);
#else
    HAL_XSPI_TxHalfCpltCallback(hxspi);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
  }
}

/**
  * @brief  DMA XSPI communication error callback.
  * @param  hdma : DMA handle
  * @retval None
  */
static void XSPI_DMAError(DMA_HandleTypeDef *hdma)
{
  XSPI_HandleTypeDef *hxspi = (XSPI_HandleTypeDef *)(hdma->Parent);
  hxspi->XferCount = 0;
  hxspi->ErrorCode = HAL_XSPI_ERROR_DMA;

  /* Disable the DMA transfer on the XSPI side */
  CLEAR_BIT(hxspi->Instance->CR, XSPI_CR_DMAEN);

  /* Abort the XSPI */
  if (HAL_XSPI_Abort_IT(hxspi) != HAL_OK)
  {
    /* Disable the interrupts */
    HAL_XSPI_DISABLE_IT(hxspi, HAL_XSPI_IT_TC | HAL_XSPI_IT_FT | HAL_XSPI_IT_TE);

    hxspi->State = HAL_XSPI_STATE_READY;

    /* Error callback */
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
    hxspi->ErrorCallback(hxspi);
#else
    HAL_XSPI_ErrorCallback(hxspi);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
  }
}

/**
  * @brief  DMA XSPI abort complete callback.
  * @param  hdma : DMA handle
  * @retval None
  */
static void XSPI_DMAAbortCplt(DMA_HandleTypeDef *hdma)
{
  XSPI_HandleTypeDef *hxspi = (XSPI_HandleTypeDef *)(hdma->Parent);
  hxspi->XferCount = 0;

  /* Check the state */
  if (hxspi->State == HAL_XSPI_STATE_ABORT)
  {
    /* DMA abort called by XSPI abort */
    if (HAL_XSPI_GET_FLAG(hxspi, HAL_XSPI_FLAG_BUSY) != RESET)
    {
      /* Clear transfer complete flag */
      HAL_XSPI_CLEAR_FLAG(hxspi, HAL_XSPI_FLAG_TC);

      /* Enable the transfer complete interrupts */
      HAL_XSPI_ENABLE_IT(hxspi, HAL_XSPI_IT_TC);

      /* Perform an abort of the XSPI */
      SET_BIT(hxspi->Instance->CR, XSPI_CR_ABORT);
    }
    else
    {
      hxspi->State = HAL_XSPI_STATE_READY;

      /* Abort callback */
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
      hxspi->AbortCpltCallback(hxspi);
#else
      HAL_XSPI_AbortCpltCallback(hxspi);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
    }
  }
  else
  {
    /* DMA abort called due to a transfer error interrupt */
    hxspi->State = HAL_XSPI_STATE_READY;

    /* Error callback */
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
    hxspi->ErrorCallback(hxspi);
#else
    HAL_XSPI_ErrorCallback(hxspi);
#endif /* defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
  }
}

/**
  * @brief  Wait for a flag state until timeout.
  * @param  hxspi     : XSPI handle
  * @param  Flag      : Flag checked
  * @param  State     : Value of the flag expected
  * @param  Timeout   : Duration of the timeout
  * @param  Tickstart : Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef XSPI_WaitFlagStateUntilTimeout(XSPI_HandleTypeDef *hxspi, uint32_t Flag,
                                                        FlagStatus State, uint32_t Tickstart, uint32_t Timeout)
{
  /* Wait until flag is in expected state */
  while ((HAL_XSPI_GET_FLAG(hxspi, Flag)) != State)
  {
    /* Check for the Timeout */
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0U))
      {
        hxspi->State     = HAL_XSPI_STATE_READY;
        hxspi->ErrorCode |= HAL_XSPI_ERROR_TIMEOUT;

        return HAL_TIMEOUT;
      }
    }
  }
  return HAL_OK;
}

/**
  * @brief  Configure the registers for the regular command mode.
  * @param  hxspi : XSPI handle
  * @param  pCmd   : structure that contains the command configuration information
  * @retval HAL status
  */
static HAL_StatusTypeDef XSPI_ConfigCmd(XSPI_HandleTypeDef *hxspi, const XSPI_RegularCmdTypeDef *pCmd)
{
  HAL_StatusTypeDef status = HAL_OK;
  __IO uint32_t *ccr_reg;
  __IO uint32_t *tcr_reg;
  __IO uint32_t *ir_reg;
  __IO uint32_t *abr_reg;

  /* Re-initialize the value of the functional mode */
  MODIFY_REG(hxspi->Instance->CR, XSPI_CR_FMODE, 0U);

  if (hxspi->Init.MemoryMode == HAL_XSPI_SINGLE_MEM)
  {
    assert_param(IS_XSPI_IO_SELECT(pCmd->IOSelect));
    MODIFY_REG(hxspi->Instance->CR, XSPI_CR_MSEL, pCmd->IOSelect);
  }

  if (pCmd->OperationType == HAL_XSPI_OPTYPE_WRITE_CFG)
  {
    ccr_reg = &(hxspi->Instance->WCCR);
    tcr_reg = &(hxspi->Instance->WTCR);
    ir_reg  = &(hxspi->Instance->WIR);
    abr_reg = &(hxspi->Instance->WABR);
  }
  else if (pCmd->OperationType == HAL_XSPI_OPTYPE_WRAP_CFG)
  {
    ccr_reg = &(hxspi->Instance->WPCCR);
    tcr_reg = &(hxspi->Instance->WPTCR);
    ir_reg  = &(hxspi->Instance->WPIR);
    abr_reg = &(hxspi->Instance->WPABR);
  }
  else
  {
    ccr_reg = &(hxspi->Instance->CCR);
    tcr_reg = &(hxspi->Instance->TCR);
    ir_reg  = &(hxspi->Instance->IR);
    abr_reg = &(hxspi->Instance->ABR);
  }

  /* Configure the CCR register with DQS and SIOO modes */
  *ccr_reg = pCmd->DQSMode;

  if (pCmd->AlternateBytesMode != HAL_XSPI_ALT_BYTES_NONE)
  {
    /* Configure the ABR register with alternate bytes value */
    *abr_reg = pCmd->AlternateBytes;

    /* Configure the CCR register with alternate bytes communication parameters */
    MODIFY_REG((*ccr_reg), (XSPI_CCR_ABMODE | XSPI_CCR_ABDTR | XSPI_CCR_ABSIZE),
               (pCmd->AlternateBytesMode | pCmd->AlternateBytesDTRMode | pCmd->AlternateBytesWidth));
  }

  /* Configure the TCR register with the number of dummy cycles */
  MODIFY_REG((*tcr_reg), XSPI_TCR_DCYC, pCmd->DummyCycles);

  if (pCmd->DataMode != HAL_XSPI_DATA_NONE)
  {
    if (pCmd->OperationType == HAL_XSPI_OPTYPE_COMMON_CFG)
    {
      /* Configure the DLR register with the number of data */
      hxspi->Instance->DLR = (pCmd->DataLength - 1U);
    }
  }

  /* Configure SSHIFT register to handle SDR/DTR data transfer */
  if (pCmd->DataMode != HAL_XSPI_DATA_NONE)
  {
    if (pCmd->DataDTRMode == HAL_XSPI_DATA_DTR_ENABLE)
    {
      /* Deactivate sample shifting when receiving data in DTR mode (DDTR=1) */
      CLEAR_BIT(hxspi->Instance->TCR, XSPI_TCR_SSHIFT);
    }
    else if (hxspi->Init.SampleShifting == HAL_XSPI_SAMPLE_SHIFT_HALFCYCLE)
    {
      /* Configure sample shifting */
      SET_BIT(hxspi->Instance->TCR, XSPI_TCR_SSHIFT);
    }
    else
    {
      /* Do nothing */
    }
  }

  if (pCmd->InstructionMode != HAL_XSPI_INSTRUCTION_NONE)
  {
    if (pCmd->AddressMode != HAL_XSPI_ADDRESS_NONE)
    {
      if (pCmd->DataMode != HAL_XSPI_DATA_NONE)
      {
        /* ---- Command with instruction, address and data ---- */

        /* Configure the CCR register with all communication parameters */
        MODIFY_REG((*ccr_reg), (XSPI_CCR_IMODE  | XSPI_CCR_IDTR  | XSPI_CCR_ISIZE  |
                                XSPI_CCR_ADMODE | XSPI_CCR_ADDTR | XSPI_CCR_ADSIZE |
                                XSPI_CCR_DMODE  | XSPI_CCR_DDTR),
                   (pCmd->InstructionMode | pCmd->InstructionDTRMode | pCmd->InstructionWidth |
                    pCmd->AddressMode     | pCmd->AddressDTRMode     | pCmd->AddressWidth     |
                    pCmd->DataMode        | pCmd->DataDTRMode));
      }
      else
      {
        /* ---- Command with instruction and address ---- */

        /* Configure the CCR register with all communication parameters */
        MODIFY_REG((*ccr_reg), (XSPI_CCR_IMODE  | XSPI_CCR_IDTR  | XSPI_CCR_ISIZE  |
                                XSPI_CCR_ADMODE | XSPI_CCR_ADDTR | XSPI_CCR_ADSIZE),
                   (pCmd->InstructionMode | pCmd->InstructionDTRMode | pCmd->InstructionWidth |
                    pCmd->AddressMode     | pCmd->AddressDTRMode     | pCmd->AddressWidth));

        /* The DHQC bit is linked with DDTR bit which should be activated */
        if ((hxspi->Init.DelayHoldQuarterCycle == HAL_XSPI_DHQC_ENABLE) &&
            (pCmd->InstructionDTRMode == HAL_XSPI_INSTRUCTION_DTR_ENABLE))
        {
          MODIFY_REG((*ccr_reg), XSPI_CCR_DDTR, HAL_XSPI_DATA_DTR_ENABLE);
        }
      }
      /* Configure the IR register with the instruction value */
      *ir_reg = pCmd->Instruction;

      /* Configure the AR register with the address value */
      hxspi->Instance->AR = pCmd->Address;
    }
    else
    {
      if (pCmd->DataMode != HAL_XSPI_DATA_NONE)
      {
        /* ---- Command with instruction and data ---- */

        /* Configure the CCR register with all communication parameters */
        MODIFY_REG((*ccr_reg), (XSPI_CCR_IMODE | XSPI_CCR_IDTR | XSPI_CCR_ISIZE |
                                XSPI_CCR_DMODE | XSPI_CCR_DDTR),
                   (pCmd->InstructionMode | pCmd->InstructionDTRMode | pCmd->InstructionWidth |
                    pCmd->DataMode        | pCmd->DataDTRMode));
      }
      else
      {
        /* ---- Command with only instruction ---- */

        /* Configure the CCR register with all communication parameters */
        MODIFY_REG((*ccr_reg), (XSPI_CCR_IMODE | XSPI_CCR_IDTR | XSPI_CCR_ISIZE),
                   (pCmd->InstructionMode | pCmd->InstructionDTRMode | pCmd->InstructionWidth));

        /* The DHQC bit is linked with DDTR bit which should be activated */
        if ((hxspi->Init.DelayHoldQuarterCycle == HAL_XSPI_DHQC_ENABLE) &&
            (pCmd->InstructionDTRMode == HAL_XSPI_INSTRUCTION_DTR_ENABLE))
        {
          MODIFY_REG((*ccr_reg), XSPI_CCR_DDTR, HAL_XSPI_DATA_DTR_ENABLE);
        }
      }

      /* Configure the IR register with the instruction value */
      *ir_reg = pCmd->Instruction;

    }
  }
  else
  {
    if (pCmd->AddressMode != HAL_XSPI_ADDRESS_NONE)
    {
      if (pCmd->DataMode != HAL_XSPI_DATA_NONE)
      {
        /* ---- Command with address and data ---- */

        /* Configure the CCR register with all communication parameters */
        MODIFY_REG((*ccr_reg), (XSPI_CCR_ADMODE | XSPI_CCR_ADDTR | XSPI_CCR_ADSIZE |
                                XSPI_CCR_DMODE  | XSPI_CCR_DDTR),
                   (pCmd->AddressMode | pCmd->AddressDTRMode | pCmd->AddressWidth     |
                    pCmd->DataMode    | pCmd->DataDTRMode));
      }
      else
      {
        /* ---- Command with only address ---- */

        /* Configure the CCR register with all communication parameters */
        MODIFY_REG((*ccr_reg), (XSPI_CCR_ADMODE | XSPI_CCR_ADDTR | XSPI_CCR_ADSIZE),
                   (pCmd->AddressMode | pCmd->AddressDTRMode | pCmd->AddressWidth));
      }

      /* Configure the AR register with the instruction value */
      hxspi->Instance->AR = pCmd->Address;
    }
    else
    {
      /* ---- Invalid command configuration (no instruction, no address) ---- */
      status = HAL_ERROR;
      hxspi->ErrorCode = HAL_XSPI_ERROR_INVALID_PARAM;
    }
  }

  return status;
}

/**
  * @brief  Get the current IOM configuration for an XSPI instance.
  * @param  instance_nb : number of the instance
  * @param  pCfg         : configuration of the IO Manager for the instance
  * @retval HAL status
  */
static void XSPIM_GetConfig(uint8_t instance_nb, XSPIM_CfgTypeDef *pCfg)
{
  uint32_t mux;
  uint32_t mode;

  if (instance_nb == 1U)
  {
    if ((XSPIM->CR & XSPIM_CR_MODE) == 0U)
    {
      pCfg->IOPort = HAL_XSPIM_IOPORT_1;
    }
    else
    {
      pCfg->IOPort = HAL_XSPIM_IOPORT_2;
    }

    if ((XSPIM->CR & XSPIM_CR_CSSEL_OVR_EN) != XSPIM_CR_CSSEL_OVR_EN)
    {
      pCfg->nCSOverride = HAL_XSPI_CSSEL_OVR_DISABLED;
    }
    else if ((XSPIM->CR & XSPIM_CR_CSSEL_OVR_O1) == XSPIM_CR_CSSEL_OVR_O1)
    {
      pCfg->nCSOverride = HAL_XSPI_CSSEL_OVR_NCS2;
    }
    else
    {
      pCfg->nCSOverride = HAL_XSPI_CSSEL_OVR_NCS1;
    }
  }
  else
  {
    mux = (XSPIM->CR & XSPIM_CR_MUXEN);
    mode = ((XSPIM->CR & XSPIM_CR_MODE) >> XSPIM_CR_MODE_Pos);
    if (mux != mode)
    {
      pCfg->IOPort = HAL_XSPIM_IOPORT_1;
    }
    else
    {
      pCfg->IOPort = HAL_XSPIM_IOPORT_2;
    }
    if ((XSPIM->CR & XSPIM_CR_CSSEL_OVR_EN) != XSPIM_CR_CSSEL_OVR_EN)
    {
      pCfg->nCSOverride = HAL_XSPI_CSSEL_OVR_DISABLED;
    }
    else if ((XSPIM->CR & XSPIM_CR_CSSEL_OVR_O2) == XSPIM_CR_CSSEL_OVR_O2)
    {
      pCfg->nCSOverride = HAL_XSPI_CSSEL_OVR_NCS2;
    }
    else
    {
      pCfg->nCSOverride = HAL_XSPI_CSSEL_OVR_NCS1;
    }
  }
}
/**
  @endcond
  */

/**
  * @}
  */

#endif /* HAL_XSPI_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */

#endif /* XSPI || XSPI1 || XSPI2 || XSPI3 */
