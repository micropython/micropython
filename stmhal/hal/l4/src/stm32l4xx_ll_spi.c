/**
  ******************************************************************************
  * @file    stm32l4xx_ll_spi.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   SPI LL module driver.
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
#if defined(USE_FULL_LL_DRIVER)

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_ll_spi.h"
#include "stm32l4xx_ll_bus.h"

#ifdef  USE_FULL_ASSERT
#include "stm32_assert.h"
#else
#define assert_param(expr) ((void)0)
#endif

/** @addtogroup STM32L4xx_LL_Driver
  * @{
  */

#if defined (SPI1) || defined (SPI2) || defined (SPI3)

/** @defgroup SPI_LL SPI
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
/** @defgroup SPI_LL_Private_Constants SPI Private Constants
  * @{
  */
/* SPI registers Masks */
#define SPI_CR1_CLEAR_MASK                 (SPI_CR1_CPHA    | SPI_CR1_CPOL     | SPI_CR1_MSTR   | \
                                            SPI_CR1_BR      | SPI_CR1_LSBFIRST | SPI_CR1_SSI    | \
                                            SPI_CR1_SSM     | SPI_CR1_RXONLY   | SPI_CR1_CRCL   | \
                                            SPI_CR1_CRCNEXT | SPI_CR1_CRCEN    | SPI_CR1_BIDIOE | \
                                            SPI_CR1_BIDIMODE)
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup SPI_LL_Private_Macros SPI Private Macros
  * @{
  */
#define IS_LL_SPI_TRANSFER_DIRECTION(__VALUE__) (((__VALUE__) == LL_SPI_FULL_DUPLEX)    \
                                              || ((__VALUE__) == LL_SPI_SIMPLEX_RX)     \
                                              || ((__VALUE__) == LL_SPI_HALF_DUPLEX_RX) \
                                              || ((__VALUE__) == LL_SPI_HALF_DUPLEX_TX))

#define IS_LL_SPI_MODE(__VALUE__) (((__VALUE__) == LL_SPI_MODE_MASTER) \
                                || ((__VALUE__) == LL_SPI_MODE_SLAVE))

#define IS_LL_SPI_DATAWIDTH(__VALUE__) (((__VALUE__) == LL_SPI_DATAWIDTH_4BIT)  \
                                     || ((__VALUE__) == LL_SPI_DATAWIDTH_5BIT)  \
                                     || ((__VALUE__) == LL_SPI_DATAWIDTH_6BIT)  \
                                     || ((__VALUE__) == LL_SPI_DATAWIDTH_7BIT)  \
                                     || ((__VALUE__) == LL_SPI_DATAWIDTH_8BIT)  \
                                     || ((__VALUE__) == LL_SPI_DATAWIDTH_9BIT)  \
                                     || ((__VALUE__) == LL_SPI_DATAWIDTH_10BIT) \
                                     || ((__VALUE__) == LL_SPI_DATAWIDTH_11BIT) \
                                     || ((__VALUE__) == LL_SPI_DATAWIDTH_12BIT) \
                                     || ((__VALUE__) == LL_SPI_DATAWIDTH_13BIT) \
                                     || ((__VALUE__) == LL_SPI_DATAWIDTH_14BIT) \
                                     || ((__VALUE__) == LL_SPI_DATAWIDTH_15BIT) \
                                     || ((__VALUE__) == LL_SPI_DATAWIDTH_16BIT))

#define IS_LL_SPI_POLARITY(__VALUE__) (((__VALUE__) == LL_SPI_POLARITY_LOW) \
                                    || ((__VALUE__) == LL_SPI_POLARITY_HIGH))

#define IS_LL_SPI_PHASE(__VALUE__) (((__VALUE__) == LL_SPI_PHASE_1EDGE) \
                                 || ((__VALUE__) == LL_SPI_PHASE_2EDGE))

#define IS_LL_SPI_NSS(__VALUE__) (((__VALUE__) == LL_SPI_NSS_SOFT) \
                               || ((__VALUE__) == LL_SPI_NSS_HARD_INPUT) \
                               || ((__VALUE__) == LL_SPI_NSS_HARD_OUTPUT))

#define IS_LL_SPI_BAUDRATE(__VALUE__) (((__VALUE__) == LL_SPI_BAUDRATEPRESCALER_DIV2)   \
                                    || ((__VALUE__) == LL_SPI_BAUDRATEPRESCALER_DIV4)   \
                                    || ((__VALUE__) == LL_SPI_BAUDRATEPRESCALER_DIV8)   \
                                    || ((__VALUE__) == LL_SPI_BAUDRATEPRESCALER_DIV16)  \
                                    || ((__VALUE__) == LL_SPI_BAUDRATEPRESCALER_DIV32)  \
                                    || ((__VALUE__) == LL_SPI_BAUDRATEPRESCALER_DIV64)  \
                                    || ((__VALUE__) == LL_SPI_BAUDRATEPRESCALER_DIV128) \
                                    || ((__VALUE__) == LL_SPI_BAUDRATEPRESCALER_DIV256))

#define IS_LL_SPI_BITORDER(__VALUE__) (((__VALUE__) == LL_SPI_LSB_FIRST) \
                                    || ((__VALUE__) == LL_SPI_MSB_FIRST))

#define IS_LL_SPI_CRCCALCULATION(__VALUE__) (((__VALUE__) == LL_SPI_CRCCALCULATION_ENABLE) \
                                          || ((__VALUE__) == LL_SPI_CRCCALCULATION_DISABLE))

#define IS_LL_SPI_CRC_POLYNOMIAL(__VALUE__) ((__VALUE__) >= 0x1)

/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @addtogroup SPI_LL_Exported_Functions
  * @{
  */

/** @addtogroup SPI_LL_EF_Init
  * @{
  */

/**
  * @brief  De-initialize the SPI registers to their default reset values.
  * @param  SPIx SPI Instance
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: SPI registers are de-initialized
  *          - ERROR: SPI registers are not de-initialized
  */
ErrorStatus LL_SPI_DeInit(SPI_TypeDef *SPIx)
{
  ErrorStatus status = SUCCESS;

  /* Check the parameters */
  assert_param(IS_SPI_ALL_INSTANCE(SPIx));

  if (SPIx == SPI1)
  {
    /* Force reset of SPI clock */
    LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_SPI1);

    /* Release reset of SPI clock */
    LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_SPI1);
  }
  else if (SPIx == SPI2)
  {
    /* Force reset of SPI clock */
    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_SPI2);

    /* Release reset of SPI clock */
    LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_SPI2);

  }
  else if (SPIx == SPI3)
  {
    /* Force reset of SPI clock */
    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_SPI3);

    /* Release reset of SPI clock */
    LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_SPI3);
  }
  else
  {
    status = ERROR;
  }

  return status;
}

/**
  * @brief  Initialize the SPI registers according to the specified parameters in SPI_InitStruct.
  * @note   As some bits in SPI configuration registers can only be written when the SPI is disabled (SPI_CR1_SPE bit =0),
  *         SPI IP should be in disabled state prior calling this function. Otherwise, ERROR result will be returned.
  * @param  SPIx SPI Instance
  * @param  SPI_InitStruct pointer to a @ref LL_SPI_InitTypeDef structure
  * @retval An ErrorStatus enumeration value. (Return always SUCCESS)
  */
ErrorStatus LL_SPI_Init(SPI_TypeDef *SPIx, LL_SPI_InitTypeDef *SPI_InitStruct)
{
  ErrorStatus status = ERROR;

  /* Check the SPI Instance SPIx*/
  assert_param(IS_SPI_ALL_INSTANCE(SPIx));

  /* Check the SPI parameters from SPI_InitStruct*/
  assert_param(IS_LL_SPI_TRANSFER_DIRECTION(SPI_InitStruct->TransferDirection));
  assert_param(IS_LL_SPI_MODE(SPI_InitStruct->Mode));
  assert_param(IS_LL_SPI_DATAWIDTH(SPI_InitStruct->DataWidth));
  assert_param(IS_LL_SPI_POLARITY(SPI_InitStruct->ClockPolarity));
  assert_param(IS_LL_SPI_PHASE(SPI_InitStruct->ClockPhase));
  assert_param(IS_LL_SPI_NSS(SPI_InitStruct->NSS));
  assert_param(IS_LL_SPI_BAUDRATE(SPI_InitStruct->BaudRate));
  assert_param(IS_LL_SPI_BITORDER(SPI_InitStruct->BitOrder));
  assert_param(IS_LL_SPI_CRCCALCULATION(SPI_InitStruct->CRCCalculation));

  if (LL_SPI_IsEnabled(SPIx) == 0x00000000U)
  {
    /*---------------------------- SPIx CR1 Configuration ------------------------
     * Configure SPIx CR1 with parameters:
     * - TransferDirection:  SPI_CR1_BIDIMODE, SPI_CR1_BIDIOE and SPI_CR1_RXONLY bits
     * - Master/Slave Mode:  SPI_CR1_MSTR bit
     * - ClockPolarity:      SPI_CR1_CPOL bit
     * - ClockPhase:         SPI_CR1_CPHA bit
     * - NSS management:     SPI_CR1_SSM bit
     * - BaudRate prescaler: SPI_CR1_BR[2:0] bits
     * - BitOrder:           SPI_CR1_LSBFIRST bit
     * - CRCCalculation:     SPI_CR1_CRCEN bit
     */
    MODIFY_REG(SPIx->CR1,
               SPI_CR1_CLEAR_MASK,
               SPI_InitStruct->TransferDirection | SPI_InitStruct->Mode |
               SPI_InitStruct->ClockPolarity | SPI_InitStruct->ClockPhase |
               SPI_InitStruct->NSS | SPI_InitStruct->BaudRate |
               SPI_InitStruct->BitOrder | SPI_InitStruct->CRCCalculation);

    /*---------------------------- SPIx CR2 Configuration ------------------------
     * Configure SPIx CR2 with parameters:
     * - DataWidth:          DS[3:0] bits
     * - NSS management:     SSOE bit
     */
    MODIFY_REG(SPIx->CR2,
               SPI_CR2_DS | SPI_CR2_SSOE,
               SPI_InitStruct->DataWidth | (SPI_InitStruct->NSS >> 16));

    /*---------------------------- SPIx CRCPR Configuration -----------------------
     * Configure SPIx CRCPR with parameters:
     * - CRCPoly:            CRCPOLY[15:0] bits
     */
    if (SPI_InitStruct->CRCCalculation == LL_SPI_CRCCALCULATION_ENABLE)
    {
      assert_param(IS_LL_SPI_CRC_POLYNOMIAL(SPI_InitStruct->CRCPoly));
      LL_SPI_SetCRCPolynomial(SPIx, SPI_InitStruct->CRCPoly);
    }
    status = SUCCESS;
  }

  return status;
}

/**
  * @brief  Set each @ref LL_SPI_InitTypeDef field to default value.
  * @param  SPI_InitStruct pointer to a @ref LL_SPI_InitTypeDef structure
  * whose fields will be set to default values.
  * @retval None
  */
void LL_SPI_StructInit(LL_SPI_InitTypeDef *SPI_InitStruct)
{
  /* Set SPI_InitStruct fields to default values */
  SPI_InitStruct->TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct->Mode              = LL_SPI_MODE_SLAVE;
  SPI_InitStruct->DataWidth         = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct->ClockPolarity     = LL_SPI_POLARITY_LOW;
  SPI_InitStruct->ClockPhase        = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct->NSS               = LL_SPI_NSS_HARD_INPUT;
  SPI_InitStruct->BaudRate          = LL_SPI_BAUDRATEPRESCALER_DIV2;
  SPI_InitStruct->BitOrder          = LL_SPI_MSB_FIRST;
  SPI_InitStruct->CRCCalculation    = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct->CRCPoly           = 7;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* defined (SPI1) || defined (SPI2) || defined (SPI3) */

/**
  * @}
  */

#endif /* USE_FULL_LL_DRIVER */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
