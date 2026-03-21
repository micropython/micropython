/*******************************************************************************
 * File Name: cycfg.h
 *
 * Description:
 * Simple wrapper containing all generated files and function to initialize
 * all generated code.
 * This file was automatically generated and should not be modified.
 * Configurator Backend 3.60.0
 * device-db 4.34.0.9502
 * mtb-dsl-pse8xxgp 1.2.0.895
 *
 *******************************************************************************
 * Copyright 2026 Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/

#if !defined(CYCFG_H)
#define CYCFG_H

#include "cycfg_notices.h"
#include "cycfg_memory.h"
#include "cymem_memory_types.h"
#include "cymem_memory_locations.h"
#include "cymem_CM33_0.h"
#include "cymem_CM33_0_S.h"
#include "cymem_CM55_0.h"
#include "cycfg_clocks.h"
#include "cycfg_clock_types.h"
#include "cycfg_peripheral_clocks.h"
#include "cycfg_system.h"
#include "cycfg_ppc.h"
#include "cycfg_dmas.h"
#include "cycfg_peripherals.h"
#include "cycfg_routing.h"
#include "cycfg_pins.h"
#include "cycfg_protection.h"
#include "cycfg_connectivity_wifi.h"
#include "cycfg_connectivity_bt.h"

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

void init_cycfg_all(void);
void cycfg_config_init(void);
static inline void cycfg_config_reservations(void) {}

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* CYCFG_H */
