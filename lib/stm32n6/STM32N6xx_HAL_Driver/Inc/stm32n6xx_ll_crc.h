/**
  ******************************************************************************
  * @file    stm32n6xx_ll_crc.h
  * @author  MCD Application Team
  * @brief   Header file of CRC LL module.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32N6xx_LL_CRC_H
#define STM32N6xx_LL_CRC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx.h"

/** @addtogroup STM32N6xx_LL_Driver
  * @{
  */

#if defined(CRC)

/** @defgroup CRC_LL CRC
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup CRC_LL_Exported_Constants CRC Exported Constants
  * @{
  */

/** @defgroup CRC_LL_EC_POLYLENGTH Polynomial length
  * @{
  */
#define LL_CRC_POLYLENGTH_32B              0x00000000U                              /*!< 32 bits Polynomial size */
#define LL_CRC_POLYLENGTH_16B              CRC_CR_POLYSIZE_0                        /*!< 16 bits Polynomial size */
#define LL_CRC_POLYLENGTH_8B               CRC_CR_POLYSIZE_1                        /*!< 8 bits Polynomial size */
#define LL_CRC_POLYLENGTH_7B               (CRC_CR_POLYSIZE_1 | CRC_CR_POLYSIZE_0)  /*!< 7 bits Polynomial size */
/**
  * @}
  */

/** @defgroup CRC_LL_EC_INPUT_REVERSE_TYPE Input Reverse Type
  * @{
  */
#define LL_CRC_INDATA_REVERSETYPE_BIT             0x00000000U               /*!< Input Data reverse type at bit level granularity */
#define LL_CRC_INDATA_REVERSETYPE_BYTE_HALFWORD   CRC_CR_RTYPE_IN           /*!< Input Data reverse type at byte or half-word level granularity */
/**
  * @}
  */

/** @defgroup CRC_LL_EC_OUTPUT_REVERSE_TYPE Output Reverse Type
  * @{
  */
#define LL_CRC_OUTDATA_REVERSETYPE_BIT            0x00000000U              /*!< Output Data reverse type at bit level granularity */
#define LL_CRC_OUTDATA_REVERSETYPE_BYTE_HALFWORD  CRC_CR_RTYPE_OUT         /*!< Output Data reverse type at byte or half-word level granularity */
/**
  * @}
  */

/** @defgroup CRC_LL_EC_INDATA_REVERSE Input Data Reverse
  * @{
  */
#define LL_CRC_INDATA_REVERSE_NONE             0x00000000U                          /*!< Input Data bit order not affected */
#define LL_CRC_INDATA_REVERSE_BIT_BYBYTE       CRC_CR_REV_IN_0                      /*!< Input Data bit reversal done by byte */
#define LL_CRC_INDATA_REVERSE_BYTE             LL_CRC_INDATA_REVERSE_BIT_BYBYTE     /*!< Definition for compatibility with legacy code */
#define LL_CRC_INDATA_REVERSE_HALFWORD_BYWORD  (CRC_CR_RTYPE_IN | CRC_CR_REV_IN_0)  /*!< Input Data half-word reversal done by word */
#define LL_CRC_INDATA_REVERSE_BIT_BYHALFWORD   CRC_CR_REV_IN_1                      /*!< Input Data bit reversal done by half-word */
#define LL_CRC_INDATA_REVERSE_HALFWORD         LL_CRC_INDATA_REVERSE_BIT_BYHALFWORD /*!< Definition for compatibility with legacy code */
#define LL_CRC_INDATA_REVERSE_BYTE_BYWORD      (CRC_CR_RTYPE_IN | CRC_CR_REV_IN_1)  /*!< Input Data byte reversal done by word */
#define LL_CRC_INDATA_REVERSE_BIT_BYWORD       (CRC_CR_REV_IN_1 | CRC_CR_REV_IN_0)  /*!< Input Data bit reversal done by word */
#define LL_CRC_INDATA_REVERSE_WORD             LL_CRC_INDATA_REVERSE_BIT_BYWORD     /*!< Definition for compatibility with legacy code */
/**
  * @}
  */

/** @defgroup CRC_LL_EC_OUTDATA_REVERSE Output Data Reverse
  * @{
  */
#define LL_CRC_OUTDATA_REVERSE_NONE        0x00000000U                           /*!< Output Data bit order not affected */
#define LL_CRC_OUTDATA_REVERSE_BIT         CRC_CR_REV_OUT_0                      /*!< Output Data bit reversal done by bit */
#define LL_CRC_OUTDATA_REVERSE_HALFWORD    (CRC_CR_RTYPE_OUT | CRC_CR_REV_OUT_0) /*!< Output Data half-word reversal done by word */
#define LL_CRC_OUTDATA_REVERSE_BYTE        (CRC_CR_RTYPE_OUT | CRC_CR_REV_OUT_1) /*!< Output Data byte reversal done by word */
/**
  * @}
  */

/** @defgroup CRC_LL_EC_Default_Polynomial_Value    Default CRC generating polynomial value
  * @brief    Normal representation of this polynomial value is
  *           X^32 + X^26 + X^23 + X^22 + X^16 + X^12 + X^11 + X^10 +X^8 + X^7 + X^5 + X^4 + X^2 + X + 1 .
  * @{
  */
#define LL_CRC_DEFAULT_CRC32_POLY          0x04C11DB7U                               /*!< Default CRC generating polynomial value */
/**
  * @}
  */

/** @defgroup CRC_LL_EC_Default_InitValue    Default CRC computation initialization value
  * @{
  */
#define LL_CRC_DEFAULT_CRC_INITVALUE       0xFFFFFFFFU                               /*!< Default CRC computation initialization value */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup CRC_LL_Exported_Macros CRC Exported Macros
  * @{
  */

/** @defgroup CRC_LL_EM_WRITE_READ Common Write and read registers Macros
  * @{
  */

/**
  * @brief  Write a value in CRC register
  * @param  __INSTANCE__ CRC Instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_CRC_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, __VALUE__)

/**
  * @brief  Read a value in CRC register
  * @param  __INSTANCE__ CRC Instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_CRC_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/**
  * @}
  */

/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/
/** @defgroup CRC_LL_Exported_Functions CRC Exported Functions
  * @{
  */

/** @defgroup CRC_LL_EF_Configuration CRC Configuration functions
  * @{
  */

/**
  * @brief  Reset the CRC calculation unit.
  * @note   If Programmable Initial CRC value feature
  *         is available, also set the Data Register to the value stored in the
  *         CRC_INIT register, otherwise, reset Data Register to its default value.
  * @rmtoll CR           RESET         LL_CRC_ResetCRCCalculationUnit
  * @param  CRCx CRC Instance
  * @retval None
  */
__STATIC_INLINE void LL_CRC_ResetCRCCalculationUnit(CRC_TypeDef *CRCx)
{
  SET_BIT(CRCx->CR, CRC_CR_RESET);
}

/**
  * @brief  Configure size of the polynomial.
  * @rmtoll CR           POLYSIZE      LL_CRC_SetPolynomialSize
  * @param  CRCx CRC Instance
  * @param  PolySize This parameter can be one of the following values:
  *         @arg @ref LL_CRC_POLYLENGTH_32B
  *         @arg @ref LL_CRC_POLYLENGTH_16B
  *         @arg @ref LL_CRC_POLYLENGTH_8B
  *         @arg @ref LL_CRC_POLYLENGTH_7B
  * @retval None
  */
__STATIC_INLINE void LL_CRC_SetPolynomialSize(CRC_TypeDef *CRCx, uint32_t PolySize)
{
  MODIFY_REG(CRCx->CR, CRC_CR_POLYSIZE, PolySize);
}

/**
  * @brief  Return size of the polynomial.
  * @rmtoll CR           POLYSIZE      LL_CRC_GetPolynomialSize
  * @param  CRCx CRC Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_CRC_POLYLENGTH_32B
  *         @arg @ref LL_CRC_POLYLENGTH_16B
  *         @arg @ref LL_CRC_POLYLENGTH_8B
  *         @arg @ref LL_CRC_POLYLENGTH_7B
  */
__STATIC_INLINE uint32_t LL_CRC_GetPolynomialSize(const CRC_TypeDef *CRCx)
{
  return (uint32_t)(READ_BIT(CRCx->CR, CRC_CR_POLYSIZE));
}

/**
  * @brief  Configure the reversal type of the input data
  * @rmtoll CR           RTYPE_IN        LL_CRC_SetInputDataReverseType
  * @param  CRCx CRC Instance
  * @param  ReverseType This parameter can be one of the following values:
  *         @arg @ref LL_CRC_INDATA_REVERSETYPE_BIT
  *         @arg @ref LL_CRC_INDATA_REVERSETYPE_BYTE_HALFWORD
  * @retval None
  */
__STATIC_INLINE void LL_CRC_SetInputDataReverseType(CRC_TypeDef *CRCx, uint32_t ReverseType)
{
  MODIFY_REG(CRCx->CR, CRC_CR_RTYPE_IN, ReverseType);
}

/**
  * @brief  Return input data type of reversal
  * @rmtoll CR           RTYPE_IN        LL_CRC_GetInputDataReverseType
  * @param  CRCx CRC Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_CRC_INDATA_REVERSETYPE_BIT
  *         @arg @ref LL_CRC_INDATA_REVERSETYPE_BYTE_HALFWORD
  */
__STATIC_INLINE uint32_t LL_CRC_GetInputDataReverseType(CRC_TypeDef *CRCx)
{
  return (uint32_t)(READ_BIT(CRCx->CR, CRC_CR_RTYPE_IN));
}

/**
  * @brief  Configure the reversal type of the output data
  * @rmtoll CR           RTYPE_OUT        LL_CRC_SetOutputDataReverseType
  * @param  CRCx CRC Instance
  * @param  ReverseType This parameter can be one of the following values:
  *         @arg @ref LL_CRC_OUTDATA_REVERSETYPE_BIT
  *         @arg @ref LL_CRC_OUTDATA_REVERSETYPE_BYTE_HALFWORD
  * @retval None
  */
__STATIC_INLINE void LL_CRC_SetOutputDataReverseType(CRC_TypeDef *CRCx, uint32_t ReverseType)
{
  MODIFY_REG(CRCx->CR, CRC_CR_RTYPE_OUT, ReverseType);
}

/**
  * @brief  Return output data type of reversal
  * @rmtoll CR           RTYPE_OUT        LL_CRC_GetOutputDataReverseType
  * @param  CRCx CRC Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_CRC_OUTDATA_REVERSETYPE_BIT
  *         @arg @ref LL_CRC_OUTDATA_REVERSETYPE_BYTE_HALFWORD
  */
__STATIC_INLINE uint32_t LL_CRC_GetOutputDataReverseType(CRC_TypeDef *CRCx)
{
  return (uint32_t)(READ_BIT(CRCx->CR, CRC_CR_RTYPE_OUT));
}

/**
  * @brief  Configure the reversal of the bit order of the input data
  * @rmtoll CR           REV_IN        LL_CRC_SetInputDataReverseMode
  * @param  CRCx CRC Instance
  * @param  ReverseMode This parameter can be one of the following values:
  *         @arg @ref LL_CRC_INDATA_REVERSE_NONE
  *         @arg @ref LL_CRC_INDATA_REVERSE_BIT_BYBYTE
  *         @arg @ref LL_CRC_INDATA_REVERSE_HALFWORD_BYWORD
  *         @arg @ref LL_CRC_INDATA_REVERSE_BIT_BYHALFWORD
  *         @arg @ref LL_CRC_INDATA_REVERSE_BYTE_BYWORD
  *         @arg @ref LL_CRC_INDATA_REVERSE_BIT_BYWORD
  * @retval None
  */
__STATIC_INLINE void LL_CRC_SetInputDataReverseMode(CRC_TypeDef *CRCx, uint32_t ReverseMode)
{
  MODIFY_REG(CRCx->CR, (CRC_CR_RTYPE_IN | CRC_CR_REV_IN), ReverseMode);
}

/**
  * @brief  Return mode of reversal for input data bit order
  * @rmtoll CR           REV_IN        LL_CRC_GetInputDataReverseMode
  * @param  CRCx CRC Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_CRC_INDATA_REVERSE_NONE
  *         @arg @ref LL_CRC_INDATA_REVERSE_BIT_BYBYTE
  *         @arg @ref LL_CRC_INDATA_REVERSE_HALFWORD_BYWORD
  *         @arg @ref LL_CRC_INDATA_REVERSE_BIT_BYHALFWORD
  *         @arg @ref LL_CRC_INDATA_REVERSE_BYTE_BYWORD
  *         @arg @ref LL_CRC_INDATA_REVERSE_BIT_BYWORD
  */
__STATIC_INLINE uint32_t LL_CRC_GetInputDataReverseMode(const CRC_TypeDef *CRCx)
{
  return (uint32_t)(READ_BIT(CRCx->CR, (CRC_CR_RTYPE_IN | CRC_CR_REV_IN)));
}

/**
  * @brief  Configure the reversal of the bit order of the Output data
  * @rmtoll CR           REV_OUT       LL_CRC_SetOutputDataReverseMode
  * @param  CRCx CRC Instance
  * @param  ReverseMode This parameter can be one of the following values:
  *         @arg @ref LL_CRC_OUTDATA_REVERSE_NONE
  *         @arg @ref LL_CRC_OUTDATA_REVERSE_BIT
  *         @arg @ref LL_CRC_OUTDATA_REVERSE_HALFWORD
  *         @arg @ref LL_CRC_OUTDATA_REVERSE_BYTE
  * @retval None
  */
__STATIC_INLINE void LL_CRC_SetOutputDataReverseMode(CRC_TypeDef *CRCx, uint32_t ReverseMode)
{
  MODIFY_REG(CRCx->CR, (CRC_CR_RTYPE_OUT | CRC_CR_REV_OUT), ReverseMode);
}

/**
  * @brief Return mode of reversal of the bit order of the Output data
  * @rmtoll CR           REV_OUT       LL_CRC_GetOutputDataReverseMode
  * @param  CRCx CRC Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_CRC_OUTDATA_REVERSE_NONE
  *         @arg @ref LL_CRC_OUTDATA_REVERSE_BIT
  *         @arg @ref LL_CRC_OUTDATA_REVERSE_HALFWORD
  *         @arg @ref LL_CRC_OUTDATA_REVERSE_BYTE
  */
__STATIC_INLINE uint32_t LL_CRC_GetOutputDataReverseMode(const CRC_TypeDef *CRCx)
{
  return (uint32_t)(READ_BIT(CRCx->CR, (CRC_CR_RTYPE_OUT | CRC_CR_REV_OUT)));
}

/**
  * @brief  Initialize the Programmable initial CRC value.
  * @note   If the CRC size is less than 32 bits, the least significant bits
  *         are used to write the correct value
  * @note   LL_CRC_DEFAULT_CRC_INITVALUE could be used as value for InitCrc parameter.
  * @rmtoll INIT         INIT          LL_CRC_SetInitialData
  * @param  CRCx CRC Instance
  * @param  InitCrc Value to be programmed in Programmable initial CRC value register
  * @retval None
  */
__STATIC_INLINE void LL_CRC_SetInitialData(CRC_TypeDef *CRCx, uint32_t InitCrc)
{
  WRITE_REG(CRCx->INIT, InitCrc);
}

/**
  * @brief  Return current Initial CRC value.
  * @note   If the CRC size is less than 32 bits, the least significant bits
  *         are used to read the correct value
  * @rmtoll INIT         INIT          LL_CRC_GetInitialData
  * @param  CRCx CRC Instance
  * @retval Value programmed in Programmable initial CRC value register
  */
__STATIC_INLINE uint32_t LL_CRC_GetInitialData(const CRC_TypeDef *CRCx)
{
  return (uint32_t)(READ_REG(CRCx->INIT));
}

/**
  * @brief  Initialize the Programmable polynomial value
  *         (coefficients of the polynomial to be used for CRC calculation).
  * @note   LL_CRC_DEFAULT_CRC32_POLY could be used as value for PolynomCoef parameter.
  * @note   Please check Reference Manual and existing Errata Sheets,
  *         regarding possible limitations for Polynomial values usage.
  *         For example, for a polynomial of degree 7, X^7 + X^6 + X^5 + X^2 + 1 is written 0x65
  * @rmtoll POL          POL           LL_CRC_SetPolynomialCoef
  * @param  CRCx CRC Instance
  * @param  PolynomCoef Value to be programmed in Programmable Polynomial value register
  * @retval None
  */
__STATIC_INLINE void LL_CRC_SetPolynomialCoef(CRC_TypeDef *CRCx, uint32_t PolynomCoef)
{
  WRITE_REG(CRCx->POL, PolynomCoef);
}

/**
  * @brief  Return current Programmable polynomial value
  * @note   Please check Reference Manual and existing Errata Sheets,
  *         regarding possible limitations for Polynomial values usage.
  *         For example, for a polynomial of degree 7, X^7 + X^6 + X^5 + X^2 + 1 is written 0x65
  * @rmtoll POL          POL           LL_CRC_GetPolynomialCoef
  * @param  CRCx CRC Instance
  * @retval Value programmed in Programmable Polynomial value register
  */
__STATIC_INLINE uint32_t LL_CRC_GetPolynomialCoef(const CRC_TypeDef *CRCx)
{
  return (uint32_t)(READ_REG(CRCx->POL));
}

/**
  * @}
  */

/** @defgroup CRC_LL_EF_Data_Management Data_Management
  * @{
  */

/**
  * @brief  Write given 32-bit data to the CRC calculator
  * @rmtoll DR           DR            LL_CRC_FeedData32
  * @param  CRCx CRC Instance
  * @param  InData value to be provided to CRC calculator between between Min_Data=0 and Max_Data=0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE void LL_CRC_FeedData32(CRC_TypeDef *CRCx, uint32_t InData)
{
  WRITE_REG(CRCx->DR, InData);
}

/**
  * @brief  Write given 16-bit data to the CRC calculator
  * @rmtoll DR           DR            LL_CRC_FeedData16
  * @param  CRCx CRC Instance
  * @param  InData 16 bit value to be provided to CRC calculator between between Min_Data=0 and Max_Data=0xFFFF
  * @retval None
  */
__STATIC_INLINE void LL_CRC_FeedData16(CRC_TypeDef *CRCx, uint16_t InData)
{
  __IO uint16_t *pReg;

  pReg = (__IO uint16_t *)(__IO void *)(&CRCx->DR);                             /* Derogation MisraC2012 R.11.5 */
  *pReg = InData;
}

/**
  * @brief  Write given 8-bit data to the CRC calculator
  * @rmtoll DR           DR            LL_CRC_FeedData8
  * @param  CRCx CRC Instance
  * @param  InData 8 bit value to be provided to CRC calculator between between Min_Data=0 and Max_Data=0xFF
  * @retval None
  */
__STATIC_INLINE void LL_CRC_FeedData8(CRC_TypeDef *CRCx, uint8_t InData)
{
  *(uint8_t __IO *)(&CRCx->DR) = (uint8_t) InData;
}

/**
  * @brief  Return current CRC calculation result. 32 bits value is returned.
  * @rmtoll DR           DR            LL_CRC_ReadData32
  * @param  CRCx CRC Instance
  * @retval Current CRC calculation result as stored in CRC_DR register (32 bits).
  */
__STATIC_INLINE uint32_t LL_CRC_ReadData32(const CRC_TypeDef *CRCx)
{
  return (uint32_t)(READ_REG(CRCx->DR));
}

/**
  * @brief  Return current CRC calculation result. 16 bits value is returned.
  * @note   This function is expected to be used in a 16 bits CRC polynomial size context.
  * @rmtoll DR           DR            LL_CRC_ReadData16
  * @param  CRCx CRC Instance
  * @retval Current CRC calculation result as stored in CRC_DR register (16 bits).
  */
__STATIC_INLINE uint16_t LL_CRC_ReadData16(const CRC_TypeDef *CRCx)
{
  return (uint16_t)READ_REG(CRCx->DR);
}

/**
  * @brief  Return current CRC calculation result. 8 bits value is returned.
  * @note   This function is expected to be used in a 8 bits CRC polynomial size context.
  * @rmtoll DR           DR            LL_CRC_ReadData8
  * @param  CRCx CRC Instance
  * @retval Current CRC calculation result as stored in CRC_DR register (8 bits).
  */
__STATIC_INLINE uint8_t LL_CRC_ReadData8(const CRC_TypeDef *CRCx)
{
  return (uint8_t)READ_REG(CRCx->DR);
}

/**
  * @brief  Return current CRC calculation result. 7 bits value is returned.
  * @note   This function is expected to be used in a 7 bits CRC polynomial size context.
  * @rmtoll DR           DR            LL_CRC_ReadData7
  * @param  CRCx CRC Instance
  * @retval Current CRC calculation result as stored in CRC_DR register (7 bits).
  */
__STATIC_INLINE uint8_t LL_CRC_ReadData7(const CRC_TypeDef *CRCx)
{
  return (uint8_t)(READ_REG(CRCx->DR) & 0x7FU);
}

/**
  * @brief  Return data stored in the Independent Data(IDR) register.
  * @note   This register can be used as a temporary storage location for one 32-bit long data.
  * @rmtoll IDR          IDR           LL_CRC_Read_IDR
  * @param  CRCx CRC Instance
  * @retval Value stored in CRC_IDR register (General-purpose 32-bit data register).
  */
__STATIC_INLINE uint32_t LL_CRC_Read_IDR(const CRC_TypeDef *CRCx)
{
  return (uint32_t)(READ_REG(CRCx->IDR));
}

/**
  * @brief  Store data in the Independent Data(IDR) register.
  * @note   This register can be used as a temporary storage location for one 32-bit long data.
  * @rmtoll IDR          IDR           LL_CRC_Write_IDR
  * @param  CRCx CRC Instance
  * @param  InData value to be stored in CRC_IDR register (32-bit) between Min_Data=0 and Max_Data=0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE void LL_CRC_Write_IDR(CRC_TypeDef *CRCx, uint32_t InData)
{
  *((uint32_t __IO *)(&CRCx->IDR)) = (uint32_t) InData;
}
/**
  * @}
  */

#if defined(USE_FULL_LL_DRIVER)
/** @defgroup CRC_LL_EF_Init Initialization and de-initialization functions
  * @{
  */

ErrorStatus LL_CRC_DeInit(const CRC_TypeDef *CRCx);

/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/**
  * @}
  */

/**
  * @}
  */

#endif /* defined(CRC) */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_LL_CRC_H */
