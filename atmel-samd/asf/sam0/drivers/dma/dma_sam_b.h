/**
 * \file
 *
 * \brief Direct Memory Access Controller Driver for SAMB
 *
 * Copyright (C) 2015-2016 Atmel Corporation. All rights reserved.
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
 * \defgroup asfdoc_samb_dma_group SAM Direct Memory Access Controller Driver (DMAC)
 *
 * This driver for Atmel&reg; | SMART SAM devices provides an interface for the configuration
 * and management of the Direct Memory Access Controller(DMAC) module within
 * the device. The DMAC can transfer data between memories and peripherals, and
 * thus off-load these tasks from the CPU. The module supports peripheral to
 * peripheral, peripheral to memory, memory to peripheral, and memory to memory
 * transfers.
 *
 * The following peripherals are used by the DMAC Driver:
 * - DMAC (Direct Memory Access Controller)
 *
 * The following devices can use this module:
 *  - Atmel | SMART SAM B11
 *
 * The outline of this documentation is as follows:
 * - \ref asfdoc_samb_dma_prerequisites
 * - \ref asfdoc_samb_dma_module_overview
 * - \ref asfdoc_samb_dma_special_considerations
 * - \ref asfdoc_samb_dma_extra_info
 * - \ref asfdoc_samb_dma_examples
 * - \ref asfdoc_samb_dma_api_overview
 *
 *
 * \section asfdoc_samb_dma_prerequisites Prerequisites
 *
 * There are no prerequisites for this module.
 *
 *
 * \section asfdoc_samb_dma_module_overview Module Overview
 *
 * SAM devices with DMAC enables high data transfer rates with minimum
 * CPU intervention and frees up CPU time. With access to all peripherals,
 * the DMAC can handle automatic transfer of data to/from modules.
 * It supports static and incremental addressing for both source and
 * destination.
 *
 * The DMAC when used with peripheral triggers, provides a
 * considerable advantage by reducing the power consumption and performing
 * data transfer in the background.
 * The CPU can remain in sleep during this time to reduce power consumption.
 *
 * <table>
 *    <tr>
 *      <th>Device</th>
 *      <th>Dma channel number</th>
 *    </tr>
 *    <tr>
 *      <td>SAMB11</td>
 *      <td>4</td>
 *    </tr>
 * </table>
 * The DMA channel operation can be suspended at any time by software, 
 * or after selectable descriptor execution. The DMAC driver for SAM 
 * supports four types of transfers such as peripheral to peripheral,
 * peripheral to memory, memory to peripheral, and memory to memory.
 *
 * The basic transfer unit is a beat which is defined as a single bus access.
 * There can be multiple beats in a single block transfer and multiple block
 * transfers in a DMA transaction.
 * DMA transfer is based on descriptors, which holds transfer properties
 * such as the source and destination addresses, transfer counter, and other
 * additional transfer control information.
 * The descriptors can be static or linked. When static, a single block transfer
 * is performed. When linked, a number of transfer descriptors can be used to
 * enable multiple block transfers within a single DMA transaction.
 *
 * The implementation of the DMA driver is based on the idea that DMA channel
 * is a finite resource of entities with the same abilities. A DMA channel resource
 * is able to move a defined set of data from a source address to destination
 * address triggered by a transfer trigger. On the SAM devices there are 12
 * DMA resources available for allocation. Each of these DMA resources can trigger
 * interrupt callback routines.
 * The other main features are:
 *
 * - Selectable transfer trigger source
 *  - Software
 *  - Peripheral
 * - Tree level channel priority
 *  - Normal level
 *  - High level
 *  - Top level
 * - Optional interrupt generation on transfer complete, channel error
 * - Supports multi-buffer or circular buffer mode by linking multiple descriptors
 * - Beat size configurable as 8-bit, 16-bit, or 32-bit
 *
 * A simplified block diagram of the DMA Resource can be seen in
 * \ref asfdoc_samb_dma_module_block_diagram "the figure below".
 *
 * \anchor asfdoc_samb_dma_module_block_diagram
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
 * \subsection asfdoc_samb_dma_module_overview_dma_channels DMA Channels
 * The DMAC in each device consists of several DMA channels, which
 * along with the transfer descriptors defines the data transfer properties.
 * - The transfer control descriptor defines the source and destination
 * addresses, source and destination address increment settings, the
 * block transfer count
 * - Dedicated channel registers control the peripheral trigger source,
 * trigger mode settings, and channel priority level settings
 *
 * With a successful DMA resource allocation, a dedicated
 * DMA channel will be assigned. The channel will be occupied until the
 * DMA resource is freed. A DMA resource handle is used to identify the specific
 * DMA resource.
 * When there are multiple channels with active requests, the arbiter prioritizes
 * the channels requesting access to the bus.
 *
 * \subsection asfdoc_samb_dma_module_overview_dma_trigger DMA Triggers
 * DMA transfer can be started only when a DMA transfer request is acknowledged/granted by the arbiter. A
 * transfer request can be triggered from software, peripheral. There
 * are dedicated source trigger selections for each DMA channel usage.
 *
 * \subsection asfdoc_samb_dma_module_overview_dma_transfer_descriptor DMA Transfer Descriptor
 * The transfer descriptor resides in the SRAM and
 * defines these channel properties.
 *
 * <table>
 *   <tr>
 *     <th>Field name</th>
 *     <th>Field width</th>
 *   </tr>
 *   <tr>
 *     <td>Source Address</td>
 *     <td>32 bits</td>
 *   </tr>
 *   <tr>
 *     <td>Destination Address</td>
 *     <td>32 bits</td>
 *   </tr>
 *   <tr>
 *     <td>Block Transfer Counter</td>
 *     <td>32 bits</td>
 *   </tr>
 *   <tr>
 *     <td>Descriptor Next Address</td>
 *     <td>30 bits</td>
 *   </tr>
 *   <tr>
 *     <td>Block Transfer Interrupt</td>
 *     <td>1 bit</td>
 *   </tr>
 *   <tr>
 *     <td>Block Transfer Stop Control</td>
 *     <td>1 bit</td>
 *   </tr>
 * </table>
 *
 * Before starting a transfer, at least one descriptor should be configured.
 * After a successful allocation of a DMA channel, the transfer descriptor can
 * be added with a call to \ref dma_add_descriptor(). If there is a transfer
 * descriptor already allocated to the DMA resource, the descriptor will
 * be linked to the next descriptor address.
 *
 * \subsection asfdoc_samb_dma_module_overview_dma_output DMA Interrupts
 * Both an interrupt callback and an peripheral can be triggered by the
 * DMA transfer. Three types of callbacks are supported by the DMA driver:
 * transfer complete, channel suspend, and transfer error. Each of these callback
 * types can be registered and enabled for each channel independently through
 * the DMA driver API.
 *
 *
 * \section asfdoc_samb_dma_special_considerations Special Considerations
 *
 * There are no special considerations for this module.
 *
 *
 * \section asfdoc_samb_dma_extra_info Extra Information
 *
 * For extra information, see \ref asfdoc_samb_dma_extra. This includes:
 * - \ref asfdoc_samb_dma_extra_acronyms
 * - \ref asfdoc_samb_dma_extra_dependencies
 * - \ref asfdoc_samb_dma_extra_errata
 * - \ref asfdoc_samb_dma_extra_history
 *
 *
 * \section asfdoc_samb_dma_examples Examples
 *
 * For a list of examples related to this driver, see
 * \ref asfdoc_samb_dma_exqsg.
 *
 *
 * \section asfdoc_samb_dma_api_overview API Overview
 * @{
 */

#include <compiler.h>
#include <system_sam_b.h>
#include "conf_dma.h"

/** DMA IRQn number. */
#define PROV_DMA_CTRL0_IRQn		15

/** DMA invalid channel number. */
#define DMA_INVALID_CHANNEL        0xff

/** DMA peripheral index */
enum dma_peripheral_index {
	MEMORY_DMA_PERIPHERAL = 0,
	UART0RX_DMA_PERIPHERAL,
	UART0TX_DMA_PERIPHERAL,
	UART1RX_DMA_PERIPHERAL,
	UART1TX_DMA_PERIPHERAL,
	SPI0RX_DMA_PERIPHERAL,
	SPI0TX_DMA_PERIPHERAL,
	SPI1RX_DMA_PERIPHERAL,
	SPI1TX_DMA_PERIPHERAL,
	I2C0RX_DMA_PERIPHERAL,
	I2C0TX_DMA_PERIPHERAL,
	I2C1RX_DMA_PERIPHERAL,
	I2C1TX_DMA_PERIPHERAL,
	DUALTIMER0_DMA_PERIPHERAL = 15,
	TIMER0_DMA_PERIPHERAL,
};

/** DMA channel index */
enum dma_ch_index {
	/** DMA channel 0 */
	DMA_CHANNEL_0 = 0,
	/** DMA channel 1 */
	DMA_CHANNEL_1,
	/** DMA channel 2 */
	DMA_CHANNEL_2,
	/** DMA channel 3 */
	DMA_CHANNEL_3,
};

enum dma_endian_swap {
	/** DMA endian no swap */
	DMA_ENDIAN_NO_SWAP,
	/** DMA endian 16-bit */
	DMA_ENDIAN_SIZE_16,
	/** DMA endian 32-bit */
	DMA_ENDIAN_SIZE_32,
	/** DMA endian 64-bit */
	DMA_ENDIAN_SIZE_64,
};

/**
 * Callback types for DMA callback driver.
 */
enum dma_callback_type {
	/** Callback for transfer complete */
	DMA_CALLBACK_TRANSFER_DONE,
	/** AHB read slave error */
	DMA_CALLBACK_READ_ERR,
	/**  AHB write slave error */
	DMA_CALLBACK_WRITE_ERR,
	/** FIFO has been overflown */
	DMA_CALLBACK_FIFO_OVERFLOW,
	/** FIFO has been underflows */
	DMA_CALLBACK_FIFO_UNDERFLOW,
	/** Read timeout on AHB bus (timeout value fixed at 1024 cycles) */
	DMA_CALLBACK_READ_TIMEOUT,
	/** Write timeout on AHB bus (timeout value fixed at 1024 cycles) */
	DMA_CALLBACK_WRITE_TIMEOUT,
	/** Channel active but did not start a burst for 2048 cycles */
	DMA_CALLBACK_WDT_TRIGGER,
	/** Number of available callbacks */
	DMA_CALLBACK_N,
};

/**
 * DMA transfer descriptor configuration. When the source or destination address
 * increment is enabled, the addresses stored into the configuration structure
 * must correspond to the end of the transfer.
 */
struct dma_descriptor {
	/** Start address of read buffer */
	uint32_t read_start_addr;
	/** Start address of write buffer */
	uint32_t write_start_addr;
	/** Size (in bytes) of buffer to transfer */
	uint32_t buffer_size;
	union {
		struct {
			/** Active high interrupt enable once buffer has been transferred */
			uint32_t set_interrupt:1;
			/** If set, channel stops when buffer done, otherwise load from cmd_next_addr */
			uint32_t last:1;
			/** Address of next command if cmd_last is not set */
			uint32_t next_addr:30;
		} cmd;
		uint32_t next;
	};
};
/** Structure for DMA source/description */
struct dma_config {
	/** Maximum number of bytes of an AHB read/write burst */
	uint8_t max_burst;
	/** Number of AHB read/write commands to issue before channel is released */
	uint8_t tokens;
	/** If true, the controller will increment the next burst address */
	bool enable_inc_addr;
	/** Index of peripheral to read/write from (0 if memory or no peripheral flow control) */
	enum dma_peripheral_index periph;
	/** 
	 * Number of cycles to wait for read/write request signal to update 
	 * after issuing the read/write clear signal
	 */
	uint8_t periph_delay;
	/** Top priority enable */
	bool enable_proi_top;
	/** Top priority channel index */
	uint8_t proi_top_index;
	/** High priority enable */
	bool enable_proi_high;
	/** High priority channel index */
	uint8_t proi_high_index;
};

/** Structure for DMA transfer resource */
struct dma_resource_config {
	struct dma_config src;
	struct dma_config des;
	/** If true, channel will work in joint mode */
	bool enable_joint_mode;
	/** Endian Byte Swapping */
	enum dma_endian_swap swap;
};

/** Forward definition of the DMA resource */
struct dma_resource;
/** Type definition for a DMA resource callback function */
typedef void (*dma_callback_t)(struct dma_resource *const resource);

/** Structure for DMA transfer resource */
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
	struct dma_descriptor* descriptor;
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
	return resource->job_status;
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
	resource->callback_enable |= 1 << type;
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
	resource->callback_enable &= ~(1 << type);
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
	resource->callback[type] = NULL;
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
 *  \li Set the read start address as 0
 *  \li Set the write start address as 0
 *  \li Set buffer size as 1
 *  \li Set beat size as byte
 *  \li Enable the interrupt
 *  \li Enable the channel stops when buffer done
 *  \li Set next command address to 0
 * \param[out] config Pointer to the configuration
 *
 */
static inline void dma_descriptor_get_config_defaults(struct dma_descriptor *config)
{
	/* Default read buffer size is set to 0 */
	config->read_start_addr = 0;
	/* Default write buffer size is set to 0 */
	config->write_start_addr = 0;
	/* Set beat size to one byte */
	config->buffer_size = 1;
	/* Enable transferred interrupt */
	config->cmd.set_interrupt = 1;
	/* Channel stops when buffer done */
	config->cmd.last = 1;
	/* Set next command to 0 */
	config->cmd.next_addr = 0;
}

/**
 * \brief Update DMA descriptor.
 *
 * This function can update the descriptor of an allocated DMA resource.
 *
 */
static inline void dma_update_descriptor(struct dma_resource *resource,
		struct dma_descriptor* descriptor)
{
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
	resource->descriptor = NULL;
}

void dma_get_config_defaults(struct dma_resource_config *config);
enum status_code dma_allocate(struct dma_resource *resource,
		struct dma_resource_config *config);
enum status_code dma_add_descriptor(struct dma_resource *resource,
		struct dma_descriptor* descriptor);
enum status_code dma_start_transfer_job(struct dma_resource *resource);
enum status_code dma_free(struct dma_resource *resource);
uint8_t dma_get_status(uint8_t channel);
uint8_t dma_get_interrupt_status(uint8_t channel);
void dma_clear_interrupt_status(uint8_t channel, uint8_t flag);
/** @} */

/**
 * \page asfdoc_samb_dma_extra Extra Information for DMAC Driver
 *
 * \section asfdoc_samb_dma_extra_acronyms Acronyms
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
 * \section asfdoc_samb_dma_extra_dependencies Dependencies
 * There are no dependencies related to this driver.
 *
 *
 * \section asfdoc_samb_dma_extra_errata Errata
 * There are no errata related to this driver.
 *
 *
 * \section asfdoc_samb_dma_extra_history Module History
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
 *     <td>Initial Release</td>
 *   </tr>
 * </table>
 */

 /**
 * \page asfdoc_samb_dma_exqsg Examples for DMAC Driver
 *
 * This is a list of the available Quick Start Guides (QSGs) and example
 * applications for \ref asfdoc_samb_dma_group. QSGs are simple examples with
 * step-by-step instructions to configure and use this driver in a selection of
 * use cases. Note that QSGs can be compiled as a standalone application or be
 * added to the user application.
 *
 * - \subpage asfdoc_samb_dma_basic_use_case
 *
 * \note More DMA usage examples are available in peripheral QSGs.
 *
 * \page asfdoc_samb_dma_document_revision_history Document Revision History
 *
 * <table>
 *    <tr>
 *        <th>Doc. Rev.</td>
 *        <th>Date</td>
 *        <th>Comments</td>
 *    </tr>
 *    <tr>
 *        <td>A</td>
 *        <td>09/2015</td>
 *        <td>Initial release</td>
 *    </tr>
 * </table>
 */

#ifdef __cplusplus
}
#endif

#endif /* DMA_H_INCLUDED */
