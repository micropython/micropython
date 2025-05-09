/**
  ******************************************************************************
  * @file    partition_stm32n6xx.h
  * @author  MCD Application Team
  * @brief   CMSIS STM32N6xx Device Header File for Initial Setup for
  *          Secure / Non-Secure Zones based on CMSIS CORE V5.3.1
  *
  *          The file is included in system_stm32L5xx_s.c in secure application.
  *          It includes the configuration section that allows to select the
  *          STM32N6xx device partitioning file for system core secure attributes
  *          and interrupt secure and non-secure assignment.
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
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup STM32N6xx
  * @{
  */

#ifndef PARTITION_STM32N6XX_H
#define PARTITION_STM32N6XX_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @addtogroup Secure_configuration_section
  * @{
  */

#if defined(STM32N657xx)
  #include "partition_stm32n657xx.h"
#elif defined(STM32N655xx)
  #include "partition_stm32n655xx.h"
#elif defined(STM32N647xx)
  #include "partition_stm32n647xx.h"
#elif defined(STM32N645xx)
  #include "partition_stm32n645xx.h"
#else
  #error "Please select first the target STM32N6xx device used in your application (in stm32n6xx.h file)"
#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PARTITION_STM32N6XX_H */

/**
  * @}
  */

/**
  * @}
  */

