/**
 * \file
 *
 * \brief SAM Serial Peripheral Interface Driver
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

#ifndef SPI_H_INCLUDED
#define SPI_H_INCLUDED

/**
 * \defgroup asfdoc_sam0_sercom_spi_group SAM Serial Peripheral Interface (SERCOM SPI) Driver
 *
 * This driver for Atmel&reg; | SMART ARM&reg;-based microcontrollers provides
 * an interface for the configuration and management of the SERCOM module in
 * its SPI mode to transfer SPI  data frames. The following driver API modes
 * are covered by this manual:
 *
 * - Polled APIs
 * \if SPI_CALLBACK_MODE
 * - Callback APIs
 * \endif
 *
 * The following peripheral is used by this module:
 * - SERCOM (Serial Communication Interface)
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
 * - \ref asfdoc_sam0_sercom_spi_prerequisites
 * - \ref asfdoc_sam0_sercom_spi_module_overview
 * - \ref asfdoc_sam0_sercom_spi_special_considerations
 * - \ref asfdoc_sam0_sercom_spi_extra_info
 * - \ref asfdoc_sam0_sercom_spi_examples
 * - \ref asfdoc_sam0_sercom_spi_api_overview
 *
 * \section asfdoc_sam0_sercom_spi_prerequisites Prerequisites
 * There are no prerequisites.
 *
 *
 * \section asfdoc_sam0_sercom_spi_module_overview Module Overview
 * The Serial Peripheral Interface (SPI) is a high-speed synchronous data
 * transfer interface using three or four pins. It allows fast communication
 * between a master device and one or more peripheral devices.
 *
 * A device connected to the bus must act as a master or a slave. The master
 * initiates and controls all data transactions.
 * The SPI master initiates a communication cycle by pulling low the Slave
 * Select (SS) pin of the desired slave. The Slave Select pin is active low.
 * Master and slave prepare data to be sent in their respective shift
 * registers, and the master generates the required clock pulses on the SCK
 * line to interchange data. Data is always shifted from master to slave on
 * the Master Out - Slave In (MOSI) line, and from slave to master on the
 * Master In - Slave Out (MISO) line. After each data transfer, the master can
 * synchronize to the slave by pulling the SS line high.
 *
 * \subsection asfdoc_sam0_sercom_spi_module_features Driver Feature Macro Definition
 * <table>
 *  <tr>
 *    <th>Driver feature macro</th>
 *    <th>Supported devices</th>
 *  </tr>
 *  <tr>
 *    <td>FEATURE_SPI_SLAVE_SELECT_LOW_DETECT</td>
 *    <td>SAM D21/R21/D10/D11/L21/L22/DA1/C20/C21/R30</td>
 *  </tr>
 *  <tr>
 *    <td>FEATURE_SPI_HARDWARE_SLAVE_SELECT</td>
 *    <td>SAM D21/R21/D10/D11/L21/L22/DA1/C20/C21/R30</td>
 *  </tr>
 *  <tr>
 *    <td>FEATURE_SPI_ERROR_INTERRUPT</td>
 *    <td>SAM D21/R21/D10/D11/L21/L22/DA1/C20/C21/R30</td>
 *  </tr>
 *  <tr>
 *    <td>FEATURE_SPI_SYNC_SCHEME_VERSION_2</td>
 *    <td>SAM D21/R21/D10/D11/L21/L22/DA1/C20/C21/R30</td>
 *  </tr>
 * </table>
 * \note The specific features are only available in the driver when the
 * selected device supports those features.
 *
 * \subsection asfdoc_sam0_sercom_spi_bus SPI Bus Connection
 * In \ref asfdoc_sam0_spi_connection_example "the figure below", the
 * connection between one master and one slave is shown.
 *
 * \anchor asfdoc_sam0_spi_connection_example
 * \dot
 * digraph spi_slaves_par {
 *   subgraph cluster_spi_master {
 *    shift_reg [label="Shift register", shape=box];
 *    mosi_m [label="MOSI", shape=none];
 *    miso_m [label="MISO", shape=none];
 *    sck_m [label="SCK", shape=none];
 *    ss_m [label="GPIO pin", shape=none];
 *    {rank=same; mosi_m miso_m sck_m ss_m}
 *    label="SPI Master";
 *   }
 *   subgraph cluster_spi_slave {
 *    mosi_s [label="MOSI", shape=none];
 *    miso_s [label="MISO", shape=none];
 *    sck_s [label="SCK", shape=none];
 *    ss_s [label="SS", shape=none];
 *    shift_reg_s [label="Shift register", shape=box];
 *    {rank=same; mosi_s miso_s sck_s ss_s}
 *    label="SPI Slave";
 *    rankdir=LR;
 *   }
 *   shift_reg:e -> mosi_m:w [label=""];
 *   mosi_m:e -> mosi_s:w [label=""];
 *   mosi_s:e -> shift_reg_s:w [label=""];
 *   miso_s:w -> miso_m:e [label=""];
 *   sck_m -> sck_s;
 *   ss_m -> ss_s;
 *   shift_reg_s:se -> miso_s:e [label=""];
 *   miso_m:w -> shift_reg:sw [label=""];
 *   rankdir=LR;
 * }
 * \enddot
 *
 * The different lines are as follows:
 * - \b MISO Master Input Slave Output. The line where the data is shifted
 *   out from the slave and into the master.
 * - \b MOSI Master Output Slave Input. The line where the data is shifted
 *   out from the master and into the slave.
 * - \b SCK Serial Clock. Generated by the master device.
 * - \b SS Slave Select. To initiate a transaction, the master must pull this
 *      line low.
 *
 * If the bus consists of several SPI slaves, they can be connected in parallel
 * and the SPI master can use general I/O pins to control separate SS lines to
 * each slave on the bus.
 *
 * It is also possible to connect all slaves in series. In this configuration,
 * a common SS is provided to \c N slaves, enabling them simultaneously. The
 * MISO from the \c N-1 slaves is connected to the MOSI on the next slave. The
 * \c N<SUP>th</SUP> slave connects its MISO back to the master. For a
 * complete transaction, the master must shift \c N+1 characters.
 *
 * \subsection asfdoc_sam0_sercom_spi_chsize SPI Character Size
 * The SPI character size is configurable to eight or nine bits.
 *
 * \subsection asfdoc_sam0_sercom_spi_master_mode Master Mode
 * When configured as a master, the SS pin will be configured as an output.
 *
 * \subsubsection asfdoc_sam0_sercom_spi_master_mode_data_transfer Data Transfer
 * Writing a character will start the SPI clock generator, and
 * the character is transferred to the shift register when the shift
 * register is empty.
 * Once this is done, a new character can be written.
 * As each character is shifted out from the master, a character is shifted in
 * from the slave. If the receiver is enabled, the data is moved to the receive
 * buffer at the completion of the frame and can be read.
 *
 * \subsection asfdoc_sam0_sercom_spi_slave_mode Slave Mode
 * When configured as a slave, the SPI interface will remain inactive with MISO
 * tri-stated as long as the SS pin is driven high.
 *
 * \subsubsection asfdoc_sam0_sercom_spi_slave_mode_data_transfer_slave Data Transfer
 * The data register can be updated at any time.
 * As the SPI slave shift register is clocked by SCK, a minimum of three SCK
 * cycles are needed from the time new data is written, until the character is
 * ready to be shifted out. If the shift register has not been loaded with
 * data, the current contents will be transmitted.
 *
 * If constant transmission of data is needed in SPI slave mode, the system
 * clock should be faster than SCK.
 * If the receiver is enabled, the received character can be read from the
 * receive buffer. When SS line is driven high, the slave will not receive any
 * additional data.
 *
 * \subsubsection asfdoc_sam0_sercom_spi_slave_mode_addr_recognition Address Recognition
 * When the SPI slave is configured with address recognition, the first
 * character in a transaction is checked for an address match. If there is a
 * match, the MISO output is enabled and the transaction is processed.
 * If the address does not match, the complete transaction is ignored.
 *
 * If the device is asleep, it can be woken up by an address match in order
 * to process the transaction.
 *
 * \note In master mode, an address packet is written by the
 *       \ref spi_select_slave function if the address_enabled configuration is
 *       set in the \ref spi_slave_inst_config struct.
 *
 * \subsection asfdoc_sam0_sercom_spi_data_modes Data Modes
 * There are four combinations of SCK phase and polarity with respect to
 * serial data. \ref asfdoc_sam0_spi_mode_table "The table below" shows the
 * clock polarity (CPOL) and clock phase (CPHA) in the different modes.
 * <i>Leading edge</i> is the first clock edge in a clock cycle and
 * <i>trailing edge</i> is the last clock edge in a clock cycle.
 *
 * \anchor asfdoc_sam0_spi_mode_table
 * <table>
 *   <caption>SPI Data Modes</caption>
 *   <tr>
 *      <th>Mode</th>
 *      <th>CPOL</th>
 *      <th>CPHA</th>
 *      <th>Leading Edge</th>
 *      <th>Trailing Edge</th>
 *   </tr>
 *   <tr>
 *      <td> 0 </td>
 *      <td> 0 </td>
 *      <td> 0 </td>
 *      <td> Rising, Sample </td>
 *      <td> Falling, Setup </td>
 *   </tr>
 *   <tr>
 *      <td> 1 </td>
 *      <td> 0 </td>
 *      <td> 1 </td>
 *      <td> Rising, Setup </td>
 *      <td> Falling, Sample </td>
 *   </tr>
 *   <tr>
 *      <td> 2 </td>
 *      <td> 1 </td>
 *      <td> 0 </td>
 *      <td> Falling, Sample </td>
 *      <td> Rising, Setup </td>
 *   </tr>
 *   <tr>
 *      <td> 3 </td>
 *      <td> 1 </td>
 *      <td> 1 </td>
 *      <td> Falling, Setup </td>
 *      <td> Rising, Sample </td>
 *   </tr>
 * </table>
 *
 *
 * \subsection asfdoc_sam0_sercom_spi_pads SERCOM Pads
 * The SERCOM pads are automatically configured as seen in
 * \ref asfdoc_sam0_spi_sercom_pad_table "the table below". If the receiver
 * is disabled, the data input (MISO for master, MOSI for slave) can be used
 * for other purposes.
 *
 * In master mode, the SS pin(s) must be configured using the \ref spi_slave_inst
 * struct.
 *
 * \anchor asfdoc_sam0_spi_sercom_pad_table
 * <table>
 *   <caption>SERCOM SPI Pad Usages</caption>
 *   <tr>
 *      <th> Pin </th>
 *      <th> Master SPI </th>
 *      <th> Slave SPI </th>
 *   </tr>
 *   <tr>
 *      <td> MOSI </td>
 *      <td> Output </td>
 *      <td> Input </td>
 *   </tr>
 *   <tr>
 *      <td> MISO </td>
 *      <td> Input </td>
 *      <td> Output </td>
 *   </tr>
 *   <tr>
 *      <td> SCK </td>
 *      <td> Output </td>
 *      <td> Input </td>
 *   </tr>
 *   <tr>
 *      <td> SS </td>
 *      <td> User defined output enable </td>
 *      <td> Input </td>
 *   </tr>
 * </table>
 *
 * \subsection asfdoc_sam0_sercom_spi_sleep_modes Operation in Sleep Modes
 * The SPI module can operate in all sleep modes by setting the run_in_standby
 * option in the \ref spi_config struct. The operation in slave and master mode
 * is shown in the table below.
 * <table>
 *   <tr>
 *      <th> run_in_standby </th>
 *      <th> Slave </th>
 *      <th> Master </th>
 *   </tr>
 *   <tr>
 *      <td> false </td>
 *      <td> Disabled, all reception is dropped </td>
 *      <td> GCLK is disabled when master is idle, wake on transmit complete </td>
 *   </tr>
 *   <tr>
 *      <td> true </td>
 *      <td> Wake on reception </td>
 *      <td> GCLK is enabled while in sleep modes, wake on all interrupts </td>
 *   </tr>
 * </table>
 *
 * \subsection asfdoc_sam0_sercom_spi_clock_generation Clock Generation
 * In SPI master mode, the clock (SCK) is generated internally using the
 * SERCOM baudrate generator. In SPI slave mode, the clock is provided by
 * an external master on the SCK pin. This clock is used to directly clock
 * the SPI shift register.
 *
 * \section asfdoc_sam0_sercom_spi_special_considerations Special Considerations
 * \subsection pin_mux pinmux Settings
 * The pin MUX settings must be configured properly, as not all settings
 * can be used in different modes of operation.
 *
 * \section asfdoc_sam0_sercom_spi_extra_info Extra Information
 * For extra information, see \ref asfdoc_sam0_sercom_spi_extra. This includes:
 * - \ref asfdoc_sam0_sercom_spi_extra_acronyms
 * - \ref asfdoc_sam0_sercom_spi_extra_dependencies
 * - \ref asfdoc_sam0_sercom_spi_extra_workarounds
 * - \ref asfdoc_sam0_sercom_spi_extra_history
 *
 * \section asfdoc_sam0_sercom_spi_examples Examples
 *
 * For a list of examples related to this driver, see
 * \ref asfdoc_sam0_sercom_spi_exqsg.
 *
 * \section asfdoc_sam0_sercom_spi_api_overview API Overview
 * @{
 */

#include <compiler.h>
#include <port.h>
#include <sercom.h>
#include <pinmux.h>
#include <string.h>
#include <conf_spi.h>

#  if SPI_CALLBACK_MODE == true
#  include <sercom_interrupt.h>
#  endif

#ifdef __cplusplus
extern "C" {
#endif

#if (CONF_SPI_MASTER_ENABLE == false) && (CONF_SPI_SLAVE_ENABLE == false)
#error "Not possible compile SPI driver, invalid driver configuration. Make sure that either/both CONF_SPI_MASTER_ENABLE/CONF_SPI_SLAVE_ENABLE is set to true."
#endif

/**
 * \name Driver Feature Definition
 * Define SERCOM SPI features set according to different device family.
 * @{
 */
#  if (SAMD21) || (SAMR21) || (SAMD11) || (SAMD10) || (SAML21) || (SAMDA1) || \
	  (SAML22) || (SAMC20) || (SAMC21) || (SAMD09) || (SAMR30) || defined(__DOXYGEN__)
/** SPI slave select low detection. */
#  define FEATURE_SPI_SLAVE_SELECT_LOW_DETECT
/** Slave select can be controlled by hardware. */
#  define FEATURE_SPI_HARDWARE_SLAVE_SELECT
/** SPI with error detect feature. */
#  define FEATURE_SPI_ERROR_INTERRUPT
/** SPI sync scheme version 2. */
#  define FEATURE_SPI_SYNC_SCHEME_VERSION_2
#  endif
/*@}*/

#  ifndef PINMUX_DEFAULT
/** Default pinmux. */
#  define PINMUX_DEFAULT 0
#  endif

#  ifndef PINMUX_UNUSED
/** Unused pinmux. */
#  define PINMUX_UNUSED 0xFFFFFFFF
#  endif

#  ifndef SPI_TIMEOUT
/** SPI timeout value. */
#  define SPI_TIMEOUT 10000
#  endif

#  if SPI_CALLBACK_MODE == true
/**
 * \brief SPI Callback enum
 *
 * Callbacks for SPI callback driver.
 *
 * \note For slave mode, these callbacks will be called when a transaction
 * is ended by the master pulling Slave Select high.
 *
 */
enum spi_callback {
	/** Callback for buffer transmitted */
	SPI_CALLBACK_BUFFER_TRANSMITTED,
	/** Callback for buffer received */
	SPI_CALLBACK_BUFFER_RECEIVED,
	/** Callback for buffers transceived */
	SPI_CALLBACK_BUFFER_TRANSCEIVED,
	/** Callback for error */
	SPI_CALLBACK_ERROR,
	/**
	* Callback for transmission ended by master before the entire buffer was
	* read or written from slave
	*/
	SPI_CALLBACK_SLAVE_TRANSMISSION_COMPLETE,
#  ifdef FEATURE_SPI_SLAVE_SELECT_LOW_DETECT
	/** Callback for  slave select low */
	SPI_CALLBACK_SLAVE_SELECT_LOW,
#  endif
#  ifdef FEATURE_SPI_ERROR_INTERRUPT
	/** Callback for combined error happen */
	SPI_CALLBACK_COMBINED_ERROR,
#  endif
#  if !defined(__DOXYGEN__)
	/** Number of available callbacks */
	SPI_CALLBACK_N,
#  endif
};
#  endif

#if SPI_CALLBACK_MODE == true
#  if !defined(__DOXYGEN__)
/**
 * \internal SPI transfer directions
 */
enum _spi_direction {
	/** Transfer direction is read */
	SPI_DIRECTION_READ,
	/** Transfer direction is write */
	SPI_DIRECTION_WRITE,
	/** Transfer direction is read and write */
	SPI_DIRECTION_BOTH,
	/** No transfer */
	SPI_DIRECTION_IDLE,
};
#  endif
#endif

/**
 * \brief SPI Interrupt Flags
 *
 * Interrupt flags for the SPI module.
 *
 */
enum spi_interrupt_flag {
	/**
	 * This flag is set when the contents of the data register has been moved
	 * to the shift register and the data register is ready for new data
	 */
	SPI_INTERRUPT_FLAG_DATA_REGISTER_EMPTY = SERCOM_SPI_INTFLAG_DRE,
	/**
	 * This flag is set when the contents of the shift register has been
	 * shifted out
	 */
	SPI_INTERRUPT_FLAG_TX_COMPLETE         = SERCOM_SPI_INTFLAG_TXC,
	/** This flag is set when data has been shifted into the data register */
	SPI_INTERRUPT_FLAG_RX_COMPLETE         = SERCOM_SPI_INTFLAG_RXC,
#  ifdef FEATURE_SPI_SLAVE_SELECT_LOW_DETECT
	/** This flag is set when slave select low  */
	SPI_INTERRUPT_FLAG_SLAVE_SELECT_LOW         = SERCOM_SPI_INTFLAG_SSL,
#  endif
#  ifdef FEATURE_SPI_ERROR_INTERRUPT
	/** This flag is set when combined error happen */
	SPI_INTERRUPT_FLAG_COMBINED_ERROR         = SERCOM_SPI_INTFLAG_ERROR,
#  endif
};

/**
 * \brief SPI transfer modes enum
 *
 * SPI transfer mode.
 */
enum spi_transfer_mode {
	/** Mode 0. Leading edge: rising, sample. Trailing edge: falling, setup */
	SPI_TRANSFER_MODE_0 = 0,
	/** Mode 1. Leading edge: rising, setup. Trailing edge: falling, sample */
	SPI_TRANSFER_MODE_1 = SERCOM_SPI_CTRLA_CPHA,
	/** Mode 2. Leading edge: falling, sample. Trailing edge: rising, setup */
	SPI_TRANSFER_MODE_2 = SERCOM_SPI_CTRLA_CPOL,
	/** Mode 3. Leading edge: falling, setup. Trailing edge: rising, sample */
	SPI_TRANSFER_MODE_3 = SERCOM_SPI_CTRLA_CPHA | SERCOM_SPI_CTRLA_CPOL,
};

/**
 * \brief SPI frame format enum
 *
 * Frame format for slave mode.
 */
enum spi_frame_format {
	/** SPI frame */
	SPI_FRAME_FORMAT_SPI_FRAME      = SERCOM_SPI_CTRLA_FORM(0),
	/** SPI frame with address */
	SPI_FRAME_FORMAT_SPI_FRAME_ADDR = SERCOM_SPI_CTRLA_FORM(2),
};

/**
 * \brief SPI signal MUX settings
 *
 * Set the functionality of the SERCOM pins. As not all combinations can be used
 * in different modes of operation, proper combinations must be chosen according
 * to the rest of the configuration.
 *
 * \note In master operation: DI is MISO, DO is MOSI.
 *       In slave operation: DI is MOSI, DO is MISO.
 *
 * See \ref asfdoc_sam0_sercom_spi_mux_settings for a description of the
 * various MUX setting options.
 */
enum spi_signal_mux_setting {
	/** SPI MUX combination A. DOPO: 0x0, DIPO: 0x0 */
	SPI_SIGNAL_MUX_SETTING_A =
			(0x0 << SERCOM_SPI_CTRLA_DOPO_Pos) |
			(0x0 << SERCOM_SPI_CTRLA_DIPO_Pos),
	/** SPI MUX combination B. DOPO: 0x0, DIPO: 0x1 */
	SPI_SIGNAL_MUX_SETTING_B =
			(0x0 << SERCOM_SPI_CTRLA_DOPO_Pos) |
			(0x1 << SERCOM_SPI_CTRLA_DIPO_Pos),
	/** SPI MUX combination C. DOPO: 0x0, DIPO: 0x2 */
	SPI_SIGNAL_MUX_SETTING_C =
			(0x0 << SERCOM_SPI_CTRLA_DOPO_Pos) |
			(0x2 << SERCOM_SPI_CTRLA_DIPO_Pos),
	/** SPI MUX combination D. DOPO: 0x0, DIPO: 0x3 */
	SPI_SIGNAL_MUX_SETTING_D =
			(0x0 << SERCOM_SPI_CTRLA_DOPO_Pos) |
			(0x3 << SERCOM_SPI_CTRLA_DIPO_Pos),
	/** SPI MUX combination E. DOPO: 0x1, DIPO: 0x0 */
	SPI_SIGNAL_MUX_SETTING_E =
			(0x1 << SERCOM_SPI_CTRLA_DOPO_Pos) |
			(0x0 << SERCOM_SPI_CTRLA_DIPO_Pos),
	/** SPI MUX combination F. DOPO: 0x1, DIPO: 0x1 */
	SPI_SIGNAL_MUX_SETTING_F =
			(0x1 << SERCOM_SPI_CTRLA_DOPO_Pos) |
			(0x1 << SERCOM_SPI_CTRLA_DIPO_Pos),
	/** SPI MUX combination G. DOPO: 0x1, DIPO: 0x2 */
	SPI_SIGNAL_MUX_SETTING_G =
			(0x1 << SERCOM_SPI_CTRLA_DOPO_Pos) |
			(0x2 << SERCOM_SPI_CTRLA_DIPO_Pos),
	/** SPI MUX combination H. DOPO: 0x1, DIPO: 0x3 */
	SPI_SIGNAL_MUX_SETTING_H =
			(0x1 << SERCOM_SPI_CTRLA_DOPO_Pos) |
			(0x3 << SERCOM_SPI_CTRLA_DIPO_Pos),
	/** SPI MUX combination I. DOPO: 0x2, DIPO: 0x0 */
	SPI_SIGNAL_MUX_SETTING_I =
			(0x2 << SERCOM_SPI_CTRLA_DOPO_Pos) |
			(0x0 << SERCOM_SPI_CTRLA_DIPO_Pos),
	/** SPI MUX combination J. DOPO: 0x2, DIPO: 0x1 */
	SPI_SIGNAL_MUX_SETTING_J =
			(0x2 << SERCOM_SPI_CTRLA_DOPO_Pos) |
			(0x1 << SERCOM_SPI_CTRLA_DIPO_Pos),
	/** SPI MUX combination K. DOPO: 0x2, DIPO: 0x2 */
	SPI_SIGNAL_MUX_SETTING_K =
			(0x2 << SERCOM_SPI_CTRLA_DOPO_Pos) |
			(0x2 << SERCOM_SPI_CTRLA_DIPO_Pos),
	/** SPI MUX combination L. DOPO: 0x2, DIPO: 0x3 */
	SPI_SIGNAL_MUX_SETTING_L =
			(0x2 << SERCOM_SPI_CTRLA_DOPO_Pos) |
			(0x3 << SERCOM_SPI_CTRLA_DIPO_Pos),
	/** SPI MUX combination M. DOPO: 0x3, DIPO: 0x0 */
	SPI_SIGNAL_MUX_SETTING_M =
			(0x3 << SERCOM_SPI_CTRLA_DOPO_Pos) |
			(0x0 << SERCOM_SPI_CTRLA_DIPO_Pos),
	/** SPI MUX combination N. DOPO: 0x3, DIPO: 0x1 */
	SPI_SIGNAL_MUX_SETTING_N =
			(0x3 << SERCOM_SPI_CTRLA_DOPO_Pos) |
			(0x1 << SERCOM_SPI_CTRLA_DIPO_Pos),
	/** SPI MUX combination O. DOPO: 0x3, DIPO: 0x2 */
	SPI_SIGNAL_MUX_SETTING_O =
			(0x3 << SERCOM_SPI_CTRLA_DOPO_Pos) |
			(0x2 << SERCOM_SPI_CTRLA_DIPO_Pos),
	/** SPI MUX combination P. DOPO: 0x3, DIPO: 0x3 */
	SPI_SIGNAL_MUX_SETTING_P =
			(0x3 << SERCOM_SPI_CTRLA_DOPO_Pos) |
			(0x3 << SERCOM_SPI_CTRLA_DIPO_Pos),
};

/**
 * \brief SPI address modes enum
 *
 * For slave mode when using the SPI frame with address format.
 *
 */
enum spi_addr_mode {
	/**
	 * \c address_mask in the \ref spi_config struct is used as a mask to the register
	 */
	SPI_ADDR_MODE_MASK      = SERCOM_SPI_CTRLB_AMODE(0),
	/**
	 * The slave responds to the two unique addresses in \c address and
	 * \c address_mask in the \ref spi_config struct
	 */
	SPI_ADDR_MODE_UNIQUE    = SERCOM_SPI_CTRLB_AMODE(1),
	/**
	 * The slave responds to the range of addresses between and including \c address
	 * and \c address_mask in the \ref spi_config struct
	 */
	SPI_ADDR_MODE_RANGE     = SERCOM_SPI_CTRLB_AMODE(2),
};

/**
 * \brief SPI modes enum
 *
 * SPI mode selection.
 */
enum spi_mode {
	/** Master mode */
	SPI_MODE_MASTER         = 1,
	/** Slave mode */
	SPI_MODE_SLAVE          = 0,
};

/**
 * \brief SPI data order enum
 *
 * SPI data order.
 *
 */
enum spi_data_order {
	/** The LSB of the data is transmitted first */
	SPI_DATA_ORDER_LSB      = SERCOM_SPI_CTRLA_DORD,
	/** The MSB of the data is transmitted first */
	SPI_DATA_ORDER_MSB      = 0,
};

/**
 * \brief SPI character size enum
 *
 * SPI character size.
 *
 */
enum spi_character_size {
	/** 8-bit character */
	SPI_CHARACTER_SIZE_8BIT = SERCOM_SPI_CTRLB_CHSIZE(0),
	/** 9-bit character */
	SPI_CHARACTER_SIZE_9BIT = SERCOM_SPI_CTRLB_CHSIZE(1),
};

#  if SPI_CALLBACK_MODE == true
/** Prototype for the device instance */
struct spi_module;

/** Type of the callback functions */
typedef void (*spi_callback_t)(struct spi_module *const module);

#  if !defined(__DOXYGEN__)
/** Prototype for the interrupt handler */
extern void _spi_interrupt_handler(uint8_t instance);
#  endif
#  endif

/**
 * \brief SERCOM SPI driver software device instance structure.
 *
 * SERCOM SPI driver software instance structure, used to retain software state
 * information of an associated hardware module instance.
 *
 * \note The fields of this structure should not be altered by the user
 *       application; they are reserved for module-internal use only.
 */
struct spi_module {
#  if !defined(__DOXYGEN__)
	/** SERCOM hardware module */
	Sercom *hw;
	/** Module lock */
	volatile bool locked;
	/** SPI mode */
	enum spi_mode mode;
	/** SPI character size */
	enum spi_character_size character_size;
	/** Receiver enabled */
	bool receiver_enabled;
#  ifdef FEATURE_SPI_HARDWARE_SLAVE_SELECT
	/** Enable Hardware Slave Select */
	bool master_slave_select_enable;
#  endif
#  if SPI_CALLBACK_MODE == true
	/** Direction of transaction */
	volatile enum _spi_direction dir;
	/** Array to store callback function pointers in */
	spi_callback_t callback[SPI_CALLBACK_N];
	/** Buffer pointer to where the next received character will be put */
	volatile uint8_t *rx_buffer_ptr;
	/** Buffer pointer to where the next character will be transmitted from
	**/
	volatile uint8_t *tx_buffer_ptr;
	/** Remaining characters to receive */
	volatile uint16_t remaining_rx_buffer_length;
	/** Remaining dummy characters to send when reading */
	volatile uint16_t remaining_dummy_buffer_length;
	/** Remaining characters to transmit */
	volatile uint16_t remaining_tx_buffer_length;
	/** Bit mask for callbacks registered */
	uint8_t registered_callback;
	/** Bit mask for callbacks enabled */
	uint8_t enabled_callback;
	/** Holds the status of the ongoing or last operation */
	volatile enum status_code status;
#  endif
#  endif
};

/**
 * \brief SPI peripheral slave instance structure
 *
 * SPI peripheral slave software instance structure, used to configure the
 * correct SPI transfer mode settings for an attached slave. See
 * \ref spi_select_slave.
 */
struct spi_slave_inst {
	/** Pin to use as slave select */
	uint8_t ss_pin;
	/** Address recognition enabled in slave device */
	bool address_enabled;
	/** Address of slave device */
	uint8_t address;
};

/**
 * \brief SPI peripheral slave configuration structure
 *
 * SPI Peripheral slave configuration structure.
 */
struct spi_slave_inst_config {
	/** Pin to use as slave select */
	uint8_t ss_pin;
	/** Enable address */
	bool address_enabled;
	/** Address of slave */
	uint8_t address;
};

/**
 * \brief SPI Master configuration structure
 *
 * SPI Master configuration structure.
 */
struct spi_master_config {
	/** Baud rate */
	uint32_t baudrate;
};

/**
 * \brief SPI slave configuration structure
 *
 * SPI slave configuration structure.
 */
struct spi_slave_config {
	/** Frame format */
	enum spi_frame_format frame_format;
	/** Address mode */
	enum spi_addr_mode address_mode;
	/** Address */
	uint8_t address;
	/** Address mask */
	uint8_t address_mask;
	/** Preload data to the shift register while SS is high */
	bool preload_enable;
};

/**
 * \brief SPI configuration structure
 *
 * Configuration structure for an SPI instance. This structure should be
 * initialized by the \ref spi_get_config_defaults function before being
 * modified by the user application.
 */
struct spi_config {
	/** SPI mode */
	enum spi_mode mode;
	/** Data order */
	enum spi_data_order data_order;
	/** Transfer mode */
	enum spi_transfer_mode transfer_mode;
	/** MUX setting */
	enum spi_signal_mux_setting mux_setting;
	/** SPI character size */
	enum spi_character_size character_size;
	/** Enabled in sleep modes */
	bool run_in_standby;
	/** Enable receiver */
	bool receiver_enable;
#  ifdef FEATURE_SPI_SLAVE_SELECT_LOW_DETECT
	/** Enable Slave Select Low Detect */
	bool select_slave_low_detect_enable;
#  endif
#  ifdef FEATURE_SPI_HARDWARE_SLAVE_SELECT
	/** Enable Master Slave Select */
	bool master_slave_select_enable;
#  endif
	/** Union for slave or master specific configuration */
	union {
		/** Slave specific configuration */
		struct spi_slave_config slave;
		/** Master specific configuration */
		struct spi_master_config master;
	} mode_specific;
	/** GCLK generator to use as clock source */
	enum gclk_generator generator_source;
	/** PAD0 pinmux */
	uint32_t pinmux_pad0;
	/** PAD1 pinmux */
	uint32_t pinmux_pad1;
	/** PAD2 pinmux */
	uint32_t pinmux_pad2;
	/** PAD3 pinmux */
	uint32_t pinmux_pad3;
};

/**
 * \brief Determines if the SPI module is currently synchronizing to the bus.
 *
 * This function will check if the underlying hardware peripheral module is
 * currently synchronizing across multiple clock domains to the hardware bus.
 * This function can be used to delay further operations on the module until it
 * is ready.
 *
 * \param[in]  module  SPI hardware module
 *
 * \return Synchronization status of the underlying hardware module.
 * \retval true   Module synchronization is ongoing
 * \retval false  Module synchronization is not ongoing
 *
 */
static inline bool spi_is_syncing(
		struct spi_module *const module)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

	SercomSpi *const spi_module = &(module->hw->SPI);

#  ifdef FEATURE_SPI_SYNC_SCHEME_VERSION_2
	/* Return synchronization status */
	return (spi_module->SYNCBUSY.reg);
#  else
	/* Return synchronization status */
	return (spi_module->STATUS.reg & SERCOM_SPI_STATUS_SYNCBUSY);
#  endif
}

/**
 * \name Driver Initialization and Configuration
 * @{
 */

/**
 * \brief Initializes an SPI configuration structure to default values
 *
 * This function will initialize a given SPI configuration structure to a set
 * of known default values. This function should be called on any new
 * instance of the configuration structures before being modified by the
 * user application.
 *
 * The default configuration is as follows:
 *  \li Master mode enabled
 *  \li MSB of the data is transmitted first
 *  \li Transfer mode 0
 *  \li MUX Setting D
 *  \li Character size eight bits
 *  \li Not enabled in sleep mode
 *  \li Receiver enabled
 *  \li Baudrate 100000
 *  \li Default pinmux settings for all pads
 *  \li GCLK generator 0
 *
 * \param[out] config  Configuration structure to initialize to default values
 */
static inline void spi_get_config_defaults(
		struct spi_config *const config)
{
	/* Sanity check arguments */
	Assert(config);

	/* Default configuration values */
	config->mode             = SPI_MODE_MASTER;
	config->data_order       = SPI_DATA_ORDER_MSB;
	config->transfer_mode    = SPI_TRANSFER_MODE_0;
	config->mux_setting      = SPI_SIGNAL_MUX_SETTING_D;
	config->character_size   = SPI_CHARACTER_SIZE_8BIT;
	config->run_in_standby   = false;
	config->receiver_enable  = true;
#  ifdef FEATURE_SPI_SLAVE_SELECT_LOW_DETECT
	config->select_slave_low_detect_enable= true;
#  endif
#  ifdef FEATURE_SPI_HARDWARE_SLAVE_SELECT
	config->master_slave_select_enable= false;
#  endif
	config->generator_source = GCLK_GENERATOR_0;

	/* Clear mode specific config */
	memset(&(config->mode_specific), 0, sizeof(config->mode_specific));

	/* Master config defaults */
	config->mode_specific.master.baudrate = 100000;

	/* pinmux config defaults */
	config->pinmux_pad0 = PINMUX_DEFAULT;
	config->pinmux_pad1 = PINMUX_DEFAULT;
	config->pinmux_pad2 = PINMUX_DEFAULT;
	config->pinmux_pad3 = PINMUX_DEFAULT;

};

/**
 * \brief Initializes an SPI peripheral slave device configuration structure to default values
 *
 * This function will initialize a given SPI slave device configuration
 * structure to a set of known default values. This function should be called
 * on any new instance of the configuration structures before being modified by
 * the user application.
 *
 * The default configuration is as follows:
 *  \li Slave Select on GPIO pin 10
 *  \li Addressing not enabled
 *
 * \param[out] config  Configuration structure to initialize to default values
 */
static inline void spi_slave_inst_get_config_defaults(
		struct spi_slave_inst_config *const config)
{
	Assert(config);

	config->ss_pin          = 10;
	config->address_enabled = false;
	config->address         = 0;
}

/**
 * \brief Attaches an SPI peripheral slave
 *
 * This function will initialize the software SPI peripheral slave, based on
 * the values of the config struct. The slave can then be selected and
 * optionally addressed by the \ref spi_select_slave function.
 *
 * \param[out] slave   Pointer to the software slave instance struct
 * \param[in]  config  Pointer to the config struct
 *
 */
static inline void spi_attach_slave(
		struct spi_slave_inst *const slave,
		const struct spi_slave_inst_config *const config)
{
	Assert(slave);
	Assert(config);

	slave->ss_pin          = config->ss_pin;
	slave->address_enabled = config->address_enabled;
	slave->address         = config->address;

	/* Get default config for pin */
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	/* Edit config to set the pin as output */
	pin_conf.direction = PORT_PIN_DIR_OUTPUT;

	/* Set config on Slave Select pin */
	port_pin_set_config(slave->ss_pin, &pin_conf);
	port_pin_set_output_level(slave->ss_pin, true);
}

enum status_code spi_init(
		struct spi_module *const module,
		Sercom *const hw,
		const struct spi_config *const config);

/** @} */

/**
 * \name Enable/Disable
 * @{
 */

/**
 * \brief Enables the SERCOM SPI module
 *
 * This function will enable the SERCOM SPI module.
 *
 * \param[in,out] module  Pointer to the software instance struct
 */
static inline void spi_enable(
		struct spi_module *const module)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

	SercomSpi *const spi_module = &(module->hw->SPI);

#  if SPI_CALLBACK_MODE == true
	system_interrupt_enable(_sercom_get_interrupt_vector(module->hw));
#  endif

	while (spi_is_syncing(module)) {
		/* Wait until the synchronization is complete */
	}

	/* Enable SPI */
	spi_module->CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;
}

/**
 * \brief Disables the SERCOM SPI module
 *
 * This function will disable the SERCOM SPI module.
 *
 * \param[in,out] module  Pointer to the software instance struct
 */
static inline void spi_disable(
		struct spi_module *const module)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

	SercomSpi *const spi_module = &(module->hw->SPI);

#  if SPI_CALLBACK_MODE == true
	system_interrupt_disable(_sercom_get_interrupt_vector(module->hw));
#  endif

	while (spi_is_syncing(module)) {
		/* Wait until the synchronization is complete */
	}

	/* Disbale interrupt */
	spi_module->INTENCLR.reg = SERCOM_SPI_INTENCLR_MASK;
	/* Clear interrupt flag */
	spi_module->INTFLAG.reg = SERCOM_SPI_INTFLAG_MASK;

	/* Disable SPI */
	spi_module->CTRLA.reg &= ~SERCOM_SPI_CTRLA_ENABLE;
}

void spi_reset(
		struct spi_module *const module);

/** @} */

enum status_code spi_set_baudrate(
		struct spi_module *const module,
		uint32_t baudrate);

/**
 * \name Lock/Unlock
 * @{
 */

/**
 * \brief Attempt to get lock on driver instance
 *
 * This function checks the instance's lock, which indicates whether or not it
 * is currently in use, and sets the lock if it was not already set.
 *
 * The purpose of this is to enable exclusive access to driver instances, so
 * that, e.g., transactions by different services will not interfere with each
 * other.
 *
 * \param[in,out] module Pointer to the driver instance to lock
 *
 * \retval STATUS_OK If the module was locked
 * \retval STATUS_BUSY If the module was already locked
 */
static inline enum status_code spi_lock(struct spi_module *const module)
{
	enum status_code status;

	system_interrupt_enter_critical_section();

	if (module->locked) {
		status = STATUS_BUSY;
	} else {
		module->locked = true;
		status = STATUS_OK;
	}

	system_interrupt_leave_critical_section();

	return status;
}

/**
 * \brief Unlock driver instance
 *
 * This function clears the instance lock, indicating that it is available for
 * use.
 *
 * \param[in,out] module Pointer to the driver instance to lock
 *
 * \retval STATUS_OK If the module was locked
 * \retval STATUS_BUSY If the module was already locked
 */
static inline void spi_unlock(struct spi_module *const module)
{
	module->locked = false;
}

/** @} */

/**
 * \name Ready to Write/Read
 * @{
 */

 /**
 * \brief Checks if the SPI in master mode has shifted out last data, or if the master has ended the transfer in slave mode.
 *
 * This function will check if the SPI master module has shifted out last data,
 * or if the slave select pin has been drawn high by the master for the SPI
 * slave module.
 *
 * \param[in]  module  Pointer to the software instance struct
 *
 * \return Indication of whether any writes are ongoing.
 * \retval true   If the SPI master module has shifted out data, or slave select
 *                has been drawn high for SPI slave
 * \retval false  If the SPI master module has not shifted out data
 */
static inline bool spi_is_write_complete(
		struct spi_module *const module)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

	SercomSpi *const spi_module = &(module->hw->SPI);

	/* Check interrupt flag */
	return (spi_module->INTFLAG.reg & SERCOM_SPI_INTFLAG_TXC);
}

 /**
 * \brief Checks if the SPI module is ready to write data
 *
 * This function will check if the SPI module is ready to write data.
 *
 * \param[in]  module  Pointer to the software instance struct
 *
 * \return Indication of whether the module is ready to read data or not.
 * \retval true   If the SPI module is ready to write data
 * \retval false  If the SPI module is not ready to write data
 */
static inline bool spi_is_ready_to_write(
		struct spi_module *const module)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

	SercomSpi *const spi_module = &(module->hw->SPI);

	/* Check interrupt flag */
	return (spi_module->INTFLAG.reg & SERCOM_SPI_INTFLAG_DRE);
}

/**
 * \brief Checks if the SPI module is ready to read data
 *
 * This function will check if the SPI module is ready to read data.
 *
 * \param[in]  module Pointer to the software instance struct
 *
 * \return Indication of whether the module is ready to read data or not.
 * \retval true   If the SPI module is ready to read data
 * \retval false  If the SPI module is not ready to read data
 */
static inline bool spi_is_ready_to_read(
		struct spi_module *const module)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

	SercomSpi *const spi_module = &(module->hw->SPI);

	/* Check interrupt flag */
	return (spi_module->INTFLAG.reg & SERCOM_SPI_INTFLAG_RXC);
}
/** @} */

/**
 * \name Read/Write
 * @{
 */

 /**
 * \brief Transfers a single SPI character
 *
 * This function will send a single SPI character via SPI and ignore any data
 * shifted in by the connected device. To both send and receive data, use the
 * \ref spi_transceive_wait function or use the \ref spi_read function after
 * writing a character. The \ref spi_is_ready_to_write function
 * should be called before calling this function.
 *
 * Note that this function does not handle the SS (Slave Select)
 * pin(s) in master mode; this must be handled from the user application.
 *
 * \note In slave mode, the data will not be transferred before a master
 *       initiates a transaction.
 *
 * \param[in] module   Pointer to the software instance struct
 * \param[in] tx_data  Data to transmit
 *
 * \return Status of the procedure.
 * \retval STATUS_OK    If the data was written
 * \retval STATUS_BUSY  If the last write was not completed
 */
static inline enum status_code spi_write(
		struct spi_module *module,
		uint16_t tx_data)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

	SercomSpi *const spi_module = &(module->hw->SPI);

	/* Check if the data register has been copied to the shift register */
	if (!spi_is_ready_to_write(module)) {
		/* Data register has not been copied to the shift register, return */
		return STATUS_BUSY;
	}

	/* Write the character to the DATA register */
	spi_module->DATA.reg = tx_data & SERCOM_SPI_DATA_MASK;

	return STATUS_OK;
}

enum status_code spi_write_buffer_wait(
		struct spi_module *const module,
		const uint8_t *tx_data,
		uint16_t length);

/**
 * \brief Reads last received SPI character
 *
 * This function will return the last SPI character shifted into the receive
 * register by the \ref spi_write function.
 *
 * \note The \ref spi_is_ready_to_read function should be called before calling
 *       this function.
 *
 * \note Receiver must be enabled in the configuration.
 *
 * \param[in] module    Pointer to the software instance struct
 * \param[out] rx_data  Pointer to store the received data
 *
 * \returns Status of the read operation.
 * \retval STATUS_OK            If data was read
 * \retval STATUS_ERR_IO        If no data is available
 * \retval STATUS_ERR_OVERFLOW  If the data is overflown
 */
static inline enum status_code spi_read(
		struct spi_module *const module,
		uint16_t *rx_data)
{
	/* Sanity check arguments */
	Assert(module);
	Assert(module->hw);

	SercomSpi *const spi_module = &(module->hw->SPI);

	/* Check if data is ready to be read */
	if (!spi_is_ready_to_read(module)) {
		/* No data has been received, return */
		return STATUS_ERR_IO;
	}

	/* Return value */
	enum status_code retval = STATUS_OK;

	/* Check if data is overflown */
	if (spi_module->STATUS.reg & SERCOM_SPI_STATUS_BUFOVF) {
		retval = STATUS_ERR_OVERFLOW;
		/* Clear overflow flag */
		spi_module->STATUS.reg = SERCOM_SPI_STATUS_BUFOVF;
	}

	/* Read the character from the DATA register */
	if (module->character_size == SPI_CHARACTER_SIZE_9BIT) {
		*rx_data = (spi_module->DATA.reg & SERCOM_SPI_DATA_MASK);
	} else {
		*rx_data = (uint8_t)spi_module->DATA.reg;
	}

	return retval;
}

enum status_code spi_read_buffer_wait(
		struct spi_module *const module,
		uint8_t *rx_data,
		uint16_t length,
		uint16_t dummy);

enum status_code spi_transceive_wait(
		struct spi_module *const module,
		uint16_t tx_data,
		uint16_t *rx_data);

enum status_code spi_transceive_buffer_wait(
		struct spi_module *const module,
		uint8_t *tx_data,
		uint8_t *rx_data,
		uint16_t length);

enum status_code spi_select_slave(
		struct spi_module *const module,
		struct spi_slave_inst *const slave,
		bool select);

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */


/**
 * \page asfdoc_sam0_sercom_spi_extra Extra Information for SERCOM SPI Driver
 *
 * \section asfdoc_sam0_sercom_spi_extra_acronyms Acronyms
 * Below is a table listing the acronyms used in this module, along with their
 * intended meanings.
 *
 * <table>
 *	<tr>
 *		<th>Acronym</th>
 *		<th>Description</th>
 *	</tr>
 *	<tr>
 *		<td>SERCOM</td>
 *		<td>Serial Communication Interface</td>
 *	</tr>
 *	<tr>
 *		<td>SPI</td>
 *		<td>Serial Peripheral Interface</td>
 *	</tr>
 *	<tr>
 *		<td>SCK</td>
 *		<td>Serial Clock</td>
 *	</tr>
 *	<tr>
 *		<td>MOSI</td>
 *		<td>Master Output Slave Input</td>
 *	</tr>
 *	<tr>
 *		<td>MISO</td>
 *		<td>Master Input Slave Output</td>
 *	</tr>
 *	<tr>
 *		<td>SS</td>
 *		<td>Slave Select</td>
 *	</tr>
 *	<tr>
 *		<td>DIO</td>
 *		<td>Data Input Output</td>
 *	</tr>
 *	<tr>
 *		<td>DO</td>
 *		<td>Data Output</td>
 *	</tr>
 *	<tr>
 *		<td>DI</td>
 *		<td>Data Input</td>
 *	</tr>
 *	<tr>
 *		<td>DMA</td>
 *		<td>Direct Memory Access</td>
 *	</tr>
 * </table>
 *
 * \section asfdoc_sam0_sercom_spi_extra_dependencies Dependencies
 * The SPI driver has the following dependencies:
 * \li \ref asfdoc_sam0_system_pinmux_group "System Pin Multiplexer Driver"
 *
 *
 * \section asfdoc_sam0_sercom_spi_extra_workarounds Workarounds Implemented by Driver
 * No workarounds in driver.
 *
 * \section asfdoc_sam0_sercom_spi_extra_history Module History
 * An overview of the module history is presented in the table below, with
 * details on the enhancements and fixes made to the module since its first
 * release. The current version of this corresponds to the newest version in the table.
 *
 * <table>
 *	<tr>
 *		<th>Changelog</th>
 *	</tr>
 *	<tr>
 *		<td>Added new features as below:
 *             \li Slave select low detect
 *             \li Hardware slave select
 *             \li DMA support </td>
 *	</tr>
 *	<tr>
 *		<td>Edited slave part of write and transceive buffer functions to ensure
 *		that second character is sent at the right time</td>
 *	</tr>
 *	<tr>
 *		<td>Renamed the anonymous union in \c struct spi_config to
 *          \c mode_specific</td>
 *	</tr>
 *	<tr>
 *		<td>Initial Release</td>
 *	</tr>
 * </table>
 */

/**
 * \page asfdoc_sam0_sercom_spi_exqsg Examples for SERCOM SPI Driver
 *
 * This is a list of the available Quick Start guides (QSGs) and example
 * applications for \ref asfdoc_sam0_sercom_spi_group. QSGs are simple examples with
 * step-by-step instructions to configure and use this driver in a selection of
 * use cases. Note that a QSG can be compiled as a standalone application or be
 * added to the user application.
 *
 * - \subpage asfdoc_sam0_sercom_spi_master_basic_use
 * - \subpage asfdoc_sam0_sercom_spi_slave_basic_use
 * \if SPI_CALLBACK_MODE
 * - \subpage asfdoc_sam0_sercom_spi_master_callback_use
 * - \subpage asfdoc_sam0_sercom_spi_slave_callback_use
 * \endif
 * - \subpage asfdoc_sam0_sercom_spi_dma_use_case
 */

 /**
  * \page asfdoc_sam0_sercom_spi_mux_settings MUX Settings
  *
  * The following lists the possible internal SERCOM module pad function
  * assignments for the four SERCOM pads in both SPI Master and SPI Slave
  * modes. They are combinations of DOPO and DIPO in CTRLA.
  * Note that this is in addition to the physical GPIO pin MUX of the device,
  * and can be used in conjunction to optimize the serial data pin-out.
  *
  * \section asfdoc_sam0_sercom_spi_mux_settings_master Master Mode Settings
  * The following table describes the SERCOM pin functionalities for the various
  * MUX settings, whilst in SPI Master mode.
  *
  * \note If MISO is unlisted, the SPI receiver must not be enabled for the
  *       given MUX setting.
  *
  * <table>
  *		<tr>
  *			<th>Combination</th>
  *			<th>DOPO / DIPO</th>
  *			<th>SERCOM PAD[0]</th>
  *			<th>SERCOM PAD[1]</th>
  *			<th>SERCOM PAD[2]</th>
  *			<th>SERCOM PAD[3]</th>
  *		</tr>
  *		<tr>
  *			<td>A</td>
  *			<td>0x0 / 0x0</td>
  *			<td>MOSI</td>
  *			<td>SCK</td>
  *			<td>-</td>
  *			<td>-</td>
  *		</tr>
  *		<tr>
  *			<td>B</td>
  *			<td>0x0 / 0x1</td>
  *			<td>MOSI</td>
  *			<td>SCK</td>
  *			<td>-</td>
  *			<td>-</td>
  *		</tr>
  *		<tr>
  *			<td>C</td>
  *			<td>0x0 / 0x2</td>
  *			<td>MOSI</td>
  *			<td>SCK</td>
  *			<td>MISO</td>
  *			<td>-</td>
  *		</tr>
  *		<tr>
  *			<td>D</td>
  *			<td>0x0 / 0x3</td>
  *			<td>MOSI</td>
  *			<td>SCK</td>
  *			<td>-</td>
  *			<td>MISO</td>
  *		</tr>
  *		<tr>
  *			<td>E</td>
  *			<td>0x1 / 0x0</td>
  *			<td>MISO</td>
  *			<td>-</td>
  *			<td>MOSI</td>
  *			<td>SCK</td>
  *		</tr>
  *		<tr>
  *			<td>F</td>
  *			<td>0x1 / 0x1</td>
  *			<td>-</td>
  *			<td>MISO</td>
  *			<td>MOSI</td>
  *			<td>SCK</td>
  *		</tr>
  *		<tr>
  *			<td>G</td>
  *			<td>0x1 / 0x2</td>
  *			<td>-</td>
  *			<td>-</td>
  *			<td>MOSI</td>
  *			<td>SCK</td>
  *		</tr>
  *		<tr>
  *			<td>H</td>
  *			<td>0x1 / 0x3</td>
  *			<td>-</td>
  *			<td>-</td>
  *			<td>MOSI</td>
  *			<td>SCK</td>
  *		</tr>
  *		<tr>
  *			<td>I</td>
  *			<td>0x2 / 0x0</td>
  *			<td>MISO</td>
  *			<td>SCK</td>
  *			<td>-</td>
  *			<td>MOSI</td>
  *		</tr>
  *		<tr>
  *			<td>J</td>
  *			<td>0x2 / 0x1</td>
  *			<td>-</td>
  *			<td>SCK</td>
  *			<td>-</td>
  *			<td>MOSI</td>
  *		</tr>
  *		<tr>
  *			<td>K</td>
  *			<td>0x2 / 0x2</td>
  *			<td>-</td>
  *			<td>SCK</td>
  *			<td>MISO</td>
  *			<td>MOSI</td>
  *		</tr>
  *		<tr>
  *			<td>L</td>
  *			<td>0x2 / 0x3</td>
  *			<td>-</td>
  *			<td>SCK</td>
  *			<td>-</td>
  *			<td>MOSI</td>
  *		</tr>
  *		<tr>
  *			<td>M</td>
  *			<td>0x3 / 0x0</td>
  *			<td>MOSI</td>
  *			<td>-</td>
  *			<td>-</td>
  *			<td>SCK</td>
  *		</tr>
  *		<tr>
  *			<td>N</td>
  *			<td>0x3 / 0x1</td>
  *			<td>MOSI</td>
  *			<td>MISO</td>
  *			<td>-</td>
  *			<td>SCK</td>
  *		</tr>
  *		<tr>
  *			<td>O</td>
  *			<td>0x3 / 0x2</td>
  *			<td>MOSI</td>
  *			<td>-</td>
  *			<td>MISO</td>
  *			<td>SCK</td>
  *		</tr>
  *		<tr>
  *			<td>P</td>
  *			<td>0x3 / 0x3</td>
  *			<td>MOSI</td>
  *			<td>-</td>
  *			<td>-</td>
  *			<td>SCK</td>
  *		</tr>
  * </table>
  *
  *
  * \section asfdoc_sam0_sercom_spi_mux_settings_slave Slave Mode Settings
  * The following table describes the SERCOM pin functionalities for the various
  * MUX settings, whilst in SPI Slave mode.
  *
  * \note If MISO is unlisted, the SPI receiver must not be enabled for the
  *       given MUX setting.
  *
  * <table>
  *		<tr>
  *			<th>Combination</th>
  *			<th>DOPO / DIPO</th>
  *			<th>SERCOM PAD[0]</th>
  *			<th>SERCOM PAD[1]</th>
  *			<th>SERCOM PAD[2]</th>
  *			<th>SERCOM PAD[3]</th>
  *		</tr>
  *		<tr>
  *			<td>A</td>
  *			<td>0x0 / 0x0</td>
  *			<td>MISO</td>
  *			<td>SCK</td>
  *			<td>/SS</td>
  *			<td>-</td>
  *		</tr>
  *		<tr>
  *			<td>B</td>
  *			<td>0x0 / 0x1</td>
  *			<td>MISO</td>
  *			<td>SCK</td>
  *			<td>/SS</td>
  *			<td>-</td>
  *		</tr>
  *		<tr>
  *			<td>C</td>
  *			<td>0x0 / 0x2</td>
  *			<td>MISO</td>
  *			<td>SCK</td>
  *			<td>/SS</td>
  *			<td>-</td>
  *		</tr>
  *		<tr>
  *			<td>D</td>
  *			<td>0x0 / 0x3</td>
  *			<td>MISO</td>
  *			<td>SCK</td>
  *			<td>/SS</td>
  *			<td>MOSI</td>
  *		</tr>
  *		<tr>
  *			<td>E</td>
  *			<td>0x1 / 0x0</td>
  *			<td>MOSI</td>
  *			<td>/SS</td>
  *			<td>MISO</td>
  *			<td>SCK</td>
  *		</tr>
  *		<tr>
  *			<td>F</td>
  *			<td>0x1 / 0x1</td>
  *			<td>-</td>
  *			<td>/SS</td>
  *			<td>MISO</td>
  *			<td>SCK</td>
  *		</tr>
  *		<tr>
  *			<td>G</td>
  *			<td>0x1 / 0x2</td>
  *			<td>-</td>
  *			<td>/SS</td>
  *			<td>MISO</td>
  *			<td>SCK</td>
  *		</tr>
  *		<tr>
  *			<td>H</td>
  *			<td>0x1 / 0x3</td>
  *			<td>-</td>
  *			<td>/SS</td>
  *			<td>MISO</td>
  *			<td>SCK</td>
  *		</tr>
  *		<tr>
  *			<td>I</td>
  *			<td>0x2 / 0x0</td>
  *			<td>MOSI</td>
  *			<td>SCK</td>
  *			<td>/SS</td>
  *			<td>MISO</td>
  *		</tr>
  *		<tr>
  *			<td>J</td>
  *			<td>0x2 / 0x1</td>
  *			<td>-</td>
  *			<td>SCK</td>
  *			<td>/SS</td>
  *			<td>MISO</td>
  *		</tr>
  *		<tr>
  *			<td>K</td>
  *			<td>0x2 / 0x2</td>
  *			<td>-</td>
  *			<td>SCK</td>
  *			<td>/SS</td>
  *			<td>MISO</td>
  *		</tr>
  *		<tr>
  *			<td>L</td>
  *			<td>0x2 / 0x3</td>
  *			<td>-</td>
  *			<td>SCK</td>
  *			<td>/SS</td>
  *			<td>MISO</td>
  *		</tr>
  *		<tr>
  *			<td>M</td>
  *			<td>0x3 / 0x0</td>
  *			<td>MISO</td>
  *			<td>/SS</td>
  *			<td>-</td>
  *			<td>SCK</td>
  *		</tr>
  *		<tr>
  *			<td>N</td>
  *			<td>0x3 / 0x1</td>
  *			<td>MISO</td>
  *			<td>/SS</td>
  *			<td>-</td>
  *			<td>SCK</td>
  *		</tr>
  *		<tr>
  *			<td>O</td>
  *			<td>0x3 / 0x2</td>
  *			<td>MISO</td>
  *			<td>/SS</td>
  *			<td>MOSI</td>
  *			<td>SCK</td>
  *		</tr>
  *		<tr>
  *			<td>P</td>
  *			<td>0x3 / 0x3</td>
  *			<td>MISO</td>
  *			<td>/SS</td>
  *			<td>-</td>
  *			<td>SCK</td>
  *		</tr>
  * </table>
  *
  *
  *
  * \page asfdoc_sam0_sercom_spi_document_revision_history Document Revision History
  *
  * <table>
  *	<tr>
  *		<th>Doc. Rev.</th>
  *		<th>Date</th>
  *		<th>Comments</th>
  *	</tr>
  *	<tr>
  *		<td>42115E</td>
  *		<td>12/2015</td>
  *		<td>Add SAM L21/L22, SAM DA1, SAM D09, SAMR30 and SAM C21 support</td>
  *	</tr>
  *	<tr>
  *		<td>42115D</td>
  *		<td>12/2014</td>
  *		<td>Add SAM R21/D10/D11 support</td>
  *	</tr>
  *	<tr>
  *		<td>42115C</td>
  *		<td>01/2014</td>
  *		<td>Add SAM D21 support</td>
  *	</tr>
  *	<tr>
  *		<td>42115B</td>
  *		<td>11/2013</td>
  *		<td>Replaced the pad multiplexing documentation with a condensed table</td>
  *	</tr>
  *	<tr>
  *		<td>42115A</td>
  *		<td>06/2013</td>
  *		<td>Initial release</td>
  *	</tr>
  * </table>
  */

#endif /* SPI_H_INCLUDED */
