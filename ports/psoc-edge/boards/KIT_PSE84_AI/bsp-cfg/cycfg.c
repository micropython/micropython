/*******************************************************************************
 * File Name: cycfg.c
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

#include "cycfg.h"

/* This function is provided for compatibility with older 2.X style projects. */
void init_cycfg_all(void) {
    cycfg_config_init();
    cycfg_config_reservations();
}
void cycfg_config_init(void) {
    init_cycfg_clocks();
    /**
     * Handled in runtime during machine
     * object creation and initialization
     * via `clk` module.
     */
    #if defined(COMPONENT_SECURE_DEVICE)
    /* Secure boot initializes minimal periph initialization */
    init_cycfg_peripheral_clocks_s_minimal();
    init_cycfg_peripheral_s_minimal();
    #endif /* defined(COMPONENT_SECURE_DEVICE) */
    init_cycfg_system();
    init_cycfg_routing();
    init_cycfg_pins();
    init_cycfg_protection();
}
