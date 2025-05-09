/**
  ******************************************************************************
  * @file    stm32n6xx_ll_spi.c
  * @author  MCD Application Team
  * @brief   SPI LL module driver.
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
  */
#if defined(USE_FULL_LL_DRIVER)

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_ll_spi.h"
#include "stm32n6xx_ll_bus.h"
#include "stm32n6xx_ll_rcc.h"
#ifdef  USE_FULL_ASSERT
#include "stm32_assert.h"
#else
#define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */

#if defined(SPI1) || defined(SPI2) || defined(SPI3) || defined(SPI4) || defined(SPI5) || defined(SPI6)

/** @addtogroup SPI_LL
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @addtogroup SPI_LL_Private_Macros
  * @{
  */

#define IS_LL_SPI_MODE(__VALUE__)                   (((__VALUE__) == LL_SPI_MODE_MASTER)         || \
                                                     ((__VALUE__) == LL_SPI_MODE_SLAVE))

#define IS_LL_SPI_SS_IDLENESS(__VALUE__)            (((__VALUE__) == LL_SPI_SS_IDLENESS_00CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_SS_IDLENESS_01CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_SS_IDLENESS_02CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_SS_IDLENESS_03CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_SS_IDLENESS_04CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_SS_IDLENESS_05CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_SS_IDLENESS_06CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_SS_IDLENESS_07CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_SS_IDLENESS_08CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_SS_IDLENESS_09CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_SS_IDLENESS_10CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_SS_IDLENESS_11CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_SS_IDLENESS_12CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_SS_IDLENESS_13CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_SS_IDLENESS_14CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_SS_IDLENESS_15CYCLE))

#define IS_LL_SPI_ID_IDLENESS(__VALUE__)            (((__VALUE__) == LL_SPI_ID_IDLENESS_00CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_ID_IDLENESS_01CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_ID_IDLENESS_02CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_ID_IDLENESS_03CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_ID_IDLENESS_04CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_ID_IDLENESS_05CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_ID_IDLENESS_06CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_ID_IDLENESS_07CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_ID_IDLENESS_08CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_ID_IDLENESS_09CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_ID_IDLENESS_10CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_ID_IDLENESS_11CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_ID_IDLENESS_12CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_ID_IDLENESS_13CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_ID_IDLENESS_14CYCLE) || \
                                                     ((__VALUE__) == LL_SPI_ID_IDLENESS_15CYCLE))

#define IS_LL_SPI_TXCRCINIT_PATTERN(__VALUE__)      (((__VALUE__) == LL_SPI_TXCRCINIT_ALL_ZERO_PATTERN) || \
                                                     ((__VALUE__) == LL_SPI_TXCRCINIT_ALL_ONES_PATTERN))

#define IS_LL_SPI_RXCRCINIT_PATTERN(__VALUE__)      (((__VALUE__) == LL_SPI_RXCRCINIT_ALL_ZERO_PATTERN) || \
                                                     ((__VALUE__) == LL_SPI_RXCRCINIT_ALL_ONES_PATTERN))

#define IS_LL_SPI_UDR_CONFIG_REGISTER(__VALUE__)    (((__VALUE__) == LL_SPI_UDR_CONFIG_REGISTER_PATTERN) || \
                                                     ((__VALUE__) == LL_SPI_UDR_CONFIG_LAST_RECEIVED)    || \
                                                     ((__VALUE__) == LL_SPI_UDR_CONFIG_LAST_TRANSMITTED))

#define IS_LL_SPI_UDR_DETECT_BEGIN_DATA(__VALUE__)  (((__VALUE__) == LL_SPI_UDR_DETECT_BEGIN_DATA_FRAME) || \
                                                     ((__VALUE__) == LL_SPI_UDR_DETECT_END_DATA_FRAME)   || \
                                                     ((__VALUE__) == LL_SPI_UDR_DETECT_BEGIN_ACTIVE_NSS))

#define IS_LL_SPI_PROTOCOL(__VALUE__)               (((__VALUE__) == LL_SPI_PROTOCOL_MOTOROLA)           || \
                                                     ((__VALUE__) == LL_SPI_PROTOCOL_TI))

#define IS_LL_SPI_PHASE(__VALUE__)                  (((__VALUE__) == LL_SPI_PHASE_1EDGE)                 || \
                                                     ((__VALUE__) == LL_SPI_PHASE_2EDGE))

#define IS_LL_SPI_POLARITY(__VALUE__)               (((__VALUE__) == LL_SPI_POLARITY_LOW)                || \
                                                     ((__VALUE__) == LL_SPI_POLARITY_HIGH))

#define IS_LL_SPI_BAUDRATEPRESCALER(__VALUE__)      (((__VALUE__) == LL_SPI_BAUDRATEPRESCALER_BYPASS)    || \
                                                     ((__VALUE__) == LL_SPI_BAUDRATEPRESCALER_DIV2)      || \
                                                     ((__VALUE__) == LL_SPI_BAUDRATEPRESCALER_DIV4)      || \
                                                     ((__VALUE__) == LL_SPI_BAUDRATEPRESCALER_DIV8)      || \
                                                     ((__VALUE__) == LL_SPI_BAUDRATEPRESCALER_DIV16)     || \
                                                     ((__VALUE__) == LL_SPI_BAUDRATEPRESCALER_DIV32)     || \
                                                     ((__VALUE__) == LL_SPI_BAUDRATEPRESCALER_DIV64)     || \
                                                     ((__VALUE__) == LL_SPI_BAUDRATEPRESCALER_DIV128)    || \
                                                     ((__VALUE__) == LL_SPI_BAUDRATEPRESCALER_DIV256))

#define IS_LL_SPI_BITORDER(__VALUE__)               (((__VALUE__) == LL_SPI_LSB_FIRST)                   || \
                                                     ((__VALUE__) == LL_SPI_MSB_FIRST))

#define IS_LL_SPI_TRANSFER_DIRECTION(__VALUE__)     (((__VALUE__) == LL_SPI_FULL_DUPLEX)                 || \
                                                     ((__VALUE__) == LL_SPI_SIMPLEX_TX)                  || \
                                                     ((__VALUE__) == LL_SPI_SIMPLEX_RX)                  || \
                                                     ((__VALUE__) == LL_SPI_HALF_DUPLEX_RX)              || \
                                                     ((__VALUE__) == LL_SPI_HALF_DUPLEX_TX))

#define IS_LL_SPI_DATAWIDTH(__VALUE__)              (((__VALUE__) == LL_SPI_DATAWIDTH_4BIT)              || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_5BIT)              || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_6BIT)              || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_7BIT)              || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_8BIT)              || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_9BIT)              || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_10BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_11BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_12BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_13BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_14BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_15BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_16BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_17BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_18BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_19BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_20BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_21BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_22BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_23BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_24BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_25BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_26BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_27BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_28BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_29BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_30BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_31BIT)             || \
                                                     ((__VALUE__) == LL_SPI_DATAWIDTH_32BIT))

#define IS_LL_SPI_FIFO_TH(__VALUE__)                (((__VALUE__) == LL_SPI_FIFO_TH_01DATA)              || \
                                                     ((__VALUE__) == LL_SPI_FIFO_TH_02DATA)              || \
                                                     ((__VALUE__) == LL_SPI_FIFO_TH_03DATA)              || \
                                                     ((__VALUE__) == LL_SPI_FIFO_TH_04DATA)              || \
                                                     ((__VALUE__) == LL_SPI_FIFO_TH_05DATA)              || \
                                                     ((__VALUE__) == LL_SPI_FIFO_TH_06DATA)              || \
                                                     ((__VALUE__) == LL_SPI_FIFO_TH_07DATA)              || \
                                                     ((__VALUE__) == LL_SPI_FIFO_TH_08DATA)              || \
                                                     ((__VALUE__) == LL_SPI_FIFO_TH_09DATA)              || \
                                                     ((__VALUE__) == LL_SPI_FIFO_TH_10DATA)              || \
                                                     ((__VALUE__) == LL_SPI_FIFO_TH_11DATA)              || \
                                                     ((__VALUE__) == LL_SPI_FIFO_TH_12DATA)              || \
                                                     ((__VALUE__) == LL_SPI_FIFO_TH_13DATA)              || \
                                                     ((__VALUE__) == LL_SPI_FIFO_TH_14DATA)              || \
                                                     ((__VALUE__) == LL_SPI_FIFO_TH_15DATA)              || \
                                                     ((__VALUE__) == LL_SPI_FIFO_TH_16DATA))

#define IS_LL_SPI_CRC(__VALUE__)                    (((__VALUE__) == LL_SPI_CRC_4BIT)                    || \
                                                     ((__VALUE__) == LL_SPI_CRC_5BIT)                    || \
                                                     ((__VALUE__) == LL_SPI_CRC_6BIT)                    || \
                                                     ((__VALUE__) == LL_SPI_CRC_7BIT)                    || \
                                                     ((__VALUE__) == LL_SPI_CRC_8BIT)                    || \
                                                     ((__VALUE__) == LL_SPI_CRC_9BIT)                    || \
                                                     ((__VALUE__) == LL_SPI_CRC_10BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_11BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_12BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_13BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_14BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_15BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_16BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_17BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_18BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_19BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_20BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_21BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_22BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_23BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_24BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_25BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_26BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_27BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_28BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_29BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_30BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_31BIT)                   || \
                                                     ((__VALUE__) == LL_SPI_CRC_32BIT))

#define IS_LL_SPI_NSS(__VALUE__)                    (((__VALUE__) == LL_SPI_NSS_SOFT)                    || \
                                                     ((__VALUE__) == LL_SPI_NSS_HARD_INPUT)              || \
                                                     ((__VALUE__) == LL_SPI_NSS_HARD_OUTPUT))

#define IS_LL_SPI_RX_FIFO(__VALUE__)                (((__VALUE__) == LL_SPI_RX_FIFO_0PACKET)             || \
                                                     ((__VALUE__) == LL_SPI_RX_FIFO_1PACKET)             || \
                                                     ((__VALUE__) == LL_SPI_RX_FIFO_2PACKET)             || \
                                                     ((__VALUE__) == LL_SPI_RX_FIFO_3PACKET))

#define IS_LL_SPI_CRCCALCULATION(__VALUE__)         (((__VALUE__) == LL_SPI_CRCCALCULATION_ENABLE)       || \
                                                     ((__VALUE__) == LL_SPI_CRCCALCULATION_DISABLE))

#define IS_LL_SPI_CRC_POLYNOMIAL(__VALUE__)          ((__VALUE__) >= 0x1UL)

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
ErrorStatus LL_SPI_DeInit(const SPI_TypeDef *SPIx)
{
  ErrorStatus status = ERROR;

  /* Check the parameters */
  assert_param(IS_SPI_ALL_INSTANCE(SPIx));

#if defined(SPI1)
  if (SPIx == SPI1)
  {
    /* Force reset of SPI clock */
    LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_SPI1);

    /* Release reset of SPI clock */
    LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_SPI1);

    /* Update the return status */
    status = SUCCESS;
  }
#endif /* SPI1 */
#if defined(SPI2)
  if (SPIx == SPI2)
  {
    /* Force reset of SPI clock */
    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_SPI2);

    /* Release reset of SPI clock */
    LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_SPI2);

    /* Update the return status */
    status = SUCCESS;
  }
#endif /* SPI2 */
#if defined(SPI3)
  if (SPIx == SPI3)
  {
    /* Force reset of SPI clock */
    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_SPI3);

    /* Release reset of SPI clock */
    LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_SPI3);

    /* Update the return status */
    status = SUCCESS;
  }
#endif /* SPI3 */
#if defined(SPI4)
  if (SPIx == SPI4)
  {
    /* Force reset of SPI clock */
    LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_SPI4);

    /* Release reset of SPI clock */
    LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_SPI4);

    /* Update the return status */
    status = SUCCESS;
  }
#endif /* SPI4 */
#if defined(SPI5)
  if (SPIx == SPI5)
  {
    /* Force reset of SPI clock */
    LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_SPI5);

    /* Release reset of SPI clock */
    LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_SPI5);

    /* Update the return status */
    status = SUCCESS;
  }
#endif /* SPI5 */
#if defined(SPI6)
  if (SPIx == SPI6)
  {
    /* Force reset of SPI clock */
    LL_APB4_GRP1_ForceReset(LL_APB4_GRP1_PERIPH_SPI6);

    /* Release reset of SPI clock */
    LL_APB4_GRP1_ReleaseReset(LL_APB4_GRP1_PERIPH_SPI6);

    /* Update the return status */
    status = SUCCESS;
  }
#endif /* SPI6 */

  return status;
}

/**
  * @brief  Initialize the SPI registers according to the specified parameters in SPI_InitStruct.
  * @note   As some bits in SPI configuration registers can only be written when the SPI is disabled
  *         (SPI_CR1_SPE bit =0), SPI IP should be in disabled state prior calling this function.
  *         Otherwise, ERROR result will be returned.
  * @param  SPIx SPI Instance
  * @param  SPI_InitStruct pointer to a @ref LL_SPI_InitTypeDef structure
  * @retval An ErrorStatus enumeration value. (Return always SUCCESS)
  */
ErrorStatus LL_SPI_Init(SPI_TypeDef *SPIx, LL_SPI_InitTypeDef *SPI_InitStruct)
{
  ErrorStatus status = ERROR;
  uint32_t tmp_nss;
  uint32_t tmp_mode;
  uint32_t tmp_nss_polarity;

  /* Check the SPI Instance SPIx*/
  assert_param(IS_SPI_ALL_INSTANCE(SPIx));

  /* Check the SPI parameters from SPI_InitStruct*/
  assert_param(IS_LL_SPI_TRANSFER_DIRECTION(SPI_InitStruct->TransferDirection));
  assert_param(IS_LL_SPI_MODE(SPI_InitStruct->Mode));
  assert_param(IS_LL_SPI_DATAWIDTH(SPI_InitStruct->DataWidth));
  assert_param(IS_LL_SPI_POLARITY(SPI_InitStruct->ClockPolarity));
  assert_param(IS_LL_SPI_PHASE(SPI_InitStruct->ClockPhase));
  assert_param(IS_LL_SPI_NSS(SPI_InitStruct->NSS));
  assert_param(IS_LL_SPI_BAUDRATEPRESCALER(SPI_InitStruct->BaudRate));
  assert_param(IS_LL_SPI_BITORDER(SPI_InitStruct->BitOrder));
  assert_param(IS_LL_SPI_CRCCALCULATION(SPI_InitStruct->CRCCalculation));

  /* Check the SPI instance is not enabled */
  if (LL_SPI_IsEnabled(SPIx) == 0x00000000UL)
  {
    /*---------------------------- SPIx CFG1 Configuration ------------------------
       * Configure SPIx CFG1 with parameters:
       * - Master Baud Rate       : SPI_CFG1_MBR[2:0] bits & SPI_CFG1_BPASS bit
       * - CRC Computation Enable : SPI_CFG1_CRCEN bit
       * - Length of data frame   : SPI_CFG1_DSIZE[4:0] bits
       */
    MODIFY_REG(SPIx->CFG1, SPI_CFG1_BPASS | SPI_CFG1_MBR | SPI_CFG1_CRCEN | SPI_CFG1_DSIZE,
               SPI_InitStruct->BaudRate  | SPI_InitStruct->CRCCalculation | SPI_InitStruct->DataWidth);

    tmp_nss  = SPI_InitStruct->NSS;
    tmp_mode = SPI_InitStruct->Mode;
    tmp_nss_polarity = LL_SPI_GetNSSPolarity(SPIx);

    /* Checks to setup Internal SS signal level and avoid a MODF Error */
    if ((tmp_nss == LL_SPI_NSS_SOFT) && (((tmp_nss_polarity == LL_SPI_NSS_POLARITY_LOW)  && \
                                          (tmp_mode == LL_SPI_MODE_MASTER))              || \
                                         ((tmp_nss_polarity == LL_SPI_NSS_POLARITY_HIGH) && \
                                          (tmp_mode == LL_SPI_MODE_SLAVE))))
    {
      LL_SPI_SetInternalSSLevel(SPIx, LL_SPI_SS_LEVEL_HIGH);
    }

    /*---------------------------- SPIx CFG2 Configuration ------------------------
       * Configure SPIx CFG2 with parameters:
       * - NSS management         : SPI_CFG2_SSM, SPI_CFG2_SSOE bits
       * - ClockPolarity          : SPI_CFG2_CPOL bit
       * - ClockPhase             : SPI_CFG2_CPHA bit
       * - BitOrder               : SPI_CFG2_LSBFRST bit
       * - Master/Slave Mode      : SPI_CFG2_MASTER bit
       * - SPI Mode               : SPI_CFG2_COMM[1:0] bits
       */
    MODIFY_REG(SPIx->CFG2, SPI_CFG2_SSM   | SPI_CFG2_SSOE    |
               SPI_CFG2_CPOL              | SPI_CFG2_CPHA    |
               SPI_CFG2_LSBFRST           | SPI_CFG2_MASTER  | SPI_CFG2_COMM,
               SPI_InitStruct->NSS        | SPI_InitStruct->ClockPolarity                    |
               SPI_InitStruct->ClockPhase | SPI_InitStruct->BitOrder                         |
               SPI_InitStruct->Mode       | (SPI_InitStruct->TransferDirection & SPI_CFG2_COMM));

    /*---------------------------- SPIx CR1 Configuration ------------------------
       * Configure SPIx CR1 with parameter:
       * - Half Duplex Direction  : SPI_CR1_HDDIR bit
       */
    MODIFY_REG(SPIx->CR1, SPI_CR1_HDDIR, SPI_InitStruct->TransferDirection & SPI_CR1_HDDIR);

    /*---------------------------- SPIx CRCPOLY Configuration ----------------------
       * Configure SPIx CRCPOLY with parameter:
       * - CRCPoly                : CRCPOLY[31:0] bits
       */
    if (SPI_InitStruct->CRCCalculation == LL_SPI_CRCCALCULATION_ENABLE)
    {
      assert_param(IS_LL_SPI_CRC_POLYNOMIAL(SPI_InitStruct->CRCPoly));
      LL_SPI_SetCRCPolynomial(SPIx, SPI_InitStruct->CRCPoly);
    }

    /* Activate the SPI mode (Reset I2SMOD bit in I2SCFGR register) */
    CLEAR_BIT(SPIx->I2SCFGR, SPI_I2SCFGR_I2SMOD);

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
  SPI_InitStruct->CRCPoly           = 7UL;
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
/** @addtogroup I2S_LL
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup I2S_LL_Private_Constants I2S Private Constants
  * @{
  */
/* I2S registers Masks */
#define I2S_I2SCFGR_CLEAR_MASK                       (SPI_I2SCFGR_CHLEN   | SPI_I2SCFGR_DATLEN | \
                                                      SPI_I2SCFGR_DATFMT  | SPI_I2SCFGR_CKPOL  | \
                                                      SPI_I2SCFGR_I2SSTD  | SPI_I2SCFGR_MCKOE  | \
                                                      SPI_I2SCFGR_I2SCFG  | SPI_I2SCFGR_I2SMOD )

/**
  * @}
  */
/* Private macros ------------------------------------------------------------*/
/** @defgroup I2S_LL_Private_Macros I2S Private Macros
  * @{
  */

#define IS_LL_I2S_DATAFORMAT(__VALUE__)            (((__VALUE__) == LL_I2S_DATAFORMAT_16B)              || \
                                                    ((__VALUE__) == LL_I2S_DATAFORMAT_16B_EXTENDED)     || \
                                                    ((__VALUE__) == LL_I2S_DATAFORMAT_24B)              || \
                                                    ((__VALUE__) == LL_I2S_DATAFORMAT_24B_LEFT_ALIGNED) || \
                                                    ((__VALUE__) == LL_I2S_DATAFORMAT_32B))

#define IS_LL_I2S_CHANNEL_LENGTH_TYPE (__VALUE__)  (((__VALUE__) == LL_I2S_SLAVE_VARIABLE_CH_LENGTH)    || \
                                                    ((__VALUE__) == LL_I2S_SLAVE_FIXED_CH_LENGTH))

#define IS_LL_I2S_CKPOL(__VALUE__)                  (((__VALUE__) == LL_I2S_POLARITY_LOW)               || \
                                                     ((__VALUE__) == LL_I2S_POLARITY_HIGH))

#define IS_LL_I2S_STANDARD(__VALUE__)              (((__VALUE__) == LL_I2S_STANDARD_PHILIPS)            || \
                                                    ((__VALUE__) == LL_I2S_STANDARD_MSB)                || \
                                                    ((__VALUE__) == LL_I2S_STANDARD_LSB)                || \
                                                    ((__VALUE__) == LL_I2S_STANDARD_PCM_SHORT)          || \
                                                    ((__VALUE__) == LL_I2S_STANDARD_PCM_LONG))

#define IS_LL_I2S_MODE(__VALUE__)                  (((__VALUE__) == LL_I2S_MODE_SLAVE_TX)               || \
                                                    ((__VALUE__) == LL_I2S_MODE_SLAVE_RX)               || \
                                                    ((__VALUE__) == LL_I2S_MODE_SLAVE_FULL_DUPLEX)      || \
                                                    ((__VALUE__) == LL_I2S_MODE_MASTER_TX)              || \
                                                    ((__VALUE__) == LL_I2S_MODE_MASTER_RX)              || \
                                                    ((__VALUE__) == LL_I2S_MODE_MASTER_FULL_DUPLEX))

#define IS_LL_I2S_MCLK_OUTPUT(__VALUE__)           (((__VALUE__) == LL_I2S_MCLK_OUTPUT_ENABLE)          || \
                                                    ((__VALUE__) == LL_I2S_MCLK_OUTPUT_DISABLE))

#define IS_LL_I2S_AUDIO_FREQ(__VALUE__)           ((((__VALUE__) >= LL_I2S_AUDIOFREQ_8K)                && \
                                                    ((__VALUE__) <= LL_I2S_AUDIOFREQ_192K))             || \
                                                   ((__VALUE__) == LL_I2S_AUDIOFREQ_DEFAULT))

#define IS_LL_I2S_PRESCALER_LINEAR(__VALUE__)       ((__VALUE__) <= 0xFFUL)

#define IS_LL_I2S_PRESCALER_PARITY(__VALUE__)      (((__VALUE__) == LL_I2S_PRESCALER_PARITY_EVEN)       || \
                                                    ((__VALUE__) == LL_I2S_PRESCALER_PARITY_ODD))

#define IS_LL_I2S_FIFO_TH (__VALUE__)              (((__VALUE__) == LL_I2S_LL_I2S_FIFO_TH_01DATA)       || \
                                                    ((__VALUE__) == LL_I2S_LL_I2S_FIFO_TH_02DATA)       || \
                                                    ((__VALUE__) == LL_I2S_LL_I2S_FIFO_TH_03DATA)       || \
                                                    ((__VALUE__) == LL_I2S_LL_I2S_FIFO_TH_04DATA)       || \
                                                    ((__VALUE__) == LL_I2S_LL_I2S_FIFO_TH_05DATA)       || \
                                                    ((__VALUE__) == LL_I2S_LL_I2S_FIFO_TH_06DATA)       || \
                                                    ((__VALUE__) == LL_I2S_LL_I2S_FIFO_TH_07DATA)       || \
                                                    ((__VALUE__) == LL_I2S_LL_I2S_FIFO_TH_08DATA))

#define IS_LL_I2S_BIT_ORDER(__VALUE__)             (((__VALUE__) == LL_I2S_LSB_FIRST)                   || \
                                                    ((__VALUE__) == LL_I2S_MSB_FIRST))
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @addtogroup I2S_LL_Exported_Functions
  * @{
  */

/** @addtogroup I2S_LL_EF_Init
  * @{
  */

/**
  * @brief  De-initialize the SPI/I2S registers to their default reset values.
  * @param  SPIx SPI Instance
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: SPI registers are de-initialized
  *          - ERROR: SPI registers are not de-initialized
  */
ErrorStatus LL_I2S_DeInit(const SPI_TypeDef *SPIx)
{
  return LL_SPI_DeInit(SPIx);
}

/**
  * @brief  Initializes the SPI/I2S registers according to the specified parameters in I2S_InitStruct.
  * @note   As some bits in I2S configuration registers can only be written when the SPI is disabled
  *         (SPI_CR1_SPE bit =0), SPI IP should be in disabled state prior calling this function.
  *         Otherwise, ERROR result will be returned.
  * @note   I2S (SPI) source clock must be ready before calling this function. Otherwise will results
  *         in wrong programming.
  * @param  SPIx SPI Instance
  * @param  I2S_InitStruct pointer to a @ref LL_I2S_InitTypeDef structure
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: SPI registers are Initialized
  *          - ERROR: SPI registers are not Initialized
  */
ErrorStatus LL_I2S_Init(SPI_TypeDef *SPIx, const LL_I2S_InitTypeDef *I2S_InitStruct)
{
  uint32_t i2sdiv = 0UL;
  uint32_t i2sodd = 0UL;
  uint32_t packetlength = 1UL;
  uint32_t ispcm = 0UL;
  uint32_t tmp;
  uint32_t sourceclock = 0UL;

  ErrorStatus status = ERROR;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(sourceclock);

  /* Check the I2S parameters */
  assert_param(IS_I2S_ALL_INSTANCE(SPIx));
  assert_param(IS_LL_I2S_MODE(I2S_InitStruct->Mode));
  assert_param(IS_LL_I2S_STANDARD(I2S_InitStruct->Standard));
  assert_param(IS_LL_I2S_DATAFORMAT(I2S_InitStruct->DataFormat));
  assert_param(IS_LL_I2S_MCLK_OUTPUT(I2S_InitStruct->MCLKOutput));
  assert_param(IS_LL_I2S_AUDIO_FREQ(I2S_InitStruct->AudioFreq));
  assert_param(IS_LL_I2S_CKPOL(I2S_InitStruct->ClockPolarity));

  /* Check that SPE bit is set to 0 in order to be sure that SPI/I2S block is disabled.
   * In this case, it is useless to check if the I2SMOD bit is set to 0 because
   * this bit I2SMOD only serves to select the desired mode.
   */
  if (LL_SPI_IsEnabled(SPIx) == 0x00000000UL)
  {
    /*---------------------------- SPIx I2SCFGR Configuration --------------------
     * Configure SPIx I2SCFGR with parameters:
     * - Mode           : SPI_I2SCFGR_I2SCFG[2:0] bits
     * - Standard       : SPI_I2SCFGR_I2SSTD[1:0] and SPI_I2SCFGR_PCMSYNC bits
     * - DataFormat     : SPI_I2SCFGR_CHLEN, SPI_I2SCFGR_DATFMT and SPI_I2SCFGR_DATLEN[1:0] bits
     * - ClockPolarity  : SPI_I2SCFGR_CKPOL bit
     * - MCLKOutput     : SPI_I2SPR_MCKOE bit
     * - I2S mode       : SPI_I2SCFGR_I2SMOD bit
     */

    /* Write to SPIx I2SCFGR */
    MODIFY_REG(SPIx->I2SCFGR,
               I2S_I2SCFGR_CLEAR_MASK,
               I2S_InitStruct->Mode       | I2S_InitStruct->Standard      |
               I2S_InitStruct->DataFormat | I2S_InitStruct->ClockPolarity |
               I2S_InitStruct->MCLKOutput | SPI_I2SCFGR_I2SMOD);

    /*---------------------------- SPIx I2SCFGR Configuration ----------------------
     * Configure SPIx I2SCFGR with parameters:
     * - AudioFreq      : SPI_I2SCFGR_I2SDIV[7:0] and SPI_I2SCFGR_ODD bits
     */

    /* If the requested audio frequency is not the default, compute the prescaler (i2sodd, i2sdiv)
     * else, default values are used:  i2sodd = 0U, i2sdiv = 0U.
     */
    if (I2S_InitStruct->AudioFreq != LL_I2S_AUDIOFREQ_DEFAULT)
    {
      /* Check the frame length (For the Prescaler computing)
       * Default value: LL_I2S_DATAFORMAT_16B (packetlength = 1U).
       */
      if (I2S_InitStruct->DataFormat != LL_I2S_DATAFORMAT_16B)
      {
        /* Packet length is 32 bits */
        packetlength = 2UL;
      }

      /* Check if PCM standard is used */
      if ((I2S_InitStruct->Standard == LL_I2S_STANDARD_PCM_SHORT) ||
          (I2S_InitStruct->Standard == LL_I2S_STANDARD_PCM_LONG))
      {
        ispcm = 1UL;
      }

      /* Get the I2S (SPI) source clock value */
      if (SPIx == SPI1)
      {
        sourceclock = LL_RCC_GetSPIClockFreq(LL_RCC_SPI1_CLKSOURCE);
      }
      else if (SPIx == SPI2)
      {
        sourceclock = LL_RCC_GetSPIClockFreq(LL_RCC_SPI2_CLKSOURCE);
      }
      else if (SPIx == SPI3)
      {
        sourceclock = LL_RCC_GetSPIClockFreq(LL_RCC_SPI3_CLKSOURCE);
      }
      else /* SPI6 */
      {
        sourceclock = LL_RCC_GetSPIClockFreq(LL_RCC_SPI6_CLKSOURCE);
      }

      /* Compute the Real divider depending on the MCLK output state with a fixed point */
      if (I2S_InitStruct->MCLKOutput == LL_I2S_MCLK_OUTPUT_ENABLE)
      {
        /* MCLK output is enabled */
        tmp = (((sourceclock / (256UL >> ispcm)) * 16UL) / I2S_InitStruct->AudioFreq) + 8UL;
      }
      else
      {
        /* MCLK output is disabled */
        tmp = (((sourceclock / ((32UL >> ispcm) * packetlength)) * 16UL) / I2S_InitStruct->AudioFreq) + 8UL;
      }

      /* Remove the fixed point */
      tmp = tmp / 16UL;

      /* Check the parity of the divider */
      i2sodd = tmp & 0x1UL;

      /* Compute the i2sdiv prescaler */
      i2sdiv = tmp / 2UL;
    }

    /* Test if the obtain values are forbidden or out of range */
    if (((i2sodd == 1UL) && (i2sdiv == 1UL)) || (i2sdiv > 0xFFUL))
    {
      /* Set the default values */
      i2sdiv = 0UL;
      i2sodd = 0UL;
    }

    /* Write to SPIx I2SCFGR register the computed value */
    MODIFY_REG(SPIx->I2SCFGR,
               SPI_I2SCFGR_ODD                 | SPI_I2SCFGR_I2SDIV,
               (i2sodd << SPI_I2SCFGR_ODD_Pos) | (i2sdiv << SPI_I2SCFGR_I2SDIV_Pos));

    status = SUCCESS;
  }

  return status;
}

/**
  * @brief  Set each @ref LL_I2S_InitTypeDef field to default value.
  * @param  I2S_InitStruct pointer to a @ref LL_I2S_InitTypeDef structure
  *         whose fields will be set to default values.
  * @retval None
  */
void LL_I2S_StructInit(LL_I2S_InitTypeDef *I2S_InitStruct)
{
  /*--------------- Reset I2S init structure parameters values -----------------*/
  I2S_InitStruct->Mode              = LL_I2S_MODE_SLAVE_TX;
  I2S_InitStruct->Standard          = LL_I2S_STANDARD_PHILIPS;
  I2S_InitStruct->DataFormat        = LL_I2S_DATAFORMAT_16B;
  I2S_InitStruct->MCLKOutput        = LL_I2S_MCLK_OUTPUT_DISABLE;
  I2S_InitStruct->AudioFreq         = LL_I2S_AUDIOFREQ_DEFAULT;
  I2S_InitStruct->ClockPolarity     = LL_I2S_POLARITY_LOW;
}

/**
  * @brief  Set linear and parity prescaler.
  * @note   To calculate value of PrescalerLinear(I2SDIV[7:0] bits) and PrescalerParity(ODD bit)\n
  *         Check Audio frequency table and formulas inside Reference Manual (SPI/I2S).
  * @param  SPIx SPI Instance
  * @param  PrescalerLinear Value between Min_Data=0x00 and Max_Data=0xFF
  * @note   PrescalerLinear '1' is not authorized with parity LL_I2S_PRESCALER_PARITY_ODD
  * @param  PrescalerParity This parameter can be one of the following values:
  *         @arg @ref LL_I2S_PRESCALER_PARITY_EVEN
  *         @arg @ref LL_I2S_PRESCALER_PARITY_ODD
  * @retval None
  */
void LL_I2S_ConfigPrescaler(SPI_TypeDef *SPIx, uint32_t PrescalerLinear, uint32_t PrescalerParity)
{
  /* Check the I2S parameters */
  assert_param(IS_I2S_ALL_INSTANCE(SPIx));
  assert_param(IS_LL_I2S_PRESCALER_LINEAR(PrescalerLinear));
  assert_param(IS_LL_I2S_PRESCALER_PARITY(PrescalerParity));

  /* Write to SPIx I2SPR */
  MODIFY_REG(SPIx->I2SCFGR, SPI_I2SCFGR_I2SDIV | SPI_I2SCFGR_ODD, (PrescalerLinear << SPI_I2SCFGR_I2SDIV_Pos) |
             (PrescalerParity << SPI_I2SCFGR_ODD_Pos));
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

#endif /* defined(SPI1) || defined(SPI2) || defined(SPI3) || defined(SPI4) || defined(SPI5) || defined(SPI6) */

/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */
