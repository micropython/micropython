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

#ifndef _FSL_ENET_PHY_CMSIS_H_
#define _FSL_ENET_PHY_CMSIS_H_
#include "Driver_ETH.h"
#include "Driver_ETH_PHY.h"
#include "RTE_Device.h"
#include "fsl_common.h"

#if defined(ENET) & RTE_ENET
extern ARM_DRIVER_ETH_PHY Driver_ETH_PHY0;
#endif
#endif
