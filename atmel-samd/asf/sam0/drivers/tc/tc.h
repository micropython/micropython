/**
 * \file
 *
 * \brief SAM TC - Timer Counter Driver
 *
 * Copyright (C) 2013-2016 Atmel Corporation. All rights reserved.
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

#ifndef TC_H_INCLUDED
#define TC_H_INCLUDED

/**
 * \defgroup asfdoc_sam0_tc_group SAM Timer/Counter (TC) Driver 
 *
 * This driver for Atmel&reg; | SMART ARM&reg;-based microcontrollers provides an interface for the configuration
 * and management of the timer modules within the device, for waveform
 * generation and timing operations. The following driver API modes are covered
 * by this manual:
 *
 *  - Polled APIs
 * \if TC_CALLBACK_MODE
 *  - Callback APIs
 * \endif
 *
 *
 * The following peripheral is used by this module:
 *  - TC (Timer/Counter)
 *
 * The following devices can use this module:
 *  - Atmel | SMART SAM D20/D21
 *  - Atmel | SMART SAM R21
 *  - Atmel | SMART SAM D09/D10/D11
 *  - Atmel | SMART SAM L21/L22
 *  - Atmel | SMART SAM DA1
 *  - Atmel | SMART SAM C20/C21
 *
 * The outline of this documentation is as follows:
 *  - \ref asfdoc_sam0_tc_prerequisites
 *  - \ref asfdoc_sam0_tc_module_overview
 *  - \ref asfdoc_sam0_tc_special_considerations
 *  - \ref asfdoc_sam0_tc_extra_info
 *  - \ref asfdoc_sam0_tc_examples
 *  - \ref asfdoc_sam0_tc_api_overview
 *
 *
 * \section asfdoc_sam0_tc_prerequisites Prerequisites
 *
 * There are no prerequisites for this module.
 *
 *
 * \section asfdoc_sam0_tc_module_overview Module Overview
 *
 * The Timer/Counter (TC) module provides a set of timing and counting related
 * functionality, such as the generation of periodic waveforms, the capturing
 * of a periodic waveform's frequency/duty cycle, and software timekeeping for
 * periodic operations. TC modules can be configured to use an 8-, 16-, or
 * 32-bit counter size.
 *
 * This TC module for the SAM is capable of the following functions:
 *
 * - Generation of PWM signals
 * - Generation of timestamps for events
 * - General time counting
 * - Waveform period capture
 * - Waveform frequency capture
 *
 * \ref asfdoc_sam0_tc_block_diagram "The diagram below" shows the overview
 * of the TC module design.
 *
 * \anchor asfdoc_sam0_tc_block_diagram
 * \image html overview.svg "Basic Overview of the TC Module"
 *
 *
 * \subsection asfdoc_sam0_tc_features Driver Feature Macro Definition
 * <table>
 *  <tr>
 *    <th>Driver Feature Macro</th>
 *    <th>Supported devices</th>
 *  </tr>
 *  <tr>
 *    <td>FEATURE_TC_DOUBLE_BUFFERED</td>
 *    <td>SAM L21/L22/C20/C21</td>
 *  </tr>
 *  <tr>
 *    <td>FEATURE_TC_SYNCBUSY_SCHEME_VERSION_2</td>
 *    <td>SAM L21/L22/C20/C21</td>
 *  </tr>
 *  <tr>
 *    <td>FEATURE_TC_STAMP_PW_CAPTURE</td>
 *    <td>SAM L21/L22/C20/C21</td>
 *  </tr>
 *  <tr>
 *    <td>FEATURE_TC_READ_SYNC</td>
 *    <td>SAM L21/L22/C20/C21</td>
 *  </tr>
 *  <tr>
 *    <td>FEATURE_TC_IO_CAPTURE</td>
 *    <td>SAM L21/L22/C20/C21</td>
 *  </tr>
 *  <tr>
 *    <td>FEATURE_TC_GENERATE_DMA_TRIGGER</td>
 *    <td>SAM L21/L22</td>
 *  </tr>
 * </table>
 * \note The specific features are only available in the driver when the
 * selected device supports those features.
 *
 * \subsection asfdoc_sam0_tc_module_overview_func_desc Functional Description
 * Independent of the configured counter size, each TC module can be set up
 * in one of two different modes; capture and compare.
 *
 * In capture mode, the counter value is stored when a configurable event
 * occurs. This mode can be used to generate timestamps used in event capture,
 * or it can be used for the measurement of a periodic input signal's
 * frequency/duty cycle.
 *
 * In compare mode, the counter value is compared against one or more of the
 * configured channel compare values. When the counter value coincides with a
 * compare value an action can be taken automatically by the module, such as
 * generating an output event or toggling a pin when used for frequency or
 * Pulse Width Modulation (PWM) signal generation.
 *
 * \note The connection of events between modules requires the use of the
 *       \ref asfdoc_sam0_events_group "SAM Event System Driver (EVENTS)"
 *       to route output event of one module to the input event of another.
 *       For more information on event routing, refer to the event driver
 *       documentation.
 *
 * \subsection asfdoc_sam0_tc_module_overview_tc_size Timer/Counter Size
 * Each timer module can be configured in one of three different counter
 * sizes; 8-, 16-, and 32-bit. The size of the counter determines the maximum
 * value it can count to before an overflow occurs and the count is reset back
 * to zero. \ref asfdoc_sam0_tc_count_size_vs_top "The table below" shows the
 * maximum values for each of the possible counter sizes.
 *
 * \anchor asfdoc_sam0_tc_count_size_vs_top
 * <table>
 *  <caption>Timer Counter Sizes and Their Maximum Count Values</caption>
 *  <tr>
 *    <th>Counter size</th>
 *    <th>Max. (hexadecimal)</th>
 *    <th>Max. (decimal)</th>
 *  </tr>
 *  <tr>
 *    <td>8-bit</td>
 *    <td>0xFF</td>
 *    <td>255</td>
 *  </tr>
 *  <tr>
 *    <td>16-bit</td>
 *    <td>0xFFFF</td>
 *    <td>65,535</td>
 *  </tr>
 *  <tr>
 *    <td>32-bit</td>
 *    <td>0xFFFFFFFF</td>
 *    <td>4,294,967,295</td>
 *  </tr>
 * </table>
 *
 * When using the counter in 16- or 32-bit count mode, Compare Capture
 * register 0 (CC0) is used to store the period value when running in PWM
 * generation match mode.
 *
 * When using 32-bit counter size, two 16-bit counters are chained together
 * in a cascade formation. Except in SAM D09/D10/D11. Even numbered TC modules
 * (e.g. TC0, TC2) can be configured as 32-bit counters. The odd numbered
 * counters will act as slaves to the even numbered masters, and will not
 * be reconfigurable until the master timer is disabled. The pairing of timer
 * modules for 32-bit mode is shown in \ref asfdoc_sam0_tc_module_ms_pairs
 * "the table below".
 *
 * \anchor asfdoc_sam0_tc_module_ms_pairs
 * <table>
 *   <caption>TC Master and Slave Module Pairings</caption>
 *   <tr>
 *     <th>Master TC module</th>
 *     <th>Slave TC module</th>
 *   </tr>
 *   <tr>
 *     <td>TC0</td>
 *     <td>TC1</td>
 *   </tr>
 *   <tr>
 *     <td>TC2</td>
 *     <td>TC3</td>
 *   </tr>
 *   <tr>
 *     <td>...</td>
 *     <td>...</td>
 *   </tr>
 *   <tr>
 *     <td>TCn-1</td>
 *     <td>TCn</td>
 *   </tr>
 * </table>
 *
 * In SAM D09/D10/D11, odd numbered TC modules (e.g. TC1) can be configured as 32-bit
 * counters. The even numbered (e.g. TC2) counters will act as slaves to the odd
 * numbered masters.
 *
 * \subsection asfdoc_sam0_tc_module_overview_clock Clock Settings
 *
 * \subsubsection asfdoc_sam0_tc_module_overview_clock_selection Clock Selection
 * Each TC peripheral is clocked asynchronously to the system clock by a GCLK
 * (Generic Clock) channel. The GCLK channel connects to any of the GCLK
 * generators. The GCLK generators are configured to use one of the available
 * clock sources on the system such as internal oscillator, external crystals,
 * etc. See the \ref asfdoc_sam0_system_clock_group "Generic Clock driver"
 *for
 * more information.
 *
 * \subsubsection asfdoc_sam0_tc_module_overview_clock_prescaler Prescaler
 * Each TC module in the SAM has its own individual clock prescaler, which
 * can be used to divide the input clock frequency used in the counter. This
 * prescaler only scales the clock used to provide clock pulses for the counter
 * to count, and does not affect the digital register interface portion of
 * the module, thus the timer registers will synchronize to the raw GCLK
 * frequency input to the module.
 *
 * As a result of this, when selecting a GCLK frequency and timer prescaler
 * value the user application should consider both the timer resolution
 * required and the synchronization frequency, to avoid lengthy
 * synchronization times of the module if a very slow GCLK frequency is fed
 * into the TC module. It is preferable to use a higher module GCLK frequency
 * as the input to the timer, and prescale this down as much as possible to
 * obtain a suitable counter frequency in latency-sensitive applications.
 *
 * \subsubsection asfdoc_sam0_tc_module_overview_clock_reloading Reloading
 * Timer modules also contain a configurable reload action, used when a
 * re-trigger event occurs. Examples of a re-trigger event are the counter
 * reaching the maximum value when counting up, or when an event from the event
 * system tells the counter to re-trigger. The reload action determines if the
 * prescaler should be reset, and when this should happen. The counter will
 * always be reloaded with the value it is set to start counting from. The user
 * can choose between three different reload actions, described in
 * \ref asfdoc_sam0_tc_module_reload_act "the table below".
 *
 * \anchor asfdoc_sam0_tc_module_reload_act
 * <table>
 *   <caption>TC Module Reload Actions</caption>
 *   <tr>
 *     <th>Reload action</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>\ref TC_RELOAD_ACTION_GCLK </td>
 *     <td>Reload TC counter value on next GCLK cycle. Leave prescaler
 *         as-is.</td>
 *   </tr>
 *   <tr>
 *     <td>\ref TC_RELOAD_ACTION_PRESC </td>
 *     <td>Reloads TC counter value on next prescaler clock. Leave prescaler
 *         as-is.</td>
 *   </tr>
 *  <tr>
 *    <td> \ref TC_RELOAD_ACTION_RESYNC </td>
 *    <td>Reload TC counter value on next GCLK cycle. Clear prescaler to
 *        zero.</td>
 *  </tr>
 * </table>
 *
 * The reload action to use will depend on the specific application being
 * implemented. One example is when an external trigger for a reload occurs; if
 * the TC uses the prescaler, the counter in the prescaler should not have a
 * value between zero and the division factor. The TC counter and the counter
 * in the prescaler should both start at zero. When the counter is set to
 * re-trigger when it reaches the maximum value on the other hand, this is not the
 * right option to use. In such a case it would be better if the prescaler is
 * left unaltered when the re-trigger happens, letting the counter reset on the
 * next GCLK cycle.
 *
 * \subsection asfdoc_sam0_tc_module_overview_compare_match Compare Match Operations
 * In compare match operation, Compare/Capture registers are used in comparison
 * with the counter value. When the timer's count value matches the value of a
 * compare channel, a user defined action can be taken.
 *
 * \subsubsection asfdoc_sam0_tc_module_overview_compare_match_timer Basic Timer
 *
 * A Basic Timer is a simple application where compare match operations are used
 * to determine when a specific period has elapsed. In Basic Timer operations,
 * one or more values in the module's Compare/Capture registers are used to
 * specify the time (as a number of prescaled GCLK cycles) when an action should
 * be taken by the microcontroller. This can be an Interrupt Service Routine
 * (ISR), event generator via the event system, or a software flag that is
 * polled via the user application.
 *
 * \subsubsection asfdoc_sam0_tc_module_overview_compare_match_wg Waveform Generation
 *
 * Waveform generation enables the TC module to generate square waves, or if
 * combined with an external passive low-pass filter; analog waveforms.
 *
 * \subsubsection asfdoc_sam0_tc_module_overview_compare_match_wg_pwm Waveform Generation - PWM
 *
 * Pulse width modulation is a form of waveform generation and a signalling
 * technique that can be useful in many situations. When PWM mode is used,
 * a digital pulse train with a configurable frequency and duty cycle can be
 * generated by the TC module and output to a GPIO pin of the device.
 *
 * Often PWM is used to communicate a control or information parameter to an
 * external circuit or component. Differing impedances of the source generator
 * and sink receiver circuits are less of an issue when using PWM compared to
 * using an analog voltage value, as noise will not generally affect the
 * signal's integrity to a meaningful extent.
 *
 * \ref asfdoc_sam0_tc_module_pwm_normal_diag "The figure below" illustrates
 * operations and different states of the counter and its output when running
 * the counter in PWM normal mode. As can be seen, the TOP value is unchanged
 * and is set to MAX. The compare match value is changed at several points to
 * illustrate the resulting waveform output changes. The PWM output is set to
 * normal (i.e. non-inverted) output mode.
 *
 * \anchor asfdoc_sam0_tc_module_pwm_normal_diag
 * \image html pwm_normal_ex.svg "Example of PWM in Normal Mode, and Different Counter Operations"
 *
 *
 * In \ref asfdoc_sam0_tc_module_pwm_match_diag "the figure below", the
 * counter is set to generate PWM in Match mode. The PWM output is inverted via
 * the appropriate configuration option in the TC driver configuration
 * structure. In this example, the counter value is changed once, but the
 * compare match value is kept unchanged. As can be seen, it is possible to
 * change the TOP value when running in PWM match mode.
 *
 * \anchor asfdoc_sam0_tc_module_pwm_match_diag
 * \image html pwm_match_ex.svg "Example of PWM in Match Mode and Different Counter Operations"
 *
 * \subsubsection asfdoc_sam0_tc_module_overview_compare_match_wg_freq Waveform Generation - Frequency
 *
 * Frequency Generation mode is in many ways identical to PWM
 * generation. However, in Frequency Generation a toggle only occurs
 * on the output when a match on a capture channels occurs. When the
 * match is made, the timer value is reset, resulting in a variable
 * frequency square wave with a fixed 50% duty cycle.
 *
 * \subsubsection asfdoc_sam0_tc_module_overview_compare_match_capt Capture Operations
 *
 * In capture operations, any event from the event system or a pin change can
 * trigger a capture of the counter value. This captured counter value can be
 * used as a timestamp for the event, or it can be used in frequency and pulse
 * width capture.
 *
 * \subsubsection asfdoc_sam0_tc_module_overview_compare_match_capt_event_capture Capture Operations - Event
 *
 * Event capture is a simple use of the capture functionality,
 * designed to create timestamps for specific events. When the TC
 * module's input capture pin is externally toggled, the current timer
 * count value is copied into a buffered register which can then be
 * read out by the user application.
 *
 * Note that when performing any capture operation, there is a risk that the
 * counter reaches its top value (MAX) when counting up, or the bottom value
 * (zero) when counting down, before the capture event occurs. This can distort
 * the result, making event timestamps to appear shorter than reality; the
 * user application should check for timer overflow when reading a capture
 * result in order to detect this situation and perform an appropriate
 * adjustment.
 *
 * Before checking for a new capture, \ref TC_STATUS_COUNT_OVERFLOW
 * should be checked. The response to an overflow error is left to the user
 * application, however it may be necessary to clear both the capture overflow
 * flag and the capture flag upon each capture reading.
 *
 * \subsubsection asfdoc_sam0_tc_module_overview_compare_match_capt_pwc Capture Operations - Pulse Width
 *
 * Pulse Width Capture mode makes it possible to measure the pulse width and
 * period of PWM signals. This mode uses two capture channels of the counter.
 * This means that the counter module used for Pulse Width Capture can not be
 * used for any other purpose. There are two modes for pulse width capture;
 * Pulse Width Period (PWP) and Period Pulse Width (PPW). In PWP mode, capture
 * channel 0 is used for storing the pulse width and capture channel 1 stores
 * the observed period. While in PPW mode, the roles of the two capture channels
 * are reversed.
 *
 * As in the above example it is necessary to poll on interrupt flags to see
 * if a new capture has happened and check that a capture overflow error has
 * not occurred.
 *
 * \subsection asfdoc_sam0_tc_module_overview_oneshot One-shot Mode
 *
 * TC modules can be configured into a one-shot mode. When configured in this
 * manner, starting the timer will cause it to count until the next overflow
 * or underflow condition before automatically halting, waiting to be manually
 * triggered by the user application software or an event signal from the event
 * system.
 *
 * \subsubsection asfdoc_sam0_tc_module_overview_inversion Wave Generation Output Inversion
 *
 * The output of the wave generation can be inverted by hardware if desired,
 * resulting in the logically inverted value being output to the configured
 * device GPIO pin.
 *
 *
 * \section asfdoc_sam0_tc_special_considerations Special Considerations
 *
 * The number of capture compare registers in each TC module is dependent on
 * the specific SAM device being used, and in some cases the counter size.
 *
 * The maximum amount of capture compare registers available in any SAM
 * device is two when running in 32-bit mode and four in 8- and 16-bit modes.
 *
 *
 * \section asfdoc_sam0_tc_extra_info Extra Information
 *
 * For extra information, see \ref asfdoc_sam0_tc_extra. This includes:
 *  - \ref asfdoc_sam0_tc_extra_acronyms
 *  - \ref asfdoc_sam0_tc_extra_dependencies
 *  - \ref asfdoc_sam0_tc_extra_errata
 *  - \ref asfdoc_sam0_tc_extra_history
 *
 *
 * \section asfdoc_sam0_tc_examples Examples
 *
 * For a list of examples related to this driver, see
 * \ref asfdoc_sam0_tc_exqsg.
 *
 * \section asfdoc_sam0_tc_api_overview API Overview
 * @{
 */

#include <compiler.h>
#include <clock.h>
#include <gclk.h>
#include <pinmux.h>

/**
 * Define port features set according to different device family
 * @{
*/
#if (SAML21) || (SAML22) || (SAMC20) || (SAMC21) || (SAMR30) || defined(__DOXYGEN__)
/** TC double buffered. */
#  define FEATURE_TC_DOUBLE_BUFFERED
/** SYNCBUSY scheme version 2. */
#  define FEATURE_TC_SYNCBUSY_SCHEME_VERSION_2
/** TC time stamp capture and pulse width capture. */
#  define FEATURE_TC_STAMP_PW_CAPTURE
/** Read synchronization of COUNT. */
#  define FEATURE_TC_READ_SYNC
/** I/O pin edge capture. */
#  define FEATURE_TC_IO_CAPTURE
#endif

#if (SAML21XXXB) || (SAMR30) || defined(__DOXYGEN__)
/** Generate Direct Memory Access (DMA) triggers. */
#  define FEATURE_TC_GENERATE_DMA_TRIGGER
#endif
/*@}*/

#if !defined(__DOXYGEN__)
#if SAMD20 || SAML21 || SAML22 || SAMC20 || SAMC21 || SAMR30
#  define TC_INSTANCE_OFFSET 0
#endif
#if SAMD21 || SAMR21 || SAMDA1
#  define TC_INSTANCE_OFFSET 3
#endif
#if SAMD09 || SAMD10 || SAMD11
#  define TC_INSTANCE_OFFSET 1
#endif

#if SAMD20
#  define NUMBER_OF_COMPARE_CAPTURE_CHANNELS TC0_CC8_NUM
#elif SAML21 || SAML22 || SAMC20 || SAMC21 || SAMR30
#  define NUMBER_OF_COMPARE_CAPTURE_CHANNELS TC0_CC_NUM
#elif SAMD09 || SAMD10 || SAMD11
#  define NUMBER_OF_COMPARE_CAPTURE_CHANNELS TC1_CC8_NUM
#else
#  define NUMBER_OF_COMPARE_CAPTURE_CHANNELS TC3_CC8_NUM
   /* Same number for 8-, 16- or 32-bit TC and all TC instances */
#endif

/** TC Instance MAX ID Number. */
#if SAMD20E || SAMD20G || SAMD21G || SAMD21E || SAMR21
#  if SAMD21GXXL
#    define TC_INST_MAX_ID  7
#  else
#    define TC_INST_MAX_ID  5
#  endif
#elif SAML21 || SAMC20 || SAMC21 || SAMR30
#  define TC_INST_MAX_ID  4
#elif SAML22
#  define TC_INST_MAX_ID  3
#elif SAMD09 || SAMD10 || SAMD11
#  define TC_INST_MAX_ID  2
#else
#  define TC_INST_MAX_ID  7
#endif

#endif

#if TC_ASYNC == true
#  include <system_interrupt.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if TC_ASYNC == true
/** Enum for the possible callback types for the TC module. */
enum tc_callback {
	/** Callback for TC overflow */
	TC_CALLBACK_OVERFLOW,
	/** Callback for capture overflow error */
	TC_CALLBACK_ERROR,
	/** Callback for capture compare channel 0 */
	TC_CALLBACK_CC_CHANNEL0,
	/** Callback for capture compare channel 1 */
	TC_CALLBACK_CC_CHANNEL1,
#  if !defined(__DOXYGEN__)
	/** Number of available callbacks */
	TC_CALLBACK_N,
#  endif
};
#endif

/**
 * \name Module Status Flags
 *
 * TC status flags, returned by \ref tc_get_status() and cleared by
 * \ref tc_clear_status().
 *
 * @{
 */

/** Timer channel 0 has matched against its compare value, or has captured a
 *  new value.
 */
#define TC_STATUS_CHANNEL_0_MATCH    (1UL << 0)

/** Timer channel 1 has matched against its compare value, or has captured a
 *  new value.
 */
#define TC_STATUS_CHANNEL_1_MATCH    (1UL << 1)

/** Timer register synchronization has completed, and the synchronized count
 *  value may be read.
 */
#define TC_STATUS_SYNC_READY         (1UL << 2)

/** A new value was captured before the previous value was read, resulting in
 *  lost data.
 */
#define TC_STATUS_CAPTURE_OVERFLOW   (1UL << 3)

/** The timer count value has overflowed from its maximum value to its minimum
 *  when counting upward, or from its minimum value to its maximum when
 *  counting downward.
 */
#define TC_STATUS_COUNT_OVERFLOW     (1UL << 4)

#ifdef FEATURE_TC_DOUBLE_BUFFERED
/** Channel 0 compare or capture buffer valid. */
#define TC_STATUS_CHN0_BUFFER_VALID     (1UL << 5)
/** Channel 1 compare or capture buffer valid. */
#define TC_STATUS_CHN1_BUFFER_VALID     (1UL << 6)
/** Period buffer valid. */
#define TC_STATUS_PERIOD_BUFFER_VALID     (1UL << 7)
#endif
/** @} */

/**
 * \brief Index of the compare capture channels.
 *
 * This enum is used to specify which capture/compare channel to do
 * operations on.
 */
enum tc_compare_capture_channel {
	/** Index of compare capture channel 0 */
	TC_COMPARE_CAPTURE_CHANNEL_0,
	/** Index of compare capture channel 1 */
	TC_COMPARE_CAPTURE_CHANNEL_1,
};

/**
 * \name TC Wave Generation Mode
 *
 * @{
 */
#if SAML21 || SAML22 || SAMC20 || SAMC21 || SAMR30
/** TC wave generation mode: normal frequency. */
#define TC_WAVE_GENERATION_NORMAL_FREQ_MODE TC_WAVE_WAVEGEN_NFRQ
/** TC wave generation mode: match frequency. */
#define TC_WAVE_GENERATION_MATCH_FREQ_MODE  TC_WAVE_WAVEGEN_MFRQ
/** TC wave generation mode: normal PWM. */
#define TC_WAVE_GENERATION_NORMAL_PWM_MODE  TC_WAVE_WAVEGEN_NPWM
/** TC wave generation mode: match PWM. */
#define TC_WAVE_GENERATION_MATCH_PWM_MODE   TC_WAVE_WAVEGEN_MPWM
#else
/** TC wave generation mode: normal frequency. */
#define TC_WAVE_GENERATION_NORMAL_FREQ_MODE TC_CTRLA_WAVEGEN_NFRQ
/** TC wave generation mode: match frequency. */
#define TC_WAVE_GENERATION_MATCH_FREQ_MODE  TC_CTRLA_WAVEGEN_MFRQ
/** TC wave generation mode: normal PWM. */
#define TC_WAVE_GENERATION_NORMAL_PWM_MODE  TC_CTRLA_WAVEGEN_NPWM
/** TC wave generation mode: match PWM. */
#define TC_WAVE_GENERATION_MATCH_PWM_MODE   TC_CTRLA_WAVEGEN_MPWM
#endif
/** @} */

/**
 * \brief TC wave generation mode enum.
 *
 * This enum is used to select which mode to run the wave
 * generation in.
 *
 */
enum tc_wave_generation {
	/** Top is maximum, except in 8-bit counter size where it is the PER
	 * register
	 */
	TC_WAVE_GENERATION_NORMAL_FREQ      = TC_WAVE_GENERATION_NORMAL_FREQ_MODE,

	/** Top is CC0, except in 8-bit counter size where it is the PER
	 * register
	 */
	TC_WAVE_GENERATION_MATCH_FREQ       = TC_WAVE_GENERATION_MATCH_FREQ_MODE,

	/** Top is maximum, except in 8-bit counter size where it is the PER
	 * register
	 */
	TC_WAVE_GENERATION_NORMAL_PWM       = TC_WAVE_GENERATION_NORMAL_PWM_MODE,

	/** Top is CC0, except in 8-bit counter size where it is the PER
	 * register
	 */
	TC_WAVE_GENERATION_MATCH_PWM        = TC_WAVE_GENERATION_MATCH_PWM_MODE,
};

/**
 * \brief Specifies if the counter is 8-, 16-, or 32-bit.
 *
 * This enum specifies the maximum value it is possible to count to.
 */
enum tc_counter_size {
	/** The counter's maximum value is 0xFF, the period register is
	 * available to be used as top value
	 */
	TC_COUNTER_SIZE_8BIT                = TC_CTRLA_MODE_COUNT8,

	/** The counter's maximum value is 0xFFFF. There is no separate
	 * period register, to modify top one of the capture compare
	 * registers has to be used. This limits the amount of
	 * available channels.
	 */
	TC_COUNTER_SIZE_16BIT               = TC_CTRLA_MODE_COUNT16,

	/** The counter's maximum value is 0xFFFFFFFF. There is no separate
	 * period register, to modify top one of the capture compare
	 * registers has to be used. This limits the amount of
	 * available channels.
	 */
	TC_COUNTER_SIZE_32BIT               = TC_CTRLA_MODE_COUNT32,
};

/**
 * \brief TC Counter reload action enum.
 *
 * This enum specify how the counter and prescaler should reload.
 */
enum tc_reload_action {
	/** The counter is reloaded/reset on the next GCLK and starts
	 * counting on the prescaler clock
	 */
	TC_RELOAD_ACTION_GCLK               = TC_CTRLA_PRESCSYNC_GCLK,

	/** The counter is reloaded/reset on the next prescaler clock
	 */
	TC_RELOAD_ACTION_PRESC              = TC_CTRLA_PRESCSYNC_PRESC,

	/** The counter is reloaded/reset on the next GCLK, and the
	 * prescaler is restarted as well
	 */
	TC_RELOAD_ACTION_RESYNC             = TC_CTRLA_PRESCSYNC_RESYNC,
};

/**
 * \brief TC clock prescaler values.
 *
 * This enum is used to choose the clock prescaler
 * configuration. The prescaler divides the clock frequency of the TC
 * module to make the counter count slower.
 */
enum tc_clock_prescaler {
	/** Divide clock by 1 */
	TC_CLOCK_PRESCALER_DIV1             = TC_CTRLA_PRESCALER(0),
	/** Divide clock by 2 */
	TC_CLOCK_PRESCALER_DIV2             = TC_CTRLA_PRESCALER(1),
	/** Divide clock by 4 */
	TC_CLOCK_PRESCALER_DIV4             = TC_CTRLA_PRESCALER(2),
	/** Divide clock by 8 */
	TC_CLOCK_PRESCALER_DIV8             = TC_CTRLA_PRESCALER(3),
	/** Divide clock by 16 */
	TC_CLOCK_PRESCALER_DIV16            = TC_CTRLA_PRESCALER(4),
	/** Divide clock by 64 */
	TC_CLOCK_PRESCALER_DIV64            = TC_CTRLA_PRESCALER(5),
	/** Divide clock by 256 */
	TC_CLOCK_PRESCALER_DIV256           = TC_CTRLA_PRESCALER(6),
	/** Divide clock by 1024 */
	TC_CLOCK_PRESCALER_DIV1024          = TC_CTRLA_PRESCALER(7),
};

/**
 * \brief TC module count direction.
 *
 * Timer/Counter count direction.
 */
enum tc_count_direction {
	/** Timer should count upward from zero to MAX */
	TC_COUNT_DIRECTION_UP,

	/** Timer should count downward to zero from MAX */
	TC_COUNT_DIRECTION_DOWN,
};

/**
 * \name Waveform Inversion Mode
 *
 * @{
 */
#if SAML21 || SAML22 || SAMC20 || SAMC21 || SAMR30
/** Waveform inversion CC0 mode. */
#define TC_WAVEFORM_INVERT_CC0_MODE  TC_DRVCTRL_INVEN(1)
/** Waveform inversion CC1 mode. */
#define TC_WAVEFORM_INVERT_CC1_MODE  TC_DRVCTRL_INVEN(2)
#else
/** Waveform inversion CC0 mode. */
#define TC_WAVEFORM_INVERT_CC0_MODE  TC_CTRLC_INVEN(1)
/** Waveform inversion CC1 mode. */
#define TC_WAVEFORM_INVERT_CC1_MODE  TC_CTRLC_INVEN(2)
#endif

/**
 * \brief Waveform inversion mode.
 *
 * Output waveform inversion mode.
 */
enum tc_waveform_invert_output {
	/** No inversion of the waveform output */
	TC_WAVEFORM_INVERT_OUTPUT_NONE      = 0,
	/** Invert output from compare channel 0 */
	TC_WAVEFORM_INVERT_OUTPUT_CHANNEL_0 = TC_WAVEFORM_INVERT_CC0_MODE,
	/** Invert output from compare channel 1 */
	TC_WAVEFORM_INVERT_OUTPUT_CHANNEL_1 = TC_WAVEFORM_INVERT_CC1_MODE,
};

/**
 * \brief Action to perform when the TC module is triggered by an event.
 *
 * Event action to perform when the module is triggered by an event.
 */
enum tc_event_action {
	/** No event action */
	TC_EVENT_ACTION_OFF                 = TC_EVCTRL_EVACT_OFF,
	/** Re-trigger on event */
	TC_EVENT_ACTION_RETRIGGER           = TC_EVCTRL_EVACT_RETRIGGER,
	/** Increment counter on event */
	TC_EVENT_ACTION_INCREMENT_COUNTER   = TC_EVCTRL_EVACT_COUNT,
	/** Start counter on event */
	TC_EVENT_ACTION_START               = TC_EVCTRL_EVACT_START,

	/** Store period in capture register 0, pulse width in capture
	 *  register 1
	 */
	TC_EVENT_ACTION_PPW                 = TC_EVCTRL_EVACT_PPW,

	/** Store pulse width in capture register 0, period in capture
	 *  register 1
	 */
	TC_EVENT_ACTION_PWP                 = TC_EVCTRL_EVACT_PWP,
#ifdef FEATURE_TC_STAMP_PW_CAPTURE
	/** Time stamp capture */
	TC_EVENT_ACTION_STAMP               = TC_EVCTRL_EVACT_STAMP,
	/** Pulse width capture */
	TC_EVENT_ACTION_PW                  = TC_EVCTRL_EVACT_PW,
#endif
};

/**
 * \brief TC event enable/disable structure.
 *
 * Event flags for the \ref tc_enable_events() and \ref tc_disable_events().
 */
struct tc_events {
	/** Generate an output event on a compare channel match */
	bool generate_event_on_compare_channel
			[NUMBER_OF_COMPARE_CAPTURE_CHANNELS];
	/** Generate an output event on counter overflow */
	bool generate_event_on_overflow;
	/** Perform the configured event action when an incoming event is signalled */
	bool on_event_perform_action;
	/** Specifies if the input event source is inverted, when used in PWP or
	 *  PPW event action modes
	 */
	bool invert_event_input;
	/** Specifies which event to trigger if an event is triggered */
	enum tc_event_action event_action;
};

/**
 * \brief Configuration struct for TC module in 8-bit size counter mode.
 */
struct tc_8bit_config {
	/** Initial timer count value */
	uint8_t value;
	/** Where to count to or from depending on the direction on the counter */
	uint8_t period;
	/** Value to be used for compare match on each channel */
	uint8_t compare_capture_channel[NUMBER_OF_COMPARE_CAPTURE_CHANNELS];
};

/**
 * \brief Configuration struct for TC module in 16-bit size counter mode.
 */
struct tc_16bit_config {
	/** Initial timer count value */
	uint16_t value;
	/** Value to be used for compare match on each channel */
	uint16_t compare_capture_channel[NUMBER_OF_COMPARE_CAPTURE_CHANNELS];
};

/**
 * \brief Configuration struct for TC module in 32-bit size counter mode.
 */
struct tc_32bit_config {
	/** Initial timer count value */
	uint32_t value;
	/** Value to be used for compare match on each channel */
	uint32_t compare_capture_channel[NUMBER_OF_COMPARE_CAPTURE_CHANNELS];
};

/**
 * \brief Configuration struct for TC module in 32-bit size counter mode.
 */
struct tc_pwm_channel {
	/** When \c true, PWM output for the given channel is enabled */
	bool enabled;
	/** Specifies pin output for each channel */
	uint32_t pin_out;
	/** Specifies Multiplexer (MUX) setting for each output channel pin */
	uint32_t pin_mux;
};

/**
 * \brief TC configuration structure.
 *
 * Configuration struct for a TC instance. This structure should be
 * initialized by the \ref tc_get_config_defaults function before being
 * modified by the user application.
 */
struct tc_config {
	/** GCLK generator used to clock the peripheral */
	enum gclk_generator clock_source;

	/** When \c true the module is enabled during standby */
	bool run_in_standby;
#if (SAML21) || (SAML22) || (SAMC20) || (SAMC21) || (SAMR30)
	/** Run on demand */
	bool on_demand;
#endif
	/** Specifies either 8-, 16-, or 32-bit counter size */
	enum tc_counter_size counter_size;
	/** Specifies the prescaler value for GCLK_TC */
	enum tc_clock_prescaler clock_prescaler;
	/** Specifies which waveform generation mode to use */
	enum tc_wave_generation wave_generation;

	/** Specifies the reload or reset time of the counter and prescaler
	 *  resynchronization on a re-trigger event for the TC
	 */
	enum tc_reload_action reload_action;

	/** Specifies which channel(s) to invert the waveform on.
		For SAM L21/L22/C20/C21, it's also used to invert I/O input pin. */
	uint8_t waveform_invert_output;

	/** Specifies which channel(s) to enable channel capture
	 *  operation on
	 */
	bool enable_capture_on_channel[NUMBER_OF_COMPARE_CAPTURE_CHANNELS];
#ifdef 	FEATURE_TC_IO_CAPTURE
	/** Specifies which channel(s) to enable I/O capture
	 *  operation on
	 */
	bool enable_capture_on_IO[NUMBER_OF_COMPARE_CAPTURE_CHANNELS];
#endif

	/** When \c true, one-shot will stop the TC on next hardware or software
	 *  re-trigger event or overflow/underflow
	 */
	bool oneshot;

	/** Specifies the direction for the TC to count */
	enum tc_count_direction count_direction;

	/** Specifies the PWM channel for TC */
	struct tc_pwm_channel pwm_channel[NUMBER_OF_COMPARE_CAPTURE_CHANNELS];

	/** Access the different counter size settings through this configuration member. */
	union {
		/** Struct for 8-bit specific timer configuration */
		struct tc_8bit_config counter_8_bit;
		/** Struct for 16-bit specific timer configuration */
		struct tc_16bit_config counter_16_bit;
		/** Struct for 32-bit specific timer configuration */
		struct tc_32bit_config counter_32_bit;
	};

#ifdef FEATURE_TC_DOUBLE_BUFFERED
	/** Set to \c true to enable double buffering write. When enabled any write
	 *  through \ref tc_set_top_value(), \ref tc_set_compare_value() and
	 *  will direct to the buffer register as buffered
	 *  value, and the buffered value will be committed to effective register
	 *  on UPDATE condition, if update is not locked.
	 */
	bool double_buffering_enabled;
#endif
};

#if TC_ASYNC == true
/** Forward Declaration for the device instance. */
struct tc_module;

/** Type of the callback functions. */
typedef void (*tc_callback_t)(struct tc_module *const module);
#endif

/**
 * \brief TC software device instance structure.
 *
 * TC software instance structure, used to retain software state information
 * of an associated hardware module instance.
 *
 * \note The fields of this structure should not be altered by the user
 *       application; they are reserved for module-internal use only.
 */
struct tc_module {
#if !defined(__DOXYGEN__)
	/** Hardware module pointer of the associated Timer/Counter peripheral */
	Tc *hw;

	/** Size of the initialized Timer/Counter module configuration */
	enum tc_counter_size counter_size;
#  if TC_ASYNC == true
	/** Array of callbacks */
	tc_callback_t callback[TC_CALLBACK_N];
	/** Bit mask for callbacks registered */
	uint8_t register_callback_mask;
	/** Bit mask for callbacks enabled */
	uint8_t enable_callback_mask;
#  endif
#ifdef FEATURE_TC_DOUBLE_BUFFERED
	/** Set to \c true to enable double buffering write */
	bool double_buffering_enabled;
#endif
#endif
};

#if !defined(__DOXYGEN__)
uint8_t _tc_get_inst_index(
		Tc *const hw);
#endif

/**
 * \name Driver Initialization and Configuration
 * @{
 */

/**
 * \brief Determines if the hardware module(s) are currently synchronizing to
 * the bus.
 *
 * Checks to see if the underlying hardware peripheral module(s) are currently
 * synchronizing across multiple clock domains to the hardware bus. This
 * function can be used to delay further operations on a module until such time
 * that it is ready, to prevent blocking delays for synchronization in the
 * user application.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 *
 * \return Synchronization status of the underlying hardware module(s).
 *
 * \retval false If the module has completed synchronization
 * \retval true  If the module synchronization is ongoing
 */
static inline bool tc_is_syncing(
		const struct tc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	TcCount8 *const tc_module = &(module_inst->hw->COUNT8);

#if (SAML21) || (SAML22) || (SAMC20) || (SAMC21) || (SAMR30)
	return (tc_module->SYNCBUSY.reg);
#else
	return (tc_module->STATUS.reg & TC_STATUS_SYNCBUSY);
#endif
}

/**
 * \brief Initializes config with predefined default values.
 *
 * This function will initialize a given TC configuration structure to
 * a set of known default values. This function should be called on
 * any new instance of the configuration structures before being
 * modified by the user application.
 *
 * The default configuration is as follows:
 *  \li GCLK generator 0 (GCLK main) clock source
 *  \li 16-bit counter size on the counter
 *  \li No prescaler
 *  \li Normal frequency wave generation
 *  \li GCLK reload action
 *  \li Don't run in standby
 *  \li Don't run on demand for SAM L21/L22/C20/C21
 *  \li No inversion of waveform output
 *  \li No capture enabled
 *  \li No I/O capture enabled for SAM L21/L22/C20/C21
 *  \li No event input enabled
 *  \li Count upward
 *  \li Don't perform one-shot operations
 *  \li No event action
 *  \li No channel 0 PWM output
 *  \li No channel 1 PWM output
 *  \li Counter starts on 0
 *  \li Capture compare channel 0 set to 0
 *  \li Capture compare channel 1 set to 0
 *  \li No PWM pin output enabled
 *  \li Pin and MUX configuration not set
 *  \li Double buffer disabled (if have this feature)
 *
 * \param[out]  config  Pointer to a TC module configuration structure to set
 */
static inline void tc_get_config_defaults(
		struct tc_config *const config)
{
	/* Sanity check arguments */
	Assert(config);

	/* Write default config to config struct */
	config->clock_source               = GCLK_GENERATOR_0;
	config->counter_size               = TC_COUNTER_SIZE_16BIT;
	config->clock_prescaler            = TC_CLOCK_PRESCALER_DIV1;
	config->wave_generation            = TC_WAVE_GENERATION_NORMAL_FREQ;
	config->reload_action              = TC_RELOAD_ACTION_GCLK;
	config->run_in_standby             = false;
#if (SAML21) || (SAML22) || (SAMC20) || (SAMC21) || (SAMR30)
	config->on_demand                  = false;
#endif
	config->waveform_invert_output     = TC_WAVEFORM_INVERT_OUTPUT_NONE;
	config->enable_capture_on_channel[TC_COMPARE_CAPTURE_CHANNEL_0] = false;
	config->enable_capture_on_channel[TC_COMPARE_CAPTURE_CHANNEL_1] = false;
#ifdef 	FEATURE_TC_IO_CAPTURE
	config->enable_capture_on_IO[TC_COMPARE_CAPTURE_CHANNEL_0] = false;
	config->enable_capture_on_IO[TC_COMPARE_CAPTURE_CHANNEL_1] = false;
#endif

	config->count_direction            = TC_COUNT_DIRECTION_UP;
	config->oneshot                    = false;

	config->pwm_channel[TC_COMPARE_CAPTURE_CHANNEL_0].enabled = false;
	config->pwm_channel[TC_COMPARE_CAPTURE_CHANNEL_0].pin_out = 0;
	config->pwm_channel[TC_COMPARE_CAPTURE_CHANNEL_0].pin_mux = 0;

	config->pwm_channel[TC_COMPARE_CAPTURE_CHANNEL_1].enabled = false;
	config->pwm_channel[TC_COMPARE_CAPTURE_CHANNEL_1].pin_out = 0;
	config->pwm_channel[TC_COMPARE_CAPTURE_CHANNEL_1].pin_mux = 0;

	config->counter_16_bit.value                   = 0x0000;
	config->counter_16_bit.compare_capture_channel\
		[TC_COMPARE_CAPTURE_CHANNEL_0]                        = 0x0000;
	config->counter_16_bit.compare_capture_channel\
		[TC_COMPARE_CAPTURE_CHANNEL_1]                        = 0x0000;
#ifdef FEATURE_TC_DOUBLE_BUFFERED
	config->double_buffering_enabled = false;
#endif

}

enum status_code tc_init(
		struct tc_module *const module_inst,
		Tc *const hw,
		const struct tc_config *const config);

/** @} */

/**
 * \name Event Management
 * @{
 */

/**
 * \brief Enables a TC module event input or output.
 *
 * Enables one or more input or output events to or from the TC module.
 * See \ref tc_events for a list of events this module supports.
 *
 * \note Events cannot be altered while the module is enabled.
 *
 * \param[in]  module_inst  Pointer to the software module instance struct
 * \param[in]  events       Struct containing flags of events to enable
 */
static inline void tc_enable_events(
		struct tc_module *const module_inst,
		struct tc_events *const events)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);
	Assert(events);

	Tc *const tc_module = module_inst->hw;

	uint32_t event_mask = 0;

	if (events->invert_event_input == true) {
		event_mask |= TC_EVCTRL_TCINV;
	}

	if (events->on_event_perform_action == true) {
		event_mask |= TC_EVCTRL_TCEI;
	}

	if (events->generate_event_on_overflow == true) {
		event_mask |= TC_EVCTRL_OVFEO;
	}

	for (uint8_t i = 0; i < NUMBER_OF_COMPARE_CAPTURE_CHANNELS; i++) {
		if (events->generate_event_on_compare_channel[i] == true) {
			event_mask |= (TC_EVCTRL_MCEO(1) << i);
		}
	}

	tc_module->COUNT8.EVCTRL.reg |= event_mask | events->event_action;
}

/**
 * \brief Disables a TC module event input or output.
 *
 * Disables one or more input or output events to or from the TC module.
 * See \ref tc_events for a list of events this module supports.
 *
 * \note Events cannot be altered while the module is enabled.
 *
 * \param[in]  module_inst  Pointer to the software module instance struct
 * \param[in]  events       Struct containing flags of events to disable
 */
static inline void tc_disable_events(
		struct tc_module *const module_inst,
		struct tc_events *const events)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);
	Assert(events);

	Tc *const tc_module = module_inst->hw;

	uint32_t event_mask = 0;

	if (events->invert_event_input == true) {
		event_mask |= TC_EVCTRL_TCINV;
	}

	if (events->on_event_perform_action == true) {
		event_mask |= TC_EVCTRL_TCEI;
	}

	if (events->generate_event_on_overflow == true) {
		event_mask |= TC_EVCTRL_OVFEO;
	}

	for (uint8_t i = 0; i < NUMBER_OF_COMPARE_CAPTURE_CHANNELS; i++) {
		if (events->generate_event_on_compare_channel[i] == true) {
			event_mask |= (TC_EVCTRL_MCEO(1) << i);
		}
	}

	tc_module->COUNT8.EVCTRL.reg &= ~event_mask;
}

/** @} */

/**
 * \name Enable/Disable/Reset
 * @{
 */

enum status_code tc_reset(
		const struct tc_module *const module_inst);

/**
 * \brief Enable the TC module.
 *
 * Enables a TC module that has been previously initialized. The counter will
 * start when the counter is enabled.
 *
 * \note When the counter is configured to re-trigger on an event, the counter
 *       will not start until the start function is used.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 */
static inline void tc_enable(
		const struct tc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	TcCount8 *const tc_module = &(module_inst->hw->COUNT8);

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}

	/* Enable TC module */
	tc_module->CTRLA.reg |= TC_CTRLA_ENABLE;
}

/**
 * \brief Disables the TC module.
 *
 * Disables a TC module and stops the counter.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 */
static inline void tc_disable(
		const struct tc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	TcCount8 *const tc_module = &(module_inst->hw->COUNT8);

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}

	/* Disbale interrupt */
	tc_module->INTENCLR.reg = TC_INTENCLR_MASK;
	/* Clear interrupt flag */
	tc_module->INTFLAG.reg = TC_INTFLAG_MASK;

	/* Disable TC module */
	tc_module->CTRLA.reg  &= ~TC_CTRLA_ENABLE;
}

/** @} */

/**
 * \name Get/Set Count Value
 * @{
 */

uint32_t tc_get_count_value(
		const struct tc_module *const module_inst);

enum status_code tc_set_count_value(
		const struct tc_module *const module_inst,
		const uint32_t count);

/** @} */

/**
 * \name Start/Stop Counter
 * @{
 */

/**
 * \brief Stops the counter.
 *
 * This function will stop the counter. When the counter is stopped
 * the value in the count value is set to 0 if the counter was
 * counting up, or maximum if the counter was counting
 * down when stopped.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 */
static inline void tc_stop_counter(
		const struct tc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	TcCount8 *const tc_module = &(module_inst->hw->COUNT8);

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}

	/* Write command to execute */
	tc_module->CTRLBSET.reg = TC_CTRLBSET_CMD(TC_CTRLBSET_CMD_STOP_Val);
}

/**
 * \brief Starts the counter.
 *
 * Starts or restarts an initialized TC module's counter.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 */
static inline void tc_start_counter(
		const struct tc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	TcCount8 *const tc_module = &(module_inst->hw->COUNT8);

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}

	/* Make certain that there are no conflicting commands in the register */
	tc_module->CTRLBCLR.reg = TC_CTRLBCLR_CMD_NONE;

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}

	/* Write command to execute */
	tc_module->CTRLBSET.reg = TC_CTRLBSET_CMD(TC_CTRLBSET_CMD_RETRIGGER_Val);
}

/** @} */

#ifdef FEATURE_TC_DOUBLE_BUFFERED
/**
 * \name Double Buffering
 * @{
 */

/**
 * \brief Update double buffer.
 *
 * Update double buffer.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 */
static inline void tc_update_double_buffer(
		const struct tc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	TcCount8 *const tc_module = &(module_inst->hw->COUNT8);

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}

	/* Make certain that there are no conflicting commands in the register */
	tc_module->CTRLBCLR.reg = TC_CTRLBCLR_CMD_NONE;

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}

	/* Write command to execute */
	tc_module->CTRLBSET.reg = TC_CTRLBSET_CMD(TC_CTRLBSET_CMD_UPDATE_Val);
}
/** @} */
#endif

#ifdef FEATURE_TC_READ_SYNC
/**
 * \name Count Read Synchronization
 * @{
 */

/**
 * \brief Read synchronization of COUNT.
 *
 * Read synchronization of COUNT.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 */
static inline void tc_sync_read_count(
		const struct tc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	TcCount8 *const tc_module = &(module_inst->hw->COUNT8);

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}

	/* Make certain that there are no conflicting commands in the register */
	tc_module->CTRLBCLR.reg = TC_CTRLBCLR_CMD_NONE;

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}

	/* Write command to execute */
	tc_module->CTRLBSET.reg = TC_CTRLBSET_CMD(TC_CTRLBSET_CMD_READSYNC_Val);
}
/** @} */
#endif

#ifdef FEATURE_TC_GENERATE_DMA_TRIGGER
/**
 * \name Generate TC DMA Triggers Command
 * @{
 */

/**
 * \brief TC DMA Trigger.
 *
 * TC DMA trigger command.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 */
static inline void tc_dma_trigger_command(
		const struct tc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	TcCount8 *const tc_module = &(module_inst->hw->COUNT8);

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}

	/* Make certain that there are no conflicting commands in the register */
	tc_module->CTRLBCLR.reg = TC_CTRLBCLR_CMD_NONE;

	while (tc_is_syncing(module_inst)) {
		/* Wait for sync */
	}

#if (SAMC20) || (SAMC21) || (SAML22) || (SAML21XXXB) || (SAMR30)
	/* Write command to execute */
	tc_module->CTRLBSET.reg = TC_CTRLBSET_CMD(TC_CTRLBSET_CMD_DMAOS_Val);
#endif
}
/** @} */
#endif

/**
 * \name Get Capture Set Compare
 * @{
 */

uint32_t tc_get_capture_value(
		const struct tc_module *const module_inst,
		const enum tc_compare_capture_channel channel_index);

enum status_code tc_set_compare_value(
		const struct tc_module *const module_inst,
		const enum tc_compare_capture_channel channel_index,
		const uint32_t compare_value);

/** @} */

/**
 * \name Set Top Value
 * @{
 */

enum status_code tc_set_top_value(
		const struct tc_module *const module_inst,
		const uint32_t top_value);

/** @} */

/**
 * \name Status Management
 * @{
 */

/**
 * \brief Retrieves the current module status.
 *
 * Retrieves the status of the module, giving overall state information.
 *
 * \param[in] module_inst  Pointer to the TC software instance struct
 *
 * \return Bitmask of \c TC_STATUS_* flags.
 *
 * \retval TC_STATUS_CHANNEL_0_MATCH   Timer channel 0 compare/capture match
 * \retval TC_STATUS_CHANNEL_1_MATCH   Timer channel 1 compare/capture match
 * \retval TC_STATUS_SYNC_READY        Timer read synchronization has completed
 * \retval TC_STATUS_CAPTURE_OVERFLOW  Timer capture data has overflowed
 * \retval TC_STATUS_COUNT_OVERFLOW    Timer count value has overflowed
 * \retval TC_STATUS_CHN0_BUFFER_VALID Timer count channel 0 compare/capture buffer valid
 * \retval TC_STATUS_CHN1_BUFFER_VALID Timer count channel 1 compare/capture buffer valid
 * \retval TC_STATUS_PERIOD_BUFFER_VALID Timer count period buffer valid
 */
static inline uint32_t tc_get_status(
		struct tc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	TcCount8 *const tc_module = &(module_inst->hw->COUNT8);

	uint32_t int_flags = tc_module->INTFLAG.reg;

	uint32_t status_flags = 0;

	/* Check for TC channel 0 match */
	if (int_flags & TC_INTFLAG_MC(1)) {
		status_flags |= TC_STATUS_CHANNEL_0_MATCH;
	}

	/* Check for TC channel 1 match */
	if (int_flags & TC_INTFLAG_MC(2)) {
		status_flags |= TC_STATUS_CHANNEL_1_MATCH;
	}

#if !defined(FEATURE_TC_SYNCBUSY_SCHEME_VERSION_2)
	/* Check for TC read synchronization ready */
	if (int_flags & TC_INTFLAG_SYNCRDY) {
		status_flags |= TC_STATUS_SYNC_READY;
	}
#endif

	/* Check for TC capture overflow */
	if (int_flags & TC_INTFLAG_ERR) {
		status_flags |= TC_STATUS_CAPTURE_OVERFLOW;
	}

	/* Check for TC count overflow */
	if (int_flags & TC_INTFLAG_OVF) {
		status_flags |= TC_STATUS_COUNT_OVERFLOW;
	}
#ifdef FEATURE_TC_DOUBLE_BUFFERED
	uint8_t double_buffer_valid_status = tc_module->STATUS.reg;

	/* Check channel 0 compare or capture buffer valid */
	if (double_buffer_valid_status & TC_STATUS_CCBUFV0) {
		status_flags |= TC_STATUS_CHN0_BUFFER_VALID;
	}
	/* Check channel 0 compare or capture buffer valid */
	if (double_buffer_valid_status & TC_STATUS_CCBUFV1) {
		status_flags |= TC_STATUS_CHN1_BUFFER_VALID;
	}
	/* Check period buffer valid */
	if (double_buffer_valid_status & TC_STATUS_PERBUFV) {
		status_flags |= TC_STATUS_PERIOD_BUFFER_VALID;
	}
#endif

	return status_flags;
}

/**
 * \brief Clears a module status flag.
 *
 * Clears the given status flag of the module.
 *
 * \param[in] module_inst   Pointer to the TC software instance struct
 * \param[in] status_flags  Bitmask of \c TC_STATUS_* flags to clear
 */
static inline void tc_clear_status(
		struct tc_module *const module_inst,
		const uint32_t status_flags)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	TcCount8 *const tc_module = &(module_inst->hw->COUNT8);

	uint32_t int_flags = 0;

	/* Check for TC channel 0 match */
	if (status_flags & TC_STATUS_CHANNEL_0_MATCH) {
		int_flags |= TC_INTFLAG_MC(1);
	}

	/* Check for TC channel 1 match */
	if (status_flags & TC_STATUS_CHANNEL_1_MATCH) {
		int_flags |= TC_INTFLAG_MC(2);
	}

#if !defined(FEATURE_TC_SYNCBUSY_SCHEME_VERSION_2)
	/* Check for TC read synchronization ready */
	if (status_flags & TC_STATUS_SYNC_READY) {
		int_flags |= TC_INTFLAG_SYNCRDY;
	}
#endif

	/* Check for TC capture overflow */
	if (status_flags & TC_STATUS_CAPTURE_OVERFLOW) {
		int_flags |= TC_INTFLAG_ERR;
	}

	/* Check for TC count overflow */
	if (status_flags & TC_STATUS_COUNT_OVERFLOW) {
		int_flags |= TC_INTFLAG_OVF;
	}

	/* Clear interrupt flag */
	tc_module->INTFLAG.reg = int_flags;
}

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

/**
 * \page asfdoc_sam0_tc_extra Extra Information for TC Driver
 *
 * \section asfdoc_sam0_tc_extra_acronyms Acronyms
 * The table below presents the acronyms used in this module:
 *
 * <table>
 *	<tr>
 *		<th>Acronym</th>
 *		<th>Description</th>
 *	</tr>
  *	<tr>
 *		<td>DMA</td>
 *		<td>Direct Memory Access</td>
 *	</tr>
 *	<tr>
 *		<td>TC</td>
 *		<td>Timer Counter</td>
 *	</tr>
 *	<tr>
 *		<td>PWM</td>
 *		<td>Pulse Width Modulation</td>
 *	</tr>
 *	<tr>
 *		<td>PWP</td>
 *		<td>Pulse Width Period</td>
 *	</tr>
 *	<tr>
 *		<td>PPW</td>
 *		<td>Period Pulse Width</td>
 *	</tr>
 * </table>
 *
 *
 * \section asfdoc_sam0_tc_extra_dependencies Dependencies
 * This driver has the following dependencies:
 *
 *  - \ref asfdoc_sam0_system_pinmux_group "System Pin Multiplexer Driver"
 *
 *
 * \section asfdoc_sam0_tc_extra_errata Errata
 * There are no errata related to this driver.
 *
 *
 * \section asfdoc_sam0_tc_extra_history Module History
 * An overview of the module history is presented in the table below, with
 * details on the enhancements and fixes made to the module since its first
 * release. The current version of this corresponds to the newest version in
 * the table.
 *
 * <table>
 *	<tr>
 *		<th>Changelog</th>
 *	</tr>
 *	<tr>
 *    <td>Added support for SAM D21 and do some modifications as below:
 *          \li Clean up in the configuration structure, the counter size
 *              setting specific registers is accessed through the counter_8_bit,
 *              counter_16_bit, and counter_32_bit structures
 *          \li All event related settings moved into the tc_event structure </td>
 *	</tr>
 *	<tr>
 *		<td>Added automatic digital clock interface enable for the slave TC
 *          module when a timer is initialized in 32-bit mode</td>
 *	</tr>
 *	<tr>
 *		<td>Initial release</td>
 *	</tr>
 * </table>
 */

/**
 * \page asfdoc_sam0_tc_exqsg Examples for TC Driver
 *
 * This is a list of the available Quick Start guides (QSGs) and example
 * applications for \ref asfdoc_sam0_tc_group. QSGs are simple examples with
 * step-by-step instructions to configure and use this driver in a selection of
 * use cases. Note that QSGs can be compiled as a standalone application or be
 * added to the user application.
 *
 *  - \subpage asfdoc_sam0_tc_basic_use_case
 *  - \subpage asfdoc_sam0_tc_macth_freq_use_case
 * \if TC_CALLBACK_MODE
 *  - \subpage asfdoc_sam0_tc_timer_use_case
 *  - \subpage asfdoc_sam0_tc_callback_use_case
 * \endif
 *  - \subpage asfdoc_sam0_tc_dma_use_case
 *
 * \page asfdoc_sam0_tc_document_revision_history Document Revision History
 *
 * <table>
 *	<tr>
 *		<th>Doc. Rev.</td>
 *		<th>Date</td>
 *		<th>Comments</td>
 *	</tr>
 *	<tr>
 *		<td>42123E</td>
 *		<td>12/2015</td>
 *		<td>Added support for SAM L21/L22, SAM DA1, SAM D09, and SAM C21</td>
 *	</tr>
 *	<tr>
 *		<td>42123D</td>
 *		<td>12/2014</td>
 *		<td>Added timer use case.
 *		    Added support for SAM R21 and SAM D10/D11</td>
 *	</tr>
 *	<tr>
 *		<td>42123C</td>
 *		<td>01/2014</td>
 *		<td>Added support for SAM D21</td>
 *	</tr>
 *	<tr>
 *		<td>42123B</td>
 *		<td>06/2013</td>
 *		<td>Corrected documentation typos</td>
 *	</tr>
 *	<tr>
 *		<td>42123A</td>
 *		<td>06/2013</td>
 *		<td>Initial document release</td>
 *	</tr>
 * </table>
 */

#endif /* TC_H_INCLUDED */
