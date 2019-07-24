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

#ifndef _FSL_I2C_CMSIS_H_
#define _FSL_I2C_CMSIS_H_
#include "fsl_common.h"
#include "Driver_I2C.h"
#include "RTE_Device.h"
#include "fsl_i2c.h"
#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
#include "fsl_dmamux.h"
#endif
#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
#include "fsl_i2c_dma.h"
#endif
#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
#include "fsl_i2c_edma.h"
#endif

#if defined(I2C0)
extern ARM_DRIVER_I2C Driver_I2C0;
#endif

#if defined(I2C1)
extern ARM_DRIVER_I2C Driver_I2C1;
#endif

#if defined(I2C2)
extern ARM_DRIVER_I2C Driver_I2C2;
#endif

#if defined(I2C3)
extern ARM_DRIVER_I2C Driver_I2C3;
#endif

/* I2C Driver state flags */
#define I2C_FLAG_UNINIT (0)
#define I2C_FLAG_INIT (1 << 0)
#define I2C_FLAG_POWER (1 << 1)

#endif
