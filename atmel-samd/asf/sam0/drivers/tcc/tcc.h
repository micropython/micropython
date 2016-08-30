/**
 * \file
 *
 * \brief SAM TCC - Timer Counter for Control Applications Driver
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

#ifndef TCC_H_INCLUDED
#define TCC_H_INCLUDED

/**
 * \defgroup asfdoc_sam0_tcc_group SAM Timer Counter for Control Applications (TCC) Driver
 *
 * This driver for Atmel&reg; | SMART ARM&reg;-based microcontrollers provides an interface for the configuration
 * and management of the TCC module within the device, for waveform
 * generation and timing operations. It also provides extended options for
 * control applications.
 *
 * The following driver API modes are covered
 * by this manual:
 *
 *  - Polled APIs
 * \if TCC_CALLBACK_MODE
 *  - Callback APIs
 * \endif
 *
 * The following peripheral is used by this module:
 *  - TCC (Timer/Counter for Control Applications)
 *
 * The following devices can use this module:
 *  - Atmel | SMART SAM D21
 *  - Atmel | SMART SAM R21
 *  - Atmel | SMART SAM D10/D11
 *  - Atmel | SMART SAM L21/L22
 *  - Atmel | SMART SAM DA1
 *  - Atmel | SMART SAM C20/C21
 *  - Atmel | SMART SAM R30
 *
 * The outline of this documentation is as follows:
 *  - \ref asfdoc_sam0_tcc_prerequisites
 *  - \ref asfdoc_sam0_tcc_module_overview
 *  - \ref asfdoc_sam0_tcc_special_considerations
 *  - \ref asfdoc_sam0_tcc_extra_info
 *  - \ref asfdoc_sam0_tcc_examples
 *  - \ref asfdoc_sam0_tcc_api_overview
 *
 * \section asfdoc_sam0_tcc_prerequisites Prerequisites
 *
 * There are no prerequisites for this module.
 *
 * \section asfdoc_sam0_tcc_module_overview Module Overview
 *
 * The Timer/Counter for Control Applications (TCC) module provides a set of
 * timing and counting related functionality, such as the generation of periodic
 * waveforms, the capturing of a periodic waveform's frequency/duty cycle,
 * software timekeeping for periodic operations, waveform extension control,
 * fault detection etc.
 *
 * The counter size of the TCC modules can be 16- or 24-bit depending on
 * the TCC instance.
 * Refer \ref asfdoc_sam0_tcc_special_considerations_tcc_d21 and
 * \ref asfdoc_sam0_tcc_special_considerations_tcc_d11 for details on TCC instances.
 *
 * The TCC module for the SAM includes the following functions:
 *
 * - Generation of PWM signals
 * - Generation of timestamps for events
 * - General time counting
 * - Waveform period capture
 * - Waveform frequency capture
 * - Additional control for generated waveform outputs
 * - Fault protection for waveform generation
 *
 * \ref asfdoc_sam0_tcc_block_diagram "The diagram below" shows the overview
 * of the TCC Module.
 *
 * \anchor asfdoc_sam0_tcc_block_diagram
 * \image html overview.svg "Overview of the TCC Module"
 *
 * \subsection asfdoc_sam0_tcc_module_overview_parts Functional Description
 * The TCC module consists of following sections:
 * - Base Counter
 * - Compare/Capture channels, with waveform generation
 * - Waveform extension control and fault detection
 * - Interface to the event system, DMAC, and the interrupt system
 *
 * The base counter can be configured to either count a prescaled generic
 * clock or events from the event system.(TCEx, with event action configured
 * to counting).
 * The counter value can be used by compare/capture channels which can be
 * set up either in compare mode or capture mode.
 *
 * In capture mode, the counter value is stored when a configurable event
 * occurs. This mode can be used to generate timestamps used in event capture,
 * or it can be used for the measurement of a periodic input signal's
 * frequency/duty cycle.
 *
 * In compare mode, the counter value is compared against one or more of the
 * configured channels' compare values. When the counter value coincides with a
 * compare value an action can be taken automatically by the module, such as
 * generating an output event or toggling a pin when used for frequency or PWM
 * signal generation.
 *
 * \note The connection of events between modules requires the use of the
 *       \ref asfdoc_sam0_events_group "SAM Event System Driver (EVENTS)"
 *       to route output event of one module to the the input event of another.
 *       For more information on event routing, refer to the event driver
 *       documentation.
 *
 * In compare mode, when output signal is generated, extended waveform controls
 * are available, to arrange the compare outputs into specific formats.
 * The Output matrix can change the channel output routing. Pattern generation
 * unit can overwrite the output signal line to specific state.
 * The Fault protection feature of the TCC supports recoverable and
 * non-recoverable faults.
 *
 * \subsection asfdoc_sam0_tcc_module_overview_tc Base Timer/Counter
 *
 * \subsubsection asfdoc_sam0_tcc_module_overview_tc_size Timer/Counter Size
 * Each TCC has a counter size of either 16- or 24-bits. The size of the
 * counter determines the maximum value it can count to before an overflow
 * occurs.
 * \ref asfdoc_sam0_tcc_count_size_vs_top "The table below" shows the
 * maximum values for each of the possible counter sizes.
 *
 * \anchor asfdoc_sam0_tcc_count_size_vs_top
 * <table>
 *  <caption>Timer Counter Sizes and Their Maximum Count Values</caption>
 *  <tr>
 *    <th>Counter size</th>
 *    <th>Max. (hexadecimal)</th>
 *    <th>Max. (decimal)</th>
 *  </tr>
 *  <tr>
 *    <td>16-bit</td>
 *    <td>0xFFFF</td>
 *    <td>65,535</td>
 *  </tr>
 *  <tr>
 *    <td>24-bit</td>
 *    <td>0xFFFFFF</td>
 *    <td>16,777,215</td>
 *  </tr>
 * </table>
 *
 * The period/top value of the counter can be set, to define counting period.
 * This will allow the counter to overflow when the counter value reaches the
 * period/top value.
 *
 * \subsubsection asfdoc_sam0_tcc_module_overview_tc_clk Timer/Counter Clock and Prescaler
 * TCC is clocked asynchronously to the system clock by a GCLK
 * (Generic Clock) channel. The GCLK channel can be connected to any of the GCLK
 * generators. The GCLK generators are configured to use one of the available
 * clock sources in the system such as internal oscillator, external crystals,
 * etc. See the \ref asfdoc_sam0_system_clock_group "Generic Clock driver" for
 * more information.
 *
 * Each TCC module in the SAM has its own individual clock prescaler, which
 * can be used to divide the input clock frequency used by the counter. This
 * prescaler only scales the clock used to provide clock pulses for the counter
 * to count, and does not affect the digital register interface portion of
 * the module, thus the timer registers will be synchronized to the raw GCLK
 * frequency input to the module.
 *
 * As a result of this, when selecting a GCLK frequency and timer prescaler
 * value, the user application should consider both the timer resolution
 * required and the synchronization frequency to avoid lengthy
 * synchronization times of the module if a very slow GCLK frequency is fed
 * into the TCC module. It is preferable to use a higher module GCLK frequency
 * as the input to the timer, and prescale this down as much as possible to
 * obtain a suitable counter frequency in latency-sensitive applications.
 *
 * \subsubsection asfdoc_sam0_tcc_module_overview_tc_ctrl Timer/Counter Control Inputs (Events)
 *
 * The TCC can take several actions on the occurrence of an input event.
 * The event actions are listed
 * in \ref asfdoc_sam0_tcc_module_event_act "events action settings".
 *
 * \anchor asfdoc_sam0_tcc_module_event_act
 * <table>
 *   <caption>TCC Module Event Actions</caption>
 *   <tr>
 *     <th>Event action</th>
 *     <th>Description</th>
 *     <th>Applied event</th>
 *   </tr>
 *   <tr>
 *     <td>TCC_EVENT_ACTION_OFF</td>
 *     <td>No action on the event input</td>
 *     <td>All</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_EVENT_ACTION_RETRIGGER</td>
 *     <td>Re-trigger Counter on event</td>
 *     <td>All</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_EVENT_ACTION_NON_RECOVERABLE_FAULT</td>
 *     <td>Generate Non-Recoverable Fault on event</td>
 *     <td>All</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_EVENT_ACTION_START</td>
 *     <td>Counter start on event</td>
 *     <td>EV0</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_EVENT_ACTION_DIR_CONTROL</td>
 *     <td>Counter direction control</td>
 *     <td>EV0</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_EVENT_ACTION_DECREMENT</td>
 *     <td>Counter decrement on event</td>
 *     <td>EV0</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_EVENT_ACTION_PERIOD_PULSE_WIDTH_CAPTURE</td>
 *     <td>Capture pulse period and pulse width</td>
 *     <td>EV0</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_EVENT_ACTION_PULSE_WIDTH_PERIOD_CAPTURE</td>
 *     <td>Capture pulse width and pulse period</td>
 *     <td>EV0</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_EVENT_ACTION_STOP</td>
 *     <td>Counter stop on event</td>
 *     <td>EV1</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_EVENT_ACTION_COUNT_EVENT</td>
 *     <td>Counter count on event</td>
 *     <td>EV1</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_EVENT_ACTION_INCREMENT</td>
 *     <td>Counter increment on event</td>
 *     <td>EV1</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_EVENT_ACTION_COUNT_DURING_ACTIVE</td>
 *     <td>Counter count during active state of asynchronous event</td>
 *     <td>EV1</td>
 *   </tr>
 * </table>
 *
 * \subsubsection asfdoc_sam0_tcc_module_overview_tc_reload Timer/Counter Reloading
 *
 * The TCC also has a configurable reload action, used when a
 * re-trigger event occurs. Examples of a re-trigger event could be the counter
 * reaching the maximum value when counting up, or when an event from the event
 * system makes the counter to re-trigger. The reload action determines if the
 * prescaler should be reset, and on which clock. The counter will
 * always be reloaded with the value it is set to start counting. The user
 * can choose between three different reload actions, described in
 * \ref asfdoc_sam0_tcc_module_reload_act "the table below".
 *
 * \anchor asfdoc_sam0_tcc_module_reload_act
 * <table>
 *   <caption>TCC Module Reload Actions</caption>
 *   <tr>
 *     <th>Reload action</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>TCC_RELOAD_ACTION_GCLK</td>
 *     <td>Reload TCC counter value on next GCLK cycle. Leave prescaler
 *         as-is.</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_RELOAD_ACTION_PRESC</td>
 *     <td>Reloads TCC counter value on next prescaler clock. Leave prescaler
 *         as-is.</td>
 *   </tr>
 *  <tr>
 *    <td>TCC_RELOAD_ACTION_RESYNC</td>
 *    <td>Reload TCC counter value on next GCLK cycle. Clear prescaler to
 *        zero.</td>
 *  </tr>
 * </table>
 *
 * The reload action to use will depend on the specific application being
 * implemented. One example is when an external trigger for a reload occurs; if
 * the TCC uses the prescaler, the counter in the prescaler should not have a
 * value between zero and the division factor. The counter in the TCC module
 * and the counter in the prescaler should both start at zero.
 * If the counter is set to re-trigger when it reaches the maximum value,
 * this is not the right option to use. In such a case it would be better if
 * the prescaler is left unaltered when the re-trigger happens, letting the
 * counter reset on the next GCLK cycle.
 *
 * \subsubsection asfdoc_sam0_tcc_module_overview_tc_oneshot One-shot Mode
 *
 * The TCC module can be configured in one-shot mode. When configured in this
 * manner, starting the timer will cause it to count until the next overflow
 * or underflow condition before automatically halting, waiting to be manually
 * triggered by the user application software or an event from the event
 * system.
 *
 * \subsection asfdoc_sam0_tcc_module_overview_capt Capture Operations
 *
 * In capture operations, any event from the event system or a pin change can
 * trigger a capture of the counter value. This captured counter value can be
 * used as timestamps for the events, or it can be used in frequency and pulse
 * width capture.
 *
 * \subsubsection asfdoc_sam0_tcc_module_overview_capt_ev Capture Operations - Event
 *
 * Event capture is a simple use of the capture functionality,
 * designed to create timestamps for specific events. When the input event
 * appears, the current counter value is copied into the corresponding
 * compare/capture register, which can then be read by the user application.
 *
 * Note that when performing any capture operation, there is a risk that the
 * counter reaches its top value (MAX) when counting up, or the bottom value
 * (zero) when counting down, before the capture event occurs. This can distort
 * the result, making event timestamps to appear shorter than they really are.
 * In this case, the user application should check for timer overflow when
 * reading a capture result in order to detect this situation and perform an
 * appropriate adjustment.
 *
 * Before checking for a new capture, \ref TCC_STATUS_COUNT_OVERFLOW
 * should be checked. The response to an overflow error is left to the user
 * application, however, it may be necessary to clear both the overflow
 * flag and the capture flag upon each capture reading.
 *
 * \subsubsection asfdoc_sam0_tcc_module_overview_capt_pulse Capture Operations - Pulse Width
 *
 * Pulse Width Capture mode makes it possible to measure the pulse width and
 * period of PWM signals. This mode uses two capture channels of the counter.
 * There are two modes for pulse width capture;
 * Pulse Width Period (PWP) and Period Pulse Width (PPW). In PWP mode, capture
 * channel 0 is used for storing the pulse width and capture channel 1 stores
 * the observed period. While in PPW mode, the roles of the two capture channels
 * are reversed.
 *
 * As in the above example it is necessary to poll on interrupt flags to see
 * if a new capture has happened and check that a capture overflow error has
 * not occurred.
 *
 * Refer to \ref asfdoc_sam0_tcc_module_overview_tc_ctrl to set up the input
 * event to perform pulse width capture.
 *
 * \subsection asfdoc_sam0_tcc_module_overview_mc Compare Match Operation
 *
 * In compare match operation, Compare/Capture registers are compared
 * with the counter value. When the timer's count value matches the value of a
 * compare channel, a user defined action can be taken.
 *
 * \subsubsection asfdoc_sam0_tcc_module_overview_mc_timer Basic Timer
 *
 * A Basic Timer is a simple application where compare match operation is used
 * to determine when a specific period has elapsed. In Basic Timer operations,
 * one or more values in the module's Compare/Capture registers are used to
 * specify the time (in terms of the number of prescaled GCLK cycles, or
 * input events) at which
 * an action should be taken by the microcontroller. This can be an Interrupt
 * Service Routine (ISR), event generation via the event system, or a software
 * flag that is polled from the user application.
 *
 * \subsubsection asfdoc_sam0_tcc_module_overview_mc_wave Waveform Generation
 *
 * Waveform generation enables the TCC module to generate square waves, or, if
 * combined with an external passive low-pass filter, analog waveforms.
 *
 * \subsubsection asfdoc_sam0_tcc_module_overview_mc_wave_pwm Waveform Generation - PWM
 *
 * Pulse width modulation is a form of waveform generation and a signalling
 * technique that can be useful in many applications. When PWM mode is used,
 * a digital pulse train with a configurable frequency and duty cycle can be
 * generated by the TCC module and output to a GPIO pin of the device.
 *
 * Often PWM is used to communicate a control or information parameter to an
 * external circuit or component. Differing impedances of the source generator
 * and sink receiver circuits is less of an issue when using PWM compared to
 * using an analog voltage value, as noise will not generally affect the
 * signal's integrity to a meaningful extent.
 *
 * \ref asfdoc_sam0_tcc_module_pwm_single_diag "The figure below" illustrates
 * operations and different states of the counter and its output when using
 * the timer in Normal PWM mode (Single Slope). As can be seen, the TOP/PERIOD
 * value is
 * unchanged and is set to MAX. The compare match value is changed at several
 * points to illustrate the resulting waveform output changes. The PWM output is
 * set to normal (i.e. non-inverted) output mode.
 *
 * \anchor asfdoc_sam0_tcc_module_pwm_single_diag
 * \image html pwm_single_ex.svg "Example Of PWM In Single-Slope Mode, and Different Counter Operations"
 *
 * Several PWM modes are supported by the TCC module, refer to
 * datasheet for the details on PWM waveform generation.
 *
 * \subsubsection asfdoc_sam0_tcc_module_overview_mc_wave_freq Waveform Generation - Frequency
 *
 * Normal Frequency Generation is in many ways identical to PWM generation.
 * However, only in Frequency Generation, a toggle occurs on the output when a
 * match on a compare channels occurs.
 *
 * When the Match Frequency Generation is used, the timer value is reset on
 * match condition, resulting in a variable frequency square wave with a
 * fixed 50% duty cycle.
 *
 * \subsection asfdoc_sam0_tcc_module_overview_ext Waveform Extended Controls
 *
 * \subsubsection asfdoc_sam0_tcc_module_overview_ext_pat Pattern Generation
 *
 * Pattern insertion allows the TCC module to change the actual pin output level
 * without modifying the compare/match settings.
 *
 * \anchor asfdoc_sam0_tcc_module_pattern_gen
 * <table>
 *   <caption>TCC Module Output Pattern Generation</caption>
 *   <tr>
 *     <th>Pattern</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>TCC_OUTPUT_PATTERN_DISABLE</td>
 *     <td>Pattern disabled, generate output as is</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_OUTPUT_PATTERN_0</td>
 *     <td>Generate pattern 0 on output (keep the output LOW)</td>
 *   </tr>
 *  <tr>
 *    <td>TCC_OUTPUT_PATTERN_1</td>
 *    <td>Generate pattern 1 on output (keep the output HIGH)</td>
 *  </tr>
 * </table>
 *
 * \subsubsection asfdoc_sam0_tcc_module_overview_ext_r_fault Recoverable Faults
 *
 * The recoverable faults can trigger one or several of following fault actions:
 * -# *Halt* action: The recoverable faults can halt the TCC timer/counter,
 *    so that the final output wave is kept at a defined state. When the fault
 *    state is removed it is possible to recover the counter and waveform
 *    generation. The halt action is defined as:
 * \anchor asfdoc_sam0_tcc_module_fault_halt_action
 * <table>
 *   <caption>TCC Module Recoverable Fault Halt Actions</caption>
 *   <tr>
 *     <th>Action</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>TCC_FAULT_HALT_ACTION_DISABLE</td>
 *     <td>Halt action is disabled</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_FAULT_HALT_ACTION_HW_HALT</td>
 *     <td>The timer/counter is halted as long as the corresponding fault is
 *         present</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_FAULT_HALT_ACTION_SW_HALT</td>
 *     <td>The timer/counter is halted until the corresponding fault is removed
 *         and fault state cleared by software</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_FAULT_HALT_ACTION_NON_RECOVERABLE</td>
 *     <td>Force all the TCC output pins to a pre-defined level, as what
 *         Non-Recoverable Fault do</td>
 *   </tr>
 * </table>
 * -# *Restart* action: When enabled, the recoverable faults can restart the TCC
 *    timer/counter.
 * -# *Keep* action: When enabled, the recoverable faults can keep the
 *    corresponding channel output to zero when the fault condition is present.
 * -# *Capture* action: When the recoverable fault occurs, the capture action can
 *    time stamps the corresponding fault. The following capture mode is
 *    supported:
 * \anchor asfdoc_sam0_tcc_module_fault_capt_action
 * <table>
 *   <caption>TCC Module Recoverable Fault Capture Actions</caption>
 *   <tr>
 *     <th>Action</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>TCC_FAULT_CAPTURE_DISABLE</td>
 *     <td>Capture action is disabled</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_FAULT_CAPTURE_EACH</td>
 *     <td>Equivalent to standard capture operation, on each fault occurrence
 *         the time stamp is captured</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_FAULT_CAPTURE_MINIMUM</td>
 *     <td>Get the minimum time stamped value in all time stamps</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_FAULT_CAPTURE_MAXIMUM</td>
 *     <td>Get the maximum time stamped value in all time stamps</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_FAULT_CAPTURE_SMALLER</td>
 *     <td>Time stamp the fault input if the value is smaller than last one</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_FAULT_CAPTURE_BIGGER</td>
 *     <td>Time stamp the fault input if the value is bigger than last one</td>
 *   </tr>
 *   <tr>
 *     <td>TCC_FAULT_CAPTURE_CHANGE</td>
 *     <td>Time stamp the fault input if the time stamps changes its increment
 *         direction</td>
 *   </tr>
 * </table>
 *
 * In TCC module, only the first two compare channels (CC0 and CC1) can work
 * with recoverable fault inputs. The corresponding event inputs (TCCx MC0
 * and TCCx MC1) are then used as fault inputs respectively.
 * The faults are called Fault A and Fault B.
 *
 * The recoverable fault can be filtered or effected by corresponding channel
 * output. On fault condition there are many other settings that can be chosen.
 * Refer to data sheet for more details about the recoverable fault
 * operations.
 *
 * \subsubsection asfdoc_sam0_tcc_module_overview_ext_n_fault Non-Recoverable Faults
 *
 * The non-recoverable faults force all the TCC output pins to a pre-defined
 * level (can be forced to 0 or 1). The input control signal of non-recoverable
 * fault is from timer/counter event (TCCx EV0 and TCCx EV1).
 * To enable non-recoverable fault,
 * corresponding TCEx event action must be set to non-recoverable fault action
 * (\ref TCC_EVENT_ACTION_NON_RECOVERABLE_FAULT).
 * Refer to \ref asfdoc_sam0_tcc_module_overview_tc_ctrl to see the available
 * event input action.
 *
 * \subsection asfdoc_sam0_tcc_module_overview_buffering Double and Circular Buffering
 *
 * The pattern, period, and the compare channels registers are double buffered.
 * For these options there are effective registers (PATT, PER, and CCx) and
 * buffer registers (PATTB, PERB, and CCx). When writing to the buffer
 * registers, the values are buffered and will be committed to effective
 * registers on UPDATE condition.
 *
 * Usually the buffered value is cleared after it is committed, but there is also
 * an option to circular the register buffers. The period (PER) and four lowest
 * compare channels register (CCx, x is 0 ~ 3) support this function. When
 * circular buffer is used, on UPDATE the previous period or compare values are
 * copied back into the corresponding period buffer and compare buffers.
 * This way, the register value and its buffer register value is actually
 * switched on UPDATE condition, and will be switched back on next UPDATE
 * condition.
 *
 * For input capture, the buffer register (CCBx) and the corresponding capture
 * channel register (CCx) act like a FIFO. When regular register (CCx) is empty
 * or read, any content in the buffer register is passed to regular one.
 *
 * In TCC module driver, when the double buffering write is enabled, any
 * write through \ref tcc_set_top_value(), \ref tcc_set_compare_value(), and
 * \ref tcc_set_pattern() will be done to the corresponding buffer register.
 * Then the value in the buffer register will be transferred to the regular
 * register on the next UPDATE condition or by a force UPDATE using
 * \ref tcc_force_double_buffer_update().
 *
 * \subsection asfdoc_sam0_tcc_module_overview_sleep Sleep Mode
 *
 * TCC modules can be configured to operate in any sleep mode, with its "run
 * in standby" function enabled. It can wake up the device using interrupts or
 * perform internal actions with the help of the Event System.
 *
 * \section asfdoc_sam0_tcc_special_considerations Special Considerations
 *
 * \subsection asfdoc_sam0_tcc_special_considerations_specific_features Driver Feature Macro Definition
 * \ref asfdoc_sam0_tcc_feature_table "The table below" shows some specific features
 * of the TCC Module.
 *
 * \anchor asfdoc_sam0_tcc_feature_table
 * <table>
 *   <caption>TCC Module Specific Features</caption>
 *  <tr>
 *    <th>Driver Feature Macro</th>
 *    <th>Supported devices</th>
 *  </tr>
 *  <tr>
 *    <td>FEATURE_TCC_GENERATE_DMA_TRIGGER</td>
 *    <td>SAM L21/L22/R30</td>
 *  </tr>
 * </table>
 *
 * \note The specific features are only available in the driver when the
 * selected device supports those features.
 *
 * \subsection asfdoc_sam0_tcc_special_considerations_tcc_feature Module Features
 *
 * The features of TCC, such as timer/counter size, number of compare capture
 * channels, and number of outputs, are dependent on the TCC module instance being
 * used.
 *
 * \subsubsection asfdoc_sam0_tcc_special_considerations_tcc_d21 SAM TCC Feature List
 * For SAM D21/R21/L21/L22/DA1/C21/R30, the TCC features are:
 * \anchor asfdoc_sam0_tcc_features_d21
 * <table>
 *   <caption>TCC module features for SAM D21/R21/L21/L22/DA1/C21/R30</caption>
 *   <tr>
 *     <th>TCC#</th>
 *     <th>Match/Capture channels</th>
 *     <th>Wave outputs</th>
 *     <th>Counter size [bits]</th>
 *     <th>Fault</th>
 *     <th>Dithering</th>
 *     <th>Output matrix</th>
 *     <th>Dead-Time insertion</th>
 *     <th>SWAP</th>
 *     <th>Pattern</th>
 *   </tr>
 *   <tr>
 *     <td>0</td>
 *     <td>4</td>
 *     <td>8</td>
 *     <td>24</td>
 *     <td>Y</td>
 *     <td>Y</td>
 *     <td>Y</td>
 *     <td>Y</td>
 *     <td>Y</td>
 *     <td>Y</td>
 *   </tr>
 *   <tr>
 *     <td>1</td>
 *     <td>2</td>
 *     <td>4</td>
 *     <td>24</td>
 *     <td>Y</td>
 *     <td>Y</td>
 *     <td></td>
 *     <td></td>
 *     <td></td>
 *     <td>Y</td>
 *   </tr>
 *   <tr>
 *     <td>2</td>
 *     <td>2</td>
 *     <td>2</td>
 *     <td>16</td>
 *     <td>Y</td>
 *     <td></td>
 *     <td></td>
 *     <td></td>
 *     <td></td>
 *     <td></td>
 *   </tr>
 * </table>
 *
 * \subsubsection asfdoc_sam0_tcc_special_considerations_tcc_d11 SAM D10/D11 TCC Feature List
 * For SAM D10/D11, the TCC features are:
 * \anchor asfdoc_sam0_tcc_features_d11
 * <table>
 *   <caption>TCC Module Features For SAM D10/D11</caption>
 *   <tr>
 *     <th>TCC#</th>
 *     <th>Match/Capture channels</th>
 *     <th>Wave outputs</th>
 *     <th>Counter size [bits]</th>
 *     <th>Fault</th>
 *     <th>Dithering</th>
 *     <th>Output matrix</th>
 *     <th>Dead-Time insertion</th>
 *     <th>SWAP</th>
 *     <th>Pattern</th>
 *   </tr>
 *   <tr>
 *     <td>0</td>
 *     <td>4</td>
 *     <td>8</td>
 *     <td>24</td>
 *     <td>Y</td>
 *     <td>Y</td>
 *     <td>Y</td>
 *     <td>Y</td>
 *     <td>Y</td>
 *     <td>Y</td>
 *   </tr>
 * </table>
 *
 * \subsection asfdoc_sam0_tcc_special_considerations_tcc_pin Channels vs. Pinouts
 *
 * As the TCC module may have more waveform output pins than the number of
 * compare/capture channels, the free pins (with number higher than number of
 * channels) will reuse the waveform generated by channels subsequently. E.g.,
 * if the number of channels is four and the number of wave output pins is eight, channel
 * 0 output will be available on out pin 0 and 4, channel 1 output
 * on wave out pin 1 and 5, and so on.
 *
 * \section asfdoc_sam0_tcc_extra_info Extra Information
 *
 * For extra information, see \ref asfdoc_sam0_tcc_extra. This includes:
 *  - \ref asfdoc_sam0_tcc_extra_acronyms
 *  - \ref asfdoc_sam0_tcc_extra_dependencies
 *  - \ref asfdoc_sam0_tcc_extra_errata
 *  - \ref asfdoc_sam0_tcc_extra_history
 *
 *
 * \section asfdoc_sam0_tcc_examples Examples
 *
 * For a list of examples related to this driver, see
 * \ref asfdoc_sam0_tcc_exqsg.
 *
 * \section asfdoc_sam0_tcc_api_overview API Overview
 * @{
 */

#include <compiler.h>
#include <clock.h>
#include <gclk.h>
#include <pinmux.h>

/** Maximum number of channels supported by the driver
 *  (Channel index from 0 to \c TCC_NUM_CHANNELS - 1).
 */
#define TCC_NUM_CHANNELS           4

/** Maximum number of wave outputs lines supported by the driver
 *  (Output line index from 0 to \c TCC_NUM_WAVE_OUTPUTS - 1).
 */
#define TCC_NUM_WAVE_OUTPUTS       8

/** Maximum number of (recoverable) faults supported by the driver. */
#define TCC_NUM_FAULTS             2

#if TCC_ASYNC == true
#  include <system_interrupt.h>
#endif

/**
 * \name Driver Feature Definition
 * Define port features set according to different device family.
 * @{
*/
#if (SAML21) || (SAML22) || (SAMR30) || defined(__DOXYGEN__)
/** Generate DMA triggers */
#  define FEATURE_TCC_GENERATE_DMA_TRIGGER
#endif
/*@}*/

#ifdef __cplusplus
extern "C" {
#endif

/** Generates a table enum list entry for a given type
   and index (e.g. "TCC_CALLBACK_MC_CHANNEL_0,"). */
#define _TCC_ENUM(n, type) TCC_##type##_##n,

/** Generates table enum list entries for all channels of a
   given type and channel number on TCC module. */
#define _TCC_CHANNEL_ENUM_LIST(type) \
		MREPEAT(TCC_NUM_CHANNELS, _TCC_ENUM, type##_CHANNEL)
/** Generates table enum list entries for all output of a
   given type and waveform output number on TCC module. */
#define _TCC_WO_ENUM_LIST(type) \
		MREPEAT(TCC_NUM_WAVE_OUTPUTS, _TCC_ENUM, type)


#if TCC_ASYNC == true
/** Enum for the possible callback types for the TCC module. */
enum tcc_callback {
	/** Callback for TCC overflow */
	TCC_CALLBACK_OVERFLOW,
	/** Callback for TCC Retrigger */
	TCC_CALLBACK_RETRIGGER,
	/** Callback for TCC counter event */
	TCC_CALLBACK_COUNTER_EVENT,
	/** Callback for capture overflow error */
	TCC_CALLBACK_ERROR,
	/** Callback for Recoverable Fault A */
	TCC_CALLBACK_FAULTA,
	/** Callback for Recoverable Fault B */
	TCC_CALLBACK_FAULTB,
	/** Callback for Non-Recoverable Fault 0 */
	TCC_CALLBACK_FAULT0,
	/** Callback for Non-Recoverable Fault 1 */
	TCC_CALLBACK_FAULT1,

#  if defined(__DOXYGEN__)
	/** Channel callback type table for TCC
	 *
	 *  Each TCC module may contain several callback types for channels; each
	 *  channel will have its own callback type in the table, with the channel
	 *  index number substituted for "n" in the channel callback type
	 *  (e.g. \c TCC_MATCH_CAPTURE_CHANNEL_0).
	 */
	TCC_CALLBACK_CHANNEL_n = n,
#  else
	/** Callbacks for Match/Capture channels, e.g., TCC_CALLBACK_CHANNEL_0 */
	_TCC_CHANNEL_ENUM_LIST(CALLBACK)
#  endif

#  if !defined(__DOXYGEN__)
	/** Number of available callbacks */
	TCC_CALLBACK_N
#  endif
};
#endif /* #if TCC_ASYNC == true */

/**
 * \name Module Status Flags
 *
 * TCC status flags, returned by \ref tcc_get_status() and cleared by
 * \ref tcc_clear_status().
 *
 * @{
 */

/** Timer channel \c ch (0 ~ 3) has matched against its compare value,
 * or has captured a new value.
 */
#define TCC_STATUS_CHANNEL_MATCH_CAPTURE(ch)        (1UL << (ch))
/** Timer channel \c ch (0 ~ 3) match/compare output state. */
#define TCC_STATUS_CHANNEL_OUTPUT(ch)               (1UL << ((ch)+8))
/** A Non-Recoverable Fault \c x (0 ~ 1) has occurred. */
#define TCC_STATUS_NON_RECOVERABLE_FAULT_OCCUR(x)   (1UL << ((x)+16))
/** A Recoverable Fault \c n (0 ~ 1 representing A ~ B) has occured. */
#define TCC_STATUS_RECOVERABLE_FAULT_OCCUR(n)       (1UL << ((n)+18))
/** The Non-Recoverable Fault \c x (0 ~ 1) input is present. */
#define TCC_STATUS_NON_RECOVERABLE_FAULT_PRESENT(x) (1UL << ((x)+20))
/** A Recoverable Fault \c n (0 ~ 1 representing A ~ B) is present. */
#define TCC_STATUS_RECOVERABLE_FAULT_PRESENT(n)     (1UL << ((n)+22))
/** Timer registers synchronization has completed, and the synchronized count
 *  value may be read.
 */
#define TCC_STATUS_SYNC_READY                       (1UL << 23)
/** A new value was captured before the previous value was read, resulting in
 *  lost data.
 */
#define TCC_STATUS_CAPTURE_OVERFLOW                 (1UL << 24)
/** A counter event occurred. */
#define TCC_STATUS_COUNTER_EVENT                    (1UL << 25)
/** A counter retrigger occurred. */
#define TCC_STATUS_COUNTER_RETRIGGERED              (1UL << 26)
/** The timer count value has overflowed from its maximum value to its minimum
 *  when counting upward, or from its minimum value to its maximum when
 *  counting downward.
 */
#define TCC_STATUS_COUNT_OVERFLOW                   (1UL << 27)
/** Ramp period cycle index.
 *  In ramp operation, each two period cycles are marked as cycle A and B,
 *  the index 0 represents cycle A and 1 represents cycle B. */
#define TCC_STATUS_RAMP_CYCLE_INDEX                 (1UL << 28)
/** The counter has been stopped (due to disable, stop command, or one-shot). */
#define TCC_STATUS_STOPPED                          (1UL << 29)

/** @} */

/**
 * \brief Index of the match capture channels
 *
 * This enum is used to specify which capture/match channel to do
 * operations on.
 */
enum tcc_match_capture_channel {
#  if defined(__DOXYGEN__)
	/** Match capture channel index table for TCC
	 *
	 *  Each TCC module may contain several match capture channels; each channel
	 *  will have its own index in the table, with the index number substituted
	 *  for "n" in the index name (e.g. \c TCC_MATCH_CAPTURE_CHANNEL_0).
	 */
	TCC_MATCH_CAPTURE_CHANNEL_n = n,
#  else
	/** Indexes of match capture channels, e.g., TCC_MATCH_CAPTURE_CHANNEL_0 */
	_TCC_CHANNEL_ENUM_LIST(MATCH_CAPTURE)
#  endif
#  if !defined(__DOXYGEN__)
	/** Number of supported channels */
	TCC_MATCH_CAPTURE_CHANNEL_N
#  endif
};

/**
 * \brief Index of the wave outputs
 *
 * This enum is used to specify which wave output to do
 * operations on.
 */
enum tcc_wave_output {
#  if defined(__DOXYGEN__)
	/** Waveform output index table for TCC
	 *
	 *  Each TCC module may contain several wave outputs; each output
	 *  will have its own index in the table, with the index number substituted
	 *  for "n" in the index name (e.g. \c TCC_WAVE_OUTPUT_0).
	 */
	TCC_WAVE_OUTPUT_n = n,
#  else
	/** Indexes of match capture channels, e.g., TCC_WAVEFORM_OUTPUT_0 */
	_TCC_WO_ENUM_LIST(WAVE_OUTPUT)
#  endif
#  if !defined(__DOXYGEN__)
	/** Number of supported channels */
	TCC_WAVE_OUTPUT_N
#  endif
};

/**
 * \brief TCC wave generation mode enum
 *
 * This enum is used to specify the waveform generation mode.
 *
 */
enum tcc_wave_generation {
	/** Normal Frequency: Top is the PER register, output toggled on each
	 *  compare match */
	TCC_WAVE_GENERATION_NORMAL_FREQ = 0,
	/** Match Frequency: Top is CC0 register, output toggles on each update
	 *  condition */
	TCC_WAVE_GENERATION_MATCH_FREQ = 1,
	/** Single-Slope PWM: Top is the PER register, CCx controls duty cycle
	 *  (output active when count is greater than CCx) */
	TCC_WAVE_GENERATION_SINGLE_SLOPE_PWM = 2,

	/** Double-slope (count up and down), non centre-aligned: Top is the PER
	 *  register, CC[x] controls duty cycle while counting up and CC[x+N/2]
	 *  controls it while counting down */
	TCC_WAVE_GENERATION_DOUBLE_SLOPE_CRITICAL = 4,
	/** Double-slope (count up and down), interrupt/event at Bottom (Top is the
	 *  PER register, output active when count is greater than CCx) */
	TCC_WAVE_GENERATION_DOUBLE_SLOPE_BOTTOM = 5,
	/** Double-slope (count up and down), interrupt/event at Bottom and Top: (Top is the
	 *  PER register, output active when count is lower than CCx) */
	TCC_WAVE_GENERATION_DOUBLE_SLOPE_BOTH = 6,
	/** Double-slope (count up and down), interrupt/event at Top (Top is the
	 *  PER register, output active when count is greater than CCx) */
	TCC_WAVE_GENERATION_DOUBLE_SLOPE_TOP = 7,
};

/**
 * \brief Polarity of TCC wave generation on channels
 *
 * Specifies whether the wave output needs to be inverted or not.
 */
enum tcc_wave_polarity {
	/** Wave output is not inverted */
	TCC_WAVE_POLARITY_0,
	/** Wave output is inverted */
	TCC_WAVE_POLARITY_1
};

/**
 * \brief TCC pattern generator for outputs
 *
 * Used when disabling output pattern or when selecting a specific pattern.
 */
enum tcc_output_pattern {
	/** SWAP output pattern is not used */
	TCC_OUTPUT_PATTERN_DISABLE,
	/** Pattern 0 is applied to SWAP output */
	TCC_OUTPUT_PATTERN_0,
	/** Pattern 1 is applied to SWAP output */
	TCC_OUTPUT_PATTERN_1
};

/**
 * \brief Ramp Operations which are supported in single-slope PWM generation
 *
 * Ramp operations which are supported in single-slope PWM generation.
 */
enum tcc_ramp {
	/** Default timer/counter PWM operation */
	TCC_RAMP_RAMP1 = 0,

	/** Uses a single channel (CC0) to control both CC0/CC1 compare outputs.
	 *  In cycle A, the channel 0 output is disabled, and
	 *  in cycle B, the channel 1 output is disabled. */
	TCC_RAMP_RAMP2A,

	/** Uses channels CC0 and CC1 to control compare outputs.
	 *  In cycle A, the channel 0 output is disabled, and
	 *  in cycle B, the channel 1 output is disabled.*/
	TCC_RAMP_RAMP2
};

/**
 * \brief Ramp Index for TCC wave generation
 *
 *  In ramp operation, each two period cycles are marked as cycle A and B,
 *  the index 0 represents cycle A and 1 represents cycle B.
 */
enum tcc_ramp_index {
	/** Default, cycle index toggles. */
	TCC_RAMP_INDEX_DEFAULT,
	/** Force next cycle to be cycle B (set to 1) */
	TCC_RAMP_INDEX_FORCE_B,
	/** Force next cycle to be cycle A (clear to 0) */
	TCC_RAMP_INDEX_FORCE_A,
	/** Force next cycle keeping the same as current */
	TCC_RAMP_INDEX_FORCE_KEEP
};

/**
 * \brief TCC output inversion
 *
 * Used when enabling or disabling output inversion.
 */
enum tcc_output_invertion {
	/** Output inversion not to be enabled */
	TCC_OUTPUT_INVERTION_DISABLE,
	/** Invert the output from WO[x] */
	TCC_OUTPUT_INVERTION_ENABLE
};

/**
 * \brief TCC Counter reload action enum
 *
 * This enum specify how the counter is reloaded and whether the prescaler
 * should be restarted.
 */
enum tcc_reload_action {
	/** The counter is reloaded/reset on the next GCLK and starts
	 * counting on the prescaler clock
	 */
	TCC_RELOAD_ACTION_GCLK,
	/** The counter is reloaded/reset on the next prescaler clock
	 */
	TCC_RELOAD_ACTION_PRESC,
	/** The counter is reloaded/reset on the next GCLK, and the
	 * prescaler is restarted as well
	 */
	TCC_RELOAD_ACTION_RESYNC
};


/**
 * \brief TCC clock prescaler values
 *
 * This enum is used to choose the clock prescaler
 * configuration. The prescaler divides the clock frequency of the TCC
 * module to operate TCC at a slower clock rate.
 */
enum tcc_clock_prescaler {
	/** Divide clock by 1 */
	TCC_CLOCK_PRESCALER_DIV1,
	/** Divide clock by 2 */
	TCC_CLOCK_PRESCALER_DIV2,
	/** Divide clock by 4 */
	TCC_CLOCK_PRESCALER_DIV4,
	/** Divide clock by 8 */
	TCC_CLOCK_PRESCALER_DIV8,
	/** Divide clock by 16 */
	TCC_CLOCK_PRESCALER_DIV16,
	/** Divide clock by 64 */
	TCC_CLOCK_PRESCALER_DIV64,
	/** Divide clock by 256 */
	TCC_CLOCK_PRESCALER_DIV256,
	/** Divide clock by 1024 */
	TCC_CLOCK_PRESCALER_DIV1024
};

/**
 * \brief TCC module count direction
 *
 * Used when selecting the Timer/Counter count direction.
 */
enum tcc_count_direction {
	/** Timer should count upward */
	TCC_COUNT_DIRECTION_UP,
	/** Timer should count downward */
	TCC_COUNT_DIRECTION_DOWN,
};

#ifdef FEATURE_TCC_GENERATE_DMA_TRIGGER
/**
 * \brief TCC module counter overflow DMA request mode
 *
 * Used when selecting the Timer/Counter overflow DMA request mode.
 */
enum tcc_count_overflow_dma_trigger_mode {
	/** TCC generates a DMA request on each cycle when an update condition 
	 * is detected
	 */
	TCC_COUNT_OVERFLOW_DMA_TRIGGER_MODE_CONTINUE,
	/** When an update condition is detected, the TCC generates a DMA trigger 
	 * on the cycle following the DMA One-Shot Command written to the Control 
	 * B register
	 */
	TCC_COUNT_OVERFLOW_DMA_TRIGGER_MODE_ONE_SHOT,
};
#endif

/**
 * \brief Action to perform when the TCC module is triggered by events
 *
 * Event action to perform when the module is triggered by events.
 */
enum tcc_event_action {
	/** No event action */
	TCC_EVENT_ACTION_OFF,
	/** Stop counting, the counter will maintain its current value, waveforms
	 *  are set to a defined Non-Recoverable State output
	 *  (\ref tcc_non_recoverable_state_output). */
	TCC_EVENT_ACTION_STOP,
	/** Re-trigger counter on event, may generate an event if the re-trigger
	 *  event output is enabled.
	 *  \note When re-trigger event action is enabled, enabling the counter
	 *        will not start until the next incoming event appears. */
	TCC_EVENT_ACTION_RETRIGGER,

	/** Start counter when previously stopped.
	 * Start counting on the event rising edge. Further events will not
	 * restart the counter;
	 * the counter keeps on counting using prescaled GCLK_TCCx, until it
	 * reaches TOP or Zero
	 * depending on the direction. */
	TCC_EVENT_ACTION_START,
	/** Count events; i.e. Increment or decrement depending on count
	 * direction. */
	TCC_EVENT_ACTION_COUNT_EVENT,
	/** The event source must be an asynchronous event, input value will
	 *  overrides the direction settings (input low: counting up, input high:
	 *  counting down). */
	TCC_EVENT_ACTION_DIR_CONTROL,
	/** Increment the counter on event, irrespective of count direction */
	TCC_EVENT_ACTION_INCREMENT,
	/** Decrement the counter on event, irrespective of count direction */
	TCC_EVENT_ACTION_DECREMENT,
	/** Count during active state of asynchronous event. In this case,
	* depending	on the count direction, the  count will be incremented 
	* or decremented on each prescaled GCLK_TCCx, as long as the input
	* event remains active. */
	TCC_EVENT_ACTION_COUNT_DURING_ACTIVE,

	/** Store period in capture register 0, pulse width in capture
	 *  register 1
	 */
	TCC_EVENT_ACTION_PERIOD_PULSE_WIDTH_CAPTURE,
	/** Store pulse width in capture register 0, period in capture
	 *  register 1
	 */
	TCC_EVENT_ACTION_PULSE_WIDTH_PERIOD_CAPTURE,

	/** Generate Non-Recoverable Fault on event */
	TCC_EVENT_ACTION_NON_RECOVERABLE_FAULT,
};


/**
 * \brief Action to be performed when the TCC module is triggered by event0
 *
 * Event action to perform when the module is triggered by event0.
 */
enum tcc_event0_action {
	/** No event action */
	TCC_EVENT0_ACTION_OFF                   = TCC_EVENT_ACTION_OFF,
	/** Re-trigger Counter on event */
	TCC_EVENT0_ACTION_RETRIGGER             = TCC_EVENT_ACTION_RETRIGGER,
	/** Count events (increment or decrement, depending on count direction)
	 */
	TCC_EVENT0_ACTION_COUNT_EVENT           = TCC_EVENT_ACTION_COUNT_EVENT,
	/** Start counter on event */
	TCC_EVENT0_ACTION_START                 = TCC_EVENT_ACTION_START,
	/** Increment counter on event */
	TCC_EVENT0_ACTION_INCREMENT             = TCC_EVENT_ACTION_INCREMENT,
	/** Count during active state of asynchronous event */
	TCC_EVENT0_ACTION_COUNT_DURING_ACTIVE   = TCC_EVENT_ACTION_COUNT_DURING_ACTIVE,

	/** Generate Non-Recoverable Fault on event */
	TCC_EVENT0_ACTION_NON_RECOVERABLE_FAULT = TCC_EVENT_ACTION_NON_RECOVERABLE_FAULT
};

/**
 * \brief Action to perform when the TCC module is triggered by event1
 *
 * Event action to perform when the module is triggered by event1.
 */
enum tcc_event1_action {
	/** No event action */
	TCC_EVENT1_ACTION_OFF                   = TCC_EVENT_ACTION_OFF,
	/** Re-trigger Counter on event */
	TCC_EVENT1_ACTION_RETRIGGER             = TCC_EVENT_ACTION_RETRIGGER,
	/** The event source must be an asynchronous event, and the input value
	 *  will override the direction settings.
	 *  If TCEINVx is 0 and input event is LOW: counter will count up.
	 *  If TCEINVx is 0 and input event is HIGH: counter will count down.
	 */
	TCC_EVENT1_ACTION_DIR_CONTROL           = TCC_EVENT_ACTION_DIR_CONTROL,
	/** Stop counter on event */
	TCC_EVENT1_ACTION_STOP                  = TCC_EVENT_ACTION_STOP,
	/** Decrement on event */
	TCC_EVENT1_ACTION_DECREMENT             = TCC_EVENT_ACTION_DECREMENT,

	/** Store period in capture register 0, pulse width in capture
	 *  register 1
	 */
	TCC_EVENT1_ACTION_PERIOD_PULSE_WIDTH_CAPTURE  = TCC_EVENT_ACTION_PERIOD_PULSE_WIDTH_CAPTURE,
	/** Store pulse width in capture register 0, period in capture
	 *  register 1
	 */
	TCC_EVENT1_ACTION_PULSE_WIDTH_PERIOD_CAPTURE  = TCC_EVENT_ACTION_PULSE_WIDTH_PERIOD_CAPTURE,

	/** Generate Non-Recoverable Fault on event */
	TCC_EVENT1_ACTION_NON_RECOVERABLE_FAULT = TCC_EVENT_ACTION_NON_RECOVERABLE_FAULT
};

/**
 * \brief On which part of the counter cycle the counter event output is generated
 *
 * This enum is used to define the point at which the counter event is generated.
 */
enum tcc_event_generation_selection {
	/** Counter Event is generated when a new counter cycle starts */
	TCC_EVENT_GENERATION_SELECTION_START,
	/** Counter Event is generated when a counter cycle ends */
	TCC_EVENT_GENERATION_SELECTION_END,
	/** Counter Event is generated when a counter cycle ends, except for the
	 *  first and last cycles */
	TCC_EVENT_GENERATION_SELECTION_BETWEEN,
	/** Counter Event is generated when a new counter cycle starts or ends */
	TCC_EVENT_GENERATION_SELECTION_BOUNDARY
};

/**
 * \brief TCC channel operation modes
 *
 * To set a timer channel either in compare or in capture mode.
 */
enum tcc_channel_function {
	/** TCC channel performs compare operation */
	TCC_CHANNEL_FUNCTION_COMPARE,
	/** TCC channel performs capture operation */
	TCC_CHANNEL_FUNCTION_CAPTURE
};

/**
 * \brief TCC (recoverable) fault Halt action
 */
enum tcc_fault_halt_action {
	/** Halt action disabled. */
	TCC_FAULT_HALT_ACTION_DISABLE,
	/** Hardware halt action, counter is halted until restart */
	TCC_FAULT_HALT_ACTION_HW_HALT,
	/** Software halt action, counter is halted until fault bit cleared */
	TCC_FAULT_HALT_ACTION_SW_HALT,
	/** Non-Recoverable fault, force output to pre-defined level */
	TCC_FAULT_HALT_ACTION_NON_RECOVERABLE
};

/**
 * \brief TCC (recoverable) fault Capture action
 */
enum tcc_fault_capture_action {
	/** Capture disabled */
	TCC_FAULT_CAPTURE_DISABLE,
	/** Capture on Fault, each value is captured */
	TCC_FAULT_CAPTURE_EACH,
	/** Capture the minimum detection, but notify on smaller ones */
	TCC_FAULT_CAPTURE_MINIMUM,
	/** Capture the maximum detection, but notify on bigger ones */
	TCC_FAULT_CAPTURE_MAXIMUM,
	/** Capture if the value is smaller than last, notify event or interrupt
	 *  if previous stamp is confirmed to be "local minimum" (not bigger than
	 *  current stamp). */
	TCC_FAULT_CAPTURE_SMALLER,
	/** Capture if the value is bigger than last, notify event or interrupt
	 *  if previous stamp is confirmed to be "local maximum" (not smaller than
	 *  current stamp). */
	TCC_FAULT_CAPTURE_BIGGER,
	/** Capture if the time stamps changes its increment direction */
	TCC_FAULT_CAPTURE_CHANGE
};

/**
 * \brief Capture Channel triggered by TCC (recoverable) fault
 */
enum tcc_fault_capture_channel {
	/** Recoverable fault triggers channel 0 capture operation */
	TCC_FAULT_CAPTURE_CHANNEL_0,
	/** Recoverable fault triggers channel 1 capture operation */
	TCC_FAULT_CAPTURE_CHANNEL_1,
	/** Recoverable fault triggers channel 2 capture operation */
	TCC_FAULT_CAPTURE_CHANNEL_2,
	/** Recoverable fault triggers channel 3 capture operation */
	TCC_FAULT_CAPTURE_CHANNEL_3
};

/**
 * \brief TCC (recoverable) fault Input Source
 */
enum tcc_fault_source {
	/** Fault input is disabled */
	TCC_FAULT_SOURCE_DISABLE,
	/** Match Capture Event x (x=0,1) input */
	TCC_FAULT_SOURCE_ENABLE,
	/** Inverted MCEx (x=0,1) event input */
	TCC_FAULT_SOURCE_INVERT,
	/** Alternate fault (A or B) state at the end of the previous period */
	TCC_FAULT_SOURCE_ALTFAULT
};

/**
 * \brief TCC (recoverable) fault Input Blanking Start Point
 */
enum tcc_fault_blanking {
	/** No blanking */
	TCC_FAULT_BLANKING_DISABLE,
	/** Blanking applied from rising edge of the output waveform */
	TCC_FAULT_BLANKING_RISING_EDGE,
	/** Blanking applied from falling edge of the output waveform */
	TCC_FAULT_BLANKING_FALLING_EDGE,
	/** Blanking applied from each toggle of the output waveform */
	TCC_FAULT_BLANKING_BOTH_EDGE
};

/**
 * \brief TCC (recoverable) fault Input Qualification Action
 */
enum tcc_fault_qualification {
	/** The input is not disabled on compare condition */
	TCC_FAULT_QUALIFICATION_DISABLE,
	/** The input is disabled when match output signal is at inactive level */
	TCC_FAULT_QUALIFICATION_BY_OUTPUT
};

/**
 * \brief TCC (recoverable) fault Output Keep Action
 */
enum tcc_fault_keep {
	/** Disable keeping, wave output released as soon as fault is released */
	TCC_FAULT_KEEP_DISABLE,
	/** Keep wave output until end of TCC cycle */
	TCC_FAULT_KEEP_TILL_END
};

/**
 * \brief TCC Non-recoverable State Outupt
 */
enum tcc_fault_state_output {
	/** Non-recoverable fault output is tri-stated */
	TCC_FAULT_STATE_OUTPUT_OFF,
	/** Non-recoverable fault force output 0 */
	TCC_FAULT_STATE_OUTPUT_0,
	/** Non-recoverable fault force output 1 */
	TCC_FAULT_STATE_OUTPUT_1
};

/**
 * \brief TCC (recoverable) fault Restart Action
 */
enum tcc_fault_restart {
	/** Restart Action disabled */
	TCC_FAULT_RESTART_DISABLE,
	/** Restart Action enabled */
	TCC_FAULT_RESTART_ENABLE
};

/**
 * \brief Configuration struct for TCC module recoverable fault
 */
struct tcc_recoverable_fault_config {
	/** Fault filter value applied on MCEx event input line (0x0 ~ 0xF).
	 *  Must be 0 when MCEx event is used as synchronous event.
	 *  Apply to both recoverable and non-recoverable fault. */
	uint8_t filter_value;
	/** Fault blanking value (0 ~ 255), disable input source for several TCC
	 * clocks after the detection of the waveform edge */
	uint8_t blanking_cycles;

	/** Set to \c true to enable restart action */
	bool restart;
	/** Set to \c true to enable keep action (keep until end of TCC cycle) */
	bool keep;

	/** Set to \c true to enable input qualification
	 *  (disable input when output is inactive) */
	bool qualification;

	/** Specifies if the event input generates recoverable Fault.
	 *  The event system channel connected to MCEx event input must be
	 *  configured as asynchronous.
	 */
	enum tcc_fault_source source;
	/** Fault Blanking Start Point for recoverable Fault */
	enum tcc_fault_blanking blanking;

	/** Halt action for recoverable Fault */
	enum tcc_fault_halt_action halt_action;
	/** Capture action for recoverable Fault */
	enum tcc_fault_capture_action capture_action;
	/** Channel triggered by recoverable Fault */
	enum tcc_fault_capture_channel capture_channel;
};

/**
 * \brief Configuration struct for TCC module non-recoverable fault
 */
struct tcc_non_recoverable_fault_config {
	/** Fault filter value applied on TCEx event input line (0x0 ~ 0xF).
	 *  Must be 0 when TCEx event is used as synchronous event. */
	uint8_t filter_value;
	/** Output */
	enum tcc_fault_state_output output;
};

/**
 * \brief TCC input event enable/disable/configure structure
 *
 * For configuring an input event.
 */
struct tcc_input_event_config {
	/** Event action on incoming event */
	enum tcc_event_action action;
	/** Modify event action */
	bool modify_action;
	/** Invert incoming event input line */
	bool invert;
};

/**
 * \brief TCC output event enable/disable/configure structure
 *
 * Structure used for configuring an output event.
 */
struct tcc_output_event_config {
	/** It decides which part of the counter cycle the counter event output
	 * is generated */
	enum tcc_event_generation_selection generation_selection;
	/** A switch to allow enable/disable of events, without modifying the
	 *  event output configuration
	 */
	bool modify_generation_selection;
};

/**
 * \brief TCC event enable/disable structure
 *
 * Event flags for the \ref tcc_enable_events() and \ref tcc_disable_events().
 */
struct tcc_events {
	/** Input events configuration */
	struct tcc_input_event_config input_config[2];
	/** Output event configuration */
	struct tcc_output_event_config output_config;

	/** Perform the configured event action when an incoming event is
	 *  signalled */
	bool on_input_event_perform_action[2];

	/** Perform the configured event action when an incoming channel event is
	 *  signalled */
	bool on_event_perform_channel_action[TCC_NUM_CHANNELS];
	/** Generate an output event on a channel capture/match.
	 *  Specify which channels will generate events */
	bool generate_event_on_channel[TCC_NUM_CHANNELS];

	/** Generate an output event on counter overflow/underflow */
	bool generate_event_on_counter_overflow;
	/** Generate an output event on counter retrigger */
	bool generate_event_on_counter_retrigger;
	/** Generate an output event on counter boundary.
	 *  See \ref tcc_event_output_action. */
	bool generate_event_on_counter_event;
};

/**
 * \brief Configuration struct for the TCC module base counter
 *
 * Structure for configuring a TCC as a counter.
 */
struct tcc_counter_config {
	/** Value to initialize the count register */
	uint32_t count;
	/** Period/top and period/top buffer values for counter */
	uint32_t period;

	/** When \c true, the counter will be stopped on the next hardware or
	 * software re-trigger event or overflow/underflow
	 */
	bool oneshot;

#ifdef FEATURE_TCC_GENERATE_DMA_TRIGGER	
	/** Counter overflow trigger a DMA request mode */
	enum tcc_count_overflow_dma_trigger_mode dma_trigger_mode;
#endif

	/** Specifies the direction for the TCC to count */
	enum tcc_count_direction direction;

	/** GCLK generator used to clock the peripheral */
	enum gclk_generator clock_source;
	/** Specifies the prescaler value for GCLK_TCC */
	enum tcc_clock_prescaler clock_prescaler;
	/** Specifies the reload or reset time of the counter and prescaler
	 *  resynchronization on a re-trigger event for the TCC
	 */
	enum tcc_reload_action reload_action;
};

/**
 * \brief Configuration struct for the TCC module capture
 *
 * Structure used when configuring TCC channels in capture mode.
 */
struct tcc_capture_config {
	/** Channel functions selection (capture/match) */
	enum tcc_channel_function channel_function[TCC_NUM_CHANNELS];
};

/**
 * \brief Configuration struct for the TCC module match/wave generation
 *
 * The structure, which helps to configure a TCC channel for compare
 * operation and wave generation.
 */
struct tcc_match_wave_config {
	/** Channel functions selection (capture/match) */
	enum tcc_channel_function channel_function[TCC_NUM_CHANNELS];

	/** Specifies polarity for match output waveform generation */
	enum tcc_wave_polarity wave_polarity[TCC_NUM_CHANNELS];
	/** Specifies which waveform generation mode to use */
	enum tcc_wave_generation wave_generation;
	/** Specifies Ramp mode for waveform generation */
	enum tcc_ramp wave_ramp;

	/** Value to be used for compare match on each channel */
	uint32_t match[TCC_NUM_CHANNELS];
};

/**
 * \brief Configuration struct for the TCC module waveform extension
 *
 * This structure is used to specify the waveform extension features for TCC.
 */
struct tcc_wave_extension_config {
	/** Configuration for recoverable faults */
	struct tcc_recoverable_fault_config
			recoverable_fault[TCC_NUM_FAULTS];
	/** Configuration for non-recoverable faults */
	struct tcc_non_recoverable_fault_config
			non_recoverable_fault[TCC_NUM_WAVE_OUTPUTS];

	/** Invert waveform final outputs lines */
	bool invert[TCC_NUM_WAVE_OUTPUTS];
};

/**
 * \brief Configuration struct for the TCC module output pins
 *
 * Structure which is used when taking wave output from TCC.
 */
struct tcc_pins_config {
	/** Specifies pin output for each channel */
	uint32_t wave_out_pin[TCC_NUM_WAVE_OUTPUTS];
	/** Specifies MUX setting for each output channel pin */
	uint32_t wave_out_pin_mux[TCC_NUM_WAVE_OUTPUTS];
	/** When \c true, PWM output pin for the given channel is enabled */
	bool enable_wave_out_pin[TCC_NUM_WAVE_OUTPUTS];
};

/**
 * \brief TCC configuration structure
 *
 * Configuration struct for a TCC instance. This structure should be
 * initialized by the \ref tcc_get_config_defaults function before being
 * modified by the user application.
 */
struct tcc_config {
	/** Structure for configuring TCC base timer/counter */
	struct tcc_counter_config counter;
	/** TCC match/capture configurations */
	union {
		/** Helps to configure a TCC channel in capture mode */
		struct tcc_capture_config capture;
		/** For configuring a TCC channel in compare mode */
		struct tcc_match_wave_config compare;
		/** Serves the same purpose as compare. Used as an alias for
		 * compare,
		 *  when a TCC channel is configured for wave generation */
		struct tcc_match_wave_config wave;
	};

	/** Structure for configuring TCC waveform extension */
	struct tcc_wave_extension_config wave_ext;

	/** Structure for configuring TCC output pins */
	struct tcc_pins_config pins;

	/** Set to \c true to enable double buffering write. When enabled any write
	 *  through \ref tcc_set_top_value(), \ref tcc_set_compare_value() and
	 *  \ref tcc_set_pattern() will direct to the buffer register as buffered
	 *  value, and the buffered value will be committed to effective register
	 *  on UPDATE condition, if update is not locked.
	 *
	 *  \note The init values in \ref tcc_config for \ref tcc_init are always
	 *        filled to effective registers, no matter if double buffering is
	 *        enabled or not.
	 */
	bool double_buffering_enabled;

	/** When \c true the module is enabled during standby */
	bool run_in_standby;
};

#if TCC_ASYNC == true
/* Forward Declaration for the device instance. */
struct tcc_module;

/** Type definition for the TCC callback function. */
typedef void (*tcc_callback_t)(struct tcc_module *const module);
#endif

/**
 * \brief TCC software device instance structure
 *
 * TCC software instance structure, used to retain software state information
 * of an associated hardware module instance.
 *
 * \note The fields of this structure should not be altered by the user
 *       application; they are reserved only for module-internal use.
 */
struct tcc_module {
	/** Hardware module pointer of the associated Timer/Counter peripheral. */
	Tcc *hw;

#  if TCC_ASYNC == true
	/** Array of callbacks */
	tcc_callback_t callback[TCC_CALLBACK_N];
	/** Bit mask for callbacks registered */
	uint32_t register_callback_mask;
	/** Bit mask for callbacks enabled */
	uint32_t enable_callback_mask;
#  endif

	/** Set to \c true to write to buffered registers */
	bool double_buffering_enabled;
};

#if !defined(__DOXYGEN__)
uint8_t _tcc_get_inst_index(
		Tcc *const hw);
#endif

/**
 * \name Driver Initialization and Configuration
 * @{
 */

/**
 * \brief Determines if the hardware module is currently synchronizing to the bus
 *
 * Checks to see if the underlying hardware peripheral module is currently
 * synchronizing across multiple clock domains to the hardware bus. This
 * function can be used to delay further operations on a module until such time
 * that it is ready, to prevent blocking delays for synchronization in the
 * user application.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 *
 * \return Synchronization status of the underlying hardware module.
 *
 * \retval false If the module has completed synchronization
 * \retval true  If the module synchronization is ongoing
 */
static inline bool tcc_is_syncing(
		const struct tcc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	return (module_inst->hw->SYNCBUSY.reg > 0);
}


void tcc_get_config_defaults(
		struct tcc_config *const config,
		Tcc *const hw);

enum status_code tcc_init(
		struct tcc_module *const module_inst,
		Tcc *const hw,
		const struct tcc_config *const config);

/** @} */

/**
 * \name Event Management
 * @{
 */

enum status_code tcc_enable_events(
		struct tcc_module *const module_inst,
		struct tcc_events *const events);

void tcc_disable_events(
		struct tcc_module *const module_inst,
		struct tcc_events *const events);

/** @} */

/**
 * \name Enable/Disable/Reset
 * @{
 */

/**
 * \brief Enable the TCC module
 *
 * Enables a TCC module that has been previously initialized. The counter will
 * start when the counter is enabled.
 *
 * \note When the counter is configured to re-trigger on an event, the counter
 *       will not start until the next incoming event appears. Then it
 *       restarts on any following event.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 */
static inline void tcc_enable(
		const struct tcc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	Tcc *const tcc_module = module_inst->hw;

	while (tcc_module->SYNCBUSY.reg & TCC_SYNCBUSY_ENABLE) {
		/* Wait for sync */
	}

	/* Enable the TCC module */
	tcc_module->CTRLA.reg |= TCC_CTRLA_ENABLE;
}

/**
 * \brief Disables the TCC module
 *
 * Disables a TCC module and stops the counter.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 */
static inline void tcc_disable(
		const struct tcc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	Tcc *const tcc_module = module_inst->hw;

	while (tcc_module->SYNCBUSY.reg & TCC_SYNCBUSY_ENABLE) {
		/* Wait for sync */
	}

	/* Disbale interrupt */
	tcc_module->INTENCLR.reg = TCC_INTENCLR_MASK;
	/* Clear interrupt flag */
	tcc_module->INTFLAG.reg = TCC_INTFLAG_MASK;

	/* Disable the TCC module */
	tcc_module->CTRLA.reg  &= ~TC_CTRLA_ENABLE;
}

/**
 * \brief Resets the TCC module
 *
 * Resets the TCC module, restoring all hardware module registers to their
 * default values and disabling the module. The TCC module will not be
 * accessible while the reset is being performed.
 *
 * \note When resetting a 32-bit counter only the master TCC module's instance
 *       structure should be passed to the function.
 *
 * \param[in]  module_inst    Pointer to the software module instance struct
 *
 */
static inline void tcc_reset(
		const struct tcc_module *const module_inst)
{
	/* Sanity check arguments  */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module hardware instance */
	Tcc *const tcc_module = module_inst->hw;

	/* Disable this module if it is running */
	if (tcc_module->CTRLA.reg & TCC_CTRLA_ENABLE) {
		tcc_disable(module_inst);
		while (tcc_is_syncing(module_inst)) {
			/* wait while module is disabling */
		}
	}

	/* Reset this TC module */
	tcc_module->CTRLA.reg  |= TCC_CTRLA_SWRST;
}

/** @} */


/**
 * \name Set/Toggle Count Direction
 * @{
 */

/**
 * \brief Sets the TCC module count direction
 *
 * Sets the count direction of an initialized TCC module. The
 * specified TCC module can remain running or stopped.
 *
 * \param[in] module_inst  Pointer to the software module instance struct
 * \param[in] dir          New timer count direction to set
 */
static inline void tcc_set_count_direction(
		const struct tcc_module *const module_inst,
		enum tcc_count_direction dir)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	Tcc *const tcc_module = module_inst->hw;

	while (tcc_module->SYNCBUSY.reg & TCC_SYNCBUSY_CTRLB) {
		/* Wait for sync */
	}

	/* Set count direction */
	if (TCC_COUNT_DIRECTION_DOWN == dir) {
		tcc_module->CTRLBSET.reg = TCC_CTRLBSET_DIR;
		return;
	}
	tcc_module->CTRLBCLR.reg = TCC_CTRLBCLR_DIR;
}

/**
 * \brief Toggles the TCC module count direction
 *
 * Toggles the count direction of an initialized TCC module. The
 * specified TCC module can remain running or stopped.
 *
 * \param[in] module_inst  Pointer to the software module instance struct
 */
static inline void tcc_toggle_count_direction(
		const struct tcc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	Tcc *const tcc_module = module_inst->hw;

	while (tcc_module->SYNCBUSY.reg & TCC_SYNCBUSY_CTRLB) {
		/* Wait for sync */
	}
	bool dir_value_1 = tcc_module->CTRLBSET.bit.DIR;
	if (dir_value_1) {
		tcc_module->CTRLBCLR.reg = TCC_CTRLBCLR_DIR;
	} else {
		tcc_module->CTRLBSET.reg = TCC_CTRLBSET_DIR;
	}
}

/** @} */

/**
 * \name Get/Set Count Value
 * @{
 */

uint32_t tcc_get_count_value(
		const struct tcc_module *const module_inst);

enum status_code tcc_set_count_value(
		const struct tcc_module *const module_inst,
		const uint32_t count);

/** @} */

/**
 * \name Stop/Restart Counter
 * @{
 */

/**
 * \brief Stops the counter
 *
 * This function will stop the counter. When the counter is stopped
 * the value in the count register is set to 0 if the counter was
 * counting up, or maximum or the top value if the counter was counting
 * down.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 */
static inline void tcc_stop_counter(
		const struct tcc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	Tcc *const tcc_module = module_inst->hw;
	uint32_t last_cmd;

	/* Wait until last command is done */
	do {
		while (tcc_module->SYNCBUSY.reg & TCC_SYNCBUSY_CTRLB) {
			/* Wait for sync */
		}
		last_cmd = tcc_module->CTRLBSET.reg & TCC_CTRLBSET_CMD_Msk;
		if (last_cmd == TCC_CTRLBSET_CMD_NONE) {
			break;
		} else if (last_cmd == TCC_CTRLBSET_CMD_STOP) {
			/* Command have been issued */
			return;
		} else if (last_cmd == TCC_CTRLBSET_CMD_RETRIGGER) {
			/* Cancel RETRIGGER command and issue STOP */
			tcc_module->CTRLBCLR.reg = TCC_CTRLBCLR_CMD_Msk;
		}
	} while (1);

	/* Write command to execute */
	tcc_module->CTRLBSET.reg = TCC_CTRLBSET_CMD_STOP;
}

/**
 * \brief Starts the counter from beginning
 *
 * Restarts an initialized TCC module's counter.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 */
static inline void tcc_restart_counter(
		const struct tcc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	Tcc *const tcc_module = module_inst->hw;
	uint32_t last_cmd;

	/* Wait until last command is done */
	do {
		while (tcc_module->SYNCBUSY.reg & TCC_SYNCBUSY_CTRLB) {
			/* Wait for sync */
		}
		last_cmd = tcc_module->CTRLBSET.reg & TCC_CTRLBSET_CMD_Msk;
		if (last_cmd == TCC_CTRLBSET_CMD_NONE) {
			break;
		} else if (last_cmd == TCC_CTRLBSET_CMD_RETRIGGER) {
			/* Command have been issued */
			return;
		} else if (last_cmd == TCC_CTRLBSET_CMD_STOP) {
			/* Cancel STOP command and issue RETRIGGER */
			tcc_module->CTRLBCLR.reg = TCC_CTRLBCLR_CMD_Msk;
		}
	} while (1);

	/* Write command to execute */
	tcc_module->CTRLBSET.reg = TCC_CTRLBSET_CMD_RETRIGGER;
}

/** @} */

#ifdef FEATURE_TCC_GENERATE_DMA_TRIGGER
/**
 * \name Generate TCC DMA Triggers Command
 * @{
 */

/**
 * \brief TCC DMA Trigger.
 *
 * TCC DMA trigger command.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 */
static inline void tcc_dma_trigger_command(
		const struct tcc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	Tcc *const tcc_module = module_inst->hw;

	while (tcc_module->SYNCBUSY.reg & TCC_SYNCBUSY_CTRLB) {
			/* Wait for sync */
	}

	/* Make certain that there are no conflicting commands in the register */
	tcc_module->CTRLBCLR.reg = TCC_CTRLBCLR_CMD_NONE;

	while (tcc_module->SYNCBUSY.reg & TCC_SYNCBUSY_CTRLB) {
			/* Wait for sync */
	}
	
#if !(SAML21 || SAML22 || SAMR30)
	/* Write command to execute */
	tcc_module->CTRLBSET.reg = TCC_CTRLBSET_CMD_DMATRG;
#endif

#if (SAML21XXXB) || (SAML22) || (SAMR30)
	/* Write command to execute */
	tcc_module->CTRLBSET.reg = TCC_CTRLBSET_CMD_DMAOS;
#endif
}
/** @} */
#endif

/**
 * \name Get/Set Compare/Capture Register
 * @{
 */

uint32_t tcc_get_capture_value(
		const struct tcc_module *const module_inst,
		const enum tcc_match_capture_channel channel_index);

enum status_code tcc_set_compare_value(
		const struct tcc_module *const module_inst,
		const enum tcc_match_capture_channel channel_index,
		const uint32_t compare);

/** @} */

/**
 * \name Set Top Value
 * @{
 */

enum status_code tcc_set_top_value(
		const struct tcc_module *const module_inst,
		const uint32_t top_value);

/** @} */


/**
 * \name Set Output Pattern
 * @{
 */

enum status_code tcc_set_pattern(
		const struct tcc_module *const module_inst,
		const uint32_t line_index,
		const enum tcc_output_pattern pattern);

/** @} */


/**
 * \name Set Ramp Index
 * @{
 */

/**
 * \brief Sets the TCC module ramp index on next cycle
 *
 * In RAMP2 and RAMP2A operation, we can force either cycle A or cycle B at
 * the output, on the next clock cycle.
 * When ramp index command is disabled, cycle A and cycle B will appear at
 * the output, on alternate clock cycles.
 * See \ref tcc_ramp.
 *
 * \param[in]  module_inst Pointer to the software module instance struct
 * \param[in]  ramp_index  Ramp index (\ref tcc_ramp_index) of the next cycle
 */
static inline void tcc_set_ramp_index(
		const struct tcc_module *const module_inst,
		const enum tcc_ramp_index ramp_index)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	Tcc *const tcc_module = module_inst->hw;
	uint32_t last_cmd;

	/* Wait until last command is done */
	do {
		while (tcc_module->SYNCBUSY.reg & TCC_SYNCBUSY_CTRLB) {
			/* Wait for sync */
		}
		if (TCC_RAMP_INDEX_DEFAULT == ramp_index) {
			/* Cancel pending command */
			tcc_module->CTRLBCLR.reg = TCC_CTRLBSET_IDXCMD_HOLD;
			return;
		}
		last_cmd = tcc_module->CTRLBSET.reg & TCC_CTRLBSET_IDXCMD_Msk;
		if (last_cmd == TCC_CTRLBSET_IDXCMD_DISABLE) {
			break;
		} else if (last_cmd == TCC_CTRLBSET_IDXCMD(ramp_index)) {
			/* Command have been issued */
			return;
		}
	} while (1);

	/* Write command to execute */
	tcc_module->CTRLBSET.reg = TCC_CTRLBSET_IDXCMD(ramp_index);
}

/** @} */

/**
 * \name Status Management
 * @{
 */

/**
 * \brief Checks if the timer/counter is running
 *
 * \param[in] module_inst  Pointer to the TCC software instance struct
 *
 * \return Status which indicates whether the module is running.
 *
 * \retval true The timer/counter is running
 * \retval false The timer/counter is stopped
 */
static inline bool tcc_is_running(
		struct tcc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	return !module_inst->hw->STATUS.bit.STOP;
}

uint32_t tcc_get_status(
		struct tcc_module *const module_inst);

void tcc_clear_status(
		struct tcc_module *const module_inst,
		const uint32_t status_flags);

/** @} */

/**
 * \name Double Buffering Management
 * @{
 */

/**
 * \brief Enable TCC double buffering write
 *
 * When double buffering write is enabled, the following function will write
 * values to buffered registers instead of effective ones (buffered):
 * - PERB: through \ref tcc_set_top_value()
 * - CCBx(x is 0~3): through \ref tcc_set_compare_value()
 * - PATTB: through \ref tcc_set_pattern()
 *
 * Then, on UPDATE condition the buffered registers are committed to regular ones
 * to take effect.
 *
 * \param[in] module_inst  Pointer to the TCC software instance struct
 */
static inline void tcc_enable_double_buffering(
		struct tcc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);

	module_inst->double_buffering_enabled = true;
}

/**
 * \brief Disable TCC double buffering Write
 *
 * When double buffering write is disabled, following function will write values
 * to effective registers (not buffered):
 * - PER: through \ref tcc_set_top_value()
 * - CCx(x is 0~3): through \ref tcc_set_compare_value()
 * - PATT: through \ref tcc_set_pattern()
 *
 * \note This function does not lock double buffer update, which means on next
 *       UPDATE condition the last written buffered values will be committed to
 *       take effect. Invoke \ref tcc_lock_double_buffer_update() before this
 *       function to disable double buffering update, if this change is not
 *       expected.
 *
 * \param[in] module_inst  Pointer to the TCC software instance struct
 */
static inline void tcc_disable_double_buffering(
		struct tcc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	module_inst->double_buffering_enabled = false;
}

/**
 * \brief Lock the TCC double buffered registers updates
 *
 * Locks the double buffered registers so they will not be updated through
 * their buffered values on UPDATE conditions.
 *
 * \param[in] module_inst  Pointer to the TCC software instance struct
 *
 */
static inline void tcc_lock_double_buffer_update(
		struct tcc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	while (module_inst->hw->SYNCBUSY.reg & TCC_SYNCBUSY_CTRLB) {
		/* Wait for sync */
	}
	module_inst->hw->CTRLBSET.reg = TCC_CTRLBSET_LUPD;
}

/**
 * \brief Unlock the TCC double buffered registers updates
 *
 * Unlock the double buffered registers so they will be updated through
 * their buffered values on UPDATE conditions.
 *
 * \param[in] module_inst  Pointer to the TCC software instance struct
 *
 */
static inline void tcc_unlock_double_buffer_update(
		struct tcc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	while (module_inst->hw->SYNCBUSY.reg & TCC_SYNCBUSY_CTRLB) {
		/* Wait for sync */
	}
	module_inst->hw->CTRLBCLR.reg = TCC_CTRLBCLR_LUPD;
}

/**
 * \brief Force the TCC double buffered registers to update once
 *
 * \param[in] module_inst  Pointer to the TCC software instance struct
 *
 */
static inline void tcc_force_double_buffer_update(
		struct tcc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Get a pointer to the module's hardware instance */
	Tcc *const tcc_module = module_inst->hw;
	uint32_t last_cmd;

	/* Wait until last command is done */
	do {
		while (tcc_module->SYNCBUSY.reg & TCC_SYNCBUSY_CTRLB) {
			/* Wait for sync */
		}
		last_cmd = tcc_module->CTRLBSET.reg & TCC_CTRLBSET_CMD_Msk;
		if (last_cmd == TCC_CTRLBSET_CMD_NONE) {
			break;
		} else if (last_cmd == TCC_CTRLBSET_CMD_UPDATE) {
			/* Command have been issued */
			return;
		}
	} while (1);

	/* Write command to execute */
	tcc_module->CTRLBSET.reg = TCC_CTRLBSET_CMD_UPDATE;
}

/**
 * \brief Enable Circular option for double buffered Top/Period Values
 *
 * Enable circular option for the double buffered top/period values.
 * On each UPDATE condition, the contents of PERB and PER are switched, meaning
 * that the contents of PERB are transferred to PER and the contents of PER are
 * transferred to PERB.
 *
 * \param[in] module_inst     Pointer to the TCC software instance struct
 */
static inline void tcc_enable_circular_buffer_top(
		struct tcc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	module_inst->hw->WAVE.reg |=  TCC_WAVE_CIPEREN;
}

/**
 * \brief Disable Circular option for double buffered Top/Period Values
 *
 * Stop circularing the double buffered top/period values.
 *
 * \param[in] module_inst     Pointer to the TCC software instance struct
 */
static inline void tcc_disable_circular_buffer_top(
		struct tcc_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	module_inst->hw->WAVE.reg &= ~TCC_WAVE_CIPEREN;
}

enum status_code tcc_set_double_buffer_top_values(
		const struct tcc_module *const module_inst,
		const uint32_t top_value, const uint32_t top_buffer_value);


enum status_code tcc_enable_circular_buffer_compare(
		struct tcc_module *const module_inst,
		enum tcc_match_capture_channel channel_index);
enum status_code tcc_disable_circular_buffer_compare(
		struct tcc_module *const module_inst,
		enum tcc_match_capture_channel channel_index);
enum status_code tcc_set_double_buffer_compare_values(
		struct tcc_module *const module_inst,
		enum tcc_match_capture_channel channel_index,
		const uint32_t compare,
		const uint32_t compare_buffer);


/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

/**
 * \page asfdoc_sam0_tcc_extra Extra Information for TCC Driver
 *
 * \section asfdoc_sam0_tcc_extra_acronyms Acronyms
 * The table below presents the acronyms used in this module:
 *
 * <table>
 *  <tr>
 *      <th>Acronym</th>
 *      <th>Description</th>
 *  </tr>
  * <tr>
 *      <td>DMA</td>
 *      <td>Direct Memory Access</td>
 *  </tr>
 *  <tr>
 *      <td>TCC</td>
 *      <td>Timer Counter for Control Applications</td>
 *  </tr>
 *  <tr>
 *      <td>PWM</td>
 *      <td>Pulse Width Modulation</td>
 *  </tr>
 *  <tr>
 *      <td>PWP</td>
 *      <td>Pulse Width Period</td>
 *  </tr>
 *  <tr>
 *      <td>PPW</td>
 *      <td>Period Pulse Width</td>
 *  </tr>
 * </table>
 *
 *
 * \section asfdoc_sam0_tcc_extra_dependencies Dependencies
 * This driver has the following dependencies:
 *
 *  - \ref asfdoc_sam0_system_pinmux_group "System Pin Multiplexer Driver"
 *
 *
 * \section asfdoc_sam0_tcc_extra_errata Errata
 * There are no errata related to this driver.
 *
 *
 * \section asfdoc_sam0_tcc_extra_history Module History
 * An overview of the module history is presented in the table below, with
 * details on the enhancements and fixes made to the module since its first
 * release. The current version of this corresponds to the newest version in
 * the table.
 *
 * <table>
 *  <tr>
 *      <th>Changelog</th>
 *  </tr>
 *  <tr>
 *      <td>Add double buffering functionality</td>
 *  </tr>
 *  <tr>
 *      <td>Add fault handling functionality</td>
 *  </tr>
 *  <tr>
 *      <td>Initial Release</td>
 *  </tr>
 * </table>
 */

/**
 * \page asfdoc_sam0_tcc_exqsg Examples for TCC Driver
 *
 * This is a list of the available Quick Start guides (QSGs) and example
 * applications for \ref asfdoc_sam0_tcc_group. QSGs are simple examples with
 * step-by-step instructions to configure and use this driver in a selection of
 * use cases. Note that QSGs can be compiled as a standalone application or be
 * added to the user application.
 *
 *  - \subpage asfdoc_sam0_tcc_basic_use_case
 *  - \subpage asfdoc_sam0_tcc_buffering_use_case
 * \if TCC_CALLBACK_MODE
 *  - \subpage asfdoc_sam0_tcc_timer_use_case
 *  - \subpage asfdoc_sam0_tcc_callback_use_case
 *  - \subpage asfdoc_sam0_tcc_faultx_use_case
 *  - \subpage asfdoc_sam0_tcc_faultn_use_case
 * \endif
 *  - \subpage asfdoc_sam0_tcc_dma_use_case
 *
 * \page asfdoc_sam0_tcc_document_revision_history Document Revision History
 *
 * <table>
 *  <tr>
 *      <th>Doc. Rev.</th>
 *      <th>Date</th>
 *      <th>Comments</th>
 *  </tr>
 *  <tr>
 *      <td>42256C</td>
 *      <td>12/2015</td>
 *      <td>Added support for SAM L21/L22, SAM DA1, and SAM C20/C21</td>
 *  </tr>
 *  <tr>
 *      <td>42256B</td>
 *      <td>12/2014</td>
 *      <td>Added fault handling functionality.
 *          Added double buffering functionality with use case.
 *          Added timer use case.
 *          Added SAM R21/D10/D11 support.</td>
 *  </tr>
 *  <tr>
 *      <td>42256A</td>
 *      <td>01/2014</td>
 *      <td>Initial release</td>
 *  </tr>
 * </table>
 */

#endif /* TCC_H_INCLUDED */
