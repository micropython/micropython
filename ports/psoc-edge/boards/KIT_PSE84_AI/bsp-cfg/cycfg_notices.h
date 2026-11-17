/*******************************************************************************
 * File Name: cycfg_notices.h
 *
 * Description:
 * Contains warnings and errors that occurred while generating code for the
 * design.
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

#if !defined(CYCFG_NOTICES_H)
#define CYCFG_NOTICES_H

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

#if defined(CY_SUPPORTS_DEVICE_VALIDATION)
#if !defined(PSE846GPS2DBZC4A)
#error "Unexpected MPN; expected DEVICE:=PSE846GPS2DBZC4A. There may be an inconsistency between the *.modus file and the makefile target configuration device sets."
#endif /* !defined(PSE846GPS2DBZC4A) */
#endif /* defined(CY_SUPPORTS_DEVICE_VALIDATION) */

#if defined(CY_SUPPORTS_COMPLETE_DEVICE_VALIDATION)
#if !defined(PSE846GPS2DBZC4A)
#error "Unexpected MPN; expected DEVICE:=PSE846GPS2DBZC4A, ADDITIONAL_DEVICES:=CYW55513IUBG. There may be an inconsistency between the *.modus file and the makefile target configuration device sets."
#endif /* !defined(PSE846GPS2DBZC4A) */
#if !defined(CYW55513IUBG)
#error "Unexpected MPN; expected DEVICE:=PSE846GPS2DBZC4A, ADDITIONAL_DEVICES:=CYW55513IUBG. There may be an inconsistency between the *.modus file and the makefile target configuration device sets."
#endif /* !defined(CYW55513IUBG) */
#endif /* defined(CY_SUPPORTS_COMPLETE_DEVICE_VALIDATION) */

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* CYCFG_NOTICES_H */
