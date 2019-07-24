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

#ifndef _FSL_USART_CMSIS_H_
#define _FSL_USART_CMSIS_H_

#include "fsl_common.h"
#include "Driver_USART.h"
#include "RTE_Device.h"
#include "fsl_usart.h"
#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
#include "fsl_usart_dma.h"
#endif

#if defined(USART0)
extern ARM_DRIVER_USART Driver_USART0;
#endif /* USART0 */

#if defined(USART1)
extern ARM_DRIVER_USART Driver_USART1;
#endif /* USART1 */

#if defined(USART2)
extern ARM_DRIVER_USART Driver_USART2;
#endif /* USART2 */

#if defined(USART3)
extern ARM_DRIVER_USART Driver_USART3;
#endif /* USART3 */

#if defined(USART4)
extern ARM_DRIVER_USART Driver_USART4;
#endif /* USART4 */

#if defined(USART5)
extern ARM_DRIVER_USART Driver_USART5;
#endif /* USART5 */

#if defined(USART6)
extern ARM_DRIVER_USART Driver_USART6;
#endif /* USART6 */

#if defined(USART7)
extern ARM_DRIVER_USART Driver_USART7;
#endif /* USART7 */

/* USART Driver state flags */
#define USART_FLAG_UNINIT (0)
#define USART_FLAG_INIT (1 << 0)
#define USART_FLAG_POWER (1 << 1)
#define USART_FLAG_CONFIGURED (1 << 2)

#endif /* _FSL_USART_CMSIS_H_ */
