/**
 * \file
 *
 * \brief SAM Event System Driver
 *
 * Copyright (C) 2012-2016 Atmel Corporation. All rights reserved.
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
#ifndef EVENTS_H_INCLUDED
#define EVENTS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup asfdoc_sam0_events_group SAM Event System (EVENTS) Driver
 *
 * This driver for Atmel&reg; | SMART ARM&reg;-based microcontrollers provides
 * an interface for the configuration and management of the device's peripheral
 * event resources and users within the device, including enabling and disabling
 * of peripheral source selection and synchronization of clock domains between
 * various modules. The following API modes is covered by this manual:
 *  - Polled API
 * \if EVENTS_INTERRUPT_HOOK_MODE
 *  - Interrupt hook API
 * \endif
 *
 * The following peripheral is used by this module:
 * - EVSYS (Event System Management)
 *
 * The following devices can use this module:
 *  - Atmel | SMART SAM D20/D21
 *  - Atmel | SMART SAM R21
 *  - Atmel | SMART SAM D09/D10/D11
 *  - Atmel | SMART SAM L21/L22
 *  - Atmel | SMART SAM DA1
 *  - Atmel | SMART SAM C20/C21
 *  - Atmel | SMART SAM R30
 *
 * The outline of this documentation is as follows:
 * - \ref asfdoc_sam0_events_prerequisites
 * - \ref asfdoc_sam0_events_module_overview
 * - \ref asfdoc_sam0_events_special_considerations
 * - \ref asfdoc_sam0_events_extra_info
 * - \ref asfdoc_sam0_events_examples
 * - \ref asfdoc_sam0_events_api_overview
 *
 *
 * \section asfdoc_sam0_events_prerequisites Prerequisites
 *
 * There are no prerequisites for this module.
 *
 *
 * \section asfdoc_sam0_events_module_overview Module Overview
 *
 * Peripherals within the SAM devices are capable of generating two types of
 * actions in response to given stimulus; set a register flag for later
 * intervention by the CPU (using interrupt or polling methods), or generate
 * event signals, which can be internally routed directly to other
 * peripherals within the device. The use of events allows for direct actions
 * to be performed in one peripheral in response to a stimulus in another
 * without CPU intervention. This can lower the overall power consumption of the
 * system if the CPU is able to remain in sleep modes for longer periods
 * (SleepWalking), and lowers the latency of the system response.
 *
 * The event system is comprised of a number of freely configurable Event
 * resources, plus a number of fixed Event Users. Each Event resource can be
 * configured to select the input peripheral that will generate the events
 * signal, as well as the synchronization path and edge detection mode.
 * The fixed-function Event Users, connected to peripherals within the device,
 * can then subscribe to an Event resource in a one-to-many relationship in order
 * to receive events as they are generated. An overview of the event system
 * chain is shown in
 * \ref asfdoc_sam0_events_module_overview_fig "the figure below".
 *
 * \anchor asfdoc_sam0_events_module_overview_fig
 * \dot
 * digraph overview {
 * rankdir=LR;
 * node [label="Source\nPeripheral\n(Generator)" shape=ellipse style=filled fillcolor=lightgray] src_peripheral;
 * node [label="Event\nResource A" shape=square style=""] event_gen0;
 * node [label="Event\nUser X" shape=square style=""] event_user0;
 * node [label="Event\nUser Y" shape=square style=""] event_user1;
 * node [label="Destination\nPeripheral\n(User)" shape=ellipse style=filled fillcolor=lightgray] dst_peripheral0;
 * node [label="Destination\nPeripheral\n(User)" shape=ellipse style=filled fillcolor=lightgray] dst_peripheral1;
 *
 * src_peripheral -> event_gen0;
 * event_gen0 -> event_user0;
 * event_gen0 -> event_user1;
 * event_user0 -> dst_peripheral0;
 * event_user1 -> dst_peripheral1;
 * }
 * \enddot
 *
 * There are many different events that can be routed in the device, which can
 * then trigger many different actions. For example, an Analog Comparator module
 * could be configured to generate an event when the input signal rises above
 * the compare threshold, which then triggers a Timer Counter module to capture
 * the current count value for later use.
 *
 * \subsection asfdoc_sam0_events_module_overview_event_channels Event Channels
 * The Event module in each device consists of several channels, which can be
 * freely linked to an event generator (i.e. a peripheral within the device
 * that is capable of generating events). Each channel can be individually
 * configured to select the generator peripheral, signal path, and edge detection
 * applied to the input event signal, before being passed to any event user(s).
 *
 * Event channels can support multiple users within the device in a standardized
 * manner. When an Event User is linked to an Event Channel, the channel will
 * automatically handshake with all attached users to ensure that all modules
 * correctly receive and acknowledge the event.
 *
 * \subsection asfdoc_sam0_events_module_overview_event_users Event Users
 * Event Users are able to subscribe to an Event Channel, once it has been
 * configured. Each Event User consists of a fixed connection to one of the
 * peripherals within the device (for example, an ADC module, or Timer module)
 * and is capable of being connected to a single Event Channel.
 *
 * \subsection asfdoc_sam0_events_module_overview_edge_detection Edge Detection
 * For asynchronous events, edge detection on the event input is not possible,
 * and the event signal must be passed directly between the event generator and
 * event user. For synchronous and re-synchronous events, the input signal from
 * the event generator must pass through an edge detection unit, so that only
 * the rising, falling, or both edges of the event signal triggers an action in
 * the event user.
 *
 * \subsection asfdoc_sam0_events_module_overview_path_selection Path Selection
 * The event system in the SAM devices supports three signal path types from
 * the event generator to Event Users: asynchronous, synchronous, and
 * re-synchronous events.
 *
 * \subsubsection asfdoc_sam0_events_module_overview_path_selection_async Asynchronous Paths
 * Asynchronous event paths allow for an asynchronous connection between the
 * event generator and Event Users, when the source and destination
 * peripherals share the same \ref asfdoc_sam0_system_clock_group "Generic Clock"
 * channel. In this mode the event is propagated between the source and
 * destination directly to reduce the event latency, thus no edge detection is
 * possible. The asynchronous event chain is shown in
 * \ref asfdoc_sam0_events_module_async_path_fig "the figure below".
 *
 * \anchor asfdoc_sam0_events_module_async_path_fig
 * \dot
 * digraph overview {
 *   rankdir=LR;
 *   node [label="Source\nPeripheral" shape=ellipse style=filled fillcolor=lightgray] src_peripheral;
 *   node [label="<f0> EVSYS | <f1> Event\nChannel/User" fillcolor=white style="dashed" shape=record] events_chan;
 *   node [label="Destination\nPeripheral" shape=ellipse style=filled fillcolor=lightgray] dst_peripheral;
 *
 *   src_peripheral -> events_chan;
 *   events_chan    -> dst_peripheral;
 *
 * }
 * \enddot
 * \note Identically shaped borders in the diagram indicate a shared generic clock channel.
 *
 * \subsubsection asfdoc_sam0_events_module_overview_path_selection_sync Synchronous Paths
 * The Synchronous event path should be used when edge detection or interrupts
 * from the event channel are required, and the source event generator and the
 * event channel shares the same Generic Clock channel. The synchronous event
 * chain is shown in
 * \ref asfdoc_sam0_events_module_sync_path_fig "the figure below".
 *
 * Not all peripherals support Synchronous event paths; refer to the device datasheet.
 *
 * \anchor asfdoc_sam0_events_module_sync_path_fig
 * \dot
 * digraph overview {
 *   rankdir=LR;
 *   node [label="Source\nPeripheral" shape=ellipse style="filled, dashed" fillcolor=lightgray] src_peripheral;
 *   node [label="<f0> EVSYS | <f1> Event\nChannel/User" fillcolor=white shape=record style="dashed"] events_chan;
 *   node [label="Destination\nPeripheral" shape=ellipse style="filled, solid" fillcolor=lightgray] dst_peripheral;
 *
 *   src_peripheral -> events_chan;
 *   events_chan    -> dst_peripheral;
 *
 * }
 * \enddot
 * \note Identically shaped borders in the diagram indicate a shared generic clock channel.
 *
 * \subsubsection asfdoc_sam0_events_module_overview_path_selection_resync Re-synchronous Paths
 * Re-synchronous event paths are a special form of synchronous events, where
 * when edge detection or interrupts from the event channel are required, but
 * the event generator and the event channel use different Generic Clock
 * channels. The re-synchronous path allows the Event System to synchronize the
 * incoming event signal from the Event Generator to the clock of the Event
 * System module to avoid missed events, at the cost of a higher latency due to
 * the re-synchronization process. The re-synchronous event chain is shown in
 * \ref asfdoc_sam0_events_module_resync_path_fig "the figure below".
 *
 * Not all peripherals support re-synchronous event paths; refer to the device datasheet.
 * \anchor asfdoc_sam0_events_module_resync_path_fig
 * \dot
 * digraph overview {
 *   rankdir=LR;
 *   node [label="Source\nPeripheral" shape=ellipse style="filled, dotted" fillcolor=lightgray] src_peripheral;
 *   node [label="<f0> EVSYS | <f1> Event\nChannel/User" fillcolor=white shape=record style="dashed"] events_chan;
 *   node [label="Destination\nPeripheral" shape=ellipse style=filled fillcolor=lightgray] dst_peripheral;
 *
 *   src_peripheral -> events_chan;
 *   events_chan    -> dst_peripheral;
 *
 * }
 * \enddot
 * \note Identically shaped borders in the diagram indicate a shared generic clock channel.
 *
 * \subsection asfdoc_sam0_events_module_overview_physical Physical Connection
 *
 * \ref asfdoc_sam0_events_module_int_connections_fig "The diagram below"
 * shows how this module is interconnected within the device.
 *
 * \anchor asfdoc_sam0_events_module_int_connections_fig
 * \dot
 * digraph overview {
 *   rankdir=LR;
 *   node [label="Source\nPeripherals" shape=ellipse style=filled fillcolor=lightgray] src_peripheral;
 *
 *   subgraph driver {
 *     node [label="<f0> EVSYS | <f1> Event Channels" fillcolor=white shape=record] events_chan;
 *     node [label="<f0> EVSYS | <f1> Event Users" fillcolor=white shape=record] events_user;
 *   }
 *
 *   node [label="Destination\nPeripherals" shape=ellipse style=filled fillcolor=lightgray] dst_peripheral;
 *
 *   src_peripheral -> events_chan:f1 [label="Source\nMUXs"];
 *   events_chan:f1 -> events_user:f1 [label="Channel\nMUXs"];
 *   events_user:f1 -> dst_peripheral;
 * }
 * \enddot
 *
 * \subsection asfdoc_sam0_events_module_overview_config Configuring Events
 * For SAM devices, several steps are required to properly configure an
 * event chain, so that hardware peripherals can respond to events generated by
 * each other, as listed below.
 *
 * \subsubsection asfdoc_sam0_events_module_overview_config_src Source Peripheral
 *  -# The source peripheral (that will generate events) must be configured and
 *     enabled.
 *  -# The source peripheral (that will generate events) must have an output
 *     event enabled.

 * \subsubsection asfdoc_sam0_events_module_overview_config_evsys Event System
 *  -# An event system channel must be allocated and configured with the
 *     correct source peripheral selected as the channel's event generator.
 *  -# The event system user must be configured and enabled, and attached to
 #     event channel previously allocated.

 * \subsubsection asfdoc_sam0_events_module_overview_config_dst Destination Peripheral
 *  -# The destination peripheral (that will receive events) must be configured
 *     and enabled.
 *  -# The destination peripheral (that will receive events) must have an input
 *     event enabled.
 *
 *
 * \section asfdoc_sam0_events_special_considerations Special Considerations
 *
 * There are no special considerations for this module.
 *
 *
 * \section asfdoc_sam0_events_extra_info Extra Information
 *
 * For extra information, see \ref asfdoc_sam0_events_extra. This includes:
 * - \ref asfdoc_sam0_events_extra_acronyms
 * - \ref asfdoc_sam0_events_extra_dependencies
 * - \ref asfdoc_sam0_events_extra_errata
 * - \ref asfdoc_sam0_events_extra_history
 *
 *
 * \section asfdoc_sam0_events_examples Examples
 *
 * For a list of examples related to this driver, see
 * \ref asfdoc_sam0_events_exqsg.
 *
 *
 * \section asfdoc_sam0_events_api_overview API Overview
 * @{
 */

#include <compiler.h>
#include "events_common.h"

/**
 * \brief Edge detect enum.
 *
 * Event channel edge detect setting.
 *
 */
enum events_edge_detect {
	/** No event output */
	EVENTS_EDGE_DETECT_NONE,
	/** Event on rising edge */
	EVENTS_EDGE_DETECT_RISING,
	/** Event on falling edge */
	EVENTS_EDGE_DETECT_FALLING,
	/** Event on both edges */
	EVENTS_EDGE_DETECT_BOTH,
};

/**
 * \brief Path selection enum.
 *
 * Event channel path selection.
 *
 */
enum events_path_selection {
	/** Select the synchronous path for this event channel */
	EVENTS_PATH_SYNCHRONOUS,
	/** Select the resynchronizer path for this event channel */
	EVENTS_PATH_RESYNCHRONIZED,
	/** Select the asynchronous path for this event channel */
	EVENTS_PATH_ASYNCHRONOUS,
};

/**
 * \brief Events configuration struct.
 *
 * This event configuration struct is used to configure each of the channels.
 *
 */
struct events_config {
	/** Select edge detection mode */
	enum events_edge_detect    edge_detect;
	/** Select events channel path */
	enum events_path_selection path;
	/** Set event generator for the channel */
	uint8_t                    generator;
	/** Clock source for the event channel */
	uint8_t                    clock_source;
#if (SAML21) || (SAML22) || (SAMC20) || (SAMC21) || (SAMR30)
	/** Run in standby mode for the channel */
	bool                       run_in_standby;
	/** Run On Demand */
	bool                       on_demand;
#endif
};

/**
 * \brief No event generator definition.
 *
 * Use this to disable any peripheral event input to a channel. This can be useful
 * if you only want to use a channel for software generated events.
 *
 */

///@cond INTERNAL
/**
 * \internal
 * Status bit offsets in the status register/interrupt register.
 *
 * @{
 */
#if (SAML21) || (SAML22) || (SAMC20) || (SAMC21) || (SAMR30)
#  define _EVENTS_START_OFFSET_BUSY_BITS           16
#  define _EVENTS_START_OFFSET_USER_READY_BIT      0
#  define _EVENTS_START_OFFSET_DETECTION_BIT       16
#  define _EVENTS_START_OFFSET_OVERRUN_BIT         0
#else /* SAM D/R */
#  define _EVENTS_START_OFFSET_BUSY_BITS           8
#  define _EVENTS_START_OFFSET_USER_READY_BIT      0
#  define _EVENTS_START_OFFSET_DETECTION_BIT       8
#  define _EVENTS_START_OFFSET_OVERRUN_BIT         0
#endif
/** @} */
///@endcond

/**
*  Definition for no generator selection.
*/
#define EVSYS_ID_GEN_NONE   0

/**
 * \brief Event channel resource.
 *
 * Event resource structure.
 *
 * \note The fields in this structure should not be altered by the user application;
 *       they are reserved for driver internals only.
 */
struct events_resource {
#if !defined(__DOXYGEN__)
	/** Channel allocated for the event resource */
	uint8_t channel;
	/** Channel setting in CHANNEL register */
	uint32_t channel_reg;
#endif
};

#if EVENTS_INTERRUPT_HOOKS_MODE == true
typedef void (*events_interrupt_hook)(struct events_resource *resource);

/**
 * \brief Event hook.
 *
 * Event hook structure.
 *
 */
struct events_hook {
	/** Event resource */
	struct events_resource *resource;
	/** Event hook function */
	events_interrupt_hook hook_func;
	/** Next event hook */
	struct events_hook *next;
};
#endif

/**
 * \brief Initializes an event configurations struct to defaults.
 *
 * Initailizes an event configuration struct to predefined safe default settings.
 *
 * \param[in] config Pointer to an instance of \ref struct events_config
 *
 */
void events_get_config_defaults(struct events_config *config);

/**
 * \brief Allocate an event channel and set configuration.
 *
 * Allocates an event channel from the event channel pool and sets
 * the channel configuration.
 *
 * \param[out] resource Pointer to a \ref events_resource struct instance
 * \param[in]  config   Pointer to a \ref events_config struct
 *
 * \return Status of the configuration procedure.
 * \retval STATUS_OK            Allocation and configuration went successful
 * \retval STATUS_ERR_NOT_FOUND No free event channel found
 *
 */
enum status_code events_allocate(struct events_resource *resource, struct events_config *config);

/**
 * \brief Attach user to the event channel.
 *
 * Attach a user peripheral to the event channel to receive events.
 *
 * \param[in] resource Pointer to an \ref events_resource struct instance
 * \param[in] user_id  A number identifying the user peripheral found in the device header file
 *
 * \return Status of the user attach procedure.
 * \retval STATUS_OK No errors detected when attaching the event user
 */
enum status_code events_attach_user(struct events_resource *resource, uint8_t user_id);

/**
 * \brief Detach a user peripheral from the event channel.
 *
 * Deattach a user peripheral from the event channels so it does not receive any more events.
 *
 * \param[in] resource Pointer to an \ref event_resource struct instance
 * \param[in] user_id  A number identifying the user peripheral found in the device header file
 *
 * \return Status of the user detach procedure.
 * \retval STATUS_OK No errors detected when detaching the event user
 */
enum status_code events_detach_user(struct events_resource *resource, uint8_t user_id);

/**
 * \brief Check if a channel is busy.
 *
 * Check if a channel is busy, a channel stays busy until all users connected to the channel
 * has handled an event.
 *
 * \param[in] resource Pointer to a \ref events_resource struct instance
 *
 * \return Status of the channels busy state.
 * \retval true   One or more users connected to the channel has not handled the last event
 * \retval false  All users are ready to handle new events
 */
bool events_is_busy(struct events_resource *resource);

/**
 * \brief Trigger software event.
 *
 * Trigger an event by software.
 *
 * \note Software event works on either a synchronous path or resynchronized path, and
 * edge detection must be configured to rising-edge detection.
 *
 * \param[in] resource Pointer to an \ref events_resource struct
 *
 * \return Status of the event software procedure.
 * \retval STATUS_OK No error was detected when the software tigger signal was issued
 * \retval STATUS_ERR_UNSUPPORTED_DEV If the channel path is asynchronous and/or the
 *                                    edge detection is not set to RISING
 */
enum status_code events_trigger(struct events_resource *resource);

/**
 * \brief Check if all users connected to the channel are ready.
 *
 * Check if all users connected to the channel are ready to handle incoming events.
 *
 * \param[in] resource Pointer to an \ref events_resource struct
 *
 * \return The ready status of users connected to an event channel.
 * \retval true  All the users connected to the event channel are ready to handle incoming events
 * \retval false One or more users connected to the event channel are not ready to handle incoming events
 */
bool events_is_users_ready(struct events_resource *resource);

/**
 * \brief Check if an event is detected on the event channel.
 *
 * Check if an event has been detected on the channel.
 *
 * \note This function will clear the event detected interrupt flag.
 *
 * \param[in] resource Pointer to an \ref events_resource struct
 *
 * \return Status of the event detection interrupt flag.
 * \retval true  Event has been detected
 * \retval false Event has not been detected
 */
bool events_is_detected(struct events_resource *resource);

/**
 * \brief Check if there has been an overrun situation on this channel.
 *
 * \note This function will clear the event overrun detected interrupt flag.
 *
 * \param[in] resource Pointer to an \ref events_resource struct
 *
 * \return Status of the event overrun interrupt flag.
 * \retval true  Event overrun has been detected
 * \retval false Event overrun has not been detected
 */
bool events_is_overrun(struct events_resource *resource);

/**
 * \brief Release allocated channel back the the resource pool.
 *
 * Release an allocated channel back to the resource pool to make it available for other purposes.
 *
 * \param[in] resource Pointer to an \ref events_resource struct
 *
 * \return Status of the channel release procedure.
 * \retval STATUS_OK                  No error was detected when the channel was released
 * \retval STATUS_BUSY                One or more event users have not processed the last event
 * \retval STATUS_ERR_NOT_INITIALIZED Channel not allocated, and can therefore not be released
 */
enum status_code events_release(struct events_resource *resource);

/**
 * \brief Get the number of free channels.
 *
 * Get the number of allocatable channels in the events system resource pool.
 *
 * \return The number of free channels in the event system.
 *
 */
uint8_t events_get_free_channels(void);


///@cond INTERNAL
/**
 * \internal
 * Function to find bit position in the CHSTATUS and INTFLAG register,
 * and return bit mask of this position.
 *
 * @{
 */
uint32_t _events_find_bit_position(uint8_t channel, uint8_t start_offset);
/** @} */
///@endcond


/** @} */

/**
 * \page asfdoc_sam0_events_extra Extra Information for EVENTS Driver
 *
 * \section asfdoc_sam0_events_extra_acronyms Acronyms
 * Below is a table listing the acronyms used in this module, along with their
 * intended meanings.
 *
 * <table>
 *   <tr>
 *     <th>Acronym</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>CPU</td>
 *     <td>Central Processing Unit</td>
 *   </tr>
 *   <tr>
 *     <td>MUX</td>
 *     <td>Multiplexer</td>
 *   </tr>
 * </table>
 *
 *
 * \section asfdoc_sam0_events_extra_dependencies Dependencies
 * This driver has the following dependencies:
 *
 * - \ref asfdoc_sam0_system_clock_group "System Clock Driver"
 *
 *
 * \section asfdoc_sam0_events_extra_errata Errata
 * There are no errata related to this driver.
 *
 *
 * \section asfdoc_sam0_events_extra_history Module History
 * An overview of the module history is presented in the table below, with
 * details on the enhancements and fixes made to the module since its first
 * release. The current version of this corresponds to the newest version in
 * the table.
 *
 * <table>
 *   <tr>
 *     <th>Changelog</th>
 *   </tr>
 *   <tr>
 *     <td>Fix a bug in internal function _events_find_bit_position()</td>
 *   </tr>
 *   <tr>
 *     <td>Rewrite of events driver</td>
 *   </tr>
 *   <tr>
 *     <td>Initial Release</td>
 *   </tr>
 * </table>
 */

 /**
 * \page asfdoc_sam0_events_exqsg Examples for EVENTS Driver
 *
 * This is a list of the available Quick Start guides (QSGs) and example
 * applications for \ref asfdoc_sam0_events_group. QSGs are simple examples with
 * step-by-step instructions to configure and use this driver in a selection of
 * use cases. Note that a QSG can be compiled as a standalone application or be
 * added to the user application.
 *
 * - \subpage asfdoc_sam0_events_basic_use_case
 * \if EVENTS_INTERRUPT_HOOK_MODE
 * - \subpage asfdoc_sam0_events_interrupt_hook_use_case
 * \endif
 *
 * \page asfdoc_sam0_events_document_revision_history Document Revision History
 *
 * <table>
 *  <tr>
 *      <th>Doc. Rev.</td>
 *      <th>Date</td>
 *      <th>Comments</td>
 *  </tr>
 *  <tr>
 *      <td>42108G</td>
 *      <td>12/2015</td>
 *      <td>Added support for SAM D09 and SAM L22</td>
 *  </tr>
 *  <tr>
 *      <td>42108F</td>
 *      <td>08/2015</td>
 *      <td>Added support for SAM L21, SAM DA1, SAMR30 and SAM C20/C21</td>
 *  </tr>
 *  <tr>
 *      <td>42108E</td>
 *      <td>12/2014</td>
 *      <td>Added support for interrupt hook mode.
 *         Added support for SAM R21 and SAM D10/D11.</td>
 *  </tr>
 *  <tr>
 *      <td>42108D</td>
 *      <td>01/2014</td>
 *      <td>Update to support SAM D21 and corrected documentation typos</td>
 *  </tr>
 *  <tr>
 *      <td>42108C</td>
 *      <td>11/2013</td>
 *      <td>Fixed incorrect documentation for the event signal paths. Added
 *          configuration steps overview to the documentation.</td>
 *  </tr>
 *  <tr>
 *      <td>42108B</td>
 *      <td>06/2013</td>
 *      <td>Corrected documentation typos</td>
 *  </tr>
 *  <tr>
 *      <td>42108A</td>
 *      <td>06/2013</td>
 *      <td>Initial release</td>
 *  </tr>
 * </table>
 */

#ifdef __cplusplus
}
#endif

#endif /* EVENTS_H_INCLUDED */
