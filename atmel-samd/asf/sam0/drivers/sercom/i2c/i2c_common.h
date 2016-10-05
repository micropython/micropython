/**
 * \file
 *
 * \brief SAM SERCOM I2C Common Driver
 *
 * Copyright (c) 2012-2016 Atmel Corporation. All rights reserved.
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
#ifndef I2C_COMMON_H_INCLUDED
#define I2C_COMMON_H_INCLUDED

#include <compiler.h>
#include <sercom.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \if (I2C_MASTER_MODE && I2C_SLAVE_MODE)
 *   \defgroup asfdoc_sam0_sercom_i2c_group SAM I2C (SERCOM I2C) Driver
 * \elseif I2C_MASTER_MODE
 *   \defgroup asfdoc_sam0_sercom_i2c_group SAM I2C Master Mode (SERCOM I2C) Driver
 * \elseif I2C_SLAVE_MODE
 *   \defgroup asfdoc_sam0_sercom_i2c_group SAM I2C Slave Mode (SERCOM I2C) Driver
 * \endif
 *
 * This driver for Atmel&reg; | SMART ARM&reg;-based microcontrollers provides
 * an interface for the configuration and management of the device's SERCOM
 * I<SUP>2</SUP>C module, for the transfer of data via an I<SUP>2</SUP>C bus.
 * The following driver API modes are covered by this manual:
 *
 * \if I2C_MASTER_MODE
 * - Master Mode Polled APIs
 * \endif
 * \if I2C_MASTER_CALLBACK_MODE
 * - Master Mode Callback APIs
 * \endif
 * \if I2C_SLAVE_MODE
 * - Slave Mode Polled APIs
 * \endif
 * \if I2C_SLAVE_CALLBACK_MODE
 * - Slave Mode Callback APIs
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
 *
 * The outline of this documentation is as follows:
 * - \ref asfdoc_sam0_sercom_i2c_prerequisites
 * - \ref asfdoc_sam0_sercom_i2c_overview
 * - \ref asfdoc_sam0_sercom_i2c_special_considerations
 * - \ref asfdoc_sam0_sercom_i2c_extra
 * - \ref asfdoc_sam0_sercom_i2c_examples
 * - \ref asfdoc_sam0_sercom_i2c_api_overview
 *
 * \section asfdoc_sam0_sercom_i2c_prerequisites Prerequisites
 * There are no prerequisites.
 *
 * \section asfdoc_sam0_sercom_i2c_overview Module Overview
 * The outline of this section is as follows:
 * - \ref asfdoc_sam0_sercom_i2c_module_features
 * - \ref asfdoc_sam0_sercom_i2c_functional_desc
 * - \ref asfdoc_sam0_sercom_i2c_bus_topology
 * - \ref asfdoc_sam0_sercom_i2c_transactions
 * - \ref asfdoc_sam0_sercom_i2c_multi_master
 * - \ref asfdoc_sam0_sercom_i2c_bus_states
 * - \ref asfdoc_sam0_sercom_i2c_timeout
 * - \ref asfdoc_sam0_sercom_i2c_sleep_modes
 *
 * \subsection asfdoc_sam0_sercom_i2c_module_features Driver Feature Macro Definition
 * <table>
 *	<tr>
 *		<th>Driver Feature Macro</th>
 *		<th>Supported devices</th>
 *	</tr>
 *	<tr>
 *		<td>FEATURE_I2C_FAST_MODE_PLUS_AND_HIGH_SPEED</td>
 *		<td>SAM D21/R21/D10/D11/L21/L22/DA1/C20/C21</td>
 *	</tr>
 *	<tr>
 *		<td>FEATURE_I2C_10_BIT_ADDRESS</td>
 *		<td>SAM D21/R21/D10/D11/L21/L22/DA1/C20/C21</td>
 *	</tr>
 *	<tr>
 *		<td>FEATURE_I2C_SCL_STRETCH_MODE</td>
 *		<td>SAM D21/R21/D10/D11/L21/L22/DA1/C20/C21</td>
 *	</tr>
 *	<tr>
 *		<td>FEATURE_I2C_SCL_EXTEND_TIMEOUT</td>
 *		<td>SAM D21/R21/D10/D11/L21/L22/DA1/C20/C21</td>
 *	</tr>
 * </table>
 * \note The specific features are only available in the driver when the selected
 * device supports those features. 
 * \note When using the I2C high-speed mode for off-board communication,
 * there are various high frequency interference, which can lead to distortion of the signals 
 * and communication failure. When using Xplained Pro boards in order to test I2C high-speed 
 * communication, the following recommendation should be followed:
 * - Use the SDA-line on PA08 and SCL-line on PA09 for both boards. This will provide stronger
 * pull-ups on both SDA and SCL.
 * - The SCL should not be higher than 1.5MHz.
 *
 * \subsection asfdoc_sam0_sercom_i2c_functional_desc Functional Description
 * The I<SUP>2</SUP>C provides a simple two-wire bidirectional bus consisting of a
 * wired-AND type serial clock line (SCL) and a wired-AND type serial data line
 * (SDA).
 *
 * The I<SUP>2</SUP>C bus provides a simple, but efficient method of interconnecting
 * multiple master and slave devices. An arbitration mechanism is provided for
 * resolving bus ownership between masters, as only one master device may own
 * the bus at any given time. The arbitration mechanism relies on the wired-AND
 * connections to avoid bus drivers short-circuiting.
 *
 * A unique address is assigned to all slave devices connected to the bus. A
 * device can contain both master and slave logic, and can emulate multiple
 * slave devices by responding to more than one address.
 *
 * \subsection asfdoc_sam0_sercom_i2c_bus_topology Bus Topology
 * The I<SUP>2</SUP>C bus topology is illustrated in
 * \ref asfdoc_sam0_sercom_i2c_bus_topology_figure "the figure below". The pull-up
 * resistors (Rs) will provide a high level on the bus lines when none of the
 * I<SUP>2</SUP>C devices are driving the bus. These are optional, and can be
 * replaced with a constant current source.
 *
 * \anchor asfdoc_sam0_sercom_i2c_bus_topology_figure
 * \image html bus_topology.svg "I2C Bus Topology" Width=100%
 *
 * \subsection asfdoc_sam0_sercom_i2c_transactions Transactions
 * The I<SUP>2</SUP>C standard defines three fundamental transaction formats:
 * - Master Write
 *   - The master transmits data packets to the slave after addressing it
 * - Master Read
 *   - The slave transmits data packets to the master after being addressed
 * - Combined Read/Write
 *   - A combined transaction consists of several write and read transactions
 *
 * A data transfer starts with the master issuing a \b Start condition on the
 * bus, followed by the address of the slave together with a bit to indicate
 * whether the master wants to read from or write to the slave.
 * The addressed slave must respond to this by sending an \b ACK back to the
 * master.
 *
 * After this, data packets are sent from the master or slave, according to the
 * read/write bit. Each packet must be acknowledged (ACK) or not
 * acknowledged (NACK) by the receiver.
 *
 * If a slave responds with a NACK, the master must assume that the slave
 * cannot receive any more data and cancel the write operation.
 *
 * The master completes a transaction by issuing a \b Stop condition.
 *
 * A master can issue multiple \b Start conditions during a transaction; this
 * is then called a \b Repeated \b Start condition.
 *
 * \subsubsection asfdoc_sam0_sercom_i2c_address_packets Address Packets
 * The slave address consists of seven bits. The 8<SUP>th</SUP> bit in the transfer
 * determines the data direction (read or write). An address packet always
 * succeeds a \b Start or \b Repeated \b Start condition. The 8<SUP>th</SUP> bit is handled
 * in the driver, and the user will only have to provide the 7-bit address.
 *
 * \subsubsection asfdoc_sam0_sercom_i2c_data_packets Data Packets
 * Data packets are nine bits long, consisting of one 8-bit data byte, and an
 * acknowledgement bit. Data packets follow either an address packet or another
 * data packet on the bus.
 *
 * \subsubsection asfdoc_sam0_sercom_i2c_trans_examples Transaction Examples
 * The gray bits in the following examples are sent from master to slave, and
 * the white bits are sent from slave to master.
 * Example of a read transaction is shown in
 * \ref asfdoc_sam0_sercom_i2c_trans_examples_i2c_read "the figure below". Here, the
 * master first issues a \b Start condition and gets ownership of the bus. An
 * address packet with the direction flag set to read is then sent and
 * acknowledged by the slave. Then the slave sends one data packet which is
 * acknowledged by the master. The slave sends another packet, which is not
 * acknowledged by the master and indicates that the master will terminate the
 * transaction. In the end, the transaction is terminated by the master issuing
 * a \b Stop condition.
 *
 * \anchor asfdoc_sam0_sercom_i2c_trans_examples_i2c_read
 * \image html i2c_read.svg "I2C Packet Read" Width=100%
 *
 * Example of a write transaction is shown in
 * \ref asfdoc_sam0_sercom_i2c_trans_examples_i2c_write "the figure below". Here, the
 * master first issues a \b Start condition and gets ownership of the bus. An
 * address packet with the dir flag set to write is then sent and acknowledged
 * by the slave. Then the master sends two data packets, each acknowledged by
 * the slave. In the end, the transaction is terminated by the master issuing
 * a \b Stop condition.
 *
 * \anchor asfdoc_sam0_sercom_i2c_trans_examples_i2c_write
 * \image html i2c_write.svg "I2C Packet Write" Width=100%
 *
 * \subsubsection asfdoc_sam0_sercom_i2c_packet_timeout Packet Timeout
 * When a master sends an I<SUP>2</SUP>C packet, there is no way of
 * being sure that a slave will acknowledge the packet. To avoid stalling the
 * device forever while waiting for an acknowledge, a user selectable timeout
 * is provided in the \ref i2c_master_config struct which
 * lets the driver exit a read or write operation after the specified time.
 * The function will then return the STATUS_ERR_TIMEOUT flag.
 *
 * This is also the case for the slave when using the functions postfixed
 * \c _wait.
 *
 * The time before the timeout occurs, will be the same as
 * for \ref asfdoc_sam0_sercom_i2c_unknown_bus_timeout "unknown bus state" timeout.
 *
 * \subsubsection asfdoc_sam0_sercom_i2c_repeated_start Repeated Start
 * To issue a \b Repeated \b Start, the functions postfixed \c _no_stop must be
 * used.
 * These functions will not send a \b Stop condition when the transfer is done,
 * thus the next transfer will start with a \b Repeated \b Start. To end the
 * transaction, the functions without the \c _no_stop postfix must be used
 * for the last read/write.
 *
 * \subsection asfdoc_sam0_sercom_i2c_multi_master Multi Master
 * In a multi master environment, arbitration of the bus is important, as only
 * one master can own the bus at any point.
 *
 * \subsubsection asfdoc_sam0_sercom_i2c_arbitration Arbitration
 *
 * \par Clock stretching
 * The serial clock line is always driven by a master device. However, all
 * devices connected to the bus are allowed stretch the low period of the clock
 * to slow down the overall clock frequency or to insert wait states while
 * processing data.
 * Both master and slave can randomly stretch the clock, which will force the
 * other device into a wait-state until the clock line goes high again.
 *
 * \par Arbitration on the data line
 * If two masters start transmitting at the same time, they will both transmit
 * until one master detects that the other master is pulling the data line low.
 * When this is detected, the master not pulling the line low, will stop the
 * transmission and wait until the bus is idle.
 * As it is the master trying to contact the slave with the lowest address that
 * will get the bus ownership, this will create an arbitration scheme always
 * prioritizing the slaves with the lowest address in case of a bus collision.
 *
 * \subsubsection asfdoc_sam0_sercom_i2c_clock_sync Clock Synchronization
 * In situations where more than one master is trying to control the bus clock
 * line at the same time, a clock synchronization algorithm based on the same
 * principles used for clock stretching is necessary.
 *
 *
 * \subsection asfdoc_sam0_sercom_i2c_bus_states Bus States
 * As the I<SUP>2</SUP>C bus is limited to one transaction at the time,
 * a master that wants to perform a bus transaction must wait until the bus is
 * free.
 * Because of this, it is necessary for all masters in a multi-master system to
 * know the current status of the bus to be able to avoid conflicts and to
 * ensure data integrity.
 * \li \b IDLE No activity on the bus (between a \b Stop and a new \b Start
 * condition)
 * \li \b OWNER If the master initiates a transaction successfully
 * \li \b BUSY If another master is driving the bus
 * \li \b UNKNOWN If the master has recently been enabled or connected to
 * the bus. Is forced to \b IDLE after given
 * \ref asfdoc_sam0_sercom_i2c_unknown_bus_timeout "timeout" when
 * the master module is enabled
 *
 * The bus state diagram can be seen in
 * \ref asfdoc_sam0_sercom_i2c_bus_states_figure "the figure below".
 * \li S: Start condition
 * \li P: Stop condition
 * \li Sr: Repeated start condition
 * \anchor asfdoc_sam0_sercom_i2c_bus_states_figure
 * \image html bus_state_diagram.svg "I2C Bus State Diagram" Width=100%
 *
 * \subsection asfdoc_sam0_sercom_i2c_timeout Bus Timing
 * Inactive bus timeout for the master and SDA hold time is configurable in the
 * drivers.
 *
 * \subsubsection asfdoc_sam0_sercom_i2c_unknown_bus_timeout Unknown Bus State Timeout
 * When a master is enabled or connected to the bus, the bus state will be
 * unknown until either a given timeout or a stop command has occurred. The
 * timeout is configurable in the \ref i2c_master_config struct.
 * The timeout time will depend on toolchain and optimization level used, as
 * the timeout is a loop incrementing a value until it reaches the specified
 * timeout value.
 *
 * \subsubsection sda_hold SDA Hold Timeout
 * When using the I<SUP>2</SUP>C in slave mode, it will be important to
 * set a SDA hold time which assures that the master will be able to pick up
 * the bit sent from the slave. The SDA hold time makes sure that this is the
 * case by holding the data line low for a given period after the negative edge
 * on the clock.
 *
 * The SDA hold time is also available for the master driver, but is not a
 * necessity.
 *
 * \subsection asfdoc_sam0_sercom_i2c_sleep_modes Operation in Sleep Modes
 * The I<SUP>2</SUP>C module can operate in all sleep modes by setting
 * the run_in_standby Boolean in the \ref i2c_master_config or
 * \ref i2c_slave_config struct.
 * The operation in slave and master mode is shown in
 * \ref asfdoc_sam0_sercom_i2c_sleep_modes_table "the table below".
 *
 * \anchor asfdoc_sam0_sercom_i2c_sleep_modes_table
 * <table>
 *   <caption>I2C Standby Operations</caption>
 *   <tr>
 *      <th>Run in standby</th>
 *      <th>Slave</th>
 *      <th>Master</th>
 *   </tr>
 *   <tr>
 *      <td>false</td>
 *      <td>Disabled, all reception is dropped</td>
 *      <td>Generic Clock (GCLK) disabled when master is idle</td>
 *   </tr>
 *   <tr>
 *      <td>true</td>
 *      <td>Wake on address match when enabled</td>
 *      <td>GCLK enabled while in sleep modes</td>
 *   </tr>
 * </table>
 *
 *
 * \section asfdoc_sam0_sercom_i2c_special_considerations Special Considerations
 *
 * \if (I2C_MASTER_CALLBACK_MODE || I2C_SLAVE_CALLBACK_MODE)
 *   \subsection asfdoc_sam0_sercom_i2c_common_interrupt Interrupt-driven Operation
 *   While an interrupt-driven operation is in progress, subsequent calls to a
 *   write or read operation will return the STATUS_BUSY flag, indicating that
 *   only one operation is allowed at any given time.
 *
 *   To check if another transmission can be initiated, the user can either call
 *   another transfer operation, or use the
 *   \ref i2c_master_get_job_status/\ref i2c_slave_get_job_status functions
 *   depending on mode.
 *
 *   If the user would like to get callback from operations while using the
 *   interrupt-driven driver, the callback must be registered and then enabled
 *   using the "register_callback" and "enable_callback" functions.
 * \else
 *   There are no special considerations for this driver for the APIs listed in
 *   this document.
 * \endif
 *
 * \section asfdoc_sam0_sercom_i2c_extra Extra Information
 * For extra information, see \ref asfdoc_sam0_sercom_i2c_extra_info_page.
 * This includes:
 *  - \ref asfdoc_sam0_sercom_i2c_acronyms
 *  - \ref asfdoc_sam0_sercom_i2c_extra_dependencies
 *  - \ref asfdoc_sam0_sercom_i2c_extra_errata
 *  - \ref asfdoc_sam0_sercom_i2c_extra_history
 *
 * \section asfdoc_sam0_sercom_i2c_examples Examples
 *
 * For a list of examples related to this driver, see
 * \ref asfdoc_sam0_sercom_i2c_exqsg.
 *
 * \section asfdoc_sam0_sercom_i2c_api_overview API Overview
 * @{
 */

/**
 * \name Driver Feature Definition
 * Define SERCOM I<SUP>2</SUP>C driver features set according to different device family.
 *
 * @{
 */
#if (SAMD21) || (SAMR21) || (SAMD10) || (SAMD11) || (SAML21) || (SAMDA1) || \
	(SAML22) || (SAMC20) || (SAMC21) || (SAMD09) || (SAMR30) || defined(__DOXYGEN__)
/** Fast mode plus and high speed support. */
#  define FEATURE_I2C_FAST_MODE_PLUS_AND_HIGH_SPEED
/** 10-bit address support */
#  define FEATURE_I2C_10_BIT_ADDRESS
/** SCL stretch mode support */
#  define FEATURE_I2C_SCL_STRETCH_MODE
/** SCL extend timeout support */
#  define FEATURE_I2C_SCL_EXTEND_TIMEOUT
#  define FEATURE_I2C_DMA_SUPPORT
#endif
/*@}*/

/** \brief Transfer direction
 *
 * For master: transfer direction or setting direction bit in address.
 * For slave: direction of request from master.
 */
enum i2c_transfer_direction {
	/** Master write operation is in progress */
	I2C_TRANSFER_WRITE = 0,
	/** Master read operation is in progress */
	I2C_TRANSFER_READ  = 1,
};

/** @} */

#ifdef __cplusplus
}
#endif

/**
 * \page asfdoc_sam0_sercom_i2c_extra_info_page  Extra Information for SERCOM I2C Driver
 *
 * \section asfdoc_sam0_sercom_i2c_acronyms Acronyms
 * \ref asfdoc_sam0_sercom_i2c_acronyms_table "Below" is a table listing the acronyms
 * used in this module, along with their intended meanings.
 *
 * \anchor asfdoc_sam0_sercom_i2c_acronyms_table
 * <table>
 *  <caption>Acronyms</caption>
 *	<tr>
 *		<th>Acronym</th>
 *		<th>Description</th>
 *	</tr>
 *	<tr>
 *		<td>SDA</td>
 *		<td>Serial Data Line</td>
 *	</tr>
 *	<tr>
 *		<td>SCL</td>
 *		<td>Serial Clock Line</td>
 *	</tr>
 *	<tr>
 *		<td>SERCOM</td>
 *		<td>Serial Communication Interface</td>
 *	</tr>
 *	<tr>
 *		<td>DMA</td>
 *		<td>Direct Memory Access</td>
 *	</tr>
 * </table>
 *
 * \section asfdoc_sam0_sercom_i2c_extra_dependencies Dependencies
 * The I<SUP>2</SUP>C driver has the following dependencies:
 * \li \ref asfdoc_sam0_system_pinmux_group "System Pin Multiplexer Driver"
 *
 *
 * \section asfdoc_sam0_sercom_i2c_extra_errata Errata
 * There are no errata related to this driver.
 *
 * \section asfdoc_sam0_sercom_i2c_extra_history Module History
 * \ref asfdoc_sam0_sercom_i2c_extra_history_table "Below" is an overview of the
 * module history, detailing enhancements and fixes made to the module since
 * its first release. The current version of this corresponds to the newest
 * version listed in
 * \ref asfdoc_sam0_sercom_i2c_extra_history_table "the table below".
 *
 * \anchor asfdoc_sam0_sercom_i2c_extra_history_table
 * <table>
 *  <caption>Module History</caption>
 *	<tr>
 *		<th>Changelog</th>
 *	</tr>
 *	<tr>
 *		<td>
 *		\li Added 10-bit addressing and high speed support in SAM D21
 *		\li Separate structure i2c_packet into i2c_master_packet and i2c_slave packet
 *		</td>
 *	</tr>
 *	<tr>
 *		<td>
 *		\li Added support for SCL stretch and extended timeout hardware features in SAM D21
 *		\li Added fast mode plus support in SAM D21
 *		</td>
 *	</tr>
 *	<tr>
 *		<td>Fixed incorrect logical mask for determining if a bus error has
 *          occurred in I<SUP>2</SUP>C Slave mode
 *      </td>
 *	</tr>
 *	<tr>
 *		<td>Initial Release</td>
 *	</tr>
 * </table>
 */

/**
 * \page asfdoc_sam0_sercom_i2c_exqsg Examples for SERCOM I2C Driver
 *
 * This is a list of the available Quick Start guides (QSGs) and example
 * applications for \ref asfdoc_sam0_sercom_i2c_group. QSGs are simple examples with
 * step-by-step instructions to configure and use this driver in a selection of
 * use cases. Note that a QSG can be compiled as a standalone application or be
 * added to the user application.  
 *
 * \if I2C_MASTER_MODE
 * - \subpage asfdoc_sam0_sercom_i2c_master_basic_use_case "Quick Start Guide for the I2C Master module - Basic Use Case"
 * \endif
 * \if I2C_MASTER_CALLBACK_MODE
 * - \subpage asfdoc_sam0_sercom_i2c_master_callback_use_case "Quick Start Guide for the I2C Master module - Callback Use Case"
 * - \subpage asfdoc_sam0_sercom_i2c_master_dma_use_case "Quick Start Guide for the I2C Master module - DMA Use Case"
 * \endif
 * \if I2C_SLAVE_MODE
 * - \subpage asfdoc_sam0_sercom_i2c_slave_basic_use_case "Quick Start Guide for the I2C Slave module - Basic Use Case"
 * \endif
 * \if I2C_SLAVE_CALLBACK_MODE
 * - \subpage asfdoc_sam0_sercom_i2c_slave_callback_use_case "Quick Start Guide for the I2C Slave module - Callback Use Case"
 * - \subpage asfdoc_sam0_sercom_i2c_slave_dma_use_case "Quick Start Guide for the I2C Slave module - DMA Use Case"
 * \endif
 *
 * \page asfdoc_sam0_sercom_i2c_document_revision_history Document Revision History
 *
 * \if (I2C_MASTER_MODE || I2C_MASTER_CALLBACK_MODE)
 * <table>
 *	<tr>
 *		<th>Doc. Rev.</th>
 *		<th>Date</th>
 *		<th>Comments</th>
 *	</tr>
 *	<tr>
 *		<td>42117E</td>
 *		<td>12/2015</td>
 *		<td>Added support for SAM L21/L22, SAM DA1, SAM D09, and SAM C21</td>
 *	</tr>
 *	<tr>
 *		<td>42117D</td>
 *		<td>12/2014</td>
 *		<td>Added support for 10-bit addressing and high speed in SAM D21.
 *		    Added support for SAM R21 and SAM D10/D11.</td>
 *	</tr>
 *	<tr>
 *		<td>42117C</td>
 *		<td>01/2014</td>
 *		<td>Added support for SAM D21</td>
 *	</tr>
 *	<tr>
 *		<td>42117B</td>
 *		<td>06/2013</td>
 *		<td>Corrected documentation typos. Updated I<SUP>2</SUP>C Bus State Diagram.</td>
 *	</tr>
 *	<tr>
 *		<td>42117A</td>
 *		<td>06/2013</td>
 *		<td>Initial release</td>
 *	</tr>
 * </table>
 * \else
 * <table>
 *	<tr>
 *		<th>Doc. Rev.</th>
 *		<th>Date</th>
 *		<th>Comments</th>
 *	</tr>
 *	<tr>
 *		<td>42116E</td>
 *		<td>12/2015</td>
 *		<td>Added support for SAM L21/L22, SAM DA1, SAM D09, and SAM C21</td>
 *	</tr>
 *	<tr>
 *		<td>42116D</td>
 *		<td>12/2014</td>
 *		<td>Added support for 10-bit addressing and high speed in SAM D21.
 *		    Added support for SAM R21 and SAM D10/D11.</td>
 *	</tr>
 *	<tr>
 *		<td>42116C</td>
 *		<td>01/2014</td>
 *		<td>Added support for SAM D21</td>
 *	</tr>
 *	<tr>
 *		<td>42116B</td>
 *		<td>06/2013</td>
 *		<td>Corrected documentation typos. Updated I<SUP>2</SUP>C Bus State Diagram.</td>
 *	</tr>
 *	<tr>
 *		<td>42116A</td>
 *		<td>06/2013</td>
 *		<td>Initial release</td>
 *	</tr>
 * </table>
 *\endif
 */

#endif /* I2C_COMMON_H_INCLUDED */
