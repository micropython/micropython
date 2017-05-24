/**
 * \file
 *
 * \brief SAM I2S - Inter-IC Sound Controller
 *
 * Copyright (c) 2014-2016 Atmel Corporation. All rights reserved.
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

#ifndef I2S_H_INCLUDED
#define I2S_H_INCLUDED

/**
 * \defgroup asfdoc_sam0_i2s_group SAM Inter-IC Sound Controller (I2S) Driver
 *
 * This driver for Atmel&reg; | SMART ARM&reg;-based microcontrollers provides
 * an interface for the configuration and management of the device's Inter-IC
 * Sound Controller functionality.
 *
 * The following driver API modes are covered by this manual:
 * - Polled APIs
 * \if I2S_CALLBACK_MODE
 * - Callback APIs
 * \endif
 *
 * The following peripheral is used by this module:
 *  - I<SUP>2</SUP>S (Inter-IC Sound Controller)
 *
 * The following devices can use this module:
 *  - Atmel | SMART SAM D21
 *  - Atmel | SMART SAM DA1
 *
 * The outline of this documentation is as follows:
 *  - \ref asfdoc_sam0_i2s_prerequisites
 *  - \ref asfdoc_sam0_i2s_module_overview
 *  - \ref asfdoc_sam0_i2s_special_considerations
 *  - \ref asfdoc_sam0_i2s_extra_info
 *  - \ref asfdoc_sam0_i2s_examples
 *  - \ref asfdoc_sam0_i2s_api_overview
 *
 * \section asfdoc_sam0_i2s_prerequisites Prerequisites
 *
 * There are no prerequisites for this module.
 *
 * \section asfdoc_sam0_i2s_module_overview Module Overview
 *
 * The I<SUP>2</SUP>S provides bidirectional, synchronous, digital audio link with
 * external audio devices through these signal pins:
 *  - Serial Data (SDm)
 *  - Frame Sync (FSn)
 *  - Serial Clock (SCKn)
 *  - Master Clock (MCKn)
 *
 * The I<SUP>2</SUP>S consists of two Clock Units and two Serializers, which can be
 * separately configured and enabled, to provide varies functionalities as follow:
 *  - Communicate to Audio CODECs as Master or Slave, or provides clock and
 *    frame sync signals as Controller
 *  - Communicate to DAC or ADC through dedicated I<SUP>2</SUP>S serial interface
 *  - Communicate to multi-slot or multiple stereo DACs or ADCs, via
 *    Time Division Multiplexed (TDM) format
 *  - Reading mono or stereo MEMS microphones, using the Pulse Density
 *    Modulation (PDM) interface
 *
 * The I<SUP>2</SUP>S supports compact stereo data word, where left channel data bits are
 * in lower half and right channel data bits are in upper half. It reduces the
 * number of data words for stereo audio data and the DMA bandwidth.
 *
 * In master mode, the frame is configured by number of slots and slot size, and
 * allows range covering 16fs to 1024fs MCK, to provide oversampling clock to an
 * external audio CODEC or digital signal processor (DSP).
 *
 * A block diagram of the I<SUP>2</SUP>S can be seen in
 * \ref asfdoc_sam0_i2s_module_block_diagram "the figure below".
 *
 * \anchor asfdoc_sam0_i2s_module_block_diagram
 * \image html i2s_blocks.svg "I2S Block Diagram"
 *
 * This driver for I<SUP>2</SUP>S module provides an interface to:
 *  - Initialize and control I<SUP>2</SUP>S module
 *  - Configure and control the I<SUP>2</SUP>S Clock Unit and Serializer
 *  - Transmit/receive data through I<SUP>2</SUP>S Serializer
 *
 * \subsection asfdoc_sam0_i2s_module_overview_clocks Clocks
 *
 * To use I<SUP>2</SUP>S module, the I<SUP>2</SUP>S bus interface clock (clk_i2s)
 * must be enabled via Power Manager.
 *
 * For each I<SUP>2</SUP>S Clock Unit, a generic clock (gclk_i2s_n) is connnected.
 * When I<SUP>2</SUP>S works in master mode the generic clock is used. It should
 * be prepared before clock unit is used. In master mode the input generic clock
 * will be used as MCK for SCKn and FSn generation, in addition, the MCK could be
 * devided and output to I<SUP>2</SUP>S MCKn pin, as oversampling clock to
 * external audio device.
 *
 * The I<SUP>2</SUP>S Serializer uses clock and control signal from Clock Unit to handle
 * transfer. Select different clock unit with different configurations allows
 * the I<SUP>2</SUP>S to work as master or slave, to work on non-related clocks.
 *
 * When using the driver with ASF, enabling the register interface is normally
 * done by the \c init function.
 * The Generic Clock Controller (GCLK) source for the asynchronous domain is
 * normally configured and set through the _configuration
 * struct_ / _init_ function.
 * If GCLK source != 0 is used, this source has to be configured and enabled
 * through invoking the system_gclk driver function when needed, or modifying
 * conf_clock.h to enable it at the beginning.
 *
 * \subsection asfdoc_sam0_i2s_module_overview_frame Audio Frame Generation
 *
 * Audio sample data for all channels are sent in frames, one frame can consist
 * 1 - 8 slots where each slot can be configured to a size 8-bit, 16-bit, 24-bit,
 * or 32-bit. The audio frame synch clock is generated by the I<SUP>2</SUP>S
 * Clock unit in the master/controller mode. The frame rate (or frame sync
 * frequency) is calculated as follows:
 *
 * FS = SCK / number_of_slots / number_of_bits_in_slot
 *
 * The serial clock (SCK) source is either an external source (slave mode) or
 * generated by the I<SUP>2</SUP>S clock unit (controller or master mode) using
 * the MCK as source.
 *
 * SCK = MCK / sck_div
 * \note SCK generation division value is MCKDIV in register.
 *
 * MCK is either an external source or generated using the GCLK input from a
 * generic clock generator.
 *
 * \subsection asfdoc_sam0_i2s_module_overview_mode Master, Controller, and Slave Modes
 *
 * The I<SUP>2</SUP>S module has three modes: master, controller, and slave.
 *
 * \subsubsection asfdoc_sam0_i2s_module_overview_mode_mst Master
 * In master mode the module will control the data flow on the I<SUP>2</SUP>S bus and can
 * be responsible for clock generation. The Serializers are enabled and will
 * transmit/receive data. On a bus with only master and slave the SCK, and FS
 * clock signal will be outputted on the SCK and FS pin on the master module.
 * MCK can optionally be outputted on the MCK pin, if there is a controller
 * module on the bus the SCK, FS, and optionally the MCK clock is sourced from
 * the same pins. Serial data will be trancieved on the SD pin in both
 * scenarios.
 *
 * \subsubsection asfdoc_sam0_i2s_module_overview_mode_ctl Controller
 * In controller mode the module will generate the clock signals, but the
 * Serializers are disabled and no data will be transmitted/received by the
 * module in this mode. The clock signals is outputted on the SCK, FS and
 * optionally the MCK pin.
 *
 * \subsubsection asfdoc_sam0_i2s_module_overview_mode_slv Slave
 * In slave mode the module will use the SCK and FS clock source from the master
 * or the controller which is received on the SCK and FS pin. The MCK can
 * optionally be sourced externally on the MCK pin. The Serializers are enabled
 * and will tranceive data on the SD pin. All data flow is controlled by the
 * master.
 *
 * \subsubsection asfdoc_sam0_i2s_module_overview_mode_chg Switch Modes
 * The mode switching between master, controller, and slave modes are actually
 * done by modifying the source mode of I<SUP>2</SUP>S pins.
 * The source mode of I<SUP>2</SUP>S pins are selected by writing corresponding
 * bits in CLKCTRLn.
 * Since source mode switching changes the direction of pin, the mode must be
 * changed when the I<SUP>2</SUP>S Clock Unit is stopped.
 *
 * \subsection asfdoc_sam0_i2s_module_overview_data Data Stream Reception/Transmission
 *
 * The I<SUP>2</SUP>S module support several data stream formats:
 * - I<SUP>2</SUP>S format
 * - Time Division Multiplexed (TDM) format
 * - Pulse Density Modulation (PDM) format (reception only)
 *
 * Basically the I<SUP>2</SUP>S module can send several words within each frame,
 * it's more like TDM format. With adjust to the number of data words in a frame,
 * the FS width, the FS to data bits delay, etc., the module is able to handle
 * I<SUP>2</SUP>S compliant data stream.
 *
 * Also the Serializer can receive PDM format data stream, which allows the 
 * I<SUP>2</SUP>S module receive 1 PDM data on each SCK edge.
 *
 * \subsubsection asfdoc_sam0_i2s_module_overview_data_i2s I2S Stream Reception/Transmission
 *
 * For 2-channel I<SUP>2</SUP>S compliant data stream format the I<SUP>2</SUP>S 
 * module uses the FS line as word select (WS) signal and will send left channel
 * data word on low WS level and right channel data word on high WS level as
 * specified in the I<SUP>2</SUP>S standard. The supported word sizes are 8-,
 * 16-, 18-, 20-, 24-, and 32- bit.
 *
 * Thus for I<SUP>2</SUP>S stream, the following settings should be applied to the module:
 *  - Data starting delay after FS transition : one SCK period
 *  - FS width : half of frame
 *  - Data bits adjust in word : left-adjusted
 *  - Bit transmitting order : MSB first
 *
 * Following is an example for I<SUP>2</SUP>S application connections and waveforms. See
 * the figure below.
 *
 * \anchor asfdoc_sam0_i2s_module_i2s_example_diagram
 * \image html i2s_example.svg "I2S Example Diagram"
 *
 * \subsubsection asfdoc_sam0_i2s_module_overview_data_tdm TDM Stream Reception/Transmission
 * In TDM format, the module sends several data words in each frame. For this
 * data stream format most of the configurations could be adjusted:
 *  - Main Frame related settings are as follow:
 *   - Frame Sync (FS) options:
 *    - The active edge of the FS (or if FS is inverted before use)
 *    - The width of the FS
 *   - The delay between FS to first data bit
 *  - Data alignment in slot
 *  - The number of slots and slot size can be adjusted, it has been mentioned
 *    in \ref asfdoc_sam0_i2s_module_overview_frame
 *  - The data word size is controlled by Serializer, it can be chosen among
 *    8, 16, 18, 20, 24, and 32 bits.
 *
 * The general TDM waveform generation is as follows:
 *
 * \anchor asfdoc_sam0_i2s_module_tdm_wave_diagram
 * \image html tdm_wave.svg "TDM Waveform Generation"
 *
 * Some other settings could also be found to set up clock, data formatting and
 * pin multiplexer (MUX).
 * Refer to \ref i2s_clock_unit_config "Clock Unit Configurations"
 * and \ref i2s_serializer_config "Serializer Configurations" for more
 * details.
 *
 * Following is examples for different application use cases.
 *
 * See \ref asfdoc_sam0_i2s_module_tdm_timeslot_example_diagram "here" for
 * the Time Slot Application connection and waveform example.
 *
 * \anchor asfdoc_sam0_i2s_module_tdm_timeslot_example_diagram
 * \image html tdm_timeslot_example.svg "Codec Example Diagram"
 *
 * See \ref asfdoc_sam0_i2s_module_tdm_codec_example_diagram "here" for the
 * Codec Application connection and waveform example.
 *
 * \anchor asfdoc_sam0_i2s_module_tdm_codec_example_diagram
 * \image html tdm_codec_example.svg "Time Slot Example Diagram"
 *
 * \subsubsection asfdoc_sam0_i2s_module_overview_data_pdm PDM Reception
 * The I<SUP>2</SUP>S Serializer integrates PDM reception feature, to use this feature,
 * simply select PDM2 mode in Serializer configuration. In PDM2 mode, it assumes
 * two microphones are input for stereo stream. The left microphone bits will
 * be stored in lower half and right microphone bits in upper half of the data
 * word, like in compact stereo format.
 *
 * See \ref asfdoc_sam0_i2s_module_pdm_example_diagram "following figure" for an
 * example of PDM Microphones Application with both left and right channel
 * microphone connected.
 *
 * \anchor asfdoc_sam0_i2s_module_pdm_example_diagram
 * \image html pdm_example.svg "Time PDM2 Example Diagram"
 *
 * \subsubsection asfdoc_sam0_i2s_module_overview_data_fmt MONO and Compact Data
 * The I<SUP>2</SUP>S Serializer can accept some pre-defined data format and generates
 * the data stream in specified way.
 *
 * When transmitting data, the Serializer can work in MONO mode: assum input
 * is single channel mono data on left channel and copy it to right channel
 * automatically.
 *
 * Also the I<SUP>2</SUP>S Serializer can support compact stereo data word. The data word
 * size of the Serializer can be set to \ref I2S_DATA_SIZE_16BIT_COMPACT
 * "16-bit compact" or \ref I2S_DATA_SIZE_8BIT_COMPACT "8-bit compact", with
 * these option I<SUP>2</SUP>S Serializer will compact left channel data and right channel
 * data together, the left channel data will take lower bytes and right channel
 * data take higher bytes.
 *
 * \subsection asfdoc_sam0_i2s_module_overview_loop Loop-back Mode
 * The I<SUP>2</SUP>S can be configured to loop back the Transmitter to Receiver. In this
 * mode Serializer's input will be connected to another Serializer's output
 * internally.
 *
 * \subsection asfdoc_sam0_i2s_module_overview_sleep Sleep Modes
 * The I<SUP>2</SUP>S will continue to operate in any sleep mode, where the selected source
 * clocks are running.
 *
 * \section asfdoc_sam0_i2s_special_considerations Special Considerations
 *
 * There is no special considerations for I<SUP>2</SUP>S module.
 *
 * \section asfdoc_sam0_i2s_extra_info Extra Information
 *
 * For extra information see \ref asfdoc_sam0_i2s_extra. This includes:
 *  - \ref asfdoc_sam0_i2s_extra_acronyms
 *  - \ref asfdoc_sam0_i2s_extra_dependencies
 *  - \ref asfdoc_sam0_i2s_extra_errata
 *  - \ref asfdoc_sam0_i2s_extra_history
 *
 * \section asfdoc_sam0_i2s_examples Examples
 *
 * For a list of examples related to this driver, see
 * \ref asfdoc_sam0_i2s_exqsg.
 *
 *
 * \section asfdoc_sam0_i2s_api_overview API Overview
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <compiler.h>
#include <system.h>

#if I2S_CALLBACK_MODE == true
#  include <system_interrupt.h>

#if !defined(__DOXYGEN__)
extern struct i2s_module *_i2s_instances[I2S_INST_NUM];
#endif

/** Forward definition of the device instance */
struct i2s_module;

/** Type of the callback functions. */
typedef void (*i2s_serializer_callback_t)
		(struct i2s_module *const module);

/**
 * \brief I<SUP>2</SUP>S Serializer Callback enum
 */
enum i2s_serializer_callback {
	/** Callback for buffer read/write finished */
	I2S_SERIALIZER_CALLBACK_BUFFER_DONE,
	/** Callback for Serializer overrun/underrun */
	I2S_SERIALIZER_CALLBACK_OVER_UNDER_RUN,
#  if !defined(__DOXYGEN__)
	I2S_SERIALIZER_CALLBACK_N
#  endif
};

#endif /* #if I2S_CALLBACK_MODE == true */

/**
 * \name Module Status Flags
 *
 * I<SUP>2</SUP>S status flags, returned by \ref i2s_get_status() and cleared by
 * \ref i2s_clear_status().
 *
 * @{
 */

/** Module Serializer x (0~1) Transmit Underrun. */
#define I2S_STATUS_TRANSMIT_UNDERRUN(x)     (1u << ((x)+0))
/** Module Serializer x (0~1) is ready to accept new data to be transmitted. */
#define I2S_STATUS_TRANSMIT_READY(x)        (1u << ((x)+2))
/** Module Serializer x (0~1) Receive Overrun. */
#define I2S_STATUS_RECEIVE_OVERRUN(x)       (1u << ((x)+4))
/** Module Serializer x (0~1) has received a new data. */
#define I2S_STATUS_RECEIVE_READY(x)         (1u << ((x)+6))
/** Module is busy on synchronization. */
#define I2S_STATUS_SYNC_BUSY                (1u << 8)

/** @} */

/**
 * Master Clock (MCK) source selection.
 */
enum i2s_master_clock_source {
	/** Master Clock (MCK) is from general clock */
	I2S_MASTER_CLOCK_SOURCE_GCLK,
	/** Master Clock (MCK) is from MCK input pin */
	I2S_MASTER_CLOCK_SOURCE_MCKPIN
};

/**
 * Serial Clock (SCK) source selection.
 */
enum i2s_serial_clock_source {
	/** Serial Clock (SCK) is divided from Master Clock */
	I2S_SERIAL_CLOCK_SOURCE_MCKDIV,
	/** Serial Clock (SCK) is input from SCK input pin */
	I2S_SERIAL_CLOCK_SOURCE_SCKPIN
};

/**
 * Data delay from Frame Sync (FS).
 */
enum i2s_data_delay {
	/** Left Justified (no delay) */
	I2S_DATA_DELAY_0,
	/** I<SUP>2</SUP>S data delay (1-bit delay) */
	I2S_DATA_DELAY_1,
	/** Left Justified (no delay) */
	I2S_DATA_DELAY_LEFT_JUSTIFIED = I2S_DATA_DELAY_0,
	/** I<SUP>2</SUP>S data delay (1-bit delay) */
	I2S_DATA_DELAY_I2S = I2S_DATA_DELAY_1
};

/**
 * Frame Sync (FS) source.
 */
enum i2s_frame_sync_source {
	/** Frame Sync (FS) is divided from I<SUP>2</SUP>S Serial Clock */
	I2S_FRAME_SYNC_SOURCE_SCKDIV,
	/** Frame Sync (FS) is input from FS input pin */
	I2S_FRAME_SYNC_SOURCE_FSPIN
};

/**
 * Frame Sync (FS) output pulse width.
 */
enum i2s_frame_sync_width {
	/** Frame Sync (FS) Pulse is one slot width */
	I2S_FRAME_SYNC_WIDTH_SLOT,
	/** Frame Sync (FS) Pulse is half a frame width */
	I2S_FRAME_SYNC_WIDTH_HALF_FRAME,
	/** Frame Sync (FS) Pulse is one bit width */
	I2S_FRAME_SYNC_WIDTH_BIT,
	/** 1-bit wide Frame Sync (FS) per Data sample, only used when Data transfer
	 *  is requested */
	I2S_FRAME_SYNC_WIDTH_BURST
};

/**
 * Time Slot Size in number of I<SUP>2</SUP>S serial clocks (bits).
 */
enum i2s_slot_size {
	/** 8-bit slot */
	I2S_SLOT_SIZE_8_BIT,
	/** 16-bit slot */
	I2S_SLOT_SIZE_16_BIT,
	/** 24-bit slot */
	I2S_SLOT_SIZE_24_BIT,
	/** 32-bit slot */
	I2S_SLOT_SIZE_32_BIT
};

/**
 * DMA channels usage for I<SUP>2</SUP>S.
 */
enum i2s_dma_usage {
	/** Single DMA channel for all I<SUP>2</SUP>S channels */
	I2S_DMA_USE_SINGLE_CHANNEL_FOR_ALL,
	/** One DMA channel per data channel */
	I2S_DMA_USE_ONE_CHANNEL_PER_DATA_CHANNEL
};

/**
 * I<SUP>2</SUP>S data format, to extend mono data to two channels.
 */
enum i2s_data_format {
	/** Normal mode, keep data to its right channel */
	I2S_DATA_FORMAT_STEREO,
	/** Assume input is mono data for left channel, the data is duplicated to
	 *  right channel */
	I2S_DATA_FORMAT_MONO
};

/**
 * I<SUP>2</SUP>S data bit order.
 */
enum i2s_bit_order {
	/** Transfer Data Most Significant Bit first 
	 * (Default for I<SUP>2</SUP>S protocol) 
	 */
	I2S_BIT_ORDER_MSB_FIRST,
	/** Transfer Data Least Significant Bit first */
	I2S_BIT_ORDER_LSB_FIRST
};

/**
 * I<SUP>2</SUP>S data bit padding.
 */
enum i2s_bit_padding {
	/** Padding with 0 */
	I2S_BIT_PADDING_0,
	/** Padding with 1 */
	I2S_BIT_PADDING_1,
	/** Padding with MSBit */
	I2S_BIT_PADDING_MSB,
	/** Padding with LSBit */
	I2S_BIT_PADDING_LSB,
};

/**
 * I<SUP>2</SUP>S data word adjust.
 */
enum i2s_data_adjust {
	/** Data is right adjusted in word */
	I2S_DATA_ADJUST_RIGHT,
	/** Data is left adjusted in word */
	I2S_DATA_ADJUST_LEFT
};

/**
 * I<SUP>2</SUP>S data word size.
 */
enum i2s_data_size {
	/** 32-bit */
	I2S_DATA_SIZE_32BIT,
	/** 24-bit */
	I2S_DATA_SIZE_24BIT,
	/** 20-bit */
	I2S_DATA_SIZE_20BIT,
	/** 18-bit */
	I2S_DATA_SIZE_18BIT,
	/** 16-bit */
	I2S_DATA_SIZE_16BIT,
	/** 16-bit compact stereo */
	I2S_DATA_SIZE_16BIT_COMPACT,
	/** 8-bit */
	I2S_DATA_SIZE_8BIT,
	/** 8-bit compact stereo */
	I2S_DATA_SIZE_8BIT_COMPACT
};

/**
 * I<SUP>2</SUP>S data slot adjust.
 */
enum i2s_slot_adjust {
	/** Data is right adjusted in slot */
	I2S_SLOT_ADJUST_RIGHT,
	/** Data is left adjusted in slot */
	I2S_SLOT_ADJUST_LEFT
};

/**
 * I<SUP>2</SUP>S data padding.
 */
enum i2s_data_padding {
	/** Padding 0 in case of under-run */
	I2S_DATA_PADDING_0,
	/** Padding last data in case of under-run */
	I2S_DATA_PADDING_SAME_AS_LAST,
	/** Padding last data in case of under-run
	 *  (abbr. \c I2S_DATA_PADDING_SAME_AS_LAST) */
	I2S_DATA_PADDING_LAST = I2S_DATA_PADDING_SAME_AS_LAST,
	/** Padding last data in case of under-run
	 *  (abbr. \c I2S_DATA_PADDING_SAME_AS_LAST) */
	I2S_DATA_PADDING_SAME = I2S_DATA_PADDING_SAME_AS_LAST
};

/**
 * I<SUP>2</SUP>S line default value when slot disabled.
 */
enum i2s_line_default_state {
	/** Output default value is 0 */
	I2S_LINE_DEFAULT_0,
	/** Output default value is 1 */
	I2S_LINE_DEFAULT_1,
	/** Output default value is high impedance */
	I2S_LINE_DEFAULT_HIGH_IMPEDANCE = 3,
	/** Output default value is high impedance
	 *  (abbr. \c I2S_LINE_DEFAULT_HIGH_IMPEDANCE) */
	I2S_LINE_DEFAULT_HIZ = I2S_LINE_DEFAULT_HIGH_IMPEDANCE
};

/**
 * I<SUP>2</SUP>S Serializer mode.
 */
enum i2s_serializer_mode {
	/** Serializer is used to receive data */
	I2S_SERIALIZER_RECEIVE,
	/** Serializer is used to transmit data */
	I2S_SERIALIZER_TRANSMIT,
	/** Serializer is used to receive PDM data on each clock edge */
	I2S_SERIALIZER_PDM2
};

/**
 * I<SUP>2</SUP>S clock unit selection.
 */
enum i2s_clock_unit {
	/** Clock Unit channel 0 */
	I2S_CLOCK_UNIT_0,
	/** Clock Unit channel 1 */
	I2S_CLOCK_UNIT_1,
	/** Number of Clock Unit channels */
	I2S_CLOCK_UNIT_N
};

/**
 * I<SUP>2</SUP>S Serializer selection.
 */
enum i2s_serializer {
	/** Serializer channel 0 */
	I2S_SERIALIZER_0,
	/** Serializer channel 1 */
	I2S_SERIALIZER_1,
	/** Number of Serializer channels */
	I2S_SERIALIZER_N
};


/**
 * Configure for I<SUP>2</SUP>S pin.
 */
struct i2s_pin_config {
	/** GPIO index to access the pin */
	uint8_t gpio;
	/** Pin function MUX */
	uint8_t mux;
	/** Enable this pin for I<SUP>2</SUP>S module */
	bool enable;
};

/**
 * Configure for I<SUP>2</SUP>S clock (SCK).
 */
struct i2s_clock_config {
	/** Divide generic clock to master clock output (1~32, 0,1 means no div) */
	uint8_t mck_out_div;
	/** Divide generic clock to serial clock (1~32, 0,1 means no div) */
	uint8_t sck_div;
	/** Clock source selection */
	enum gclk_generator gclk_src;
	/** Master clock source selection: generated or input from pin */
	enum i2s_master_clock_source mck_src;
	/** Serial clock source selection: generated or input from pin */
	enum i2s_serial_clock_source sck_src;
	/** Invert master clock output */
	bool mck_out_invert;
	/** Invert serial clock output */
	bool sck_out_invert;
	/** Generate MCK clock output */
	bool mck_out_enable;
};

/**
 * Configure for I<SUP>2</SUP>S frame sync (FS).
 */
struct i2s_frame_sync_config {
	/** Frame Sync (FS) generated or input from pin */
	enum i2s_frame_sync_source source;
	/** Frame Sync (FS) width */
	enum i2s_frame_sync_width width;
	/** Invert Frame Sync (FS) signal before use */
	bool invert_use;
	/** Invert Frame Sync (FS) signal before output */
	bool invert_out;
};

/**
 * Configure for I<SUP>2</SUP>S frame.
 */
struct i2s_frame_config {
	/** Number of slots in a frame (1~8, 0,1 means minimum 1) */
	uint8_t number_slots;
	/** Size of each slot in frame */
	enum i2s_slot_size slot_size;
	/** Data delay from Frame Sync (FS) to first data bit */
	enum i2s_data_delay data_delay;
	/** Frame sync (FS) */
	struct i2s_frame_sync_config frame_sync;
};

/**
 * Configure for I<SUP>2</SUP>S clock unit.
 */
struct i2s_clock_unit_config {
	/** Configure clock generation */
	struct i2s_clock_config clock;
	/** Configure frame generation */
	struct i2s_frame_config frame;

	/** Configure master clock pin */
	struct i2s_pin_config mck_pin;
	/** Configure serial clock pin */
	struct i2s_pin_config sck_pin;
	/** Configure frame sync pin */
	struct i2s_pin_config fs_pin;
};

/**
 * Configure for I<SUP>2</SUP>S Serializer.
 */
struct i2s_serializer_config {
	/** Configure Serializer data pin */
	struct i2s_pin_config data_pin;

	/** Set to \c true to loop-back output to input pin for test */
	bool loop_back;

	/** Set to \c true to assumes mono input and duplicate it (left channel) to
	 *  right channel */
	bool mono_mode;

	/** Disable data slot */
	bool disable_data_slot[8];

	/** Set to \c true to transfer LSB first, \c false to transfer MSB first */
	bool transfer_lsb_first;
	/** Data Word Formatting Adjust,
	 *  set to \c true to adjust bits in word to left */
	bool data_adjust_left_in_word;
	/** Data Slot Formatting Adjust,
	 *  set to \c true to adjust words in slot to left */
	bool data_adjust_left_in_slot;

	/** Data Word Size */
	enum i2s_data_size data_size;
	/** Data Formatting Bit Extension */
	enum i2s_bit_padding bit_padding;
	/** Data padding when under-run */
	enum i2s_data_padding data_padding;

	/** DMA usage */
	enum i2s_dma_usage dma_usage;

	/** Clock unit selection */
	enum i2s_clock_unit clock_unit;

	/** Line default state where slot is disabled */
	enum i2s_line_default_state line_default_state;

	/** Serializer Mode */
	enum i2s_serializer_mode mode;
};

/**
 * \brief I<SUP>2</SUP>S Serializer instance struct.
 */
struct i2s_serializer_module {

#if I2S_CALLBACK_MODE == true
	/** Callbacks list for Serializer */
	i2s_serializer_callback_t callback[I2S_SERIALIZER_CALLBACK_N];

	/** Job buffer */
	void *job_buffer;
	/** Requested data words to read/write */
	uint32_t requested_words;
	/** Transferred data words for read/write */
	uint32_t transferred_words;

	/** Callback mask for registered callbacks */
	uint8_t registered_callback_mask;
	/** Callback mask for enabled callbacks */
	uint8_t enabled_callback_mask;

	/** Status of the ongoing or last transfer job */
	enum status_code job_status;
#endif

	/** Serializer mode */
	enum i2s_serializer_mode mode;
	/** Serializer data word size */
	enum i2s_data_size data_size;
};

/**
 * \brief I<SUP>2</SUP>S Software Module instance struct.
 */
struct i2s_module {
	/** Module HW register access base */
	I2s *hw;

	/** Module Serializer used */
	struct i2s_serializer_module serializer[2];
};

/**
 * \brief Determines if the hardware module(s) are currently synchronizing to the bus.
 *
 * Checks to see if the underlying hardware peripheral module(s) are currently
 * synchronizing across multiple clock domains to the hardware bus, This
 * function can be used to delay further operations on a module until such time
 * that it is ready, to prevent blocking delays for synchronization in the
 * user application.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 *
 * \return Synchronization status of the underlying hardware module(s).
 *
 * \retval false If the module has completed synchronization
 * \retval true If the module synchronization is ongoing
 */
static inline bool i2s_is_syncing(
		const struct i2s_module *const module_inst)
{
	/* Sanity check arguments */
	Assert(module_inst);
	Assert(module_inst->hw);

	return (module_inst->hw->SYNCBUSY.reg > 0);
}

/**
 * \name Driver Initialization
 * @{
 */

enum status_code i2s_init(
		struct i2s_module *const module_inst,
		I2s *hw);

/** @} */

/**
 * \name Enable/Disable/Reset
 * @{
 */

/**
 * \brief Enable the I<SUP>2</SUP>S module.
 *
 * Enables a I<SUP>2</SUP>S module that has been previously initialized.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 */
static inline void i2s_enable(const struct i2s_module *const module_inst)
{
	Assert(module_inst);
	Assert(module_inst->hw);

	while (module_inst->hw->SYNCBUSY.reg & I2S_SYNCBUSY_ENABLE) {
		/* Sync wait */
	}
	module_inst->hw->CTRLA.reg |= I2S_SYNCBUSY_ENABLE;
}

/**
 * \brief Disables the I<SUP>2</SUP>S module.
 *
 * Disables a I<SUP>2</SUP>S module.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 */
static inline void i2s_disable(const struct i2s_module *const module_inst)
{
	Assert(module_inst);
	Assert(module_inst->hw);

	while (module_inst->hw->SYNCBUSY.reg & I2S_SYNCBUSY_ENABLE) {
		/* Sync wait */
	}

	module_inst->hw->INTENCLR.reg = I2S_INTENCLR_MASK;
	module_inst->hw->INTFLAG.reg = I2S_INTFLAG_MASK;
	module_inst->hw->CTRLA.reg &= ~I2S_SYNCBUSY_ENABLE;
}

/**
 * \brief Resets the I<SUP>2</SUP>S module.
 *
 * Resets the I<SUP>2</SUP>S module, restoring all hardware module registers to their
 * default values and disabling the module. The I<SUP>2</SUP>S module will not be
 * accessible while the reset is being performed.
 *
 * \param[in]  module_inst    Pointer to the software module instance struct
 */
static inline void i2s_reset(const struct i2s_module *const module_inst)
{
	Assert(module_inst);
	Assert(module_inst->hw);

	/* Disable the module if it is running */
	if (module_inst->hw->CTRLA.reg & I2S_CTRLA_ENABLE) {
		i2s_disable(module_inst);
		while (i2s_is_syncing(module_inst)) {
			/* Sync wait */
		}
	}
	/* Reset the HW module */
	module_inst->hw->CTRLA.reg = I2S_CTRLA_SWRST;
}

/** @} */

/**
 * \name Clock Unit Initialization and Configuration
 * @{
 */

/**
 * \brief Initializes config with predefined default values for I<SUP>2</SUP>S clock unit.
 *
 * This function will initialize a given I<SUP>2</SUP>S Clock Unit configuration structure
 * to a set of known default values. This function should be called on any new
 * instance of the configuration structures before being modified by the user
 * application.
 *
 * The default configuration is as follows:
 * - The clock unit does not generate output clocks (MCK, SCK, and FS)
 * - The pins (MCK, SCK, and FS) and MUX configurations are not set
 *
 * \param[out]  config  Pointer to a I<SUP>2</SUP>S module clock unit configuration struct
 *                      to set
 */
static inline void i2s_clock_unit_get_config_defaults(
		struct i2s_clock_unit_config *const config)
{
	Assert(config);

	config->clock.mck_out_enable = false;
	config->clock.gclk_src = GCLK_GENERATOR_0;

	config->clock.mck_src = I2S_MASTER_CLOCK_SOURCE_GCLK;
	config->clock.mck_out_div = 1;
	config->clock.mck_out_invert = false;

	config->clock.sck_src = I2S_SERIAL_CLOCK_SOURCE_MCKDIV;
	config->clock.sck_div = 1;
	config->clock.sck_out_invert = false;

	config->frame.number_slots = 1;
	config->frame.slot_size = I2S_SLOT_SIZE_32_BIT;
	config->frame.data_delay = I2S_DATA_DELAY_I2S;

	config->frame.frame_sync.source = I2S_FRAME_SYNC_SOURCE_SCKDIV;
	config->frame.frame_sync.width = I2S_FRAME_SYNC_WIDTH_HALF_FRAME;
	config->frame.frame_sync.invert_use = false;
	config->frame.frame_sync.invert_out = false;

	config->mck_pin.enable = false;
	config->mck_pin.mux = 0;
	config->mck_pin.gpio = 0;

	config->sck_pin.enable = false;
	config->sck_pin.mux = 0;
	config->sck_pin.gpio = 0;

	config->fs_pin.enable = false;
	config->fs_pin.mux = 0;
	config->fs_pin.gpio = 0;
}

enum status_code i2s_clock_unit_set_config(
		struct i2s_module *const module_inst,
		const enum i2s_clock_unit clock_unit,
		const struct i2s_clock_unit_config *config);

/** @} */


/**
 * \name Clock Unit Enable/Disable
 * @{
 */

/**
 * \brief Enable the Specified Clock Unit of I<SUP>2</SUP>S module.
 *
 * Enables a Clock Unit in I<SUP>2</SUP>S module that has been previously initialized.
 *
 * \param[in] module_inst   Pointer to the software module instance struct
 * \param[in] clock_unit    I<SUP>2</SUP>S Clock Unit to enable
 */
static inline void i2s_clock_unit_enable(
		const struct i2s_module *const module_inst,
		const enum i2s_clock_unit clock_unit)
{
	uint32_t cken_bit;

	Assert(module_inst);
	Assert(module_inst->hw);

	cken_bit = I2S_CTRLA_CKEN0 << clock_unit;

	while (module_inst->hw->SYNCBUSY.reg & cken_bit) {
		/* Sync wait */
	}
	module_inst->hw->CTRLA.reg |= cken_bit;
}

/**
 * \brief Disable the Specified Clock Unit of I<SUP>2</SUP>S module.
 *
 * Disables a Clock Unit in I<SUP>2</SUP>S module that has been previously initialized.
 *
 * \param[in]  module_inst  Pointer to the software module instance struct
 * \param[in] clock_unit    I<SUP>2</SUP>S Clock Unit to disable
 */
static inline void i2s_clock_unit_disable(
		const struct i2s_module *const module_inst,
		const enum i2s_clock_unit clock_unit)
{
	uint32_t cken_bit;

	Assert(module_inst);
	Assert(module_inst->hw);

	cken_bit = I2S_CTRLA_CKEN0 << clock_unit;

	while (module_inst->hw->SYNCBUSY.reg & cken_bit) {
		/* Sync wait */
	}
	module_inst->hw->CTRLA.reg &= ~cken_bit;
}

/** @} */


/**
 * \name Serializer Initialization and Configuration
 * @{
 */

/**
 * \brief Initializes config with predefined default values for I<SUP>2</SUP>S Serializer.
 *
 * This function will initialize a given I<SUP>2</SUP>S Clock Unit configuration structure
 * to a set of known default values. This function should be called on any new
 * instance of the configuration structures before being modified by the user
 * application.
 *
 * The default configuration is as follows:
 * - Output data does not internally loopback to input line
 * - Does not extend mono data (left channel) to right channel
 * - None of the data slot is disabled
 * - MSB of I<SUP>2</SUP>S data is transferred first
 * - In data word data is adjusted right
 * - In slot data word is adjusted left
 * - The data size is 16-bit width
 * - I<SUP>2</SUP>S will padd 0 to not defined bits
 * - I<SUP>2</SUP>S will padd 0 to not defined words
 * - I<SUP>2</SUP>S will use single DMA channel for all data channels
 * - I<SUP>2</SUP>S will use clock unit 0 to serve as clock
 * - The default data line state is 0, when there is no data
 * - I<SUP>2</SUP>S will transmit data to output line
 * - The data pin and MUX configuration are not set
 *
 * \param[out]  config  Pointer to a I<SUP>2</SUP>S module Serializer configuration struct
 *                      to set
 */
static inline void i2s_serializer_get_config_defaults(
		struct i2s_serializer_config *const config)
{
	config->loop_back = false;

	config->mono_mode = false;

	config->disable_data_slot[0] = false;
	config->disable_data_slot[1] = false;
	config->disable_data_slot[2] = false;
	config->disable_data_slot[3] = false;
	config->disable_data_slot[4] = false;
	config->disable_data_slot[5] = false;
	config->disable_data_slot[6] = false;
	config->disable_data_slot[7] = false;

	config->transfer_lsb_first = false;
	config->data_adjust_left_in_word = false;
	config->data_adjust_left_in_slot = true;

	config->data_size = I2S_DATA_SIZE_16BIT;

	config->bit_padding = I2S_BIT_PADDING_0;
	config->data_padding = I2S_DATA_PADDING_0;

	config->dma_usage = I2S_DMA_USE_SINGLE_CHANNEL_FOR_ALL;

	config->clock_unit = I2S_CLOCK_UNIT_0;

	config->line_default_state = I2S_LINE_DEFAULT_0;

	config->mode = I2S_SERIALIZER_TRANSMIT;

	config->data_pin.enable = false;
	config->data_pin.gpio = 0;
	config->data_pin.mux = 0;
}

enum status_code i2s_serializer_set_config(
		struct i2s_module *const module_inst,
		const enum i2s_serializer serializer,
		const struct i2s_serializer_config *config);
/** @} */

/**
 * \name Serializer Enable/Disable
 * @{
 */

/**
 * \brief Enable the Specified Serializer of I<SUP>2</SUP>S module.
 *
 * Enables a Serializer in I<SUP>2</SUP>S module that has been previously initialized.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 * \param[in]  serializer    I<SUP>2</SUP>S Serializer to enable
 */
static inline void i2s_serializer_enable(
		const struct i2s_module *const module_inst,
		const enum i2s_serializer serializer)
{
	uint32_t seren_bit;

	Assert(module_inst);
	Assert(module_inst->hw);

	seren_bit = I2S_CTRLA_SEREN0 << serializer;

	while (module_inst->hw->SYNCBUSY.reg & seren_bit) {
		/* Sync wait */
	}
	module_inst->hw->CTRLA.reg |= seren_bit;
}

/**
 * \brief Disable the Specified Serializer of I<SUP>2</SUP>S module.
 *
 * Disables a Serializer in I<SUP>2</SUP>S module that has been previously initialized.
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 * \param[in]  serializer    I<SUP>2</SUP>S Serializer to disable
 */
static inline void i2s_serializer_disable(
		const struct i2s_module *const module_inst,
		const enum i2s_serializer serializer)
{
	uint32_t seren_bit;

	Assert(module_inst);
	Assert(module_inst->hw);

	seren_bit = I2S_CTRLA_SEREN0 << serializer;

	while (module_inst->hw->SYNCBUSY.reg & seren_bit) {
		/* Sync wait */
	}
	module_inst->hw->CTRLA.reg &= ~seren_bit;
}
/** @} */

/**
 * \name Status Management
 * @{
 */

uint32_t i2s_get_status(
		const struct i2s_module *const module_inst);

void i2s_clear_status(
		const struct i2s_module *const module_inst,
		uint32_t status);


enum status_code i2s_enable_status_interrupt(
		struct i2s_module *const module_inst,
		uint32_t status);

void i2s_disable_status_interrupt(
		struct i2s_module *const module_inst,
		uint32_t status);

/** @}*/

/**
 * \name Data Read/Write
 * @{
 */

/**
 * \brief Write a data word to the specified Serializer of I<SUP>2</SUP>S module
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 * \param[in]  serializer    The Serializer to write to
 * \param[in]  data          The data to write
 *
 */
static inline void i2s_serializer_write_wait(
		const struct i2s_module *const module_inst,
		enum i2s_serializer serializer,
		uint32_t data)
{
	uint32_t sync_bit, ready_bit;

	Assert(module_inst);
	Assert(module_inst->hw);

	ready_bit = I2S_INTFLAG_TXRDY0 << serializer;
	while (!(module_inst->hw->INTFLAG.reg & ready_bit)) {
		/* Wait until ready to transmit */
	}
	sync_bit = I2S_SYNCBUSY_DATA0 << serializer;
	while (module_inst->hw->SYNCBUSY.reg & sync_bit) {
		/* Wait sync */
	}
	/* Write data */
	module_inst->hw->DATA[serializer].reg = data;
	module_inst->hw->INTFLAG.reg = ready_bit;
}

/**
 * \brief Read a data word from the specified Serializer of I<SUP>2</SUP>S module
 *
 * \param[in]  module_inst   Pointer to the software module instance struct
 * \param[in]  serializer    The Serializer to read
 */
static inline uint32_t i2s_serializer_read_wait(
		const struct i2s_module *const module_inst,
		enum i2s_serializer serializer)
{
	uint32_t sync_bit, ready_bit;
	uint32_t data;

	Assert(module_inst);
	Assert(module_inst->hw);

	ready_bit = I2S_INTFLAG_RXRDY0 << serializer;
	while (!(module_inst->hw->INTFLAG.reg & ready_bit)) {
		/* Wait until ready to transmit */
	}
	sync_bit = I2S_SYNCBUSY_DATA0 << serializer;
	while (module_inst->hw->SYNCBUSY.reg & sync_bit) {
		/* Wait sync */
	}
	/* Read data */
	data = module_inst->hw->DATA[serializer].reg;
	module_inst->hw->INTFLAG.reg = ready_bit;
	return data;
}

enum status_code i2s_serializer_write_buffer_wait(
		const struct i2s_module *const module_inst,
		enum i2s_serializer serializer,
		void *buffer, uint32_t size);

enum status_code i2s_serializer_read_buffer_wait(
		const struct i2s_module *const module_inst,
		enum i2s_serializer serializer,
		void *buffer, uint32_t size);

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */


/**
 * \page asfdoc_sam0_i2s_extra Extra Information for I2S Driver
 *
 * \section asfdoc_sam0_i2s_extra_acronyms Acronyms
 * Below is a table listing the acronyms used in this module, along with their
 * intended meanings.
 *
 * <table>
 *  <tr>
 *      <th>Acronym</th>
 *      <th>Description</th>
 *  </tr>
 *  <tr>
 *      <td>I<SUP>2</SUP>S, IIS</td>
 *      <td>Inter-IC Sound Controller</td>
 *  </tr>
 *  <tr>
 *      <td>MCK</td>
 *      <td>Master Clock</td>
 *  </tr>
 *  <tr>
 *      <td>SCK</td>
 *      <td>Serial Clock</td>
 *  </tr>
 *  <tr>
 *      <td>FS</td>
 *      <td>Frame Sync</td>
 *  </tr>
 *  <tr>
 *      <td>SD</td>
 *      <td>Serial Data</td>
 *  </tr>
 *  <tr>
 *      <td>ADC</td>
 *      <td>Analog-to-Digital Converter</td>
 *  </tr>
 *  <tr>
 *      <td>DAC</td>
 *      <td>Digital-to-Analog Converter</td>
 *  </tr>
 *  <tr>
 *      <td>TDM</td>
 *      <td>Time Division Multiplexed</td>
 *  </tr>
 *  <tr>
 *      <td>PDM</td>
 *      <td>Pulse Density Modulation</td>
 *  </tr>
 *  <tr>
 *      <td>LSB</td>
 *      <td>Least Significant Bit</td>
 *  </tr>
 *  <tr>
 *      <td>MSB</td>
 *      <td>Most Significant Bit</td>
 *  </tr>
 *  <tr>
 *      <td>DSP</td>
 *      <td>Digital Signal Processor</td>
 *  </tr>
 * </table>
 *
 *
 * \section asfdoc_sam0_i2s_extra_dependencies Dependencies
 * This driver has the following dependencies:
 *
 *  - \ref asfdoc_sam0_system_pinmux_group "System Pin Multiplexer Driver"
 *
 *
 * \section asfdoc_sam0_i2s_extra_errata Errata
 * There are no errata related to this driver.
 *
 *
 * \section asfdoc_sam0_i2s_extra_history Module History
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
 *		<td>Initial Release</td>
 *	</tr>
 * </table>
 */

/**
 * \page asfdoc_sam0_i2s_exqsg Examples for I2S Driver
 *
 * This is a list of the available Quick Start guides (QSGs) and example
 * applications for \ref asfdoc_sam0_i2s_group. QSGs are simple examples with
 * step-by-step instructions to configure and use this driver in a selection of
 * use cases. Note that QSGs can be compiled as a standalone application or be
 * added to the user application.
 *
 *  - \subpage asfdoc_sam0_i2s_basic_use_case
 * \if I2S_CALLBACK_MODE
 *  - \subpage asfdoc_sam0_i2s_callback_use_case
 * \endif
 *  - \subpage asfdoc_sam0_i2s_dma_use_case
 *
 * \page asfdoc_sam0_i2s_document_revision_history Document Revision History
 *
 * <table>
 *	<tr>
 *		<th>Doc. Rev.</th>
 *		<th>Date</th>
 *		<th>Comments</th>
 *	</tr>
 *	<tr>
 *		<td>42255B</td>
 *		<td>12/2015</td>
 *		<td>Added support for SAM DA1</td>
 *	</tr>
 *	<tr>
 *		<td>42255A</td>
 *		<td>01/2014</td>
 *		<td>Initial release</td>
 *	</tr>
 * </table>
 */

#endif /* #ifndef I2S_H_INCLUDED */
