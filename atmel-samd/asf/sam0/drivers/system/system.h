/**
 * \file
 *
 * \brief SAM System related functionality
 *
 * Copyright (C) 2012-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#include <compiler.h>
#include <clock.h>
#include <gclk.h>
#include <pinmux.h>
#include <power.h>
#include <reset.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup asfdoc_sam0_system_group SAM System (SYSTEM) Driver
 *
 * This driver for Atmel&reg; | SMART ARM&reg;-based microcontrollers provides an interface for the configuration
 * and management of the device's system relation functionality, necessary for
 * the basic device operation. This is not limited to a single peripheral, but
 * extends across multiple hardware peripherals.
 *
 * The following peripherals are used by this module:
 * \if DEVICE_SAML21_SYSTEM_SUPPORT
 *  - PM (Power Manager)
 *  - RSTC (Reset Controller)
 *  - SUPC (Supply Controller)
 * \endif
 * \if DEVICE_SAMC21_SYSTEM_SUPPORT
 *  - PM (Power Manager)
 *  - RSTC (Reset Controller)
 *  - SUPC (Supply Controller)
 * \endif
 * \if DEVICE_SAMD21_SYSTEM_SUPPORT
 *  - SYSCTRL (System Control)
 *  - PM (Power Manager)
 * \endif
 *
 * The following devices can use this module:
 * \if DEVICE_SAML21_SYSTEM_SUPPORT
 *  - Atmel | SMART SAM L21
 * \endif
 * \if DEVICE_SAMC21_SYSTEM_SUPPORT
 *  - Atmel | SMART SAM C20/C21
 * \endif
 * \if DEVICE_SAMD21_SYSTEM_SUPPORT
 *  - Atmel | SMART SAM D20/D21
 *  - Atmel | SMART SAM R21
 *  - Atmel | SMART SAM D09/D10/D11
 *  - Atmel | SMART SAM DA1
 * \endif
 *
 * The outline of this documentation is as follows:
 *  - \ref asfdoc_sam0_system_prerequisites
 *  - \ref asfdoc_sam0_system_module_overview
 *  - \ref asfdoc_sam0_system_special_considerations
 *  - \ref asfdoc_sam0_system_extra_info
 *  - \ref asfdoc_sam0_system_examples
 *  - \ref asfdoc_sam0_system_api_overview
 *
 *
 * \section asfdoc_sam0_system_prerequisites Prerequisites
 *
 * There are no prerequisites for this module.
 *
 *
 * \section asfdoc_sam0_system_module_overview Module Overview
 *
 * The System driver provides a collection of interfaces between the user
 * application logic, and the core device functionality (such as clocks, reset
 * cause determination, etc.) that is required for all applications. It contains
 * a number of sub-modules that control one specific aspect of the device:
 *
 * - System Core (this module)
 * - \ref asfdoc_sam0_system_clock_group "System Clock Control" (sub-module)
 * - \ref asfdoc_sam0_system_interrupt_group "System Interrupt Control" (sub-module)
 * - \ref asfdoc_sam0_system_pinmux_group "System Pin Multiplexer Control" (sub-module)
 *
 *
 * \if DEVICE_SAML21_SYSTEM_SUPPORT
 * \subsection asfdoc_sam0_system_module_overview_vreg_l21 Voltage Regulator
 * The SAM device controls the voltage regulators for the core (VDDCORE) and
 * backup (VDDBU) domains. It sets the voltage regulators according to the sleep
 * modes, the performance level, or the user configuration.
 *
 * In active mode, the voltage regulator can be chosen on the fly between a LDO
 * or a Buck converter. In standby mode, the low power voltage regulator is used
 * to supply VDDCORE.
 *
 * \subsection asfdoc_sam0_system_module_overview_bbps Battery Backup Power Switch
 * The SAM device supports connection of a battery backup to the VBAT power pin.
 * It includes functionality that enables automatic power switching between main
 * power and battery backup power. This will ensure power to the backup domain,
 * when the main battery or power source is unavailable.
 * \endif
 *
 * \if DEVICE_SAMC21_SYSTEM_SUPPORT
 * \subsection asfdoc_sam0_system_module_overview_vreg_c21 Voltage Regulator
 * The SAM device controls the voltage regulators for the core (VDDCORE). It sets
 * the voltage regulators according to the sleep modes.
 *
 * There are a selectable reference voltage and voltage dependent on the temperature
 * which can be used by analog modules like the ADC.
 * \endif
 *
 * \subsection asfdoc_sam0_system_module_overview_vref Voltage References
 * The various analog modules within the SAM devices (such as AC, ADC, and
 * DAC) require a voltage reference to be configured to act as a reference point
 * for comparisons and conversions.
 *
 * The SAM devices contain multiple references, including an internal
 * temperature sensor and a fixed band-gap voltage source. When enabled, the
 * associated voltage reference can be selected within the desired peripheral
 * where applicable.
 *
 * \subsection asfdoc_sam0_system_module_overview_reset_cause System Reset Cause
 * In some applications there may be a need to execute a different program
 * flow based on how the device was reset. For example, if the cause of reset
 * was the Watchdog timer (WDT), this might indicate an error in the application,
 * and a form of error handling or error logging might be needed.
 *
 * For this reason, an API is provided to retrieve the cause of the last system
 * reset, so that appropriate action can be taken.
 *
 * \if DEVICE_SAML21_SYSTEM_SUPPORT
 * There are three groups of reset sources:
 *   - Power supply reset: Resets caused by an electrical issue. It covers POR and BOD reset.
 *   - User reset: Resets caused by the application. It covers external reset,
 *             system reset, and watchdog reset.
 *   - Backup reset: Resets caused by a backup mode exit condition.
 *
 * \subsection asfdoc_sam0_system_module_overview_performance_level Performance Level
 * Performance level allows the user to adjust the regulator output voltage to reduce
 * power consumption. The user can on the fly select the most suitable performance
 * level, depending on the application demands.
 *
 * The SAM device can operate at two different performance levels (PL0 and PL2).
 * When operating at PL0, the voltage applied on the full logic area is reduced
 * by voltage scaling. This voltage scaling technique allows to reduce the active
 * power consumption while decreasing the maximum frequency of the device. When
 * operating at PL2, the voltage regulator supplies the highest voltage, allowing
 * the device to run at higher clock speeds.
 *
 * Performance level transition is possible only when the device is in active
 * mode. After a reset, the device starts at the lowest performance level
 * (lowest power consumption and lowest max. frequency). The application can then
 * switch to another performance level at any time without any stop in the code
 * execution. As shown in \ref asfdoc_sam0_system_performance_level_transition_figure.
 *
 * \note When scaling down the performance level, the bus frequency should first be
 *  scaled down in order to not exceed the maximum frequency allowed for the
 *  low performance level.
 *  When scaling up the performance level (e.g. from PL0 to PL2), check the performance
 *  level status before increasing the bus frequency. It can be increased only
 *  when the performance level transition is completed.
 *
 * \anchor asfdoc_sam0_system_performance_level_transition_figure
 * \image html performance_level_transition.svg "Performance Level Transition"
 *
 * \subsection asfdoc_sam0_system_module_overview_power_domain Power Domain Gating
 * Power domain gating allows power saving by reducing the voltage in logic
 * areas in the device to a low-power supply. The feature is available in
 * Standby sleep mode and will reduce the voltage in domains where all peripherals
 * are idle. Internal logic will maintain its content, meaning the corresponding
 * peripherals will not need to be reconfigured when normal operating voltage
 * is returned. Most power domains can be in the following three states:
 *
 * - Active state: The power domain is powered on.
 * - Retention state: The main voltage supply for the power domain is switched off,
 * while maintaining a secondary low-power supply for the sequential cells. The
 * logic context is restored when waking up.
 * - Off state: The power domain is entirely powered off. The logic context is lost.
 *
 * The SAM L21 device  contains three power domains which can be controlled using
 * power domain gating, namely PD0, PD1, and PD2. These power domains can be
 * configured to the following cases:
 * - Default with no sleepwalking peripherals: A power domain is automatically set
 * to retention state in standby sleep mode if no activity require it. The application
 * can force all power domains to remain in active state during standby sleep mode
 * in order to accelerate wakeup time.
 * - Default with sleepwalking peripherals: If one or more peripherals are enabled
 * to perform sleepwalking tasks in standby sleep mode, the corresponding power
 * domain (PDn) remains in active state as well as all inferior power domains (<PDn).
 * - Sleepwalking with dynamic power domain gating: During standby sleep mode, a
 * power domain (PDn) in active can wake up a superior power domain (>PDn) in order
 * to perform a sleepwalking task. The superior power domain is then automatically
 * set to active state. At the end of the sleepwalking task, the device can either
 * be woken up or the superior power domain can return to retention state.
 *
 * Power domains can be linked to each other, it allows a power domain (PDn) to be kept
 * in active state if the inferior power domain (PDn-1) is in active state too.
 *
 * \ref asfdoc_sam0_system_power_domain_overview_table illustrates the
 * four cases to consider in standby mode.
 *
 * \anchor asfdoc_sam0_system_power_domain_overview_table
 * <table>
 *  <caption>Sleep Mode versus Power Domain State Overview</caption>
 *  <tr>
 *      <th>Sleep mode</th>
 *      <th>PD0</th>
 *      <th>PD1</th>
 *      <th>PD2</th>
 *      <th>PDTOP</th>
 *      <th>PDBACKUP</th>
 *  </tr>
 *  <tr>
 *      <td>Idle</td>
 *      <td>active</td>
 *      <td>active</td>
 *      <td>active</td>
 *      <td>active</td>
 *      <td>active</td>
 *  </tr>
 *  <tr>
 *      <td>Standby - Case 1</td>
 *      <td>active</td>
 *      <td>active</td>
 *      <td>active</td>
 *      <td>active</td>
 *      <td>active</td>
 *  </tr>
 *  <tr>
 *      <td>Standby - Case 2</td>
 *      <td>active</td>
 *      <td>active</td>
 *      <td>retention</td>
 *      <td>active</td>
 *      <td>active</td>
 *  </tr>
 *  <tr>
 *      <td>Standby - Case 3</td>
 *      <td>active</td>
 *      <td>retention</td>
 *      <td>retention</td>
 *      <td>active</td>
 *      <td>active</td>
 *  </tr>
 *  <tr>
 *      <td>Standby - Case 4</td>
 *      <td>retention</td>
 *      <td>retention</td>
 *      <td>retention</td>
 *      <td>active</td>
 *      <td>active</td>
 *  </tr>
 *  <tr>
 *      <td>Backup</td>
 *      <td>off</td>
 *      <td>off</td>
 *      <td>off</td>
 *      <td>off</td>
 *      <td>active</td>
 *  </tr>
 *  <tr>
 *      <td>Off</td>
 *      <td>off</td>
 *      <td>off</td>
 *      <td>off</td>
 *      <td>off</td>
 *      <td>off</td>
 *  </tr>
 * </table>
 *
 * \subsection asfdoc_sam0_system_module_overview_ram_state RAMs Low Power Mode
 * By default, in standby sleep mode, RAM is in low power mode (back biased)
 * if its power domain is in retention state.
 * \ref asfdoc_sam0_system_power_ram_state_table lists RAMs low power mode.
 *
 * \anchor asfdoc_sam0_system_power_ram_state_table
 * <table>
 *  <caption>RAM Back-biasing Mode</caption>
 *  <tr>
 *      <th>RAM mode</th>
 *      <th>Description</th>
 *  </tr>
 *  <tr>
 *      <td>Retention Back-biasing mode</td>
 *      <td>RAM is back-biased if its power domain is in retention mode</td>
 *  </tr>
 *  <tr>
 *      <td>Standby Back-biasing mode</td>
 *      <td>RAM is back-biased if the device is in standby mode</td>
 *  </tr>
 *  <tr>
 *      <td>Standby OFF mode</td>
 *      <td>RAM is OFF if the device is in standby mode</td>
 *  </tr>
 *  <tr>
 *      <td>Always OFF mode</td>
 *      <td>RAM is OFF if the device is in RET mode</td>
 *  </tr>
 * </table>
 *
 * \endif
 *
 * \subsection asfdoc_sam0_system_module_overview_sleep_mode Sleep Modes
 * The SAM devices have several sleep modes. The sleep mode controls
 * which clock systems on the device will remain enabled or disabled when the
 * device enters a low power sleep mode.
 * \ref asfdoc_sam0_system_module_sleep_mode_table "The table below" lists the
 * clock settings of the different sleep modes.
 *
 * \anchor asfdoc_sam0_system_module_sleep_mode_table
 * <table>
 *  <caption>SAM Device Sleep Modes</caption>
 * \if DEVICE_SAML21_SYSTEM_SUPPORT
 *  <tr>
 *      <th>Sleep mode</th>
 *      <th>System clock</th>
 *      <th>CPU clock</th>
 *      <th>AHB/AHB clock</th>
 *      <th>GCLK clocks</th>
 *      <th>Oscillators (ONDEMAND  = 0)</th>
 *      <th>Oscillators (ONDEMAND  = 1)</th>
 *      <th>Regulator mode</th>
 *      <th>RAM mode</th>
 *  </tr>
 *  <tr>
 *      <td>Idle</td>
 *      <td>Run</td>
 *      <td>Stop</td>
 *      <td>Run if requested</td>
 *      <td>Run</td>
 *      <td>Run</td>
 *      <td>Run if requested</td>
 *      <td>Normal</td>
 *      <td>Normal</td>
 *  </tr>
 *  <tr>
 *      <td>Standby</td>
 *      <td>Stop</td>
 *      <td>Stop</td>
 *      <td>Run if requested</td>
 *      <td>Run if requested</td>
 *      <td>Run if requested or RUNSTDBY  = 1</td>
 *      <td>Run if requested</td>
 *      <td>Low pwer</td>
 *      <td>Low pwer</td>
 *  </tr>
 *  <tr>
 *      <td>Backup</td>
 *      <td>Stop</td>
 *      <td>Stop</td>
 *      <td>Stop</td>
 *      <td>Stop</td>
 *      <td>Stop</td>
 *      <td>Stop</td>
 *      <td>Backup</td>
 *      <td>Off</td>
 *  </tr>
 *  <tr>
 *      <td>Off</td>
 *      <td>Off</td>
 *      <td>Off</td>
 *      <td>Off</td>
 *      <td>Off</td>
 *      <td>Off</td>
 *      <td>Off</td>
 *      <td>Off</td>
 *      <td>Off</td>
 *  </tr>
 * \else
 *  <tr>
 *      <th>Sleep mode</th>
 *      <th>CPU clock</th>
 *      <th>AHB clock</th>
 *      <th>APB clocks</th>
 *      <th>Clock sources</th>
 *      <th>System clock</th>
 *      <th>32KHz</th>
 *      <th>Reg mode</th>
 *      <th>RAM mode</th>
 *  </tr>
 *  <tr>
 *      <td>Idle 0</td>
 *      <td>Stop</td>
 *      <td>Run</td>
 *      <td>Run</td>
 *      <td>Run</td>
 *      <td>Run</td>
 *      <td>Run</td>
 *      <td>Normal</td>
 *      <td>Normal</td>
 *  </tr>
 *  <tr>
 *      <td>Idle 1</td>
 *      <td>Stop</td>
 *      <td>Stop</td>
 *      <td>Run</td>
 *      <td>Run</td>
 *      <td>Run</td>
 *      <td>Run</td>
 *      <td>Normal</td>
 *      <td>Normal</td>
 *  </tr>
 *  <tr>
 *      <td>Idle 2</td>
 *      <td>Stop</td>
 *      <td>Stop</td>
 *      <td>Stop</td>
 *      <td>Run</td>
 *      <td>Run</td>
 *      <td>Run</td>
 *      <td>Normal</td>
 *      <td>Normal</td>
 *  </tr>
 *  <tr>
 *      <td>Standby</td>
 *      <td>Stop</td>
 *      <td>Stop</td>
 *      <td>Stop</td>
 *      <td>Stop</td>
 *      <td>Stop</td>
 *      <td>Stop</td>
 *      <td>Low Power</td>
 *      <td>Source/Drain biasing</td>
 *  </tr>
 * \endif
 * </table>
 *
 * Before entering device sleep, one of the available sleep modes must be set.
 * The device will automatically wake up in response to an interrupt being
 * generated or upon any other sleep mode exit condition.
 *
 * Some peripheral clocks will remain enabled during sleep, depending on their
 * configuration. If desired, the modules can remain clocked during sleep to allow
 * them continue to operate while other parts of the system are powered down
 * to save power.
 *
 *
 * \section asfdoc_sam0_system_special_considerations Special Considerations
 *
 * Most of the functions in this driver have device specific restrictions and
 * caveats; refer to your device datasheet.
 *
 *
 * \section asfdoc_sam0_system_extra_info Extra Information
 *
 * For extra information, see \ref asfdoc_sam0_system_extra. This includes:
 *  - \ref asfdoc_sam0_system_extra_acronyms
 *  - \ref asfdoc_sam0_system_extra_dependencies
 *  - \ref asfdoc_sam0_system_extra_errata
 *  - \ref asfdoc_sam0_system_extra_history
 *
 *
 * \section asfdoc_sam0_system_examples Examples
 *
 * For SYSTEM module related examples, refer to the sub-modules listed in
 * the \ref asfdoc_sam0_system_module_overview "Module Overview".
 *
 * \if DEVICE_SAML21_SYSTEM_SUPPORT
 * For a list of examples related to this driver, see
 * \ref asfdoc_sam0_drivers_power_exqsg.
 * \endif
 *
 *
 * \section asfdoc_sam0_system_api_overview API Overview
 * @{
 */

/**
 * \name System Debugger
 * @{
 */

/**
 * \brief Check if debugger is present.
 *
 * Check if debugger is connected to the onboard debug system (DAP).
 *
 * \return A bool identifying if a debugger is present.
 *
 * \retval true  Debugger is connected to the system
 * \retval false Debugger is not connected to the system
 *
 */
static inline bool system_is_debugger_present(void)
{
	return DSU->STATUSB.reg & DSU_STATUSB_DBGPRES;
}

/**
 * @}
 */

/**
 * \name System Identification
 * @{
 */

/**
 * \brief Retrieve the device identification signature.
 *
 * Retrieves the signature of the current device.
 *
 * \return Device ID signature as a 32-bit integer.
 */
static inline uint32_t system_get_device_id(void)
{
	return DSU->DID.reg;
}

/**
 * @}
 */

/**
 * \name System Initialization
 * @{
 */

void system_init(void);

/**
 * @}
 */


/**
 * @}
 */

/**
 *
 * \if DEVICE_SAML21_SYSTEM_SUPPORT
 * \page asfdoc_sam0_drivers_power_exqsg Examples for SYSTEM Driver
 *
 * This is a list of the available Quick Start Guides (QSGs) and example
 * applications for \ref asfdoc_sam0_system_group. QSGs are simple examples with step-by-step instructions to
 * configure and use this driver in a selection of
 * use cases. Note that a QSG can be compiled as a standalone application or be
 * added to the user application.
 *
 *  - \subpage asfdoc_sam0_power_basic_use_case
 * \endif
 *
 * \page asfdoc_sam0_system_extra Extra Information for SYSTEM Driver
 *
 * \section asfdoc_sam0_system_extra_acronyms Acronyms
 * Below is a table listing the acronyms used in this module, along with their
 * intended meanings.
 *
 * <table>
 *  <tr>
 *      <th>Acronym</th>
 *      <th>Definition</th>
 *  </tr>
 *  <tr>
 *      <td>PM</td>
 *      <td>Power Manager</td>
 *  </tr>
 * \if DEVICE_SAML21_SYSTEM_SUPPORT
 *  <tr>
 *      <td>SUPC</td>
 *      <td>Supply Controller</td>
 *  </tr>
 *  <tr>
 *      <td>RSTC</td>
 *      <td>Reset Controller</td>
 *  </tr>
 * \endif
 * \if DEVICE_SAMC21_SYSTEM_SUPPORT
 *  <tr>
 *      <td>SUPC</td>
 *      <td>Supply Controller</td>
 *  </tr>
 *  <tr>
 *      <td>RSTC</td>
 *      <td>Reset Controller</td>
 *  </tr>
 * \endif
 * \if DEVICE_SAMD21_SYSTEM_SUPPORT
 *  <tr>
 *      <td>SYSCTRL</td>
 *      <td>System control interface</td>
 *  </tr>
 * \endif
 * </table>
 *
 *
 * \section asfdoc_sam0_system_extra_dependencies Dependencies
 * This driver has the following dependencies:
 *
 *  - None
 *
 *
 * \section asfdoc_sam0_system_extra_errata Errata
 * There are no errata related to this driver.
 *
 *
 * \section asfdoc_sam0_system_extra_history Module History
 * An overview of the module history is presented in the table below, with
 * details on the enhancements and fixes made to the module since its first
 * release. The current version of this corresponds to the newest version in
 * the table.
 *
 * <table>
 *  <tr>
 *      <th>Changelog</th>
 *  </tr>
 * \if DEVICE_SAML21_SYSTEM_SUPPORT
 *  <tr>
 *      <td>Initial Release</td>
 *  </tr>
 * \endif
 * \if DEVICE_SAMC21_SYSTEM_SUPPORT
 *  <tr>
 *      <td>Initial Release</td>
 *  </tr>
 * \endif
 * \if DEVICE_SAMD21_SYSTEM_SUPPORT
 *  <tr>
 *      <td>Added new \c system_reset() to reset the complete MCU with some exceptions</td>
 *  </tr>
 *  <tr>
 *      <td>Added new \c system_get_device_id() function to retrieved the device
 *          ID</td>
 *  </tr>
 *  <tr>
 *      <td>Initial Release</td>
 *  </tr>
 * \endif
 * </table>
 *
 * \page asfdoc_sam0_system_document_revision_history Document Revision History
 *
 * <table>
 * <tr>
 *      <th>Doc. Rev.</th>
 *      <th>Date</th>
 *      <th>Comments</th>
 *  </tr>
 * \if DEVICE_SAML21_SYSTEM_SUPPORT
 *  <tr>
 *      <td>42449A</td>
 *      <td>07/2015</td>
 *      <td>Initial document release</td>
 * </tr>
 * \endif
 * \if DEVICE_SAMC21_SYSTEM_SUPPORT
 *  <tr>
 *      <td>42484A</td>
 *      <td>12/2015</td>
 *      <td>Initial document release.</td>
 * </tr>
 * \endif
 * \if DEVICE_SAMD21_SYSTEM_SUPPORT
 *  <tr>
 *      <td>42120E</td>
 *      <td>12/2015</td>
 *      <td>Added support for SAM DA1 and SAM D09</td>
 * </tr>
 *  <tr>
 *      <td>42120D</td>
 *      <td>12/2014</td>
 *      <td>Added support for SAM R21 and SAM D10/D11</td>
 * </tr>
 * <tr>
 *      <td>42120C</td>
 *      <td>01/2014</td>
 *      <td>Added support for SAM D21</td>
 *  </tr>
 *  <tr>
 *      <td>42120B</td>
 *      <td>06/2013</td>
 *      <td>Corrected documentation typos</td>
 *  </tr>
 *  <tr>
 *      <td>42120A</td>
 *      <td>06/2013</td>
 *      <td>Initial document release</td>
 *  </tr>
 * \endif
 * </table>
 */

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_H_INCLUDED */

