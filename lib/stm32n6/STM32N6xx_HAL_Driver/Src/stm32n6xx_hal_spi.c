/**
  ******************************************************************************
  * @file    stm32n6xx_hal_spi.c
  * @author  MCD Application Team
  * @brief   SPI HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Serial Peripheral Interface (SPI) peripheral:
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
      The SPI HAL driver can be used as follows:

      (#) Declare a SPI_HandleTypeDef handle structure, for example:
          SPI_HandleTypeDef  hspi;

      (#)Initialize the SPI low level resources by implementing the HAL_SPI_MspInit() API:
          (##) Enable the SPIx interface clock
          (##) SPI pins configuration
              (+++) Enable the clock for the SPI GPIOs
              (+++) Configure these SPI pins as alternate function push-pull
          (##) NVIC configuration if you need to use interrupt process or DMA process
              (+++) Configure the SPIx interrupt priority
              (+++) Enable the NVIC SPI IRQ handle
          (##) DMA Configuration if you need to use DMA process
              (+++) Declare a DMA_HandleTypeDef handle structure for the transmit or receive Stream/Channel
              (+++) Enable the DMAx clock
              (+++) Configure the DMA handle parameters
              (+++) Configure the DMA Tx or Rx Stream/Channel
              (+++) Associate the initialized hdma_tx handle to the hspi DMA Tx or Rx handle
              (+++) Configure the priority and enable the NVIC for the transfer complete interrupt on the DMA Tx
                    or Rx Stream/Channel

      (#) Program the Mode, BidirectionalMode , Data size, Baudrate Prescaler, NSS
          management, Clock polarity and phase, FirstBit and CRC configuration in the hspi Init structure.

      (#) Initialize the SPI registers by calling the HAL_SPI_Init() API:
          (++) This API configures also the low level Hardware GPIO, CLOCK, CORTEX...etc)
              by calling the customized HAL_SPI_MspInit() API.
     [..]
       Callback registration:

      (#) The compilation flag USE_HAL_SPI_REGISTER_CALLBACKS when set to 1UL
          allows the user to configure dynamically the driver callbacks.
          Use Functions HAL_SPI_RegisterCallback() to register an interrupt callback.

          Function HAL_SPI_RegisterCallback() allows to register following callbacks:
            (+) TxCpltCallback        : SPI Tx Completed callback
            (+) RxCpltCallback        : SPI Rx Completed callback
            (+) TxRxCpltCallback      : SPI TxRx Completed callback
            (+) TxHalfCpltCallback    : SPI Tx Half Completed callback
            (+) RxHalfCpltCallback    : SPI Rx Half Completed callback
            (+) TxRxHalfCpltCallback  : SPI TxRx Half Completed callback
            (+) ErrorCallback         : SPI Error callback
            (+) AbortCpltCallback     : SPI Abort callback
            (+) SuspendCallback       : SPI Suspend callback
            (+) MspInitCallback       : SPI Msp Init callback
            (+) MspDeInitCallback     : SPI Msp DeInit callback
          This function takes as parameters the HAL peripheral handle, the Callback ID
          and a pointer to the user callback function.


      (#) Use function HAL_SPI_UnRegisterCallback to reset a callback to the default
          weak function.
          HAL_SPI_UnRegisterCallback takes as parameters the HAL peripheral handle,
          and the Callback ID.
          This function allows to reset following callbacks:
            (+) TxCpltCallback        : SPI Tx Completed callback
            (+) RxCpltCallback        : SPI Rx Completed callback
            (+) TxRxCpltCallback      : SPI TxRx Completed callback
            (+) TxHalfCpltCallback    : SPI Tx Half Completed callback
            (+) RxHalfCpltCallback    : SPI Rx Half Completed callback
            (+) TxRxHalfCpltCallback  : SPI TxRx Half Completed callback
            (+) ErrorCallback         : SPI Error callback
            (+) AbortCpltCallback     : SPI Abort callback
            (+) SuspendCallback       : SPI Suspend callback
            (+) MspInitCallback       : SPI Msp Init callback
            (+) MspDeInitCallback     : SPI Msp DeInit callback

       By default, after the HAL_SPI_Init() and when the state is HAL_SPI_STATE_RESET
       all callbacks are set to the corresponding weak functions:
       examples HAL_SPI_MasterTxCpltCallback(), HAL_SPI_MasterRxCpltCallback().
       Exception done for MspInit and MspDeInit functions that are
       reset to the legacy weak functions in the HAL_SPI_Init()/ HAL_SPI_DeInit() only when
       these callbacks are null (not registered beforehand).
       If MspInit or MspDeInit are not null, the HAL_SPI_Init()/ HAL_SPI_DeInit()
       keep and use the user MspInit/MspDeInit callbacks (registered beforehand) whatever the state.

       Callbacks can be registered/unregistered in HAL_SPI_STATE_READY state only.
       Exception done MspInit/MspDeInit functions that can be registered/unregistered
       in HAL_SPI_STATE_READY or HAL_SPI_STATE_RESET state,
       thus registered (user) MspInit/DeInit callbacks can be used during the Init/DeInit.
       Then, the user first registers the MspInit/MspDeInit user callbacks
       using HAL_SPI_RegisterCallback() before calling HAL_SPI_DeInit()
       or HAL_SPI_Init() function.

       When The compilation define USE_HAL_PPP_REGISTER_CALLBACKS is set to 0 or not defined,
       the callback registering feature is not available and weak callbacks are used.

       SuspendCallback restriction:
           SuspendCallback is called only when MasterReceiverAutoSusp is enabled and
       EOT interrupt is activated. SuspendCallback is used in relation with functions
       HAL_SPI_Transmit_IT, HAL_SPI_Receive_IT and HAL_SPI_TransmitReceive_IT.

    [..]
      Circular mode restriction:
      (+) The DMA circular mode cannot be used when the SPI is configured in these modes:
          (++) Master 2Lines RxOnly
          (++) Master 1Line Rx
      (+) The CRC feature is not managed when the DMA circular mode is enabled
      (+) The functions HAL_SPI_DMAPause()/ HAL_SPI_DMAResume() are not supported. Return always
          HAL_ERROR with ErrorCode set to HAL_SPI_ERROR_NOT_SUPPORTED.
          Those functions are maintained for backward compatibility reasons.

  @endverbatim
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup SPI SPI
  * @brief SPI HAL module driver
  * @{
  */
#ifdef HAL_SPI_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/** @defgroup SPI_Private_Constants SPI Private Constants
  * @{
  */
#define SPI_DEFAULT_TIMEOUT 100UL
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @defgroup SPI_Private_Functions SPI Private Functions
  * @{
  */
static void SPI_DMATransmitCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMAReceiveCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMATransmitReceiveCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMAHalfTransmitCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMAHalfReceiveCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMAHalfTransmitReceiveCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMAError(DMA_HandleTypeDef *hdma);
static void SPI_DMAAbortOnError(DMA_HandleTypeDef *hdma);
static void SPI_DMATxAbortCallback(DMA_HandleTypeDef *hdma);
static void SPI_DMARxAbortCallback(DMA_HandleTypeDef *hdma);
static HAL_StatusTypeDef SPI_WaitOnFlagUntilTimeout(const SPI_HandleTypeDef *hspi, uint32_t Flag,
                                                    FlagStatus FlagStatus, uint32_t Timeout, uint32_t Tickstart);
static void SPI_TxISR_8BIT(SPI_HandleTypeDef *hspi);
static void SPI_TxISR_16BIT(SPI_HandleTypeDef *hspi);
static void SPI_TxISR_32BIT(SPI_HandleTypeDef *hspi);
static void SPI_RxISR_8BIT(SPI_HandleTypeDef *hspi);
static void SPI_RxISR_16BIT(SPI_HandleTypeDef *hspi);
static void SPI_RxISR_32BIT(SPI_HandleTypeDef *hspi);
static void SPI_AbortTransfer(SPI_HandleTypeDef *hspi);
static void SPI_CloseTransfer(SPI_HandleTypeDef *hspi);
static uint32_t SPI_GetPacketSize(const SPI_HandleTypeDef *hspi);


/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup SPI_Exported_Functions SPI Exported Functions
  * @{
  */

/** @defgroup SPI_Exported_Functions_Group1 Initialization and de-initialization functions
  *  @brief    Initialization and Configuration functions
  *
@verbatim
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This subsection provides a set of functions allowing to initialize and
          de-initialize the SPIx peripheral:

      (+) User must implement HAL_SPI_MspInit() function in which he configures
          all related peripherals resources (CLOCK, GPIO, DMA, IT and NVIC ).

      (+) Call the function HAL_SPI_Init() to configure the selected device with
          the selected configuration:
        (++) Mode
        (++) Direction
        (++) Data Size
        (++) Clock Polarity and Phase
        (++) NSS Management
        (++) BaudRate Prescaler
        (++) FirstBit
        (++) TIMode
        (++) CRC Calculation
        (++) CRC Polynomial if CRC enabled
        (++) CRC Length, used only with Data8 and Data16
        (++) FIFO reception threshold
        (++) FIFO transmission threshold

      (+) Call the function HAL_SPI_DeInit() to restore the default configuration
          of the selected SPIx peripheral.

@endverbatim
  * @{
  */

/**
  * @brief  Initialize the SPI according to the specified parameters
  *         in the SPI_InitTypeDef and initialize the associated handle.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi)
{
  uint32_t crc_length;
  uint32_t packet_length;
#if (USE_SPI_CRC != 0UL)
  uint32_t crc_poly_msb_mask;
#endif /* USE_SPI_CRC */

  /* Check the SPI handle allocation */
  if (hspi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_SPI_ALL_INSTANCE(hspi->Instance));
  assert_param(IS_SPI_MODE(hspi->Init.Mode));
  assert_param(IS_SPI_DIRECTION(hspi->Init.Direction));
  if (IS_SPI_LIMITED_INSTANCE(hspi->Instance))
  {
    assert_param(IS_SPI_LIMITED_DATASIZE(hspi->Init.DataSize));
    assert_param(IS_SPI_LIMITED_FIFOTHRESHOLD(hspi->Init.FifoThreshold));
  }
  else
  {
    assert_param(IS_SPI_DATASIZE(hspi->Init.DataSize));
    assert_param(IS_SPI_FIFOTHRESHOLD(hspi->Init.FifoThreshold));
  }
  assert_param(IS_SPI_NSS(hspi->Init.NSS));
  assert_param(IS_SPI_NSSP(hspi->Init.NSSPMode));
  assert_param(IS_SPI_BAUDRATE_PRESCALER(hspi->Init.BaudRatePrescaler));
  assert_param(IS_SPI_FIRST_BIT(hspi->Init.FirstBit));
  assert_param(IS_SPI_TIMODE(hspi->Init.TIMode));
  if (hspi->Init.TIMode == SPI_TIMODE_DISABLE)
  {
    assert_param(IS_SPI_CPOL(hspi->Init.CLKPolarity));
    assert_param(IS_SPI_CPHA(hspi->Init.CLKPhase));
  }
#if (USE_SPI_CRC != 0UL)
  assert_param(IS_SPI_CRC_CALCULATION(hspi->Init.CRCCalculation));
  if (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    if (IS_SPI_LIMITED_INSTANCE(hspi->Instance))
    {
      assert_param(IS_SPI_LIMITED_CRC_LENGTH(hspi->Init.CRCLength));
    }
    else
    {
      assert_param(IS_SPI_CRC_LENGTH(hspi->Init.CRCLength));
    }
    assert_param(IS_SPI_CRC_POLYNOMIAL(hspi->Init.CRCPolynomial));
    assert_param(IS_SPI_CRC_INITIALIZATION_PATTERN(hspi->Init.TxCRCInitializationPattern));
    assert_param(IS_SPI_CRC_INITIALIZATION_PATTERN(hspi->Init.RxCRCInitializationPattern));
  }
#else
  hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
#endif /* USE_SPI_CRC */

  assert_param(IS_SPI_RDY_MASTER_MANAGEMENT(hspi->Init.ReadyMasterManagement));
  assert_param(IS_SPI_RDY_POLARITY(hspi->Init.ReadyPolarity));
  assert_param(IS_SPI_MASTER_RX_AUTOSUSP(hspi->Init.MasterReceiverAutoSusp));

  /* Verify that the SPI instance supports Data Size higher than 16bits */
  if ((IS_SPI_LIMITED_INSTANCE(hspi->Instance)) && (hspi->Init.DataSize > SPI_DATASIZE_16BIT))
  {
    return HAL_ERROR;
  }

  /* Verify that the SPI instance supports requested data packing */
  packet_length = SPI_GetPacketSize(hspi);
  if (((IS_SPI_LIMITED_INSTANCE(hspi->Instance)) && (packet_length > SPI_LOWEND_FIFO_SIZE)) ||
      ((IS_SPI_FULL_INSTANCE(hspi->Instance)) && (packet_length > SPI_HIGHEND_FIFO_SIZE)))
  {
    return HAL_ERROR;
  }
#if (USE_SPI_CRC != 0UL)
  if (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    /* Verify that the SPI instance supports CRC Length higher than 16bits */
    if ((IS_SPI_LIMITED_INSTANCE(hspi->Instance)) && (hspi->Init.CRCLength > SPI_CRC_LENGTH_16BIT))
    {
      return HAL_ERROR;
    }

    /* Align the CRC Length on the data size */
    if (hspi->Init.CRCLength == SPI_CRC_LENGTH_DATASIZE)
    {
      crc_length = (hspi->Init.DataSize >> SPI_CFG1_DSIZE_Pos) << SPI_CFG1_CRCSIZE_Pos;
    }
    else
    {
      crc_length = hspi->Init.CRCLength;
    }

    /* Verify the correctness of polynom size */
    assert_param(IS_SPI_CRC_POLYNOMIAL_SIZE(hspi->Init.CRCPolynomial, crc_length));

    /* Verify that the CRC Length is higher than DataSize */
    if ((hspi->Init.DataSize >> SPI_CFG1_DSIZE_Pos) > (crc_length >> SPI_CFG1_CRCSIZE_Pos))
    {
      return HAL_ERROR;
    }
  }
  else
  {
    crc_length = hspi->Init.DataSize << SPI_CFG1_CRCSIZE_Pos;
  }
#endif /* USE_SPI_CRC */

  if (hspi->State == HAL_SPI_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hspi->Lock = HAL_UNLOCKED;

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
    /* Init the SPI Callback settings */
    hspi->TxCpltCallback       = HAL_SPI_TxCpltCallback;       /* Legacy weak TxCpltCallback       */
    hspi->RxCpltCallback       = HAL_SPI_RxCpltCallback;       /* Legacy weak RxCpltCallback       */
    hspi->TxRxCpltCallback     = HAL_SPI_TxRxCpltCallback;     /* Legacy weak TxRxCpltCallback     */
    hspi->TxHalfCpltCallback   = HAL_SPI_TxHalfCpltCallback;   /* Legacy weak TxHalfCpltCallback   */
    hspi->RxHalfCpltCallback   = HAL_SPI_RxHalfCpltCallback;   /* Legacy weak RxHalfCpltCallback   */
    hspi->TxRxHalfCpltCallback = HAL_SPI_TxRxHalfCpltCallback; /* Legacy weak TxRxHalfCpltCallback */
    hspi->ErrorCallback        = HAL_SPI_ErrorCallback;        /* Legacy weak ErrorCallback        */
    hspi->AbortCpltCallback    = HAL_SPI_AbortCpltCallback;    /* Legacy weak AbortCpltCallback    */
    hspi->SuspendCallback      = HAL_SPI_SuspendCallback;      /* Legacy weak SuspendCallback      */

    if (hspi->MspInitCallback == NULL)
    {
      hspi->MspInitCallback = HAL_SPI_MspInit; /* Legacy weak MspInit  */
    }

    /* Init the low level hardware : GPIO, CLOCK, NVIC... */
    hspi->MspInitCallback(hspi);
#else
    /* Init the low level hardware : GPIO, CLOCK, NVIC... */
    HAL_SPI_MspInit(hspi);
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
  }

  hspi->State = HAL_SPI_STATE_BUSY;

  /* Disable the selected SPI peripheral */
  __HAL_SPI_DISABLE(hspi);

#if (USE_SPI_CRC == 0)
  /* Keep the default value of CRCSIZE in case of CRC is not used */
  crc_length = hspi->Instance->CFG1 & SPI_CFG1_CRCSIZE;
#endif /* USE_SPI_CRC */

  /*----------------------- SPIx CR1 & CR2 Configuration ---------------------*/
  /* Configure : SPI Mode, Communication Mode, Clock polarity and phase, NSS management,
  Communication speed, First bit, CRC calculation state, CRC Length */

  /* SPIx NSS Software Management Configuration */
  if ((hspi->Init.NSS == SPI_NSS_SOFT) && (((hspi->Init.Mode == SPI_MODE_MASTER) &&  \
                                            (hspi->Init.NSSPolarity == SPI_NSS_POLARITY_LOW)) || \
                                           ((hspi->Init.Mode == SPI_MODE_SLAVE) && \
                                            (hspi->Init.NSSPolarity == SPI_NSS_POLARITY_HIGH))))
  {
    SET_BIT(hspi->Instance->CR1, SPI_CR1_SSI);
  }

  /* SPIx Master Rx Auto Suspend Configuration */
  if (((hspi->Init.Mode & SPI_MODE_MASTER) == SPI_MODE_MASTER) && (hspi->Init.DataSize >= SPI_DATASIZE_8BIT))
  {
    MODIFY_REG(hspi->Instance->CR1, SPI_CR1_MASRX, hspi->Init.MasterReceiverAutoSusp);
  }
  else
  {
    CLEAR_BIT(hspi->Instance->CR1, SPI_CR1_MASRX);
  }

  /* SPIx CFG1 Configuration */
  WRITE_REG(hspi->Instance->CFG1, (hspi->Init.BaudRatePrescaler | hspi->Init.CRCCalculation | crc_length |
                                   hspi->Init.FifoThreshold     | hspi->Init.DataSize));

  /* SPIx CFG2 Configuration */
  WRITE_REG(hspi->Instance->CFG2, (hspi->Init.NSSPMode                | hspi->Init.TIMode    |
                                   hspi->Init.NSSPolarity             | hspi->Init.NSS       |
                                   hspi->Init.CLKPolarity             | hspi->Init.CLKPhase  |
                                   hspi->Init.FirstBit                | hspi->Init.Mode      |
                                   hspi->Init.MasterInterDataIdleness | hspi->Init.Direction |
                                   hspi->Init.MasterSSIdleness        | hspi->Init.IOSwap    |
                                   hspi->Init.ReadyMasterManagement   | hspi->Init.ReadyPolarity));

#if (USE_SPI_CRC != 0UL)
  /*---------------------------- SPIx CRCPOLY Configuration ------------------*/
  /* Configure : CRC Polynomial */
  if (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    /* Initialize TXCRC Pattern Initial Value */
    if (hspi->Init.TxCRCInitializationPattern == SPI_CRC_INITIALIZATION_ALL_ONE_PATTERN)
    {
      SET_BIT(hspi->Instance->CR1, SPI_CR1_TCRCINI);
    }
    else
    {
      CLEAR_BIT(hspi->Instance->CR1, SPI_CR1_TCRCINI);
    }

    /* Initialize RXCRC Pattern Initial Value */
    if (hspi->Init.RxCRCInitializationPattern == SPI_CRC_INITIALIZATION_ALL_ONE_PATTERN)
    {
      SET_BIT(hspi->Instance->CR1, SPI_CR1_RCRCINI);
    }
    else
    {
      CLEAR_BIT(hspi->Instance->CR1, SPI_CR1_RCRCINI);
    }

    /* Enable 33/17 bits CRC computation */
    if (((IS_SPI_LIMITED_INSTANCE(hspi->Instance)) && (crc_length == SPI_CRC_LENGTH_16BIT)) ||
        ((IS_SPI_FULL_INSTANCE(hspi->Instance)) && (crc_length == SPI_CRC_LENGTH_32BIT)))
    {
      /* Set SPI_CR1_CRC33_17 bit */
      SET_BIT(hspi->Instance->CR1, SPI_CR1_CRC33_17);
      /* Write CRC polynomial in SPI Register */
      WRITE_REG(hspi->Instance->CRCPOLY, hspi->Init.CRCPolynomial);
    }
    else
    {
      /* Clear SPI_CR1_CRC33_17 bit */
      CLEAR_BIT(hspi->Instance->CR1, SPI_CR1_CRC33_17);

      /* Write CRC polynomial and set MSB bit at 1 in SPI Register */
      /* Set MSB is mandatory for a correct CRC computation        */
      crc_poly_msb_mask = (0x1UL << ((crc_length >> SPI_CFG1_CRCSIZE_Pos) + 0x1U));
      WRITE_REG(hspi->Instance->CRCPOLY, (hspi->Init.CRCPolynomial) | crc_poly_msb_mask);
    }
  }
#endif /* USE_SPI_CRC */

  /* Insure that Underrun configuration is managed only by Salve */
  if (hspi->Init.Mode == SPI_MODE_SLAVE)
  {
#if (USE_SPI_CRC != 0UL)
    MODIFY_REG(hspi->Instance->CFG1, SPI_CFG1_UDRCFG, SPI_CFG1_UDRCFG);
#endif /* USE_SPI_CRC */
  }

#if defined(SPI_I2SCFGR_I2SMOD)
  /* Activate the SPI mode (Make sure that I2SMOD bit in I2SCFGR register is reset) */
  CLEAR_BIT(hspi->Instance->I2SCFGR, SPI_I2SCFGR_I2SMOD);
#endif /* SPI_I2SCFGR_I2SMOD */

  /* Insure that AFCNTR is managed only by Master */
  if ((hspi->Init.Mode & SPI_MODE_MASTER) == SPI_MODE_MASTER)
  {
    /* Alternate function GPIOs control */
    MODIFY_REG(hspi->Instance->CFG2, SPI_CFG2_AFCNTR, (hspi->Init.MasterKeepIOState));
  }

  hspi->ErrorCode = HAL_SPI_ERROR_NONE;
  hspi->State     = HAL_SPI_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  De-Initialize the SPI peripheral.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_DeInit(SPI_HandleTypeDef *hspi)
{
  /* Check the SPI handle allocation */
  if (hspi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check SPI Instance parameter */
  assert_param(IS_SPI_ALL_INSTANCE(hspi->Instance));

  hspi->State = HAL_SPI_STATE_BUSY;

  /* Disable the SPI Peripheral Clock */
  __HAL_SPI_DISABLE(hspi);

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
  if (hspi->MspDeInitCallback == NULL)
  {
    hspi->MspDeInitCallback = HAL_SPI_MspDeInit; /* Legacy weak MspDeInit  */
  }

  /* DeInit the low level hardware: GPIO, CLOCK, NVIC... */
  hspi->MspDeInitCallback(hspi);
#else
  /* DeInit the low level hardware: GPIO, CLOCK, NVIC... */
  HAL_SPI_MspDeInit(hspi);
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */

  hspi->ErrorCode = HAL_SPI_ERROR_NONE;
  hspi->State = HAL_SPI_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hspi);

  return HAL_OK;
}

/**
  * @brief  Initialize the SPI MSP.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__weak void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hspi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_MspInit should be implemented in the user file
   */
}

/**
  * @brief  De-Initialize the SPI MSP.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__weak void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hspi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_MspDeInit should be implemented in the user file
   */
}

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
/**
  * @brief  Register a User SPI Callback
  *         To be used instead of the weak predefined callback
  * @param  hspi Pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for the specified SPI.
  * @param  CallbackID ID of the callback to be registered
  * @param  pCallback pointer to the Callback function
  * @note   The HAL_SPI_RegisterCallback() may be called before HAL_SPI_Init() in HAL_SPI_STATE_RESET
  *         to register callbacks for HAL_SPI_MSPINIT_CB_ID and HAL_SPI_MSPDEINIT_CB_ID
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_RegisterCallback(SPI_HandleTypeDef *hspi, HAL_SPI_CallbackIDTypeDef CallbackID,
                                           pSPI_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hspi->ErrorCode |= HAL_SPI_ERROR_INVALID_CALLBACK;

    return HAL_ERROR;
  }

  if (HAL_SPI_STATE_READY == hspi->State)
  {
    switch (CallbackID)
    {
      case HAL_SPI_TX_COMPLETE_CB_ID :
        hspi->TxCpltCallback = pCallback;
        break;

      case HAL_SPI_RX_COMPLETE_CB_ID :
        hspi->RxCpltCallback = pCallback;
        break;

      case HAL_SPI_TX_RX_COMPLETE_CB_ID :
        hspi->TxRxCpltCallback = pCallback;
        break;

      case HAL_SPI_TX_HALF_COMPLETE_CB_ID :
        hspi->TxHalfCpltCallback = pCallback;
        break;

      case HAL_SPI_RX_HALF_COMPLETE_CB_ID :
        hspi->RxHalfCpltCallback = pCallback;
        break;

      case HAL_SPI_TX_RX_HALF_COMPLETE_CB_ID :
        hspi->TxRxHalfCpltCallback = pCallback;
        break;

      case HAL_SPI_ERROR_CB_ID :
        hspi->ErrorCallback = pCallback;
        break;

      case HAL_SPI_ABORT_CB_ID :
        hspi->AbortCpltCallback = pCallback;
        break;

      case HAL_SPI_SUSPEND_CB_ID :
        hspi->SuspendCallback = pCallback;
        break;

      case HAL_SPI_MSPINIT_CB_ID :
        hspi->MspInitCallback = pCallback;
        break;

      case HAL_SPI_MSPDEINIT_CB_ID :
        hspi->MspDeInitCallback = pCallback;
        break;

      default :
        /* Update the error code */
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_INVALID_CALLBACK);

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (HAL_SPI_STATE_RESET == hspi->State)
  {
    switch (CallbackID)
    {
      case HAL_SPI_MSPINIT_CB_ID :
        hspi->MspInitCallback = pCallback;
        break;

      case HAL_SPI_MSPDEINIT_CB_ID :
        hspi->MspDeInitCallback = pCallback;
        break;

      default :
        /* Update the error code */
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_INVALID_CALLBACK);

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_INVALID_CALLBACK);

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Unregister an SPI Callback
  *         SPI callback is redirected to the weak predefined callback
  * @param  hspi Pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for the specified SPI.
  * @param  CallbackID ID of the callback to be unregistered
  * @note   The HAL_SPI_UnRegisterCallback() may be called before HAL_SPI_Init() in HAL_SPI_STATE_RESET
  *         to un-register callbacks for HAL_SPI_MSPINIT_CB_ID and HAL_SPI_MSPDEINIT_CB_ID
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_UnRegisterCallback(SPI_HandleTypeDef *hspi, HAL_SPI_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (HAL_SPI_STATE_READY == hspi->State)
  {
    switch (CallbackID)
    {
      case HAL_SPI_TX_COMPLETE_CB_ID :
        hspi->TxCpltCallback = HAL_SPI_TxCpltCallback;             /* Legacy weak TxCpltCallback       */
        break;

      case HAL_SPI_RX_COMPLETE_CB_ID :
        hspi->RxCpltCallback = HAL_SPI_RxCpltCallback;             /* Legacy weak RxCpltCallback       */
        break;

      case HAL_SPI_TX_RX_COMPLETE_CB_ID :
        hspi->TxRxCpltCallback = HAL_SPI_TxRxCpltCallback;         /* Legacy weak TxRxCpltCallback     */
        break;

      case HAL_SPI_TX_HALF_COMPLETE_CB_ID :
        hspi->TxHalfCpltCallback = HAL_SPI_TxHalfCpltCallback;     /* Legacy weak TxHalfCpltCallback   */
        break;

      case HAL_SPI_RX_HALF_COMPLETE_CB_ID :
        hspi->RxHalfCpltCallback = HAL_SPI_RxHalfCpltCallback;     /* Legacy weak RxHalfCpltCallback   */
        break;

      case HAL_SPI_TX_RX_HALF_COMPLETE_CB_ID :
        hspi->TxRxHalfCpltCallback = HAL_SPI_TxRxHalfCpltCallback; /* Legacy weak TxRxHalfCpltCallback */
        break;

      case HAL_SPI_ERROR_CB_ID :
        hspi->ErrorCallback = HAL_SPI_ErrorCallback;               /* Legacy weak ErrorCallback        */
        break;

      case HAL_SPI_ABORT_CB_ID :
        hspi->AbortCpltCallback = HAL_SPI_AbortCpltCallback;       /* Legacy weak AbortCpltCallback    */
        break;

      case HAL_SPI_SUSPEND_CB_ID :
        hspi->SuspendCallback = HAL_SPI_SuspendCallback;           /* Legacy weak SuspendCallback      */
        break;

      case HAL_SPI_MSPINIT_CB_ID :
        hspi->MspInitCallback = HAL_SPI_MspInit;                   /* Legacy weak MspInit              */
        break;

      case HAL_SPI_MSPDEINIT_CB_ID :
        hspi->MspDeInitCallback = HAL_SPI_MspDeInit;               /* Legacy weak MspDeInit            */
        break;

      default :
        /* Update the error code */
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_INVALID_CALLBACK);

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (HAL_SPI_STATE_RESET == hspi->State)
  {
    switch (CallbackID)
    {
      case HAL_SPI_MSPINIT_CB_ID :
        hspi->MspInitCallback = HAL_SPI_MspInit;                   /* Legacy weak MspInit              */
        break;

      case HAL_SPI_MSPDEINIT_CB_ID :
        hspi->MspDeInitCallback = HAL_SPI_MspDeInit;               /* Legacy weak MspDeInit            */
        break;

      default :
        /* Update the error code */
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_INVALID_CALLBACK);

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_INVALID_CALLBACK);

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
/**
  * @}
  */

/** @defgroup SPI_Exported_Functions_Group2 IO operation functions
  *  @brief   Data transfers functions
  *
@verbatim
  ==============================================================================
                      ##### IO operation functions #####
 ===============================================================================
 [..]
    This subsection provides a set of functions allowing to manage the SPI
    data transfers.

    [..] The SPI supports master and slave mode :

    (#) There are two modes of transfer:
       (##) Blocking mode: The communication is performed in polling mode.
            The HAL status of all data processing is returned by the same function
            after finishing transfer.
       (##) No-Blocking mode: The communication is performed using Interrupts
            or DMA, These APIs return the HAL status.
            The end of the data processing will be indicated through the
            dedicated SPI IRQ when using Interrupt mode or the DMA IRQ when
            using DMA mode.
            The HAL_SPI_TxCpltCallback(), HAL_SPI_RxCpltCallback() and HAL_SPI_TxRxCpltCallback() user callbacks
            will be executed respectively at the end of the transmit or Receive process
            The HAL_SPI_ErrorCallback()user callback will be executed when a communication error is detected

    (#) APIs provided for these 2 transfer modes (Blocking mode or Non blocking mode using either Interrupt or DMA)
        exist for 1Line (simplex) and 2Lines (full duplex) modes.

@endverbatim
  * @{
  */

/**
  * @brief  Transmit an amount of data in blocking mode.
  * @param  hspi   : pointer to a SPI_HandleTypeDef structure that contains
  *                  the configuration information for SPI module.
  * @param  pData  : pointer to data buffer
  * @param  Size   : amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, const uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
#if defined (__GNUC__)
  __IO uint16_t *ptxdr_16bits = (__IO uint16_t *)(&(hspi->Instance->TXDR));
#endif /* __GNUC__ */

  uint32_t tickstart;

  /* Check Direction parameter */
  assert_param(IS_SPI_DIRECTION_2LINES_OR_1LINE_2LINES_TXONLY(hspi->Init.Direction));

  /* Init tickstart for timeout management*/
  tickstart = HAL_GetTick();

  if (hspi->State != HAL_SPI_STATE_READY)
  {
    return HAL_BUSY;
  }

  if ((pData == NULL) || (Size == 0UL))
  {
    return HAL_ERROR;
  }

  /* Lock the process */
  __HAL_LOCK(hspi);

  /* Set the transaction information */
  hspi->State       = HAL_SPI_STATE_BUSY_TX;
  hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
  hspi->pTxBuffPtr  = (const uint8_t *)pData;
  hspi->TxXferSize  = Size;
  hspi->TxXferCount = Size;

  /*Init field not used in handle to zero */
  hspi->pRxBuffPtr  = NULL;
  hspi->RxXferSize  = (uint16_t) 0UL;
  hspi->RxXferCount = (uint16_t) 0UL;
  hspi->TxISR       = NULL;
  hspi->RxISR       = NULL;

  /* Configure communication direction : 1Line */
  if (hspi->Init.Direction == SPI_DIRECTION_1LINE)
  {
    SPI_1LINE_TX(hspi);
  }
  else
  {
    SPI_2LINES_TX(hspi);
  }

  /* Set the number of data at current transfer */
  MODIFY_REG(hspi->Instance->CR2, SPI_CR2_TSIZE, Size);

  /* Enable SPI peripheral */
  __HAL_SPI_ENABLE(hspi);

  if (hspi->Init.Mode == SPI_MODE_MASTER)
  {
    /* Master transfer start */
    SET_BIT(hspi->Instance->CR1, SPI_CR1_CSTART);
  }

  /* Transmit data in 32 Bit mode */
  if ((hspi->Init.DataSize > SPI_DATASIZE_16BIT) && (IS_SPI_FULL_INSTANCE(hspi->Instance)))
  {
    /* Transmit data in 32 Bit mode */
    while (hspi->TxXferCount > 0UL)
    {
      /* Wait until TXP flag is set to send data */
      if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXP))
      {
        *((__IO uint32_t *)&hspi->Instance->TXDR) = *((const uint32_t *)hspi->pTxBuffPtr);
        hspi->pTxBuffPtr += sizeof(uint32_t);
        hspi->TxXferCount--;
      }
      else
      {
        /* Timeout management */
        if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
        {
          /* Call standard close procedure with error check */
          SPI_CloseTransfer(hspi);

          SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_TIMEOUT);
          hspi->State = HAL_SPI_STATE_READY;

          /* Unlock the process */
          __HAL_UNLOCK(hspi);

          return HAL_TIMEOUT;
        }
      }
    }
  }
  /* Transmit data in 16 Bit mode */
  else if (hspi->Init.DataSize > SPI_DATASIZE_8BIT)
  {
    /* Transmit data in 16 Bit mode */
    while (hspi->TxXferCount > 0UL)
    {
      /* Wait until TXP flag is set to send data */
      if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXP))
      {
        if ((hspi->TxXferCount > 1UL) && (hspi->Init.FifoThreshold > SPI_FIFO_THRESHOLD_01DATA))
        {
          *((__IO uint32_t *)&hspi->Instance->TXDR) = *((const uint32_t *)hspi->pTxBuffPtr);
          hspi->pTxBuffPtr += sizeof(uint32_t);
          hspi->TxXferCount -= (uint16_t)2UL;
        }
        else
        {
#if defined (__GNUC__)
          *ptxdr_16bits = *((const uint16_t *)hspi->pTxBuffPtr);
#else
          *((__IO uint16_t *)&hspi->Instance->TXDR) = *((const uint16_t *)hspi->pTxBuffPtr);
#endif /* __GNUC__ */
          hspi->pTxBuffPtr += sizeof(uint16_t);
          hspi->TxXferCount--;
        }
      }
      else
      {
        /* Timeout management */
        if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
        {
          /* Call standard close procedure with error check */
          SPI_CloseTransfer(hspi);

          SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_TIMEOUT);
          hspi->State = HAL_SPI_STATE_READY;

          /* Unlock the process */
          __HAL_UNLOCK(hspi);

          return HAL_TIMEOUT;
        }
      }
    }
  }
  /* Transmit data in 8 Bit mode */
  else
  {
    while (hspi->TxXferCount > 0UL)
    {
      /* Wait until TXP flag is set to send data */
      if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXP))
      {
        if ((hspi->TxXferCount > 3UL) && (hspi->Init.FifoThreshold > SPI_FIFO_THRESHOLD_03DATA))
        {
          *((__IO uint32_t *)&hspi->Instance->TXDR) = *((const uint32_t *)hspi->pTxBuffPtr);
          hspi->pTxBuffPtr += sizeof(uint32_t);
          hspi->TxXferCount -= (uint16_t)4UL;
        }
        else if ((hspi->TxXferCount > 1UL) && (hspi->Init.FifoThreshold > SPI_FIFO_THRESHOLD_01DATA))
        {
#if defined (__GNUC__)
          *ptxdr_16bits = *((const uint16_t *)hspi->pTxBuffPtr);
#else
          *((__IO uint16_t *)&hspi->Instance->TXDR) = *((const uint16_t *)hspi->pTxBuffPtr);
#endif /* __GNUC__ */
          hspi->pTxBuffPtr += sizeof(uint16_t);
          hspi->TxXferCount -= (uint16_t)2UL;
        }
        else
        {
          *((__IO uint8_t *)&hspi->Instance->TXDR) = *((const uint8_t *)hspi->pTxBuffPtr);
          hspi->pTxBuffPtr += sizeof(uint8_t);
          hspi->TxXferCount--;
        }
      }
      else
      {
        /* Timeout management */
        if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
        {
          /* Call standard close procedure with error check */
          SPI_CloseTransfer(hspi);

          SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_TIMEOUT);
          hspi->State = HAL_SPI_STATE_READY;

          /* Unlock the process */
          __HAL_UNLOCK(hspi);

          return HAL_TIMEOUT;
        }
      }
    }
  }

  /* Wait for Tx (and CRC) data to be sent */
  if (SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_EOT, RESET, Timeout, tickstart) != HAL_OK)
  {
    SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
  }

  /* Call standard close procedure with error check */
  SPI_CloseTransfer(hspi);

  hspi->State = HAL_SPI_STATE_READY;

  /* Unlock the process */
  __HAL_UNLOCK(hspi);

  if (hspi->ErrorCode != HAL_SPI_ERROR_NONE)
  {
    return HAL_ERROR;
  }
  else
  {
    return HAL_OK;
  }
}

/**
  * @brief  Receive an amount of data in blocking mode.
  * @param  hspi   : pointer to a SPI_HandleTypeDef structure that contains
  *                  the configuration information for SPI module.
  * @param  pData  : pointer to data buffer
  * @param  Size   : amount of data to be received
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
  uint32_t tickstart;
  uint32_t temp_sr_reg;
  uint16_t init_max_data_in_fifo;
  init_max_data_in_fifo = (((uint16_t)(hspi->Init.FifoThreshold >> 5U) + 1U));
#if defined (__GNUC__)
  __IO uint16_t *prxdr_16bits = (__IO uint16_t *)(&(hspi->Instance->RXDR));
#endif /* __GNUC__ */

  /* Check Direction parameter */
  assert_param(IS_SPI_DIRECTION_2LINES_OR_1LINE_2LINES_RXONLY(hspi->Init.Direction));

  /* Init tickstart for timeout management*/
  tickstart = HAL_GetTick();

  if (hspi->State != HAL_SPI_STATE_READY)
  {
    return HAL_BUSY;
  }

  if ((pData == NULL) || (Size == 0UL))
  {
    return HAL_ERROR;
  }

  /* Lock the process */
  __HAL_LOCK(hspi);

  /* Set the transaction information */
  hspi->State       = HAL_SPI_STATE_BUSY_RX;
  hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
  hspi->pRxBuffPtr  = (uint8_t *)pData;
  hspi->RxXferSize  = Size;
  hspi->RxXferCount = Size;

  /*Init field not used in handle to zero */
  hspi->pTxBuffPtr  = NULL;
  hspi->TxXferSize  = (uint16_t) 0UL;
  hspi->TxXferCount = (uint16_t) 0UL;
  hspi->RxISR       = NULL;
  hspi->TxISR       = NULL;

  /* Configure communication direction: 1Line */
  if (hspi->Init.Direction == SPI_DIRECTION_1LINE)
  {
    SPI_1LINE_RX(hspi);
  }
  else
  {
    SPI_2LINES_RX(hspi);
  }

  /* Set the number of data at current transfer */
  MODIFY_REG(hspi->Instance->CR2, SPI_CR2_TSIZE, Size);

  /* Enable SPI peripheral */
  __HAL_SPI_ENABLE(hspi);

  if (hspi->Init.Mode == SPI_MODE_MASTER)
  {
    /* Master transfer start */
    SET_BIT(hspi->Instance->CR1, SPI_CR1_CSTART);
  }

  /* Receive data in 32 Bit mode */
  if ((hspi->Init.DataSize > SPI_DATASIZE_16BIT) && (IS_SPI_FULL_INSTANCE(hspi->Instance)))
  {
    /* Transfer loop */
    while (hspi->RxXferCount > 0UL)
    {
      /* Evaluate state of SR register */
      temp_sr_reg = hspi->Instance->SR;

      /* Check the RXP flag */
      if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXP))
      {
        *((uint32_t *)hspi->pRxBuffPtr) = *((__IO uint32_t *)&hspi->Instance->RXDR);
        hspi->pRxBuffPtr += sizeof(uint32_t);
        hspi->RxXferCount--;
      }
      /* Check RXWNE flag if RXP cannot be reached */
      else if ((hspi->RxXferCount < init_max_data_in_fifo) && ((temp_sr_reg & SPI_SR_RXWNE_Msk) != 0UL))
      {
        *((uint32_t *)hspi->pRxBuffPtr) = *((__IO uint32_t *)&hspi->Instance->RXDR);
        hspi->pRxBuffPtr += sizeof(uint32_t);
        hspi->RxXferCount--;
      }
      else
      {
        /* Timeout management */
        if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
        {
          /* Call standard close procedure with error check */
          SPI_CloseTransfer(hspi);

          SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_TIMEOUT);
          hspi->State = HAL_SPI_STATE_READY;

          /* Unlock the process */
          __HAL_UNLOCK(hspi);

          return HAL_TIMEOUT;
        }
      }
    }
  }
  /* Receive data in 16 Bit mode */
  else if (hspi->Init.DataSize > SPI_DATASIZE_8BIT)
  {
    /* Transfer loop */
    while (hspi->RxXferCount > 0UL)
    {
      /* Evaluate state of SR register */
      temp_sr_reg = hspi->Instance->SR;

      /* Check the RXP flag */
      if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXP))
      {
#if defined (__GNUC__)
        *((uint16_t *)hspi->pRxBuffPtr) = *prxdr_16bits;
#else
        *((uint16_t *)hspi->pRxBuffPtr) = *((__IO uint16_t *)&hspi->Instance->RXDR);
#endif /* __GNUC__ */
        hspi->pRxBuffPtr += sizeof(uint16_t);
        hspi->RxXferCount--;
      }
      /* Check RXWNE flag if RXP cannot be reached */
      else if ((hspi->RxXferCount < init_max_data_in_fifo) && ((temp_sr_reg & SPI_SR_RXWNE_Msk) != 0UL))
      {
#if defined (__GNUC__)
        *((uint16_t *)hspi->pRxBuffPtr) = *prxdr_16bits;
#else
        *((uint16_t *)hspi->pRxBuffPtr) = *((__IO uint16_t *)&hspi->Instance->RXDR);
#endif /* __GNUC__ */
        hspi->pRxBuffPtr += sizeof(uint16_t);
#if defined (__GNUC__)
        *((uint16_t *)hspi->pRxBuffPtr) = *prxdr_16bits;
#else
        *((uint16_t *)hspi->pRxBuffPtr) = *((__IO uint16_t *)&hspi->Instance->RXDR);
#endif /* __GNUC__ */
        hspi->pRxBuffPtr += sizeof(uint16_t);
        hspi->RxXferCount -= (uint16_t)2UL;
      }
      /* Check RXPLVL flags when RXWNE cannot be reached */
      else if ((hspi->RxXferCount == 1UL) && ((temp_sr_reg & SPI_SR_RXPLVL_0) != 0UL))
      {
#if defined (__GNUC__)
        *((uint16_t *)hspi->pRxBuffPtr) = *prxdr_16bits;
#else
        *((uint16_t *)hspi->pRxBuffPtr) = *((__IO uint16_t *)&hspi->Instance->RXDR);
#endif /* __GNUC__ */
        hspi->pRxBuffPtr += sizeof(uint16_t);
        hspi->RxXferCount--;
      }
      else
      {
        /* Timeout management */
        if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
        {
          /* Call standard close procedure with error check */
          SPI_CloseTransfer(hspi);

          SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_TIMEOUT);
          hspi->State = HAL_SPI_STATE_READY;

          /* Unlock the process */
          __HAL_UNLOCK(hspi);

          return HAL_TIMEOUT;
        }
      }
    }
  }
  /* Receive data in 8 Bit mode */
  else
  {
    /* Transfer loop */
    while (hspi->RxXferCount > 0UL)
    {
      /* Evaluate state of SR register */
      temp_sr_reg = hspi->Instance->SR;

      /* Check the RXP flag */
      if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXP))
      {
        *((uint8_t *)hspi->pRxBuffPtr) = *((__IO uint8_t *)&hspi->Instance->RXDR);
        hspi->pRxBuffPtr += sizeof(uint8_t);
        hspi->RxXferCount--;
      }
      /* Check RXWNE flag if RXP cannot be reached */
      else if ((hspi->RxXferCount < init_max_data_in_fifo) && ((temp_sr_reg & SPI_SR_RXWNE_Msk) != 0UL))
      {
        *((uint8_t *)hspi->pRxBuffPtr) = *((__IO uint8_t *)&hspi->Instance->RXDR);
        hspi->pRxBuffPtr += sizeof(uint8_t);
        *((uint8_t *)hspi->pRxBuffPtr) = *((__IO uint8_t *)&hspi->Instance->RXDR);
        hspi->pRxBuffPtr += sizeof(uint8_t);
        *((uint8_t *)hspi->pRxBuffPtr) = *((__IO uint8_t *)&hspi->Instance->RXDR);
        hspi->pRxBuffPtr += sizeof(uint8_t);
        *((uint8_t *)hspi->pRxBuffPtr) = *((__IO uint8_t *)&hspi->Instance->RXDR);
        hspi->pRxBuffPtr += sizeof(uint8_t);
        hspi->RxXferCount -= (uint16_t)4UL;
      }
      /* Check RXPLVL flags when RXWNE cannot be reached */
      else if ((hspi->RxXferCount < 4UL) && ((temp_sr_reg & SPI_SR_RXPLVL_Msk) != 0UL))
      {
        *((uint8_t *)hspi->pRxBuffPtr) = *((__IO uint8_t *)&hspi->Instance->RXDR);
        hspi->pRxBuffPtr += sizeof(uint8_t);
        hspi->RxXferCount--;
      }
      else
      {
        /* Timeout management */
        if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
        {
          /* Call standard close procedure with error check */
          SPI_CloseTransfer(hspi);

          SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_TIMEOUT);
          hspi->State = HAL_SPI_STATE_READY;

          /* Unlock the process */
          __HAL_UNLOCK(hspi);

          return HAL_TIMEOUT;
        }
      }
    }
  }

#if (USE_SPI_CRC != 0UL)
  if (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    /* Wait for crc data to be received */
    if (SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_EOT, RESET, Timeout, tickstart) != HAL_OK)
    {
      SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
    }
  }
#endif /* USE_SPI_CRC */

  /* Call standard close procedure with error check */
  SPI_CloseTransfer(hspi);

  hspi->State = HAL_SPI_STATE_READY;

  /* Unlock the process */
  __HAL_UNLOCK(hspi);


  if (hspi->ErrorCode != HAL_SPI_ERROR_NONE)
  {
    return HAL_ERROR;
  }
  else
  {
    return HAL_OK;
  }
}

/**
  * @brief  Transmit and Receive an amount of data in blocking mode.
  * @param  hspi   : pointer to a SPI_HandleTypeDef structure that contains
  *                  the configuration information for SPI module.
  * @param  pTxData: pointer to transmission data buffer
  * @param  pRxData: pointer to reception data buffer
  * @param  Size   : amount of data to be sent and received
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, const uint8_t *pTxData, uint8_t *pRxData,
                                          uint16_t Size, uint32_t Timeout)
{
#if defined (__GNUC__)
  __IO uint16_t *ptxdr_16bits = (__IO uint16_t *)(&(hspi->Instance->TXDR));
  __IO uint16_t *prxdr_16bits = (__IO uint16_t *)(&(hspi->Instance->RXDR));
#endif /* __GNUC__ */

  uint32_t   tickstart;
  uint32_t   fifo_length;
  uint32_t   temp_sr_reg;
  uint16_t   initial_TxXferCount;
  uint16_t   initial_RxXferCount;
  uint16_t   init_max_data_in_fifo;
  init_max_data_in_fifo = (((uint16_t)(hspi->Init.FifoThreshold >> 5U) + 1U));

  /* Check Direction parameter */
  assert_param(IS_SPI_DIRECTION_2LINES(hspi->Init.Direction));

  /* Init tickstart for timeout management*/
  tickstart = HAL_GetTick();

  initial_TxXferCount = Size;
  initial_RxXferCount = Size;

  if (hspi->State != HAL_SPI_STATE_READY)
  {
    return HAL_BUSY;
  }

  if ((pTxData == NULL) || (pRxData == NULL) || (Size == 0UL))
  {
    return HAL_ERROR;
  }

  /* Lock the process */
  __HAL_LOCK(hspi);

  /* Set the transaction information */
  hspi->State       = HAL_SPI_STATE_BUSY_TX_RX;
  hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
  hspi->pRxBuffPtr  = (uint8_t *)pRxData;
  hspi->RxXferCount = Size;
  hspi->RxXferSize  = Size;
  hspi->pTxBuffPtr  = (const uint8_t *)pTxData;
  hspi->TxXferCount = Size;
  hspi->TxXferSize  = Size;

  /*Init field not used in handle to zero */
  hspi->RxISR       = NULL;
  hspi->TxISR       = NULL;

  /* Set Full-Duplex mode */
  SPI_2LINES(hspi);

  /* Initialize FIFO length */
  if (IS_SPI_FULL_INSTANCE(hspi->Instance))
  {
    fifo_length = SPI_HIGHEND_FIFO_SIZE;
  }
  else
  {
    fifo_length = SPI_LOWEND_FIFO_SIZE;
  }

  /* Set the number of data at current transfer */
  MODIFY_REG(hspi->Instance->CR2, SPI_CR2_TSIZE, Size);

  __HAL_SPI_ENABLE(hspi);

  if (hspi->Init.Mode == SPI_MODE_MASTER)
  {
    /* Master transfer start */
    SET_BIT(hspi->Instance->CR1, SPI_CR1_CSTART);
  }

  /* Transmit and Receive data in 32 Bit mode */
  if ((hspi->Init.DataSize > SPI_DATASIZE_16BIT) && (IS_SPI_FULL_INSTANCE(hspi->Instance)))
  {
    /* Adapt fifo length to 32bits data width */
    fifo_length = (fifo_length / 4UL);

    while ((initial_TxXferCount > 0UL) || (initial_RxXferCount > 0UL))
    {
      /* Check TXP flag */
      if ((__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXP)) && (initial_TxXferCount > 0UL) &&
          (initial_RxXferCount  < (initial_TxXferCount + fifo_length)))
      {
        *((__IO uint32_t *)&hspi->Instance->TXDR) = *((const uint32_t *)hspi->pTxBuffPtr);
        hspi->pTxBuffPtr += sizeof(uint32_t);
        hspi->TxXferCount --;
        initial_TxXferCount = hspi->TxXferCount;
      }

      /* Evaluate state of SR register */
      temp_sr_reg = hspi->Instance->SR;

      if (initial_RxXferCount > 0UL)
      {
        /* Check the RXP flag */
        if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXP))
        {
          *((uint32_t *)hspi->pRxBuffPtr) = *((__IO uint32_t *)&hspi->Instance->RXDR);
          hspi->pRxBuffPtr += sizeof(uint32_t);
          hspi->RxXferCount--;
          initial_RxXferCount = hspi->RxXferCount;
        }
        /* Check RXWNE flag if RXP cannot be reached */
        else if ((initial_RxXferCount < init_max_data_in_fifo) && ((temp_sr_reg & SPI_SR_RXWNE_Msk) != 0UL))
        {
          *((uint32_t *)hspi->pRxBuffPtr) = *((__IO uint32_t *)&hspi->Instance->RXDR);
          hspi->pRxBuffPtr += sizeof(uint32_t);
          hspi->RxXferCount--;
          initial_RxXferCount = hspi->RxXferCount;
        }
        else
        {
          /* Timeout management */
          if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
          {
            /* Call standard close procedure with error check */
            SPI_CloseTransfer(hspi);

            SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_TIMEOUT);
            hspi->State = HAL_SPI_STATE_READY;

            /* Unlock the process */
            __HAL_UNLOCK(hspi);

            return HAL_TIMEOUT;
          }
        }
      }
    }
  }
  /* Transmit and Receive data in 16 Bit mode */
  else if (hspi->Init.DataSize > SPI_DATASIZE_8BIT)
  {
    /* Adapt fifo length to 16bits data width */
    fifo_length = (fifo_length / 2UL);

    while ((initial_TxXferCount > 0UL) || (initial_RxXferCount > 0UL))
    {
      /* Check the TXP flag */
      if ((__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXP)) && (initial_TxXferCount > 0UL) &&
          (initial_RxXferCount  < (initial_TxXferCount + fifo_length)))
      {
#if defined (__GNUC__)
        *ptxdr_16bits = *((const uint16_t *)hspi->pTxBuffPtr);
#else
        *((__IO uint16_t *)&hspi->Instance->TXDR) = *((const uint16_t *)hspi->pTxBuffPtr);
#endif /* __GNUC__ */
        hspi->pTxBuffPtr += sizeof(uint16_t);
        hspi->TxXferCount--;
        initial_TxXferCount = hspi->TxXferCount;
      }

      /* Evaluate state of SR register */
      temp_sr_reg = hspi->Instance->SR;

      if (initial_RxXferCount > 0UL)
      {
        /* Check the RXP flag */
        if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXP))
        {
#if defined (__GNUC__)
          *((uint16_t *)hspi->pRxBuffPtr) = *prxdr_16bits;
#else
          *((uint16_t *)hspi->pRxBuffPtr) = *((__IO uint16_t *)&hspi->Instance->RXDR);
#endif /* __GNUC__ */
          hspi->pRxBuffPtr += sizeof(uint16_t);
          hspi->RxXferCount--;
          initial_RxXferCount = hspi->RxXferCount;
        }
        /* Check RXWNE flag if RXP cannot be reached */
        else if ((initial_RxXferCount < init_max_data_in_fifo) && ((temp_sr_reg & SPI_SR_RXWNE_Msk) != 0UL))
        {
#if defined (__GNUC__)
          *((uint16_t *)hspi->pRxBuffPtr) = *prxdr_16bits;
#else
          *((uint16_t *)hspi->pRxBuffPtr) = *((__IO uint16_t *)&hspi->Instance->RXDR);
#endif /* __GNUC__ */
          hspi->pRxBuffPtr += sizeof(uint16_t);
#if defined (__GNUC__)
          *((uint16_t *)hspi->pRxBuffPtr) = *prxdr_16bits;
#else
          *((uint16_t *)hspi->pRxBuffPtr) = *((__IO uint16_t *)&hspi->Instance->RXDR);
#endif /* __GNUC__ */
          hspi->pRxBuffPtr += sizeof(uint16_t);
          hspi->RxXferCount -= (uint16_t)2UL;
          initial_RxXferCount = hspi->RxXferCount;
        }
        /* Check RXPLVL flags when RXWNE cannot be reached */
        else if ((initial_RxXferCount == 1UL) && ((temp_sr_reg & SPI_SR_RXPLVL_0) != 0UL))
        {
#if defined (__GNUC__)
          *((uint16_t *)hspi->pRxBuffPtr) = *prxdr_16bits;
#else
          *((uint16_t *)hspi->pRxBuffPtr) = *((__IO uint16_t *)&hspi->Instance->RXDR);
#endif /* __GNUC__ */
          hspi->pRxBuffPtr += sizeof(uint16_t);
          hspi->RxXferCount--;
          initial_RxXferCount = hspi->RxXferCount;
        }
        else
        {
          /* Timeout management */
          if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
          {
            /* Call standard close procedure with error check */
            SPI_CloseTransfer(hspi);

            SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_TIMEOUT);
            hspi->State = HAL_SPI_STATE_READY;

            /* Unlock the process */
            __HAL_UNLOCK(hspi);

            return HAL_TIMEOUT;
          }
        }
      }
    }
  }
  /* Transmit and Receive data in 8 Bit mode */
  else
  {
    while ((initial_TxXferCount > 0UL) || (initial_RxXferCount > 0UL))
    {
      /* Check the TXP flag */
      if ((__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXP)) && (initial_TxXferCount > 0UL) &&
          (initial_RxXferCount  < (initial_TxXferCount + fifo_length)))
      {
        *((__IO uint8_t *)&hspi->Instance->TXDR) = *((const uint8_t *)hspi->pTxBuffPtr);
        hspi->pTxBuffPtr += sizeof(uint8_t);
        hspi->TxXferCount--;
        initial_TxXferCount = hspi->TxXferCount;
      }

      /* Evaluate state of SR register */
      temp_sr_reg = hspi->Instance->SR;

      if (initial_RxXferCount > 0UL)
      {
        /* Check the RXP flag */
        if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_RXP))
        {
          *((uint8_t *)hspi->pRxBuffPtr) = *((__IO uint8_t *)&hspi->Instance->RXDR);
          hspi->pRxBuffPtr += sizeof(uint8_t);
          hspi->RxXferCount--;
          initial_RxXferCount = hspi->RxXferCount;
        }
        /* Check RXWNE flag if RXP cannot be reached */
        else if ((initial_RxXferCount < init_max_data_in_fifo) && ((temp_sr_reg & SPI_SR_RXWNE_Msk) != 0UL))
        {
          *((uint8_t *)hspi->pRxBuffPtr) = *((__IO uint8_t *)&hspi->Instance->RXDR);
          hspi->pRxBuffPtr += sizeof(uint8_t);
          *((uint8_t *)hspi->pRxBuffPtr) = *((__IO uint8_t *)&hspi->Instance->RXDR);
          hspi->pRxBuffPtr += sizeof(uint8_t);
          *((uint8_t *)hspi->pRxBuffPtr) = *((__IO uint8_t *)&hspi->Instance->RXDR);
          hspi->pRxBuffPtr += sizeof(uint8_t);
          *((uint8_t *)hspi->pRxBuffPtr) = *((__IO uint8_t *)&hspi->Instance->RXDR);
          hspi->pRxBuffPtr += sizeof(uint8_t);
          hspi->RxXferCount -= (uint16_t)4UL;
          initial_RxXferCount = hspi->RxXferCount;
        }
        /* Check RXPLVL flags when RXWNE cannot be reached */
        else if ((initial_RxXferCount < 4UL) && ((temp_sr_reg & SPI_SR_RXPLVL_Msk) != 0UL))
        {
          *((uint8_t *)hspi->pRxBuffPtr) = *((__IO uint8_t *)&hspi->Instance->RXDR);
          hspi->pRxBuffPtr += sizeof(uint8_t);
          hspi->RxXferCount--;
          initial_RxXferCount = hspi->RxXferCount;
        }
        else
        {
          /* Timeout management */
          if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
          {
            /* Call standard close procedure with error check */
            SPI_CloseTransfer(hspi);

            SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_TIMEOUT);
            hspi->State = HAL_SPI_STATE_READY;

            /* Unlock the process */
            __HAL_UNLOCK(hspi);

            return HAL_TIMEOUT;
          }
        }
      }
    }
  }

  /* Wait for Tx/Rx (and CRC) data to be sent/received */
  if (SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_EOT, RESET, Timeout, tickstart) != HAL_OK)
  {
    SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
  }

  /* Call standard close procedure with error check */
  SPI_CloseTransfer(hspi);

  hspi->State = HAL_SPI_STATE_READY;

  /* Unlock the process */
  __HAL_UNLOCK(hspi);

  if (hspi->ErrorCode != HAL_SPI_ERROR_NONE)
  {
    return HAL_ERROR;
  }
  else
  {
    return HAL_OK;
  }
}

/**
  * @brief  Transmit an amount of data in non-blocking mode with Interrupt.
  * @param  hspi : pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @param  pData: pointer to data buffer
  * @param  Size : amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Transmit_IT(SPI_HandleTypeDef *hspi, const uint8_t *pData, uint16_t Size)
{
  /* Check Direction parameter */
  assert_param(IS_SPI_DIRECTION_2LINES_OR_1LINE_2LINES_TXONLY(hspi->Init.Direction));

  if ((pData == NULL) || (Size == 0UL))
  {
    return HAL_ERROR;
  }

  if (hspi->State != HAL_SPI_STATE_READY)
  {
    return HAL_BUSY;
  }

  /* Lock the process */
  __HAL_LOCK(hspi);

  /* Set the transaction information */
  hspi->State       = HAL_SPI_STATE_BUSY_TX;
  hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
  hspi->pTxBuffPtr  = (const uint8_t *)pData;
  hspi->TxXferSize  = Size;
  hspi->TxXferCount = Size;

  /* Init field not used in handle to zero */
  hspi->pRxBuffPtr  = NULL;
  hspi->RxXferSize  = (uint16_t) 0UL;
  hspi->RxXferCount = (uint16_t) 0UL;
  hspi->RxISR       = NULL;

  /* Set the function for IT treatment */
  if ((hspi->Init.DataSize > SPI_DATASIZE_16BIT) && (IS_SPI_FULL_INSTANCE(hspi->Instance)))
  {
    hspi->TxISR = SPI_TxISR_32BIT;
  }
  else if (hspi->Init.DataSize > SPI_DATASIZE_8BIT)
  {
    hspi->TxISR = SPI_TxISR_16BIT;
  }
  else
  {
    hspi->TxISR = SPI_TxISR_8BIT;
  }

  /* Configure communication direction : 1Line */
  if (hspi->Init.Direction == SPI_DIRECTION_1LINE)
  {
    SPI_1LINE_TX(hspi);
  }
  else
  {
    SPI_2LINES_TX(hspi);
  }

  /* Set the number of data at current transfer */
  MODIFY_REG(hspi->Instance->CR2, SPI_CR2_TSIZE, Size);

  /* Enable SPI peripheral */
  __HAL_SPI_ENABLE(hspi);

  /* Unlock the process */
  __HAL_UNLOCK(hspi);

  /* Enable EOT, TXP, FRE, MODF and UDR interrupts */
  __HAL_SPI_ENABLE_IT(hspi, (SPI_IT_EOT | SPI_IT_TXP | SPI_IT_UDR | SPI_IT_FRE | SPI_IT_MODF));

  if (hspi->Init.Mode == SPI_MODE_MASTER)
  {
    /* Master transfer start */
    SET_BIT(hspi->Instance->CR1, SPI_CR1_CSTART);
  }

  return HAL_OK;
}

/**
  * @brief  Receive an amount of data in non-blocking mode with Interrupt.
  * @param  hspi : pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @param  pData: pointer to data buffer
  * @param  Size : amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Receive_IT(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size)
{
  /* Check Direction parameter */
  assert_param(IS_SPI_DIRECTION_2LINES_OR_1LINE_2LINES_RXONLY(hspi->Init.Direction));

  if (hspi->State != HAL_SPI_STATE_READY)
  {
    return HAL_BUSY;
  }

  if ((pData == NULL) || (Size == 0UL))
  {
    return HAL_ERROR;
  }

  /* Lock the process */
  __HAL_LOCK(hspi);

  /* Set the transaction information */
  hspi->State       = HAL_SPI_STATE_BUSY_RX;
  hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
  hspi->pRxBuffPtr  = (uint8_t *)pData;
  hspi->RxXferSize  = Size;
  hspi->RxXferCount = Size;

  /* Init field not used in handle to zero */
  hspi->pTxBuffPtr  = NULL;
  hspi->TxXferSize  = (uint16_t) 0UL;
  hspi->TxXferCount = (uint16_t) 0UL;
  hspi->TxISR       = NULL;

  /* Set the function for IT treatment */
  if ((hspi->Init.DataSize > SPI_DATASIZE_16BIT) && (IS_SPI_FULL_INSTANCE(hspi->Instance)))
  {
    hspi->RxISR = SPI_RxISR_32BIT;
  }
  else if (hspi->Init.DataSize > SPI_DATASIZE_8BIT)
  {
    hspi->RxISR = SPI_RxISR_16BIT;
  }
  else
  {
    hspi->RxISR = SPI_RxISR_8BIT;
  }

  /* Configure communication direction : 1Line */
  if (hspi->Init.Direction == SPI_DIRECTION_1LINE)
  {
    SPI_1LINE_RX(hspi);
  }
  else
  {
    SPI_2LINES_RX(hspi);
  }

  /* Note : The SPI must be enabled after unlocking current process
            to avoid the risk of SPI interrupt handle execution before current
            process unlock */

  /* Set the number of data at current transfer */
  MODIFY_REG(hspi->Instance->CR2, SPI_CR2_TSIZE, Size);

  /* Enable SPI peripheral */
  __HAL_SPI_ENABLE(hspi);

  /* Unlock the process */
  __HAL_UNLOCK(hspi);

  /* Enable EOT, RXP, OVR, FRE and MODF interrupts */
  __HAL_SPI_ENABLE_IT(hspi, (SPI_IT_EOT | SPI_IT_RXP | SPI_IT_OVR | SPI_IT_FRE | SPI_IT_MODF));

  if (hspi->Init.Mode == SPI_MODE_MASTER)
  {
    /* Master transfer start */
    SET_BIT(hspi->Instance->CR1, SPI_CR1_CSTART);
  }

  return HAL_OK;
}

/**
  * @brief  Transmit and Receive an amount of data in non-blocking mode with Interrupt.
  * @param  hspi   : pointer to a SPI_HandleTypeDef structure that contains
  *                  the configuration information for SPI module.
  * @param  pTxData: pointer to transmission data buffer
  * @param  pRxData: pointer to reception data buffer
  * @param  Size   : amount of data to be sent and received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_TransmitReceive_IT(SPI_HandleTypeDef *hspi, const uint8_t *pTxData, uint8_t *pRxData,
                                             uint16_t Size)
{
  uint32_t tmp_TxXferCount;
#if defined (__GNUC__)
  __IO uint16_t *ptxdr_16bits = (__IO uint16_t *)(&(hspi->Instance->TXDR));
#endif /* __GNUC__ */

  /* Check Direction parameter */
  assert_param(IS_SPI_DIRECTION_2LINES(hspi->Init.Direction));

  if (hspi->State != HAL_SPI_STATE_READY)
  {
    return HAL_BUSY;
  }

  if ((pTxData == NULL) || (pRxData == NULL) || (Size == 0UL))
  {
    return HAL_ERROR;
  }

  /* Lock the process */
  __HAL_LOCK(hspi);

  /* Set the transaction information */
  hspi->State       = HAL_SPI_STATE_BUSY_TX_RX;
  hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
  hspi->pTxBuffPtr  = (const uint8_t *)pTxData;
  hspi->TxXferSize  = Size;
  hspi->TxXferCount = Size;
  hspi->pRxBuffPtr  = (uint8_t *)pRxData;
  hspi->RxXferSize  = Size;
  hspi->RxXferCount = Size;
  tmp_TxXferCount   = hspi->TxXferCount;

  /* Set the function for IT treatment */
  if ((hspi->Init.DataSize > SPI_DATASIZE_16BIT) && (IS_SPI_FULL_INSTANCE(hspi->Instance)))
  {
    hspi->TxISR     = SPI_TxISR_32BIT;
    hspi->RxISR     = SPI_RxISR_32BIT;
  }
  else if (hspi->Init.DataSize > SPI_DATASIZE_8BIT)
  {
    hspi->RxISR     = SPI_RxISR_16BIT;
    hspi->TxISR     = SPI_TxISR_16BIT;
  }
  else
  {
    hspi->RxISR     = SPI_RxISR_8BIT;
    hspi->TxISR     = SPI_TxISR_8BIT;
  }

  /* Set Full-Duplex mode */
  SPI_2LINES(hspi);

  /* Set the number of data at current transfer */
  MODIFY_REG(hspi->Instance->CR2, SPI_CR2_TSIZE, Size);

  /* Enable SPI peripheral */
  __HAL_SPI_ENABLE(hspi);

  /* Fill in the TxFIFO */
  while ((__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXP)) && (tmp_TxXferCount != 0UL))
  {
    /* Transmit data in 32 Bit mode */
    if (hspi->Init.DataSize > SPI_DATASIZE_16BIT)
    {
      *((__IO uint32_t *)&hspi->Instance->TXDR) = *((const uint32_t *)hspi->pTxBuffPtr);
      hspi->pTxBuffPtr += sizeof(uint32_t);
      hspi->TxXferCount--;
      tmp_TxXferCount = hspi->TxXferCount;
    }
    /* Transmit data in 16 Bit mode */
    else if (hspi->Init.DataSize > SPI_DATASIZE_8BIT)
    {
#if defined (__GNUC__)
      *ptxdr_16bits = *((const uint16_t *)hspi->pTxBuffPtr);
#else
      *((__IO uint16_t *)&hspi->Instance->TXDR) = *((const uint16_t *)hspi->pTxBuffPtr);
#endif /* __GNUC__ */
      hspi->pTxBuffPtr += sizeof(uint16_t);
      hspi->TxXferCount--;
      tmp_TxXferCount = hspi->TxXferCount;
    }
    /* Transmit data in 8 Bit mode */
    else
    {
      *((__IO uint8_t *)&hspi->Instance->TXDR) = *((const uint8_t *)hspi->pTxBuffPtr);
      hspi->pTxBuffPtr += sizeof(uint8_t);
      hspi->TxXferCount--;
      tmp_TxXferCount = hspi->TxXferCount;
    }
  }

  /* Unlock the process */
  __HAL_UNLOCK(hspi);

  /* Enable EOT, DXP, UDR, OVR, FRE and MODF interrupts */
  __HAL_SPI_ENABLE_IT(hspi, (SPI_IT_EOT | SPI_IT_DXP | SPI_IT_UDR | SPI_IT_OVR | SPI_IT_FRE | SPI_IT_MODF));

  if (hspi->Init.Mode == SPI_MODE_MASTER)
  {
    /* Start Master transfer */
    SET_BIT(hspi->Instance->CR1, SPI_CR1_CSTART);
  }

  return HAL_OK;
}




/**
  * @brief  Transmit an amount of data in non-blocking mode with DMA.
  * @param  hspi : pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @param  pData: pointer to data buffer
  * @param  Size : amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Transmit_DMA(SPI_HandleTypeDef *hspi, const uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef status;

  /* Check Direction parameter */
  assert_param(IS_SPI_DIRECTION_2LINES_OR_1LINE_2LINES_TXONLY(hspi->Init.Direction));

  if (hspi->State != HAL_SPI_STATE_READY)
  {
    return HAL_BUSY;
  }

  if ((pData == NULL) || (Size == 0UL))
  {
    return HAL_ERROR;
  }

  /* Lock the process */
  __HAL_LOCK(hspi);

  /* Set the transaction information */
  hspi->State       = HAL_SPI_STATE_BUSY_TX;
  hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
  hspi->pTxBuffPtr  = (const uint8_t *)pData;
  hspi->TxXferSize  = Size;
  hspi->TxXferCount = Size;

  /* Init field not used in handle to zero */
  hspi->pRxBuffPtr  = NULL;
  hspi->TxISR       = NULL;
  hspi->RxISR       = NULL;
  hspi->RxXferSize  = (uint16_t)0UL;
  hspi->RxXferCount = (uint16_t)0UL;

  /* Configure communication direction : 1Line */
  if (hspi->Init.Direction == SPI_DIRECTION_1LINE)
  {
    SPI_1LINE_TX(hspi);
  }
  else
  {
    SPI_2LINES_TX(hspi);
  }

  /* Packing mode management is enabled by the DMA settings */
  if (((hspi->Init.DataSize > SPI_DATASIZE_16BIT) && (hspi->hdmatx->Init.SrcDataWidth !=  DMA_SRC_DATAWIDTH_WORD) && \
       (IS_SPI_FULL_INSTANCE(hspi->Instance))) || \
      ((hspi->Init.DataSize > SPI_DATASIZE_8BIT)  && (hspi->hdmatx->Init.SrcDataWidth ==  DMA_SRC_DATAWIDTH_BYTE)))
  {
    /* Restriction the DMA data received is not allowed in this mode */
    __HAL_UNLOCK(hspi);
    return HAL_ERROR;
  }

  /* Adjust XferCount according to DMA alignment / Data size */
  if (hspi->Init.DataSize <= SPI_DATASIZE_8BIT)
  {
    if (hspi->hdmatx->Init.SrcDataWidth == DMA_SRC_DATAWIDTH_HALFWORD)
    {
      hspi->TxXferCount = (hspi->TxXferCount + (uint16_t) 1UL) >> 1UL;
    }
    if (hspi->hdmatx->Init.SrcDataWidth == DMA_SRC_DATAWIDTH_WORD)
    {
      hspi->TxXferCount = (hspi->TxXferCount + (uint16_t) 3UL) >> 2UL;
    }
  }
  else if (hspi->Init.DataSize <= SPI_DATASIZE_16BIT)
  {
    if (hspi->hdmatx->Init.SrcDataWidth == DMA_SRC_DATAWIDTH_WORD)
    {
      hspi->TxXferCount = (hspi->TxXferCount + (uint16_t) 1UL) >> 1UL;
    }
  }
  else
  {
    /* Adjustment done */
  }

  /* Set the SPI TxDMA Half transfer complete callback */
  hspi->hdmatx->XferHalfCpltCallback = SPI_DMAHalfTransmitCplt;

  /* Set the SPI TxDMA transfer complete callback */
  hspi->hdmatx->XferCpltCallback = SPI_DMATransmitCplt;

  /* Set the DMA error callback */
  hspi->hdmatx->XferErrorCallback = SPI_DMAError;

  /* Set the DMA AbortCpltCallback */
  hspi->hdmatx->XferAbortCallback = NULL;

  /* Clear TXDMAEN bit*/
  CLEAR_BIT(hspi->Instance->CFG1, SPI_CFG1_TXDMAEN);

  if (hspi->Init.DataSize <= SPI_DATASIZE_8BIT)
  {
    hspi->TxXferCount = Size;
  }
  else if (hspi->Init.DataSize <= SPI_DATASIZE_16BIT)
  {
    hspi->TxXferCount = Size * 2U;
  }
  else
  {
    hspi->TxXferCount = Size * 4U;
  }

  /* Enable the Tx DMA Stream/Channel */
  if ((hspi->hdmatx->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
  {
    if (hspi->hdmatx->LinkedListQueue != NULL)
    {
      /* Set DMA data size */
      hspi->hdmatx->LinkedListQueue->Head->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] = hspi->TxXferCount;

      /* Set DMA source address */
      hspi->hdmatx->LinkedListQueue->Head->LinkRegisters[NODE_CSAR_DEFAULT_OFFSET] = (uint32_t)hspi->pTxBuffPtr;

      /* Set DMA destination address */
      hspi->hdmatx->LinkedListQueue->Head->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET] = (uint32_t)&hspi->Instance->TXDR;

      status = HAL_DMAEx_List_Start_IT(hspi->hdmatx);
    }
    else
    {
      status = HAL_ERROR;
    }
  }
  else
  {
    status = HAL_DMA_Start_IT(hspi->hdmatx, (uint32_t)hspi->pTxBuffPtr, (uint32_t)&hspi->Instance->TXDR,
                              hspi->TxXferCount);
  }

  /* Check status */
  if (status != HAL_OK)
  {
    /* Update SPI error code */
    SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_DMA);
    hspi->State = HAL_SPI_STATE_READY;

    /* Unlock the process */
    __HAL_UNLOCK(hspi);

    return HAL_ERROR;
  }

  /* Set the number of data at current transfer */
  if (hspi->hdmatx->Mode == DMA_LINKEDLIST_CIRCULAR)
  {
    MODIFY_REG(hspi->Instance->CR2, SPI_CR2_TSIZE, 0UL);
  }
  else
  {
    MODIFY_REG(hspi->Instance->CR2, SPI_CR2_TSIZE, Size);
  }

  /* Enable Tx DMA Request */
  SET_BIT(hspi->Instance->CFG1, SPI_CFG1_TXDMAEN);

  /* Enable the SPI Error Interrupt Bit */
  __HAL_SPI_ENABLE_IT(hspi, (SPI_IT_UDR | SPI_IT_FRE | SPI_IT_MODF));

  /* Enable SPI peripheral */
  __HAL_SPI_ENABLE(hspi);

  if (hspi->Init.Mode == SPI_MODE_MASTER)
  {
    /* Master transfer start */
    SET_BIT(hspi->Instance->CR1, SPI_CR1_CSTART);
  }

  /* Unlock the process */
  __HAL_UNLOCK(hspi);

  return HAL_OK;
}

/**
  * @brief  Receive an amount of data in non-blocking mode with DMA.
  * @param  hspi : pointer to a SPI_HandleTypeDef structure that contains
  *                the configuration information for SPI module.
  * @param  pData: pointer to data buffer
  * @param  Size : amount of data to be sent
  * @note   When the CRC feature is enabled the pData Length must be Size + 1.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Receive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef status;

  /* Check Direction parameter */
  assert_param(IS_SPI_DIRECTION_2LINES_OR_1LINE_2LINES_RXONLY(hspi->Init.Direction));


  if (hspi->State != HAL_SPI_STATE_READY)
  {
    __HAL_UNLOCK(hspi);
    return HAL_BUSY;
  }

  if ((pData == NULL) || (Size == 0UL))
  {
    __HAL_UNLOCK(hspi);
    return HAL_ERROR;
  }

  /* Lock the process */
  __HAL_LOCK(hspi);

  /* Set the transaction information */
  hspi->State       = HAL_SPI_STATE_BUSY_RX;
  hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
  hspi->pRxBuffPtr  = (uint8_t *)pData;
  hspi->RxXferSize  = Size;
  hspi->RxXferCount = Size;

  /*Init field not used in handle to zero */
  hspi->RxISR       = NULL;
  hspi->TxISR       = NULL;
  hspi->TxXferSize  = (uint16_t) 0UL;
  hspi->TxXferCount = (uint16_t) 0UL;

  /* Configure communication direction : 1Line */
  if (hspi->Init.Direction == SPI_DIRECTION_1LINE)
  {
    SPI_1LINE_RX(hspi);
  }
  else
  {
    SPI_2LINES_RX(hspi);
  }

  /* Packing mode management is enabled by the DMA settings */
  if (((hspi->Init.DataSize > SPI_DATASIZE_16BIT) && (hspi->hdmarx->Init.DestDataWidth != DMA_DEST_DATAWIDTH_WORD) &&  \
       (IS_SPI_FULL_INSTANCE(hspi->Instance))) || \
      ((hspi->Init.DataSize > SPI_DATASIZE_8BIT)  && (hspi->hdmarx->Init.DestDataWidth == DMA_DEST_DATAWIDTH_BYTE)))
  {
    /* Restriction the DMA data received is not allowed in this mode */
    __HAL_UNLOCK(hspi);
    return HAL_ERROR;
  }

  /* Clear RXDMAEN bit */
  CLEAR_BIT(hspi->Instance->CFG1, SPI_CFG1_RXDMAEN);

  /* Adjust XferCount according to DMA alignment / Data size */
  if (hspi->Init.DataSize <= SPI_DATASIZE_8BIT)
  {
    if (hspi->hdmarx->Init.DestDataWidth == DMA_DEST_DATAWIDTH_HALFWORD)
    {
      hspi->RxXferCount = (hspi->RxXferCount + (uint16_t) 1UL) >> 1UL;
    }
    if (hspi->hdmarx->Init.DestDataWidth == DMA_DEST_DATAWIDTH_WORD)
    {
      hspi->RxXferCount = (hspi->RxXferCount + (uint16_t) 3UL) >> 2UL;
    }
  }
  else if (hspi->Init.DataSize <= SPI_DATASIZE_16BIT)
  {
    if (hspi->hdmarx->Init.DestDataWidth == DMA_DEST_DATAWIDTH_WORD)
    {
      hspi->RxXferCount = (hspi->RxXferCount + (uint16_t) 1UL) >> 1UL;
    }
  }
  else
  {
    /* Adjustment done */
  }

  /* Set the SPI RxDMA Half transfer complete callback */
  hspi->hdmarx->XferHalfCpltCallback = SPI_DMAHalfReceiveCplt;

  /* Set the SPI Rx DMA transfer complete callback */
  hspi->hdmarx->XferCpltCallback = SPI_DMAReceiveCplt;

  /* Set the DMA error callback */
  hspi->hdmarx->XferErrorCallback = SPI_DMAError;

  /* Set the DMA AbortCpltCallback */
  hspi->hdmarx->XferAbortCallback = NULL;

  if (hspi->Init.DataSize <= SPI_DATASIZE_8BIT)
  {
    hspi->RxXferCount = Size;
  }
  else if (hspi->Init.DataSize <= SPI_DATASIZE_16BIT)
  {
    hspi->RxXferCount = Size * 2U;
  }
  else
  {
    hspi->RxXferCount = Size * 4U;
  }

  /* Enable the Rx DMA Stream/Channel  */
  if ((hspi->hdmarx->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
  {
    if (hspi->hdmarx->LinkedListQueue != NULL)
    {
      /* Set DMA data size */
      hspi->hdmarx->LinkedListQueue->Head->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] = hspi->RxXferCount;

      /* Set DMA source address */
      hspi->hdmarx->LinkedListQueue->Head->LinkRegisters[NODE_CSAR_DEFAULT_OFFSET] = (uint32_t)&hspi->Instance->RXDR;

      /* Set DMA destination address */
      hspi->hdmarx->LinkedListQueue->Head->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET] = (uint32_t)hspi->pRxBuffPtr;

      status = HAL_DMAEx_List_Start_IT(hspi->hdmarx);
    }
    else
    {
      status = HAL_ERROR;
    }
  }
  else
  {
    status = HAL_DMA_Start_IT(hspi->hdmarx, (uint32_t)&hspi->Instance->RXDR, (uint32_t)hspi->pRxBuffPtr,
                              hspi->RxXferCount);
  }

  /* Check status */
  if (status != HAL_OK)
  {
    /* Update SPI error code */
    SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_DMA);
    hspi->State = HAL_SPI_STATE_READY;

    /* Unlock the process */
    __HAL_UNLOCK(hspi);

    return HAL_ERROR;
  }

  /* Set the number of data at current transfer */
  if (hspi->hdmarx->Mode == DMA_LINKEDLIST_CIRCULAR)
  {
    MODIFY_REG(hspi->Instance->CR2, SPI_CR2_TSIZE, 0UL);
  }
  else
  {
    MODIFY_REG(hspi->Instance->CR2, SPI_CR2_TSIZE, Size);
  }

  /* Enable Rx DMA Request */
  SET_BIT(hspi->Instance->CFG1, SPI_CFG1_RXDMAEN);

  /* Enable the SPI Error Interrupt Bit */
  __HAL_SPI_ENABLE_IT(hspi, (SPI_IT_OVR | SPI_IT_FRE | SPI_IT_MODF));

  /* Enable SPI peripheral */
  __HAL_SPI_ENABLE(hspi);

  if (hspi->Init.Mode == SPI_MODE_MASTER)
  {
    /* Master transfer start */
    SET_BIT(hspi->Instance->CR1, SPI_CR1_CSTART);
  }

  /* Unlock the process */
  __HAL_UNLOCK(hspi);

  return HAL_OK;
}

/**
  * @brief  Transmit and Receive an amount of data in non-blocking mode with DMA.
  * @param  hspi   : pointer to a SPI_HandleTypeDef structure that contains
  *                  the configuration information for SPI module.
  * @param  pTxData: pointer to transmission data buffer
  * @param  pRxData: pointer to reception data buffer
  * @param  Size   : amount of data to be sent
  * @note   When the CRC feature is enabled the pRxData Length must be Size + 1
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_TransmitReceive_DMA(SPI_HandleTypeDef *hspi, const uint8_t *pTxData, uint8_t *pRxData,
                                              uint16_t Size)
{
  HAL_StatusTypeDef status;

  /* Check Direction parameter */
  assert_param(IS_SPI_DIRECTION_2LINES(hspi->Init.Direction));

  if (hspi->State != HAL_SPI_STATE_READY)
  {
    return HAL_BUSY;
  }

  if ((pTxData == NULL) || (pRxData == NULL) || (Size == 0UL))
  {
    return HAL_ERROR;
  }

  /* Lock the process */
  __HAL_LOCK(hspi);

  /* Set the transaction information */
  hspi->State       = HAL_SPI_STATE_BUSY_TX_RX;
  hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
  hspi->pTxBuffPtr  = (const uint8_t *)pTxData;
  hspi->TxXferSize  = Size;
  hspi->TxXferCount = Size;
  hspi->pRxBuffPtr  = (uint8_t *)pRxData;
  hspi->RxXferSize  = Size;
  hspi->RxXferCount = Size;

  /* Init field not used in handle to zero */
  hspi->RxISR       = NULL;
  hspi->TxISR       = NULL;

  /* Set Full-Duplex mode */
  SPI_2LINES(hspi);

  /* Reset the Tx/Rx DMA bits */
  CLEAR_BIT(hspi->Instance->CFG1, SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN);

  /* Packing mode management is enabled by the DMA settings */
  if (((hspi->Init.DataSize > SPI_DATASIZE_16BIT) && (hspi->hdmarx->Init.DestDataWidth != DMA_DEST_DATAWIDTH_WORD) && \
       (IS_SPI_FULL_INSTANCE(hspi->Instance))) || \
      ((hspi->Init.DataSize > SPI_DATASIZE_8BIT)  && (hspi->hdmarx->Init.DestDataWidth == DMA_DEST_DATAWIDTH_BYTE)))
  {
    /* Restriction the DMA data received is not allowed in this mode */
    /* Unlock the process */
    __HAL_UNLOCK(hspi);
    return HAL_ERROR;
  }

  /* Adjust XferCount according to DMA alignment / Data size */
  if (hspi->Init.DataSize <= SPI_DATASIZE_8BIT)
  {
    if (hspi->hdmatx->Init.SrcDataWidth == DMA_SRC_DATAWIDTH_HALFWORD)
    {
      hspi->TxXferCount = (hspi->TxXferCount + (uint16_t) 1UL) >> 1UL;
    }
    if (hspi->hdmatx->Init.SrcDataWidth == DMA_SRC_DATAWIDTH_WORD)
    {
      hspi->TxXferCount = (hspi->TxXferCount + (uint16_t) 3UL) >> 2UL;
    }
    if (hspi->hdmarx->Init.DestDataWidth == DMA_DEST_DATAWIDTH_HALFWORD)
    {
      hspi->RxXferCount = (hspi->RxXferCount + (uint16_t) 1UL) >> 1UL;
    }
    if (hspi->hdmarx->Init.DestDataWidth == DMA_DEST_DATAWIDTH_WORD)
    {
      hspi->RxXferCount = (hspi->RxXferCount + (uint16_t) 3UL) >> 2UL;
    }
  }
  else if (hspi->Init.DataSize <= SPI_DATASIZE_16BIT)
  {
    if (hspi->hdmatx->Init.SrcDataWidth == DMA_SRC_DATAWIDTH_WORD)
    {
      hspi->TxXferCount = (hspi->TxXferCount + (uint16_t) 1UL) >> 1UL;
    }
    if (hspi->hdmarx->Init.DestDataWidth == DMA_DEST_DATAWIDTH_WORD)
    {
      hspi->RxXferCount = (hspi->RxXferCount + (uint16_t) 1UL) >> 1UL;
    }
  }
  else
  {
    /* Adjustment done */
  }

  /* Set the SPI Tx/Rx DMA Half transfer complete callback */
  hspi->hdmarx->XferHalfCpltCallback = SPI_DMAHalfTransmitReceiveCplt;
  hspi->hdmarx->XferCpltCallback     = SPI_DMATransmitReceiveCplt;

  /* Set the DMA error callback */
  hspi->hdmarx->XferErrorCallback = SPI_DMAError;

  /* Set the DMA AbortCallback */
  hspi->hdmarx->XferAbortCallback = NULL;

  if (hspi->Init.DataSize <= SPI_DATASIZE_8BIT)
  {
    hspi->RxXferCount = Size;
  }
  else if (hspi->Init.DataSize <= SPI_DATASIZE_16BIT)
  {
    hspi->RxXferCount = Size * 2U;
  }
  else
  {
    hspi->RxXferCount = Size * 4U;
  }
  /* Enable the Rx DMA Stream/Channel  */
  if ((hspi->hdmarx->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
  {
    if (hspi->hdmarx->LinkedListQueue != NULL)
    {
      /* Set DMA data size */
      hspi->hdmarx->LinkedListQueue->Head->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] = hspi->RxXferCount;

      /* Set DMA source address */
      hspi->hdmarx->LinkedListQueue->Head->LinkRegisters[NODE_CSAR_DEFAULT_OFFSET] = (uint32_t)&hspi->Instance->RXDR;

      /* Set DMA destination address */
      hspi->hdmarx->LinkedListQueue->Head->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET] = (uint32_t)hspi->pRxBuffPtr;

      status = HAL_DMAEx_List_Start_IT(hspi->hdmarx);
    }
    else
    {
      status = HAL_ERROR;
    }
  }
  else
  {
    status = HAL_DMA_Start_IT(hspi->hdmarx, (uint32_t)&hspi->Instance->RXDR, (uint32_t)hspi->pRxBuffPtr,
                              hspi->RxXferCount);
  }

  /* Check status */
  if (status != HAL_OK)
  {
    /* Update SPI error code */
    SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_DMA);
    hspi->State = HAL_SPI_STATE_READY;

    /* Unlock the process */
    __HAL_UNLOCK(hspi);

    return HAL_ERROR;
  }

  /* Enable Rx DMA Request */
  SET_BIT(hspi->Instance->CFG1, SPI_CFG1_RXDMAEN);

  /* Set the SPI Tx DMA transfer complete callback as NULL because the communication closing
  is performed in DMA reception complete callback  */
  hspi->hdmatx->XferHalfCpltCallback = NULL;
  hspi->hdmatx->XferCpltCallback     = NULL;
  hspi->hdmatx->XferAbortCallback    = NULL;

  /* Set the DMA error callback */
  hspi->hdmatx->XferErrorCallback    = SPI_DMAError;

  if (hspi->Init.DataSize <= SPI_DATASIZE_8BIT)
  {
    hspi->TxXferCount = Size;
  }
  else if (hspi->Init.DataSize <= SPI_DATASIZE_16BIT)
  {
    hspi->TxXferCount = Size * 2U;
  }
  else
  {
    hspi->TxXferCount = Size * 4U;
  }

  /* Enable the Tx DMA Stream/Channel  */
  if ((hspi->hdmatx->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
  {
    if (hspi->hdmatx->LinkedListQueue != NULL)
    {
      /* Set DMA data size */
      hspi->hdmatx->LinkedListQueue->Head->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] = hspi->TxXferCount;

      /* Set DMA source address */
      hspi->hdmatx->LinkedListQueue->Head->LinkRegisters[NODE_CSAR_DEFAULT_OFFSET] = (uint32_t)hspi->pTxBuffPtr;

      /* Set DMA destination address */
      hspi->hdmatx->LinkedListQueue->Head->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET] = (uint32_t)&hspi->Instance->TXDR;

      status = HAL_DMAEx_List_Start_IT(hspi->hdmatx);
    }
    else
    {
      status = HAL_ERROR;
    }
  }
  else
  {
    status = HAL_DMA_Start_IT(hspi->hdmatx, (uint32_t)hspi->pTxBuffPtr, (uint32_t)&hspi->Instance->TXDR,
                              hspi->TxXferCount);
  }

  /* Check status */
  if (status != HAL_OK)
  {
    /* Abort Rx DMA Channel already started */
    (void)HAL_DMA_Abort(hspi->hdmarx);

    /* Update SPI error code */
    SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_DMA);
    hspi->State = HAL_SPI_STATE_READY;

    /* Unlock the process */
    __HAL_UNLOCK(hspi);

    return HAL_ERROR;
  }

  if ((hspi->hdmarx->Mode == DMA_LINKEDLIST_CIRCULAR) && (hspi->hdmatx->Mode == DMA_LINKEDLIST_CIRCULAR))
  {
    MODIFY_REG(hspi->Instance->CR2, SPI_CR2_TSIZE, 0UL);
  }
  else
  {
    MODIFY_REG(hspi->Instance->CR2, SPI_CR2_TSIZE, Size);
  }

  /* Enable Tx DMA Request */
  SET_BIT(hspi->Instance->CFG1, SPI_CFG1_TXDMAEN);

  /* Enable the SPI Error Interrupt Bit */
  __HAL_SPI_ENABLE_IT(hspi, (SPI_IT_OVR | SPI_IT_UDR | SPI_IT_FRE | SPI_IT_MODF));

  /* Enable SPI peripheral */
  __HAL_SPI_ENABLE(hspi);

  if (hspi->Init.Mode == SPI_MODE_MASTER)
  {
    /* Master transfer start */
    SET_BIT(hspi->Instance->CR1, SPI_CR1_CSTART);
  }

  /* Unlock the process */
  __HAL_UNLOCK(hspi);

  return HAL_OK;
}

/**
  * @brief  Abort ongoing transfer (blocking mode).
  * @param  hspi SPI handle.
  * @note   This procedure could be used for aborting any ongoing transfer (Tx and Rx),
  *         started in Interrupt or DMA mode.
  * @note   This procedure performs following operations :
  *          + Disable SPI Interrupts (depending of transfer direction)
  *          + Disable the DMA transfer in the peripheral register (if enabled)
  *          + Abort DMA transfer by calling HAL_DMA_Abort (in case of transfer in DMA mode)
  *          + Set handle State to READY.
  * @note   This procedure is executed in blocking mode : when exiting function, Abort is considered as completed.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Abort(SPI_HandleTypeDef *hspi)
{
  HAL_StatusTypeDef errorcode;

  __IO uint32_t count;

  /* Lock the process */
  __HAL_LOCK(hspi);

  /* Set hspi->state to aborting to avoid any interaction */
  hspi->State = HAL_SPI_STATE_ABORT;

  /* Initialized local variable  */
  errorcode = HAL_OK;
  count = SPI_DEFAULT_TIMEOUT * (SystemCoreClock / 24UL / 1000UL);

  /* If master communication on going, make sure current frame is done before closing the connection */
  if (HAL_IS_BIT_SET(hspi->Instance->CR1, SPI_CR1_CSTART))
  {
    /* Disable EOT interrupt */
    __HAL_SPI_DISABLE_IT(hspi, SPI_IT_EOT);
    do
    {
      count--;
      if (count == 0UL)
      {
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_ABORT);
        break;
      }
    } while (HAL_IS_BIT_SET(hspi->Instance->IER, SPI_IT_EOT));

    /* Request a Suspend transfer */
    SET_BIT(hspi->Instance->CR1, SPI_CR1_CSUSP);
    do
    {
      count--;
      if (count == 0UL)
      {
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_ABORT);
        break;
      }
    } while (HAL_IS_BIT_SET(hspi->Instance->CR1, SPI_CR1_CSTART));

    /* Clear SUSP flag */
    __HAL_SPI_CLEAR_SUSPFLAG(hspi);
    do
    {
      count--;
      if (count == 0UL)
      {
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_ABORT);
        break;
      }
    } while (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_SUSP));
  }

  /* Disable the SPI DMA Tx request if enabled */
  if (HAL_IS_BIT_SET(hspi->Instance->CFG1, SPI_CFG1_TXDMAEN))
  {
    if (hspi->hdmatx != NULL)
    {
      /* Abort the SPI DMA Tx Stream/Channel : use blocking DMA Abort API (no callback) */
      hspi->hdmatx->XferAbortCallback = NULL;

      /* Abort DMA Tx Handle linked to SPI Peripheral */
      if (HAL_DMA_Abort(hspi->hdmatx) != HAL_OK)
      {
        if (HAL_DMA_GetError(hspi->hdmatx) == HAL_DMA_ERROR_TIMEOUT)
        {
          hspi->ErrorCode = HAL_SPI_ERROR_ABORT;
        }
      }
    }
  }

  /* Disable the SPI DMA Rx request if enabled */
  if (HAL_IS_BIT_SET(hspi->Instance->CFG1, SPI_CFG1_RXDMAEN))
  {
    if (hspi->hdmarx != NULL)
    {
      /* Abort the SPI DMA Rx Stream/Channel : use blocking DMA Abort API (no callback) */
      hspi->hdmarx->XferAbortCallback = NULL;

      /* Abort DMA Rx Handle linked to SPI Peripheral */
      if (HAL_DMA_Abort(hspi->hdmarx) != HAL_OK)
      {
        if (HAL_DMA_GetError(hspi->hdmarx) == HAL_DMA_ERROR_TIMEOUT)
        {
          hspi->ErrorCode = HAL_SPI_ERROR_ABORT;
        }
      }
    }
  }

  /* Proceed with abort procedure */
  SPI_AbortTransfer(hspi);

  /* Check error during Abort procedure */
  if (HAL_IS_BIT_SET(hspi->ErrorCode, HAL_SPI_ERROR_ABORT))
  {
    /* return HAL_Error in case of error during Abort procedure */
    errorcode = HAL_ERROR;
  }
  else
  {
    /* Reset errorCode */
    hspi->ErrorCode = HAL_SPI_ERROR_NONE;
  }

  /* Restore hspi->state to ready */
  hspi->State = HAL_SPI_STATE_READY;

  /* Unlock the process */
  __HAL_UNLOCK(hspi);

  return errorcode;
}

/**
  * @brief  Abort ongoing transfer (Interrupt mode).
  * @param  hspi SPI handle.
  * @note   This procedure could be used for aborting any ongoing transfer (Tx and Rx),
  *         started in Interrupt or DMA mode.
  * @note   This procedure performs following operations :
  *          + Disable SPI Interrupts (depending of transfer direction)
  *          + Disable the DMA transfer in the peripheral register (if enabled)
  *          + Abort DMA transfer by calling HAL_DMA_Abort_IT (in case of transfer in DMA mode)
  *          + Set handle State to READY
  *          + At abort completion, call user abort complete callback.
  * @note   This procedure is executed in Interrupt mode, meaning that abort procedure could be
  *         considered as completed only when user abort complete callback is executed (not when exiting function).
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Abort_IT(SPI_HandleTypeDef *hspi)
{
  HAL_StatusTypeDef errorcode;
  __IO uint32_t count;
  uint32_t dma_tx_abort_done = 1UL;
  uint32_t dma_rx_abort_done = 1UL;

  /* Set hspi->state to aborting to avoid any interaction */
  hspi->State = HAL_SPI_STATE_ABORT;

  /* Initialized local variable  */
  errorcode = HAL_OK;
  count = SPI_DEFAULT_TIMEOUT * (SystemCoreClock / 24UL / 1000UL);

  /* If master communication on going, make sure current frame is done before closing the connection */
  if (HAL_IS_BIT_SET(hspi->Instance->CR1, SPI_CR1_CSTART))
  {
    /* Disable EOT interrupt */
    __HAL_SPI_DISABLE_IT(hspi, SPI_IT_EOT);
    do
    {
      count--;
      if (count == 0UL)
      {
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_ABORT);
        break;
      }
    } while (HAL_IS_BIT_SET(hspi->Instance->IER, SPI_IT_EOT));

    /* Request a Suspend transfer */
    SET_BIT(hspi->Instance->CR1, SPI_CR1_CSUSP);
    do
    {
      count--;
      if (count == 0UL)
      {
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_ABORT);
        break;
      }
    } while (HAL_IS_BIT_SET(hspi->Instance->CR1, SPI_CR1_CSTART));

    /* Clear SUSP flag */
    __HAL_SPI_CLEAR_SUSPFLAG(hspi);
    do
    {
      count--;
      if (count == 0UL)
      {
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_ABORT);
        break;
      }
    } while (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_SUSP));
  }

  /* If DMA Tx and/or DMA Rx Handles are associated to SPI Handle, DMA Abort complete callbacks should be initialized
     before any call to DMA Abort functions */

  if (hspi->hdmatx != NULL)
  {
    if (HAL_IS_BIT_SET(hspi->Instance->CFG1, SPI_CFG1_TXDMAEN))
    {
      /* Set DMA Abort Complete callback if SPI DMA Tx request if enabled */
      hspi->hdmatx->XferAbortCallback = SPI_DMATxAbortCallback;

      dma_tx_abort_done = 0UL;

      /* Abort DMA Tx Handle linked to SPI Peripheral */
      if (HAL_DMA_Abort_IT(hspi->hdmatx) != HAL_OK)
      {
        if (HAL_DMA_GetError(hspi->hdmatx) == HAL_DMA_ERROR_NO_XFER)
        {
          dma_tx_abort_done = 1UL;
          hspi->hdmatx->XferAbortCallback = NULL;
        }
      }
    }
    else
    {
      hspi->hdmatx->XferAbortCallback = NULL;
    }
  }

  if (hspi->hdmarx != NULL)
  {
    if (HAL_IS_BIT_SET(hspi->Instance->CFG1, SPI_CFG1_RXDMAEN))
    {
      /* Set DMA Abort Complete callback if SPI DMA Rx request if enabled */
      hspi->hdmarx->XferAbortCallback = SPI_DMARxAbortCallback;

      dma_rx_abort_done = 0UL;

      /* Abort DMA Rx Handle linked to SPI Peripheral */
      if (HAL_DMA_Abort_IT(hspi->hdmarx) != HAL_OK)
      {
        if (HAL_DMA_GetError(hspi->hdmarx) == HAL_DMA_ERROR_NO_XFER)
        {
          dma_rx_abort_done = 1UL;
          hspi->hdmarx->XferAbortCallback = NULL;
        }
      }
    }
    else
    {
      hspi->hdmarx->XferAbortCallback = NULL;
    }
  }

  /* If no running DMA transfer, finish cleanup and call callbacks */
  if ((dma_tx_abort_done == 1UL) && (dma_rx_abort_done == 1UL))
  {
    /* Proceed with abort procedure */
    SPI_AbortTransfer(hspi);

    /* Check error during Abort procedure */
    if (HAL_IS_BIT_SET(hspi->ErrorCode, HAL_SPI_ERROR_ABORT))
    {
      /* return HAL_Error in case of error during Abort procedure */
      errorcode = HAL_ERROR;
    }
    else
    {
      /* Reset errorCode */
      hspi->ErrorCode = HAL_SPI_ERROR_NONE;
    }

    /* Restore hspi->state to ready */
    hspi->State = HAL_SPI_STATE_READY;

    /* Call user Abort complete callback */
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
    hspi->AbortCpltCallback(hspi);
#else
    HAL_SPI_AbortCpltCallback(hspi);
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
  }

  return errorcode;
}

/**
  * @brief  Pause the DMA Transfer.
  *         This API is not supported, it is maintained for backward compatibility.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for the specified SPI module.
  * @retval HAL_ERROR
  */
HAL_StatusTypeDef HAL_SPI_DMAPause(SPI_HandleTypeDef *hspi)
{
  /* Set error code to not supported */
  SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_NOT_SUPPORTED);

  return HAL_ERROR;
}

/**
  * @brief  Resume the DMA Transfer.
  *         This API is not supported, it is maintained for backward compatibility.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for the specified SPI module.
  * @retval HAL_ERROR
  */
HAL_StatusTypeDef HAL_SPI_DMAResume(SPI_HandleTypeDef *hspi)
{
  /* Set error code to not supported */
  SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_NOT_SUPPORTED);

  return HAL_ERROR;
}

/**
  * @brief  Stop the DMA Transfer.
  *         This API is not supported, it is maintained for backward compatibility.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for the specified SPI module.
  * @retval HAL_ERROR
  */
HAL_StatusTypeDef HAL_SPI_DMAStop(SPI_HandleTypeDef *hspi)
{
  /* Set error code to not supported */
  SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_NOT_SUPPORTED);

  return HAL_ERROR;
}

/**
  * @brief  Handle SPI interrupt request.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for the specified SPI module.
  * @retval None
  */
void HAL_SPI_IRQHandler(SPI_HandleTypeDef *hspi)
{
  uint32_t itsource = hspi->Instance->IER;
  uint32_t itflag   = hspi->Instance->SR;
  uint32_t trigger  = itsource & itflag;
  uint32_t cfg1     = hspi->Instance->CFG1;
  uint32_t handled  = 0UL;

  HAL_SPI_StateTypeDef State = hspi->State;
#if defined (__GNUC__)
  __IO uint16_t *prxdr_16bits = (__IO uint16_t *)(&(hspi->Instance->RXDR));
#endif /* __GNUC__ */

  /* SPI in SUSPEND mode  ----------------------------------------------------*/
  if (HAL_IS_BIT_SET(itflag, SPI_FLAG_SUSP) && HAL_IS_BIT_SET(itsource, SPI_FLAG_EOT))
  {
    /* Clear the Suspend flag */
    __HAL_SPI_CLEAR_SUSPFLAG(hspi);

    /* Suspend on going, Call the Suspend callback */
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
    hspi->SuspendCallback(hspi);
#else
    HAL_SPI_SuspendCallback(hspi);
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
    return;
  }

  /* SPI in mode Transmitter and Receiver ------------------------------------*/
  if (HAL_IS_BIT_CLR(trigger, SPI_FLAG_OVR) && HAL_IS_BIT_CLR(trigger, SPI_FLAG_UDR) && \
      HAL_IS_BIT_SET(trigger, SPI_FLAG_DXP))
  {
    hspi->TxISR(hspi);
    hspi->RxISR(hspi);
    handled = 1UL;
  }

  /* SPI in mode Receiver ----------------------------------------------------*/
  if (HAL_IS_BIT_CLR(trigger, SPI_FLAG_OVR) && HAL_IS_BIT_SET(trigger, SPI_FLAG_RXP) && \
      HAL_IS_BIT_CLR(trigger, SPI_FLAG_DXP))
  {
    hspi->RxISR(hspi);
    handled = 1UL;
  }

  /* SPI in mode Transmitter -------------------------------------------------*/
  if (HAL_IS_BIT_CLR(trigger, SPI_FLAG_UDR) && HAL_IS_BIT_SET(trigger, SPI_FLAG_TXP) && \
      HAL_IS_BIT_CLR(trigger, SPI_FLAG_DXP))
  {
    hspi->TxISR(hspi);
    handled = 1UL;
  }

  if (handled != 0UL)
  {
    return;
  }

  /* SPI End Of Transfer: DMA or IT based transfer */
  if (HAL_IS_BIT_SET(trigger, SPI_FLAG_EOT))
  {
    /* Clear EOT/TXTF/SUSP flag */
    __HAL_SPI_CLEAR_EOTFLAG(hspi);
    __HAL_SPI_CLEAR_TXTFFLAG(hspi);
    __HAL_SPI_CLEAR_SUSPFLAG(hspi);

    /* Disable EOT interrupt */
    __HAL_SPI_DISABLE_IT(hspi, SPI_IT_EOT);

    /* For the IT based receive extra polling maybe required for last packet */
    if (HAL_IS_BIT_CLR(hspi->Instance->CFG1, SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN))
    {
      /* Pooling remaining data */
      while (hspi->RxXferCount != 0UL)
      {
        /* Receive data in 32 Bit mode */
        if (hspi->Init.DataSize > SPI_DATASIZE_16BIT)
        {
          *((uint32_t *)hspi->pRxBuffPtr) = *((__IO uint32_t *)&hspi->Instance->RXDR);
          hspi->pRxBuffPtr += sizeof(uint32_t);
        }
        /* Receive data in 16 Bit mode */
        else if (hspi->Init.DataSize > SPI_DATASIZE_8BIT)
        {
#if defined (__GNUC__)
          *((uint16_t *)hspi->pRxBuffPtr) = *prxdr_16bits;
#else
          *((uint16_t *)hspi->pRxBuffPtr) = *((__IO uint16_t *)&hspi->Instance->RXDR);
#endif /* __GNUC__ */
          hspi->pRxBuffPtr += sizeof(uint16_t);
        }
        /* Receive data in 8 Bit mode */
        else
        {
          *((uint8_t *)hspi->pRxBuffPtr) = *((__IO uint8_t *)&hspi->Instance->RXDR);
          hspi->pRxBuffPtr += sizeof(uint8_t);
        }

        hspi->RxXferCount--;
      }
    }

    /* Call SPI Standard close procedure */
    SPI_CloseTransfer(hspi);

    hspi->State = HAL_SPI_STATE_READY;
    if (hspi->ErrorCode != HAL_SPI_ERROR_NONE)
    {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
      hspi->ErrorCallback(hspi);
#else
      HAL_SPI_ErrorCallback(hspi);
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
      return;
    }

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
    /* Call appropriate user callback */
    if (State == HAL_SPI_STATE_BUSY_TX_RX)
    {
      hspi->TxRxCpltCallback(hspi);
    }
    else if (State == HAL_SPI_STATE_BUSY_RX)
    {
      hspi->RxCpltCallback(hspi);
    }
    else if (State == HAL_SPI_STATE_BUSY_TX)
    {
      hspi->TxCpltCallback(hspi);
    }
#else
    /* Call appropriate user callback */
    if (State == HAL_SPI_STATE_BUSY_TX_RX)
    {
      HAL_SPI_TxRxCpltCallback(hspi);
    }
    else if (State == HAL_SPI_STATE_BUSY_RX)
    {
      HAL_SPI_RxCpltCallback(hspi);
    }
    else if (State == HAL_SPI_STATE_BUSY_TX)
    {
      HAL_SPI_TxCpltCallback(hspi);
    }
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
    else
    {
      /* End of the appropriate call */
    }

    return;
  }

  /* SPI in Error Treatment --------------------------------------------------*/
  if ((trigger & (SPI_FLAG_MODF | SPI_FLAG_OVR | SPI_FLAG_FRE | SPI_FLAG_UDR)) != 0UL)
  {
    /* SPI Overrun error interrupt occurred ----------------------------------*/
    if ((trigger & SPI_FLAG_OVR) != 0UL)
    {
      SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_OVR);
      __HAL_SPI_CLEAR_OVRFLAG(hspi);
    }

    /* SPI Mode Fault error interrupt occurred -------------------------------*/
    if ((trigger & SPI_FLAG_MODF) != 0UL)
    {
      SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_MODF);
      __HAL_SPI_CLEAR_MODFFLAG(hspi);
    }

    /* SPI Frame error interrupt occurred ------------------------------------*/
    if ((trigger & SPI_FLAG_FRE) != 0UL)
    {
      SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FRE);
      __HAL_SPI_CLEAR_FREFLAG(hspi);
    }

    /* SPI Underrun error interrupt occurred ------------------------------------*/
    if ((trigger & SPI_FLAG_UDR) != 0UL)
    {
      SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_UDR);
      __HAL_SPI_CLEAR_UDRFLAG(hspi);
    }

    if (hspi->ErrorCode != HAL_SPI_ERROR_NONE)
    {
      /* Disable SPI peripheral */
      __HAL_SPI_DISABLE(hspi);

      /* Disable all interrupts */
      __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_EOT | SPI_IT_RXP | SPI_IT_TXP | SPI_IT_MODF |
                                  SPI_IT_OVR | SPI_IT_FRE | SPI_IT_UDR));

      /* Disable the SPI DMA requests if enabled */
      if (HAL_IS_BIT_SET(cfg1, SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN))
      {
        /* Disable the SPI DMA requests */
        CLEAR_BIT(hspi->Instance->CFG1, SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN);

        /* Abort the SPI DMA Rx channel */
        if (hspi->hdmarx != NULL)
        {
          /* Set the SPI DMA Abort callback :
          will lead to call HAL_SPI_ErrorCallback() at end of DMA abort procedure */
          hspi->hdmarx->XferAbortCallback = SPI_DMAAbortOnError;
          if (HAL_OK != HAL_DMA_Abort_IT(hspi->hdmarx))
          {
            SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_ABORT);
          }
        }
        /* Abort the SPI DMA Tx channel */
        if (hspi->hdmatx != NULL)
        {
          /* Set the SPI DMA Abort callback :
          will lead to call HAL_SPI_ErrorCallback() at end of DMA abort procedure */
          hspi->hdmatx->XferAbortCallback = SPI_DMAAbortOnError;
          if (HAL_OK != HAL_DMA_Abort_IT(hspi->hdmatx))
          {
            SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_ABORT);
          }
        }
      }
      else
      {
        /* Restore hspi->State to Ready */
        hspi->State = HAL_SPI_STATE_READY;

        /* Call user error callback */
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
        hspi->ErrorCallback(hspi);
#else
        HAL_SPI_ErrorCallback(hspi);
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
      }
    }
    return;
  }
}

/**
  * @brief Tx Transfer completed callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__weak void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) /* Derogation MISRAC2012-Rule-8.13 */
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hspi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_TxCpltCallback should be implemented in the user file
   */
}

/**
  * @brief Rx Transfer completed callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__weak void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) /* Derogation MISRAC2012-Rule-8.13 */
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hspi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_RxCpltCallback should be implemented in the user file
   */
}

/**
  * @brief Tx and Rx Transfer completed callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__weak void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) /* Derogation MISRAC2012-Rule-8.13 */
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hspi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_TxRxCpltCallback should be implemented in the user file
   */
}

/**
  * @brief Tx Half Transfer completed callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__weak void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi) /* Derogation MISRAC2012-Rule-8.13 */
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hspi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_TxHalfCpltCallback should be implemented in the user file
   */
}

/**
  * @brief Rx Half Transfer completed callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__weak void HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef *hspi) /* Derogation MISRAC2012-Rule-8.13 */
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hspi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_RxHalfCpltCallback() should be implemented in the user file
   */
}

/**
  * @brief Tx and Rx Half Transfer callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__weak void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi) /* Derogation MISRAC2012-Rule-8.13 */
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hspi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_TxRxHalfCpltCallback() should be implemented in the user file
   */
}

/**
  * @brief SPI error callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__weak void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) /* Derogation MISRAC2012-Rule-8.13 */
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hspi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_ErrorCallback should be implemented in the user file
   */
  /* NOTE : The ErrorCode parameter in the hspi handle is updated by the SPI processes
            and user can use HAL_SPI_GetError() API to check the latest error occurred
   */
}

/**
  * @brief  SPI Abort Complete callback.
  * @param  hspi SPI handle.
  * @retval None
  */
__weak void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef *hspi) /* Derogation MISRAC2012-Rule-8.13 */
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hspi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_AbortCpltCallback can be implemented in the user file.
   */
}

/**
  * @brief  SPI Suspend callback.
  * @param  hspi SPI handle.
  * @retval None
  */
__weak void HAL_SPI_SuspendCallback(SPI_HandleTypeDef *hspi) /* Derogation MISRAC2012-Rule-8.13 */
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hspi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_SuspendCallback can be implemented in the user file.
   */
}

/**
  * @}
  */

/** @defgroup SPI_Exported_Functions_Group3 Peripheral State and Errors functions
  * @brief   SPI control functions
  *
@verbatim
 ===============================================================================
                      ##### Peripheral State and Errors functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to control the SPI.
     (+) HAL_SPI_GetState() API can be helpful to check in run-time the state of the SPI peripheral
     (+) HAL_SPI_GetError() check in run-time Errors occurring during communication
@endverbatim
  * @{
  */

/**
  * @brief  Return the SPI handle state.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval SPI state
  */
HAL_SPI_StateTypeDef HAL_SPI_GetState(const SPI_HandleTypeDef *hspi)
{
  /* Return SPI handle state */
  return hspi->State;
}

/**
  * @brief  Return the SPI error code.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval SPI error code in bitmap format
  */
uint32_t HAL_SPI_GetError(const SPI_HandleTypeDef *hspi)
{
  /* Return SPI ErrorCode */
  return hspi->ErrorCode;
}

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup SPI_Private_Functions
  * @brief   Private functions
  * @{
  */

/**
  * @brief DMA SPI transmit process complete callback.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMATransmitCplt(DMA_HandleTypeDef *hdma)
{
  SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  if (hspi->State != HAL_SPI_STATE_ABORT)
  {
    if (hspi->hdmatx->Mode == DMA_LINKEDLIST_CIRCULAR)
    {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
      hspi->TxCpltCallback(hspi);
#else
      HAL_SPI_TxCpltCallback(hspi);
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
    }
    else
    {
      /* Enable EOT interrupt */
      __HAL_SPI_ENABLE_IT(hspi, SPI_IT_EOT);
    }
  }
}

/**
  * @brief DMA SPI receive process complete callback.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMAReceiveCplt(DMA_HandleTypeDef *hdma)
{
  SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  if (hspi->State != HAL_SPI_STATE_ABORT)
  {
    if (hspi->hdmarx->Mode == DMA_LINKEDLIST_CIRCULAR)
    {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
      hspi->RxCpltCallback(hspi);
#else
      HAL_SPI_RxCpltCallback(hspi);
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
    }
    else
    {
      /* Enable EOT interrupt */
      __HAL_SPI_ENABLE_IT(hspi, SPI_IT_EOT);
    }
  }
}

/**
  * @brief  DMA SPI transmit receive process complete callback.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMATransmitReceiveCplt(DMA_HandleTypeDef *hdma)
{
  SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  if (hspi->State != HAL_SPI_STATE_ABORT)
  {
    if ((hspi->hdmarx->Mode == DMA_LINKEDLIST_CIRCULAR) &&
        (hspi->hdmatx->Mode == DMA_LINKEDLIST_CIRCULAR))
    {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
      hspi->TxRxCpltCallback(hspi);
#else
      HAL_SPI_TxRxCpltCallback(hspi);
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
    }
    else
    {
      /* Enable EOT interrupt */
      __HAL_SPI_ENABLE_IT(hspi, SPI_IT_EOT);
    }
  }
}

/**
  * @brief  DMA SPI half transmit process complete callback.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMAHalfTransmitCplt(DMA_HandleTypeDef *hdma) /* Derogation MISRAC2012-Rule-8.13 */
{
  SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)
                            ((DMA_HandleTypeDef *)hdma)->Parent; /* Derogation MISRAC2012-Rule-8.13 */

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
  hspi->TxHalfCpltCallback(hspi);
#else
  HAL_SPI_TxHalfCpltCallback(hspi);
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
}

/**
  * @brief  DMA SPI half receive process complete callback
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMAHalfReceiveCplt(DMA_HandleTypeDef *hdma) /* Derogation MISRAC2012-Rule-8.13 */
{
  SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)
                            ((DMA_HandleTypeDef *)hdma)->Parent; /* Derogation MISRAC2012-Rule-8.13 */

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
  hspi->RxHalfCpltCallback(hspi);
#else
  HAL_SPI_RxHalfCpltCallback(hspi);
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
}

/**
  * @brief  DMA SPI half transmit receive process complete callback.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMAHalfTransmitReceiveCplt(DMA_HandleTypeDef *hdma) /* Derogation MISRAC2012-Rule-8.13 */
{
  SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)
                            ((DMA_HandleTypeDef *)hdma)->Parent; /* Derogation MISRAC2012-Rule-8.13 */

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
  hspi->TxRxHalfCpltCallback(hspi);
#else
  HAL_SPI_TxRxHalfCpltCallback(hspi);
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
}

/**
  * @brief  DMA SPI communication error callback.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMAError(DMA_HandleTypeDef *hdma)
{
  SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  /* if DMA error is FIFO error ignore it */
  if (HAL_DMA_GetError(hdma) != HAL_DMA_ERROR_NONE)
  {
    /* Call SPI standard close procedure */
    SPI_CloseTransfer(hspi);

    SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_DMA);
    hspi->State = HAL_SPI_STATE_READY;
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
    hspi->ErrorCallback(hspi);
#else
    HAL_SPI_ErrorCallback(hspi);
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
  }
}

/**
  * @brief  DMA SPI communication abort callback, when initiated by HAL services on Error
  *         (To be called at end of DMA Abort procedure following error occurrence).
  * @param  hdma DMA handle.
  * @retval None
  */
static void SPI_DMAAbortOnError(DMA_HandleTypeDef *hdma)
{
  SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
  hspi->RxXferCount = (uint16_t) 0UL;
  hspi->TxXferCount = (uint16_t) 0UL;

  /* Restore hspi->State to Ready */
  hspi->State = HAL_SPI_STATE_READY;

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
  hspi->ErrorCallback(hspi);
#else
  HAL_SPI_ErrorCallback(hspi);
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
}

/**
  * @brief  DMA SPI Tx communication abort callback, when initiated by user
  *         (To be called at end of DMA Tx Abort procedure following user abort request).
  * @note   When this callback is executed, User Abort complete call back is called only if no
  *         Abort still ongoing for Rx DMA Handle.
  * @param  hdma DMA handle.
  * @retval None
  */
static void SPI_DMATxAbortCallback(DMA_HandleTypeDef *hdma)
{
  SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  hspi->hdmatx->XferAbortCallback = NULL;

  /* Check if an Abort process is still ongoing */
  if (hspi->hdmarx != NULL)
  {
    if (hspi->hdmarx->XferAbortCallback != NULL)
    {
      return;
    }
  }

  /* Call the Abort procedure */
  SPI_AbortTransfer(hspi);

  /* Restore hspi->State to Ready */
  hspi->State = HAL_SPI_STATE_READY;

  /* Call user Abort complete callback */
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
  hspi->AbortCpltCallback(hspi);
#else
  HAL_SPI_AbortCpltCallback(hspi);
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
}

/**
  * @brief  DMA SPI Rx communication abort callback, when initiated by user
  *         (To be called at end of DMA Rx Abort procedure following user abort request).
  * @note   When this callback is executed, User Abort complete call back is called only if no
  *         Abort still ongoing for Tx DMA Handle.
  * @param  hdma DMA handle.
  * @retval None
  */
static void SPI_DMARxAbortCallback(DMA_HandleTypeDef *hdma)
{
  SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  hspi->hdmarx->XferAbortCallback = NULL;

  /* Check if an Abort process is still ongoing */
  if (hspi->hdmatx != NULL)
  {
    if (hspi->hdmatx->XferAbortCallback != NULL)
    {
      return;
    }
  }

  /* Call the Abort procedure */
  SPI_AbortTransfer(hspi);

  /* Restore hspi->State to Ready */
  hspi->State = HAL_SPI_STATE_READY;

  /* Call user Abort complete callback */
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
  hspi->AbortCpltCallback(hspi);
#else
  HAL_SPI_AbortCpltCallback(hspi);
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
}

/**
  * @brief  Manage the receive 8-bit in Interrupt context.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_RxISR_8BIT(SPI_HandleTypeDef *hspi)
{
  /* Receive data in 8 Bit mode */
  *((uint8_t *)hspi->pRxBuffPtr) = (*(__IO uint8_t *)&hspi->Instance->RXDR);
  hspi->pRxBuffPtr += sizeof(uint8_t);
  hspi->RxXferCount--;

  /* Disable IT if no more data excepted */
  if (hspi->RxXferCount == 0UL)
  {
    /* Disable RXP interrupts */
    __HAL_SPI_DISABLE_IT(hspi, SPI_IT_RXP);
  }
}


/**
  * @brief  Manage the 16-bit receive in Interrupt context.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_RxISR_16BIT(SPI_HandleTypeDef *hspi)
{
  /* Receive data in 16 Bit mode */
#if defined (__GNUC__)
  __IO uint16_t *prxdr_16bits = (__IO uint16_t *)(&(hspi->Instance->RXDR));

  *((uint16_t *)hspi->pRxBuffPtr) = *prxdr_16bits;
#else
  *((uint16_t *)hspi->pRxBuffPtr) = (*(__IO uint16_t *)&hspi->Instance->RXDR);
#endif /* __GNUC__ */
  hspi->pRxBuffPtr += sizeof(uint16_t);
  hspi->RxXferCount--;

  /* Disable IT if no more data excepted */
  if (hspi->RxXferCount == 0UL)
  {
    /* Disable RXP interrupts */
    __HAL_SPI_DISABLE_IT(hspi, SPI_IT_RXP);
  }
}


/**
  * @brief  Manage the 32-bit receive in Interrupt context.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_RxISR_32BIT(SPI_HandleTypeDef *hspi)
{
  /* Receive data in 32 Bit mode */
  *((uint32_t *)hspi->pRxBuffPtr) = (*(__IO uint32_t *)&hspi->Instance->RXDR);
  hspi->pRxBuffPtr += sizeof(uint32_t);
  hspi->RxXferCount--;

  /* Disable IT if no more data excepted */
  if (hspi->RxXferCount == 0UL)
  {
    /* Disable RXP interrupts */
    __HAL_SPI_DISABLE_IT(hspi, SPI_IT_RXP);
  }
}


/**
  * @brief  Handle the data 8-bit transmit in Interrupt mode.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_TxISR_8BIT(SPI_HandleTypeDef *hspi)
{
  /* Transmit data in 8 Bit mode */
  *(__IO uint8_t *)&hspi->Instance->TXDR = *((const uint8_t *)hspi->pTxBuffPtr);
  hspi->pTxBuffPtr += sizeof(uint8_t);
  hspi->TxXferCount--;

  /* Disable IT if no more data excepted */
  if (hspi->TxXferCount == 0UL)
  {
    /* Disable TXP interrupts */
    __HAL_SPI_DISABLE_IT(hspi, SPI_IT_TXP);
  }
}

/**
  * @brief  Handle the data 16-bit transmit in Interrupt mode.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_TxISR_16BIT(SPI_HandleTypeDef *hspi)
{
  /* Transmit data in 16 Bit mode */
#if defined (__GNUC__)
  __IO uint16_t *ptxdr_16bits = (__IO uint16_t *)(&(hspi->Instance->TXDR));

  *ptxdr_16bits = *((const uint16_t *)hspi->pTxBuffPtr);
#else
  *((__IO uint16_t *)&hspi->Instance->TXDR) = *((const uint16_t *)hspi->pTxBuffPtr);
#endif /* __GNUC__ */
  hspi->pTxBuffPtr += sizeof(uint16_t);
  hspi->TxXferCount--;

  /* Disable IT if no more data excepted */
  if (hspi->TxXferCount == 0UL)
  {
    /* Disable TXP interrupts */
    __HAL_SPI_DISABLE_IT(hspi, SPI_IT_TXP);
  }
}

/**
  * @brief  Handle the data 32-bit transmit in Interrupt mode.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_TxISR_32BIT(SPI_HandleTypeDef *hspi)
{
  /* Transmit data in 32 Bit mode */
  *((__IO uint32_t *)&hspi->Instance->TXDR) = *((const uint32_t *)hspi->pTxBuffPtr);
  hspi->pTxBuffPtr += sizeof(uint32_t);
  hspi->TxXferCount--;

  /* Disable IT if no more data excepted */
  if (hspi->TxXferCount == 0UL)
  {
    /* Disable TXP interrupts */
    __HAL_SPI_DISABLE_IT(hspi, SPI_IT_TXP);
  }
}

/**
  * @brief  Abort Transfer and clear flags.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
static void SPI_AbortTransfer(SPI_HandleTypeDef *hspi)
{
  /* Disable SPI peripheral */
  __HAL_SPI_DISABLE(hspi);

  /* Disable ITs */
  __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_EOT | SPI_IT_TXP | SPI_IT_RXP | SPI_IT_DXP | SPI_IT_UDR | SPI_IT_OVR | \
                              SPI_IT_FRE | SPI_IT_MODF));

  /* Clear the Status flags in the SR register */
  __HAL_SPI_CLEAR_EOTFLAG(hspi);
  __HAL_SPI_CLEAR_TXTFFLAG(hspi);

  /* Disable Tx DMA Request */
  CLEAR_BIT(hspi->Instance->CFG1, SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN);

  /* Clear the Error flags in the SR register */
  __HAL_SPI_CLEAR_OVRFLAG(hspi);
  __HAL_SPI_CLEAR_UDRFLAG(hspi);
  __HAL_SPI_CLEAR_FREFLAG(hspi);
  __HAL_SPI_CLEAR_MODFFLAG(hspi);
  __HAL_SPI_CLEAR_SUSPFLAG(hspi);

#if (USE_SPI_CRC != 0U)
  __HAL_SPI_CLEAR_CRCERRFLAG(hspi);
#endif /* USE_SPI_CRC */

  hspi->TxXferCount = (uint16_t)0UL;
  hspi->RxXferCount = (uint16_t)0UL;
}


/**
  * @brief  Close Transfer and clear flags.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval HAL_ERROR: if any error detected
  *         HAL_OK: if nothing detected
  */
static void SPI_CloseTransfer(SPI_HandleTypeDef *hspi)
{
  uint32_t itflag = hspi->Instance->SR;

  __HAL_SPI_CLEAR_EOTFLAG(hspi);
  __HAL_SPI_CLEAR_TXTFFLAG(hspi);

  /* Disable SPI peripheral */
  __HAL_SPI_DISABLE(hspi);

  /* Disable ITs */
  __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_EOT | SPI_IT_TXP | SPI_IT_RXP | SPI_IT_DXP | SPI_IT_UDR | SPI_IT_OVR | \
                              SPI_IT_FRE | SPI_IT_MODF));

  /* Disable Tx DMA Request */
  CLEAR_BIT(hspi->Instance->CFG1, SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN);

  /* Report UnderRun error for non RX Only communication */
  if (hspi->State != HAL_SPI_STATE_BUSY_RX)
  {
    if ((itflag & SPI_FLAG_UDR) != 0UL)
    {
      SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_UDR);
      __HAL_SPI_CLEAR_UDRFLAG(hspi);
    }
  }

  /* Report OverRun error for non TX Only communication */
  if (hspi->State != HAL_SPI_STATE_BUSY_TX)
  {
    if ((itflag & SPI_FLAG_OVR) != 0UL)
    {
      SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_OVR);
      __HAL_SPI_CLEAR_OVRFLAG(hspi);
    }

#if (USE_SPI_CRC != 0UL)
    /* Check if CRC error occurred */
    if (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
    {
      if ((itflag & SPI_FLAG_CRCERR) != 0UL)
      {
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_CRC);
        __HAL_SPI_CLEAR_CRCERRFLAG(hspi);
      }
    }
#endif /* USE_SPI_CRC */
  }

  /* SPI Mode Fault error interrupt occurred -------------------------------*/
  if ((itflag & SPI_FLAG_MODF) != 0UL)
  {
    SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_MODF);
    __HAL_SPI_CLEAR_MODFFLAG(hspi);
  }

  /* SPI Frame error interrupt occurred ------------------------------------*/
  if ((itflag & SPI_FLAG_FRE) != 0UL)
  {
    SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FRE);
    __HAL_SPI_CLEAR_FREFLAG(hspi);
  }

  hspi->TxXferCount = (uint16_t)0UL;
  hspi->RxXferCount = (uint16_t)0UL;
}

/**
  * @brief Handle SPI Communication Timeout.
  * @param hspi: pointer to a SPI_HandleTypeDef structure that contains
  *              the configuration information for SPI module.
  * @param Flag: SPI flag to check
  * @param Status: flag state to check
  * @param Timeout: Timeout duration
  * @param Tickstart: Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef SPI_WaitOnFlagUntilTimeout(const SPI_HandleTypeDef *hspi, uint32_t Flag, FlagStatus Status,
                                                    uint32_t Timeout, uint32_t Tickstart)
{
  /* Wait until flag is set */
  while ((__HAL_SPI_GET_FLAG(hspi, Flag) ? SET : RESET) == Status)
  {
    /* Check for the Timeout */
    if ((((HAL_GetTick() - Tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
    {
      return HAL_TIMEOUT;
    }
  }
  return HAL_OK;
}

/**
  * @brief  Compute configured packet size from fifo perspective.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval Packet size occupied in the fifo
  */
static uint32_t SPI_GetPacketSize(const SPI_HandleTypeDef *hspi)
{
  uint32_t fifo_threashold = (hspi->Init.FifoThreshold >> SPI_CFG1_FTHLV_Pos) + 1UL;
  uint32_t data_size       = (hspi->Init.DataSize      >> SPI_CFG1_DSIZE_Pos) + 1UL;

  /* Convert data size to Byte */
  data_size = (data_size + 7UL) / 8UL;

  return data_size * fifo_threashold;
}

/**
  * @}
  */

#endif /* HAL_SPI_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */
