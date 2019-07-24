/*
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
 * Copyright (c) 2016, Freescale Semiconductor, Inc. Not a Contribution.
 * Copyright 2016-2017 NXP. Not a Contribution.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _FSL_SPI_CMSIS_H_
#define _FSL_SPI_CMSIS_H_

#include "fsl_ecspi.h"
#include "RTE_Device.h"
#include "Driver_SPI.h"
#if (RTE_SPI1_DMA_EN || RTE_SPI2_DMA_EN || RTE_SPI3_DMA_EN)
#if defined(FSL_FEATURE_SOC_SDMA_COUNT) && (FSL_FEATURE_SOC_SDMA_COUNT)
#include "fsl_ecspi_sdma.h"
#endif
#endif

#if defined(ECSPI1)
extern ARM_DRIVER_SPI Driver_SPI1;
#endif /* SPI1 */

#if defined(ECSPI2)
extern ARM_DRIVER_SPI Driver_SPI2;
#endif /* SPI2 */

#if defined(ECSPI3)
extern ARM_DRIVER_SPI Driver_SPI3;
#endif /* SPI3 */

#define SPI_FLAG_UNINIT (0)
#define SPI_FLAG_INIT (1 << 0)
#define SPI_FLAG_POWER (1 << 1)
#define SPI_FLAG_CONFIGURED (1 << 2)
#define SPI_FLAG_MASTER (1 << 3)

#endif
