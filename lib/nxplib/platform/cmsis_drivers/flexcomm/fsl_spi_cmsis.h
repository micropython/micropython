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

#include "fsl_spi.h"
#include "RTE_Device.h"
#include "Driver_SPI.h"
#if defined(FSL_FEATURE_SOC_DMA_COUNT) && (FSL_FEATURE_SOC_DMA_COUNT)
#include "fsl_spi_dma.h"
#endif

#if defined(SPI0)
extern ARM_DRIVER_SPI Driver_SPI0;
#endif /* SPI0 */

#if defined(SPI1)
extern ARM_DRIVER_SPI Driver_SPI1;
#endif /* SPI1 */

#if defined(SPI2)
extern ARM_DRIVER_SPI Driver_SPI2;
#endif /* SPI2 */

#if defined(SPI3)
extern ARM_DRIVER_SPI Driver_SPI3;
#endif /* SPI3 */

#if defined(SPI4)
extern ARM_DRIVER_SPI Driver_SPI4;
#endif /* SPI4 */

#if defined(SPI5)
extern ARM_DRIVER_SPI Driver_SPI5;
#endif /* SPI5 */

#if defined(SPI6)
extern ARM_DRIVER_SPI Driver_SPI6;
#endif /* SPI6 */

#if defined(SPI7)
extern ARM_DRIVER_SPI Driver_SPI7;
#endif /* SPI7 */

#if defined(SPI8)
extern ARM_DRIVER_SPI Driver_SPI8;
#endif /* SPI8 */

#if defined(SPI9)
extern ARM_DRIVER_SPI Driver_SPI9;
#endif /* SPI9 */

#define SPI_FLAG_UNINIT (0)
#define SPI_FLAG_INIT (1 << 0)
#define SPI_FLAG_POWER (1 << 1)
#define SPI_FLAG_CONFIGURED (1 << 2)
#define SPI_FLAG_MASTER (1 << 3)

#endif
