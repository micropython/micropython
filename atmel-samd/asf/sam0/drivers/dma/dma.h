/**
 * \file
 *
 * \brief SAM Direct Memory Access Controller Driver
 *
 * Copyright (C) 2014-2016 Atmel Corporation. All rights reserved.
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
#ifndef DMA_H_INCLUDED
#define DMA_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup asfdoc_sam0_dma_group SAM Direct Memory Access Controller (DMAC) Driver
 *
 * This driver for Atmel&reg; | SMART ARM®-based microcontrollers provides an interface for the configuration
 * and management of the Direct Memory Access Controller(DMAC) module within
 * the device. The DMAC can transfer data between memories and peripherals, and
 * thus off-load these tasks from the CPU. The module supports peripheral to
 * peripheral, peripheral to memory, memory to peripheral, and memory to memory
 * transfers.
 *
 * The following peripheral is used by the DMAC Driver:
 * - DMAC (Direct Memory Access Controller)
 *
 * The following devices can use this module:
 *  - Atmel | SMART SAM D21
 *  - Atmel | SMART SAM R21
 *  - Atmel | SMART SAM D09/D10/D11
 *  - Atmel | SMART SAM L21/L22
 *  - Atmel | SMART SAM DA1
 *  - Atmel | SMART SAM C20/C21
 *  - Atmel | SMART SAM R30
 *
 * The outline of this documentation is as follows:
 * - \ref asfdoc_sam0_dma_prerequisites
 * - \ref asfdoc_sam0_dma_module_overview
 * - \ref asfdoc_sam0_dma_special_considerations
 * - \ref asfdoc_sam0_dma_extra_info
 * - \ref asfdoc_sam0_dma_examples
 * - \ref asfdoc_sam0_dma_api_overview
 *
 *
 * \section asfdoc_sam0_dma_prerequisites Prerequisites
 *
 * There are no prerequisites for this module.
 *
 *
 * \section asfdoc_sam0_dma_module_overview Module Overview
 *
 * SAM devices with DMAC enables high data transfer rates with minimum
 * CPU intervention and frees up CPU time. With access to all peripherals,
 * the DMAC can handle automatic transfer of data to/from modules.
 * It supports static and incremental addressing for both source and
 * destination.
 *
 * The DMAC when used with Event System or peripheral triggers, provides a
 * considerable advantage by reducing the power consumption and performing
 * data transfer in the background.
 * For example, if the ADC is configured to generate an event, it can trigger
 * the DMAC to transfer the data into another peripheral or SRAM.
 * The CPU can remain in sleep during this time to reduce the power consumption.
 *
 * <table>
 *    <tr>
 *      <th>Device</th>
 *      <th>Dma channel number</th>
 *    </tr>
 *    <tr>
 *      <td>SAM D21/R21/C20/C21</td>
 *      <td>12</td>
 *    </tr>
 *    <tr>
 *      <td>SAM D09/D10/D11</td>
 *      <td>6</td>
 *    </tr>
 *    <tr>
 *      <td>SAM L21,SAMR30</td>
 *      <td>16</td>
 *    </tr>
 * </table>
 * The DMA channel operation can be suspended at any time by software, by events
 * from event system, or after selectable descriptor execution. The operation
 * can be resumed by software or by events from the event system.
 * The DMAC driver for SAM supports four types of transfers such as
 * peripheral to peripheral, peripheral to memory, memory to peripheral, and
 * memory to memory.
 *
 * The basic transfer unit is a beat, which is defined as a single bus access.
 * There can be multiple beats in a single block transfer and multiple block
 * transfers in a DMA transaction.
 * DMA transfer is based on descriptors, which holds transfer properties
 * such as the source and destination addresses, transfer counter, and other
 * additional transfer control information.
 * The descriptors can be static or linked. When static, a single block transfer
 * is performed. When linked, a number of transfer descriptors can be used to
 * enable multiple block transfers within a single DMA transaction.
 *
 * The implementation of the DMA driver is based on the idea that the DMA channel
 * is a finite resource of entities with the same abilities. A DMA channel resource
 * is able to move a defined set of data from a source address to destination
 * address triggered by a transfer trigger. On the SAM devices there are 12
 * DMA resources available for allocation. Each of these DMA resources can trigger
 * interrupt callback routines and peripheral events.
 * The other main features are:
 *
 * - Selectable transfer trigger source
 *  - Software
 *  - Event System
 *  - Peripheral
 * - Event input and output is supported for the four lower channels
 * - Four level channel priority
 * - Optional interrupt generation on transfer complete, channel error, or channel suspend
 * - Supports multi-buffer or circular buffer mode by linking multiple descriptors
 * - Beat size configurable as 8-bit, 16-bit, or 32-bit
 *
 * A simplified block diagram of the DMA Resource can be seen in
 * \ref asfdoc_sam0_dma_module_block_diagram "the figure below".
 *
 * \anchor asfdoc_sam0_dma_module_block_diagram
 * \dot
 * digraph overview {
 * splines = false;
 * rankdir=LR;
 *
 * mux1 [label="Transfer Trigger", shape=box];
 *
 * dma [label="DMA Channel", shape=polygon, sides=6, orientation=60, style=filled, fillcolor=darkolivegreen1, height=1, width=1];
 * descriptor [label="Transfer Descriptor", shape=box, style=filled, fillcolor=lightblue];
 *
 * mux1 -> dma;
 * descriptor -> dma;
 *
 * interrupt [label="Interrupt", shape=box];
 * events [label="Events", shape=box];
 *
 * dma:e -> interrupt:w;
 * dma:e -> events:w;
 *
 * {rank=same; descriptor dma}
 *
 * }
 * \enddot
 *
 * \subsection asfdoc_sam0_dma_features Driver Feature Macro Definition
 * <table>
 *  <tr>
 *    <th>Driver Feature Macro</th>
 *    <th>Supported devices</th>
 *  </tr>
 *  <tr>
 *    <td>FEATURE_DMA_CHANNEL_STANDBY</td>
 *    <td>SAM L21/L22/C20/C21/R30</td>
 *  </tr>
 * </table>
 * \note The specific features are only available in the driver when the
 * selected device supports those features.
 *
 * \subsection asfdoc_sam0_dma_module_overview_dma_transf_term Terminology Used in DMAC Transfers
 *
 *   <table border="0" cellborder="1" cellspacing="0" >
 *    <tr>
 *        <th> Name </th> <th> Description </th>
 *    </tr>
 *    <tr>
 *     <td > Beat </td>
 *     <td > It is a single bus access by the DMAC.
 *           Configurable as 8-bit, 16-bit, or 32-bit.
 *     </td>
 *    </tr>
 *    <tr>
 *     <td > Burst </td>
 *     <td> It is a transfer of n-beats (n=1,4,8,16).
 *          For the DMAC module in SAM, the burst size is one beat.
 *          Arbitration takes place each time a burst transfer is completed.
 *     </td>
 *    </tr>
 *    <tr>
 *     <td > Block transfer </td>
 *     <td>  A single block transfer is a configurable number of (1 to 64k)
 *           beat transfers
 *     </td>
 *    </tr>
 *   </table>
 *
 * \subsection asfdoc_sam0_dma_module_overview_dma_channels DMA Channels
 * The DMAC in each device consists of several DMA channels, which
 * along with the transfer descriptors defines the data transfer properties.
 * - The transfer control descriptor defines the source and destination
 * addresses, source and destination address increment settings, the
 * block transfer count, and event output condition selection
 * - Dedicated channel registers control the peripheral trigger source,
 * trigger mode settings, event input actions, and channel priority level
 * settings
 *
 * With a successful DMA resource allocation, a dedicated
 * DMA channel will be assigned. The channel will be occupied until the
 * DMA resource is freed. A DMA resource handle is used to identify the specific
 * DMA resource.
 * When there are multiple channels with active requests, the arbiter prioritizes
 * the channels requesting access to the bus.
 *
 * \subsection asfdoc_sam0_dma_module_overview_dma_trigger DMA Triggers
 * DMA transfer can be started only when a DMA transfer request is acknowledged/granted by the arbiter. A
 * transfer request can be triggered from software, peripheral, or an event. There
 * are dedicated source trigger selections for each DMA channel usage.

 *
 * \subsection asfdoc_sam0_dma_module_overview_dma_transfer_descriptor DMA Transfer Descriptor
 * The transfer descriptor resides in the SRAM and
 * defines these channel properties.
 *   <table border="0" cellborder="1" cellspacing="0" >
 *    <tr>
 *        <th> Field name </th> <th> Field width </th>
 *    </tr>
 *    <tr>
 *     <td > Descriptor Next Address </td> <td > 32 bits </td>
 *    </tr>
 *    <tr>
 *     <td > Destination Address </td> <td> 32 bits </td>
 *    </tr>
 *    <tr>
 *     <td > Source Address </td> <td> 32 bits </td>
 *    </tr>
 *    <tr>
 *     <td > Block Transfer Counter </td> <td> 16 bits </td>
 *    </tr>
 *    <tr>
 *     <td > Block Transfer Control </td> <td> 16 bits </td>
 *    </tr>
 *   </table>
 *
 * Before starting a transfer, at least one descriptor should be configured.
 * After a successful allocation of a DMA channel, the transfer descriptor can
 * be added with a call to \ref dma_add_descriptor(). If there is a transfer
 * descriptor already allocated to the DMA resource, the descriptor will
 * be linked to the next descriptor address.
 *
 * \subsection asfdoc_sam0_dma_module_overview_dma_output DMA Interrupts/Events
 * Both an interrupt callback and an peripheral event can be triggered by the
 * DMA transfer. Three types of callbacks are supported by the DMA driver:
 * transfer complete, channel suspend, and transfer error. Each of these callback
 * types can be registered and enabled for each channel independently through
 * the DMA driver API.
 *
 * The DMAC module can also generate events on transfer complete. Event
 * generation is enabled through the DMA channel, event channel configuration,
 * and event user multiplexing is done through the events driver.
 *
 * The DMAC can generate events in the below cases:
 *
 * - When a block transfer is complete
 *
 * - When each beat transfer within a block transfer is complete
 *
 * \section asfdoc_sam0_dma_special_considerations Special Considerations
 *
 * There are no special considerations for this module.
 *
 *
 * \section asfdoc_sam0_dma_extra_info Extra Information
 *
 * For extra information, see \ref asfdoc_sam0_dma_extra. This includes:
 * - \ref asfdoc_sam0_dma_extra_acronyms
 * - \ref asfdoc_sam0_dma_extra_dependencies
 * - \ref asfdoc_sam0_dma_extra_errata
 * - \ref asfdoc_sam0_dma_extra_history
 *
 *
 * \section asfdoc_sam0_dma_examples Examples
 *
 * For a list of examples related to this driver, see
 * \ref asfdoc_sam0_dma_exqsg.
 *
 *
 * \section asfdoc_sam0_dma_api_overview API Overview
 * @{
 */

#include <compiler.h>
#include "conf_dma.h"

#if (SAML21) || (SAML22) || (SAMC20) || (SAMC21) || (SAMR30) || defined(__DOXYGEN__)
#define FEATURE_DMA_CHANNEL_STANDBY
#endif

/** DMA invalid channel number. */
#define DMA_INVALID_CHANNEL        0xff

/** ExInitial description section. */
extern DmacDescriptor descriptor_section[CONF_MAX_USED_CHANNEL_NUM];

/* DMA channel interrup flag. */
extern uint8_t g_chan_interrupt_flag[CONF_MAX_USED_CHANNEL_NUM];

/** DMA priority level. */
enum dma_priority_level {
	/** Priority level 0. */
	DMA_PRIORITY_LEVEL_0,
	/** Priority level 1. */
	DMA_PRIORITY_LEVEL_1,
	/** Priority level 2. */
	DMA_PRIORITY_LEVEL_2,
	/** Priority level 3. */
	DMA_PRIORITY_LEVEL_3,
};

/** DMA input actions. */
enum dma_event_input_action {
	/** No action. */
	DMA_EVENT_INPUT_NOACT,
	/** Normal transfer and periodic transfer trigger. */
	DMA_EVENT_INPUT_TRIG,
	/** Conditional transfer trigger. */
	DMA_EVENT_INPUT_CTRIG,
	/** Conditional block transfer. */
	DMA_EVENT_INPUT_CBLOCK,
	/** Channel suspend operation. */
	DMA_EVENT_INPUT_SUSPEND,
	/** Channel resume operation. */
	DMA_EVENT_INPUT_RESUME,
	/** Skip next block suspend action. */
	DMA_EVENT_INPUT_SSKIP,
};

/**
 * Address increment step size. These bits select the address increment step
 * size. The setting apply to source or destination address, depending on
 * STEPSEL setting.
 */
enum dma_address_increment_stepsize {
	/** The address is incremented by (beat size * 1). */
	DMA_ADDRESS_INCREMENT_STEP_SIZE_1 = 0,
	/** The address is incremented by (beat size * 2). */
	DMA_ADDRESS_INCREMENT_STEP_SIZE_2,
	/** The address is incremented by (beat size * 4). */
	DMA_ADDRESS_INCREMENT_STEP_SIZE_4,
	/** The address is incremented by (beat size * 8). */
	DMA_ADDRESS_INCREMENT_STEP_SIZE_8,
	/** The address is incremented by (beat size * 16). */
	DMA_ADDRESS_INCREMENT_STEP_SIZE_16,
	/** The address is incremented by (beat size * 32). */
	DMA_ADDRESS_INCREMENT_STEP_SIZE_32,
	/** The address is incremented by (beat size * 64). */
	DMA_ADDRESS_INCREMENT_STEP_SIZE_64,
	/** The address is incremented by (beat size * 128). */
	DMA_ADDRESS_INCREMENT_STEP_SIZE_128,
};

/**
 * DMA step selection. This bit determines whether the step size setting
 * is applied to source or destination address.
 */
enum dma_step_selection {
	/** Step size settings apply to the destination address. */
	DMA_STEPSEL_DST = 0,
	/** Step size settings apply to the source address. */
	DMA_STEPSEL_SRC,
};

/** The basic transfer unit in DMAC is a beat, which is defined as a
 *  single bus access. Its size is configurable and applies to both read
 *  and write. */
enum dma_beat_size {
	/** 8-bit access. */
	DMA_BEAT_SIZE_BYTE = 0,
	/** 16-bit access. */
	DMA_BEAT_SIZE_HWORD,
	/** 32-bit access. */
	DMA_BEAT_SIZE_WORD,
};

/**
 * Block action definitions.
 */
enum dma_block_action {
	/** No action. */
	DMA_BLOCK_ACTION_NOACT = 0,
	/** Channel in normal operation and sets transfer complete interrupt flag
	 *  after block transfer. */
	DMA_BLOCK_ACTION_INT,
	/** Trigger channel suspend after block transfer and sets channel
	 *  suspend interrupt flag once the channel is suspended. */
	DMA_BLOCK_ACTION_SUSPEND,
	/** Sets transfer complete interrupt flag after a block transfer and
	 *  trigger channel suspend. The channel suspend interrupt flag will be set
	 *  once the channel is suspended. */
	DMA_BLOCK_ACTION_BOTH,
};

/** Event output selection. */
enum dma_event_output_selection {
	/** Event generation disable. */
	DMA_EVENT_OUTPUT_DISABLE = 0,
	/** Event strobe when block transfer complete. */
	DMA_EVENT_OUTPUT_BLOCK,
	/** Event output reserved. */
	DMA_EVENT_OUTPUT_RESERVED,
	/** Event strobe when beat transfer complete. */
	DMA_EVENT_OUTPUT_BEAT,
};

/** DMA trigger action type. */
enum dma_transfer_trigger_action{
	/** Perform a block transfer when triggered. */
	DMA_TRIGGER_ACTION_BLOCK = DMAC_CHCTRLB_TRIGACT_BLOCK_Val,
	/** Perform a beat transfer when triggered. */
	DMA_TRIGGER_ACTION_BEAT = DMAC_CHCTRLB_TRIGACT_BEAT_Val,
	/** Perform a transaction when triggered. */
	DMA_TRIGGER_ACTION_TRANSACTION = DMAC_CHCTRLB_TRIGACT_TRANSACTION_Val,
};

/**
 * Callback types for DMA callback driver.
 */
enum dma_callback_type {
	/** Callback for any of transfer errors. A transfer error is flagged
     *	if a bus error is detected during an AHB access or when the DMAC
	 *  fetches an invalid descriptor. */
	DMA_CALLBACK_TRANSFER_ERROR,
	/** Callback for transfer complete. */
	DMA_CALLBACK_TRANSFER_DONE,
	/** Callback for channel suspend. */
	DMA_CALLBACK_CHANNEL_SUSPEND,
	/** Number of available callbacks. */
	DMA_CALLBACK_N,
};

/**
 * DMA transfer descriptor configuration. When the source or destination address
 * increment is enabled, the addresses stored into the configuration structure
 * must correspond to the end of the transfer.
 *
 */
struct dma_descriptor_config {
	/** Descriptor valid flag used to identify whether a descriptor is
	    valid or not */
	bool descriptor_valid;
	/** This is used to generate an event on specific transfer action in
	    a channel. Supported only in four lower channels. */
	enum dma_event_output_selection event_output_selection;
	/** Action taken when a block transfer is completed */
	enum dma_block_action block_action;
	/** Beat size is configurable as 8-bit, 16-bit, or 32-bit */
	enum dma_beat_size beat_size;
	/** Used for enabling the source address increment */
	bool src_increment_enable;
	/** Used for enabling the destination address increment */
	bool dst_increment_enable;
	/** This bit selects whether the source or destination address is
	    using the step size settings */
	enum dma_step_selection step_selection;
	/** The step size for source/destination address increment.
	    The next address is calculated
	    as next_addr = addr + (2^step_size * beat size). */
	enum dma_address_increment_stepsize step_size;
	/** It is the number of beats in a block. This count value is
	 * decremented by one after each beat data transfer. */
	uint16_t block_transfer_count;
	/** Transfer source address */
	uint32_t source_address;
	/** Transfer destination address */
	uint32_t destination_address;
	/** Set to zero for static descriptors. This must have a valid memory
	    address for linked descriptors. */
	uint32_t next_descriptor_address;
};

/** Configurations for DMA events. */
struct dma_events_config {
	/** Event input actions */
	enum dma_event_input_action input_action;
	/** Enable DMA event output */
	bool event_output_enable;
};

/** DMA configurations for transfer. */
struct dma_resource_config {
	/** DMA transfer priority */
	enum dma_priority_level priority;
	/**DMA peripheral trigger index */
	uint8_t peripheral_trigger;
	/** DMA trigger action */
	enum dma_transfer_trigger_action trigger_action;
#ifdef FEATURE_DMA_CHANNEL_STANDBY
	/** Keep DMA channel enabled in standby sleep mode if true */
	bool run_in_standby;
#endif
	/** DMA events configurations */
	struct dma_events_config event_config;
};

/** Forward definition of the DMA resource. */
struct dma_resource;
/** Type definition for a DMA resource callback function. */
typedef void (*dma_callback_t)(struct dma_resource *const resource);

/** Structure for DMA transfer resource. */
struct dma_resource {
	/** Allocated DMA channel ID */
	uint8_t channel_id;
	/** Array of callback functions for DMA transfer job */
	dma_callback_t callback[DMA_CALLBACK_N];
	/** Bit mask for enabled callbacks */
	uint8_t callback_enable;
	/** Status of the last job */
	volatile enum status_code job_status;
	/** Transferred data size */
	uint32_t transfered_size;
	/** DMA transfer descriptor */
	DmacDescriptor* descriptor;
};

/**
 * \brief Get DMA resource status.
 *
 * \param[in] resource Pointer to the DMA resource
 *
 * \return Status of the DMA resource.
 */
static inline enum status_code dma_get_job_status(struct dma_resource *resource)
{
	Assert(resource);

	return resource->job_status;
}

/**
 * \brief Check if the given DMA resource is busy.
 *
 * \param[in] resource Pointer to the DMA resource
 *
 * \return Status which indicates whether the DMA resource is busy.
 *
 * \retval true The DMA resource has an on-going transfer
 * \retval false The DMA resource is not busy
 */
static inline bool dma_is_busy(struct dma_resource *resource)
{
	Assert(resource);

	return (resource->job_status == STATUS_BUSY);
}

/**
 * \brief Enable a callback function for a dedicated DMA resource.
 *
 * \param[in] resource Pointer to the DMA resource
 * \param[in] type Callback function type
 *
 */
static inline void dma_enable_callback(struct dma_resource *resource,
		enum dma_callback_type type)
{
	Assert(resource);

	resource->callback_enable |= 1 << type;
	g_chan_interrupt_flag[resource->channel_id] |= (1UL << type);
}

/**
 * \brief Disable a callback function for a dedicated DMA resource.
 *
 * \param[in] resource Pointer to the DMA resource
 * \param[in] type Callback function type
 *
 */
static inline void dma_disable_callback(struct dma_resource *resource,
		enum dma_callback_type type)
{
	Assert(resource);

	resource->callback_enable &= ~(1 << type);
	g_chan_interrupt_flag[resource->channel_id] &= (~(1UL << type) & DMAC_CHINTENSET_MASK);
	DMAC->CHINTENCLR.reg = (1UL << type);
}

/**
 * \brief Register a callback function for a dedicated DMA resource.
 *
 * There are three types of callback functions, which can be registered:
 * - Callback for transfer complete
 * - Callback for transfer error
 * - Callback for channel suspend
 *
 * \param[in] resource Pointer to the DMA resource
 * \param[in] callback Pointer to the callback function
 * \param[in] type Callback function type
 *
 */
static inline void dma_register_callback(struct dma_resource *resource,
		dma_callback_t callback, enum dma_callback_type type)
{
	Assert(resource);

	resource->callback[type] = callback;
}

/**
 * \brief Unregister a callback function for a dedicated DMA resource.
 *
 * There are three types of callback functions:
 * - Callback for transfer complete
 * - Callback for transfer error
 * - Callback for channel suspend
 *
 * The application can unregister any of the callback functions which
 * are already registered and are no longer needed.
 *
 * \param[in] resource Pointer to the DMA resource
 * \param[in] type Callback function type
 *
 */
static inline void dma_unregister_callback(struct dma_resource *resource,
		enum dma_callback_type type)
{
	Assert(resource);

	resource->callback[type] = NULL;
}

/**
 * \brief Will set a software trigger for resource.
 *
 * This function is used to set a software trigger on the DMA channel
 * associated with resource. If a trigger is already pending no new trigger
 * will be generated for the channel.
 *
 * \param[in] resource Pointer to the DMA resource
 */
static inline void dma_trigger_transfer(struct dma_resource *resource) {
	Assert(resource);

	DMAC->SWTRIGCTRL.reg |= (1 << resource->channel_id);
}

/**
 * \brief Initializes DMA transfer configuration with predefined default values.
 *
 * This function will initialize a given DMA descriptor configuration structure to
 * a set of known default values. This function should be called on
 * any new instance of the configuration structure before being
 * modified by the user application.
 *
 * The default configuration is as follows:
 *  \li Set the descriptor as valid
 *  \li Disable event output
 *  \li No block action
 *  \li Set beat size as byte
 *  \li Enable source increment
 *  \li Enable destination increment
 *  \li Step size is applied to the destination address
 *  \li Address increment is beat size multiplied by 1
 *  \li Default transfer size is set to 0
 *  \li Default source address is set to NULL
 *  \li Default destination address is set to NULL
 *  \li Default next descriptor not available
 * \param[out] config Pointer to the configuration
 *
 */
static inline void dma_descriptor_get_config_defaults(struct dma_descriptor_config *config)
{
	Assert(config);

	/* Set descriptor as valid */
	config->descriptor_valid = true;
	/* Disable event output */
	config->event_output_selection = DMA_EVENT_OUTPUT_DISABLE;
	/* No block action */
	config->block_action = DMA_BLOCK_ACTION_NOACT;
	/* Set beat size to one byte */
	config->beat_size = DMA_BEAT_SIZE_BYTE;
	/* Enable source increment */
	config->src_increment_enable = true;
	/* Enable destination increment */
	config->dst_increment_enable = true;
	/* Step size is applied to the destination address */
	config->step_selection = DMA_STEPSEL_DST;
	/* Address increment is beat size multiplied by 1*/
	config->step_size = DMA_ADDRESS_INCREMENT_STEP_SIZE_1;
	/* Default transfer size is set to 0 */
	config->block_transfer_count = 0;
	/* Default source address is set to NULL */
	config->source_address = (uint32_t)NULL;
	/* Default destination address is set to NULL */
	config->destination_address = (uint32_t)NULL;
	/** Next descriptor address set to 0 */
	config->next_descriptor_address = 0;
}

/**
 * \brief Update DMA descriptor.
 *
 * This function can update the descriptor of an allocated DMA resource.
 *
 */
static inline void dma_update_descriptor(struct dma_resource *resource,
		DmacDescriptor* descriptor)
{
	Assert(resource);

	resource->descriptor = descriptor;
}

/**
 * \brief Reset DMA descriptor.
 *
 * This function will clear the DESCADDR register of an allocated DMA resource.
 *
 */
static inline void dma_reset_descriptor(struct dma_resource *resource)
{
	Assert(resource);

	resource->descriptor = NULL;
}

void dma_get_config_defaults(struct dma_resource_config *config);
enum status_code dma_allocate(struct dma_resource *resource,
		struct dma_resource_config *config);
enum status_code dma_free(struct dma_resource *resource);
enum status_code dma_start_transfer_job(struct dma_resource *resource);
void dma_abort_job(struct dma_resource *resource);
void dma_suspend_job(struct dma_resource *resource);
void dma_resume_job(struct dma_resource *resource);
void dma_descriptor_create(DmacDescriptor* descriptor,
	struct dma_descriptor_config *config);
enum status_code dma_add_descriptor(struct dma_resource *resource,
		DmacDescriptor* descriptor);

/** @} */

/**
 * \page asfdoc_sam0_dma_extra Extra Information for DMAC Driver
 *
 * \section asfdoc_sam0_dma_extra_acronyms Acronyms
 * Below is a table listing the acronyms used in this module, along with their
 * intended meanings.
 *
 * <table>
 *   <tr>
 *     <th>Acronym</th>
 *     <th>Description</th>
 *   </tr>
 *   <tr>
 *     <td>DMA</td>
 *     <td>Direct Memory Access</td>
 *   </tr>
 *   <tr>
 *     <td>DMAC</td>
 *     <td>Direct Memory Access Controller </td>
 *   </tr>
 *   <tr>
 *     <td>CPU</td>
 *     <td>Central Processing Unit</td>
 *   </tr>
 * </table>
 *
 *
 * \section asfdoc_sam0_dma_extra_dependencies Dependencies
 * This driver has the following dependencies:
 *
 * - \ref asfdoc_sam0_system_clock_group "System Clock Driver"
 *
 *
 * \section asfdoc_sam0_dma_extra_errata Errata
 * There are no errata related to this driver.
 *
 *
 * \section asfdoc_sam0_dma_extra_history Module History
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
 *     <td>Add SAM C21 support</td>
 *   </tr>
 *   <tr>
 *     <td>Add SAM L21 support</td>
 *   </tr>
 *   <tr>
 *     <td>Add SAM R30 support</td>
 *   </tr>
 *   <tr>
 *     <td>Initial Release</td>
 *   </tr>
 * </table>
 */

 /**
 * \page asfdoc_sam0_dma_exqsg Examples for DMAC Driver
 *
 * This is a list of the available Quick Start Guides (QSGs) and example
 * applications for \ref asfdoc_sam0_dma_group. QSGs are simple examples with
 * step-by-step instructions to configure and use this driver in a selection of
 * use cases. Note that a QSG can be compiled as a standalone application or be
 * added to the user application.
 *
 * - \subpage asfdoc_sam0_dma_basic_use_case
 *
 * \note More DMA usage examples are available in peripheral QSGs.
 * A quick start guide for TC/TCC
 * shows the usage of DMA event trigger; SERCOM SPI/USART/I<SUP>2</SUP>C has example for
 * DMA transfer from peripheral to memory or from memory to peripheral;
 * ADC/DAC shows peripheral to peripheral transfer.
 *
 * \page asfdoc_sam0_dma_document_revision_history Document Revision History
 *
 * <table>
 *    <tr>
 *        <th>Doc. Rev.</th>
 *        <th>Date</th>
 *        <th>Comments</th>
 *    </tr>
 *    <tr>
 *        <td>42257C</td>
 *        <td>12/2015</td>
 *        <td>Added suppport for SAM L21/L22, SAM C21, SAM D09, SAMR30 and SAM DA1</td>
 *    </tr>
 *    <tr>
 *        <td>42257B</td>
 *        <td>12/2014</td>
 *        <td>Added support for SAM R21 and SAM D10/D11</td>
 *    </tr>
 *    <tr>
 *        <td>42257A</td>
 *        <td>02/2014</td>
 *        <td>Initial release</td>
 *    </tr>
 * </table>
 */

#ifdef __cplusplus
}
#endif

#endif /* DMA_H_INCLUDED */
