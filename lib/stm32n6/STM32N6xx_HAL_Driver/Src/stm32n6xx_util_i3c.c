/**
  **********************************************************************************************************************
  * @file    stm32n6xx_util_i3c.c
  * @author  MCD Application Team
  * @brief   This utility help to calculate the different I3C Timing.
  **********************************************************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  **********************************************************************************************************************
  */

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "stm32n6xx_util_i3c.h"

/** @addtogroup STM32N6xx_UTIL_Driver
  * @{
  */

/** @addtogroup UTILITY_I3C
  * @{
  */

#if (defined(USE_HAL_DRIVER) && defined(HAL_I3C_MODULE_ENABLED)) || defined(USE_FULL_LL_DRIVER)

/* Private typedef ---------------------------------------------------------------------------------------------------*/
/* Private define ----------------------------------------------------------------------------------------------------*/
/** @defgroup I3C_UTIL_Private_Define I3C Utility Private Define
  * @{
  */
#define SEC210PSEC              (uint64_t)100000000000 /*!< 10ps, to take two decimal float of ns calculation */
#define TI3CH_MIN               3200U       /*!< Open drain & push pull SCL high min, 32ns */
#define TI3CH_OD_MAX            4100U       /*!< Open drain SCL high max, 41 ns */
#define TI3CL_OD_MIN            20000U      /*!< Open drain SCL low min, 200 ns */
#define TFMPL_OD_MIN            50000U      /*!< Fast Mode Plus Open drain SCL low min, 500 ns */
#define TFML_OD_MIN             130000U     /*!< Fast Mode Open drain SCL low min, 1300 ns */
#define TFM_MIN                 250000U     /*!< Fast Mode, period min for ti3cclk, 2.5us */
#define TSM_MIN                 1000000U    /*!< Standard Mode, period min for ti3cclk, 10us */
#define TI3C_CAS_MIN            3840U       /*!< Time SCL after START min, 38.4 ns */
#define TCAPA                   35000U      /*!< Capacitor effect Value measure on Nucleo around 350ns */
#define I3C_FREQUENCY_MAX       257000000U  /*!< Maximum I3C frequency */
/**
  * @}
  */

/* Private macro -----------------------------------------------------------------------------------------------------*/
/** @defgroup I3C_UTIL_Private_Macro I3C Utility Private Macro
  * @{
  */
#define DIV_ROUND_CLOSEST(x, d) (((x) + ((d) / 2U)) / (d))
/**
  * @}
  */

/* Private function prototypes ---------------------------------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------------------------------------------------*/
/** @defgroup I3C_UTIL_Exported_Functions I3C Utility Exported Functions
  * @{
  */

/** @defgroup I3C_UTIL_EF_Computation Computation
  * @{
  */
/**
  * @brief  Calculate the I3C Controller timing according current I3C clock source and required I3C bus clock.
  * @param  pInputTiming    : [IN]  Pointer to an I3C_CtrlTimingTypeDef structure that contains
  *                                 the required parameter for I3C timing computation.
  * @param  pOutputConfig   : [OUT]  Pointer to an LL_I3C_CtrlBusConfTypeDef structure that contains
  *                                  the configuration information for the specified I3C.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Timing calculation successfully
  *          - ERROR: Parameters or timing calculation error
  */
ErrorStatus I3C_CtrlTimingComputation(const I3C_CtrlTimingTypeDef *pInputTiming,
                                      LL_I3C_CtrlBusConfTypeDef *pOutputConfig)
{
  ErrorStatus status = SUCCESS;

  /* MIPI Standard constants */
  /* I3C: Open drain & push pull SCL high min, tDIG_H & tDIG_H_MIXED: 32 ns */
  uint32_t ti3ch_min       = TI3CH_MIN;

  /* I3C: Open drain SCL high max, t_HIGH: 41 ns */
  uint32_t ti3ch_od_max   = TI3CH_OD_MAX;

  /* I3C: Open drain SCL high max, tHIGH: 41 ns (Ti3ch_od_max= 410)
      I3C (pure bus): Open drain SCL low min, tLOW_OD: 200 ns */
  uint32_t ti3cl_od_min    = TI3CL_OD_MIN;

  /* I3C (mixed bus): Open drain SCL low min,
     tLOW: 500 ns (FM+ I2C on the bus)
     tLOW: 1300 ns (FM I2C on the bus) */
  uint32_t tfmpl_od_min    = TFMPL_OD_MIN;
  uint32_t tfml_od_min     = TFML_OD_MIN;

  /* I2C: min ti3cclk
          fSCL: 1 MHz (FM+)
          fSCL: 100 kHz (SM) */
  uint32_t tfm_min         = TFM_MIN;
  uint32_t tsm_min         = TSM_MIN;

  /* I3C: time SCL after START min, Tcas: 38,4 ns */
  uint32_t ti3c_cas_min    = TI3C_CAS_MIN;

  /* Period Clock source */
  uint32_t ti3cclk = 0U;

  /* I3C: Push pull period */
  uint32_t ti3c_pp_min = 0U;

  /* I2C: Open drain period */
  uint32_t ti2c_od_min = 0U;

  /* Time for SDA rise to 70% VDD from GND, capacitor effect */
  /* Value measure on Nucleo around 350ns */
  uint32_t tcapa = TCAPA;

  /* Compute variable */
  uint32_t sclhi3c;
  uint32_t scllpp;
  uint32_t scllod;
  uint32_t sclhi2c;
  uint32_t oneus;
  uint32_t free;
  uint32_t sdahold;

  /* Verify Parameters */
  if (pInputTiming->clockSrcFreq > I3C_FREQUENCY_MAX)
  {
    /* Above this frequency, some timing register parameters are over than field value */
    status = ERROR;
  }

  if ((pInputTiming->busType != I3C_PURE_I3C_BUS) && (pInputTiming->busType != I3C_MIXED_BUS))
  {
    status = ERROR;
  }

  if (((pInputTiming->clockSrcFreq == 0U) || (pInputTiming->i3cPPFreq == 0U)) &&
      (pInputTiming->busType == I3C_PURE_I3C_BUS))
  {
    status = ERROR;
  }

  if (((pInputTiming->clockSrcFreq == 0U) || (pInputTiming->i3cPPFreq == 0U) || (pInputTiming->i2cODFreq == 0U)) &&
      (pInputTiming->busType == I3C_MIXED_BUS))
  {
    status = ERROR;
  }

  if (status == SUCCESS)
  {
    /* Period Clock source */
    ti3cclk = (uint32_t)((SEC210PSEC + ((uint64_t)pInputTiming->clockSrcFreq / (uint64_t)2)) /
                         (uint64_t)pInputTiming->clockSrcFreq);

    if ((pInputTiming->dutyCycle > 50U) || (ti3cclk == 0U))
    {
      status = ERROR;
    }
  }

  if ((status == SUCCESS) && (ti3cclk != 0U))
  {
    /* I3C: Push pull period */
    ti3c_pp_min = (uint32_t)((SEC210PSEC + ((uint64_t)pInputTiming->i3cPPFreq / (uint64_t)2)) /
                             (uint64_t)pInputTiming->i3cPPFreq);

    /* I2C: Open drain period */
    ti2c_od_min = (uint32_t)((SEC210PSEC + ((uint64_t)pInputTiming->i2cODFreq / (uint64_t)2)) /
                             (uint64_t)pInputTiming->i2cODFreq);

    if ((pInputTiming->busType != I3C_PURE_I3C_BUS) && (ti2c_od_min > tsm_min))
    {
      status = ERROR;
    }
  }

  /* SCL Computation */
  if ((status == SUCCESS) && (ti3cclk != 0U))
  {
    /* I3C SCL high level (push-pull & open drain) */
    if (pInputTiming->busType == I3C_PURE_I3C_BUS)
    {
      sclhi3c = DIV_ROUND_CLOSEST(DIV_ROUND_CLOSEST(ti3c_pp_min * pInputTiming->dutyCycle, ti3cclk), 100U) - 1U;

      /* Check if sclhi3c < ti3ch_min, in that case calculate sclhi3c based on ti3ch_min */
      if (((sclhi3c + 1U) * ti3cclk) < ti3ch_min)
      {
        sclhi3c = DIV_ROUND_CLOSEST(ti3ch_min, ti3cclk) - 1U;

        /* Check if sclhi3c < ti3ch_min */
        if (((sclhi3c + 1U) * ti3cclk) < ti3ch_min)
        {
          sclhi3c += 1U;
        }

        scllpp = DIV_ROUND_CLOSEST(ti3c_pp_min, ti3cclk) - (sclhi3c + 1U) - 1U;
      }
      else
      {
        sclhi3c = DIV_ROUND_CLOSEST(DIV_ROUND_CLOSEST(ti3c_pp_min * pInputTiming->dutyCycle, ti3cclk), 100U) - 1U;

        /* Check if sclhi3c < ti3ch_min */
        if (((sclhi3c + 1U) * ti3cclk) < ti3ch_min)
        {
          sclhi3c += 1U;
        }

        scllpp  = DIV_ROUND_CLOSEST((ti3c_pp_min - ((sclhi3c + 1U) * ti3cclk) + (ti3cclk / 2U)), ti3cclk) - 1U;
      }

    }
    else
    {
      /* Warning: (sclhi3c + 1) * ti3cclk > Ti3ch_od_max expected */
      sclhi3c = DIV_ROUND_CLOSEST(ti3ch_od_max, ti3cclk) - 1U;

      if (((sclhi3c + 1U) * ti3cclk) < ti3ch_min)
      {
        sclhi3c += 1U;
      }
      else if (((sclhi3c + 1U) * ti3cclk) > ti3ch_od_max)
      {
        sclhi3c = (ti3ch_od_max / ti3cclk);
      }
      else
      {
        /* Do nothing, keep sclhi3c as previously calculated */
      }

      /* I3C SCL low level (push-pull) */
      /* tscllpp = (scllpp + 1) x ti3cclk */
      scllpp  = DIV_ROUND_CLOSEST((ti3c_pp_min - ((sclhi3c + 1U) * ti3cclk)), ti3cclk) - 1U;
    }

    /* Check if scllpp is superior at (ti3c_pp_min + 1/2 clock source cycle) */
    /* Goal is to choice the scllpp approach lowest, to have a value frequency highest approach as possible */
    uint32_t ideal_scllpp = (ti3c_pp_min - ((sclhi3c + 1U) * ti3cclk));
    if (((scllpp + 1U) * ti3cclk) >= (ideal_scllpp + (ti3cclk / 2U) + 1U))
    {
      scllpp -= 1U;
    }

    /* Check if scllpp + sclhi3c is inferior at (ti3c_pp_min + 1/2 clock source cycle) */
    /* Goal is to increase the scllpp, to have a value frequency not out of the clock request */
    if (((scllpp + sclhi3c + 1U + 1U) * ti3cclk) < (ideal_scllpp + (ti3cclk / 2U) + 1U))
    {
      scllpp += 1U;
    }

    /* I3C SCL low level (pure I3C bus) */
    if (pInputTiming->busType == I3C_PURE_I3C_BUS)
    {
      if (ti3c_pp_min < ti3cl_od_min)
      {
        scllod  = DIV_ROUND_CLOSEST(ti3cl_od_min, ti3cclk) - 1U;

        if (((scllod + 1U) * ti3cclk) < ti3cl_od_min)
        {
          scllod += 1U;
        }
      }
      else
      {
        scllod = scllpp;
      }

      /* Verify that SCL Open drain Low duration is superior as SDA rise time 70% */
      if (((scllod + 1U) * ti3cclk) < tcapa)
      {
        scllod = DIV_ROUND_CLOSEST(tcapa, ti3cclk) + 1U;
      }

      sclhi2c = 0U; /* I2C SCL not used in pure I3C bus */
    }
    /* SCL low level on mixed bus (open-drain) */
    /* I2C SCL high level (mixed bus with I2C) */
    else
    {
      scllod  = DIV_ROUND_CLOSEST(DIV_ROUND_CLOSEST(ti2c_od_min * (100U - pInputTiming->dutyCycle),
                                                    ti3cclk), 100U) - 1U;

      /* Mix Bus Fast Mode plus */
      if (ti2c_od_min < tfm_min)
      {
        if (((scllod + 1U) * ti3cclk) < tfmpl_od_min)
        {
          scllod  = DIV_ROUND_CLOSEST(tfmpl_od_min, ti3cclk) - 1U;
        }
      }
      /* Mix Bus Fast Mode */
      else
      {
        if (((scllod + 1U) * ti3cclk) < tfml_od_min)
        {
          scllod  = DIV_ROUND_CLOSEST(tfml_od_min, ti3cclk) - 1U;
        }
      }

      sclhi2c = DIV_ROUND_CLOSEST((ti2c_od_min - ((scllod + 1U) * ti3cclk)), ti3cclk) - 1U;
    }

    /* Clock After Start computation */

    /* I3C pure bus: (Tcas + tcapa)/2 */
    if (pInputTiming->busType == I3C_PURE_I3C_BUS)
    {
      free = DIV_ROUND_CLOSEST((ti3c_cas_min + tcapa), (2U * ti3cclk)) + 1U;
    }
    /* I3C, I2C mixed: (scllod + tcapa)/2 */
    else
    {
      free = DIV_ROUND_CLOSEST((((scllod + 1U) * ti3cclk) + tcapa), (2U * ti3cclk));
    }

    /* One cycle hold time addition */
    /* By default 1/2 cycle: must be > 3 ns */
    if (ti3cclk > 600U)
    {
      sdahold = 0U;
    }
    else
    {
      sdahold = 1U;
    }

    /* 1 microsecond reference */
    oneus = DIV_ROUND_CLOSEST(100000U, ti3cclk) - 2U;

    if ((scllpp > 0xFFU) || (sclhi3c > 0xFFU) || (scllod > 0xFFU) || (sclhi2c > 0xFFU) ||
        (free > 0xFFU) || (oneus > 0xFFU))
    {
      /* Case of value is over 8bits, issue may be due to clocksource have a rate too high for bus clock request */
      /* Update the return status */
      status = ERROR;
    }
    else
    {
      /* SCL configuration */
      pOutputConfig->SCLPPLowDuration = (uint8_t)scllpp;
      pOutputConfig->SCLI3CHighDuration = (uint8_t)sclhi3c;
      pOutputConfig->SCLODLowDuration = (uint8_t)scllod;
      pOutputConfig->SCLI2CHighDuration = (uint8_t)sclhi2c;

      /* Free, Idle and SDA hold time configuration */
      pOutputConfig->BusFreeDuration = (uint8_t)free;
      pOutputConfig->BusIdleDuration = (uint8_t)oneus;
      pOutputConfig->SDAHoldTime = (uint32_t)(sdahold << I3C_TIMINGR1_SDA_HD_Pos);
    }
  }

  return status;
}

/**
  * @brief  Calculate the I3C Controller timing according current I3C clock source and required I3C bus clock.
  * @param  pInputTiming    : [IN]  Pointer to an I3C_TgtTimingTypeDef structure that contains
  *                                 the required parameter for I3C timing computation.
  * @param  pOutputConfig   : [OUT]  Pointer to an LL_I3C_TgtBusConfTypeDef structure that contains
  *                                      the configuration information for the specified I3C.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Timing calculation successfully
  *          - ERROR: Parameters or timing calculation error
  */
ErrorStatus I3C_TgtTimingComputation(const I3C_TgtTimingTypeDef *pInputTiming,
                                     LL_I3C_TgtBusConfTypeDef *pOutputConfig)
{
  ErrorStatus status = SUCCESS;
  uint32_t oneus;
  uint32_t ti3cclk = 0U;

  /* Verify Parameters */
  if (pInputTiming->clockSrcFreq == 0U)
  {
    status = ERROR;
  }

  if (status == SUCCESS)
  {
    /* Period Clock source */
    ti3cclk = (uint32_t)((SEC210PSEC + ((uint64_t)pInputTiming->clockSrcFreq / (uint64_t)2)) /
                         (uint64_t)pInputTiming->clockSrcFreq);

    /* Verify Parameters */
    if (ti3cclk == 0U)
    {
      status = ERROR;
    }
  }

  if ((status == SUCCESS) && (ti3cclk != 0U))
  {
    /* 1 microsecond reference */
    oneus =  DIV_ROUND_CLOSEST(100000U, ti3cclk) - 2U;

    /* Bus available time configuration */
    pOutputConfig->BusAvailableDuration = (uint8_t)oneus;
  }

  return status;
}
/**
  * @}
  */
/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/

#endif /* (defined(USE_HAL_DRIVER) && defined(HAL_I3C_MODULE_ENABLED)) || defined(USE_FULL_LL_DRIVER) */
/**
  * @}
  */

/**
  * @}
  */
