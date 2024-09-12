/**
  ******************************************************************************
  * @file    stm32n6xx_hal_mmc.c
  * @author  MCD Application Team
  * @brief   MMC card HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Secure Digital (MMC) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions
  *           + MMC card Control functions
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
    this memory. The needed STM32 hardware resources (SDMMC and GPIO) are performed by
    the user in HAL_MMC_MspInit() function (MSP layer).
    Basically, the MSP layer configuration should be the same as we provide in the
    examples.
    You can easily tailor this configuration according to hardware resources.

  [..]
    This driver is a generic layered driver for SDMMC memories which uses the HAL
    SDMMC driver functions to interface with MMC and eMMC cards devices.
    It is used as follows:

    (#)Initialize the SDMMC low level resources by implement the HAL_MMC_MspInit() API:
        (##) Enable the SDMMC interface clock using __HAL_RCC_SDMMC_CLK_ENABLE();
        (##) SDMMC pins configuration for MMC card
            (+++) Enable the clock for the SDMMC GPIOs using the functions __HAL_RCC_GPIOx_CLK_ENABLE();
            (+++) Configure these SDMMC pins as alternate function pull-up using HAL_GPIO_Init()
                  and according to your pin assignment;
        (##) NVIC configuration if you need to use interrupt process (HAL_MMC_ReadBlocks_IT()
             and HAL_MMC_WriteBlocks_IT() APIs).
            (+++) Configure the SDMMC interrupt priorities using function HAL_NVIC_SetPriority();
            (+++) Enable the NVIC SDMMC IRQs using function HAL_NVIC_EnableIRQ()
            (+++) SDMMC interrupts are managed using the macros __HAL_MMC_ENABLE_IT()
                  and __HAL_MMC_DISABLE_IT() inside the communication process.
            (+++) SDMMC interrupts pending bits are managed using the macros __HAL_MMC_GET_IT()
                  and __HAL_MMC_CLEAR_IT()
        (##) No general propose DMA Configuration is needed, an Internal DMA for SDMMC Peripheral are used.

    (#) At this stage, you can perform MMC read/write/erase operations after MMC card initialization

  *** MMC Card Initialization and configuration ***
  ================================================
  [..]
    To initialize the MMC Card, use the HAL_MMC_Init() function. It Initializes
    SDMMC Peripheral (STM32 side) and the MMC Card, and put it into StandBy State (Ready for data transfer).
    This function provide the following operations:

    (#) Initialize the SDMMC peripheral interface with default configuration.
        The initialization process is done at 400KHz. You can change or adapt
        this frequency by adjusting the "ClockDiv" field.
        The MMC Card frequency (SDMMC_CK) is computed as follows:

           SDMMC_CK = SDMMCCLK / (2 * ClockDiv)

        In initialization mode and according to the MMC Card standard,
        make sure that the SDMMC_CK frequency doesn't exceed 400KHz.

        This phase of initialization is done through SDMMC_Init() and
        SDMMC_PowerState_ON() SDMMC low level APIs.

    (#) Initialize the MMC card. The API used is HAL_MMC_InitCard().
        This phase allows the card initialization and identification
        and check the MMC Card type (Standard Capacity or High Capacity)
        The initialization flow is compatible with MMC standard.

        This API (HAL_MMC_InitCard()) could be used also to reinitialize the card in case
        of plug-off plug-in.

    (#) Configure the MMC Card Data transfer frequency. By Default, the card transfer
        frequency by adjusting the "ClockDiv" field.
        In transfer mode and according to the MMC Card standard, make sure that the
        SDMMC_CK frequency doesn't exceed 25MHz and 100MHz in High-speed mode switch.

    (#) Select the corresponding MMC Card according to the address read with the step 2.

    (#) Configure the MMC Card in wide bus mode: 4-bits data.
    (#) Select the MMC Card partition using HAL_MMC_SwitchPartition()

  *** MMC Card Read operation ***
  ==============================
  [..]
    (+) You can read from MMC card in polling mode by using function HAL_MMC_ReadBlocks().
        This function support only 512-bytes block length (the block size should be
        chosen as 512 bytes).
        You can choose either one block read operation or multiple block read operation
        by adjusting the "NumberOfBlocks" parameter.
        After this, you have to ensure that the transfer is done correctly. The check is done
        through HAL_MMC_GetCardState() function for MMC card state.

    (+) You can read from MMC card in DMA mode by using function HAL_MMC_ReadBlocks_DMA().
        This function support only 512-bytes block length (the block size should be
        chosen as 512 bytes).
        You can choose either one block read operation or multiple block read operation
        by adjusting the "NumberOfBlocks" parameter.
        After this, you have to ensure that the transfer is done correctly. The check is done
        through HAL_MMC_GetCardState() function for MMC card state.
        You could also check the DMA transfer process through the MMC Rx interrupt event.

    (+) You can read from MMC card in Interrupt mode by using function HAL_MMC_ReadBlocks_IT().
        This function allows the read of 512 bytes blocks.
        You can choose either one block read operation or multiple block read operation
        by adjusting the "NumberOfBlocks" parameter.
        After this, you have to ensure that the transfer is done correctly. The check is done
        through HAL_MMC_GetCardState() function for MMC card state.
        You could also check the IT transfer process through the MMC Rx interrupt event.

  *** MMC Card Write operation ***
  ===============================
  [..]
    (+) You can write to MMC card in polling mode by using function HAL_MMC_WriteBlocks().
        This function support only 512-bytes block length (the block size should be
        chosen as 512 bytes).
        You can choose either one block read operation or multiple block read operation
        by adjusting the "NumberOfBlocks" parameter.
        After this, you have to ensure that the transfer is done correctly. The check is done
        through HAL_MMC_GetCardState() function for MMC card state.

    (+) You can write to MMC card in DMA mode by using function HAL_MMC_WriteBlocks_DMA().
        This function support only 512-bytes block length (the block size should be
        chosen as 512 byte).
        You can choose either one block read operation or multiple block read operation
        by adjusting the "NumberOfBlocks" parameter.
        After this, you have to ensure that the transfer is done correctly. The check is done
        through HAL_MMC_GetCardState() function for MMC card state.
        You could also check the DMA transfer process through the MMC Tx interrupt event.

    (+) You can write to MMC card in Interrupt mode by using function HAL_MMC_WriteBlocks_IT().
        This function allows the read of 512 bytes blocks.
        You can choose either one block read operation or multiple block read operation
        by adjusting the "NumberOfBlocks" parameter.
        After this, you have to ensure that the transfer is done correctly. The check is done
        through HAL_MMC_GetCardState() function for MMC card state.
        You could also check the IT transfer process through the MMC Tx interrupt event.

  *** MMC card information ***
  ===========================
  [..]
    (+) To get MMC card information, you can use the function HAL_MMC_GetCardInfo().
        It returns useful information about the MMC card such as block size, card type,
        block number ...

  *** MMC card CSD register ***
  ============================
  [..]
    (+) The HAL_MMC_GetCardCSD() API allows to get the parameters of the CSD register.
        Some of the CSD parameters are useful for card initialization and identification.

  *** MMC card CID register ***
  ============================
  [..]
    (+) The HAL_MMC_GetCardCID() API allows to get the parameters of the CID register.
        Some of the CID parameters are useful for card initialization and identification.

  *** MMC Card Reply Protected Memory Block (RPMB) Key Programming operation ***
  ==============================
  [..]
    (+) You can program the authentication key of RPMB area in polling mode by using function
        HAL_MMC_RPMB_ProgramAuthenticationKey().
        This function is only used once during the life of an MMC card.
        After this, you have to ensure that the transfer is done correctly. The check is done
        through HAL_MMC_GetRPMBError() function for operation state.
    (+) You can program the authentication key of RPMB area in Interrupt mode by using function
        HAL_MMC_RPMB_ProgramAuthenticationKey_IT().
        This function is only used once during the life of an MMC card.
        After this, you have to ensure that the transfer is done correctly. The check is done
        through HAL_MMC_GetRPMBError() function for operation state.

  *** MMC Card Reply Protected Memory Block (RPMB) write counter operation ***
  ==============================
  [..]
    (+) You can get the write counter value of RPMB area in polling mode by using function
        HAL_MMC_RPMB_GetWriteCounter().
    (+) You can get the write counter value of RPMB area in Interrupt mode by using function
        HAL_MMC_RPMB_GetWriteCounter_IT().

  *** MMC Card Reply Protected Memory Block (RPMB) write operation ***
  ==============================
  [..]
    (+) You can write to the RPMB area of MMC card in polling mode by using function
        HAL_MMC_WriteBlocks().
        This function supports the one, two, or thirty two blocks write operation
        (with 512-bytes block length).
        You can choose the number of blocks at the multiple block read operation by adjusting
        the "NumberOfBlocks" parameter.
        After this, you have to ensure that the transfer is done correctly. The check is done
        through HAL_MMC_GetRPMBError() function for operation state.
    (+) You can write to the RPMB area of MMC card in Interrupt mode by using function
        HAL_MMC_WriteBlocks_IT().
        This function supports the one, two, or thirty two blocks write operation
        (with 512-bytes block length).
        You can choose the number of blocks at the multiple block read operation by adjusting
        the "NumberOfBlocks" parameter.
        After this, you have to ensure that the transfer is done correctly. The check is done
        through HAL_MMC_GetRPMBError() function for operation state.

  *** MMC Card Reply Protected Memory Block (RPMB) read operation ***
  ==============================
  [..]
    (+) You can read from the RPMB area of MMC card in polling mode by using function
        HAL_MMC_RPMB_ReadBlocks().
        The block size should be chosen as multiple of 512 bytes.
        You can choose the number of blocks by adjusting the "NumberOfBlocks" parameter.
        After this, you have to ensure that the transfer is done correctly. The check is done
        through HAL_MMC_GetRPMBError() function for MMC card state.
    (+) You can read from the RPMB area of MMC card in Interrupt mode by using function
        HAL_MMC_RPMB_ReadBlocks_IT().
        The block size should be chosen as multiple of 512 bytes.
        You can choose the number of blocks by adjusting the "NumberOfBlocks" parameter.
        After this, you have to ensure that the transfer is done correctly. The check is done
        through HAL_MMC_GetRPMBError() function for MMC card state.

  *** MMC HAL driver macros list ***
  ==================================
  [..]
    Below the list of most used macros in MMC HAL driver.

    (+) __HAL_MMC_ENABLE_IT: Enable the MMC device interrupt
    (+) __HAL_MMC_DISABLE_IT: Disable the MMC device interrupt
    (+) __HAL_MMC_GET_FLAG:Check whether the specified MMC flag is set or not
    (+) __HAL_MMC_CLEAR_FLAG: Clear the MMC's pending flags

  [..]
    (@) You can refer to the MMC HAL driver header file for more useful macros

  *** Callback registration ***
  =============================================
  [..]
    The compilation define USE_HAL_MMC_REGISTER_CALLBACKS when set to 1
    allows the user to configure dynamically the driver callbacks.

    Use Functions HAL_MMC_RegisterCallback() to register a user callback,
    it allows to register following callbacks:
      (+) TxCpltCallback : callback when a transmission transfer is completed.
      (+) RxCpltCallback : callback when a reception transfer is completed.
      (+) ErrorCallback : callback when error occurs.
      (+) AbortCpltCallback : callback when abort is completed.
      (+) Read_DMALnkLstBufCpltCallback : callback when the DMA reception of linked list node buffer is completed.
      (+) Write_DMALnkLstBufCpltCallback : callback when the DMA transmission of linked list node buffer is completed.
      (+) MspInitCallback    : MMC MspInit.
      (+) MspDeInitCallback  : MMC MspDeInit.
    This function takes as parameters the HAL peripheral handle, the Callback ID
    and a pointer to the user callback function.

    Use function HAL_MMC_UnRegisterCallback() to reset a callback to the default
    weak (overridden) function. It allows to reset following callbacks:
      (+) TxCpltCallback : callback when a transmission transfer is completed.
      (+) RxCpltCallback : callback when a reception transfer is completed.
      (+) ErrorCallback : callback when error occurs.
      (+) AbortCpltCallback : callback when abort is completed.
      (+) Read_DMADblBuf0CpltCallback : callback when the DMA reception of first buffer is completed.
      (+) Read_DMADblBuf1CpltCallback : callback when the DMA reception of second buffer is completed.
      (+) Write_DMADblBuf0CpltCallback : callback when the DMA transmission of first buffer is completed.
      (+) Write_DMADblBuf1CpltCallback : callback when the DMA transmission of second buffer is completed.
      (+) MspInitCallback    : MMC MspInit.
      (+) MspDeInitCallback  : MMC MspDeInit.
    This function) takes as parameters the HAL peripheral handle and the Callback ID.

    By default, after the HAL_MMC_Init and if the state is HAL_MMC_STATE_RESET
    all callbacks are reset to the corresponding legacy weak (overridden) functions.
    Exception done for MspInit and MspDeInit callbacks that are respectively
    reset to the legacy weak (overridden) functions in the HAL_MMC_Init
    and HAL_MMC_DeInit only when these callbacks are null (not registered beforehand).
    If not, MspInit or MspDeInit are not null, the HAL_MMC_Init and HAL_MMC_DeInit
    keep and use the user MspInit/MspDeInit callbacks (registered beforehand)

    Callbacks can be registered/unregistered in READY state only.
    Exception done for MspInit/MspDeInit callbacks that can be registered/unregistered
    in READY or RESET state, thus registered (user) MspInit/DeInit callbacks can be used
    during the Init/DeInit.
    In that case first register the MspInit/MspDeInit user callbacks
    using HAL_MMC_RegisterCallback before calling HAL_MMC_DeInit
    or HAL_MMC_Init function.

    When The compilation define USE_HAL_MMC_REGISTER_CALLBACKS is set to 0 or
    not defined, the callback registering feature is not available
    and weak (overridden) callbacks are used.

  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup MMC MMC
  * @brief MMC HAL module driver
  * @{
  */

#if defined (SDMMC1) || defined (SDMMC2)
#ifdef HAL_MMC_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup MMC_Private_Defines
  * @{
  */
#if defined (VDD_VALUE) && (VDD_VALUE <= 1950U)
#define MMC_VOLTAGE_RANGE               EMMC_LOW_VOLTAGE_RANGE

#define MMC_EXT_CSD_PWR_CL_26_INDEX     201
#define MMC_EXT_CSD_PWR_CL_52_INDEX     200
#define MMC_EXT_CSD_PWR_CL_DDR_52_INDEX 238

#define MMC_EXT_CSD_PWR_CL_26_POS       8
#define MMC_EXT_CSD_PWR_CL_52_POS       0
#define MMC_EXT_CSD_PWR_CL_DDR_52_POS   16
#else
#define MMC_VOLTAGE_RANGE               EMMC_HIGH_VOLTAGE_RANGE

#define MMC_EXT_CSD_PWR_CL_26_INDEX     203
#define MMC_EXT_CSD_PWR_CL_52_INDEX     202
#define MMC_EXT_CSD_PWR_CL_DDR_52_INDEX 239

#define MMC_EXT_CSD_PWR_CL_26_POS       24
#define MMC_EXT_CSD_PWR_CL_52_POS       16
#define MMC_EXT_CSD_PWR_CL_DDR_52_POS   24
#endif /* (VDD_VALUE) && (VDD_VALUE <= 1950U)*/

#define MMC_EXT_CSD_SLEEP_NOTIFICATION_TIME_INDEX 216
#define MMC_EXT_CSD_SLEEP_NOTIFICATION_TIME_POS   0
#define MMC_EXT_CSD_S_A_TIMEOUT_INDEX             217
#define MMC_EXT_CSD_S_A_TIMEOUT_POS               8

/* Frequencies used in the driver for clock divider calculation */
#define MMC_INIT_FREQ                   400000U   /* Initialization phase : 400 kHz max */
#define MMC_HIGH_SPEED_FREQ             52000000U /* High speed phase : 52 MHz max */

/* The Data elements' postitions in the frame Frame for RPMB area */
#define MMC_RPMB_KEYMAC_POSITION         196U
#define MMC_RPMB_DATA_POSITION           228U
#define MMC_RPMB_NONCE_POSITION          484U
#define MMC_RPMB_WRITE_COUNTER_POSITION  500U
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/** @defgroup MMC_Private_Functions MMC Private Functions
  * @{
  */
static uint32_t MMC_InitCard(MMC_HandleTypeDef *hmmc);
static uint32_t MMC_PowerON(MMC_HandleTypeDef *hmmc);
static uint32_t MMC_SendStatus(MMC_HandleTypeDef *hmmc, uint32_t *pCardStatus);
static void     MMC_PowerOFF(MMC_HandleTypeDef *hmmc);
static void     MMC_Write_IT(MMC_HandleTypeDef *hmmc);
static void     MMC_Read_IT(MMC_HandleTypeDef *hmmc);
static uint32_t MMC_HighSpeed(MMC_HandleTypeDef *hmmc, FunctionalState state);
static uint32_t MMC_DDR_Mode(MMC_HandleTypeDef *hmmc, FunctionalState state);
static HAL_StatusTypeDef MMC_ReadExtCSD(MMC_HandleTypeDef *hmmc, uint32_t *pFieldData, uint16_t FieldIndex,
                                        uint32_t Timeout);
static uint32_t MMC_PwrClassUpdate(MMC_HandleTypeDef *hmmc, uint32_t Wide, uint32_t Speed);

/**
  * @}
  */
/* Exported functions --------------------------------------------------------*/
/** @addtogroup MMC_Exported_Functions
  * @{
  */

/** @addtogroup MMC_Exported_Functions_Group1
  *  @brief   Initialization and de-initialization functions
  *
@verbatim
  ==============================================================================
          ##### Initialization and de-initialization functions #####
  ==============================================================================
  [..]
    This section provides functions allowing to initialize/de-initialize the MMC
    card device to be ready for use.

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the MMC according to the specified parameters in the
            MMC_HandleTypeDef and create the associated handle.
  * @param  hmmc: Pointer to the MMC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_Init(MMC_HandleTypeDef *hmmc)
{
  /* Check the MMC handle allocation */
  if (hmmc == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_SDMMC_ALL_INSTANCE(hmmc->Instance));
  assert_param(IS_SDMMC_CLOCK_EDGE(hmmc->Init.ClockEdge));
  assert_param(IS_SDMMC_CLOCK_POWER_SAVE(hmmc->Init.ClockPowerSave));
  assert_param(IS_SDMMC_BUS_WIDE(hmmc->Init.BusWide));
  assert_param(IS_SDMMC_HARDWARE_FLOW_CONTROL(hmmc->Init.HardwareFlowControl));
  assert_param(IS_SDMMC_CLKDIV(hmmc->Init.ClockDiv));

  if (hmmc->State == HAL_MMC_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hmmc->Lock = HAL_UNLOCKED;
#if defined (USE_HAL_MMC_REGISTER_CALLBACKS) && (USE_HAL_MMC_REGISTER_CALLBACKS == 1U)
    /* Reset Callback pointers in HAL_MMC_STATE_RESET only */
    hmmc->TxCpltCallback    = HAL_MMC_TxCpltCallback;
    hmmc->RxCpltCallback    = HAL_MMC_RxCpltCallback;
    hmmc->ErrorCallback     = HAL_MMC_ErrorCallback;
    hmmc->AbortCpltCallback = HAL_MMC_AbortCallback;
    hmmc->Read_DMALnkLstBufCpltCallback  = HAL_MMCEx_Read_DMALnkLstBufCpltCallback;
    hmmc->Write_DMALnkLstBufCpltCallback = HAL_MMCEx_Write_DMALnkLstBufCpltCallback;

    if (hmmc->MspInitCallback == NULL)
    {
      hmmc->MspInitCallback = HAL_MMC_MspInit;
    }

    /* Init the low level hardware */
    hmmc->MspInitCallback(hmmc);
#else
    /* Init the low level hardware : GPIO, CLOCK, CORTEX...etc */
    HAL_MMC_MspInit(hmmc);
#endif /* USE_HAL_MMC_REGISTER_CALLBACKS */
  }

  hmmc->State = HAL_MMC_STATE_BUSY;

  /* Initialize the Card parameters */
  if (HAL_MMC_InitCard(hmmc) == HAL_ERROR)
  {
    return HAL_ERROR;
  }

  /* Initialize the error code */
  hmmc->ErrorCode = HAL_DMA_ERROR_NONE;

  /* Initialize the MMC operation */
  hmmc->Context = MMC_CONTEXT_NONE;

  /* Initialize the MMC state */
  hmmc->State = HAL_MMC_STATE_READY;

  /* Configure bus width */
  if (hmmc->Init.BusWide != SDMMC_BUS_WIDE_1B)
  {
    if (HAL_MMC_ConfigWideBusOperation(hmmc, hmmc->Init.BusWide) != HAL_OK)
    {
      return HAL_ERROR;
    }
  }

  return HAL_OK;
}

/**
  * @brief  Initializes the MMC Card.
  * @param  hmmc: Pointer to MMC handle
  * @note   This function initializes the MMC card. It could be used when a card
            re-initialization is needed.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_InitCard(MMC_HandleTypeDef *hmmc)
{
  uint32_t errorstate;
  MMC_InitTypeDef Init;
  uint32_t sdmmc_clk;

  /* Default SDMMC peripheral configuration for MMC card initialization */
  Init.ClockEdge           = SDMMC_CLOCK_EDGE_RISING;
  Init.ClockPowerSave      = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  Init.BusWide             = SDMMC_BUS_WIDE_1B;
  Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;

  /* Init Clock should be less or equal to 400Khz*/
  if (hmmc->Instance == SDMMC1)
  {
    sdmmc_clk = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SDMMC1);
  }
  else if (hmmc->Instance == SDMMC2)
  {
    sdmmc_clk = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SDMMC2);
  }
  else
  {
    sdmmc_clk = 0;
  }
  if (sdmmc_clk == 0U)
  {
    hmmc->State = HAL_MMC_STATE_READY;
    hmmc->ErrorCode = SDMMC_ERROR_INVALID_PARAMETER;
    return HAL_ERROR;
  }
  Init.ClockDiv = sdmmc_clk / (2U * MMC_INIT_FREQ);

#if (USE_SD_TRANSCEIVER != 0U)
  Init.TranceiverPresent = SDMMC_TRANSCEIVER_NOT_PRESENT;
#endif /* USE_SD_TRANSCEIVER */

  /* Initialize SDMMC peripheral interface with default configuration */
  (void)SDMMC_Init(hmmc->Instance, Init);

  /* Set Power State to ON */
  (void)SDMMC_PowerState_ON(hmmc->Instance);

  /* wait 74 Cycles: required power up waiting time before starting
     the MMC initialization sequence */
  if (Init.ClockDiv != 0U)
  {
    sdmmc_clk = sdmmc_clk / (2U * Init.ClockDiv);
  }

  if (sdmmc_clk != 0U)
  {
    HAL_Delay(1U + (74U * 1000U / (sdmmc_clk)));
  }

  /* Identify card operating voltage */
  errorstate = MMC_PowerON(hmmc);
  if (errorstate != HAL_MMC_ERROR_NONE)
  {
    hmmc->State = HAL_MMC_STATE_READY;
    hmmc->ErrorCode |= errorstate;
    return HAL_ERROR;
  }

  /* Card initialization */
  errorstate = MMC_InitCard(hmmc);
  if (errorstate != HAL_MMC_ERROR_NONE)
  {
    hmmc->State = HAL_MMC_STATE_READY;
    hmmc->ErrorCode |= errorstate;
    return HAL_ERROR;
  }

  /* Set Block Size for Card */
  errorstate = SDMMC_CmdBlockLength(hmmc->Instance, MMC_BLOCKSIZE);
  if (errorstate != HAL_MMC_ERROR_NONE)
  {
    /* Clear all the static flags */
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
    hmmc->ErrorCode |= errorstate;
    hmmc->State = HAL_MMC_STATE_READY;
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  De-Initializes the MMC card.
  * @param  hmmc: Pointer to MMC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_DeInit(MMC_HandleTypeDef *hmmc)
{
  /* Check the MMC handle allocation */
  if (hmmc == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_SDMMC_ALL_INSTANCE(hmmc->Instance));

  hmmc->State = HAL_MMC_STATE_BUSY;

  /* Set MMC power state to off */
  MMC_PowerOFF(hmmc);

#if defined (USE_HAL_MMC_REGISTER_CALLBACKS) && (USE_HAL_MMC_REGISTER_CALLBACKS == 1U)
  if (hmmc->MspDeInitCallback == NULL)
  {
    hmmc->MspDeInitCallback = HAL_MMC_MspDeInit;
  }

  /* DeInit the low level hardware */
  hmmc->MspDeInitCallback(hmmc);
#else
  /* De-Initialize the MSP layer */
  HAL_MMC_MspDeInit(hmmc);
#endif /* USE_HAL_MMC_REGISTER_CALLBACKS */

  hmmc->ErrorCode = HAL_MMC_ERROR_NONE;
  hmmc->State = HAL_MMC_STATE_RESET;

  return HAL_OK;
}

/**
  * @brief  Initializes the MMC MSP.
  * @param  hmmc: Pointer to MMC handle
  * @retval None
  */
__weak void HAL_MMC_MspInit(MMC_HandleTypeDef *hmmc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmmc);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_MMC_MspInit could be implemented in the user file
   */
}

/**
  * @brief  De-Initialize MMC MSP.
  * @param  hmmc: Pointer to MMC handle
  * @retval None
  */
__weak void HAL_MMC_MspDeInit(MMC_HandleTypeDef *hmmc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmmc);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_MMC_MspDeInit could be implemented in the user file
   */
}

/**
  * @}
  */

/** @addtogroup MMC_Exported_Functions_Group2
  *  @brief   Data transfer functions
  *
@verbatim
  ==============================================================================
                        ##### IO operation functions #####
  ==============================================================================
  [..]
    This subsection provides a set of functions allowing to manage the data
    transfer from/to MMC card.

@endverbatim
  * @{
  */

/**
  * @brief  Reads block(s) from a specified address in a card. The Data transfer
  *         is managed by polling mode.
  * @note   This API should be followed by a check on the card state through
  *         HAL_MMC_GetCardState().
  * @param  hmmc: Pointer to MMC handle
  * @param  pData: pointer to the buffer that will contain the received data
  * @param  BlockAdd: Block Address from where data is to be read
  * @param  NumberOfBlocks: Number of MMC blocks to read
  * @param  Timeout: Specify timeout value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_ReadBlocks(MMC_HandleTypeDef *hmmc, uint8_t *pData, uint32_t BlockAdd,
                                     uint32_t NumberOfBlocks,
                                     uint32_t Timeout)
{
  SDMMC_DataInitTypeDef config;
  uint32_t errorstate;
  uint32_t tickstart = HAL_GetTick();
  uint32_t count;
  uint32_t data;
  uint32_t dataremaining;
  uint32_t add = BlockAdd;
  uint8_t *tempbuff = pData;

  if (NULL == pData)
  {
    hmmc->ErrorCode |= HAL_MMC_ERROR_PARAM;
    return HAL_ERROR;
  }

  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    hmmc->ErrorCode = HAL_MMC_ERROR_NONE;

    if ((BlockAdd + NumberOfBlocks) > (hmmc->MmcCard.LogBlockNbr))
    {
      hmmc->ErrorCode |= HAL_MMC_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }

    /* Check the case of 4kB blocks (field DATA SECTOR SIZE of extended CSD register) */
    if (((hmmc->Ext_CSD[(MMC_EXT_CSD_DATA_SEC_SIZE_INDEX / 4)] >> MMC_EXT_CSD_DATA_SEC_SIZE_POS)
         & 0x000000FFU) != 0x0U)
    {
      if ((NumberOfBlocks % 8U) != 0U)
      {
        /* The number of blocks should be a multiple of 8 sectors of 512 bytes = 4 KBytes */
        hmmc->ErrorCode |= HAL_MMC_ERROR_BLOCK_LEN_ERR;
        return HAL_ERROR;
      }

      if ((BlockAdd % 8U) != 0U)
      {
        /* The address should be aligned to 8 (corresponding to 4 KBytes blocks) */
        hmmc->ErrorCode |= HAL_MMC_ERROR_ADDR_MISALIGNED;
        return HAL_ERROR;
      }
    }

    hmmc->State = HAL_MMC_STATE_BUSY;

    /* Initialize data control register */
    hmmc->Instance->DCTRL = 0U;

    if ((hmmc->MmcCard.CardType) != MMC_HIGH_CAPACITY_CARD)
    {
      add *= MMC_BLOCKSIZE;
    }

    /* Configure the MMC DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = NumberOfBlocks * MMC_BLOCKSIZE;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_SDMMC;
    config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hmmc->Instance, &config);
    __SDMMC_CMDTRANS_ENABLE(hmmc->Instance);

    /* Read block(s) in polling mode */
    if (NumberOfBlocks > 1U)
    {
      hmmc->Context = MMC_CONTEXT_READ_MULTIPLE_BLOCK;

      /* Read Multi Block command */
      errorstate = SDMMC_CmdReadMultiBlock(hmmc->Instance, add);
    }
    else
    {
      hmmc->Context = MMC_CONTEXT_READ_SINGLE_BLOCK;

      /* Read Single Block command */
      errorstate = SDMMC_CmdReadSingleBlock(hmmc->Instance, add);
    }
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Poll on SDMMC flags */
    dataremaining = config.DataLength;
    while (!__HAL_MMC_GET_FLAG(hmmc,
                               SDMMC_FLAG_RXOVERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DATAEND))
    {
      if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_RXFIFOHF) && (dataremaining >= SDMMC_FIFO_SIZE))
      {
        /* Read data from SDMMC Rx FIFO */
        for (count = 0U; count < (SDMMC_FIFO_SIZE / 4U); count++)
        {
          data = SDMMC_ReadFIFO(hmmc->Instance);
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

      if (((HAL_GetTick() - tickstart) >=  Timeout) || (Timeout == 0U))
      {
        /* Clear all the static flags */
        __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
        hmmc->ErrorCode |= HAL_MMC_ERROR_TIMEOUT;
        hmmc->State = HAL_MMC_STATE_READY;
        return HAL_TIMEOUT;
      }
    }
    __SDMMC_CMDTRANS_DISABLE(hmmc->Instance);

    /* Send stop transmission command in case of multiblock read */
    if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DATAEND) && (NumberOfBlocks > 1U))
    {
      /* Send stop transmission command */
      errorstate = SDMMC_CmdStopTransfer(hmmc->Instance);
      if (errorstate != HAL_MMC_ERROR_NONE)
      {
        /* Clear all the static flags */
        __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
        hmmc->ErrorCode |= errorstate;
        hmmc->State = HAL_MMC_STATE_READY;
        return HAL_ERROR;
      }
    }

    /* Get error state */
    if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DTIMEOUT))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_DATA_TIMEOUT;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }
    else if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DCRCFAIL))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_DATA_CRC_FAIL;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }
    else if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_RXOVERR))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_RX_OVERRUN;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }
    else
    {
      /* Nothing to do */
    }

    /* Clear all the static flags */
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_DATA_FLAGS);

    hmmc->State = HAL_MMC_STATE_READY;

    return HAL_OK;
  }
  else
  {
    hmmc->ErrorCode |= HAL_MMC_ERROR_BUSY;
    return HAL_ERROR;
  }
}

/**
  * @brief  Allows to write block(s) to a specified address in a card. The Data
  *         transfer is managed by polling mode.
  * @note   This API should be followed by a check on the card state through
  *         HAL_MMC_GetCardState().
  * @param  hmmc: Pointer to MMC handle
  * @param  pData: pointer to the buffer that will contain the data to transmit
  * @param  BlockAdd: Block Address where data will be written
  * @param  NumberOfBlocks: Number of MMC blocks to write
  * @param  Timeout: Specify timeout value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_WriteBlocks(MMC_HandleTypeDef *hmmc, const uint8_t *pData, uint32_t BlockAdd,
                                      uint32_t NumberOfBlocks, uint32_t Timeout)
{
  SDMMC_DataInitTypeDef config;
  uint32_t errorstate;
  uint32_t tickstart = HAL_GetTick();
  uint32_t count;
  uint32_t data;
  uint32_t dataremaining;
  uint32_t add = BlockAdd;
  const uint8_t *tempbuff = pData;

  if (NULL == pData)
  {
    hmmc->ErrorCode |= HAL_MMC_ERROR_PARAM;
    return HAL_ERROR;
  }

  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    hmmc->ErrorCode = HAL_MMC_ERROR_NONE;

    if ((BlockAdd + NumberOfBlocks) > (hmmc->MmcCard.LogBlockNbr))
    {
      hmmc->ErrorCode |= HAL_MMC_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }

    /* Check the case of 4kB blocks (field DATA SECTOR SIZE of extended CSD register) */
    if (((hmmc->Ext_CSD[(MMC_EXT_CSD_DATA_SEC_SIZE_INDEX / 4)] >> MMC_EXT_CSD_DATA_SEC_SIZE_POS) & 0x000000FFU) != 0x0U)
    {
      if ((NumberOfBlocks % 8U) != 0U)
      {
        /* The number of blocks should be a multiple of 8 sectors of 512 bytes = 4 KBytes */
        hmmc->ErrorCode |= HAL_MMC_ERROR_BLOCK_LEN_ERR;
        return HAL_ERROR;
      }

      if ((BlockAdd % 8U) != 0U)
      {
        /* The address should be aligned to 8 (corresponding to 4 KBytes blocks) */
        hmmc->ErrorCode |= HAL_MMC_ERROR_ADDR_MISALIGNED;
        return HAL_ERROR;
      }
    }

    hmmc->State = HAL_MMC_STATE_BUSY;

    /* Initialize data control register */
    hmmc->Instance->DCTRL = 0U;

    if ((hmmc->MmcCard.CardType) != MMC_HIGH_CAPACITY_CARD)
    {
      add *= MMC_BLOCKSIZE;
    }

    /* Configure the MMC DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = NumberOfBlocks * MMC_BLOCKSIZE;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_CARD;
    config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hmmc->Instance, &config);
    __SDMMC_CMDTRANS_ENABLE(hmmc->Instance);

    /* Write Blocks in Polling mode */
    if (NumberOfBlocks > 1U)
    {
      hmmc->Context = MMC_CONTEXT_WRITE_MULTIPLE_BLOCK;

      /* Write Multi Block command */
      errorstate = SDMMC_CmdWriteMultiBlock(hmmc->Instance, add);
    }
    else
    {
      hmmc->Context = MMC_CONTEXT_WRITE_SINGLE_BLOCK;

      /* Write Single Block command */
      errorstate = SDMMC_CmdWriteSingleBlock(hmmc->Instance, add);
    }
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Write block(s) in polling mode */
    dataremaining = config.DataLength;
    while (!__HAL_MMC_GET_FLAG(hmmc,
                               SDMMC_FLAG_TXUNDERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DATAEND))
    {
      if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_TXFIFOHE) && (dataremaining >= SDMMC_FIFO_SIZE))
      {
        /* Write data to SDMMC Tx FIFO */
        for (count = 0U; count < (SDMMC_FIFO_SIZE / 4U); count++)
        {
          data = (uint32_t)(*tempbuff);
          tempbuff++;
          data |= ((uint32_t)(*tempbuff) << 8U);
          tempbuff++;
          data |= ((uint32_t)(*tempbuff) << 16U);
          tempbuff++;
          data |= ((uint32_t)(*tempbuff) << 24U);
          tempbuff++;
          (void)SDMMC_WriteFIFO(hmmc->Instance, &data);
        }
        dataremaining -= SDMMC_FIFO_SIZE;
      }

      if (((HAL_GetTick() - tickstart) >=  Timeout) || (Timeout == 0U))
      {
        /* Clear all the static flags */
        __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
        hmmc->ErrorCode |= errorstate;
        hmmc->State = HAL_MMC_STATE_READY;
        return HAL_TIMEOUT;
      }
    }
    __SDMMC_CMDTRANS_DISABLE(hmmc->Instance);

    /* Send stop transmission command in case of multiblock write */
    if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DATAEND) && (NumberOfBlocks > 1U))
    {
      /* Send stop transmission command */
      errorstate = SDMMC_CmdStopTransfer(hmmc->Instance);
      if (errorstate != HAL_MMC_ERROR_NONE)
      {
        /* Clear all the static flags */
        __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
        hmmc->ErrorCode |= errorstate;
        hmmc->State = HAL_MMC_STATE_READY;
        return HAL_ERROR;
      }
    }

    /* Get error state */
    if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DTIMEOUT))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_DATA_TIMEOUT;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }
    else if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DCRCFAIL))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_DATA_CRC_FAIL;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }
    else if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_TXUNDERR))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_TX_UNDERRUN;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }
    else
    {
      /* Nothing to do */
    }

    /* Clear all the static flags */
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_DATA_FLAGS);

    hmmc->State = HAL_MMC_STATE_READY;

    return HAL_OK;
  }
  else
  {
    hmmc->ErrorCode |= HAL_MMC_ERROR_BUSY;
    return HAL_ERROR;
  }
}

/**
  * @brief  Reads block(s) from a specified address in a card. The Data transfer
  *         is managed in interrupt mode.
  * @note   This API should be followed by a check on the card state through
  *         HAL_MMC_GetCardState().
  * @note   You could also check the IT transfer process through the MMC Rx
  *         interrupt event.
  * @param  hmmc: Pointer to MMC handle
  * @param  pData: Pointer to the buffer that will contain the received data
  * @param  BlockAdd: Block Address from where data is to be read
  * @param  NumberOfBlocks: Number of blocks to read.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_ReadBlocks_IT(MMC_HandleTypeDef *hmmc, uint8_t *pData, uint32_t BlockAdd,
                                        uint32_t NumberOfBlocks)
{
  SDMMC_DataInitTypeDef config;
  uint32_t errorstate;
  uint32_t add = BlockAdd;

  if (NULL == pData)
  {
    hmmc->ErrorCode |= HAL_MMC_ERROR_PARAM;
    return HAL_ERROR;
  }

  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    hmmc->ErrorCode = HAL_MMC_ERROR_NONE;

    if ((BlockAdd + NumberOfBlocks) > (hmmc->MmcCard.LogBlockNbr))
    {
      hmmc->ErrorCode |= HAL_MMC_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }

    /* Check the case of 4kB blocks (field DATA SECTOR SIZE of extended CSD register) */
    if (((hmmc->Ext_CSD[(MMC_EXT_CSD_DATA_SEC_SIZE_INDEX / 4)] >> MMC_EXT_CSD_DATA_SEC_SIZE_POS) & 0x000000FFU) != 0x0U)
    {
      if ((NumberOfBlocks % 8U) != 0U)
      {
        /* The number of blocks should be a multiple of 8 sectors of 512 bytes = 4 KBytes */
        hmmc->ErrorCode |= HAL_MMC_ERROR_BLOCK_LEN_ERR;
        return HAL_ERROR;
      }

      if ((BlockAdd % 8U) != 0U)
      {
        /* The address should be aligned to 8 (corresponding to 4 KBytes blocks) */
        hmmc->ErrorCode |= HAL_MMC_ERROR_ADDR_MISALIGNED;
        return HAL_ERROR;
      }
    }

    hmmc->State = HAL_MMC_STATE_BUSY;

    /* Initialize data control register */
    hmmc->Instance->DCTRL = 0U;

    hmmc->pRxBuffPtr = pData;
    hmmc->RxXferSize = MMC_BLOCKSIZE * NumberOfBlocks;

    if ((hmmc->MmcCard.CardType) != MMC_HIGH_CAPACITY_CARD)
    {
      add *= MMC_BLOCKSIZE;
    }

    /* Configure the MMC DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = MMC_BLOCKSIZE * NumberOfBlocks;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_SDMMC;
    config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hmmc->Instance, &config);
    __SDMMC_CMDTRANS_ENABLE(hmmc->Instance);

    /* Read Blocks in IT mode */
    if (NumberOfBlocks > 1U)
    {
      hmmc->Context = (MMC_CONTEXT_READ_MULTIPLE_BLOCK | MMC_CONTEXT_IT);

      /* Read Multi Block command */
      errorstate = SDMMC_CmdReadMultiBlock(hmmc->Instance, add);
    }
    else
    {
      hmmc->Context = (MMC_CONTEXT_READ_SINGLE_BLOCK | MMC_CONTEXT_IT);

      /* Read Single Block command */
      errorstate = SDMMC_CmdReadSingleBlock(hmmc->Instance, add);
    }

    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    __HAL_MMC_ENABLE_IT(hmmc, (SDMMC_IT_DCRCFAIL | SDMMC_IT_DTIMEOUT | SDMMC_IT_RXOVERR | SDMMC_IT_DATAEND |
                               SDMMC_FLAG_RXFIFOHF));

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Writes block(s) to a specified address in a card. The Data transfer
  *         is managed in interrupt mode.
  * @note   This API should be followed by a check on the card state through
  *         HAL_MMC_GetCardState().
  * @note   You could also check the IT transfer process through the MMC Tx
  *         interrupt event.
  * @param  hmmc: Pointer to MMC handle
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  BlockAdd: Block Address where data will be written
  * @param  NumberOfBlocks: Number of blocks to write
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_WriteBlocks_IT(MMC_HandleTypeDef *hmmc, const uint8_t *pData,
                                         uint32_t BlockAdd, uint32_t NumberOfBlocks)
{
  SDMMC_DataInitTypeDef config;
  uint32_t errorstate;
  uint32_t add = BlockAdd;

  if (NULL == pData)
  {
    hmmc->ErrorCode |= HAL_MMC_ERROR_PARAM;
    return HAL_ERROR;
  }

  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    hmmc->ErrorCode = HAL_MMC_ERROR_NONE;

    if ((BlockAdd + NumberOfBlocks) > (hmmc->MmcCard.LogBlockNbr))
    {
      hmmc->ErrorCode |= HAL_MMC_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }

    /* Check the case of 4kB blocks (field DATA SECTOR SIZE of extended CSD register) */
    if (((hmmc->Ext_CSD[(MMC_EXT_CSD_DATA_SEC_SIZE_INDEX / 4)] >> MMC_EXT_CSD_DATA_SEC_SIZE_POS) & 0x000000FFU) != 0x0U)
    {
      if ((NumberOfBlocks % 8U) != 0U)
      {
        /* The number of blocks should be a multiple of 8 sectors of 512 bytes = 4 KBytes */
        hmmc->ErrorCode |= HAL_MMC_ERROR_BLOCK_LEN_ERR;
        return HAL_ERROR;
      }

      if ((BlockAdd % 8U) != 0U)
      {
        /* The address should be aligned to 8 (corresponding to 4 KBytes blocks) */
        hmmc->ErrorCode |= HAL_MMC_ERROR_ADDR_MISALIGNED;
        return HAL_ERROR;
      }
    }

    hmmc->State = HAL_MMC_STATE_BUSY;

    /* Initialize data control register */
    hmmc->Instance->DCTRL = 0U;

    hmmc->pTxBuffPtr = pData;
    hmmc->TxXferSize = MMC_BLOCKSIZE * NumberOfBlocks;

    if ((hmmc->MmcCard.CardType) != MMC_HIGH_CAPACITY_CARD)
    {
      add *= MMC_BLOCKSIZE;
    }

    /* Configure the MMC DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = MMC_BLOCKSIZE * NumberOfBlocks;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_CARD;
    config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hmmc->Instance, &config);

    __SDMMC_CMDTRANS_ENABLE(hmmc->Instance);

    /* Write Blocks in Polling mode */
    if (NumberOfBlocks > 1U)
    {
      hmmc->Context = (MMC_CONTEXT_WRITE_MULTIPLE_BLOCK | MMC_CONTEXT_IT);

      /* Write Multi Block command */
      errorstate = SDMMC_CmdWriteMultiBlock(hmmc->Instance, add);
    }
    else
    {
      hmmc->Context = (MMC_CONTEXT_WRITE_SINGLE_BLOCK | MMC_CONTEXT_IT);

      /* Write Single Block command */
      errorstate = SDMMC_CmdWriteSingleBlock(hmmc->Instance, add);
    }
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Enable transfer interrupts */
    __HAL_MMC_ENABLE_IT(hmmc, (SDMMC_IT_DCRCFAIL | SDMMC_IT_DTIMEOUT | SDMMC_IT_TXUNDERR | SDMMC_IT_DATAEND |
                               SDMMC_FLAG_TXFIFOHE));

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Reads block(s) from a specified address in a card. The Data transfer
  *         is managed by DMA mode.
  * @note   This API should be followed by a check on the card state through
  *         HAL_MMC_GetCardState().
  * @note   You could also check the DMA transfer process through the MMC Rx
  *         interrupt event.
  * @param  hmmc: Pointer MMC handle
  * @param  pData: Pointer to the buffer that will contain the received data
  * @param  BlockAdd: Block Address from where data is to be read
  * @param  NumberOfBlocks: Number of blocks to read.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_ReadBlocks_DMA(MMC_HandleTypeDef *hmmc, uint8_t *pData, uint32_t BlockAdd,
                                         uint32_t NumberOfBlocks)
{
  SDMMC_DataInitTypeDef config;
  uint32_t errorstate;
  uint32_t add = BlockAdd;

  if (NULL == pData)
  {
    hmmc->ErrorCode |= HAL_MMC_ERROR_PARAM;
    return HAL_ERROR;
  }

  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    hmmc->ErrorCode = HAL_DMA_ERROR_NONE;

    if ((BlockAdd + NumberOfBlocks) > (hmmc->MmcCard.LogBlockNbr))
    {
      hmmc->ErrorCode |= HAL_MMC_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }

    /* Check the case of 4kB blocks (field DATA SECTOR SIZE of extended CSD register) */
    if (((hmmc->Ext_CSD[(MMC_EXT_CSD_DATA_SEC_SIZE_INDEX / 4)] >> MMC_EXT_CSD_DATA_SEC_SIZE_POS) & 0x000000FFU) != 0x0U)
    {
      if ((NumberOfBlocks % 8U) != 0U)
      {
        /* The number of blocks should be a multiple of 8 sectors of 512 bytes = 4 KBytes */
        hmmc->ErrorCode |= HAL_MMC_ERROR_BLOCK_LEN_ERR;
        return HAL_ERROR;
      }

      if ((BlockAdd % 8U) != 0U)
      {
        /* The address should be aligned to 8 (corresponding to 4 KBytes blocks) */
        hmmc->ErrorCode |= HAL_MMC_ERROR_ADDR_MISALIGNED;
        return HAL_ERROR;
      }
    }

    hmmc->State = HAL_MMC_STATE_BUSY;

    /* Initialize data control register */
    hmmc->Instance->DCTRL = 0U;

    hmmc->pRxBuffPtr = pData;
    hmmc->RxXferSize = MMC_BLOCKSIZE * NumberOfBlocks;

    if ((hmmc->MmcCard.CardType) != MMC_HIGH_CAPACITY_CARD)
    {
      add *= MMC_BLOCKSIZE;
    }

    /* Configure the MMC DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = MMC_BLOCKSIZE * NumberOfBlocks;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_SDMMC;
    config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hmmc->Instance, &config);

    __SDMMC_CMDTRANS_ENABLE(hmmc->Instance);
    hmmc->Instance->IDMABASER = (uint32_t) pData ;
    hmmc->Instance->IDMACTRL  = SDMMC_ENABLE_IDMA_SINGLE_BUFF;

    /* Read Blocks in DMA mode */
    if (NumberOfBlocks > 1U)
    {
      hmmc->Context = (MMC_CONTEXT_READ_MULTIPLE_BLOCK | MMC_CONTEXT_DMA);

      /* Read Multi Block command */
      errorstate = SDMMC_CmdReadMultiBlock(hmmc->Instance, add);
    }
    else
    {
      hmmc->Context = (MMC_CONTEXT_READ_SINGLE_BLOCK | MMC_CONTEXT_DMA);

      /* Read Single Block command */
      errorstate = SDMMC_CmdReadSingleBlock(hmmc->Instance, add);
    }
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode = errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Enable transfer interrupts */
    __HAL_MMC_ENABLE_IT(hmmc, (SDMMC_IT_DCRCFAIL | SDMMC_IT_DTIMEOUT | SDMMC_IT_RXOVERR | SDMMC_IT_DATAEND));

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Writes block(s) to a specified address in a card. The Data transfer
  *         is managed by DMA mode.
  * @note   This API should be followed by a check on the card state through
  *         HAL_MMC_GetCardState().
  * @note   You could also check the DMA transfer process through the MMC Tx
  *         interrupt event.
  * @param  hmmc: Pointer to MMC handle
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  BlockAdd: Block Address where data will be written
  * @param  NumberOfBlocks: Number of blocks to write
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_WriteBlocks_DMA(MMC_HandleTypeDef *hmmc, const uint8_t *pData,
                                          uint32_t BlockAdd, uint32_t NumberOfBlocks)
{
  SDMMC_DataInitTypeDef config;
  uint32_t errorstate;
  uint32_t add = BlockAdd;

  if (NULL == pData)
  {
    hmmc->ErrorCode |= HAL_MMC_ERROR_PARAM;
    return HAL_ERROR;
  }

  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    hmmc->ErrorCode = HAL_MMC_ERROR_NONE;

    if ((BlockAdd + NumberOfBlocks) > (hmmc->MmcCard.LogBlockNbr))
    {
      hmmc->ErrorCode |= HAL_MMC_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }

    /* Check the case of 4kB blocks (field DATA SECTOR SIZE of extended CSD register) */
    if (((hmmc->Ext_CSD[(MMC_EXT_CSD_DATA_SEC_SIZE_INDEX / 4)] >> MMC_EXT_CSD_DATA_SEC_SIZE_POS) & 0x000000FFU) != 0x0U)
    {
      if ((NumberOfBlocks % 8U) != 0U)
      {
        /* The number of blocks should be a multiple of 8 sectors of 512 bytes = 4 KBytes */
        hmmc->ErrorCode |= HAL_MMC_ERROR_BLOCK_LEN_ERR;
        return HAL_ERROR;
      }

      if ((BlockAdd % 8U) != 0U)
      {
        /* The address should be aligned to 8 (corresponding to 4 KBytes blocks) */
        hmmc->ErrorCode |= HAL_MMC_ERROR_ADDR_MISALIGNED;
        return HAL_ERROR;
      }
    }

    hmmc->State = HAL_MMC_STATE_BUSY;

    /* Initialize data control register */
    hmmc->Instance->DCTRL = 0U;

    hmmc->pTxBuffPtr = pData;
    hmmc->TxXferSize = MMC_BLOCKSIZE * NumberOfBlocks;

    if ((hmmc->MmcCard.CardType) != MMC_HIGH_CAPACITY_CARD)
    {
      add *= MMC_BLOCKSIZE;
    }

    /* Configure the MMC DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = MMC_BLOCKSIZE * NumberOfBlocks;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_CARD;
    config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hmmc->Instance, &config);

    __SDMMC_CMDTRANS_ENABLE(hmmc->Instance);

    hmmc->Instance->IDMABASER = (uint32_t) pData ;
    hmmc->Instance->IDMACTRL  = SDMMC_ENABLE_IDMA_SINGLE_BUFF;

    /* Write Blocks in Polling mode */
    if (NumberOfBlocks > 1U)
    {
      hmmc->Context = (MMC_CONTEXT_WRITE_MULTIPLE_BLOCK | MMC_CONTEXT_DMA);

      /* Write Multi Block command */
      errorstate = SDMMC_CmdWriteMultiBlock(hmmc->Instance, add);
    }
    else
    {
      hmmc->Context = (MMC_CONTEXT_WRITE_SINGLE_BLOCK | MMC_CONTEXT_DMA);

      /* Write Single Block command */
      errorstate = SDMMC_CmdWriteSingleBlock(hmmc->Instance, add);
    }
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Enable transfer interrupts */
    __HAL_MMC_ENABLE_IT(hmmc, (SDMMC_IT_DCRCFAIL | SDMMC_IT_DTIMEOUT | SDMMC_IT_TXUNDERR | SDMMC_IT_DATAEND));

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Erases the specified memory area of the given MMC card.
  * @note   This API should be followed by a check on the card state through
  *         HAL_MMC_GetCardState().
  * @param  hmmc: Pointer to MMC handle
  * @param  BlockStartAdd: Start Block address
  * @param  BlockEndAdd: End Block address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_Erase(MMC_HandleTypeDef *hmmc, uint32_t BlockStartAdd, uint32_t BlockEndAdd)
{
  uint32_t errorstate;
  uint32_t start_add = BlockStartAdd;
  uint32_t end_add = BlockEndAdd;

  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    hmmc->ErrorCode = HAL_MMC_ERROR_NONE;

    if (end_add < start_add)
    {
      hmmc->ErrorCode |= HAL_MMC_ERROR_PARAM;
      return HAL_ERROR;
    }

    if (end_add > (hmmc->MmcCard.LogBlockNbr))
    {
      hmmc->ErrorCode |= HAL_MMC_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }

    /* Check the case of 4kB blocks (field DATA SECTOR SIZE of extended CSD register) */
    if (((hmmc->Ext_CSD[(MMC_EXT_CSD_DATA_SEC_SIZE_INDEX / 4)] >> MMC_EXT_CSD_DATA_SEC_SIZE_POS)
         & 0x000000FFU) != 0x0U)
    {
      if (((start_add % 8U) != 0U) || ((end_add % 8U) != 0U))
      {
        /* The address should be aligned to 8 (corresponding to 4 KBytes blocks) */
        hmmc->ErrorCode |= HAL_MMC_ERROR_ADDR_MISALIGNED;
        return HAL_ERROR;
      }
    }

    hmmc->State = HAL_MMC_STATE_BUSY;

    /* Check if the card command class supports erase command */
    if (((hmmc->MmcCard.Class) & SDMMC_CCCC_ERASE) == 0U)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_REQUEST_NOT_APPLICABLE;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    if ((SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP1) & SDMMC_CARD_LOCKED) == SDMMC_CARD_LOCKED)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_LOCK_UNLOCK_FAILED;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    if ((hmmc->MmcCard.CardType) != MMC_HIGH_CAPACITY_CARD)
    {
      start_add *= MMC_BLOCKSIZE;
      end_add   *= MMC_BLOCKSIZE;
    }

    /* Send CMD35 MMC_ERASE_GRP_START with argument as addr  */
    errorstate = SDMMC_CmdEraseStartAdd(hmmc->Instance, start_add);
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Send CMD36 MMC_ERASE_GRP_END with argument as addr  */
    errorstate = SDMMC_CmdEraseEndAdd(hmmc->Instance, end_add);
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Send CMD38 ERASE */
    errorstate = SDMMC_CmdErase(hmmc->Instance, 0UL);
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    hmmc->State = HAL_MMC_STATE_READY;

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  This function handles MMC card interrupt request.
  * @param  hmmc: Pointer to MMC handle
  * @retval None
  */
void HAL_MMC_IRQHandler(MMC_HandleTypeDef *hmmc)
{
  uint32_t errorstate;
  uint32_t context = hmmc->Context;

  /* Check for SDMMC interrupt flags */
  if ((__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_RXFIFOHF) != RESET) && ((context & MMC_CONTEXT_IT) != 0U))
  {
    MMC_Read_IT(hmmc);
  }

  else if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DATAEND) != RESET)
  {
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_FLAG_DATAEND);

    __HAL_MMC_DISABLE_IT(hmmc, SDMMC_IT_DATAEND  | SDMMC_IT_DCRCFAIL | SDMMC_IT_DTIMEOUT | \
                         SDMMC_IT_TXUNDERR | SDMMC_IT_RXOVERR  | SDMMC_IT_TXFIFOHE | \
                         SDMMC_IT_RXFIFOHF);

    __HAL_MMC_DISABLE_IT(hmmc, SDMMC_IT_IDMABTC);
    __SDMMC_CMDTRANS_DISABLE(hmmc->Instance);

    if ((context & MMC_CONTEXT_DMA) != 0U)
    {
      hmmc->Instance->DLEN = 0;
      hmmc->Instance->DCTRL = 0;
      hmmc->Instance->IDMACTRL = SDMMC_DISABLE_IDMA ;

      /* Stop Transfer for Write Multi blocks or Read Multi blocks */
      if (((context & MMC_CONTEXT_READ_MULTIPLE_BLOCK) != 0U) || ((context & MMC_CONTEXT_WRITE_MULTIPLE_BLOCK) != 0U))
      {
        errorstate = SDMMC_CmdStopTransfer(hmmc->Instance);
        if (errorstate != HAL_MMC_ERROR_NONE)
        {
          hmmc->ErrorCode |= errorstate;
#if defined (USE_HAL_MMC_REGISTER_CALLBACKS) && (USE_HAL_MMC_REGISTER_CALLBACKS == 1U)
          hmmc->ErrorCallback(hmmc);
#else
          HAL_MMC_ErrorCallback(hmmc);
#endif /* USE_HAL_MMC_REGISTER_CALLBACKS */
        }
      }

      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_DATA_FLAGS);

      hmmc->State = HAL_MMC_STATE_READY;
      if (((context & MMC_CONTEXT_WRITE_SINGLE_BLOCK) != 0U) || ((context & MMC_CONTEXT_WRITE_MULTIPLE_BLOCK) != 0U))
      {
#if defined (USE_HAL_MMC_REGISTER_CALLBACKS) && (USE_HAL_MMC_REGISTER_CALLBACKS == 1U)
        hmmc->TxCpltCallback(hmmc);
#else
        HAL_MMC_TxCpltCallback(hmmc);
#endif /* USE_HAL_MMC_REGISTER_CALLBACKS */
      }
      if (((context & MMC_CONTEXT_READ_SINGLE_BLOCK) != 0U) || ((context & MMC_CONTEXT_READ_MULTIPLE_BLOCK) != 0U))
      {
#if defined (USE_HAL_MMC_REGISTER_CALLBACKS) && (USE_HAL_MMC_REGISTER_CALLBACKS == 1U)
        hmmc->RxCpltCallback(hmmc);
#else
        HAL_MMC_RxCpltCallback(hmmc);
#endif /* USE_HAL_MMC_REGISTER_CALLBACKS */
      }
    }
    else if ((context & MMC_CONTEXT_IT) != 0U)
    {
      /* Stop Transfer for Write Multi blocks or Read Multi blocks */
      if (((context & MMC_CONTEXT_READ_MULTIPLE_BLOCK) != 0U) || ((context & MMC_CONTEXT_WRITE_MULTIPLE_BLOCK) != 0U))
      {
        errorstate = SDMMC_CmdStopTransfer(hmmc->Instance);
        if (errorstate != HAL_MMC_ERROR_NONE)
        {
          hmmc->ErrorCode |= errorstate;
#if defined (USE_HAL_MMC_REGISTER_CALLBACKS) && (USE_HAL_MMC_REGISTER_CALLBACKS == 1U)
          hmmc->ErrorCallback(hmmc);
#else
          HAL_MMC_ErrorCallback(hmmc);
#endif /* USE_HAL_MMC_REGISTER_CALLBACKS */
        }
      }

      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_DATA_FLAGS);

      hmmc->State = HAL_MMC_STATE_READY;
      if (((context & MMC_CONTEXT_READ_SINGLE_BLOCK) != 0U) || ((context & MMC_CONTEXT_READ_MULTIPLE_BLOCK) != 0U))
      {
#if defined (USE_HAL_MMC_REGISTER_CALLBACKS) && (USE_HAL_MMC_REGISTER_CALLBACKS == 1U)
        hmmc->RxCpltCallback(hmmc);
#else
        HAL_MMC_RxCpltCallback(hmmc);
#endif /* USE_HAL_MMC_REGISTER_CALLBACKS */
      }
      else
      {
#if defined (USE_HAL_MMC_REGISTER_CALLBACKS) && (USE_HAL_MMC_REGISTER_CALLBACKS == 1U)
        hmmc->TxCpltCallback(hmmc);
#else
        HAL_MMC_TxCpltCallback(hmmc);
#endif /* USE_HAL_MMC_REGISTER_CALLBACKS */
      }
    }
    else
    {
      /* Nothing to do */
    }
  }

  else if ((__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_TXFIFOHE) != RESET) && ((context & MMC_CONTEXT_IT) != 0U))
  {
    MMC_Write_IT(hmmc);
  }

  else if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DCRCFAIL |
                              SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_RXOVERR | SDMMC_FLAG_TXUNDERR) != RESET)
  {
    /* Set Error code */
    if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_IT_DCRCFAIL) != RESET)
    {
      hmmc->ErrorCode |= HAL_MMC_ERROR_DATA_CRC_FAIL;
    }
    if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_IT_DTIMEOUT) != RESET)
    {
      hmmc->ErrorCode |= HAL_MMC_ERROR_DATA_TIMEOUT;
    }
    if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_IT_RXOVERR) != RESET)
    {
      hmmc->ErrorCode |= HAL_MMC_ERROR_RX_OVERRUN;
    }
    if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_IT_TXUNDERR) != RESET)
    {
      hmmc->ErrorCode |= HAL_MMC_ERROR_TX_UNDERRUN;
    }

    /* Clear All flags */
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_DATA_FLAGS);

    /* Disable all interrupts */
    __HAL_MMC_DISABLE_IT(hmmc, SDMMC_IT_DATAEND | SDMMC_IT_DCRCFAIL | SDMMC_IT_DTIMEOUT | \
                         SDMMC_IT_TXUNDERR | SDMMC_IT_RXOVERR);

    __SDMMC_CMDTRANS_DISABLE(hmmc->Instance);
    hmmc->Instance->DCTRL |= SDMMC_DCTRL_FIFORST;
    hmmc->Instance->CMD |= SDMMC_CMD_CMDSTOP;
    hmmc->ErrorCode |= SDMMC_CmdStopTransfer(hmmc->Instance);
    hmmc->Instance->CMD &= ~(SDMMC_CMD_CMDSTOP);
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_FLAG_DABORT);

    if ((context & MMC_CONTEXT_IT) != 0U)
    {
      /* Set the MMC state to ready to be able to start again the process */
      hmmc->State = HAL_MMC_STATE_READY;
#if defined (USE_HAL_MMC_REGISTER_CALLBACKS) && (USE_HAL_MMC_REGISTER_CALLBACKS == 1U)
      hmmc->ErrorCallback(hmmc);
#else
      HAL_MMC_ErrorCallback(hmmc);
#endif /* USE_HAL_MMC_REGISTER_CALLBACKS */
    }
    else if ((context & MMC_CONTEXT_DMA) != 0U)
    {
      if (hmmc->ErrorCode != HAL_MMC_ERROR_NONE)
      {
        /* Disable Internal DMA */
        __HAL_MMC_DISABLE_IT(hmmc, SDMMC_IT_IDMABTC);
        hmmc->Instance->IDMACTRL = SDMMC_DISABLE_IDMA;

        /* Set the MMC state to ready to be able to start again the process */
        hmmc->State = HAL_MMC_STATE_READY;
#if defined (USE_HAL_MMC_REGISTER_CALLBACKS) && (USE_HAL_MMC_REGISTER_CALLBACKS == 1U)
        hmmc->ErrorCallback(hmmc);
#else
        HAL_MMC_ErrorCallback(hmmc);
#endif /* USE_HAL_MMC_REGISTER_CALLBACKS */
      }
    }
    else
    {
      /* Nothing to do */
    }
  }

  else if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_IDMABTC) != RESET)
  {
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_IT_IDMABTC);

    if ((context & MMC_CONTEXT_WRITE_MULTIPLE_BLOCK) != 0U)
    {
#if defined (USE_HAL_MMC_REGISTER_CALLBACKS) && (USE_HAL_MMC_REGISTER_CALLBACKS == 1U)
      hmmc->Write_DMALnkLstBufCpltCallback(hmmc);
#else
      HAL_MMCEx_Write_DMALnkLstBufCpltCallback(hmmc);
#endif /* USE_HAL_MMC_REGISTER_CALLBACKS */
    }
    else /* MMC_CONTEXT_READ_MULTIPLE_BLOCK */
    {
#if defined (USE_HAL_MMC_REGISTER_CALLBACKS) && (USE_HAL_MMC_REGISTER_CALLBACKS == 1U)
      hmmc->Read_DMALnkLstBufCpltCallback(hmmc);
#else
      HAL_MMCEx_Read_DMALnkLstBufCpltCallback(hmmc);
#endif /* USE_HAL_MMC_REGISTER_CALLBACKS */
    }
  }

  else
  {
    /* Nothing to do */
  }
}

/**
  * @brief return the MMC state
  * @param hmmc: Pointer to mmc handle
  * @retval HAL state
  */
HAL_MMC_StateTypeDef HAL_MMC_GetState(const MMC_HandleTypeDef *hmmc)
{
  return hmmc->State;
}

/**
  * @brief  Return the MMC error code
  * @param  hmmc : Pointer to a MMC_HandleTypeDef structure that contains
  *              the configuration information.
  * @retval MMC Error Code
  */
uint32_t HAL_MMC_GetError(const MMC_HandleTypeDef *hmmc)
{
  return hmmc->ErrorCode;
}

/**
  * @brief Tx Transfer completed callbacks
  * @param hmmc: Pointer to MMC handle
  * @retval None
  */
__weak void HAL_MMC_TxCpltCallback(MMC_HandleTypeDef *hmmc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmmc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_MMC_TxCpltCallback can be implemented in the user file
   */
}

/**
  * @brief Rx Transfer completed callbacks
  * @param hmmc: Pointer MMC handle
  * @retval None
  */
__weak void HAL_MMC_RxCpltCallback(MMC_HandleTypeDef *hmmc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmmc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_MMC_RxCpltCallback can be implemented in the user file
   */
}

/**
  * @brief MMC error callbacks
  * @param hmmc: Pointer MMC handle
  * @retval None
  */
__weak void HAL_MMC_ErrorCallback(MMC_HandleTypeDef *hmmc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmmc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_MMC_ErrorCallback can be implemented in the user file
   */
}

/**
  * @brief MMC Abort callbacks
  * @param hmmc: Pointer MMC handle
  * @retval None
  */
__weak void HAL_MMC_AbortCallback(MMC_HandleTypeDef *hmmc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmmc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_MMC_AbortCallback can be implemented in the user file
   */
}

#if defined (USE_HAL_MMC_REGISTER_CALLBACKS) && (USE_HAL_MMC_REGISTER_CALLBACKS == 1U)
/**
  * @brief  Register a User MMC Callback
  *         To be used instead of the weak (overridden) predefined callback
  * @note   The HAL_MMC_RegisterCallback() may be called before HAL_MMC_Init() in
  *         HAL_MMC_STATE_RESET to register callbacks for HAL_MMC_MSP_INIT_CB_ID
  *         and HAL_MMC_MSP_DEINIT_CB_ID.
  * @param hmmc : MMC handle
  * @param CallbackId : ID of the callback to be registered
  *        This parameter can be one of the following values:
  *          @arg @ref HAL_MMC_TX_CPLT_CB_ID                 MMC Tx Complete Callback ID
  *          @arg @ref HAL_MMC_RX_CPLT_CB_ID                 MMC Rx Complete Callback ID
  *          @arg @ref HAL_MMC_ERROR_CB_ID                   MMC Error Callback ID
  *          @arg @ref HAL_MMC_ABORT_CB_ID                   MMC Abort Callback ID
  *          @arg @ref HAL_MMC_READ_DMA_LNKLST_BUF_CPLT_CB_ID  MMC DMA Rx Linked List Node buffer Callback ID
  *          @arg @ref HAL_MMC_WRITE_DMA_LNKLST_BUF_CPLT_CB_ID MMC DMA Tx Linked List Node buffer Callback ID
  *          @arg @ref HAL_MMC_MSP_INIT_CB_ID                MMC MspInit Callback ID
  *          @arg @ref HAL_MMC_MSP_DEINIT_CB_ID              MMC MspDeInit Callback ID
  * @param pCallback : pointer to the Callback function
  * @retval status
  */
HAL_StatusTypeDef HAL_MMC_RegisterCallback(MMC_HandleTypeDef *hmmc, HAL_MMC_CallbackIDTypeDef CallbackId,
                                           pMMC_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hmmc->ErrorCode |= HAL_MMC_ERROR_INVALID_CALLBACK;
    return HAL_ERROR;
  }

  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    switch (CallbackId)
    {
      case HAL_MMC_TX_CPLT_CB_ID :
        hmmc->TxCpltCallback = pCallback;
        break;
      case HAL_MMC_RX_CPLT_CB_ID :
        hmmc->RxCpltCallback = pCallback;
        break;
      case HAL_MMC_ERROR_CB_ID :
        hmmc->ErrorCallback = pCallback;
        break;
      case HAL_MMC_ABORT_CB_ID :
        hmmc->AbortCpltCallback = pCallback;
        break;
      case HAL_MMC_READ_DMA_LNKLST_BUF_CPLT_CB_ID :
        hmmc->Read_DMALnkLstBufCpltCallback = pCallback;
        break;
      case HAL_MMC_WRITE_DMA_LNKLST_BUF_CPLT_CB_ID :
        hmmc->Write_DMALnkLstBufCpltCallback = pCallback;
        break;
      case HAL_MMC_MSP_INIT_CB_ID :
        hmmc->MspInitCallback = pCallback;
        break;
      case HAL_MMC_MSP_DEINIT_CB_ID :
        hmmc->MspDeInitCallback = pCallback;
        break;
      default :
        /* Update the error code */
        hmmc->ErrorCode |= HAL_MMC_ERROR_INVALID_CALLBACK;
        /* update return status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (hmmc->State == HAL_MMC_STATE_RESET)
  {
    switch (CallbackId)
    {
      case HAL_MMC_MSP_INIT_CB_ID :
        hmmc->MspInitCallback = pCallback;
        break;
      case HAL_MMC_MSP_DEINIT_CB_ID :
        hmmc->MspDeInitCallback = pCallback;
        break;
      default :
        /* Update the error code */
        hmmc->ErrorCode |= HAL_MMC_ERROR_INVALID_CALLBACK;
        /* update return status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hmmc->ErrorCode |= HAL_MMC_ERROR_INVALID_CALLBACK;
    /* update return status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Unregister a User MMC Callback
  *         MMC Callback is redirected to the weak (overridden) predefined callback
  * @note   The HAL_MMC_UnRegisterCallback() may be called before HAL_MMC_Init() in
  *         HAL_MMC_STATE_RESET to register callbacks for HAL_MMC_MSP_INIT_CB_ID
  *         and HAL_MMC_MSP_DEINIT_CB_ID.
  * @param hmmc : MMC handle
  * @param CallbackId : ID of the callback to be unregistered
  *        This parameter can be one of the following values:
  *          @arg @ref HAL_MMC_TX_CPLT_CB_ID                 MMC Tx Complete Callback ID
  *          @arg @ref HAL_MMC_RX_CPLT_CB_ID                 MMC Rx Complete Callback ID
  *          @arg @ref HAL_MMC_ERROR_CB_ID                   MMC Error Callback ID
  *          @arg @ref HAL_MMC_ABORT_CB_ID                   MMC Abort Callback ID
  *          @arg @ref HAL_MMC_READ_DMA_LNKLST_BUF_CPLT_CB_ID  MMC DMA Rx Linked List Node buffer Callback ID
  *          @arg @ref HAL_MMC_WRITE_DMA_LNKLST_BUF_CPLT_CB_ID MMC DMA Tx Linked List Node buffer Callback ID
  *          @arg @ref HAL_MMC_MSP_INIT_CB_ID                MMC MspInit Callback ID
  *          @arg @ref HAL_MMC_MSP_DEINIT_CB_ID              MMC MspDeInit Callback ID
  * @retval status
  */
HAL_StatusTypeDef HAL_MMC_UnRegisterCallback(MMC_HandleTypeDef *hmmc, HAL_MMC_CallbackIDTypeDef CallbackId)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    switch (CallbackId)
    {
      case HAL_MMC_TX_CPLT_CB_ID :
        hmmc->TxCpltCallback = HAL_MMC_TxCpltCallback;
        break;
      case HAL_MMC_RX_CPLT_CB_ID :
        hmmc->RxCpltCallback = HAL_MMC_RxCpltCallback;
        break;
      case HAL_MMC_ERROR_CB_ID :
        hmmc->ErrorCallback = HAL_MMC_ErrorCallback;
        break;
      case HAL_MMC_ABORT_CB_ID :
        hmmc->AbortCpltCallback = HAL_MMC_AbortCallback;
        break;
      case HAL_MMC_READ_DMA_LNKLST_BUF_CPLT_CB_ID :
        hmmc->Read_DMALnkLstBufCpltCallback = HAL_MMCEx_Read_DMALnkLstBufCpltCallback;
        break;
      case HAL_MMC_WRITE_DMA_LNKLST_BUF_CPLT_CB_ID :
        hmmc->Write_DMALnkLstBufCpltCallback = HAL_MMCEx_Write_DMALnkLstBufCpltCallback;
        break;
      case HAL_MMC_MSP_INIT_CB_ID :
        hmmc->MspInitCallback = HAL_MMC_MspInit;
        break;
      case HAL_MMC_MSP_DEINIT_CB_ID :
        hmmc->MspDeInitCallback = HAL_MMC_MspDeInit;
        break;
      default :
        /* Update the error code */
        hmmc->ErrorCode |= HAL_MMC_ERROR_INVALID_CALLBACK;
        /* update return status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (hmmc->State == HAL_MMC_STATE_RESET)
  {
    switch (CallbackId)
    {
      case HAL_MMC_MSP_INIT_CB_ID :
        hmmc->MspInitCallback = HAL_MMC_MspInit;
        break;
      case HAL_MMC_MSP_DEINIT_CB_ID :
        hmmc->MspDeInitCallback = HAL_MMC_MspDeInit;
        break;
      default :
        /* Update the error code */
        hmmc->ErrorCode |= HAL_MMC_ERROR_INVALID_CALLBACK;
        /* update return status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hmmc->ErrorCode |= HAL_MMC_ERROR_INVALID_CALLBACK;
    /* update return status */
    status =  HAL_ERROR;
  }

  return status;
}
#endif /* USE_HAL_MMC_REGISTER_CALLBACKS */

/**
  * @}
  */

/** @addtogroup MMC_Exported_Functions_Group3
  *  @brief   management functions
  *
@verbatim
  ==============================================================================
                      ##### Peripheral Control functions #####
  ==============================================================================
  [..]
    This subsection provides a set of functions allowing to control the MMC card
    operations and get the related information

@endverbatim
  * @{
  */

/**
  * @brief  Returns information the information of the card which are stored on
  *         the CID register.
  * @param  hmmc: Pointer to MMC handle
  * @param  pCID: Pointer to a HAL_MMC_CIDTypedef structure that
  *         contains all CID register parameters
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_GetCardCID(const MMC_HandleTypeDef *hmmc, HAL_MMC_CardCIDTypeDef *pCID)
{
  pCID->ManufacturerID = (uint8_t)((hmmc->CID[0] & 0xFF000000U) >> 24U);

  pCID->OEM_AppliID = (uint16_t)((hmmc->CID[0] & 0x00FFFF00U) >> 8U);

  pCID->ProdName1 = (((hmmc->CID[0] & 0x000000FFU) << 24U) | ((hmmc->CID[1] & 0xFFFFFF00U) >> 8U));

  pCID->ProdName2 = (uint8_t)(hmmc->CID[1] & 0x000000FFU);

  pCID->ProdRev = (uint8_t)((hmmc->CID[2] & 0xFF000000U) >> 24U);

  pCID->ProdSN = (((hmmc->CID[2] & 0x00FFFFFFU) << 8U) | ((hmmc->CID[3] & 0xFF000000U) >> 24U));

  pCID->Reserved1 = (uint8_t)((hmmc->CID[3] & 0x00F00000U) >> 20U);

  pCID->ManufactDate = (uint16_t)((hmmc->CID[3] & 0x000FFF00U) >> 8U);

  pCID->CID_CRC = (uint8_t)((hmmc->CID[3] & 0x000000FEU) >> 1U);

  pCID->Reserved2 = 1U;

  return HAL_OK;
}

/**
  * @brief  Returns information the information of the card which are stored on
  *         the CSD register.
  * @param  hmmc: Pointer to MMC handle
  * @param  pCSD: Pointer to a HAL_MMC_CardCSDTypeDef structure that
  *         contains all CSD register parameters
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_GetCardCSD(MMC_HandleTypeDef *hmmc, HAL_MMC_CardCSDTypeDef *pCSD)
{
  uint32_t block_nbr = 0;

  pCSD->CSDStruct = (uint8_t)((hmmc->CSD[0] & 0xC0000000U) >> 30U);

  pCSD->SysSpecVersion = (uint8_t)((hmmc->CSD[0] & 0x3C000000U) >> 26U);

  pCSD->Reserved1 = (uint8_t)((hmmc->CSD[0] & 0x03000000U) >> 24U);

  pCSD->TAAC = (uint8_t)((hmmc->CSD[0] & 0x00FF0000U) >> 16U);

  pCSD->NSAC = (uint8_t)((hmmc->CSD[0] & 0x0000FF00U) >> 8U);

  pCSD->MaxBusClkFrec = (uint8_t)(hmmc->CSD[0] & 0x000000FFU);

  pCSD->CardComdClasses = (uint16_t)((hmmc->CSD[1] & 0xFFF00000U) >> 20U);

  pCSD->RdBlockLen = (uint8_t)((hmmc->CSD[1] & 0x000F0000U) >> 16U);

  pCSD->PartBlockRead   = (uint8_t)((hmmc->CSD[1] & 0x00008000U) >> 15U);

  pCSD->WrBlockMisalign = (uint8_t)((hmmc->CSD[1] & 0x00004000U) >> 14U);

  pCSD->RdBlockMisalign = (uint8_t)((hmmc->CSD[1] & 0x00002000U) >> 13U);

  pCSD->DSRImpl = (uint8_t)((hmmc->CSD[1] & 0x00001000U) >> 12U);

  pCSD->Reserved2 = 0U; /*!< Reserved */

  if (MMC_ReadExtCSD(hmmc, &block_nbr, 212, 0x0FFFFFFFU) != HAL_OK) /* Field SEC_COUNT [215:212] */
  {
    return HAL_ERROR;
  }

  if (hmmc->MmcCard.CardType == MMC_LOW_CAPACITY_CARD)
  {
    pCSD->DeviceSize = (((hmmc->CSD[1] & 0x000003FFU) << 2U) | ((hmmc->CSD[2] & 0xC0000000U) >> 30U));

    pCSD->MaxRdCurrentVDDMin = (uint8_t)((hmmc->CSD[2] & 0x38000000U) >> 27U);

    pCSD->MaxRdCurrentVDDMax = (uint8_t)((hmmc->CSD[2] & 0x07000000U) >> 24U);

    pCSD->MaxWrCurrentVDDMin = (uint8_t)((hmmc->CSD[2] & 0x00E00000U) >> 21U);

    pCSD->MaxWrCurrentVDDMax = (uint8_t)((hmmc->CSD[2] & 0x001C0000U) >> 18U);

    pCSD->DeviceSizeMul = (uint8_t)((hmmc->CSD[2] & 0x00038000U) >> 15U);

    hmmc->MmcCard.BlockNbr  = (pCSD->DeviceSize + 1U) ;
    hmmc->MmcCard.BlockNbr *= (1UL << ((pCSD->DeviceSizeMul & 0x07U) + 2U));
    hmmc->MmcCard.BlockSize = (1UL << (pCSD->RdBlockLen & 0x0FU));

    hmmc->MmcCard.LogBlockNbr = (hmmc->MmcCard.BlockNbr) * ((hmmc->MmcCard.BlockSize) / MMC_BLOCKSIZE);
    hmmc->MmcCard.LogBlockSize = MMC_BLOCKSIZE;
  }
  else if (hmmc->MmcCard.CardType == MMC_HIGH_CAPACITY_CARD)
  {
    hmmc->MmcCard.BlockNbr = block_nbr;
    hmmc->MmcCard.LogBlockNbr = hmmc->MmcCard.BlockNbr;
    hmmc->MmcCard.BlockSize = MMC_BLOCKSIZE;
    hmmc->MmcCard.LogBlockSize = hmmc->MmcCard.BlockSize;
  }
  else
  {
    /* Clear all the static flags */
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
    hmmc->ErrorCode |= HAL_MMC_ERROR_UNSUPPORTED_FEATURE;
    hmmc->State = HAL_MMC_STATE_READY;
    return HAL_ERROR;
  }

  pCSD->EraseGrSize = (uint8_t)((hmmc->CSD[2] & 0x00004000U) >> 14U);

  pCSD->EraseGrMul = (uint8_t)((hmmc->CSD[2] & 0x00003F80U) >> 7U);

  pCSD->WrProtectGrSize = (uint8_t)(hmmc->CSD[2] & 0x0000007FU);

  pCSD->WrProtectGrEnable = (uint8_t)((hmmc->CSD[3] & 0x80000000U) >> 31U);

  pCSD->ManDeflECC = (uint8_t)((hmmc->CSD[3] & 0x60000000U) >> 29U);

  pCSD->WrSpeedFact = (uint8_t)((hmmc->CSD[3] & 0x1C000000U) >> 26U);

  pCSD->MaxWrBlockLen = (uint8_t)((hmmc->CSD[3] & 0x03C00000U) >> 22U);

  pCSD->WriteBlockPaPartial = (uint8_t)((hmmc->CSD[3] & 0x00200000U) >> 21U);

  pCSD->Reserved3 = 0;

  pCSD->ContentProtectAppli = (uint8_t)((hmmc->CSD[3] & 0x00010000U) >> 16U);

  pCSD->FileFormatGroup = (uint8_t)((hmmc->CSD[3] & 0x00008000U) >> 15U);

  pCSD->CopyFlag = (uint8_t)((hmmc->CSD[3] & 0x00004000U) >> 14U);

  pCSD->PermWrProtect = (uint8_t)((hmmc->CSD[3] & 0x00002000U) >> 13U);

  pCSD->TempWrProtect = (uint8_t)((hmmc->CSD[3] & 0x00001000U) >> 12U);

  pCSD->FileFormat = (uint8_t)((hmmc->CSD[3] & 0x00000C00U) >> 10U);

  pCSD->ECC = (uint8_t)((hmmc->CSD[3] & 0x00000300U) >> 8U);

  pCSD->CSD_CRC = (uint8_t)((hmmc->CSD[3] & 0x000000FEU) >> 1U);

  pCSD->Reserved4 = 1;

  return HAL_OK;
}

/**
  * @brief  Gets the MMC card info.
  * @param  hmmc: Pointer to MMC handle
  * @param  pCardInfo: Pointer to the HAL_MMC_CardInfoTypeDef structure that
  *         will contain the MMC card status information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_GetCardInfo(const MMC_HandleTypeDef *hmmc, HAL_MMC_CardInfoTypeDef *pCardInfo)
{
  pCardInfo->CardType     = (uint32_t)(hmmc->MmcCard.CardType);
  pCardInfo->Class        = (uint32_t)(hmmc->MmcCard.Class);
  pCardInfo->RelCardAdd   = (uint32_t)(hmmc->MmcCard.RelCardAdd);
  pCardInfo->BlockNbr     = (uint32_t)(hmmc->MmcCard.BlockNbr);
  pCardInfo->BlockSize    = (uint32_t)(hmmc->MmcCard.BlockSize);
  pCardInfo->LogBlockNbr  = (uint32_t)(hmmc->MmcCard.LogBlockNbr);
  pCardInfo->LogBlockSize = (uint32_t)(hmmc->MmcCard.LogBlockSize);

  return HAL_OK;
}

/**
  * @brief  Returns information the information of the card which are stored on
  *         the Extended CSD register.
  * @param  hmmc Pointer to MMC handle
  * @param  pExtCSD Pointer to a memory area (512 bytes) that contains all
  *         Extended CSD register parameters
  * @param  Timeout Specify timeout value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_GetCardExtCSD(MMC_HandleTypeDef *hmmc, uint32_t *pExtCSD, uint32_t Timeout)
{
  SDMMC_DataInitTypeDef config;
  uint32_t errorstate;
  uint32_t tickstart = HAL_GetTick();
  uint32_t count;
  uint32_t dataremaining;
  uint32_t *tmp_buf;

  if (NULL == pExtCSD)
  {
    hmmc->ErrorCode |= HAL_MMC_ERROR_PARAM;
    return HAL_ERROR;
  }

  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    hmmc->ErrorCode = HAL_MMC_ERROR_NONE;

    hmmc->State = HAL_MMC_STATE_BUSY;

    /* Initialize data control register */
    hmmc->Instance->DCTRL = 0;

    /* Initiaize the destination pointer */
    tmp_buf = pExtCSD;

    /* Configure the MMC DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = MMC_BLOCKSIZE;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_SDMMC;
    config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hmmc->Instance, &config);
    __SDMMC_CMDTRANS_ENABLE(hmmc->Instance);

    /* Send ExtCSD Read command to Card */
    errorstate = SDMMC_CmdSendEXTCSD(hmmc->Instance, 0);
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Poll on SDMMC flags */
    dataremaining = config.DataLength;
    while (!__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_RXOVERR |
                               SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DATAEND))
    {
      if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_RXFIFOHF) && (dataremaining >= SDMMC_FIFO_SIZE))
      {
        /* Read data from SDMMC Rx FIFO */
        for (count = 0U; count < (SDMMC_FIFO_SIZE / 4U); count++)
        {
          *tmp_buf = SDMMC_ReadFIFO(hmmc->Instance);
          tmp_buf++;
        }
        dataremaining -= SDMMC_FIFO_SIZE;
      }

      if (((HAL_GetTick() - tickstart) >=  Timeout) || (Timeout == 0U))
      {
        /* Clear all the static flags */
        __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
        hmmc->ErrorCode |= HAL_MMC_ERROR_TIMEOUT;
        hmmc->State = HAL_MMC_STATE_READY;
        return HAL_TIMEOUT;
      }
    }

    __SDMMC_CMDTRANS_DISABLE(hmmc->Instance);

    /* Get error state */
    if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DTIMEOUT))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_DATA_TIMEOUT;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }
    else if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DCRCFAIL))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_DATA_CRC_FAIL;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }
    else if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_RXOVERR))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_RX_OVERRUN;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }
    else
    {
      /* Nothing to do */
    }

    /* Clear all the static flags */
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_DATA_FLAGS);
    hmmc->State = HAL_MMC_STATE_READY;
  }

  return HAL_OK;
}

/**
  * @brief  Enables wide bus operation for the requested card if supported by
  *         card.
  * @param  hmmc: Pointer to MMC handle
  * @param  WideMode: Specifies the MMC card wide bus mode
  *          This parameter can be one of the following values:
  *            @arg SDMMC_BUS_WIDE_8B: 8-bit data transfer
  *            @arg SDMMC_BUS_WIDE_4B: 4-bit data transfer
  *            @arg SDMMC_BUS_WIDE_1B: 1-bit data transfer
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_ConfigWideBusOperation(MMC_HandleTypeDef *hmmc, uint32_t WideMode)
{
  uint32_t count;
  SDMMC_InitTypeDef Init;
  uint32_t errorstate;
  uint32_t response = 0U;

  /* Check the parameters */
  assert_param(IS_SDMMC_BUS_WIDE(WideMode));

  /* Change State */
  hmmc->State = HAL_MMC_STATE_BUSY;

  /* Check and update the power class if needed */
  if ((hmmc->Instance->CLKCR & SDMMC_CLKCR_BUSSPEED) != 0U)
  {
    if ((hmmc->Instance->CLKCR & SDMMC_CLKCR_DDR) != 0U)
    {
      errorstate = MMC_PwrClassUpdate(hmmc, WideMode, SDMMC_SPEED_MODE_DDR);
    }
    else
    {
      errorstate = MMC_PwrClassUpdate(hmmc, WideMode, SDMMC_SPEED_MODE_HIGH);
    }
  }
  else
  {
    errorstate = MMC_PwrClassUpdate(hmmc, WideMode, SDMMC_SPEED_MODE_DEFAULT);
  }

  if (errorstate == HAL_MMC_ERROR_NONE)
  {
    if (WideMode == SDMMC_BUS_WIDE_8B)
    {
      errorstate = SDMMC_CmdSwitch(hmmc->Instance, 0x03B70200U);
    }
    else if (WideMode == SDMMC_BUS_WIDE_4B)
    {
      errorstate = SDMMC_CmdSwitch(hmmc->Instance, 0x03B70100U);
    }
    else if (WideMode == SDMMC_BUS_WIDE_1B)
    {
      errorstate = SDMMC_CmdSwitch(hmmc->Instance, 0x03B70000U);
    }
    else
    {
      /* WideMode is not a valid argument*/
      errorstate = HAL_MMC_ERROR_PARAM;
    }

    /* Check for switch error and violation of the trial number of sending CMD 13 */
    if (errorstate == HAL_MMC_ERROR_NONE)
    {
      /* While card is not ready for data and trial number for sending CMD13 is not exceeded */
      count = SDMMC_MAX_TRIAL;
      do
      {
        errorstate = SDMMC_CmdSendStatus(hmmc->Instance, (uint32_t)(((uint32_t)hmmc->MmcCard.RelCardAdd) << 16U));
        if (errorstate != HAL_MMC_ERROR_NONE)
        {
          break;
        }

        /* Get command response */
        response = SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP1);
        count--;
      } while (((response & 0x100U) == 0U) && (count != 0U));

      /* Check the status after the switch command execution */
      if ((count != 0U) && (errorstate == HAL_MMC_ERROR_NONE))
      {
        /* Check the bit SWITCH_ERROR of the device status */
        if ((response & 0x80U) != 0U)
        {
          errorstate = SDMMC_ERROR_GENERAL_UNKNOWN_ERR;
        }
        else
        {
          /* Configure the SDMMC peripheral */
          Init = hmmc->Init;
          Init.BusWide = WideMode;
          (void)SDMMC_Init(hmmc->Instance, Init);
        }
      }
      else if (count == 0U)
      {
        errorstate = SDMMC_ERROR_TIMEOUT;
      }
      else
      {
        /* Nothing to do */
      }
    }
  }

  /* Change State */
  hmmc->State = HAL_MMC_STATE_READY;

  if (errorstate != HAL_MMC_ERROR_NONE)
  {
    /* Clear all the static flags */
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
    hmmc->ErrorCode |= errorstate;
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Configure the speed bus mode
  * @param  hmmc: Pointer to the MMC handle
  * @param  SpeedMode: Specifies the MMC card speed bus mode
  *          This parameter can be one of the following values:
  *            @arg SDMMC_SPEED_MODE_AUTO: Max speed mode supported by the card
  *            @arg SDMMC_SPEED_MODE_DEFAULT: Default Speed (MMC @ 26MHz)
  *            @arg SDMMC_SPEED_MODE_HIGH: High Speed (MMC @ 52 MHz)
  *            @arg SDMMC_SPEED_MODE_DDR: High Speed DDR (MMC DDR @ 52 MHz)
  * @retval HAL status
  */

HAL_StatusTypeDef HAL_MMC_ConfigSpeedBusOperation(MMC_HandleTypeDef *hmmc, uint32_t SpeedMode)
{
  uint32_t tickstart;
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t device_type;
  uint32_t errorstate;

  /* Check the parameters */
  assert_param(IS_SDMMC_SPEED_MODE(SpeedMode));

  /* Change State */
  hmmc->State = HAL_MMC_STATE_BUSY;

  /* Field DEVICE_TYPE [196 = 49*4] of Extended CSD register */
  device_type = (hmmc->Ext_CSD[49] & 0x000000FFU);

  switch (SpeedMode)
  {
    case SDMMC_SPEED_MODE_AUTO:
    {
      if (((hmmc->Instance->CLKCR & SDMMC_CLKCR_WIDBUS) != 0U) && ((device_type & 0x04U) != 0U))
      {
        /* High Speed DDR mode allowed */
        errorstate = MMC_HighSpeed(hmmc, ENABLE);
        if (errorstate != HAL_MMC_ERROR_NONE)
        {
          hmmc->ErrorCode |= errorstate;
        }
        else
        {
          if ((hmmc->Instance->CLKCR & SDMMC_CLKCR_CLKDIV) != 0U)
          {
            /* DDR mode not supported with CLKDIV = 0 */
            errorstate = MMC_DDR_Mode(hmmc, ENABLE);
            if (errorstate != HAL_MMC_ERROR_NONE)
            {
              hmmc->ErrorCode |= errorstate;
            }
          }
        }
      }
      else if ((device_type & 0x02U) != 0U)
      {
        /* High Speed mode allowed */
        errorstate = MMC_HighSpeed(hmmc, ENABLE);
        if (errorstate != HAL_MMC_ERROR_NONE)
        {
          hmmc->ErrorCode |= errorstate;
        }
      }
      else
      {
        /* Nothing to do : keep current speed */
      }
      break;
    }
    case SDMMC_SPEED_MODE_DDR:
    {
      if (((hmmc->Instance->CLKCR & SDMMC_CLKCR_WIDBUS) != 0U) && ((device_type & 0x04U) != 0U))
      {
        /* High Speed DDR mode allowed */
        errorstate = MMC_HighSpeed(hmmc, ENABLE);
        if (errorstate != HAL_MMC_ERROR_NONE)
        {
          hmmc->ErrorCode |= errorstate;
        }
        else
        {
          if ((hmmc->Instance->CLKCR & SDMMC_CLKCR_CLKDIV) != 0U)
          {
            /* DDR mode not supported with CLKDIV = 0 */
            errorstate = MMC_DDR_Mode(hmmc, ENABLE);
            if (errorstate != HAL_MMC_ERROR_NONE)
            {
              hmmc->ErrorCode |= errorstate;
            }
          }
        }
      }
      else
      {
        /* High Speed DDR mode not allowed */
        hmmc->ErrorCode |= HAL_MMC_ERROR_UNSUPPORTED_FEATURE;
        status = HAL_ERROR;
      }
      break;
    }
    case SDMMC_SPEED_MODE_HIGH:
    {
      if ((device_type & 0x02U) != 0U)
      {
        /* High Speed mode allowed */
        errorstate = MMC_HighSpeed(hmmc, ENABLE);
        if (errorstate != HAL_MMC_ERROR_NONE)
        {
          hmmc->ErrorCode |= errorstate;
        }
      }
      else
      {
        /* High Speed mode not allowed */
        hmmc->ErrorCode |= HAL_MMC_ERROR_UNSUPPORTED_FEATURE;
        status = HAL_ERROR;
      }
      break;
    }
    case SDMMC_SPEED_MODE_DEFAULT:
    {
      if ((hmmc->Instance->CLKCR & SDMMC_CLKCR_DDR) != 0U)
      {
        /* High Speed DDR mode activated */
        errorstate = MMC_DDR_Mode(hmmc, DISABLE);
        if (errorstate != HAL_MMC_ERROR_NONE)
        {
          hmmc->ErrorCode |= errorstate;
        }
      }
      if ((hmmc->Instance->CLKCR & SDMMC_CLKCR_BUSSPEED) != 0U)
      {
        /* High Speed mode activated */
        errorstate = MMC_HighSpeed(hmmc, DISABLE);
        if (errorstate != HAL_MMC_ERROR_NONE)
        {
          hmmc->ErrorCode |= errorstate;
        }
      }
      break;
    }
    default:
      hmmc->ErrorCode |= HAL_MMC_ERROR_PARAM;
      status = HAL_ERROR;
      break;
  }

  /* Verify that MMC card is ready to use after Speed mode switch*/
  tickstart = HAL_GetTick();
  while ((HAL_MMC_GetCardState(hmmc) != HAL_MMC_CARD_TRANSFER))
  {
    if ((HAL_GetTick() - tickstart) >=  SDMMC_DATATIMEOUT)
    {
      hmmc->ErrorCode = HAL_MMC_ERROR_TIMEOUT;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_TIMEOUT;
    }
  }

  /* Change State */
  hmmc->State = HAL_MMC_STATE_READY;
  return status;
}

/**
  * @brief  Gets the current mmc card data state.
  * @param  hmmc: pointer to MMC handle
  * @retval Card state
  */
HAL_MMC_CardStateTypeDef HAL_MMC_GetCardState(MMC_HandleTypeDef *hmmc)
{
  uint32_t cardstate;
  uint32_t errorstate;
  uint32_t resp1 = 0U;

  errorstate = MMC_SendStatus(hmmc, &resp1);
  if (errorstate != HAL_MMC_ERROR_NONE)
  {
    hmmc->ErrorCode |= errorstate;
  }

  cardstate = ((resp1 >> 9U) & 0x0FU);

  return (HAL_MMC_CardStateTypeDef)cardstate;
}

/**
  * @brief  Abort the current transfer and disable the MMC.
  * @param  hmmc: pointer to a MMC_HandleTypeDef structure that contains
  *                the configuration information for MMC module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_Abort(MMC_HandleTypeDef *hmmc)
{
  uint32_t error_code;
  uint32_t tickstart;

  if (hmmc->State == HAL_MMC_STATE_BUSY)
  {
    /* DIsable All interrupts */
    __HAL_MMC_DISABLE_IT(hmmc, SDMMC_IT_DATAEND | SDMMC_IT_DCRCFAIL | SDMMC_IT_DTIMEOUT | \
                         SDMMC_IT_TXUNDERR | SDMMC_IT_RXOVERR);
    __SDMMC_CMDTRANS_DISABLE(hmmc->Instance);

    /*we will send the CMD12 in all cases in order to stop the data transfers*/
    /*In case the data transfer just finished, the external memory will not respond
      and will return HAL_MMC_ERROR_CMD_RSP_TIMEOUT*/
    /*In case the data transfer aborted , the external memory will respond and will return HAL_MMC_ERROR_NONE*/
    /*Other scenario will return HAL_ERROR*/

    hmmc->ErrorCode = SDMMC_CmdStopTransfer(hmmc->Instance);
    error_code = hmmc->ErrorCode;
    if ((error_code != HAL_MMC_ERROR_NONE) && (error_code != HAL_MMC_ERROR_CMD_RSP_TIMEOUT))
    {
      return HAL_ERROR;
    }

    tickstart = HAL_GetTick();
    if ((hmmc->Instance->DCTRL & SDMMC_DCTRL_DTDIR) == SDMMC_TRANSFER_DIR_TO_CARD)
    {
      if (hmmc->ErrorCode == HAL_MMC_ERROR_NONE)
      {
        while (!__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DABORT | SDMMC_FLAG_BUSYD0END))
        {
          if ((HAL_GetTick() - tickstart) >=  SDMMC_DATATIMEOUT)
          {
            hmmc->ErrorCode = HAL_MMC_ERROR_TIMEOUT;
            hmmc->State = HAL_MMC_STATE_READY;
            return HAL_TIMEOUT;
          }
        }
      }

      if (hmmc->ErrorCode == HAL_MMC_ERROR_CMD_RSP_TIMEOUT)
      {
        while (!__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DATAEND))
        {
          if ((HAL_GetTick() - tickstart) >=  SDMMC_DATATIMEOUT)
          {
            hmmc->ErrorCode = HAL_MMC_ERROR_TIMEOUT;
            hmmc->State = HAL_MMC_STATE_READY;
            return HAL_TIMEOUT;
          }
        }
      }
    }
    else if ((hmmc->Instance->DCTRL & SDMMC_DCTRL_DTDIR) == SDMMC_TRANSFER_DIR_TO_SDMMC)
    {
      while (!__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DABORT | SDMMC_FLAG_DATAEND))
      {
        if ((HAL_GetTick() - tickstart) >=  SDMMC_DATATIMEOUT)
        {
          hmmc->ErrorCode = HAL_MMC_ERROR_TIMEOUT;
          hmmc->State = HAL_MMC_STATE_READY;
          return HAL_TIMEOUT;
        }
      }
    }
    else
    {
      /* Nothing to do*/
    }

    /*The reason of all these while conditions previously is that we need to wait the SDMMC and clear
      the appropriate flags that will be set depending of the abort/non abort of the memory */
    /*Not waiting the SDMMC flags will cause the next SDMMC_DISABLE_IDMA to not get cleared and will result
      in next SDMMC read/write operation to fail */

    /*SDMMC ready for clear data flags*/
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_FLAG_BUSYD0END);
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_DATA_FLAGS);
    /* If IDMA Context, disable Internal DMA */
    hmmc->Instance->IDMACTRL = SDMMC_DISABLE_IDMA;

    hmmc->State = HAL_MMC_STATE_READY;

    /* Initialize the MMC operation */
    hmmc->Context = MMC_CONTEXT_NONE;
  }
  return HAL_OK;
}
/**
  * @brief  Abort the current transfer and disable the MMC (IT mode).
  * @param  hmmc: pointer to a MMC_HandleTypeDef structure that contains
  *                the configuration information for MMC module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_Abort_IT(MMC_HandleTypeDef *hmmc)
{
  HAL_MMC_CardStateTypeDef CardState;

  /* DIsable All interrupts */
  __HAL_MMC_DISABLE_IT(hmmc, SDMMC_IT_DATAEND | SDMMC_IT_DCRCFAIL | SDMMC_IT_DTIMEOUT | \
                       SDMMC_IT_TXUNDERR | SDMMC_IT_RXOVERR);

  /* If IDMA Context, disable Internal DMA */
  hmmc->Instance->IDMACTRL = SDMMC_DISABLE_IDMA;

  /* Clear All flags */
  __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_DATA_FLAGS);

  CardState = HAL_MMC_GetCardState(hmmc);
  hmmc->State = HAL_MMC_STATE_READY;

  if ((CardState == HAL_MMC_CARD_RECEIVING) || (CardState == HAL_MMC_CARD_SENDING))
  {
    hmmc->ErrorCode = SDMMC_CmdStopTransfer(hmmc->Instance);
  }
  if (hmmc->ErrorCode != HAL_MMC_ERROR_NONE)
  {
    return HAL_ERROR;
  }
  else
  {
#if defined (USE_HAL_MMC_REGISTER_CALLBACKS) && (USE_HAL_MMC_REGISTER_CALLBACKS == 1U)
    hmmc->AbortCpltCallback(hmmc);
#else
    HAL_MMC_AbortCallback(hmmc);
#endif /* USE_HAL_MMC_REGISTER_CALLBACKS */
  }

  return HAL_OK;
}

/**
  * @brief  Perform specific commands sequence for the different type of erase.
  * @note   This API should be followed by a check on the card state through
  *         HAL_MMC_GetCardState().
  * @param  hmmc Pointer to MMC handle
  * @param  EraseType Specifies the type of erase to be performed
  *          This parameter can be one of the following values:
  *             @arg HAL_MMC_TRIM Erase the write blocks identified by CMD35 & 36
  *             @arg HAL_MMC_ERASE Erase the erase groups identified by CMD35 & 36
  *             @arg HAL_MMC_DISCARD Discard the write blocks identified by CMD35 & 36
  *             @arg HAL_MMC_SECURE_ERASE Perform a secure purge according SRT on the erase groups identified
  *                  by CMD35 & 36
  *             @arg HAL_MMC_SECURE_TRIM_STEP1 Mark the write blocks identified by CMD35 & 36 for secure erase
  *             @arg HAL_MMC_SECURE_TRIM_STEP2 Perform a secure purge according SRT on the write blocks
  *                  previously identified
  * @param  BlockStartAdd Start Block address
  * @param  BlockEndAdd End Block address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_EraseSequence(MMC_HandleTypeDef *hmmc, uint32_t EraseType,
                                        uint32_t BlockStartAdd, uint32_t BlockEndAdd)
{
  uint32_t errorstate;
  uint32_t start_add = BlockStartAdd;
  uint32_t end_add = BlockEndAdd;
  uint32_t tickstart = HAL_GetTick();

  /* Check the erase type value is correct */
  assert_param(IS_MMC_ERASE_TYPE(EraseType));

  /* Check the coherence between start and end address */
  if (end_add < start_add)
  {
    hmmc->ErrorCode |= HAL_MMC_ERROR_PARAM;
    return HAL_ERROR;
  }

  /* Check that the end address is not out of range of device memory */
  if (end_add > (hmmc->MmcCard.LogBlockNbr))
  {
    hmmc->ErrorCode |= HAL_MMC_ERROR_ADDR_OUT_OF_RANGE;
    return HAL_ERROR;
  }

  /* Check the case of 4kB blocks (field DATA SECTOR SIZE of extended CSD register) */
  if (((hmmc->Ext_CSD[(MMC_EXT_CSD_DATA_SEC_SIZE_INDEX / 4)] >> MMC_EXT_CSD_DATA_SEC_SIZE_POS) & 0x000000FFU) != 0x0U)
  {
    if (((start_add % 8U) != 0U) || ((end_add % 8U) != 0U))
    {
      /* The address should be aligned to 8 (corresponding to 4 KBytes blocks) */
      hmmc->ErrorCode |= HAL_MMC_ERROR_ADDR_MISALIGNED;
      return HAL_ERROR;
    }
  }

  /* Check if the card command class supports erase command */
  if (((hmmc->MmcCard.Class) & SDMMC_CCCC_ERASE) == 0U)
  {
    hmmc->ErrorCode |= HAL_MMC_ERROR_REQUEST_NOT_APPLICABLE;
    return HAL_ERROR;
  }

  /* Check the state of the driver */
  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    /* Change State */
    hmmc->State = HAL_MMC_STATE_BUSY;

    /* Check that the card is not locked */
    if ((SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP1) & SDMMC_CARD_LOCKED) == SDMMC_CARD_LOCKED)
    {
      hmmc->ErrorCode |= HAL_MMC_ERROR_LOCK_UNLOCK_FAILED;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* In case of low capacity card, the address is not block number but bytes */
    if ((hmmc->MmcCard.CardType) != MMC_HIGH_CAPACITY_CARD)
    {
      start_add *= MMC_BLOCKSIZE;
      end_add   *= MMC_BLOCKSIZE;
    }

    /* Send CMD35 MMC_ERASE_GRP_START with start address as argument */
    errorstate = SDMMC_CmdEraseStartAdd(hmmc->Instance, start_add);
    if (errorstate == HAL_MMC_ERROR_NONE)
    {
      /* Send CMD36 MMC_ERASE_GRP_END with end address as argument */
      errorstate = SDMMC_CmdEraseEndAdd(hmmc->Instance, end_add);
      if (errorstate == HAL_MMC_ERROR_NONE)
      {
        /* Send CMD38 ERASE with erase type as argument */
        errorstate = SDMMC_CmdErase(hmmc->Instance, EraseType);
        if (errorstate == HAL_MMC_ERROR_NONE)
        {
          if ((EraseType == HAL_MMC_SECURE_ERASE) || (EraseType == HAL_MMC_SECURE_TRIM_STEP2))
          {
            /* Wait that the device is ready by checking the D0 line */
            while ((!__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_BUSYD0END)) && (errorstate == HAL_MMC_ERROR_NONE))
            {
              if ((HAL_GetTick() - tickstart) >= SDMMC_MAXERASETIMEOUT)
              {
                errorstate = HAL_MMC_ERROR_TIMEOUT;
              }
            }

            /* Clear the flag corresponding to end D0 bus line */
            __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_FLAG_BUSYD0END);
          }
        }
      }
    }

    /* Change State */
    hmmc->State = HAL_MMC_STATE_READY;

    /* Manage errors */
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;

      if (errorstate != HAL_MMC_ERROR_TIMEOUT)
      {
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }
    else
    {
      return HAL_OK;
    }
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Perform sanitize operation on the device.
  * @note   This API should be followed by a check on the card state through
  *         HAL_MMC_GetCardState().
  * @param  hmmc Pointer to MMC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_Sanitize(MMC_HandleTypeDef *hmmc)
{
  uint32_t errorstate;
  uint32_t response = 0U;
  uint32_t count;
  uint32_t tickstart = HAL_GetTick();

  /* Check the state of the driver */
  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    /* Change State */
    hmmc->State = HAL_MMC_STATE_BUSY;

    /* Index : 165 - Value : 0x01 */
    errorstate = SDMMC_CmdSwitch(hmmc->Instance, 0x03A50100U);
    if (errorstate == HAL_MMC_ERROR_NONE)
    {
      /* Wait that the device is ready by checking the D0 line */
      while ((!__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_BUSYD0END)) && (errorstate == HAL_MMC_ERROR_NONE))
      {
        if ((HAL_GetTick() - tickstart) >= SDMMC_MAXERASETIMEOUT)
        {
          errorstate = HAL_MMC_ERROR_TIMEOUT;
        }
      }

      /* Clear the flag corresponding to end D0 bus line */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_FLAG_BUSYD0END);

      if (errorstate == HAL_MMC_ERROR_NONE)
      {
        /* While card is not ready for data and trial number for sending CMD13 is not exceeded */
        count = SDMMC_MAX_TRIAL;
        do
        {
          errorstate = SDMMC_CmdSendStatus(hmmc->Instance, (uint32_t)(((uint32_t)hmmc->MmcCard.RelCardAdd) << 16U));
          if (errorstate != HAL_MMC_ERROR_NONE)
          {
            break;
          }

          /* Get command response */
          response = SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP1);
          count--;
        } while (((response & 0x100U) == 0U) && (count != 0U));

        /* Check the status after the switch command execution */
        if ((count != 0U) && (errorstate == HAL_MMC_ERROR_NONE))
        {
          /* Check the bit SWITCH_ERROR of the device status */
          if ((response & 0x80U) != 0U)
          {
            errorstate = SDMMC_ERROR_GENERAL_UNKNOWN_ERR;
          }
        }
        else if (count == 0U)
        {
          errorstate = SDMMC_ERROR_TIMEOUT;
        }
        else
        {
          /* Nothing to do */
        }
      }
    }

    /* Change State */
    hmmc->State = HAL_MMC_STATE_READY;

    /* Manage errors */
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;

      if (errorstate != HAL_MMC_ERROR_TIMEOUT)
      {
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }
    else
    {
      return HAL_OK;
    }
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Configure the Secure Removal Type (SRT) in the Extended CSD register.
  * @note   This API should be followed by a check on the card state through
  *         HAL_MMC_GetCardState().
  * @param  hmmc Pointer to MMC handle
  * @param  SRTMode Specifies the type of erase to be performed
  *          This parameter can be one of the following values:
  *            @arg HAL_MMC_SRT_ERASE Information removed by an erase
  *            @arg HAL_MMC_SRT_WRITE_CHAR_ERASE Information removed by an overwriting with a character
  *                 followed by an erase
  *            @arg HAL_MMC_SRT_WRITE_CHAR_COMPL_RANDOM Information removed by an overwriting with a character,
  *                 its complement then a random character
  *            @arg HAL_MMC_SRT_VENDOR_DEFINED Information removed using a vendor defined
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_ConfigSecRemovalType(MMC_HandleTypeDef *hmmc, uint32_t SRTMode)
{
  uint32_t srt;
  uint32_t errorstate;
  uint32_t response = 0U;
  uint32_t count;

  /* Check the erase type value is correct */
  assert_param(IS_MMC_SRT_TYPE(SRTMode));

  /* Check the state of the driver */
  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    /* Get the supported values by the device */
    if (HAL_MMC_GetSupportedSecRemovalType(hmmc, &srt) == HAL_OK)
    {
      /* Change State */
      hmmc->State = HAL_MMC_STATE_BUSY;

      /* Check the value passed as parameter is supported by the device */
      if ((SRTMode & srt) != 0U)
      {
        /* Index : 16 - Value : SRTMode */
        srt |= ((POSITION_VAL(SRTMode)) << 4U);
        errorstate = SDMMC_CmdSwitch(hmmc->Instance, (0x03100000U | (srt << 8U)));
        if (errorstate == HAL_MMC_ERROR_NONE)
        {
          /* While card is not ready for data and trial number for sending CMD13 is not exceeded */
          count = SDMMC_MAX_TRIAL;
          do
          {
            errorstate = SDMMC_CmdSendStatus(hmmc->Instance, (uint32_t)(((uint32_t)hmmc->MmcCard.RelCardAdd) << 16U));
            if (errorstate != HAL_MMC_ERROR_NONE)
            {
              break;
            }

            /* Get command response */
            response = SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP1);
            count--;
          } while (((response & 0x100U) == 0U) && (count != 0U));

          /* Check the status after the switch command execution */
          if ((count != 0U) && (errorstate == HAL_MMC_ERROR_NONE))
          {
            /* Check the bit SWITCH_ERROR of the device status */
            if ((response & 0x80U) != 0U)
            {
              errorstate = SDMMC_ERROR_GENERAL_UNKNOWN_ERR;
            }
          }
          else if (count == 0U)
          {
            errorstate = SDMMC_ERROR_TIMEOUT;
          }
          else
          {
            /* Nothing to do */
          }
        }
      }
      else
      {
        errorstate = SDMMC_ERROR_UNSUPPORTED_FEATURE;
      }

      /* Change State */
      hmmc->State = HAL_MMC_STATE_READY;
    }
    else
    {
      errorstate = SDMMC_ERROR_GENERAL_UNKNOWN_ERR;
    }

    /* Manage errors */
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      return HAL_ERROR;
    }
    else
    {
      return HAL_OK;
    }
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Gets the supported values of the the Secure Removal Type (SRT).
  * @param  hmmc pointer to MMC handle
  * @param  SupportedSRT pointer for supported SRT value
  *          This parameter is a bit field of the following values:
  *            @arg HAL_MMC_SRT_ERASE Information removed by an erase
  *            @arg HAL_MMC_SRT_WRITE_CHAR_ERASE Information removed by an overwriting with a character followed
  *                  by an erase
  *            @arg HAL_MMC_SRT_WRITE_CHAR_COMPL_RANDOM Information removed by an overwriting with a character,
  *                 its complement then a random character
  *            @arg HAL_MMC_SRT_VENDOR_DEFINED Information removed using a vendor defined
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_GetSupportedSecRemovalType(MMC_HandleTypeDef *hmmc, uint32_t *SupportedSRT)
{
  /* Check the state of the driver */
  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    /* Change State */
    hmmc->State = HAL_MMC_STATE_BUSY;

    /* Read field SECURE_REMOVAL_TYPE [16 = 4*4] of the Extended CSD register */
    *SupportedSRT = (hmmc->Ext_CSD[4] & 0x0000000FU); /* Bits [3:0] of field 16 */

    /* Change State */
    hmmc->State = HAL_MMC_STATE_READY;

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Switch the device from Standby State to Sleep State.
  * @param  hmmc pointer to MMC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_SleepDevice(MMC_HandleTypeDef *hmmc)
{
  uint32_t errorstate,
           sleep_timeout,
           timeout,
           count,
           response = 0U  ;
  uint32_t tickstart = HAL_GetTick();

  /* Check the state of the driver */
  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    /* Change State */
    hmmc->State = HAL_MMC_STATE_BUSY;

    /* Set the power-off notification to powered-on : Ext_CSD[34] = 1 */
    errorstate = SDMMC_CmdSwitch(hmmc->Instance, (0x03220100U));
    if (errorstate == HAL_MMC_ERROR_NONE)
    {
      /* While card is not ready for data and trial number for sending CMD13 is not exceeded */
      count = SDMMC_MAX_TRIAL;
      do
      {
        errorstate = SDMMC_CmdSendStatus(hmmc->Instance, (uint32_t)(((uint32_t)hmmc->MmcCard.RelCardAdd) << 16U));
        if (errorstate != HAL_MMC_ERROR_NONE)
        {
          break;
        }

        /* Get command response */
        response = SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP1);
        count--;
      } while (((response & 0x100U) == 0U) && (count != 0U));

      /* Check the status after the switch command execution */
      if (count == 0U)
      {
        errorstate = SDMMC_ERROR_TIMEOUT;
      }
      else if (errorstate == HAL_MMC_ERROR_NONE)
      {
        /* Check the bit SWITCH_ERROR of the device status */
        if ((response & 0x80U) != 0U)
        {
          errorstate = SDMMC_ERROR_UNSUPPORTED_FEATURE;
        }
        else
        {
          /* Set the power-off notification to sleep notification : Ext_CSD[34] = 4 */
          errorstate = SDMMC_CmdSwitch(hmmc->Instance, (0x03220400U));
          if (errorstate == HAL_MMC_ERROR_NONE)
          {
            /* Field SLEEP_NOTIFICATION_TIME [216] */
            sleep_timeout = ((hmmc->Ext_CSD[(MMC_EXT_CSD_SLEEP_NOTIFICATION_TIME_INDEX / 4)] >>
                              MMC_EXT_CSD_SLEEP_NOTIFICATION_TIME_POS) & 0x000000FFU);

            /* Sleep/Awake Timeout = 10us * 2^SLEEP_NOTIFICATION_TIME */
            /* In HAL, the tick interrupt occurs each ms */
            if ((sleep_timeout == 0U) || (sleep_timeout > 0x17U))
            {
              sleep_timeout = 0x17U; /* Max register value defined is 0x17 */
            }
            timeout = (((1UL << sleep_timeout) / 100U) + 1U);

            /* Wait that the device is ready by checking the D0 line */
            while ((!__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_BUSYD0END)) && (errorstate == HAL_MMC_ERROR_NONE))
            {
              if ((HAL_GetTick() - tickstart) >= timeout)
              {
                errorstate = SDMMC_ERROR_TIMEOUT;
              }
            }

            /* Clear the flag corresponding to end D0 bus line */
            __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_FLAG_BUSYD0END);

            if (errorstate == HAL_MMC_ERROR_NONE)
            {
              /* While card is not ready for data and trial number for sending CMD13 is not exceeded */
              count = SDMMC_MAX_TRIAL;
              do
              {
                errorstate = SDMMC_CmdSendStatus(hmmc->Instance,
                                                 (uint32_t)(((uint32_t)hmmc->MmcCard.RelCardAdd) << 16U));
                if (errorstate != HAL_MMC_ERROR_NONE)
                {
                  break;
                }

                /* Get command response */
                response = SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP1);
                count--;
              } while (((response & 0x100U) == 0U) && (count != 0U));

              /* Check the status after the switch command execution */
              if (count == 0U)
              {
                errorstate = SDMMC_ERROR_TIMEOUT;
              }
              else if (errorstate == HAL_MMC_ERROR_NONE)
              {
                /* Check the bit SWITCH_ERROR of the device status */
                if ((response & 0x80U) != 0U)
                {
                  errorstate = SDMMC_ERROR_UNSUPPORTED_FEATURE;
                }
                else
                {
                  /* Switch the device in stand-by mode */
                  (void)SDMMC_CmdSelDesel(hmmc->Instance, 0U);

                  /* Field S_A_TIEMOUT [217] */
                  sleep_timeout = ((hmmc->Ext_CSD[(MMC_EXT_CSD_S_A_TIMEOUT_INDEX / 4)] >>
                                    MMC_EXT_CSD_S_A_TIMEOUT_POS) & 0x000000FFU);

                  /* Sleep/Awake Timeout = 100ns * 2^S_A_TIMEOUT */
                  /* In HAL, the tick interrupt occurs each ms */
                  if ((sleep_timeout == 0U) || (sleep_timeout > 0x17U))
                  {
                    sleep_timeout = 0x17U; /* Max register value defined is 0x17 */
                  }
                  timeout = (((1UL << sleep_timeout) / 10000U) + 1U);

                  if (HAL_MMC_GetCardState(hmmc) == HAL_MMC_CARD_STANDBY)
                  {
                    /* Send CMD5 CMD_MMC_SLEEP_AWAKE with RCA and SLEEP as argument */
                    errorstate = SDMMC_CmdSleepMmc(hmmc->Instance,
                                                   ((hmmc->MmcCard.RelCardAdd << 16U) | (0x1U << 15U)));
                    if (errorstate == HAL_MMC_ERROR_NONE)
                    {
                      /* Wait that the device is ready by checking the D0 line */
                      while ((!__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_BUSYD0END)) && (errorstate == HAL_MMC_ERROR_NONE))
                      {
                        if ((HAL_GetTick() - tickstart) >= timeout)
                        {
                          errorstate = SDMMC_ERROR_TIMEOUT;
                        }
                      }

                      /* Clear the flag corresponding to end D0 bus line */
                      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_FLAG_BUSYD0END);
                    }
                  }
                  else
                  {
                    errorstate = SDMMC_ERROR_REQUEST_NOT_APPLICABLE;
                  }
                }
              }
              else
              {
                /* Nothing to do */
              }
            }
          }
        }
      }
      else
      {
        /* Nothing to do */
      }
    }

    /* Change State */
    hmmc->State = HAL_MMC_STATE_READY;

    /* Manage errors */
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;

      if (errorstate != HAL_MMC_ERROR_TIMEOUT)
      {
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }
    else
    {
      return HAL_OK;
    }
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Switch the device from Sleep State to Standby State.
  * @param  hmmc pointer to MMC handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_AwakeDevice(MMC_HandleTypeDef *hmmc)
{
  uint32_t errorstate;
  uint32_t sleep_timeout;
  uint32_t timeout;
  uint32_t count;
  uint32_t response = 0U;
  uint32_t tickstart = HAL_GetTick();

  /* Check the state of the driver */
  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    /* Change State */
    hmmc->State = HAL_MMC_STATE_BUSY;

    /* Field S_A_TIEMOUT [217] */
    sleep_timeout = ((hmmc->Ext_CSD[(MMC_EXT_CSD_S_A_TIMEOUT_INDEX / 4)] >> MMC_EXT_CSD_S_A_TIMEOUT_POS) &
                     0x000000FFU);

    /* Sleep/Awake Timeout = 100ns * 2^S_A_TIMEOUT */
    /* In HAL, the tick interrupt occurs each ms */
    if ((sleep_timeout == 0U) || (sleep_timeout > 0x17U))
    {
      sleep_timeout = 0x17U; /* Max register value defined is 0x17 */
    }
    timeout = (((1UL << sleep_timeout) / 10000U) + 1U);

    /* Send CMD5 CMD_MMC_SLEEP_AWAKE with RCA and AWAKE as argument */
    errorstate = SDMMC_CmdSleepMmc(hmmc->Instance, (hmmc->MmcCard.RelCardAdd << 16U));
    if (errorstate == HAL_MMC_ERROR_NONE)
    {
      /* Wait that the device is ready by checking the D0 line */
      while ((!__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_BUSYD0END)) && (errorstate == HAL_MMC_ERROR_NONE))
      {
        if ((HAL_GetTick() - tickstart) >= timeout)
        {
          errorstate = SDMMC_ERROR_TIMEOUT;
        }
      }

      /* Clear the flag corresponding to end D0 bus line */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_FLAG_BUSYD0END);

      if (errorstate == HAL_MMC_ERROR_NONE)
      {
        if (HAL_MMC_GetCardState(hmmc) == HAL_MMC_CARD_STANDBY)
        {
          /* Switch the device in transfer mode */
          errorstate = SDMMC_CmdSelDesel(hmmc->Instance, (hmmc->MmcCard.RelCardAdd << 16U));
          if (errorstate == HAL_MMC_ERROR_NONE)
          {
            if (HAL_MMC_GetCardState(hmmc) == HAL_MMC_CARD_TRANSFER)
            {
              /* Set the power-off notification to powered-on : Ext_CSD[34] = 1 */
              errorstate = SDMMC_CmdSwitch(hmmc->Instance, (0x03220100U));
              if (errorstate == HAL_MMC_ERROR_NONE)
              {
                /* While card is not ready for data and trial number for sending CMD13 is not exceeded */
                count = SDMMC_MAX_TRIAL;
                do
                {
                  errorstate = SDMMC_CmdSendStatus(hmmc->Instance,
                                                   (uint32_t)(((uint32_t)hmmc->MmcCard.RelCardAdd) << 16U));
                  if (errorstate != HAL_MMC_ERROR_NONE)
                  {
                    break;
                  }

                  /* Get command response */
                  response = SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP1);
                  count--;
                } while (((response & 0x100U) == 0U) && (count != 0U));

                /* Check the status after the switch command execution */
                if (count == 0U)
                {
                  errorstate = SDMMC_ERROR_TIMEOUT;
                }
                else if (errorstate == HAL_MMC_ERROR_NONE)
                {
                  /* Check the bit SWITCH_ERROR of the device status */
                  if ((response & 0x80U) != 0U)
                  {
                    errorstate = SDMMC_ERROR_UNSUPPORTED_FEATURE;
                  }
                }
                else
                {
                  /* NOthing to do */
                }
              }
            }
            else
            {
              errorstate = SDMMC_ERROR_REQUEST_NOT_APPLICABLE;
            }
          }
        }
        else
        {
          errorstate = SDMMC_ERROR_REQUEST_NOT_APPLICABLE;
        }
      }
    }

    /* Change State */
    hmmc->State = HAL_MMC_STATE_READY;

    /* Manage errors */
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;

      if (errorstate != HAL_MMC_ERROR_TIMEOUT)
      {
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }
    else
    {
      return HAL_OK;
    }
  }
  else
  {
    return HAL_BUSY;
  }
}
/**
  * @}
  */

/**
  * @}
  */

/* Private function ----------------------------------------------------------*/
/** @addtogroup MMC_Private_Functions
  * @{
  */

/**
  * @brief  Initializes the mmc card.
  * @param  hmmc: Pointer to MMC handle
  * @retval MMC Card error state
  */
static uint32_t MMC_InitCard(MMC_HandleTypeDef *hmmc)
{
  HAL_MMC_CardCSDTypeDef CSD;
  uint32_t errorstate;
  uint16_t mmc_rca = 2U;
  MMC_InitTypeDef Init;

  /* Check the power State */
  if (SDMMC_GetPowerState(hmmc->Instance) == 0U)
  {
    /* Power off */
    return HAL_MMC_ERROR_REQUEST_NOT_APPLICABLE;
  }

  /* Send CMD2 ALL_SEND_CID */
  errorstate = SDMMC_CmdSendCID(hmmc->Instance);
  if (errorstate != HAL_MMC_ERROR_NONE)
  {
    return errorstate;
  }
  else
  {
    /* Get Card identification number data */
    hmmc->CID[0U] = SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP1);
    hmmc->CID[1U] = SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP2);
    hmmc->CID[2U] = SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP3);
    hmmc->CID[3U] = SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP4);
  }

  /* Send CMD3 SET_REL_ADDR with RCA = 2 (should be greater than 1) */
  /* MMC Card publishes its RCA. */
  errorstate = SDMMC_CmdSetRelAddMmc(hmmc->Instance, mmc_rca);
  if (errorstate != HAL_MMC_ERROR_NONE)
  {
    return errorstate;
  }

  /* Get the MMC card RCA */
  hmmc->MmcCard.RelCardAdd = mmc_rca;

  /* Send CMD9 SEND_CSD with argument as card's RCA */
  errorstate = SDMMC_CmdSendCSD(hmmc->Instance, (uint32_t)(hmmc->MmcCard.RelCardAdd << 16U));
  if (errorstate != HAL_MMC_ERROR_NONE)
  {
    return errorstate;
  }
  else
  {
    /* Get Card Specific Data */
    hmmc->CSD[0U] = SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP1);
    hmmc->CSD[1U] = SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP2);
    hmmc->CSD[2U] = SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP3);
    hmmc->CSD[3U] = SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP4);
  }

  /* Get the Card Class */
  hmmc->MmcCard.Class = (SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP2) >> 20U);

  /* Select the Card */
  errorstate = SDMMC_CmdSelDesel(hmmc->Instance, (uint32_t)(((uint32_t)hmmc->MmcCard.RelCardAdd) << 16U));
  if (errorstate != HAL_MMC_ERROR_NONE)
  {
    return errorstate;
  }

  /* Get CSD parameters */
  if (HAL_MMC_GetCardCSD(hmmc, &CSD) != HAL_OK)
  {
    return hmmc->ErrorCode;
  }

  /* While card is not ready for data and trial number for sending CMD13 is not exceeded */
  errorstate = SDMMC_CmdSendStatus(hmmc->Instance, (uint32_t)(((uint32_t)hmmc->MmcCard.RelCardAdd) << 16U));
  if (errorstate != HAL_MMC_ERROR_NONE)
  {
    hmmc->ErrorCode |= errorstate;
  }

  /* Get Extended CSD parameters */
  if (HAL_MMC_GetCardExtCSD(hmmc, hmmc->Ext_CSD, SDMMC_DATATIMEOUT) != HAL_OK)
  {
    return hmmc->ErrorCode;
  }

  /* While card is not ready for data and trial number for sending CMD13 is not exceeded */
  errorstate = SDMMC_CmdSendStatus(hmmc->Instance, (uint32_t)(((uint32_t)hmmc->MmcCard.RelCardAdd) << 16U));
  if (errorstate != HAL_MMC_ERROR_NONE)
  {
    hmmc->ErrorCode |= errorstate;
  }

  /* Configure the SDMMC peripheral */
  Init = hmmc->Init;
  Init.BusWide = SDMMC_BUS_WIDE_1B;
  (void)SDMMC_Init(hmmc->Instance, Init);

  /* All cards are initialized */
  return HAL_MMC_ERROR_NONE;
}

/**
  * @brief  Enquires cards about their operating voltage and configures clock
  *         controls and stores MMC information that will be needed in future
  *         in the MMC handle.
  * @param  hmmc: Pointer to MMC handle
  * @retval error state
  */
static uint32_t MMC_PowerON(MMC_HandleTypeDef *hmmc)
{
  __IO uint32_t count = 0U;
  uint32_t response = 0U;
  uint32_t validvoltage = 0U;
  uint32_t errorstate;

  /* CMD0: GO_IDLE_STATE */
  errorstate = SDMMC_CmdGoIdleState(hmmc->Instance);
  if (errorstate != HAL_MMC_ERROR_NONE)
  {
    return errorstate;
  }

  while (validvoltage == 0U)
  {
    if (count++ == SDMMC_MAX_VOLT_TRIAL)
    {
      return HAL_MMC_ERROR_INVALID_VOLTRANGE;
    }

    /* SEND CMD1 APP_CMD with voltage range as argument */
    errorstate = SDMMC_CmdOpCondition(hmmc->Instance, MMC_VOLTAGE_RANGE);
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      return HAL_MMC_ERROR_UNSUPPORTED_FEATURE;
    }

    /* Get command response */
    response = SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP1);

    /* Get operating voltage*/
    validvoltage = (((response >> 31U) == 1U) ? 1U : 0U);
  }

  /* When power routine is finished and command returns valid voltage */
  if (((response & (0xFF000000U)) >> 24) == 0xC0U)
  {
    hmmc->MmcCard.CardType = MMC_HIGH_CAPACITY_CARD;
  }
  else
  {
    hmmc->MmcCard.CardType = MMC_LOW_CAPACITY_CARD;
  }

  return HAL_MMC_ERROR_NONE;
}

/**
  * @brief  Turns the SDMMC output signals off.
  * @param  hmmc: Pointer to MMC handle
  * @retval None
  */
static void MMC_PowerOFF(MMC_HandleTypeDef *hmmc)
{
  /* Set Power State to OFF */
  (void)SDMMC_PowerState_OFF(hmmc->Instance);
}

/**
  * @brief  Returns the current card's status.
  * @param  hmmc: Pointer to MMC handle
  * @param  pCardStatus: pointer to the buffer that will contain the MMC card
  *         status (Card Status register)
  * @retval error state
  */
static uint32_t MMC_SendStatus(MMC_HandleTypeDef *hmmc, uint32_t *pCardStatus)
{
  uint32_t errorstate;

  if (pCardStatus == NULL)
  {
    return HAL_MMC_ERROR_PARAM;
  }

  /* Send Status command */
  errorstate = SDMMC_CmdSendStatus(hmmc->Instance, (uint32_t)(hmmc->MmcCard.RelCardAdd << 16U));
  if (errorstate != HAL_MMC_ERROR_NONE)
  {
    return errorstate;
  }

  /* Get MMC card status */
  *pCardStatus = SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP1);

  return HAL_MMC_ERROR_NONE;
}

/**
  * @brief  Reads extended CSD register to get the sectors number of the device
  * @param  hmmc: Pointer to MMC handle
  * @param  pFieldData: Pointer to the read buffer
  * @param  FieldIndex: Index of the field to be read
  * @param  Timeout: Specify timeout value
  * @retval HAL status
  */
static HAL_StatusTypeDef MMC_ReadExtCSD(MMC_HandleTypeDef *hmmc, uint32_t *pFieldData,
                                        uint16_t FieldIndex, uint32_t Timeout)
{
  SDMMC_DataInitTypeDef config;
  uint32_t errorstate;
  uint32_t tickstart = HAL_GetTick();
  uint32_t count;
  uint32_t i = 0;
  uint32_t tmp_data;
  uint32_t dataremaining;

  hmmc->ErrorCode = HAL_MMC_ERROR_NONE;

  /* Initialize data control register */
  hmmc->Instance->DCTRL = 0;

  /* Configure the MMC DPSM (Data Path State Machine) */
  config.DataTimeOut   = SDMMC_DATATIMEOUT;
  config.DataLength    = MMC_BLOCKSIZE;
  config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
  config.TransferDir   = SDMMC_TRANSFER_DIR_TO_SDMMC;
  config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
  config.DPSM          = SDMMC_DPSM_ENABLE;
  (void)SDMMC_ConfigData(hmmc->Instance, &config);

  /* Set Block Size for Card */
  errorstate = SDMMC_CmdSendEXTCSD(hmmc->Instance, 0);
  if (errorstate != HAL_MMC_ERROR_NONE)
  {
    /* Clear all the static flags */
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
    hmmc->ErrorCode |= errorstate;
    hmmc->State = HAL_MMC_STATE_READY;
    return HAL_ERROR;
  }

  /* Poll on SDMMC flags */
  dataremaining = config.DataLength;
  while (!__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_RXOVERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT |
                             SDMMC_FLAG_DATAEND))
  {
    if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_RXFIFOHF) && (dataremaining >= SDMMC_FIFO_SIZE))
    {
      /* Read data from SDMMC Rx FIFO */
      for (count = 0U; count < (SDMMC_FIFO_SIZE / 4U); count++)
      {
        tmp_data = SDMMC_ReadFIFO(hmmc->Instance);
        /* eg : SEC_COUNT   : FieldIndex = 212 => i+count = 53 */
        /*      DEVICE_TYPE : FieldIndex = 196 => i+count = 49 */
        if ((i + count) == ((uint32_t)FieldIndex / 4U))
        {
          *pFieldData = tmp_data;
        }
      }
      i += 8U;
      dataremaining -= SDMMC_FIFO_SIZE;
    }

    if (((HAL_GetTick() - tickstart) >=  Timeout) || (Timeout == 0U))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_TIMEOUT;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_TIMEOUT;
    }
  }

  /* Get error state */
  if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DTIMEOUT))
  {
    /* Clear all the static flags */
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
    hmmc->ErrorCode |= HAL_MMC_ERROR_DATA_TIMEOUT;
    hmmc->State = HAL_MMC_STATE_READY;
    return HAL_ERROR;
  }
  else if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DCRCFAIL))
  {
    /* Clear all the static flags */
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
    hmmc->ErrorCode |= HAL_MMC_ERROR_DATA_CRC_FAIL;
    hmmc->State = HAL_MMC_STATE_READY;
    return HAL_ERROR;
  }
  else if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_RXOVERR))
  {
    /* Clear all the static flags */
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
    hmmc->ErrorCode |= HAL_MMC_ERROR_RX_OVERRUN;
    hmmc->State = HAL_MMC_STATE_READY;
    return HAL_ERROR;
  }
  else
  {
    /* Nothing to do */
  }

  /* While card is not ready for data and trial number for sending CMD13 is not exceeded */
  errorstate = SDMMC_CmdSendStatus(hmmc->Instance, (uint32_t)(((uint32_t)hmmc->MmcCard.RelCardAdd) << 16));
  if (errorstate != HAL_MMC_ERROR_NONE)
  {
    hmmc->ErrorCode |= errorstate;
  }

  /* Clear all the static flags */
  __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_DATA_FLAGS);

  hmmc->State = HAL_MMC_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Wrap up reading in non-blocking mode.
  * @param  hmmc: pointer to a MMC_HandleTypeDef structure that contains
  *              the configuration information.
  * @retval None
  */
static void MMC_Read_IT(MMC_HandleTypeDef *hmmc)
{
  uint32_t count;
  uint32_t data;
  uint8_t *tmp;

  tmp = hmmc->pRxBuffPtr;

  if (hmmc->RxXferSize >= SDMMC_FIFO_SIZE)
  {
    /* Read data from SDMMC Rx FIFO */
    for (count = 0U; count < (SDMMC_FIFO_SIZE / 4U); count++)
    {
      data = SDMMC_ReadFIFO(hmmc->Instance);
      *tmp = (uint8_t)(data & 0xFFU);
      tmp++;
      *tmp = (uint8_t)((data >> 8U) & 0xFFU);
      tmp++;
      *tmp = (uint8_t)((data >> 16U) & 0xFFU);
      tmp++;
      *tmp = (uint8_t)((data >> 24U) & 0xFFU);
      tmp++;
    }

    hmmc->pRxBuffPtr = tmp;
    hmmc->RxXferSize -= SDMMC_FIFO_SIZE;
  }
}

/**
  * @brief  Wrap up writing in non-blocking mode.
  * @param  hmmc: pointer to a MMC_HandleTypeDef structure that contains
  *              the configuration information.
  * @retval None
  */
static void MMC_Write_IT(MMC_HandleTypeDef *hmmc)
{
  uint32_t count;
  uint32_t data;
  const uint8_t *tmp;

  tmp = hmmc->pTxBuffPtr;

  if (hmmc->TxXferSize >= SDMMC_FIFO_SIZE)
  {
    /* Write data to SDMMC Tx FIFO */
    for (count = 0U; count < (SDMMC_FIFO_SIZE / 4U); count++)
    {
      data = (uint32_t)(*tmp);
      tmp++;
      data |= ((uint32_t)(*tmp) << 8U);
      tmp++;
      data |= ((uint32_t)(*tmp) << 16U);
      tmp++;
      data |= ((uint32_t)(*tmp) << 24U);
      tmp++;
      (void)SDMMC_WriteFIFO(hmmc->Instance, &data);
    }

    hmmc->pTxBuffPtr = tmp;
    hmmc->TxXferSize -= SDMMC_FIFO_SIZE;
  }
}

/**
  * @brief  Switches the MMC card to high speed mode.
  * @param  hmmc: MMC handle
  * @param  state: State of high speed mode
  * @retval MMC Card error state
  */
static uint32_t MMC_HighSpeed(MMC_HandleTypeDef *hmmc, FunctionalState state)
{
  uint32_t errorstate = HAL_MMC_ERROR_NONE;
  uint32_t response = 0U;
  uint32_t count;
  uint32_t sdmmc_clk;
  SDMMC_InitTypeDef Init;

  if (((hmmc->Instance->CLKCR & SDMMC_CLKCR_BUSSPEED) != 0U) && (state == DISABLE))
  {
    errorstate = MMC_PwrClassUpdate(hmmc, (hmmc->Instance->CLKCR & SDMMC_CLKCR_WIDBUS), SDMMC_SPEED_MODE_DEFAULT);
    if (errorstate == HAL_MMC_ERROR_NONE)
    {
      /* Index : 185 - Value : 0 */
      errorstate = SDMMC_CmdSwitch(hmmc->Instance, 0x03B90000U);
    }
  }

  if (((hmmc->Instance->CLKCR & SDMMC_CLKCR_BUSSPEED) == 0U) && (state != DISABLE))
  {
    errorstate = MMC_PwrClassUpdate(hmmc, (hmmc->Instance->CLKCR & SDMMC_CLKCR_WIDBUS), SDMMC_SPEED_MODE_HIGH);
    if (errorstate == HAL_MMC_ERROR_NONE)
    {
      /* Index : 185 - Value : 1 */
      errorstate = SDMMC_CmdSwitch(hmmc->Instance, 0x03B90100U);
    }
  }

  if (errorstate == HAL_MMC_ERROR_NONE)
  {
    /* While card is not ready for data and trial number for sending CMD13 is not exceeded */
    count = SDMMC_MAX_TRIAL;
    do
    {
      errorstate = SDMMC_CmdSendStatus(hmmc->Instance, (uint32_t)(((uint32_t)hmmc->MmcCard.RelCardAdd) << 16U));
      if (errorstate != HAL_MMC_ERROR_NONE)
      {
        break;
      }

      /* Get command response */
      response = SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP1);
      count--;
    } while (((response & 0x100U) == 0U) && (count != 0U));

    /* Check the status after the switch command execution */
    if ((count != 0U) && (errorstate == HAL_MMC_ERROR_NONE))
    {
      /* Check the bit SWITCH_ERROR of the device status */
      if ((response & 0x80U) != 0U)
      {
        errorstate = SDMMC_ERROR_UNSUPPORTED_FEATURE;
      }
      else
      {
        /* Configure high speed */
        Init.ClockEdge           = hmmc->Init.ClockEdge;
        Init.ClockPowerSave      = hmmc->Init.ClockPowerSave;
        Init.BusWide             = (hmmc->Instance->CLKCR & SDMMC_CLKCR_WIDBUS);
        Init.HardwareFlowControl = hmmc->Init.HardwareFlowControl;

        if (state == DISABLE)
        {
          Init.ClockDiv = hmmc->Init.ClockDiv;
          (void)SDMMC_Init(hmmc->Instance, Init);

          CLEAR_BIT(hmmc->Instance->CLKCR, SDMMC_CLKCR_BUSSPEED);
        }
        else
        {
          /* High Speed Clock should be less or equal to 52MHz*/
          if (hmmc->Instance == SDMMC1)
          {
            sdmmc_clk = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SDMMC1);
          }
          else if (hmmc->Instance == SDMMC2)
          {
            sdmmc_clk = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SDMMC2);
          }
          else
          {
            sdmmc_clk = 0;
          }

          if (sdmmc_clk == 0U)
          {
            errorstate = SDMMC_ERROR_INVALID_PARAMETER;
          }
          else
          {
            if (sdmmc_clk <= MMC_HIGH_SPEED_FREQ)
            {
              Init.ClockDiv = 0;
            }
            else
            {
              Init.ClockDiv = (sdmmc_clk / (2U * MMC_HIGH_SPEED_FREQ)) + 1U;
            }
            (void)SDMMC_Init(hmmc->Instance, Init);

            SET_BIT(hmmc->Instance->CLKCR, SDMMC_CLKCR_BUSSPEED);
          }
        }
      }
    }
    else if (count == 0U)
    {
      errorstate = SDMMC_ERROR_TIMEOUT;
    }
    else
    {
      /* Nothing to do */
    }
  }

  return errorstate;
}

/**
  * @brief  Switches the MMC card to Double Data Rate (DDR) mode.
  * @param  hmmc: MMC handle
  * @param  state: State of DDR mode
  * @retval MMC Card error state
  */
static uint32_t MMC_DDR_Mode(MMC_HandleTypeDef *hmmc, FunctionalState state)
{
  uint32_t errorstate = HAL_MMC_ERROR_NONE;
  uint32_t response = 0U;
  uint32_t count;

  if (((hmmc->Instance->CLKCR & SDMMC_CLKCR_DDR) != 0U) && (state == DISABLE))
  {
    if ((hmmc->Instance->CLKCR & SDMMC_CLKCR_WIDBUS_0) != 0U)
    {
      errorstate = MMC_PwrClassUpdate(hmmc, SDMMC_BUS_WIDE_4B, SDMMC_SPEED_MODE_HIGH);
      if (errorstate == HAL_MMC_ERROR_NONE)
      {
        /* Index : 183 - Value : 1 */
        errorstate = SDMMC_CmdSwitch(hmmc->Instance, 0x03B70100U);
      }
    }
    else
    {
      errorstate = MMC_PwrClassUpdate(hmmc, SDMMC_BUS_WIDE_8B, SDMMC_SPEED_MODE_HIGH);
      if (errorstate == HAL_MMC_ERROR_NONE)
      {
        /* Index : 183 - Value : 2 */
        errorstate = SDMMC_CmdSwitch(hmmc->Instance, 0x03B70200U);
      }
    }
  }

  if (((hmmc->Instance->CLKCR & SDMMC_CLKCR_DDR) == 0U) && (state != DISABLE))
  {
    if ((hmmc->Instance->CLKCR & SDMMC_CLKCR_WIDBUS_0) != 0U)
    {
      errorstate = MMC_PwrClassUpdate(hmmc, SDMMC_BUS_WIDE_4B, SDMMC_SPEED_MODE_DDR);
      if (errorstate == HAL_MMC_ERROR_NONE)
      {
        /* Index : 183 - Value : 5 */
        errorstate = SDMMC_CmdSwitch(hmmc->Instance, 0x03B70500U);
      }
    }
    else
    {
      errorstate = MMC_PwrClassUpdate(hmmc, SDMMC_BUS_WIDE_8B, SDMMC_SPEED_MODE_DDR);
      if (errorstate == HAL_MMC_ERROR_NONE)
      {
        /* Index : 183 - Value : 6 */
        errorstate = SDMMC_CmdSwitch(hmmc->Instance, 0x03B70600U);
      }
    }
  }

  if (errorstate == HAL_MMC_ERROR_NONE)
  {
    /* While card is not ready for data and trial number for sending CMD13 is not exceeded */
    count = SDMMC_MAX_TRIAL;
    do
    {
      errorstate = SDMMC_CmdSendStatus(hmmc->Instance, (uint32_t)(((uint32_t)hmmc->MmcCard.RelCardAdd) << 16U));
      if (errorstate != HAL_MMC_ERROR_NONE)
      {
        break;
      }

      /* Get command response */
      response = SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP1);
      count--;
    } while (((response & 0x100U) == 0U) && (count != 0U));

    /* Check the status after the switch command execution */
    if ((count != 0U) && (errorstate == HAL_MMC_ERROR_NONE))
    {
      /* Check the bit SWITCH_ERROR of the device status */
      if ((response & 0x80U) != 0U)
      {
        errorstate = SDMMC_ERROR_UNSUPPORTED_FEATURE;
      }
      else
      {
        /* Configure DDR mode */
        if (state == DISABLE)
        {
          CLEAR_BIT(hmmc->Instance->CLKCR, SDMMC_CLKCR_DDR);
        }
        else
        {
          SET_BIT(hmmc->Instance->CLKCR, SDMMC_CLKCR_DDR);
        }
      }
    }
    else if (count == 0U)
    {
      errorstate = SDMMC_ERROR_TIMEOUT;
    }
    else
    {
      /* Nothing to do */
    }
  }

  return errorstate;
}

/**
  * @brief  Update the power class of the device.
  * @param  hmmc MMC handle
  * @param  Wide Wide of MMC bus
  * @param  Speed Speed of the MMC bus
  * @retval MMC Card error state
  */
static uint32_t MMC_PwrClassUpdate(MMC_HandleTypeDef *hmmc, uint32_t Wide, uint32_t Speed)
{
  uint32_t count;
  uint32_t response = 0U;
  uint32_t errorstate = HAL_MMC_ERROR_NONE;
  uint32_t power_class;
  uint32_t supported_pwr_class;

  if ((Wide == SDMMC_BUS_WIDE_8B) || (Wide == SDMMC_BUS_WIDE_4B))
  {
    power_class = 0U; /* Default value after power-on or software reset */

    /* Read the PowerClass field of the Extended CSD register */
    if (MMC_ReadExtCSD(hmmc, &power_class, 187, SDMMC_DATATIMEOUT) != HAL_OK) /* Field POWER_CLASS [187] */
    {
      errorstate = SDMMC_ERROR_GENERAL_UNKNOWN_ERR;
    }
    else
    {
      power_class = ((power_class >> 24U) & 0x000000FFU);
    }

    /* Get the supported PowerClass field of the Extended CSD register */
    if (Speed == SDMMC_SPEED_MODE_DDR)
    {
      /* Field PWR_CL_DDR_52_xxx [238 or 239] */
      supported_pwr_class = ((hmmc->Ext_CSD[(MMC_EXT_CSD_PWR_CL_DDR_52_INDEX / 4)] >> MMC_EXT_CSD_PWR_CL_DDR_52_POS) &
                             0x000000FFU);
    }
    else if (Speed == SDMMC_SPEED_MODE_HIGH)
    {
      /* Field PWR_CL_52_xxx [200 or 202] */
      supported_pwr_class = ((hmmc->Ext_CSD[(MMC_EXT_CSD_PWR_CL_52_INDEX / 4)] >> MMC_EXT_CSD_PWR_CL_52_POS) &
                             0x000000FFU);
    }
    else
    {
      /* Field PWR_CL_26_xxx [201 or 203] */
      supported_pwr_class = ((hmmc->Ext_CSD[(MMC_EXT_CSD_PWR_CL_26_INDEX / 4)] >> MMC_EXT_CSD_PWR_CL_26_POS) &
                             0x000000FFU);
    }

    if (errorstate == HAL_MMC_ERROR_NONE)
    {
      if (Wide == SDMMC_BUS_WIDE_8B)
      {
        /* Bit [7:4]: power class for 8-bits bus configuration - Bit [3:0]: power class for 4-bits bus configuration */
        supported_pwr_class = (supported_pwr_class >> 4U);
      }

      if ((power_class & 0x0FU) != (supported_pwr_class & 0x0FU))
      {
        /* Need to change current power class */
        errorstate = SDMMC_CmdSwitch(hmmc->Instance, (0x03BB0000U | ((supported_pwr_class & 0x0FU) << 8U)));

        if (errorstate == HAL_MMC_ERROR_NONE)
        {
          /* While card is not ready for data and trial number for sending CMD13 is not exceeded */
          count = SDMMC_MAX_TRIAL;
          do
          {
            errorstate = SDMMC_CmdSendStatus(hmmc->Instance, (uint32_t)(((uint32_t)hmmc->MmcCard.RelCardAdd) << 16U));
            if (errorstate != HAL_MMC_ERROR_NONE)
            {
              break;
            }

            /* Get command response */
            response = SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP1);
            count--;
          } while (((response & 0x100U) == 0U) && (count != 0U));

          /* Check the status after the switch command execution */
          if ((count != 0U) && (errorstate == HAL_MMC_ERROR_NONE))
          {
            /* Check the bit SWITCH_ERROR of the device status */
            if ((response & 0x80U) != 0U)
            {
              errorstate = SDMMC_ERROR_UNSUPPORTED_FEATURE;
            }
          }
          else if (count == 0U)
          {
            errorstate = SDMMC_ERROR_TIMEOUT;
          }
          else
          {
            /* Nothing to do */
          }
        }
      }
    }
  }

  return errorstate;
}

/**
  * @brief  Used to select the partition.
  * @param  hmmc: Pointer to MMC handle
  * @param  Partition: Partition type
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_SwitchPartition(MMC_HandleTypeDef *hmmc, HAL_MMC_PartitionTypeDef Partition)
{
  uint32_t errorstate;
  uint32_t response = 0U;
  uint32_t count;
  uint32_t tickstart = HAL_GetTick();
  uint32_t arg = Partition | 0x03B30000U;

  /* Check the state of the driver */
  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    /* Change State */
    hmmc->State = HAL_MMC_STATE_BUSY;

    /* Index : 179 - Value : partition */
    errorstate = SDMMC_CmdSwitch(hmmc->Instance, arg);
    if (errorstate == HAL_MMC_ERROR_NONE)
    {
      /* Wait that the device is ready by checking the D0 line */
      while ((!__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_BUSYD0END)) && (errorstate == HAL_MMC_ERROR_NONE))
      {
        if ((HAL_GetTick() - tickstart) >= SDMMC_MAXERASETIMEOUT)
        {
          errorstate = HAL_MMC_ERROR_TIMEOUT;
        }
      }

      /* Clear the flag corresponding to end D0 bus line */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_FLAG_BUSYD0END);

      if (errorstate == HAL_MMC_ERROR_NONE)
      {
        /* While card is not ready for data and trial number for sending CMD13 is not exceeded */
        count = SDMMC_MAX_TRIAL;
        do
        {
          errorstate = SDMMC_CmdSendStatus(hmmc->Instance, (uint32_t)(((uint32_t)hmmc->MmcCard.RelCardAdd) << 16U));
          if (errorstate != HAL_MMC_ERROR_NONE)
          {
            break;
          }

          /* Get command response */
          response = SDMMC_GetResponse(hmmc->Instance, SDMMC_RESP1);
          count--;
        } while (((response & 0x100U) == 0U) && (count != 0U));

        /* Check the status after the switch command execution */
        if ((count != 0U) && (errorstate == HAL_MMC_ERROR_NONE))
        {
          /* Check the bit SWITCH_ERROR of the device status */
          if ((response & 0x80U) != 0U)
          {
            errorstate = SDMMC_ERROR_GENERAL_UNKNOWN_ERR;
          }
        }
        else if (count == 0U)
        {
          errorstate = SDMMC_ERROR_TIMEOUT;
        }
        else
        {
          /* Nothing to do */
        }
      }
    }

    /* Change State */
    hmmc->State = HAL_MMC_STATE_READY;

    /* Manage errors */
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;

      if (errorstate != HAL_MMC_ERROR_TIMEOUT)
      {
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }
    else
    {
      return HAL_OK;
    }
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Allows to program the authentication key within the RPMB partition
  * @param  hmmc: Pointer to MMC handle
  * @param  pKey: pointer to the authentication key (32 bytes)
  * @param  Timeout: Specify timeout value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_RPMB_ProgramAuthenticationKey(MMC_HandleTypeDef *hmmc, const uint8_t *pKey, uint32_t Timeout)
{
  SDMMC_DataInitTypeDef config;
  uint32_t errorstate;
  uint32_t tickstart = HAL_GetTick();
  uint32_t count;
  uint32_t byte_count = 0;
  uint32_t data;
  uint32_t dataremaining;
  uint8_t tail_pack[12] = {0};
  uint8_t zero_pack[4] = {0};
  const uint8_t *rtempbuff;
  uint8_t  *tempbuff;

  tail_pack[11] = 0x01;

  if (NULL == pKey)
  {
    hmmc->ErrorCode |= HAL_MMC_ERROR_PARAM;
    return HAL_ERROR;
  }

  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    hmmc->ErrorCode = HAL_MMC_ERROR_NONE;

    hmmc->State = HAL_MMC_STATE_BUSY;

    /* Initialize data control register */
    hmmc->Instance->DCTRL = 0U;

    errorstate = SDMMC_CmdBlockCount(hmmc->Instance,  0x80000001U);
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Configure the MMC DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = MMC_BLOCKSIZE;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_CARD;
    config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hmmc->Instance, &config);
    __SDMMC_CMDTRANS_ENABLE(hmmc->Instance);

    /* Write Blocks in Polling mode */
    {
      hmmc->Context = MMC_CONTEXT_WRITE_MULTIPLE_BLOCK;

      /* Write Multi Block command */
      errorstate = SDMMC_CmdWriteMultiBlock(hmmc->Instance, 0);
    }

    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Write block(s) in polling mode */
    rtempbuff = zero_pack;
    dataremaining = config.DataLength;
    while (!__HAL_MMC_GET_FLAG(hmmc,
                               SDMMC_FLAG_TXUNDERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DATAEND))
    {
      if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_TXFIFOHE) && (dataremaining >= SDMMC_FIFO_SIZE))
      {
        /* Write data to SDMMC Tx FIFO */
        for (count = 0U; count < (SDMMC_FIFO_SIZE / 4U); count++)
        {
          data = (uint32_t)(*rtempbuff);
          rtempbuff++;
          byte_count++;
          data |= ((uint32_t)(*rtempbuff) << 8U);
          rtempbuff++;
          byte_count++;
          data |= ((uint32_t)(*rtempbuff) << 16U);
          rtempbuff++;
          byte_count++;
          data |= ((uint32_t)(*rtempbuff) << 24U);
          rtempbuff++;
          byte_count++;
          (void)SDMMC_WriteFIFO(hmmc->Instance, &data);
          if (byte_count < MMC_RPMB_KEYMAC_POSITION)
          {
            rtempbuff = zero_pack;
          }
          else if (byte_count == MMC_RPMB_KEYMAC_POSITION)
          {
            rtempbuff = pKey;
          }
          else if ((byte_count < MMC_RPMB_WRITE_COUNTER_POSITION) && \
                   (byte_count >= MMC_RPMB_DATA_POSITION))
          {
            rtempbuff = zero_pack;
          }
          else if (byte_count == MMC_RPMB_WRITE_COUNTER_POSITION)
          {
            rtempbuff = tail_pack;
          }
          else
          {
            /* Nothing to do */
          }

        }
        dataremaining -= SDMMC_FIFO_SIZE;
      }

      if (((HAL_GetTick() - tickstart) >=  Timeout) || (Timeout == 0U))
      {
        /* Clear all the static flags */
        __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
        hmmc->ErrorCode |= errorstate;
        hmmc->State = HAL_MMC_STATE_READY;
        return HAL_TIMEOUT;
      }
    }
    __SDMMC_CMDTRANS_DISABLE(hmmc->Instance);

    /* Read Response Packet */
    errorstate = SDMMC_CmdBlockCount(hmmc->Instance,  0x00000001);
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Configure the MMC DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = MMC_BLOCKSIZE;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_SDMMC;
    config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hmmc->Instance, &config);
    __SDMMC_CMDTRANS_ENABLE(hmmc->Instance);

    /* Write Blocks in Polling mode */
    hmmc->Context = MMC_CONTEXT_READ_MULTIPLE_BLOCK;

    /* Write Multi Block command */
    errorstate = SDMMC_CmdReadMultiBlock(hmmc->Instance, 0);

    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Clear all the static flags */
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_DATA_FLAGS);
    /* Poll on SDMMC flags */
    tempbuff = zero_pack;
    byte_count = 0;

    dataremaining = config.DataLength;
    while (!__HAL_MMC_GET_FLAG(hmmc,
                               SDMMC_FLAG_RXOVERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DATAEND))
    {
      if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_RXFIFOHF) && (dataremaining >= SDMMC_FIFO_SIZE))
      {
        /* Read data from SDMMC Rx FIFO */
        for (count = 0U; count < (SDMMC_FIFO_SIZE / 4U); count++)
        {
          data = SDMMC_ReadFIFO(hmmc->Instance);
          *tempbuff = (uint8_t)(data & 0xFFU);
          tempbuff++;
          byte_count++;
          *tempbuff = (uint8_t)((data >> 8U) & 0xFFU);
          tempbuff++;
          byte_count++;
          *tempbuff = (uint8_t)((data >> 16U) & 0xFFU);
          tempbuff++;
          byte_count++;
          *tempbuff = (uint8_t)((data >> 24U) & 0xFFU);
          tempbuff++;
          byte_count++;
          if (byte_count < MMC_RPMB_WRITE_COUNTER_POSITION)
          {
            tempbuff = zero_pack;
          }
          else if (byte_count == MMC_RPMB_WRITE_COUNTER_POSITION)
          {
            tempbuff = tail_pack;
          }
          else
          {
            /* Nothing to do */
          }
        }
        dataremaining -= SDMMC_FIFO_SIZE;
      }

      if (((HAL_GetTick() - tickstart) >=  Timeout) || (Timeout == 0U))
      {
        /* Clear all the static flags */
        __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
        hmmc->ErrorCode |= HAL_MMC_ERROR_TIMEOUT;
        hmmc->State = HAL_MMC_STATE_READY;
        return HAL_TIMEOUT;
      }
    }
    __SDMMC_CMDTRANS_DISABLE(hmmc->Instance);

    /* Get error state */
    if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DTIMEOUT))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_DATA_TIMEOUT;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }
    else if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DCRCFAIL))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_DATA_CRC_FAIL;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }
    else if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_TXUNDERR))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_TX_UNDERRUN;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }
    else
    {
      /* Nothing to do */
    }

    /* Clear all the static flags */
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_DATA_FLAGS);

    hmmc->State = HAL_MMC_STATE_READY;

    /* Check result of operation */
    if ((tail_pack[9] != 0x00U) || (tail_pack[10] != 0x01U))
    {
      hmmc->RPMBErrorCode |= tail_pack[9];
      return HAL_ERROR;
    }

    return HAL_OK;
  }
  else
  {
    hmmc->ErrorCode |= HAL_MMC_ERROR_BUSY;
    return HAL_ERROR;
  }
}

/**
  * @brief  Allows to get the value of write counter within the RPMB partition.
  * @param  hmmc: Pointer to MMC handle
  * @param  pNonce: pointer to the value of nonce (16 bytes)
  * @param  Timeout: Specify timeout value
  * @retval write counter value.
  */
uint32_t HAL_MMC_RPMB_GetWriteCounter(MMC_HandleTypeDef *hmmc, uint8_t *pNonce, uint32_t Timeout)
{
  SDMMC_DataInitTypeDef config;
  uint32_t errorstate;
  uint32_t tickstart = HAL_GetTick();
  uint32_t count;
  uint32_t byte_count = 0;
  uint32_t data;
  uint32_t dataremaining;
  uint8_t tail_pack[12] = {0};
  uint8_t zero_pack[4] = {0};
  uint8_t echo_nonce[16] = {0};
  uint8_t *tempbuff = zero_pack;

  tail_pack[11] = 0x02;

  if (NULL == pNonce)
  {
    hmmc->ErrorCode |= HAL_MMC_ERROR_PARAM;
    hmmc->RPMBErrorCode |= HAL_MMC_ERROR_RPMB_COUNTER_FAILURE;
    return 0;
  }

  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    hmmc->ErrorCode = HAL_MMC_ERROR_NONE;
    hmmc->State = HAL_MMC_STATE_BUSY;

    /* Initialize data control register */
    hmmc->Instance->DCTRL = 0U;

    errorstate = SDMMC_CmdBlockCount(hmmc->Instance,  0x00000001U);
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      hmmc->RPMBErrorCode |= HAL_MMC_ERROR_RPMB_COUNTER_FAILURE;
      return 0;
    }

    /* Send Request Packet */

    /* Configure the MMC DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = MMC_BLOCKSIZE;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_CARD;
    config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hmmc->Instance, &config);
    __SDMMC_CMDTRANS_ENABLE(hmmc->Instance);

    /* Write Blocks in Polling mode */
    hmmc->Context = MMC_CONTEXT_WRITE_MULTIPLE_BLOCK;

    /* Write Multi Block command */
    errorstate = SDMMC_CmdWriteMultiBlock(hmmc->Instance, 0);

    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      hmmc->RPMBErrorCode |= HAL_MMC_ERROR_RPMB_COUNTER_FAILURE;
      return 0;
    }

    /* Write block(s) in polling mode */
    dataremaining = config.DataLength;
    while (!__HAL_MMC_GET_FLAG(hmmc,
                               SDMMC_FLAG_TXUNDERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DATAEND))
    {
      if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_TXFIFOHE) && (dataremaining >= SDMMC_FIFO_SIZE))
      {

        /* Write data to SDMMC Tx FIFO */
        for (count = 0U; count < (SDMMC_FIFO_SIZE / 4U); count++)
        {
          data = (uint32_t)(*tempbuff);
          tempbuff++;
          byte_count++;
          data |= ((uint32_t)(*tempbuff) << 8U);
          tempbuff++;
          byte_count++;
          data |= ((uint32_t)(*tempbuff) << 16U);
          tempbuff++;
          byte_count++;
          data |= ((uint32_t)(*tempbuff) << 24U);
          tempbuff++;
          byte_count++;
          (void)SDMMC_WriteFIFO(hmmc->Instance, &data);
          if (byte_count < MMC_RPMB_NONCE_POSITION)
          {
            tempbuff = zero_pack;
          }
          else if (byte_count == MMC_RPMB_NONCE_POSITION)
          {
            tempbuff = (uint8_t *)pNonce;
          }
          else if (byte_count == MMC_RPMB_WRITE_COUNTER_POSITION)
          {
            tempbuff = tail_pack;
          }
          else
          {
            /* Nothing to do */
          }

        }
        dataremaining -= SDMMC_FIFO_SIZE;
      }

      if (((HAL_GetTick() - tickstart) >=  Timeout) || (Timeout == 0U))
      {
        /* Clear all the static flags */
        __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
        hmmc->ErrorCode |= errorstate;
        hmmc->State = HAL_MMC_STATE_READY;
        hmmc->RPMBErrorCode |= HAL_MMC_ERROR_RPMB_COUNTER_FAILURE;
        return 0;
      }
    }
    __SDMMC_CMDTRANS_DISABLE(hmmc->Instance);

    /* Read Response Packt */
    errorstate = SDMMC_CmdBlockCount(hmmc->Instance,  0x00000001U);
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      hmmc->RPMBErrorCode |= HAL_MMC_ERROR_RPMB_COUNTER_FAILURE;
      return 0;
    }

    /* Configure the MMC DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = MMC_BLOCKSIZE;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_SDMMC;
    config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hmmc->Instance, &config);
    __SDMMC_CMDTRANS_ENABLE(hmmc->Instance);

    /* Write Blocks in Polling mode */
    hmmc->Context = MMC_CONTEXT_READ_MULTIPLE_BLOCK;

    /* Write Multi Block command */
    errorstate = SDMMC_CmdReadMultiBlock(hmmc->Instance, 0);

    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      hmmc->RPMBErrorCode |= HAL_MMC_ERROR_RPMB_COUNTER_FAILURE;
      return 0;
    }

    /* Clear all the static flags */
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_DATA_FLAGS);
    /* Poll on SDMMC flags */
    tempbuff = zero_pack;

    byte_count = 0;
    dataremaining = config.DataLength;
    while (!__HAL_MMC_GET_FLAG(hmmc,
                               SDMMC_FLAG_RXOVERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DATAEND))
    {
      if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_RXFIFOHF) && (dataremaining >= SDMMC_FIFO_SIZE))
      {
        /* Read data from SDMMC Rx FIFO */
        for (count = 0U; count < (SDMMC_FIFO_SIZE / 4U); count++)
        {
          data = SDMMC_ReadFIFO(hmmc->Instance);
          *tempbuff = (uint8_t)(data & 0xFFU);
          tempbuff++;
          byte_count++;
          *tempbuff = (uint8_t)((data >> 8U) & 0xFFU);
          tempbuff++;
          byte_count++;
          *tempbuff = (uint8_t)((data >> 16U) & 0xFFU);
          tempbuff++;
          byte_count++;
          *tempbuff = (uint8_t)((data >> 24U) & 0xFFU);
          tempbuff++;
          byte_count++;
          if (byte_count < MMC_RPMB_NONCE_POSITION)
          {
            tempbuff = zero_pack;
          }
          else if (byte_count == MMC_RPMB_NONCE_POSITION)
          {
            tempbuff = echo_nonce;
          }
          else if (byte_count == MMC_RPMB_WRITE_COUNTER_POSITION)
          {
            tempbuff = tail_pack;
          }
          else
          {
            /* Nothing to do */
          }
        }
        dataremaining -= SDMMC_FIFO_SIZE;
      }

      if (((HAL_GetTick() - tickstart) >=  Timeout) || (Timeout == 0U))
      {
        /* Clear all the static flags */
        __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
        hmmc->ErrorCode |= HAL_MMC_ERROR_TIMEOUT;
        hmmc->State = HAL_MMC_STATE_READY;
        hmmc->RPMBErrorCode |= HAL_MMC_ERROR_RPMB_COUNTER_FAILURE;
        return 0;
      }
    }
    __SDMMC_CMDTRANS_DISABLE(hmmc->Instance);

    /* Get error state */
    if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DTIMEOUT))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_DATA_TIMEOUT;
      hmmc->State = HAL_MMC_STATE_READY;
      hmmc->RPMBErrorCode |= HAL_MMC_ERROR_RPMB_COUNTER_FAILURE;
      return 0;
    }
    else if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DCRCFAIL))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_DATA_CRC_FAIL;
      hmmc->State = HAL_MMC_STATE_READY;
      hmmc->RPMBErrorCode |= HAL_MMC_ERROR_RPMB_COUNTER_FAILURE;
      return 0;
    }
    else if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_TXUNDERR))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_TX_UNDERRUN;
      hmmc->State = HAL_MMC_STATE_READY;
      hmmc->RPMBErrorCode |= HAL_MMC_ERROR_RPMB_COUNTER_FAILURE;
      return 0;
    }
    else
    {
      /* Nothing to do */
    }

    /* Clear all the static flags */
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_DATA_FLAGS);

    hmmc->State = HAL_MMC_STATE_READY;

    for (uint8_t i = 0; i < 16U; i++)
    {
      if (pNonce[i] != echo_nonce[i])
      {
        hmmc->RPMBErrorCode |= HAL_MMC_ERROR_RPMB_COUNTER_FAILURE;
        return 0;
      }
    }

    return ((uint32_t)tail_pack[3] | ((uint32_t)tail_pack[2] << 8) | ((uint32_t)tail_pack[1] << 16) | \
            ((uint32_t)tail_pack[0] << 24));
  }
  else
  {
    hmmc->ErrorCode |= HAL_MMC_ERROR_BUSY;
    hmmc->RPMBErrorCode |= HAL_MMC_ERROR_RPMB_COUNTER_FAILURE;
    return 0;
  }
}

/**
  * @brief  Allows to write block(s) to a specified address in the RPMB partition. The Data
  *         transfer is managed by polling mode.
  * @param  hmmc: Pointer to MMC handle
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  BlockAdd: Block Address where data will be written
  * @param  NumberOfBlocks: Number of blocks to write
  * @param  pMAC: Pointer to the authentication MAC buffer
  * @param  Timeout: Specify timeout value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_RPMB_WriteBlocks(MMC_HandleTypeDef *hmmc, const uint8_t *pData, uint16_t BlockAdd,
                                           uint16_t NumberOfBlocks, const uint8_t *pMAC, uint32_t Timeout)
{

  SDMMC_DataInitTypeDef config;
  uint32_t errorstate;
  uint32_t tickstart = HAL_GetTick();
  uint32_t count;
  uint32_t byte_count = 0;
  uint32_t data;
  uint32_t dataremaining;
  uint8_t tail_pack[12] = {0};
  uint8_t zero_pack[4] = {0};
  uint8_t echo_nonce[16] = {0};
  const uint8_t local_nonce[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x00, 0x01, 0x02,
                                   0x03, 0x04, 0x00, 0x01, 0x02, 0x03, 0x04, 0x08
                                  };
  const uint8_t *rtempbuff;
  uint8_t  *tempbuff;
  uint32_t arg = 0x80000000U;
  uint32_t offset = 0;

  if ((NumberOfBlocks != 0x1U) && (NumberOfBlocks != 0x2U) && (NumberOfBlocks != 0x20U))
  {
    hmmc->ErrorCode |= HAL_MMC_ERROR_PARAM;
    return HAL_ERROR;
  }

  if ((NULL == pData) || (NULL == pMAC))
  {
    hmmc->ErrorCode |= HAL_MMC_ERROR_PARAM;
    return HAL_ERROR;
  }

  tail_pack[11] = 0x02;

  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    hmmc->ErrorCode = HAL_MMC_ERROR_NONE;
    hmmc->State = HAL_MMC_STATE_BUSY;

    /* Initialize data control register */
    hmmc->Instance->DCTRL = 0U;

    errorstate = SDMMC_CmdBlockCount(hmmc->Instance,  0x00000001U);
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Send Request Packet */

    /* Configure the MMC DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = MMC_BLOCKSIZE;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_CARD;
    config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hmmc->Instance, &config);
    __SDMMC_CMDTRANS_ENABLE(hmmc->Instance);

    /* Write Blocks in Polling mode */
    hmmc->Context = MMC_CONTEXT_WRITE_MULTIPLE_BLOCK;

    /* Write Multi Block command */
    errorstate = SDMMC_CmdWriteMultiBlock(hmmc->Instance, 0);

    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Write block(s) in polling mode */
    rtempbuff = zero_pack;
    dataremaining = config.DataLength;
    while (!__HAL_MMC_GET_FLAG(hmmc,
                               SDMMC_FLAG_TXUNDERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DATAEND))
    {
      if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_TXFIFOHE) && (dataremaining >= SDMMC_FIFO_SIZE))
      {

        /* Write data to SDMMC Tx FIFO */
        for (count = 0U; count < (SDMMC_FIFO_SIZE / 4U); count++)
        {
          data = (uint32_t)(*rtempbuff);
          rtempbuff++;
          byte_count++;
          data |= ((uint32_t)(*rtempbuff) << 8U);
          rtempbuff++;
          byte_count++;
          data |= ((uint32_t)(*rtempbuff) << 16U);
          rtempbuff++;
          byte_count++;
          data |= ((uint32_t)(*rtempbuff) << 24U);
          rtempbuff++;
          byte_count++;
          (void)SDMMC_WriteFIFO(hmmc->Instance, &data);
          if (byte_count < MMC_RPMB_NONCE_POSITION)
          {
            rtempbuff = zero_pack;
          }
          else if (byte_count == MMC_RPMB_NONCE_POSITION)
          {
            rtempbuff = local_nonce;
          }
          else if (byte_count == MMC_RPMB_WRITE_COUNTER_POSITION)
          {
            rtempbuff = tail_pack;
          }
          else
          {
            /* Nothing to do */
          }
        }
        dataremaining -= SDMMC_FIFO_SIZE;
      }

      if (((HAL_GetTick() - tickstart) >=  Timeout) || (Timeout == 0U))
      {
        /* Clear all the static flags */
        __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
        hmmc->ErrorCode |= errorstate;
        hmmc->State = HAL_MMC_STATE_READY;
        return HAL_TIMEOUT;
      }
    }
    __SDMMC_CMDTRANS_DISABLE(hmmc->Instance);

    /* Read Response Packt */
    errorstate = SDMMC_CmdBlockCount(hmmc->Instance,  0x00000001);
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Configure the MMC DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = MMC_BLOCKSIZE;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_SDMMC;
    config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hmmc->Instance, &config);
    __SDMMC_CMDTRANS_ENABLE(hmmc->Instance);

    /* Write Blocks in Polling mode */
    hmmc->Context = MMC_CONTEXT_READ_MULTIPLE_BLOCK;

    /* Write Multi Block command */
    errorstate = SDMMC_CmdReadMultiBlock(hmmc->Instance, 0);

    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Clear all the static flags */
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_DATA_FLAGS);
    /* Poll on SDMMC flags */
    tempbuff = zero_pack;

    byte_count = 0;
    dataremaining = config.DataLength;
    while (!__HAL_MMC_GET_FLAG(hmmc,
                               SDMMC_FLAG_RXOVERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DATAEND))
    {
      if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_RXFIFOHF) && (dataremaining >= SDMMC_FIFO_SIZE))
      {
        /* Read data from SDMMC Rx FIFO */
        for (count = 0U; count < (SDMMC_FIFO_SIZE / 4U); count++)
        {
          data = SDMMC_ReadFIFO(hmmc->Instance);
          *tempbuff = (uint8_t)(data & 0xFFU);
          tempbuff++;
          byte_count++;
          *tempbuff = (uint8_t)((data >> 8U) & 0xFFU);
          tempbuff++;
          byte_count++;
          *tempbuff = (uint8_t)((data >> 16U) & 0xFFU);
          tempbuff++;
          byte_count++;
          *tempbuff = (uint8_t)((data >> 24U) & 0xFFU);
          tempbuff++;
          byte_count++;
          if (byte_count < MMC_RPMB_NONCE_POSITION)
          {
            tempbuff = zero_pack;
          }
          else if (byte_count == MMC_RPMB_NONCE_POSITION)
          {
            tempbuff = echo_nonce;
          }
          else if (byte_count == MMC_RPMB_WRITE_COUNTER_POSITION)
          {
            tempbuff = tail_pack;
          }
          else
          {
            /* Nothing to do */
          }
        }
        dataremaining -= SDMMC_FIFO_SIZE;
      }

      if (((HAL_GetTick() - tickstart) >=  Timeout) || (Timeout == 0U))
      {
        /* Clear all the static flags */
        __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
        hmmc->ErrorCode |= HAL_MMC_ERROR_TIMEOUT;
        hmmc->State = HAL_MMC_STATE_READY;
        return HAL_TIMEOUT;
      }
    }
    __SDMMC_CMDTRANS_DISABLE(hmmc->Instance);

    /* Get error state */
    if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DTIMEOUT))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_DATA_TIMEOUT;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }
    else if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DCRCFAIL))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_DATA_CRC_FAIL;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }
    else if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_TXUNDERR))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_TX_UNDERRUN;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }
    else
    {
      /* Nothing to do */
    }

    /* Clear all the static flags */
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_DATA_FLAGS);

    hmmc->State = HAL_MMC_STATE_READY;

    for (uint8_t i = 0; i < 16U; i++)
    {
      if (local_nonce[i] != echo_nonce[i])
      {
        return HAL_ERROR;
      }
    }
  }
  else
  {
    hmmc->ErrorCode |= HAL_MMC_ERROR_BUSY;
    return HAL_ERROR;
  }
  tail_pack[11]  = 0x03;
  tail_pack[10]  = 0x00;
  tail_pack[7]   = (uint8_t)(NumberOfBlocks) & 0xFFU;
  tail_pack[6]   = (uint8_t)(NumberOfBlocks >> 8) & 0xFFU;
  tail_pack[5]   = (uint8_t)(BlockAdd) & 0xFFU;
  tail_pack[4]   = (uint8_t)(BlockAdd >> 8) & 0xFFU;

  rtempbuff = zero_pack;
  byte_count = 0;
  arg |= NumberOfBlocks;

  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    hmmc->ErrorCode = HAL_MMC_ERROR_NONE;


    hmmc->State = HAL_MMC_STATE_BUSY;

    /* Initialize data control register */
    hmmc->Instance->DCTRL = 0U;

    errorstate = SDMMC_CmdBlockCount(hmmc->Instance, arg);
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Send Request Packet */
    /* Configure the MMC DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = NumberOfBlocks * MMC_BLOCKSIZE;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_CARD;
    config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hmmc->Instance, &config);
    __SDMMC_CMDTRANS_ENABLE(hmmc->Instance);

    /* Write Blocks in Polling mode */

    {
      hmmc->Context = MMC_CONTEXT_WRITE_MULTIPLE_BLOCK;

      /* Write Multi Block command */
      errorstate = SDMMC_CmdWriteMultiBlock(hmmc->Instance, 0);
    }

    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }


    /* Write block(s) in polling mode */
    dataremaining = config.DataLength;
    while (!__HAL_MMC_GET_FLAG(hmmc,
                               SDMMC_FLAG_TXUNDERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DATAEND))
    {
      if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_TXFIFOHE) && (dataremaining >= SDMMC_FIFO_SIZE))
      {

        /* Write data to SDMMC Tx FIFO */
        for (count = 0U; count < (SDMMC_FIFO_SIZE / 4U); count++)
        {
          data = (uint32_t)(*rtempbuff);
          rtempbuff++;
          byte_count++;
          data |= ((uint32_t)(*rtempbuff) << 8U);
          rtempbuff++;
          byte_count++;
          data |= ((uint32_t)(*rtempbuff) << 16U);
          rtempbuff++;
          byte_count++;
          data |= ((uint32_t)(*rtempbuff) << 24U);
          rtempbuff++;
          byte_count++;
          (void)SDMMC_WriteFIFO(hmmc->Instance, &data);
          if (byte_count == MMC_RPMB_KEYMAC_POSITION)
          {
            rtempbuff = pMAC;
          }
          if (byte_count == MMC_RPMB_DATA_POSITION)
          {
            rtempbuff = &pData[offset];
          }
          if ((byte_count >= MMC_RPMB_NONCE_POSITION) && \
              (byte_count < MMC_RPMB_WRITE_COUNTER_POSITION))
          {
            rtempbuff = zero_pack;
          }
          if (byte_count == MMC_RPMB_WRITE_COUNTER_POSITION)
          {
            rtempbuff = tail_pack;
          }
          else if (byte_count == MMC_BLOCKSIZE)
          {
            offset += (uint32_t)256U;
            byte_count = 0;
          }
          else
          {
            /* Nothing to do */
          }
        }
        dataremaining -= SDMMC_FIFO_SIZE;
      }

      if (((HAL_GetTick() - tickstart) >=  Timeout) || (Timeout == 0U))
      {
        /* Clear all the static flags */
        __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
        hmmc->ErrorCode |= errorstate;
        hmmc->State = HAL_MMC_STATE_READY;
        return HAL_TIMEOUT;
      }
    }
    __SDMMC_CMDTRANS_DISABLE(hmmc->Instance);

    /* Response Packet */

    errorstate = SDMMC_CmdBlockCount(hmmc->Instance, arg);
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Configure the MMC DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = MMC_BLOCKSIZE;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_SDMMC;
    config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hmmc->Instance, &config);
    __SDMMC_CMDTRANS_ENABLE(hmmc->Instance);

    /* Write Blocks in Polling mode */

    {
      hmmc->Context = MMC_CONTEXT_READ_MULTIPLE_BLOCK;

      /* Write Multi Block command */
      errorstate = SDMMC_CmdReadMultiBlock(hmmc->Instance, 0);
    }

    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }


    /* Clear all the static flags */
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_DATA_FLAGS);
    /* Poll on SDMMC flags */
    tempbuff = zero_pack;
    byte_count = 0;
    dataremaining = config.DataLength;
    while (!__HAL_MMC_GET_FLAG(hmmc,
                               SDMMC_FLAG_RXOVERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DATAEND))
    {
      if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_RXFIFOHF) && (dataremaining >= SDMMC_FIFO_SIZE))
      {
        /* Read data from SDMMC Rx FIFO */
        for (count = 0U; count < (SDMMC_FIFO_SIZE / 4U); count++)
        {
          data = SDMMC_ReadFIFO(hmmc->Instance);
          *tempbuff = (uint8_t)(data & 0xFFU);
          tempbuff++;
          byte_count++;
          *tempbuff = (uint8_t)((data >> 8U) & 0xFFU);
          tempbuff++;
          byte_count++;
          *tempbuff = (uint8_t)((data >> 16U) & 0xFFU);
          tempbuff++;
          byte_count++;
          *tempbuff = (uint8_t)((data >> 24U) & 0xFFU);
          tempbuff++;
          byte_count++;
          if (byte_count < MMC_RPMB_WRITE_COUNTER_POSITION)
          {
            tempbuff = zero_pack;
          }
          else if (byte_count == MMC_RPMB_WRITE_COUNTER_POSITION)
          {
            tempbuff = tail_pack;
          }
          else
          {
            /* Nothing to do */
          }
        }
        dataremaining -= SDMMC_FIFO_SIZE;
      }

      if (((HAL_GetTick() - tickstart) >=  Timeout) || (Timeout == 0U))
      {
        /* Clear all the static flags */
        __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
        hmmc->ErrorCode |= HAL_MMC_ERROR_TIMEOUT;
        hmmc->State = HAL_MMC_STATE_READY;
        return HAL_TIMEOUT;
      }
    }
    __SDMMC_CMDTRANS_DISABLE(hmmc->Instance);

    /* Get error state */
    if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DTIMEOUT))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_DATA_TIMEOUT;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }
    else if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DCRCFAIL))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_DATA_CRC_FAIL;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }
    else if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_TXUNDERR))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_TX_UNDERRUN;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }
    else
    {
      /* Nothing to do */
    }

    /* Clear all the static flags */
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_DATA_FLAGS);

    hmmc->State = HAL_MMC_STATE_READY;

    /* Check result of operation */
    if (((tail_pack[9] & (uint8_t)0xFEU) != 0x00U) || (tail_pack[10] != 0x03U))
    {
      hmmc->RPMBErrorCode |= tail_pack[9];
      return HAL_ERROR;
    }

    return HAL_OK;
  }
  else
  {
    hmmc->ErrorCode |= HAL_MMC_ERROR_BUSY;
    return HAL_ERROR;
  }
}

/**
  * @brief  Allows to read block(s) to a specified address in the RPMB partition. The Data
  *         transfer is managed by polling mode.
  * @param  hmmc: Pointer to MMC handle
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  BlockAdd: Block Address where data will be written
  * @param  NumberOfBlocks: Number of blocks to write
  * @param  pNonce: Pointer to the buffer that will contain the nonce to transmit
  * @param  pMAC: Pointer to the authentication MAC buffer
  * @param  Timeout: Specify timeout value
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMC_RPMB_ReadBlocks(MMC_HandleTypeDef *hmmc, uint8_t *pData, uint16_t BlockAdd,
                                          uint16_t NumberOfBlocks, const uint8_t *pNonce, uint8_t *pMAC,
                                          uint32_t Timeout)
{
  SDMMC_DataInitTypeDef config;
  uint32_t errorstate;
  uint32_t tickstart = HAL_GetTick();
  uint32_t count;
  uint32_t byte_count = 0;
  uint32_t data;
  uint8_t tail_pack[12] = {0};
  uint8_t zero_pack[4] = {0};
  uint8_t echo_nonce[16] = {0};
  uint32_t dataremaining;
  const uint8_t *rtempbuff;
  uint8_t  *tempbuff;
  uint32_t arg = 0;
  uint32_t offset = 0;

  arg |= NumberOfBlocks;

  tail_pack[11] = 0x04;
  tail_pack[10] = 0x00;
  tail_pack[7]  = 0x00;
  tail_pack[6]  = 0x00;
  tail_pack[5]  = (uint8_t)(BlockAdd) & 0xFFU;
  tail_pack[4]  = (uint8_t)(BlockAdd >> 8) & 0xFFU;
  tail_pack[3]  = 0x00;
  tail_pack[2]  = 0x00;
  tail_pack[1]  = 0x00;
  tail_pack[0]  = 0x00;

  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    hmmc->ErrorCode = HAL_MMC_ERROR_NONE;
    hmmc->State = HAL_MMC_STATE_BUSY;

    /* Initialize data control register */
    hmmc->Instance->DCTRL = 0U;

    errorstate = SDMMC_CmdBlockCount(hmmc->Instance, 1);
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Send Request Packet */

    /* Configure the MMC DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = MMC_BLOCKSIZE;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_CARD;
    config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hmmc->Instance, &config);
    __SDMMC_CMDTRANS_ENABLE(hmmc->Instance);

    /* Write Blocks in Polling mode */
    hmmc->Context = MMC_CONTEXT_WRITE_MULTIPLE_BLOCK;

    /* Write Multi Block command */
    errorstate = SDMMC_CmdWriteMultiBlock(hmmc->Instance, 0);

    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Write block(s) in polling mode */
    rtempbuff = zero_pack;
    dataremaining = config.DataLength;
    while (!__HAL_MMC_GET_FLAG(hmmc,
                               SDMMC_FLAG_TXUNDERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DATAEND))
    {
      if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_TXFIFOHE) && (dataremaining >= SDMMC_FIFO_SIZE))
      {

        /* Write data to SDMMC Tx FIFO */
        for (count = 0U; count < (SDMMC_FIFO_SIZE / 4U); count++)
        {
          data = (uint32_t)(*rtempbuff);
          rtempbuff++;
          byte_count++;
          data |= ((uint32_t)(*rtempbuff) << 8U);
          rtempbuff++;
          byte_count++;
          data |= ((uint32_t)(*rtempbuff) << 16U);
          rtempbuff++;
          byte_count++;
          data |= ((uint32_t)(*rtempbuff) << 24U);
          rtempbuff++;
          byte_count++;
          (void)SDMMC_WriteFIFO(hmmc->Instance, &data);
          if (byte_count < MMC_RPMB_NONCE_POSITION)
          {
            rtempbuff = zero_pack;
          }
          else if (byte_count == MMC_RPMB_NONCE_POSITION)
          {
            rtempbuff = pNonce;
          }
          else if (byte_count == MMC_RPMB_WRITE_COUNTER_POSITION)
          {
            rtempbuff = tail_pack;
          }
          else
          {
            /* Nothing to do */
          }
        }
        dataremaining -= SDMMC_FIFO_SIZE;
      }

      if (((HAL_GetTick() - tickstart) >=  Timeout) || (Timeout == 0U))
      {
        /* Clear all the static flags */
        __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
        hmmc->ErrorCode |= errorstate;
        hmmc->State = HAL_MMC_STATE_READY;
        return HAL_TIMEOUT;
      }
    }
    __SDMMC_CMDTRANS_DISABLE(hmmc->Instance);

    /* Read Response Packet */
    errorstate = SDMMC_CmdBlockCount(hmmc->Instance, arg);
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Configure the MMC DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = NumberOfBlocks * MMC_BLOCKSIZE;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_SDMMC;
    config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hmmc->Instance, &config);
    __SDMMC_CMDTRANS_ENABLE(hmmc->Instance);

    /* Write Blocks in Polling mode */
    hmmc->Context = MMC_CONTEXT_READ_MULTIPLE_BLOCK;

    /* Write Multi Block command */
    errorstate = SDMMC_CmdReadMultiBlock(hmmc->Instance, 0);

    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= errorstate;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }

    /* Clear all the static flags */
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_DATA_FLAGS);
    /* Poll on SDMMC flags */
    tempbuff = zero_pack;
    byte_count = 0;

    dataremaining = config.DataLength;
    while (!__HAL_MMC_GET_FLAG(hmmc,
                               SDMMC_FLAG_RXOVERR | SDMMC_FLAG_DCRCFAIL | SDMMC_FLAG_DTIMEOUT | SDMMC_FLAG_DATAEND))
    {
      if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_RXFIFOHF) && (dataremaining >= SDMMC_FIFO_SIZE))
      {
        /* Read data from SDMMC Rx FIFO */
        for (count = 0U; count < (SDMMC_FIFO_SIZE / 4U); count++)
        {
          data = SDMMC_ReadFIFO(hmmc->Instance);
          *tempbuff = (uint8_t)(data & 0xFFU);
          tempbuff++;
          byte_count++;
          *tempbuff = (uint8_t)((data >> 8U) & 0xFFU);
          tempbuff++;
          byte_count++;
          *tempbuff = (uint8_t)((data >> 16U) & 0xFFU);
          tempbuff++;
          byte_count++;
          *tempbuff = (uint8_t)((data >> 24U) & 0xFFU);
          tempbuff++;
          byte_count++;
          if (byte_count < MMC_RPMB_KEYMAC_POSITION)
          {
            tempbuff = zero_pack;
          }
          else if (byte_count == MMC_RPMB_KEYMAC_POSITION)
          {
            tempbuff = (uint8_t *)pMAC;
          }
          else if (byte_count == MMC_RPMB_DATA_POSITION)
          {
            tempbuff = &pData[offset];
          }
          else if (byte_count == MMC_RPMB_NONCE_POSITION)
          {
            tempbuff = echo_nonce;
          }
          else if (byte_count == MMC_RPMB_WRITE_COUNTER_POSITION)
          {
            tempbuff = tail_pack;
          }
          else if (byte_count == MMC_BLOCKSIZE)
          {
            byte_count = 0;
            offset += (uint32_t)256U;
          }
          else
          {
            /* Nothing to do */
          }
        }
        dataremaining -= SDMMC_FIFO_SIZE;
      }

      if (((HAL_GetTick() - tickstart) >=  Timeout) || (Timeout == 0U))
      {
        /* Clear all the static flags */
        __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
        hmmc->ErrorCode |= HAL_MMC_ERROR_TIMEOUT;
        hmmc->State = HAL_MMC_STATE_READY;
        return HAL_TIMEOUT;
      }
    }
    __SDMMC_CMDTRANS_DISABLE(hmmc->Instance);

    /* Get error state */
    if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DTIMEOUT))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_DATA_TIMEOUT;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }
    else if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_DCRCFAIL))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_DATA_CRC_FAIL;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }
    else if (__HAL_MMC_GET_FLAG(hmmc, SDMMC_FLAG_TXUNDERR))
    {
      /* Clear all the static flags */
      __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_FLAGS);
      hmmc->ErrorCode |= HAL_MMC_ERROR_TX_UNDERRUN;
      hmmc->State = HAL_MMC_STATE_READY;
      return HAL_ERROR;
    }
    else
    {
      /* Nothing to do */
    }

    /* Clear all the static flags */
    __HAL_MMC_CLEAR_FLAG(hmmc, SDMMC_STATIC_DATA_FLAGS);

    hmmc->State = HAL_MMC_STATE_READY;

    for (uint8_t i = 0; i < 16U; i++)
    {
      if (pNonce[i] != echo_nonce[i])
      {
        return HAL_ERROR;
      }
    }

    /* Check result of operation */
    if ((tail_pack[9] != 0x00U) || (tail_pack[10] != 0x04U))
    {
      hmmc->RPMBErrorCode |= tail_pack[9];
      return HAL_ERROR;
    }

    return HAL_OK;
  }
  else
  {
    hmmc->ErrorCode |= HAL_MMC_ERROR_BUSY;
    return HAL_ERROR;
  }
}


/**
  * @brief Read DMA Linked list node Transfer completed callbacks
  * @param hmmc: MMC handle
  * @retval None
  */
__weak void HAL_MMCEx_Read_DMALnkLstBufCpltCallback(MMC_HandleTypeDef *hmmc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmmc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_MMCEx_Read_DMALnkLstBufCpltCallback can be implemented in the user file
   */
}
/**
  * @brief Read DMA Linked list node Transfer completed callbacks
  * @param hmmc: MMC handle
  * @retval None
  */
__weak void HAL_MMCEx_Write_DMALnkLstBufCpltCallback(MMC_HandleTypeDef *hmmc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmmc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_MMCEx_Write_DMALnkLstBufCpltCallback can be implemented in the user file
   */
}

/**
  * @}
  */

#endif /* HAL_MMC_MODULE_ENABLED */
#endif /* SDMMC1 || SDMMC2 */

/**
  * @}
  */

/**
  * @}
  */
