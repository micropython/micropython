/**
  ******************************************************************************
  * @file    stm32n6xx_hal_sdio.c
  * @author  MCD Application Team
  * @brief   SDIO HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Secure Digital Input Output (SDIO) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions
  *           + Peripheral State functions
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
                        ##### How to use this driver #####
  ==============================================================================
  [..]
    This driver implements a high level communication layer for read and write from/to
    this SDIO card. The needed STM32 hardware resources (SDMMC and GPIO) are performed by
    the user in HAL_SDIO_MspInit() function (MSP layer).
    Basically, the MSP layer configuration should be the same as we provide in the
    examples.
    You can easily tailor this configuration according to hardware resources.

  [..]
    This driver is a generic layered driver for SDMMC memories which uses the HAL
    SDMMC driver functions to interface with SDIO cards devices.
    It is used as follows:

    (#)Initialize the SDMMC low level resources by implementing the HAL_SDIO_MspInit() API:
        (##) Enable the SDMMC interface clock using __HAL_RCC_SDMMC_CLK_ENABLE();
        (##) SDMMC pins configuration for SDIO card
            (+++) Enable the clock for the SDMMC GPIOs using the functions __HAL_RCC_GPIOx_CLK_ENABLE();
            (+++) Configure these SDMMC pins as alternate function pull-up using HAL_GPIO_Init()
                  and according to your pin assignment;
        (##) NVIC configuration if you need to use interrupt process (HAL_SDIO_ReadExtended_DMA()
             and HAL_SDIO_WriteExtended_DMA() APIs).
            (+++) Configure the SDMMC interrupt priorities using function HAL_NVIC_SetPriority();
            (+++) Enable the NVIC SDMMC IRQs using function HAL_NVIC_EnableIRQ()
            (+++) SDMMC interrupts are managed using the macros __HAL_SDIO_ENABLE_IT()
                  and __HAL_SDIO_DISABLE_IT() inside the communication process.
            (+++) SDMMC interrupts pending bits are managed using the macros __HAL_SDIO_GET_IT().
        (##) No general propose DMA Configuration is needed, an Internal DMA for SDMMC Peripheral are used.

    (#) At this stage, you can perform SDIO read/write/erase operations after SDIO card initialization.

  *** SDIO Card Initialization and configuration ***
  ================================================
  [..]
    To initialize the SDIO Card, use the HAL_SDIO_Init() function. It Initializes
    SDMMC Peripheral(STM32 side) and the SDIO Card, and put it into StandBy State (Ready for data transfer).
    This function provide the following operations:

    (#) Apply the SDIO Card initialization process at 400KHz. You can change or adapt this
        frequency by adjusting the "ClockDiv" field.
        The SDIO Card frequency (SDMMC_CK) is computed as follows:

           SDMMC_CK = SDMMCCLK / (2 * ClockDiv)

        In initialization mode and according to the SDIO Card standard,
        make sure that the SDMMC_CK frequency doesn't exceed 400KHz.

        This phase of initialization is done through SDMMC_Init() and
        SDMMC_PowerState_ON() SDMMC low level APIs.

    (#) Initialize the SDIO card. The API used is HAL_SDIO_Init().
        This phase allows the card initialization and identification.

    (#) Configure the SDIO Card Data transfer frequency. You can change or adapt this
        frequency by adjusting the "ClockDiv" field by the API HAL_SDIO_ConfigFrequency().

    (#) Configure the SDIO Card in wide bus mode: 4-bits data by the API HAL_SDIO_SetDataBusWidth().

    (#) Configure the SDIO Card data block size by the API : HAL_SDIO_SetBlockSize().

    (#) Configure the SDIO Card speed mode by the API : HAL_SDIO_SetSpeedMode().

    (#) To custumize the SDIO Init card function for the enumeration card sequence, you can register a user callback
        function by calling the HAL_SDIO_RegisterIdentifyCardCallback before the HAL_SDIO_Init() function.

  *** SDIO Card Read operation ***
  ==============================
  [..]
    (+) You can read from SDIO card in polling mode by using function HAL_SDIO_ReadExtended().
        This function support only 2048-bytes block length (the block size should be
        chosen by using the API HAL_SDIO_SetBlockSize).

    (+) You can read from SDIO card in DMA mode by using function HAL_SDIO_ReadExtended_DMA().
        This function support only 2048-bytes block length (the block size should be
        chosen by using the API HAL_SDIO_SetBlockSize).
        After this, you have to ensure that the transfer is done correctly.
        You could also check the DMA transfer process through the SDIO Rx interrupt event.

  *** SDIO Card Write operation ***
  ===============================
  [..]
    (+) You can write to SDIO card in polling mode by using function HAL_SDIO_WriteExtended().
        This function support only 2048-bytes block length (the block size should be
        chosen by using the API HAL_SDIO_SetBlockSize).

    (+) You can write to SDIO card in DMA mode by using function HAL_SDIO_WriteExtended_DMA().
        This function support only 2048-bytes block length (the block size should be
        chosen by using the API HAL_SDIO_SetBlockSize).
        You could also check the DMA transfer process through the SDIO Tx interrupt event.


  *** SDIO card common control register (CCCR) ***
  ======================
  [..]
    (+) The SDIO CCCR allow for quick host checking and control of an IO card's enable and interrupts on a per card and
        per function basis.
        To get the Card common control registers field, you can use the API HAL_SDIO_GetCardCommonControlRegister().

  *** SDIO card Function basic register (FBR) ***
  ===========================
  [..]
    (+) The SDIO card function basic register are used to allow the host to quickly determine the abilities and
        requirements of each function.
    (+) To get the SDIO function basic register information, you can use the API HAL_SDIO_GetCardFBRRegister().

  *** SDIO HAL driver macros list ***
  ==================================
  [..]
    Below the list of most used macros in SDIO HAL driver.

    (+) __HAL_SDIO_ENABLE_IT: Enable the SDIO device interrupt
    (+) __HAL_SDIO_DISABLE_IT: Disable the SDIO device interrupt
    (+) __HAL_SDIO_GET_FLAG: Check whether the specified SDIO flag is set or not
    (+) __HAL_SDIO_CLEAR_FLAG: Clear the SDIO's pending flags
    (+) __HAL_SDIO_GET_IT: Check whether the specified SDIO interrupt has occurred or not
    (+) __HAL_SDIO_GET_IT_SOURCE: Checks whether the specified SDIO interrupt is enabled or not

    (@) You can refer to the SDIO HAL driver header file for more useful macros

  *** Callback registration ***
  =============================================
  [..]
    The compilation define USE_HAL_SDIO_REGISTER_CALLBACKS when set to 1
    allows the user to configure dynamically the driver callbacks.

    Use Functions HAL_SDIO_RegisterCallback() to register a user callback,
    it allows to register following callbacks:
      (+) TxCpltCallback : callback when a transmission transfer is completed.
      (+) RxCpltCallback : callback when a reception transfer is completed.
      (+) ErrorCallback : callback when error occurs.
      (+) MspInitCallback    : SDIO MspInit.
      (+) MspDeInitCallback  : SDIO MspDeInit.
    This function takes as parameters the HAL peripheral handle, the Callback ID
    and a pointer to the user callback function.
    For specific callbacks TransceiverCallback use dedicated register callbacks:
    respectively HAL_SDIO_RegisterTransceiverCallback().

    Use function HAL_SDIO_UnRegisterCallback() to reset a callback to the default
    weak (overridden) function. It allows to reset following callbacks:
      (+) TxCpltCallback : callback when a transmission transfer is completed.
      (+) RxCpltCallback : callback when a reception transfer is completed.
      (+) ErrorCallback : callback when error occurs.
      (+) MspInitCallback    : SDIO MspInit.
      (+) MspDeInitCallback  : SDIO MspDeInit.
    This function) takes as parameters the HAL peripheral handle and the Callback ID.
    For specific callbacks TransceiverCallback use dedicated unregister callbacks:
    respectively HAL_SDIO_UnRegisterTransceiverCallback().

    By default, after the HAL_SDIO_Init and if the state is HAL_SDIO_STATE_RESET
    all callbacks are reset to the corresponding legacy weak (overridden) functions.
    Exception done for MspInit and MspDeInit callbacks that are respectively
    reset to the legacy weak (overridden) functions in the HAL_SDIO_Init
    and HAL_SDIO_DeInit only when these callbacks are null (not registered beforehand).
    If not, MspInit or MspDeInit are not null, the HAL_SDIO_Init and HAL_SDIO_DeInit
    keep and use the user MspInit/MspDeInit callbacks (registered beforehand)

    Callbacks can be registered/unregistered in READY state only.
    Exception done for MspInit/MspDeInit callbacks that can be registered/unregistered
    in READY or RESET state, thus registered (user) MspInit/DeInit callbacks can be used
    during the Init/DeInit.
    In that case first register the MspInit/MspDeInit user callbacks
    using HAL_SDIO_RegisterCallback before calling HAL_SDIO_DeInit
    or HAL_SDIO_Init function.

    When The compilation define USE_HAL_SDIO_REGISTER_CALLBACKS is set to 0 or
    not defined, the callback registering feature is not available
    and weak (overridden) callbacks are used.

  *** SDIO peripheral IO interrupt ***
  =============================================
  [..]
  (+) Below the list of most used SDIO function to check and control the IO card's enable and interrupts on a per
      functions basis.

    (+) HAL_SDIO_EnableIOFunctionInterrupt: Enable SDIO IO interrupt.
    (+) HAL_SDIO_DisableIOFunctionInterrupt: Disable SDIO IO interrupt.
    (+) HAL_SDIO_EnableIOFunction: Enable Function number(0-7)
    (+) HAL_SDIO_DisableIOFunction: Disable Function number(0-7)
    (+) HAL_SDIO_SelectIOFunction: Select a function number(0-7)
    (+) HAL_SDIO_AbortIOFunction: Abort an IO read or write operation and free the SDIO bus.
    (+) HAL_SDIO_EnableIOAsynInterrupt: Enable Bit of asynchronous interrupt
    (+) HAL_SDIO_DisableIOAsynInterrupt: Disable Bit of asynchronous interrupt

  @endverbatim
  ******************************************************************************
  */

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @addtogroup SDIO
  * @{
  */
#if defined (SDMMC1) || defined (SDMMC2)
#ifdef HAL_SDIO_MODULE_ENABLED

/* Private define ----------------------------------------------------------------------------------------------------*/
/** @addtogroup SDIO_Private_Defines
  * @{
  */
#define SDIO_INIT_FREQ         400000U /*!< Initialization phase : 400 kHz max */
#define SDIO_TIMEOUT           1000U   /*!< SDIO timeout millisecond           */

#define SDIO_FUNCTION_0        0x00U   /*!< SDIO_Functions 0                   */
#define SDIO_FUNCTION_1        0x01U   /*!< SDIO_Functions 1                   */

#define SDIO_READ              0x0U    /*!< Read flag for cmd52 and cmd53      */
#define SDIO_WRITE             0x1U    /*!< Write flag for cmd52 and cmd53     */

#define SDIO_BUS_SPEED_SDR12   0x00U   /*!< SDIO bus speed mode SDR12          */
#define SDIO_BUS_SPEED_SDR25   0x02U   /*!< SDIO bus speed mode SDR25          */
#define SDIO_BUS_SPEED_SDR50   0x04U   /*!< SDIO bus speed mode SDR50          */
#define SDIO_BUS_SPEED_DDR50   0x08U   /*!< SDIO bus speed mode DDR50          */

#define SDIO_CCCR_REG_NUMBER   0x16U   /*!< SDIO card cccr register number     */

#define SDIO_OCR_VDD_32_33     (1U << 20U)
#define SDIO_OCR_SDIO_S18R     (1U << 24U)
/**
  * @}
  */

/* Private macro -----------------------------------------------------------------------------------------------------*/
#define IS_SDIO_RAW_FLAG(ReadAfterWrite)   (((ReadAfterWrite) == HAL_SDIO_WRITE_ONLY) || \
                                            ((ReadAfterWrite) == HAL_SDIO_READ_AFTER_WRITE))

#define IS_SDIO_FUNCTION(FN)        (((FN) >= HAL_SDIO_FUNCTION_1) && ((FN) <= HAL_SDIO_FUNCTION_7))

#define IS_SDIO_SUPPORTED_BLOCK_SIZE(SDIO_BLOCKSIZE) (((SDIO_BLOCKSIZE) == HAL_SDIO_DATA_BLOCK_SIZE_1BYTE)    || \
                                                      ((SDIO_BLOCKSIZE) == HAL_SDIO_DATA_BLOCK_SIZE_2BYTE)    || \
                                                      ((SDIO_BLOCKSIZE) == HAL_SDIO_DATA_BLOCK_SIZE_4BYTE)    || \
                                                      ((SDIO_BLOCKSIZE) == HAL_SDIO_DATA_BLOCK_SIZE_8BYTE)    || \
                                                      ((SDIO_BLOCKSIZE) == HAL_SDIO_DATA_BLOCK_SIZE_16BYTE)   || \
                                                      ((SDIO_BLOCKSIZE) == HAL_SDIO_DATA_BLOCK_SIZE_32BYTE)   || \
                                                      ((SDIO_BLOCKSIZE) == HAL_SDIO_DATA_BLOCK_SIZE_64BYTE)   || \
                                                      ((SDIO_BLOCKSIZE) == HAL_SDIO_DATA_BLOCK_SIZE_128BYTE)  || \
                                                      ((SDIO_BLOCKSIZE) == HAL_SDIO_DATA_BLOCK_SIZE_256BYTE)  || \
                                                      ((SDIO_BLOCKSIZE) == HAL_SDIO_DATA_BLOCK_SIZE_512BYTE)  || \
                                                      ((SDIO_BLOCKSIZE) == HAL_SDIO_DATA_BLOCK_SIZE_1024BYTE) || \
                                                      ((SDIO_BLOCKSIZE) == HAL_SDIO_DATA_BLOCK_SIZE_2048BYTE))

/* Private functions -------------------------------------------------------------------------------------------------*/
/** @defgroup SDIO_Private_Functions SDIO Private Functions
  * @{
  */
static HAL_StatusTypeDef SDIO_InitCard(SDIO_HandleTypeDef *hsdio);
static HAL_StatusTypeDef SDIO_ReadDirect(SDIO_HandleTypeDef *hsdio, uint32_t addr, uint32_t raw, uint32_t function_nbr,
                                         uint8_t *pData);
static HAL_StatusTypeDef SDIO_WriteDirect(SDIO_HandleTypeDef *hsdio, uint32_t addr, uint32_t raw, uint32_t function_nbr,
                                          const uint8_t *pData);
static HAL_StatusTypeDef SDIO_WriteExtended(SDIO_HandleTypeDef *hsdio, HAL_SDIO_ExtendedCmd_TypeDef *cmd_arg,
                                            uint8_t *pData, uint16_t Size_byte);
static uint8_t SDIO_Convert_Block_Size(const SDIO_HandleTypeDef *hsdio, uint32_t block_size);
static HAL_StatusTypeDef SDIO_IOFunction_IRQHandler(SDIO_HandleTypeDef *hsdio);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup SDIO_Exported_Functions
  * @{
  */
/** @addtogroup SDIO_Exported_Functions_Group1
  *  @brief   Initialization and de-initialization functions
  *
@verbatim
  ==============================================================================
          ##### Initialization and de-initialization functions #####
  ==============================================================================
  [..]
    This section provides functions allowing to initialize/de-initialize the SDIO
    device to be ready for use.

@endverbatim
  * @{
  */
/**
  * @brief  Initializes the SDIO according to the specified parameters in the
            SDIO_HandleTypeDef and create the associated handle.
  * @param  hsdio: Pointer to the SDIO handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_Init(SDIO_HandleTypeDef *hsdio)
{
  SDIO_InitTypeDef Init;
  uint32_t sdmmc_clk;
  uint8_t data;

  /* Check the parameters */
  assert_param(hsdio != NULL);
  assert_param(IS_SDMMC_ALL_INSTANCE(hsdio->Instance));
  assert_param(IS_SDMMC_CLOCK_EDGE(hsdio->Init.ClockEdge));
  assert_param(IS_SDMMC_CLOCK_POWER_SAVE(hsdio->Init.ClockPowerSave));
  assert_param(IS_SDMMC_BUS_WIDE(hsdio->Init.BusWide));
  assert_param(IS_SDMMC_HARDWARE_FLOW_CONTROL(hsdio->Init.HardwareFlowControl));
  assert_param(IS_SDMMC_CLKDIV(hsdio->Init.ClockDiv));

  /* Check the SDIO handle allocation */
  if (hsdio == NULL)
  {
    return HAL_ERROR;
  }

  if (hsdio->State == HAL_SDIO_STATE_RESET)
  {
#if defined (USE_HAL_SDIO_REGISTER_CALLBACKS) && (USE_HAL_SDIO_REGISTER_CALLBACKS == 1U)
    /* Reset Callback pointers in HAL_SDIO_STATE_RESET only */
    hsdio->TxCpltCallback = HAL_SDIO_TxCpltCallback;
    hsdio->RxCpltCallback = HAL_SDIO_RxCpltCallback;
    hsdio->ErrorCallback  = HAL_SDIO_ErrorCallback;
#if (USE_SDIO_TRANSCEIVER != 0U)
    if (hsdio->Init.TranceiverPresent == SDMMC_TRANSCEIVER_PRESENT)
    {
      hsdio->DriveTransceiver_1_8V_Callback = HAL_SDIO_DriveTransceiver_1_8V_Callback;
    }
#endif /* USE_SDIO_TRANSCEIVER */

    if (hsdio->MspInitCallback == NULL)
    {
      hsdio->MspInitCallback = HAL_SDIO_MspInit;
    }
    /* Init the low level hardware */
    hsdio->MspInitCallback(hsdio);
#else
    /* Init the low level hardware : GPIO, CLOCK, CORTEX...etc */
    HAL_SDIO_MspInit(hsdio);
#endif /* USE_HAL_SDIO_REGISTER_CALLBACKS */
  }

  Init.ClockEdge           = SDMMC_CLOCK_EDGE_RISING;
  Init.ClockPowerSave      = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  Init.BusWide             = SDMMC_BUS_WIDE_1B;
  Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;

  if (hsdio->Instance == SDMMC1)
  {
    sdmmc_clk = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SDMMC1);
  }
  else if (hsdio->Instance == SDMMC2)
  {
    sdmmc_clk = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SDMMC2);
  }
  else
  {
    sdmmc_clk = 0;
  }
  if (sdmmc_clk == 0U)
  {
    hsdio->ErrorCode = SDMMC_ERROR_INVALID_PARAMETER;
    return HAL_ERROR;
  }
  Init.ClockDiv = sdmmc_clk / (2U * SDIO_INIT_FREQ);
  /* Initialize SDMMC peripheral interface with default configuration */
  (void)SDMMC_Init(hsdio->Instance, Init);

  /* Set Power State to ON */
  (void)SDMMC_PowerState_ON(hsdio->Instance);

  /* wait 74 Cycles: required power up waiting time before starting the SDIO initialization sequence */
  sdmmc_clk = sdmmc_clk / (2U * Init.ClockDiv);
  HAL_Delay(1U + (74U * 1000U / (sdmmc_clk)));

  if (hsdio->SDIO_IdentifyCard == NULL)
  {
    hsdio->SDIO_IdentifyCard = SDIO_InitCard;
  }
  /* SDIO enumeration sequence */
  if (hsdio->SDIO_IdentifyCard(hsdio) != HAL_OK)
  {
    hsdio->State = HAL_SDIO_STATE_RESET;
    return HAL_ERROR;
  }

  /* Configure the SDMMC user parameters */
  Init.ClockEdge           = hsdio->Init.ClockEdge;
  Init.ClockPowerSave      = hsdio->Init.ClockPowerSave;
  Init.BusWide             = hsdio->Init.BusWide;
  Init.HardwareFlowControl = hsdio->Init.HardwareFlowControl;
  Init.ClockDiv            = hsdio->Init.ClockDiv;
  (void)SDMMC_Init(hsdio->Instance, Init);

  data = (hsdio->Init.BusWide == HAL_SDIO_4_WIRES_MODE) ? 2U : 0U;
  if (SDIO_WriteDirect(hsdio, SDMMC_SDIO_CCCR4_SD_BYTE3, HAL_SDIO_WRITE_ONLY, SDIO_FUNCTION_0, &data) != HAL_OK)
  {
    return HAL_ERROR;
  }

  hsdio->Context = SDIO_CONTEXT_NONE;
  hsdio->State = HAL_SDIO_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  De-Initializes the SDIO device.
  * @param  hsdio: Pointer to the SDIO handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_DeInit(SDIO_HandleTypeDef *hsdio)
{
  /* Check the parameters */
  assert_param(IS_SDMMC_ALL_INSTANCE(hsdio->Instance));

  /* Check the SDIO handle allocation */
  if (hsdio == NULL)
  {
    return HAL_ERROR;
  }

  /* Set Power State to OFF */
  (void)SDMMC_PowerState_OFF(hsdio->Instance);

#if defined (USE_HAL_SDIO_REGISTER_CALLBACKS) && (USE_HAL_SDIO_REGISTER_CALLBACKS == 1U)
  if (hsdio->MspDeInitCallback == NULL)
  {
    hsdio->MspDeInitCallback = HAL_SDIO_MspDeInit;
  }

  /* DeInit the low level hardware */
  hsdio->MspDeInitCallback(hsdio);
#else
  /* De-Initialize the MSP layer */
  HAL_SDIO_MspDeInit(hsdio);
#endif /* USE_HAL_SDIO_REGISTER_CALLBACKS */

  hsdio->ErrorCode = HAL_SDIO_ERROR_NONE;
  hsdio->State = HAL_SDIO_STATE_RESET;

  return HAL_OK;
}

/**
  * @brief  Initializes the SDIO MSP.
  * @param  hsdio: Pointer to SDIO handle
  * @retval None
  */
__weak void HAL_SDIO_MspInit(SDIO_HandleTypeDef *hsdio)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsdio);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SDIO_MspInit could be implemented in the user file
   */
}

/**
  * @brief  De-Initialize SDIO MSP.
  * @param  hsdio: Pointer to SDIO handle
  * @retval None
  */
__weak void HAL_SDIO_MspDeInit(SDIO_HandleTypeDef *hsdio)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsdio);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SDIO_MspDeInit could be implemented in the user file
   */
}
/**
  * @}
  */

/** @addtogroup SDIO_Exported_Functions_Group2
  *  @brief
  *
@verbatim
  ==============================================================================
          ##### Initialization and de-initialization functions #####
  ==============================================================================
  [..]
    This subsection provides a set of functions allowing to re-configure the SDIO peripheral.

@endverbatim
  * @{
  */
/**
  * @brief  Enables wide bus operation for the requested card if supported by card.
  * @param  hsdio: Pointer to SDIO handle
  * @param  BusWide: Specifies the SDIO card wide bus mode
  *          This parameter can be one of the following values:
  *            @arg SDMMC_BUS_WIDE_8B: 8-bit data transfer
  *            @arg SDMMC_BUS_WIDE_4B: 4-bit data transfer
  *            @arg SDMMC_BUS_WIDE_1B: 1-bit data transfer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_SetDataBusWidth(SDIO_HandleTypeDef *hsdio, uint32_t BusWide)
{
  uint8_t data;
  HAL_StatusTypeDef error_state = HAL_OK;

  /* Check the parameters */
  assert_param(hsdio != NULL);

  /* Check the SDIO peripheral handle parameter */
  if (hsdio == NULL)
  {
    return HAL_ERROR;
  }

  if (hsdio->State == HAL_SDIO_STATE_READY)
  {
    data = (BusWide == HAL_SDIO_4_WIRES_MODE) ? 2U : 0U;
    MODIFY_REG(hsdio->Instance->CLKCR, SDMMC_CLKCR_WIDBUS,
               (BusWide == HAL_SDIO_4_WIRES_MODE) ? SDMMC_BUS_WIDE_4B : SDMMC_BUS_WIDE_1B);

    if (SDIO_WriteDirect(hsdio, SDMMC_SDIO_CCCR4_SD_BYTE3, HAL_SDIO_WRITE_ONLY, SDIO_FUNCTION_0, &data) != HAL_OK)
    {
      error_state =  HAL_ERROR;
    }
  }
  else
  {
    error_state =  HAL_ERROR;
  }

  return error_state;
}

/**
  * @brief  Update the SDIO Clock.
  * @param  hsdio: Pointer to SDIO handle.
  * @param  ClockSpeed: SDIO Clock speed.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_ConfigFrequency(SDIO_HandleTypeDef *hsdio, uint32_t ClockSpeed)
{
  uint32_t ClockDiv;

  /* Check the parameters */
  assert_param(hsdio != NULL);

  /* Check the SDIO peripheral handle parameter */
  if (hsdio == NULL)
  {
    return HAL_ERROR;
  }

  if (hsdio->State == HAL_SDIO_STATE_READY)
  {
    if (hsdio->Instance == SDMMC1)
    {
      ClockDiv = (HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SDMMC1)) / (2U * ClockSpeed);
    }
#if defined(SDMMC2)
    if (hsdio->Instance == SDMMC2)
    {
      ClockDiv = (HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SDMMC2)) / (2U * ClockSpeed);
    }
#endif /* SDMMC2 */
    MODIFY_REG(hsdio->Instance->CLKCR, SDMMC_CLKCR_CLKDIV, ClockDiv);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Set the SDIO block size.
  * @param  hsdio: Pointer to SDIO handle
  * @param  function_nbr: Specifies the SDIO function number.
  * @param  BlockSize: Specifies the SDIO Block size to set.
  *         This parameter can be one of the following values @ref SDIO_Exported_Constansts_Group7.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_SetBlockSize(SDIO_HandleTypeDef *hsdio, uint8_t function_nbr, uint16_t BlockSize)
{
  HAL_SDIO_ExtendedCmd_TypeDef cmd53;

  /* Check the parameters */
  assert_param(hsdio != NULL);
  assert_param(IS_SDIO_FUNCTION(function_nbr));
  assert_param(IS_SDIO_SUPPORTED_BLOCK_SIZE(BlockSize));

  /* Check the SDIO peripheral handle parameter */
  if (hsdio == NULL)
  {
    return HAL_ERROR;
  }

  /* Set SDIO F1 block size */
  cmd53.IOFunctionNbr = SDIO_FUNCTION_0;
  cmd53.OpCode = HAL_SDIO_OP_CODE_AUTO_INC;
  cmd53.Block_Mode = HAL_SDIO_MODE_BYTE;
  cmd53.Reg_Addr = (function_nbr * 0x100UL) + 0x10UL;
  if (SDIO_WriteExtended(hsdio, &cmd53, (uint8_t *)(&BlockSize), 2U) != HAL_OK)
  {
    return HAL_ERROR;
  }

  hsdio->block_size = BlockSize;

  return HAL_OK;
}

/**
  * @brief  Configure the data rate.
  * @param  hsdio: Pointer to SDIO handle
  * @param  DataRate: Specifies the SDIO data rate to set.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_SetSpeedMode(SDIO_HandleTypeDef *hsdio, uint32_t DataRate)
{
  HAL_StatusTypeDef errorstate = HAL_OK;
  uint8_t data;

  /* Check the parameters */
  assert_param(hsdio != NULL);

  /* Check the SDIO peripheral handle parameter */
  if (hsdio == NULL)
  {
    return HAL_ERROR;
  }

  switch (DataRate)
  {
    case HAL_SDIOS_DATA_RATE_SDR25:
      data = SDIO_BUS_SPEED_SDR25;
      errorstate = SDIO_WriteDirect(hsdio, SDMMC_SDIO_CCCR16_SD_BYTE3, HAL_SDIO_WRITE_ONLY, SDIO_FUNCTION_0, &data);
      break;

    case HAL_SDIOS_DATA_RATE_SDR50:
      data = SDIO_BUS_SPEED_SDR50;
      errorstate = SDIO_WriteDirect(hsdio, ((SDIO_FUNCTION_0 << 2U) | (SDIO_FUNCTION_0 << 1U) | (SDIO_FUNCTION_0 << 14U)
                                            | SDMMC_SDIO_CCCR16_SD_BYTE3), HAL_SDIO_WRITE_ONLY, SDIO_FUNCTION_0, &data);
      MODIFY_REG(hsdio->Instance->CLKCR, SDMMC_CLKCR_BUSSPEED, SDMMC_CLKCR_BUSSPEED);
      break;

    case HAL_SDIOS_DATA_RATE_DDR50:
      data = SDIO_BUS_SPEED_DDR50;
      errorstate = SDIO_WriteDirect(hsdio, ((SDIO_FUNCTION_0 << 2) | (SDIO_FUNCTION_0 << 1) | (SDIO_FUNCTION_0 << 14) |
                                            SDMMC_SDIO_CCCR16_SD_BYTE3), HAL_SDIO_WRITE_ONLY, SDIO_FUNCTION_0, &data);
      MODIFY_REG(hsdio->Instance->CLKCR, SDMMC_CLKCR_DDR | SDMMC_CLKCR_BUSSPEED,
                 SDMMC_CLKCR_DDR | SDMMC_CLKCR_BUSSPEED);
      break;
    default: /* SDR12 */
      break;
  }

  return (errorstate != HAL_OK) ? HAL_ERROR : HAL_OK;
}

/**
  * @brief  Reset SDIO Card
  * @param  hsdio: Pointer to SDIO handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_CardReset(SDIO_HandleTypeDef *hsdio)
{
  uint8_t data = 0U;

  /* Check the parameters */
  assert_param(hsdio != NULL);

  /* Check the SDIO peripheral handle parameter */
  if (hsdio == NULL)
  {
    return HAL_ERROR;
  }

  /**  To reset the SDIO module by CMD52 with writing to RES in CCCR or send CMD0 the card shall change the speed mode
    *  default speed mode.
    *  The reset cmd (cmd0) is only used for memory. In order to reset an I/O card or the I/O portion of a combo card,
    *  Use CMD52 to write 1 to the RES bit in the CCC(bit3 of register 6).
    */
  if (SDIO_WriteDirect(hsdio, ((SDIO_FUNCTION_0 << 2) | (SDIO_FUNCTION_0 << 1) | (SDIO_FUNCTION_0 << 14) |
                               SDMMC_SDIO_CCCR4_SD_BYTE2),
                       HAL_SDIO_WRITE_ONLY,
                       0U,
                       &data) != HAL_OK)
  {
    return HAL_ERROR;
  }

  hsdio->State = HAL_SDIO_STATE_RESET;

  return HAL_OK;
}

/**
  * @brief  Get Card Common Control register (CCCR).
  * @param  hsdio: Pointer to SDIO handle.
  * @param  pCccr: Pointer to Cccr register.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_GetCardCommonControlRegister(SDIO_HandleTypeDef *hsdio, HAL_SDIO_CCCR_TypeDef *pCccr)
{
  uint8_t tempBuffer[256] = {0U};
  uint32_t count;

  assert_param(hsdio != NULL);
  assert_param(pCccr != NULL);

  if ((hsdio == NULL) || (pCccr == NULL))
  {
    return HAL_ERROR;
  }

  for (count = 0U; count <= SDIO_CCCR_REG_NUMBER; count++)
  {
    if (SDIO_ReadDirect(hsdio, SDMMC_SDIO_CCCR0 + count, HAL_SDIO_WRITE_ONLY, SDIO_FUNCTION_0, &tempBuffer[count]) !=
        HAL_OK)
    {
      return HAL_ERROR;
    }
  }

  pCccr->cccr_revision = tempBuffer[0] & 0x0FU;
  pCccr->sdio_revision = (tempBuffer[0] & 0xF0U) >> 4U;
  pCccr->sd_spec_revision = tempBuffer[0x01U] & 0x0FU;
  pCccr->bus_width_8Bit = ((tempBuffer[0x07U] & 0x04U) != 0U) ? HAL_SDIO_BUS_WIDTH_8BIT_SUPPORTED
                          : HAL_SDIO_BUS_WIDTH_8BIT_NOT_SUPPORTED;
  pCccr->card_capability = (tempBuffer[0x08U] & 0xDFUL);
  /* common CIS pointer */
  pCccr->commonCISPointer = tempBuffer[0x09U] | ((uint32_t)tempBuffer[(uint32_t)0x09U + 1U] << 8U) |
                            ((uint32_t)tempBuffer[(uint32_t)0x09U + 2U] << 16U);

  return HAL_OK;
}

/**
  * @brief  Get Card Function Basic register(FBR).
  * @param  hsdio: Pointer to SDIO handle.
  * @param  pFbr: Pointer to Fbr register.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_GetCardFBRRegister(SDIO_HandleTypeDef *hsdio, HAL_SDIO_FBR_t *pFbr)
{
  uint8_t tempBuffer[256] = {0U};
  uint32_t count;
  uint8_t func_idx;

  assert_param(hsdio != NULL);
  assert_param(pFbr != NULL);

  if ((hsdio == NULL) || (pFbr == NULL))
  {
    return HAL_ERROR;
  }

  for (func_idx = 2U; func_idx <= SDIO_MAX_IO_NUMBER; func_idx++)
  {
    for (count = 0U; count <= SDIO_CCCR_REG_NUMBER; count++)
    {
      if (SDIO_ReadDirect(hsdio, (((uint32_t)SDMMC_SDIO_F1BR0 * (uint32_t)func_idx) + count),
                          HAL_SDIO_WRITE_ONLY, SDIO_FUNCTION_0, &tempBuffer[count]) != HAL_OK)
      {
        return HAL_ERROR;
      }
    }
    pFbr[(uint32_t)func_idx - 1U].ioStdFunctionCode = tempBuffer[0U] & 0x0FU;
    pFbr[(uint32_t)func_idx - 1U].ioExtFunctionCode = tempBuffer[1U];
    pFbr[(uint32_t)func_idx - 1U].ioPointerToCIS = tempBuffer[9U] | ((uint32_t)tempBuffer[10U] << 8U) |
                                                   ((uint32_t)tempBuffer[11U] << 16U);
    pFbr[(uint32_t)func_idx - 1U].ioPointerToCSA = tempBuffer[12U] | ((uint32_t)tempBuffer[13U] << 8U) |
                                                   ((uint32_t)tempBuffer[14U] << 16U);
    if ((tempBuffer[2U] & 0x01U) != 0U)
    {
      pFbr[(uint32_t)func_idx - 1U].flags |= (uint8_t)HAL_SDIO_FBR_SUPPORT_POWER_SELECTION;
    }
    if ((tempBuffer[0U] & 0x40U) != 0U)
    {
      pFbr[(uint32_t)func_idx - 1U].flags |= (uint8_t)HAL_SDIO_FBR_SUPPORT_CSA;
    }
  }

  return HAL_OK;
}
/**
  * @}
  */

/** @addtogroup SDIO_Exported_Functions_Group3
  *  @brief
  *
@verbatim
  ==============================================================================
          ##### Data management functions #####
  ==============================================================================
  [..]
    This subsection provides a set of functions allowing to manage the data transfer from/to SDIO card.

@endverbatim
  * @{
  */
/**
  * @brief  Read data from a specified address using the direct mode through cmd52.
  * @param  hsdio: Pointer to SDIO handle
  * @param  Argument: Specifies the SDIO Argument.
  * @param  pData: pointer to the buffer that will contain the received data.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_ReadDirect(SDIO_HandleTypeDef *hsdio, const HAL_SDIO_DirectCmd_TypeDef *Argument,
                                      uint8_t *pData)
{
  uint32_t cmd;
  uint32_t errorstate;

  /* Check the parameters */
  assert_param(hsdio != NULL);
  assert_param(Argument != NULL);
  assert_param(pData != NULL);
  assert_param(IS_SDIO_RAW_FLAG(Argument->ReadAfterWrite));

  if ((hsdio == NULL) || (Argument == NULL) || (NULL == pData))
  {
    return HAL_ERROR;
  }

  if (hsdio->State == HAL_SDIO_STATE_READY)
  {
    hsdio->ErrorCode = HAL_SDIO_ERROR_NONE;
    hsdio->State = HAL_SDIO_STATE_BUSY;

    cmd = SDIO_READ << 31U;
    cmd |= (((uint32_t)Argument->IOFunctionNbr) << 28U);
    cmd |= (((uint32_t)Argument->ReadAfterWrite) << 27U);
    cmd |= (Argument->Reg_Addr & 0x1FFFFU) << 9U;
    cmd |= 0U;
    errorstate = SDMMC_SDIO_CmdReadWriteDirect(hsdio->Instance, cmd, pData);

    if (errorstate != HAL_SDIO_ERROR_NONE)
    {
      hsdio->ErrorCode |= errorstate;
      if (errorstate != (SDMMC_ERROR_ADDR_OUT_OF_RANGE | SDMMC_ERROR_ILLEGAL_CMD | SDMMC_ERROR_COM_CRC_FAILED |
                         SDMMC_ERROR_GENERAL_UNKNOWN_ERR))
      {
        /* Clear all the static flags */
        __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_FLAGS);
        hsdio->State = HAL_SDIO_STATE_READY;
        hsdio->Context = SDIO_CONTEXT_NONE;
        return HAL_ERROR;
      }
    }

    __SDMMC_CMDTRANS_DISABLE(hsdio->Instance);

    /* Clear all the static flags */
    __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_DATA_FLAGS);

    hsdio->State = HAL_SDIO_STATE_READY;
  }
  else
  {
    return HAL_BUSY;
  }

  return HAL_OK;
}


/**
  * @brief  Read data from a specified address using the direct mode through cmd52.
  * @param  hsdio: Pointer to SDIO handle
  * @param  Argument: Specifies the SDIO Argument.
  * @param  Data: pointer to the buffer that will contain the received data.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_WriteDirect(SDIO_HandleTypeDef *hsdio, const HAL_SDIO_DirectCmd_TypeDef *Argument,
                                       uint8_t Data)
{
  uint32_t cmd;
  uint32_t errorstate;

  /* Check the parameters */
  assert_param(hsdio != NULL);
  assert_param(Argument != NULL);
  assert_param(IS_SDIO_RAW_FLAG(Argument->ReadAfterWrite));

  if ((hsdio == NULL) || (Argument == NULL))
  {
    return HAL_ERROR;
  }

  if (hsdio->State == HAL_SDIO_STATE_READY)
  {
    hsdio->ErrorCode = HAL_SDIO_ERROR_NONE;
    hsdio->State = HAL_SDIO_STATE_BUSY;

    cmd = SDIO_WRITE << 31U;
    cmd |= ((uint32_t)Argument->IOFunctionNbr) << 28U;
    cmd |= ((uint32_t)Argument->ReadAfterWrite) << 27U;
    cmd |= (Argument->Reg_Addr & 0x1FFFFU) << 9U;
    cmd |= Data;
    errorstate = SDMMC_SDIO_CmdReadWriteDirect(hsdio->Instance, cmd, &Data);
    if (errorstate != HAL_SDIO_ERROR_NONE)
    {
      hsdio->ErrorCode |= errorstate;
      if (errorstate != (SDMMC_ERROR_ADDR_OUT_OF_RANGE | SDMMC_ERROR_ILLEGAL_CMD | SDMMC_ERROR_COM_CRC_FAILED |
                         SDMMC_ERROR_GENERAL_UNKNOWN_ERR))
      {
        /* Clear all the static flags */
        __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_FLAGS);
        hsdio->State = HAL_SDIO_STATE_READY;
        hsdio->Context = SDIO_CONTEXT_NONE;
        return HAL_ERROR;
      }
    }

    __SDMMC_CMDTRANS_DISABLE(hsdio->Instance);

    /* Clear all the static flags */
    __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_DATA_FLAGS);

    hsdio->State = HAL_SDIO_STATE_READY;
  }
  else
  {
    return HAL_BUSY;
  }

  return HAL_OK;
}

/**
  * @brief  Read data from a specified address using extended mode through cmd53.
  * @param  hsdio: Pointer to SDIO handle
  * @param  Argument: Pointer to SDIO argument
  * @param  pData:  pointer to the buffer that will contain the data to transmit
  * @param  Size_byte:   size to read.
  * @param  Timeout_Ms:  Specify timeout value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_ReadExtended(SDIO_HandleTypeDef *hsdio, const HAL_SDIO_ExtendedCmd_TypeDef *Argument,
                                        uint8_t *pData, uint32_t Size_byte, uint32_t Timeout_Ms)
{
  uint32_t cmd;
  SDMMC_DataInitTypeDef config;
  uint32_t errorstate;
  uint32_t tickstart = HAL_GetTick();
  uint32_t regCount;
  uint8_t byteCount;
  uint32_t data;
  uint32_t dataremaining;
  uint8_t *tempbuff = pData;
  uint32_t nbr_of_block;

  /* Check the parameters */
  assert_param(hsdio != NULL);
  assert_param(Argument != NULL);
  assert_param(pData != NULL);

  if ((hsdio == NULL) || (Argument == NULL) || (pData == NULL))
  {
    return HAL_ERROR;
  }

  if (hsdio->State == HAL_SDIO_STATE_READY)
  {
    hsdio->ErrorCode = HAL_SDIO_ERROR_NONE;
    hsdio->State = HAL_SDIO_STATE_BUSY;

    /* Compute how many blocks are to be send for pData of length data_size to be send */
    nbr_of_block = (Size_byte & ~(hsdio->block_size & 1U)) >> __CLZ(__RBIT(hsdio->block_size));

    /* Initialize data control register */
    if ((hsdio->Instance->DCTRL & SDMMC_DCTRL_SDIOEN) != 0U)
    {
      hsdio->Instance->DCTRL = SDMMC_DCTRL_SDIOEN;
    }
    else
    {
      hsdio->Instance->DCTRL = 0U;
    }

    /* Configure the SDIO DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    /* (HAL_SDIO_MODE_BLOCK << 27) corresponds to the block mode bit of the CMD argument */
    if (Argument->Block_Mode == HAL_SDIO_MODE_BLOCK)
    {
      /* (Argument & 0x1FFU) is to get the 9 bits of Block/Byte counts */
      config.DataLength    = (uint32_t)(nbr_of_block * hsdio->block_size);
      config.DataBlockSize = SDIO_Convert_Block_Size(hsdio, hsdio->block_size);
    }
    else
    {
      /* (Argument & 0x1FFU) is to get the 9 bits of Block/Byte counts */
      config.DataLength    = (Size_byte > 0U) ? Size_byte : HAL_SDIO_DATA_BLOCK_SIZE_512BYTE;
      config.DataBlockSize = SDMMC_DATABLOCK_SIZE_1B;
    }

    config.TransferDir = SDMMC_TRANSFER_DIR_TO_SDMMC ;
    /* (HAL_SDIO_MODE_BLOCK << 27) corresponds to the block mode bit of the CMD argument */
    config.TransferMode = (Argument->Block_Mode == HAL_SDIO_MODE_BLOCK) ? SDMMC_TRANSFER_MODE_BLOCK :
                          SDMMC_TRANSFER_MODE_SDIO;
    config.DPSM         = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hsdio->Instance, &config);
    __SDMMC_CMDTRANS_ENABLE(hsdio->Instance);

    /* Correspond to the write or read bit of the CMD argument */
    /* Read */
    hsdio->Context = (Argument->Block_Mode == HAL_SDIO_MODE_BLOCK) ? SDIO_CONTEXT_READ_MULTIPLE_BLOCK :
                     SDIO_CONTEXT_READ_SINGLE_BLOCK;
    cmd = SDIO_READ << 31U;
    cmd |= Argument->IOFunctionNbr << 28U;
    cmd |= Argument->Block_Mode << 27U;
    cmd |= Argument->OpCode << 26U;
    cmd |= (Argument->Reg_Addr & 0x1FFFFU) << 9U;
    cmd |= (Size_byte & 0x1FFU);
    errorstate = SDMMC_SDIO_CmdReadWriteExtended(hsdio->Instance, cmd);
    if (errorstate != HAL_SDIO_ERROR_NONE)
    {
      hsdio->ErrorCode |= errorstate;
      if (errorstate != (SDMMC_ERROR_ADDR_OUT_OF_RANGE | SDMMC_ERROR_ILLEGAL_CMD | SDMMC_ERROR_COM_CRC_FAILED |
                         SDMMC_ERROR_GENERAL_UNKNOWN_ERR))
      {
        MODIFY_REG(hsdio->Instance->DCTRL, SDMMC_DCTRL_FIFORST, SDMMC_DCTRL_FIFORST);
        __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_FLAGS);
        __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_DATA_FLAGS);
        hsdio->State = HAL_SDIO_STATE_READY;
        hsdio->Context = SDIO_CONTEXT_NONE;
        return HAL_ERROR;
      }
    }
    /* (SDIO_WRITE << 31) correspond to the write or read bit of the CMD argument */
    /* Poll on SDMMC flags */
    dataremaining = config.DataLength;

    while (!__HAL_SDIO_GET_FLAG(hsdio, SDMMC_FLAG_RXOVERR | SDMMC_FLAG_DCRCFAIL |
                                SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DATAEND))
    {
      if (__HAL_SDIO_GET_FLAG(hsdio, SDMMC_FLAG_RXFIFOHF) && (dataremaining >= SDMMC_FIFO_SIZE))
      {
        /* Read data from SDMMC Rx FIFO */
        for (regCount = 0U; regCount < (SDMMC_FIFO_SIZE / 4U); regCount++)
        {
          data = SDMMC_ReadFIFO(hsdio->Instance);
          *tempbuff = (uint8_t)(data & 0xFFU);
          tempbuff++;
          *tempbuff = (uint8_t)((data >> 8U) & 0xFFU);
          tempbuff++;
          *tempbuff = (uint8_t)((data >> 16U) & 0xFFU);
          tempbuff++;
          *tempbuff = (uint8_t)((data >> 24U) & 0xFFU);
          tempbuff++;
        }
        dataremaining -= SDMMC_FIFO_SIZE;
      }
      else if (dataremaining < SDMMC_FIFO_SIZE)
      {
        while (!(__HAL_SDIO_GET_FLAG(hsdio, SDMMC_FLAG_RXFIFOE)) && (dataremaining > 0U))
        {
          data = SDMMC_ReadFIFO(hsdio->Instance);
          for (byteCount = 0U; byteCount < 4U; byteCount++)
          {
            if (dataremaining > 0U)
            {
              *tempbuff = (uint8_t)((data >> (byteCount * 8U)) & 0xFFU);
              tempbuff++;
              dataremaining--;
            }
          }
        }
      }
      else
      {
        /* Nothing to do */
      }
      if ((HAL_GetTick() - tickstart) >=  Timeout_Ms)
      {
        /* Clear all the static flags */
        __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_FLAGS);
        hsdio->ErrorCode |= HAL_SDIO_ERROR_TIMEOUT;
        hsdio->State = HAL_SDIO_STATE_READY;
        hsdio->Context = SDIO_CONTEXT_NONE;
        return HAL_TIMEOUT;
      }
    }
    __SDMMC_CMDTRANS_DISABLE(hsdio->Instance);
    /* Get error state */
    if (__HAL_SDIO_GET_FLAG(hsdio, SDMMC_FLAG_DTIMEOUT))
    {
      /* Clear all the static flags */
      __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_FLAGS);
      hsdio->ErrorCode |= HAL_SDIO_ERROR_DATA_TIMEOUT;
      hsdio->State = HAL_SDIO_STATE_READY;
      hsdio->Context = SDIO_CONTEXT_NONE;
      return HAL_ERROR;
    }
    else if (__HAL_SDIO_GET_FLAG(hsdio, SDMMC_FLAG_DCRCFAIL))
    {
      /* Clear all the static flags */
      __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_FLAGS);
      hsdio->ErrorCode |= HAL_SDIO_ERROR_DATA_CRC_FAIL;
      hsdio->State = HAL_SDIO_STATE_READY;
      hsdio->Context = SDIO_CONTEXT_NONE;
      return HAL_ERROR;
    }
    else if (__HAL_SDIO_GET_FLAG(hsdio, SDMMC_FLAG_RXOVERR))
    {
      /* Clear all the static flags */
      __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_FLAGS);
      /* (SDIO_WRITE << 31) correspond to the write or read bit of the CMD argument */
      hsdio->ErrorCode |= HAL_SDIO_ERROR_RX_OVERRUN;
      hsdio->State = HAL_SDIO_STATE_READY;
      hsdio->Context = SDIO_CONTEXT_NONE;
      return HAL_ERROR;
    }
    else if (hsdio->ErrorCode == SDMMC_ERROR_INVALID_PARAMETER)
    {
      __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_DATA_FLAGS);
      hsdio->State = HAL_SDIO_STATE_READY;
      hsdio->Context = SDIO_CONTEXT_NONE;
      return HAL_ERROR;
    }
    else
    {
      /* Nothing to do */
    }

    /* Clear all the static flags */
    __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_DATA_FLAGS);

    hsdio->State = HAL_SDIO_STATE_READY;
  }
  else
  {
    return HAL_BUSY;
  }

  return HAL_OK;
}

/**
  * @brief  Write data from a specified address using extended mode through cmd53.
  * @param  hsdio: Pointer to SDIO handle
  * @param  Argument: Pointer to SDIO argument
  * @param  pData:  pointer to the buffer that will contain the data to transmit
  * @param  Size_byte:  Block size to write.
  * @param  Timeout_Ms:  Specify timeout value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_WriteExtended(SDIO_HandleTypeDef *hsdio, const HAL_SDIO_ExtendedCmd_TypeDef *Argument,
                                         uint8_t *pData, uint32_t Size_byte, uint32_t Timeout_Ms)
{
  uint32_t cmd;
  SDMMC_DataInitTypeDef config;
  uint32_t errorstate;
  uint32_t tickstart = HAL_GetTick();
  uint32_t regCount;
  uint8_t byteCount;
  uint32_t data;
  uint32_t dataremaining;
  uint8_t *u32tempbuff = pData;
  uint32_t nbr_of_block;

  /* Check the parameters */
  assert_param(hsdio != NULL);
  assert_param(Argument != NULL);
  assert_param(pData != NULL);

  if ((hsdio == NULL) || (Argument == NULL) || (pData == NULL))
  {
    return HAL_ERROR;
  }

  if (hsdio->State == HAL_SDIO_STATE_READY)
  {
    hsdio->ErrorCode = HAL_SDIO_ERROR_NONE;
    hsdio->State = HAL_SDIO_STATE_BUSY;

    /* Compute how many blocks are to be send for pData of length data_size to be send */
    nbr_of_block = (Size_byte & ~(hsdio->block_size & 1U)) >> __CLZ(__RBIT(hsdio->block_size));

    /* Initialize data control register */
    if ((hsdio->Instance->DCTRL & SDMMC_DCTRL_SDIOEN) != 0U)
    {
      hsdio->Instance->DCTRL = SDMMC_DCTRL_SDIOEN;
    }
    else
    {
      hsdio->Instance->DCTRL = 0U;
    }

    /* Configure the SDIO DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    if (Argument->Block_Mode == HAL_SDIO_MODE_BLOCK)
    {
      config.DataLength    = (uint32_t)(nbr_of_block * hsdio->block_size);
      config.DataBlockSize = SDIO_Convert_Block_Size(hsdio, hsdio->block_size);
    }
    else
    {
      config.DataLength    = (Size_byte > 0U) ? Size_byte : HAL_SDIO_DATA_BLOCK_SIZE_512BYTE;
      config.DataBlockSize = SDMMC_DATABLOCK_SIZE_1B;
    }

    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_CARD;
    /* (HAL_SDIO_MODE_BLOCK << 27) corresponds to the block mode bit of the CMD argument */
    config.TransferMode  = (Argument->Block_Mode == HAL_SDIO_MODE_BLOCK) ? SDMMC_TRANSFER_MODE_BLOCK :
                           SDMMC_TRANSFER_MODE_SDIO;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hsdio->Instance, &config);
    __SDMMC_CMDTRANS_ENABLE(hsdio->Instance);

    /* Correspond to the write or read bit of the CMD argument */
    hsdio->Context = (Argument->Block_Mode == HAL_SDIO_MODE_BLOCK) ? SDIO_CONTEXT_WRITE_MULTIPLE_BLOCK :
                     SDIO_CONTEXT_WRITE_SINGLE_BLOCK;
    cmd = SDIO_WRITE << 31U;
    cmd |= Argument->IOFunctionNbr << 28U;
    cmd |= Argument->Block_Mode << 27U;
    cmd |= Argument->OpCode << 26U;
    cmd |= (Argument->Reg_Addr & 0x1FFFFU) << 9U;
    cmd |= (Size_byte & 0x1FFU);
    errorstate = SDMMC_SDIO_CmdReadWriteExtended(hsdio->Instance, cmd);
    if (errorstate != HAL_SDIO_ERROR_NONE)
    {
      hsdio->ErrorCode |= errorstate;
      if (errorstate != (SDMMC_ERROR_ADDR_OUT_OF_RANGE | SDMMC_ERROR_ILLEGAL_CMD | SDMMC_ERROR_COM_CRC_FAILED |
                         SDMMC_ERROR_GENERAL_UNKNOWN_ERR))
      {
        MODIFY_REG(hsdio->Instance->DCTRL, SDMMC_DCTRL_FIFORST, SDMMC_DCTRL_FIFORST);
        __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_FLAGS);
        __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_DATA_FLAGS);
        hsdio->State = HAL_SDIO_STATE_READY;
        hsdio->Context = SDIO_CONTEXT_NONE;
        return HAL_ERROR;
      }
    }
    /* Write block(s) in polling mode */
    dataremaining = config.DataLength;
    while (!__HAL_SDIO_GET_FLAG(hsdio, SDMMC_FLAG_TXUNDERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT |
                                SDMMC_FLAG_DATAEND))
    {

      if (__HAL_SDIO_GET_FLAG(hsdio, SDMMC_FLAG_TXFIFOHE) && (dataremaining >= SDMMC_FIFO_SIZE))
      {
        /* Read data from SDMMC Rx FIFO */
        for (regCount = 0U; regCount < (SDMMC_FIFO_SIZE / 4U); regCount++)
        {
          hsdio->Instance->FIFO = *u32tempbuff;
          u32tempbuff++;
        }
        dataremaining -= SDMMC_FIFO_SIZE;
      }
      else if ((__HAL_SDIO_GET_FLAG(hsdio, SDMMC_FLAG_TXFIFOHE | SDMMC_FLAG_TXFIFOE)) &&
               (dataremaining < SDMMC_FIFO_SIZE))
      {
        const uint8_t *u8buff = (uint8_t *)u32tempbuff;
        while (dataremaining > 0U)
        {
          data = 0U;
          for (byteCount = 0U; (byteCount < 4U) && (dataremaining > 0U); byteCount++)
          {
            data |= ((uint32_t)(*u8buff) << (byteCount << 3U));
            u8buff++;
            dataremaining--;
          }
          hsdio->Instance->FIFO = data;
        }
      }
      if (((HAL_GetTick() - tickstart) >=  Timeout_Ms))
      {
        /* Clear all the static flags */
        __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_FLAGS);
        hsdio->ErrorCode |= HAL_SDIO_ERROR_TIMEOUT;
        hsdio->State = HAL_SDIO_STATE_READY;
        hsdio->Context = SDIO_CONTEXT_NONE;
        return HAL_TIMEOUT;
      }
    }

    __SDMMC_CMDTRANS_DISABLE(hsdio->Instance);
    /* Get error state */
    if (__HAL_SDIO_GET_FLAG(hsdio, SDMMC_FLAG_DTIMEOUT))
    {
      /* Clear all the static flags */
      __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_FLAGS);
      hsdio->ErrorCode |= HAL_SDIO_ERROR_DATA_TIMEOUT;
      hsdio->State = HAL_SDIO_STATE_READY;
      hsdio->Context = SDIO_CONTEXT_NONE;
      return HAL_ERROR;
    }
    else if (__HAL_SDIO_GET_FLAG(hsdio, SDMMC_FLAG_DCRCFAIL))
    {
      /* Clear all the static flags */
      __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_FLAGS);
      hsdio->ErrorCode |= HAL_SDIO_ERROR_DATA_CRC_FAIL;
      hsdio->State = HAL_SDIO_STATE_READY;
      hsdio->Context = SDIO_CONTEXT_NONE;
      return HAL_ERROR;
    }
    else if (__HAL_SDIO_GET_FLAG(hsdio, SDMMC_FLAG_TXUNDERR))
    {
      /* Clear all the static flags */
      __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_FLAGS);
      /* (SDIO_WRITE << 31) correspond to the write or read bit of the CMD argument */
      hsdio->ErrorCode |= HAL_SDIO_ERROR_TX_UNDERRUN;
      hsdio->State = HAL_SDIO_STATE_READY;
      hsdio->Context = SDIO_CONTEXT_NONE;
      return HAL_ERROR;
    }
    else if (hsdio->ErrorCode == SDMMC_ERROR_INVALID_PARAMETER)
    {
      __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_DATA_FLAGS);
      hsdio->State = HAL_SDIO_STATE_READY;
      hsdio->Context = SDIO_CONTEXT_NONE;
      return HAL_ERROR;
    }
    else
    {
      /* Nothing to do */
    }

    /* Clear all the static flags */
    __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_DATA_FLAGS);

    hsdio->State = HAL_SDIO_STATE_READY;
  }
  else
  {
    return HAL_BUSY;
  }

  return HAL_OK;
}

/**
  * @brief  Read data from a specified address using extended mode through cmd53 in DMA mode.
  * @param  hsdio: Pointer to SDIO handle
  * @param  Argument: Pointer to SDIO argument
  * @param  pData:  pointer to the buffer that will contain the data to transmit
  * @param  Size_byte:  Block size to write.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_ReadExtended_DMA(SDIO_HandleTypeDef *hsdio, const HAL_SDIO_ExtendedCmd_TypeDef *Argument,
                                            uint8_t *pData, uint32_t Size_byte)
{
  SDMMC_DataInitTypeDef config;
  uint32_t errorstate;
  uint8_t *p_dma_buffer;
  uint32_t cmd;
  uint32_t nbr_of_block;

  /* Check the parameters */
  assert_param(hsdio != NULL);
  assert_param(Argument != NULL);
  assert_param(pData != NULL);

  if ((hsdio == NULL) || (Argument == NULL) || (pData == NULL))
  {
    return HAL_ERROR;
  }

  if (hsdio->State == HAL_SDIO_STATE_READY)
  {
    hsdio->ErrorCode = HAL_SDIO_ERROR_NONE;
    hsdio->State = HAL_SDIO_STATE_BUSY;

    /* Initialize data control register */
    if ((hsdio->Instance->DCTRL & SDMMC_DCTRL_SDIOEN) != 0U)
    {
      hsdio->Instance->DCTRL = SDMMC_DCTRL_SDIOEN;
    }
    else
    {
      hsdio->Instance->DCTRL = 0U;
    }

    p_dma_buffer = (uint8_t *)pData;
    hsdio->pRxBuffPtr = (uint8_t *)pData;
    hsdio->RxXferSize = Size_byte;
    hsdio->next_data_addr = (uint32_t)pData;

    /* Compute how many blocks are to be send for pData of length data_size to be send */
    nbr_of_block = (Size_byte & ~(hsdio->block_size & 1U)) >> __CLZ(__RBIT(hsdio->block_size));

    if (nbr_of_block != 0U)
    {
      hsdio->remaining_data = (Size_byte - (hsdio->block_size * nbr_of_block));
      hsdio->next_reg_addr = (Argument->Reg_Addr) | ((((nbr_of_block * hsdio->block_size) >> 1U) & 0x3FFFU) << 1U)
                             | ((hsdio->remaining_data <= HAL_SDIO_DATA_BLOCK_SIZE_512BYTE) ? 1U : 0U);
      hsdio->next_data_addr += (nbr_of_block * hsdio->block_size);
    }
    else
    {
      hsdio->next_data_addr += (Size_byte < HAL_SDIO_DATA_BLOCK_SIZE_512BYTE) ? Size_byte :
                               HAL_SDIO_DATA_BLOCK_SIZE_512BYTE;
      if (hsdio->remaining_data != 0U)
      {
        hsdio->remaining_data = (Size_byte >= HAL_SDIO_DATA_BLOCK_SIZE_512BYTE) ?
                                (Size_byte - HAL_SDIO_DATA_BLOCK_SIZE_512BYTE) :
                                (Size_byte - hsdio->remaining_data);
        hsdio->next_reg_addr += (Size_byte >= HAL_SDIO_DATA_BLOCK_SIZE_512BYTE) ? \
                                (HAL_SDIO_DATA_BLOCK_SIZE_512BYTE + 1U) : (Size_byte + 1U);
      }
    }

    /* DMA configuration (use single buffer) */
    hsdio->Instance->IDMACTRL  = SDMMC_ENABLE_IDMA_SINGLE_BUFF;
    hsdio->Instance->IDMABASER = (uint32_t)p_dma_buffer;

    /* Configure the SD DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    if (Argument->Block_Mode == HAL_SDIO_MODE_BLOCK)
    {
      config.DataLength    = (uint32_t)(nbr_of_block * hsdio->block_size);
      config.DataBlockSize = SDIO_Convert_Block_Size(hsdio, hsdio->block_size);
    }
    else
    {
      config.DataLength    = (Size_byte > 0U) ? Size_byte : HAL_SDIO_DATA_BLOCK_SIZE_512BYTE;
      config.DataBlockSize = SDMMC_DATABLOCK_SIZE_1B;
    }

    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_SDMMC ;
    config.TransferMode  = (Argument->Block_Mode == HAL_SDIO_MODE_BLOCK) ? SDMMC_TRANSFER_MODE_BLOCK :
                           SDMMC_TRANSFER_MODE_SDIO;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hsdio->Instance, &config);

    __SDMMC_CMDTRANS_ENABLE(hsdio->Instance);

    /* Read */
    hsdio->Context = (uint32_t)((Argument->Block_Mode == HAL_SDIO_MODE_BLOCK) ? SDIO_CONTEXT_READ_MULTIPLE_BLOCK :
                                SDIO_CONTEXT_READ_SINGLE_BLOCK) | SDIO_CONTEXT_DMA;

    cmd = SDIO_READ << 31U;
    cmd |= Argument->IOFunctionNbr << 28U;
    cmd |= Argument->Block_Mode << 27U;
    cmd |= Argument->OpCode << 26U;
    cmd |= (Argument->Reg_Addr & 0x1FFFFU) << 9U;
    cmd |= ((nbr_of_block == 0U) ? Size_byte :  nbr_of_block) & 0x1FFU;
    errorstate = SDMMC_SDIO_CmdReadWriteExtended(hsdio->Instance, cmd);
    if (errorstate != HAL_SDIO_ERROR_NONE)
    {
      hsdio->ErrorCode |= errorstate;
      if (errorstate != (SDMMC_ERROR_ADDR_OUT_OF_RANGE | SDMMC_ERROR_ILLEGAL_CMD | SDMMC_ERROR_COM_CRC_FAILED |
                         SDMMC_ERROR_GENERAL_UNKNOWN_ERR))
      {
        MODIFY_REG(hsdio->Instance->DCTRL, SDMMC_DCTRL_FIFORST, SDMMC_DCTRL_FIFORST);
        __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_FLAGS);
        __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_DATA_FLAGS);
        hsdio->State = HAL_SDIO_STATE_READY;
        hsdio->Context = SDIO_CONTEXT_NONE;
        return HAL_ERROR;
      }
    }
    /* Enable interrupt */
    __HAL_SDIO_ENABLE_IT(hsdio, (SDMMC_IT_DCRCFAIL | SDMMC_IT_DTIMEOUT | SDMMC_IT_RXOVERR | SDMMC_IT_DATAEND));
  }
  else
  {
    return HAL_BUSY;
  }

  return HAL_OK;
}

/**
  * @brief  Write data from a specified address using extended mode through cmd53 in DMA mode.
  * @param  hsdio: Pointer to SDIO handle
  * @param  Argument: Pointer to SDIO argument
  * @param  pData:  pointer to the buffer that will contain the data to transmit
  * @param  Size_byte:  Block size to write.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_WriteExtended_DMA(SDIO_HandleTypeDef *hsdio, const HAL_SDIO_ExtendedCmd_TypeDef *Argument,
                                             uint8_t *pData, uint32_t Size_byte)
{
  uint32_t cmd;
  SDMMC_DataInitTypeDef config;
  uint32_t errorstate;
  uint8_t *p_dma_buffer;
  uint32_t nbr_of_block;

  /* Check the parameters */
  assert_param(hsdio != NULL);
  assert_param(Argument != NULL);
  assert_param(pData != NULL);

  if ((hsdio == NULL) || (Argument == NULL) || (pData == NULL))
  {
    return HAL_ERROR;
  }

  if (hsdio->State == HAL_SDIO_STATE_READY)
  {
    hsdio->ErrorCode = HAL_SDIO_ERROR_NONE;
    hsdio->State = HAL_SDIO_STATE_BUSY;

    /* Initialize data control register */
    if ((hsdio->Instance->DCTRL & SDMMC_DCTRL_SDIOEN) != 0U)
    {
      hsdio->Instance->DCTRL = SDMMC_DCTRL_SDIOEN;
    }
    else
    {
      hsdio->Instance->DCTRL = 0U;
    }

    p_dma_buffer = (uint8_t *)pData;
    hsdio->pTxBuffPtr = (uint8_t *)pData;
    hsdio->TxXferSize = Size_byte;
    hsdio->next_data_addr = (uint32_t)pData;

    nbr_of_block = (Size_byte & ~(hsdio->block_size & 1U)) >> __CLZ(__RBIT(hsdio->block_size));

    if (nbr_of_block != 0U)
    {
      hsdio->remaining_data = (Size_byte - (hsdio->block_size * nbr_of_block));
      if (hsdio->block_size <= 128U)
      {
        hsdio->next_reg_addr = (Argument->Reg_Addr) |
                               ((((nbr_of_block * hsdio->block_size) >> 1U) & 0x3FFFU) << 1U) |
                               ((hsdio->remaining_data <= HAL_SDIO_DATA_BLOCK_SIZE_512BYTE) ? 1U : 0U);
      }
      else
      {
        hsdio->next_reg_addr = (nbr_of_block * hsdio->block_size) >> 1U;
      }
      hsdio->next_data_addr += (nbr_of_block * hsdio->block_size);
    }
    else
    {
      hsdio->remaining_data = (Size_byte >= HAL_SDIO_DATA_BLOCK_SIZE_512BYTE) ?
                              (Size_byte - HAL_SDIO_DATA_BLOCK_SIZE_512BYTE) :
                              (Size_byte - hsdio->remaining_data);
      if (hsdio->remaining_data != 0U)
      {
        hsdio->remaining_data = (Size_byte >= HAL_SDIO_DATA_BLOCK_SIZE_512BYTE) ?
                                (Size_byte - HAL_SDIO_DATA_BLOCK_SIZE_512BYTE) :
                                (Size_byte - hsdio->remaining_data);
        hsdio->next_reg_addr += ((Size_byte >= HAL_SDIO_DATA_BLOCK_SIZE_512BYTE) ? \
                                 (HAL_SDIO_DATA_BLOCK_SIZE_512BYTE >> 1U) : (Size_byte >> 1U)) |
                                (((hsdio->remaining_data > 0U) ? 0U : 1U));
      }
      hsdio->next_data_addr += (Size_byte < HAL_SDIO_DATA_BLOCK_SIZE_512BYTE) ? Size_byte :
                               HAL_SDIO_DATA_BLOCK_SIZE_512BYTE;
    }

    /* DMA configuration (use single buffer) */
    hsdio->Instance->IDMACTRL  = SDMMC_ENABLE_IDMA_SINGLE_BUFF;
    hsdio->Instance->IDMABASER = (uint32_t)p_dma_buffer;

    /* Configure the SDIO DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    if (Argument->Block_Mode == HAL_SDIO_MODE_BLOCK)
    {
      config.DataLength    = (uint32_t)(nbr_of_block * hsdio->block_size);
      config.DataBlockSize = SDIO_Convert_Block_Size(hsdio, hsdio->block_size);
    }
    else
    {
      config.DataLength = (Size_byte > HAL_SDIO_DATA_BLOCK_SIZE_512BYTE) ? HAL_SDIO_DATA_BLOCK_SIZE_512BYTE : Size_byte;
      config.DataBlockSize = SDMMC_DATABLOCK_SIZE_1B;
    }

    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_CARD;
    config.TransferMode  = (Argument->Block_Mode == HAL_SDIO_MODE_BLOCK) ? SDMMC_TRANSFER_MODE_BLOCK
                           : SDMMC_TRANSFER_MODE_SDIO;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hsdio->Instance, &config);

    __SDMMC_CMDTRANS_ENABLE(hsdio->Instance);

    /* Write */
    hsdio->Context = (uint32_t)((Argument->Block_Mode == HAL_SDIO_MODE_BLOCK) ?
                                SDIO_CONTEXT_WRITE_MULTIPLE_BLOCK :
                                SDIO_CONTEXT_WRITE_SINGLE_BLOCK) | SDIO_CONTEXT_DMA;
    cmd = SDIO_WRITE << 31U;
    cmd |= Argument->IOFunctionNbr << 28U;
    cmd |= Argument->Block_Mode << 27U;
    cmd |= Argument->OpCode << 26U;
    cmd |= (Argument->Reg_Addr & 0x1FFFFU) << 9U;
    cmd |= ((nbr_of_block == 0U) ? ((Size_byte > HAL_SDIO_DATA_BLOCK_SIZE_512BYTE) ?
                                    HAL_SDIO_DATA_BLOCK_SIZE_512BYTE : Size_byte) : nbr_of_block) & 0x1FFU;
    errorstate = SDMMC_SDIO_CmdReadWriteExtended(hsdio->Instance, cmd);
    if (errorstate != HAL_SDIO_ERROR_NONE)
    {
      hsdio->ErrorCode |= errorstate;
      if (errorstate != (SDMMC_ERROR_ADDR_OUT_OF_RANGE | SDMMC_ERROR_ILLEGAL_CMD | SDMMC_ERROR_COM_CRC_FAILED |
                         SDMMC_ERROR_GENERAL_UNKNOWN_ERR))
      {
        MODIFY_REG(hsdio->Instance->DCTRL, SDMMC_DCTRL_FIFORST, SDMMC_DCTRL_FIFORST);
        __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_FLAGS);
        __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_DATA_FLAGS);
        hsdio->State = HAL_SDIO_STATE_READY;
        hsdio->Context = SDIO_CONTEXT_NONE;
        return HAL_ERROR;
      }
    }
    /* Enable interrupt */
    __HAL_SDIO_ENABLE_IT(hsdio, (SDMMC_IT_DCRCFAIL | SDMMC_IT_DTIMEOUT | SDMMC_IT_TXUNDERR | SDMMC_IT_DATAEND));
  }
  else
  {
    return HAL_BUSY;
  }

  return HAL_OK;
}

/**
  * @}
  */

/** @addtogroup SDIO_Exported_Functions_Group4
  *  @brief
  *
@verbatim
  ==============================================================================
          ##### IO operation functions #####
  ==============================================================================
  [..]
    This subsection provides a set callback functions allowing to manage the data transfer from/to SDIO card.

@endverbatim
  * @{
  */
/**
  * @brief  This function handles SDIO device interrupt request.
  * @param  hsdio: Pointer to SDIO handle
  * @retval None
  */
void HAL_SDIO_IRQHandler(SDIO_HandleTypeDef *hsdio)
{
  HAL_SDIO_ExtendedCmd_TypeDef CMD53_desc;
  HAL_StatusTypeDef errorstate;
  uint32_t ctx = hsdio->Context;
  uint32_t flags;

  flags   = READ_REG(((SDMMC_TypeDef *)((uint32_t)(hsdio)->Instance))->STA);

  if (READ_BIT(flags, SDMMC_FLAG_SDIOIT) != 0U)
  {
    (void)SDIO_IOFunction_IRQHandler(hsdio);
  }

  if (READ_BIT(flags, SDMMC_FLAG_DATAEND) != 0U)
  {
    __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_FLAG_DATAEND);

    hsdio->State = HAL_SDIO_STATE_READY;

    __HAL_SDIO_DISABLE_IT(hsdio, SDMMC_IT_DATAEND  | SDMMC_IT_DCRCFAIL | SDMMC_IT_DTIMEOUT | SDMMC_IT_TXUNDERR |
                          SDMMC_IT_RXOVERR  | SDMMC_IT_TXFIFOHE | SDMMC_IT_RXFIFOHF);

    __HAL_SDIO_DISABLE_IT(hsdio, SDMMC_IT_IDMABTC);
    __SDMMC_CMDTRANS_DISABLE(hsdio->Instance);

    if ((ctx & SDIO_CONTEXT_DMA) != 0U)
    {
      hsdio->Instance->DLEN = 0;
      hsdio->Instance->IDMACTRL = SDMMC_DISABLE_IDMA;
      if ((hsdio->Instance->DCTRL & SDMMC_DCTRL_SDIOEN) != 0U)
      {
        hsdio->Instance->DCTRL = SDMMC_DCTRL_SDIOEN;
      }
      else
      {
        hsdio->Instance->DCTRL = 0U;
      }

      hsdio->Context = SDIO_CONTEXT_NONE;
      hsdio->State = HAL_SDIO_STATE_READY;
    }

    if (hsdio->remaining_data != 0U)
    {
      CMD53_desc.Block_Mode = HAL_SDIO_MODE_BYTE;
      CMD53_desc.Reg_Addr = hsdio->next_reg_addr;
      CMD53_desc.IOFunctionNbr = 1;
      CMD53_desc.OpCode = 1;
      if (((ctx & SDIO_CONTEXT_READ_SINGLE_BLOCK) != 0U) || ((ctx & SDIO_CONTEXT_READ_MULTIPLE_BLOCK) != 0U))
      {
        hsdio->pRxBuffPtr = (uint8_t *)hsdio->next_data_addr;
        errorstate =  HAL_SDIO_ReadExtended_DMA(hsdio, &CMD53_desc, hsdio->pRxBuffPtr, hsdio->remaining_data);
      }
      else
      {
        hsdio->pTxBuffPtr = (uint8_t *)hsdio->next_data_addr;
        errorstate =  HAL_SDIO_WriteExtended_DMA(hsdio, &CMD53_desc, hsdio->pTxBuffPtr, hsdio->remaining_data);
      }
      if (errorstate != HAL_OK)
      {
#if defined (USE_HAL_SDIO_REGISTER_CALLBACKS) && (USE_HAL_SDIO_REGISTER_CALLBACKS == 1)
        hsdio->ErrorCallback(hsdio);
#else
        HAL_SDIO_ErrorCallback(hsdio);
#endif /* USE_HAL_SDIO_REGISTER_CALLBACKS */
      }
    }
    else if (((ctx & SDIO_CONTEXT_READ_SINGLE_BLOCK) != 0U) || ((ctx & SDIO_CONTEXT_READ_MULTIPLE_BLOCK) != 0U))
    {
#if defined (USE_HAL_SDIO_REGISTER_CALLBACKS) && (USE_HAL_SDIO_REGISTER_CALLBACKS == 1U)
      hsdio->RxCpltCallback(hsdio);
#else
      HAL_SDIO_RxCpltCallback(hsdio);
#endif /* USE_HAL_SDIO_REGISTER_CALLBACKS */
    }
    else
    {
#if defined (USE_HAL_SDIO_REGISTER_CALLBACKS) && (USE_HAL_SDIO_REGISTER_CALLBACKS == 1U)
      hsdio->TxCpltCallback(hsdio);
#else
      HAL_SDIO_TxCpltCallback(hsdio);
#endif /* USE_HAL_SDIO_REGISTER_CALLBACKS */
    }
  }

  if (__HAL_SDIO_GET_FLAG(hsdio, SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_RXOVERR | SDMMC_FLAG_TXUNDERR))
  {
#if defined (USE_HAL_SDIO_REGISTER_CALLBACKS) && (USE_HAL_SDIO_REGISTER_CALLBACKS == 1)
    hsdio->ErrorCallback(hsdio);
#else
    HAL_SDIO_ErrorCallback(hsdio);
#endif /* USE_HAL_SDIO_REGISTER_CALLBACKS */
  }
}

/**
  * @brief Tx Transfer completed callbacks
  * @param hsdio: Pointer to SDIO handle
  * @retval None
  */
__weak void HAL_SDIO_TxCpltCallback(SDIO_HandleTypeDef *hsdio)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsdio);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SDIO_TxCpltCallback can be implemented in the user file
   */
}

/**
  * @brief Rx Transfer completed callbacks
  * @param hsdio: Pointer SDIO handle
  * @retval None
  */
__weak void HAL_SDIO_RxCpltCallback(SDIO_HandleTypeDef *hsdio)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsdio);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SDIO_RxCpltCallback can be implemented in the user file
   */
}

/**
  * @brief SDIO error callbacks
  * @param hsdio: Pointer SDIO handle
  * @retval None
  */
__weak void HAL_SDIO_ErrorCallback(SDIO_HandleTypeDef *hsdio)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsdio);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SDIO_ErrorCallback can be implemented in the user file
   */
}

/**
  * @brief SDIO IO Function complete callback
  * @param hsdio: Pointer SDIO handle
  * @param func: SDIO IO Function
  * @retval None
  */
__weak void HAL_SDIO_IOFunctionCallback(SDIO_HandleTypeDef *hsdio, uint32_t func)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsdio);
  UNUSED(func);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SDIO_ErrorCallback can be implemented in the user file
   */
}

#if (USE_SDIO_TRANSCEIVER != 0U)
/**
  * @brief  Enable/Disable the SDIO Transceiver 1.8V Mode Callback.
  * @param  hsdio: Pointer SDIO handle
  * @param  status: Voltage Switch State
  * @retval None
  */
__weak  void HAL_SDIO_DriveTransceiver_1_8V_Callback(SDIO_HandleTypeDef *hsdio, FlagStatus status)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsdio);
  UNUSED(status);
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SDIO_EnableTransceiver could be implemented in the user file
   */
}
#endif /* USE_SDIO_TRANSCEIVER  */

#if defined (USE_HAL_SDIO_REGISTER_CALLBACKS) && (USE_HAL_SDIO_REGISTER_CALLBACKS == 1U)
/**
  * @brief  Register a User SDIO Callback
  *         To be used instead of the weak (overridden) predefined callback
  * @param hsdio : SDIO handle
  * @param CallbackID : ID of the callback to be registered
  *        This parameter can be one of the following values:
  *          @arg @ref HAL_SDIO_TX_CPLT_CB_ID                 SDIO Tx Complete Callback ID
  *          @arg @ref HAL_SDIO_RX_CPLT_CB_ID                 SDIO Rx Complete Callback ID
  *          @arg @ref HAL_SDIO_ERROR_CB_ID                   SDIO Error Callback ID
  *          @arg @ref HAL_SDIO_MSP_INIT_CB_ID                SDIO MspInit Callback ID
  *          @arg @ref HAL_SDIO_MSP_DEINIT_CB_ID              SDIO MspDeInit Callback ID
  * @param pCallback : pointer to the Callback function
  * @retval status
  */
HAL_StatusTypeDef HAL_SDIO_RegisterCallback(SDIO_HandleTypeDef *hsdio, HAL_SDIO_CallbackIDTypeDef CallbackID,
                                            pSDIO_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param(hsdio != NULL);
  assert_param(pCallback != NULL);

  if (pCallback == NULL)
  {
    /* Update the error code */
    hsdio->ErrorCode |= HAL_SDIO_ERROR_INVALID_CALLBACK;
    return HAL_ERROR;
  }

  if (hsdio->State == HAL_SDIO_STATE_READY)
  {
    switch (CallbackID)
    {
      case HAL_SDIO_TX_CPLT_CB_ID :
        hsdio->TxCpltCallback = pCallback;
        break;
      case HAL_SDIO_RX_CPLT_CB_ID :
        hsdio->RxCpltCallback = pCallback;
        break;
      case HAL_SDIO_ERROR_CB_ID :
        hsdio->ErrorCallback = pCallback;
        break;
      case HAL_SDIO_MSP_INIT_CB_ID :
        hsdio->MspInitCallback = pCallback;
        break;
      case HAL_SDIO_MSP_DEINIT_CB_ID :
        hsdio->MspDeInitCallback = pCallback;
        break;
      default :
        /* Update the error code */
        hsdio->ErrorCode |= HAL_SDIO_ERROR_INVALID_CALLBACK;
        /* update return status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (hsdio->State == HAL_SDIO_STATE_RESET)
  {
    switch (CallbackID)
    {
      case HAL_SDIO_MSP_INIT_CB_ID :
        hsdio->MspInitCallback = pCallback;
        break;
      case HAL_SDIO_MSP_DEINIT_CB_ID :
        hsdio->MspDeInitCallback = pCallback;
        break;
      default :
        /* Update the error code */
        hsdio->ErrorCode |= HAL_SDIO_ERROR_INVALID_CALLBACK;
        /* update return status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hsdio->ErrorCode |= HAL_SDIO_ERROR_INVALID_CALLBACK;
    /* update return status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Unregister a User SDIO Callback
  *         SDIO Callback is redirected to the weak (overridden) predefined callback.
  * @note   The HAL_SDIO_UnRegisterCallback() may be called before HAL_SDIO_Init() in
  *         HAL_SDIO_STATE_RESET to register callbacks for HAL_SDIO_MSP_INIT_CB_ID
  *         and HAL_SDIO_MSP_DEINIT_CB_ID.
  * @param hsdio : SDIO handle
  * @param CallbackID : ID of the callback to be unregistered
  *        This parameter can be one of the following values @ref SDIO_Exported_Types_Group3.
  * @retval status
  */
HAL_StatusTypeDef HAL_SDIO_UnRegisterCallback(SDIO_HandleTypeDef *hsdio, HAL_SDIO_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  assert_param(hsdio != NULL);

  /* Check the SDIO peripheral handle parameter */
  if (hsdio == NULL)
  {
    return HAL_ERROR;
  }

  if (hsdio->State == HAL_SDIO_STATE_READY)
  {
    switch (CallbackID)
    {
      case HAL_SDIO_TX_CPLT_CB_ID :
        hsdio->TxCpltCallback = HAL_SDIO_TxCpltCallback;
        break;
      case HAL_SDIO_RX_CPLT_CB_ID :
        hsdio->RxCpltCallback = HAL_SDIO_RxCpltCallback;
        break;
      case HAL_SDIO_ERROR_CB_ID :
        hsdio->ErrorCallback = HAL_SDIO_ErrorCallback;
        break;
      case HAL_SDIO_MSP_INIT_CB_ID :
        hsdio->MspInitCallback = HAL_SDIO_MspInit;
        break;
      case HAL_SDIO_MSP_DEINIT_CB_ID :
        hsdio->MspDeInitCallback = HAL_SDIO_MspDeInit;
        break;
      default :
        hsdio->ErrorCode |= HAL_SDIO_ERROR_INVALID_CALLBACK;
        status =  HAL_ERROR;
        break;
    }
  }
  else if (hsdio->State == HAL_SDIO_STATE_RESET)
  {
    switch (CallbackID)
    {
      case HAL_SDIO_MSP_INIT_CB_ID :
        hsdio->MspInitCallback = HAL_SDIO_MspInit;
        break;
      case HAL_SDIO_MSP_DEINIT_CB_ID :
        hsdio->MspDeInitCallback = HAL_SDIO_MspDeInit;
        break;
      default :
        hsdio->ErrorCode |= HAL_SDIO_ERROR_INVALID_CALLBACK;
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    hsdio->ErrorCode |= HAL_SDIO_ERROR_INVALID_CALLBACK;
    status =  HAL_ERROR;
  }

  return status;
}
#endif /* USE_HAL_SDIO_REGISTER_CALLBACKS */

#if (USE_SDIO_TRANSCEIVER != 0U)
/**
  * @brief  Register a User SDIO Transceiver Callback
  *         To be used instead of the weak (overridden) predefined callback
  * @param hsdio : SDIO handle
  * @param pCallback : pointer to the Callback function
  * @retval status
  */
HAL_StatusTypeDef HAL_SDIO_RegisterTransceiverCallback(SDIO_HandleTypeDef *hsdio,
                                                       pSDIO_TransceiverCallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hsdio->ErrorCode |= HAL_SDIO_ERROR_INVALID_CALLBACK;
    return HAL_ERROR;
  }

  if (hsdio->State == HAL_SDIO_STATE_READY)
  {
    hsdio->DriveTransceiver_1_8V_Callback = pCallback;
  }
  else
  {
    /* Update the error code */
    hsdio->ErrorCode |= HAL_SDIO_ERROR_INVALID_CALLBACK;
    /* update return status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Unregister a User SDIO Transceiver Callback
  *         SDIO Callback is redirected to the weak (overridden) predefined callback
  * @param hsdio : SDIO handle
  * @retval status
  */
HAL_StatusTypeDef HAL_SDIO_UnRegisterTransceiverCallback(SDIO_HandleTypeDef *hsdio)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hsdio->State == HAL_SDIO_STATE_READY)
  {
    hsdio->DriveTransceiver_1_8V_Callback = HAL_SDIO_DriveTransceiver_1_8V_Callback;
  }
  else
  {
    /* Update the error code */
    hsdio->ErrorCode |= HAL_SDIO_ERROR_INVALID_CALLBACK;
    /* update return status */
    status =  HAL_ERROR;
  }

  return status;
}
#endif /* USE_SDIO_TRANSCEIVER */

/**
  * @brief Register a User SDIO Identification Callback
  * @param hsdio: Pointer to SDIO handle
  * @param pCallback: pointer to the Callback function
  * @retval status
  */
HAL_StatusTypeDef HAL_SDIO_RegisterIdentifyCardCallback(SDIO_HandleTypeDef *hsdio,
                                                        pSDIO_IdentifyCardCallbackTypeDef pCallback)
{
  /* Check the parameters */
  assert_param(hsdio != NULL);
  assert_param(pCallback != NULL);

  if (pCallback == NULL)
  {
    /* Update the error code */
    hsdio->ErrorCode |= HAL_SDIO_ERROR_INVALID_CALLBACK;
    return HAL_ERROR;
  }

  hsdio->SDIO_IdentifyCard = pCallback;

  return HAL_OK;
}
/**
  * @}
  */

/** @addtogroup SDIO_Exported_Functions_Group5
  *  @brief
  *
@verbatim
  ==============================================================================
          ##### Peripheral State and Errors functions #####
  ==============================================================================
  [..]
    This subsection provides a set of functions allowing to control the SDIO card operations.

@endverbatim
  * @{
  */
/**
  * @brief return the SDIO state
  * @param hsdio: Pointer to SDIO handle
  * @retval HAL state
  */
HAL_SDIO_StateTypeDef HAL_SDIO_GetState(const SDIO_HandleTypeDef *hsdio)
{
  return hsdio->State;
}

/**
  * @brief  Return the SDIO error code
  * @param  hsdio : Pointer to a SDIO_HandleTypeDef structure that contains the configuration information.
  * @retval SDIO Error Code
  */
uint32_t HAL_SDIO_GetError(const SDIO_HandleTypeDef *hsdio)
{
  return hsdio->ErrorCode;
}

/**
  * @}
  */

/** @addtogroup SDIO_Exported_Functions_Group6
  *  @brief
  *
@verbatim
  ==============================================================================
          ##### Peripheral IO interrupt #####
  ==============================================================================
  [..]
    This subsection provides a set functions allowing to enable/disable IO functions interrupt features
    on the SDIO card.

@endverbatim
  * @{
  */
/**
  * @brief  Enable SDIO IO interrupt.
  * @param  hsdio: Pointer to SDIO handle
  * @param  IOFunction: Specifies the SDIO IO function.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_EnableIOFunctionInterrupt(SDIO_HandleTypeDef *hsdio, uint32_t IOFunction)
{
  uint8_t intEn  = 0U;

  /* Check the parameters */
  assert_param(hsdio != NULL);
  assert_param(IS_SDIO_FUNCTION(IOFunction));

  /* Check the SDIO peripheral handle parameter */
  if (hsdio == NULL)
  {
    return HAL_ERROR;
  }

  if (SDIO_ReadDirect(hsdio, SDMMC_SDIO_CCCR4, HAL_SDIO_WRITE_ONLY, SDIO_FUNCTION_0, &intEn) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* if already enable , do not need enable again */
  if ((((intEn >> (uint32_t)IOFunction) & 0x01U) == 0x01U) && ((intEn & 0x01U) != 0U))
  {
    return HAL_OK;
  }
  else
  {
    intEn |= (1U << (uint32_t)IOFunction) | 0x01U;
    hsdio->IOInterruptNbr++;
  }

  if (SDIO_WriteDirect(hsdio, SDMMC_SDIO_CCCR4, HAL_SDIO_WRITE_ONLY, SDIO_FUNCTION_0,
                       &intEn) != HAL_OK)
  {
    return HAL_ERROR;
  }

  __HAL_SDIO_ENABLE_IT(hsdio, SDMMC_IT_SDIOIT);

  /* Enable host SDIO interrupt operations */
  __SDMMC_OPERATION_ENABLE(hsdio->Instance);

  return HAL_OK;
}

/**
  * @brief  Enable SDIO IO interrupt.
  * @param  hsdio: Pointer to SDIO handle
  * @param  IOFunction: Specifies the SDIO IO function.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_DisableIOFunctionInterrupt(SDIO_HandleTypeDef *hsdio, uint32_t IOFunction)
{
  uint8_t intEn  = 0U;

  /* Check the parameters */
  assert_param(hsdio != NULL);
  assert_param(IS_SDIO_FUNCTION(IOFunction));

  /* Check the SDIO peripheral handle parameter */
  if (hsdio == NULL)
  {
    return HAL_ERROR;
  }

  if (SDIO_ReadDirect(hsdio, SDMMC_SDIO_CCCR4, HAL_SDIO_WRITE_ONLY, SDIO_FUNCTION_0, &intEn) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* if already disable , do not need enable again */
  if (((intEn >> (uint32_t)IOFunction) & 0x01U) == 0x00U)
  {
    return HAL_OK;
  }
  else
  {
    /* disable the interrupt, don't disable the interrupt master here */
    intEn &= ~(1U << (uint32_t)IOFunction);
  }

  if (SDIO_WriteDirect(hsdio, SDMMC_SDIO_CCCR4, HAL_SDIO_READ_AFTER_WRITE, SDIO_FUNCTION_0,
                       &intEn) != HAL_OK)
  {
    return HAL_ERROR;
  }

  if (hsdio->IOInterruptNbr > 1U)
  {
    hsdio->IOInterruptNbr--;
  }
  else
  {
    hsdio->IOInterruptNbr = 0U;
    __HAL_SDIO_DISABLE_IT(hsdio, SDMMC_IT_SDIOIT);
  }
  return HAL_OK;
}

/**
  * @brief  Enable SDIO IO Enable.
  * @param  hsdio: Pointer to SDIO handle
  * @param  IOFunction: Specifies the SDIO IO function.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_EnableIOFunction(SDIO_HandleTypeDef *hsdio, uint32_t IOFunction)
{
  uint8_t ioEn  = 0U;
  uint8_t ioReady = 0U;

  /* Check the parameters */
  assert_param(hsdio != NULL);
  assert_param(IS_SDIO_FUNCTION(IOFunction));

  /* Check the SDIO peripheral handle parameter */
  if (hsdio == NULL)
  {
    return HAL_ERROR;
  }

  if (SDIO_ReadDirect(hsdio, SDMMC_SDIO_CCCR0_SD_BYTE2, HAL_SDIO_WRITE_ONLY, SDIO_FUNCTION_0, &ioEn) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* if already enable , do not need to enable again */
  if (((ioEn >> (uint32_t)IOFunction) & 0x01U) == 0x01U)
  {
    return HAL_OK;
  }
  else
  {
    ioEn |= (1U << (uint32_t)IOFunction);
  }

  if (SDIO_WriteDirect(hsdio, SDMMC_SDIO_CCCR0_SD_BYTE2, HAL_SDIO_READ_AFTER_WRITE, SDIO_FUNCTION_0, &ioEn) != HAL_OK)
  {
    return HAL_ERROR;
  }

  if (SDIO_ReadDirect(hsdio, SDMMC_SDIO_CCCR0_SD_BYTE3, HAL_SDIO_WRITE_ONLY, SDIO_FUNCTION_0, &ioReady) != HAL_OK)
  {
    return HAL_ERROR;
  }
  /* check if IO ready */
  if ((ioReady & (1U << (uint32_t)IOFunction)) != 0U)
  {
    return HAL_OK;
  }

  return HAL_ERROR;
}

/**
  * @brief  Disable SDIO IO Enable.
  * @param  hsdio: Pointer to SDIO handle
  * @param  IOFunction: Specifies the SDIO IO function.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_DisableIOFunction(SDIO_HandleTypeDef *hsdio, uint32_t IOFunction)
{
  uint8_t ioEn  = 0U;

  /* Check the parameters */
  assert_param(hsdio != NULL);
  assert_param(IS_SDIO_FUNCTION(IOFunction));

  /* Check the SDIO peripheral handle parameter */
  if (hsdio == NULL)
  {
    return HAL_ERROR;
  }

  if (SDIO_ReadDirect(hsdio, SDMMC_SDIO_CCCR0_SD_BYTE2, HAL_SDIO_WRITE_ONLY, SDIO_FUNCTION_0, &ioEn) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* if already enable , do not need enable again */
  if (((ioEn >> (uint32_t)IOFunction) & 0x01U) == 0x00U)
  {
    return HAL_OK;
  }
  else
  {
    ioEn &= ~(1U << (uint32_t)IOFunction);
  }

  if (SDIO_WriteDirect(hsdio, SDMMC_SDIO_CCCR0_SD_BYTE2, HAL_SDIO_READ_AFTER_WRITE, SDIO_FUNCTION_0, &ioEn) != HAL_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Select SDIO IO Enable.
  * @param  hsdio: Pointer to SDIO handle
  * @param  IOFunction: Specifies the SDIO IO function.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_SelectIOFunction(SDIO_HandleTypeDef *hsdio, uint32_t IOFunction)
{
  /* Check the parameters */
  assert_param(hsdio != NULL);
  assert_param(IS_SDIO_FUNCTION(IOFunction));

  /* Check the SDIO peripheral handle parameter */
  if (hsdio == NULL)
  {
    return HAL_ERROR;
  }

  if (SDIO_WriteDirect(hsdio, SDMMC_SDIO_CCCR12_SD_BYTE1, HAL_SDIO_READ_AFTER_WRITE, SDIO_FUNCTION_0,
                       (uint8_t *)&IOFunction) != HAL_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Abort IO transfer.
  * @param  hsdio:  Pointer to SDIO handle
  * @param IOFunction IO number
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_AbortIOFunction(SDIO_HandleTypeDef *hsdio, uint32_t IOFunction)
{
  /* Check the parameters */
  assert_param(hsdio != NULL);
  assert_param(IS_SDIO_FUNCTION(IOFunction));

  /* Check the SDIO peripheral handle parameter */
  if (hsdio == NULL)
  {
    return HAL_ERROR;
  }

  if (SDIO_WriteDirect(hsdio, SDMMC_SDIO_CCCR4_SD_BYTE2, HAL_SDIO_READ_AFTER_WRITE, SDIO_FUNCTION_0,
                       (uint8_t *)&IOFunction) != HAL_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Enable Assynchrone interrupt.
  * @param  hsdio: Pointer to SDIO handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_EnableIOAsynInterrupt(SDIO_HandleTypeDef *hsdio)
{
  uint8_t enable_asyn_it  = 0U;

  /* Check the parameters */
  assert_param(hsdio != NULL);

  /* Check the SDIO peripheral handle parameter */
  if (hsdio == NULL)
  {
    return HAL_ERROR;
  }

  if (SDIO_ReadDirect(hsdio, SDMMC_SDIO_CCCR20_SD_BYTE2, HAL_SDIO_WRITE_ONLY, SDIO_FUNCTION_0, &enable_asyn_it)
      != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* if already enable , do not need enable again */
  if ((enable_asyn_it & 0x02U) == 0x02U)
  {
    return HAL_OK;
  }
  else
  {
    enable_asyn_it |= 0x02U;
  }

  if (SDIO_WriteDirect(hsdio, SDMMC_SDIO_CCCR20_SD_BYTE2, HAL_SDIO_READ_AFTER_WRITE, SDIO_FUNCTION_0,
                       &enable_asyn_it) != HAL_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Disable Assynchrone interrupt.
  * @param  hsdio: Pointer to SDIO handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDIO_DisableIOAsynInterrupt(SDIO_HandleTypeDef *hsdio)
{
  uint8_t enable_asyn_it  = 0U;

  /* Check the parameters */
  assert_param(hsdio != NULL);

  /* Check the SDIO peripheral handle parameter */
  if (hsdio == NULL)
  {
    return HAL_ERROR;
  }

  if (SDIO_ReadDirect(hsdio, SDMMC_SDIO_CCCR20_SD_BYTE2, HAL_SDIO_WRITE_ONLY, SDIO_FUNCTION_0, &enable_asyn_it)
      != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* if already disable , do not need disable again */
  if ((enable_asyn_it & 0x02U) == 0x00U)
  {
    return HAL_OK;
  }
  else
  {
    enable_asyn_it &= (uint8_t) ~(0x02U);
  }

  if (SDIO_WriteDirect(hsdio, SDMMC_SDIO_CCCR20_SD_BYTE2, HAL_SDIO_READ_AFTER_WRITE, SDIO_FUNCTION_0,
                       &enable_asyn_it) != HAL_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief sdio set io IRQ handler.
  * @param hsdio  Pointer to SDIO handle
  * @param IOFunction IO function io number.
  * @param Callback io IRQ handler.
  */
HAL_StatusTypeDef HAL_SDIO_RegisterIOFunctionCallback(SDIO_HandleTypeDef *hsdio, uint32_t IOFunction,
                                                      HAL_SDIO_IOFunction_CallbackTypeDef pCallback)
{
  /* Check the parameters */
  assert_param(hsdio != NULL);
  assert_param(IS_SDIO_FUNCTION(IOFunction));

  /* Check the SDIO peripheral handle parameter */
  if (hsdio == NULL)
  {
    return HAL_ERROR;
  }

  hsdio->SDIO_IOFunction_Callback[(uint32_t)IOFunction] = pCallback;
  hsdio->IOFunctionMask |= (1U << (uint8_t)IOFunction);

  return HAL_OK;
}
/**
  * @}
  */

/**
  * @}
  */

/* Private function --------------------------------------------------------------------------------------------------*/
/** @addtogroup SDIO_Private_Functions
  * @{
  */
/**
  * @brief  Initializes the SDIO device.
  * @param  hsdio: Pointer to the SDIO handle
  * @retval HAL status
  */
static HAL_StatusTypeDef SDIO_InitCard(SDIO_HandleTypeDef *hsdio)
{
  uint32_t errorstate;
  uint32_t timeout = 0U;
  uint16_t sdio_rca = 1U;
  uint32_t Resp4;
  uint32_t nbr_of_func;

  /* Identify card operating voltage */
  errorstate = SDMMC_CmdGoIdleState(hsdio->Instance);
  if (errorstate != HAL_SDIO_ERROR_NONE)
  {
    return HAL_ERROR;
  }

  /* Check the power State */
  if (SDMMC_GetPowerState(hsdio->Instance) == 0U)
  {
    return HAL_ERROR;
  }

  /* Send CMD5 */
  errorstate = SDMMC_CmdSendOperationcondition(hsdio->Instance, 0U, &Resp4);
  if (errorstate != HAL_SDIO_ERROR_NONE)
  {
    return HAL_ERROR;
  }

  nbr_of_func = ((Resp4 & 0x70000000U) >> 28U);
  /* Check if Nbr of function > 0 and OCR valid */
  if (nbr_of_func > 0U)
  {
    /* Send CMD5 with arg= S18R, WV*/
    if (SDMMC_CmdSendOperationcondition(hsdio->Instance, (SDIO_OCR_VDD_32_33 | SDIO_OCR_SDIO_S18R), &Resp4)
        != HAL_SDIO_ERROR_NONE)
    {
      return HAL_ERROR;
    }
    /* Check if IORDY = 1 and S18A = 1 */
    if ((((Resp4 & 0x80000000U) >> 31U) != 0U) && (((Resp4 & 0x1000000U) >> 24U) != 0U))
    {
      /* Send CMD11 to switch 1.8V mode */
      errorstate = SDMMC_CmdVoltageSwitch(hsdio->Instance);
      if (errorstate != HAL_SDIO_ERROR_NONE)
      {
        return HAL_ERROR;
      }
    }
    else
    {
      /* S18A is not supported */
    }
  }

  /** Cmd3 is sent while response is SDMMC_ERROR_ILLEGAL_CMD, due to the partial init test done before
    * (sending cmd0 after the sequence cmd0/cmd3 is sent is considered illegal).
    */
  do
  {
    errorstate = SDMMC_CmdSetRelAdd(hsdio->Instance, &sdio_rca);
    timeout++;
    HAL_Delay(1);
  } while ((errorstate == SDMMC_ERROR_ILLEGAL_CMD) && (timeout != SDIO_TIMEOUT));

  if ((timeout == SDIO_TIMEOUT) || (errorstate != HAL_SDIO_ERROR_NONE))
  {
    return HAL_ERROR;
  }

  /* Select the Card ( Sending CMD7)*/
  errorstate = SDMMC_CmdSelDesel(hsdio->Instance, (uint32_t)(((uint32_t)sdio_rca) << 16U));
  if (errorstate != HAL_SDIO_ERROR_NONE)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Read 1 byte data.
  * @param  hsdio: Pointer to SDIO handle
  * @param  cmd_arg: formatted CMD52 structure
  * @param  pData: pointer to write or read data
  * @retval HAL status
  */
static HAL_StatusTypeDef SDIO_ReadDirect(SDIO_HandleTypeDef *hsdio, uint32_t addr, uint32_t raw,
                                         uint32_t function_nbr, uint8_t *pData)
{
  uint32_t errorstate;
  uint32_t cmd;

  cmd = SDIO_READ << 31U;
  cmd |= function_nbr << 28U;
  cmd |= raw << 27U;
  cmd |= (addr & 0x1FFFFU) << 9U;
  errorstate = SDMMC_SDIO_CmdReadWriteDirect(hsdio->Instance, cmd, pData);
  if (errorstate != HAL_SDIO_ERROR_NONE)
  {
    hsdio->ErrorCode |= errorstate;
    /* Clear all the static flags */
    __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_FLAGS);
    hsdio->State = HAL_SDIO_STATE_READY;
    hsdio->Context = SDIO_CONTEXT_NONE;
    return HAL_ERROR;
  }
  __SDMMC_CMDTRANS_DISABLE(hsdio->Instance);

  /* Clear all the static flags */
  __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_DATA_FLAGS);

  return HAL_OK;
}

/**
  * @brief  Write 1 byte data.
  * @param  hsdio: Pointer to SDIO handle
  * @param  cmd_arg: formatted CMD52 structure
  * @param  pData: pointer to write or read data
  * @retval HAL status
  */
static HAL_StatusTypeDef SDIO_WriteDirect(SDIO_HandleTypeDef *hsdio, uint32_t addr, uint32_t raw,
                                          uint32_t function_nbr, const uint8_t *pData)
{
  uint32_t errorstate;
  uint32_t cmd;
  uint8_t response;

  cmd = SDIO_WRITE << 31U;
  cmd |= function_nbr << 28U;
  cmd |= raw << 27U;
  cmd |= (addr & 0x1FFFFU) << 9U;
  cmd |= ((uint32_t) * pData & 0x000000FFU);
  errorstate = SDMMC_SDIO_CmdReadWriteDirect(hsdio->Instance, cmd, &response);

  if (errorstate != HAL_SDIO_ERROR_NONE)
  {
    hsdio->ErrorCode |= errorstate;
    /* Clear all the static flags */
    __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_FLAGS);
    hsdio->State = HAL_SDIO_STATE_READY;
    hsdio->Context = SDIO_CONTEXT_NONE;
    return HAL_ERROR;
  }
  __SDMMC_CMDTRANS_DISABLE(hsdio->Instance);

  /* Clear all the static flags */
  __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_DATA_FLAGS);

  return HAL_OK;
}

/**
  * @brief  Write multiple data with a single command.
  * @param  hsdio: Pointer to SDIO handle
  * @param  cmd_arg: formatted cmd53 structure
  * @param  Size_byte: block size if CMD53 defined in HAL_SDIO_MODE_BLOCK
  * @param  pData: pointer to write or read data
  * @retval HAL status
  */
static HAL_StatusTypeDef SDIO_WriteExtended(SDIO_HandleTypeDef *hsdio, HAL_SDIO_ExtendedCmd_TypeDef *cmd_arg,
                                            uint8_t *pData, uint16_t Size_byte)
{
  SDMMC_DataInitTypeDef config;
  uint32_t errorstate;
  uint32_t tickstart = HAL_GetTick();
  uint32_t regCount;
  uint8_t byteCount;
  uint32_t data;
  uint32_t dataremaining;
  uint32_t *u32tempbuff = (uint32_t *)(uint32_t)pData;
  SDMMC_TypeDef *SDMMCx;
  uint32_t cmd;
  uint32_t nbr_of_block;

  hsdio->ErrorCode = HAL_SDIO_ERROR_NONE;

  /* Compute how many blocks are to be send for pData of length data_size to be send */
  nbr_of_block = (((uint32_t)Size_byte & ~((uint32_t)hsdio->block_size & 1U))) >> __CLZ(__RBIT(hsdio->block_size));

  /* Initialize data control register */
  if ((hsdio->Instance->DCTRL & SDMMC_DCTRL_SDIOEN) != 0U)
  {
    hsdio->Instance->DCTRL = SDMMC_DCTRL_SDIOEN;
  }
  else
  {
    hsdio->Instance->DCTRL = 0U;
  }

  /* Configure the SDIO DPSM (Data Path State Machine) */
  config.DataTimeOut   = SDMMC_DATATIMEOUT;
  if (cmd_arg->Block_Mode == HAL_SDIO_MODE_BLOCK)
  {
    config.DataLength    = (uint32_t)(nbr_of_block * hsdio->block_size);
    config.DataBlockSize = SDIO_Convert_Block_Size(hsdio, hsdio->block_size);
  }
  else
  {
    config.DataLength    = Size_byte;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_1B;
  }

  config.TransferDir   = SDMMC_TRANSFER_DIR_TO_CARD;
  config.TransferMode  = (cmd_arg->Block_Mode == HAL_SDIO_MODE_BLOCK) ? SDMMC_TRANSFER_MODE_BLOCK :
                         SDMMC_TRANSFER_MODE_SDIO;
  config.DPSM          = SDMMC_DPSM_DISABLE;
  (void)SDMMC_ConfigData(hsdio->Instance, &config);
  __SDMMC_CMDTRANS_ENABLE(hsdio->Instance);

  hsdio->Context = (cmd_arg->Block_Mode == HAL_SDIO_MODE_BLOCK) ? SDIO_CONTEXT_WRITE_MULTIPLE_BLOCK :
                   SDIO_CONTEXT_WRITE_SINGLE_BLOCK;
  cmd = SDIO_WRITE << 31U;
  cmd |= cmd_arg->IOFunctionNbr << 28U;
  cmd |= cmd_arg->Block_Mode << 27U;
  cmd |= cmd_arg->OpCode << 26U;
  cmd |= (cmd_arg->Reg_Addr & 0x1FFFFU) << 9U;
  cmd |= (((uint32_t)Size_byte) & 0x1FFU);
  errorstate = SDMMC_SDIO_CmdReadWriteExtended(hsdio->Instance, cmd);
  if (errorstate != HAL_SDIO_ERROR_NONE)
  {
    MODIFY_REG(hsdio->Instance->DCTRL, SDMMC_DCTRL_FIFORST, SDMMC_DCTRL_FIFORST);
    __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_FLAGS);
    __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_DATA_FLAGS);
    hsdio->ErrorCode |= errorstate;
    hsdio->State = HAL_SDIO_STATE_READY;
    hsdio->Context = SDIO_CONTEXT_NONE;
    return HAL_ERROR;
  }

  SDMMCx = hsdio->Instance;
  dataremaining = config.DataLength;
  while (!__HAL_SDIO_GET_FLAG(hsdio, SDMMC_FLAG_TXUNDERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT |
                              SDMMC_FLAG_DATAEND))
  {
    if (__HAL_SDIO_GET_FLAG(hsdio, SDMMC_FLAG_TXFIFOHE) &&
        (dataremaining >= SDMMC_FIFO_SIZE))
    {
      for (regCount = SDMMC_FIFO_SIZE / 4U; regCount > 0U; regCount--)
      {
        SDMMCx->FIFO = *u32tempbuff;
        u32tempbuff++;
      }
      dataremaining -= SDMMC_FIFO_SIZE;
    }
    else if ((__HAL_SDIO_GET_FLAG(hsdio, SDMMC_FLAG_TXFIFOHE | SDMMC_FLAG_TXFIFOE)) &&
             (dataremaining < SDMMC_FIFO_SIZE))
    {
      const uint8_t *u8buff = (uint8_t *)u32tempbuff;
      while (dataremaining > 0U)
      {
        data = 0U;
        for (byteCount = 0U; (byteCount < 4U) && (dataremaining > 0U); byteCount++)
        {
          data |= ((uint32_t)(*u8buff) << (byteCount << 3U));
          u8buff++;
          dataremaining--;
        }
        SDMMCx->FIFO = data;
      }
    }
    if ((HAL_GetTick() - tickstart) >= SDMMC_DATATIMEOUT)
    {
      __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_FLAGS);
      hsdio->ErrorCode |= HAL_SDIO_ERROR_TIMEOUT;
      hsdio->State = HAL_SDIO_STATE_READY;
      hsdio->Context = SDIO_CONTEXT_NONE;
      return HAL_TIMEOUT;
    }
  }

  __SDMMC_CMDTRANS_DISABLE(hsdio->Instance);
  if (__HAL_SDIO_GET_FLAG(hsdio, SDMMC_FLAG_DTIMEOUT))
  {
    __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_FLAGS);
    hsdio->ErrorCode |= HAL_SDIO_ERROR_DATA_TIMEOUT;
    hsdio->State = HAL_SDIO_STATE_READY;
    hsdio->Context = SDIO_CONTEXT_NONE;
    return HAL_ERROR;
  }
  else if (__HAL_SDIO_GET_FLAG(hsdio, SDMMC_FLAG_DCRCFAIL))
  {
    __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_FLAGS);
    hsdio->ErrorCode |= HAL_SDIO_ERROR_DATA_CRC_FAIL;
    hsdio->State = HAL_SDIO_STATE_READY;
    hsdio->Context = SDIO_CONTEXT_NONE;
    return HAL_ERROR;
  }
  else if (__HAL_SDIO_GET_FLAG(hsdio, SDMMC_FLAG_TXUNDERR))
  {
    __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_FLAGS);
    hsdio->ErrorCode |= HAL_SDIO_ERROR_TX_UNDERRUN;
    hsdio->State = HAL_SDIO_STATE_READY;
    hsdio->Context = SDIO_CONTEXT_NONE;
    return HAL_ERROR;
  }
  else if (hsdio->ErrorCode == SDMMC_ERROR_INVALID_PARAMETER)
  {
    __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_DATA_FLAGS);
    hsdio->State = HAL_SDIO_STATE_READY;
    hsdio->Context = SDIO_CONTEXT_NONE;
    return HAL_ERROR;
  }
  else
  {
    /* Nothing to do */
  }

  __HAL_SDIO_CLEAR_FLAG(hsdio, SDMMC_STATIC_DATA_FLAGS);

  return HAL_OK;
}

/**
  * @brief  Allows to convert a block size in the according SDMMC value for configuring the SDMMC when doing a CMD53
  * @param  hsdio: Pointer to the SDIO handle.
  * @param  block_size: block size in bytes
  * @retval block size as DBLOCKSIZE[3:0] bits format
  */
static uint8_t SDIO_Convert_Block_Size(const SDIO_HandleTypeDef *hsdio, uint32_t block_size)
{
  UNUSED(hsdio);

  uint8_t most_bit = (uint8_t)__CLZ(__RBIT(block_size));
  /*(1 << most_bit) - 1) is the mask used for blocksize*/
  if (((uint8_t)block_size & ((1U << most_bit) - 1U)) != 0U)
  {
    return (uint8_t)SDMMC_DATABLOCK_SIZE_4B;
  }
  return most_bit << SDMMC_DCTRL_DBLOCKSIZE_Pos;
}

/*!
 * @brief SDIO card io pending interrupt handle function.
 * @note  This function is used to handle the pending io interrupt.
 *        To register a IO IRQ handler, Use HAL_SDIO_EnableIOInterrupt and HAL_SDIO_SetIOIRQHandler
 * @param  hsdio: Pointer to SDIO handle
 * @retval HAL status
 */
static HAL_StatusTypeDef SDIO_IOFunction_IRQHandler(SDIO_HandleTypeDef *hsdio)
{
  uint8_t count;
  uint8_t pendingInt;

  if (hsdio->IOInterruptNbr == 1U)
  {
    if ((hsdio->SDIO_IOFunction_Callback[hsdio->IOFunctionMask - 1U]) != NULL)
    {
      (hsdio->SDIO_IOFunction_Callback[hsdio->IOFunctionMask - 1U])(hsdio, hsdio->IOFunctionMask - 1U);
    }
  }
  else if ((hsdio->IOInterruptNbr > 1U) && (hsdio->IOFunctionMask != 0U))
  {
    /* Get pending int firstly */
    if (SDIO_ReadDirect(hsdio, SDMMC_SDIO_CCCR4_SD_BYTE1, HAL_SDIO_WRITE_ONLY, SDIO_FUNCTION_0, &pendingInt) !=
        HAL_OK)
    {
      return HAL_ERROR;
    }

    if ((pendingInt != 0U) && (hsdio->IOFunctionMask != 0U))
    {
      for (count = 1; count <= SDIO_MAX_IO_NUMBER; count++)
      {
        if (((pendingInt & (1U << count)) != 0U) && (((1U << count)  & hsdio->IOFunctionMask) != 0U))
        {
          if ((hsdio->SDIO_IOFunction_Callback[count - 1U]) != NULL)
          {
            (hsdio->SDIO_IOFunction_Callback[count - 1U])(hsdio, count);
          }
        }
      }
    }
  }
  else
  {
    /* Nothing to do */
  }

  return HAL_OK;
}
/**
  * @}
  */
#endif /* HAL_SDIO_MODULE_ENABLED */
#endif /* SDMMC1 || SDMMC2 */
/**
  * @}
  */

/**
  * @}
  */
