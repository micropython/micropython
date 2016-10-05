/**
 * \file
 *
 * \brief Interface of the USB Device Controller (UDC)
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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

#ifndef _UDC_H_
#define _UDC_H_

#include "conf_usb.h"
#include "usb_protocol.h"
#include "udc_desc.h"
#include "udd.h"

#if USB_DEVICE_VENDOR_ID == 0
#   error USB_DEVICE_VENDOR_ID cannot be equal to 0
#endif

#if USB_DEVICE_PRODUCT_ID == 0
#   error USB_DEVICE_PRODUCT_ID cannot be equal to 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup usb_device_group
 * \defgroup udc_group USB Device Controller (UDC)
 *
 * The UDC provides a high-level abstraction of the usb device.
 * You can use these functions to control the main device state
 * (start/attach/wakeup).
 *
 * \section USB_DEVICE_CONF USB Device Custom configuration
 * The following USB Device configuration must be included in the conf_usb.h
 * file of the application.
 *
 * USB_DEVICE_VENDOR_ID (Word)<br>
 * Vendor ID provided by USB org (ATMEL 0x03EB).
 *
 * USB_DEVICE_PRODUCT_ID (Word)<br>
 * Product ID (Referenced in usb_atmel.h).
 *
 * USB_DEVICE_MAJOR_VERSION (Byte)<br>
 * Major version of the device
 *
 * USB_DEVICE_MINOR_VERSION (Byte)<br>
 * Minor version of the device
 *
 * USB_DEVICE_MANUFACTURE_NAME (string)<br>
 * ASCII name for the manufacture
 *
 * USB_DEVICE_PRODUCT_NAME (string)<br>
 * ASCII name for the product
 *
 * USB_DEVICE_SERIAL_NAME (string)<br>
 * ASCII name to enable and set a serial number
 *
 * USB_DEVICE_POWER (Numeric)<br>
 * (unit mA) Maximum device power
 *
 * USB_DEVICE_ATTR (Byte)<br>
 * USB attributes available:
 *  - USB_CONFIG_ATTR_SELF_POWERED
 *  - USB_CONFIG_ATTR_REMOTE_WAKEUP
 *  Note: if remote wake enabled then defines remotewakeup callbacks,
 * see Table 5-2. External API from UDC - Callback
 *
 * USB_DEVICE_LOW_SPEED (Only defined)<br>
 * Force the USB Device to run in low speed
 *
 * USB_DEVICE_HS_SUPPORT (Only defined)<br>
 * Authorize the USB Device to run in high speed
 *
 * USB_DEVICE_MAX_EP (Byte)<br>
 * Define the maximum endpoint number used by the USB Device.<br>
 * This one is already defined in UDI default configuration.
 * Ex:
 * - When endpoint control 0x00, endpoint 0x01 and
 *   endpoint 0x82 is used then USB_DEVICE_MAX_EP=2
 * - When only endpoint control 0x00 is used then USB_DEVICE_MAX_EP=0
 * - When endpoint 0x01 and endpoint 0x81 is used then USB_DEVICE_MAX_EP=1<br>
 *   (configuration not possible on USBB interface)
 * @{
 */

/**
 * \brief Authorizes the VBUS event
 *
 * \return true, if the VBUS monitoring is possible.
 *
 * \section udc_vbus_monitoring VBus monitoring used cases
 *
 * The VBus monitoring is used only for USB SELF Power application.
 *
 * - By default the USB device is automatically attached when Vbus is high
 * or when USB is start for devices without internal Vbus monitoring.
 * conf_usb.h file does not contains define USB_DEVICE_ATTACH_AUTO_DISABLE.
 * \code //#define USB_DEVICE_ATTACH_AUTO_DISABLE \endcode
 *
 * - Add custom VBUS monitoring. conf_usb.h file contains define
 * USB_DEVICE_ATTACH_AUTO_DISABLE:
 * \code #define USB_DEVICE_ATTACH_AUTO_DISABLE \endcode
 * User C file contains:
 * \code  
	// Authorize VBUS monitoring
	if (!udc_include_vbus_monitoring()) {
	  // Implement custom VBUS monitoring via GPIO or other
	}
	Event_VBUS_present() // VBUS interrupt or GPIO interrupt or other
	{
	  // Attach USB Device
	  udc_attach();
	}
\endcode
 *
 * - Case of battery charging. conf_usb.h file contains define
 * USB_DEVICE_ATTACH_AUTO_DISABLE:
 * \code #define USB_DEVICE_ATTACH_AUTO_DISABLE \endcode
 * User C file contains:
 * \code  
	Event VBUS present() // VBUS interrupt or GPIO interrupt or ..
	{
	  // Authorize battery charging, but wait key press to start USB.
	}
	Event Key press()
	{
	  // Stop batteries charging
	  // Start USB
	  udc_attach();
	}
\endcode
 */
static inline bool udc_include_vbus_monitoring(void)
{
	return udd_include_vbus_monitoring();
}

/*! \brief Start the USB Device stack
 */
void udc_start(void);

/*! \brief Stop the USB Device stack
 */
void udc_stop(void);

/**
 * \brief Attach device to the bus when possible
 *
 * \warning If a VBus control is included in driver,
 * then it will attach device when an acceptable Vbus
 * level from the host is detected.
 */
static inline void udc_attach(void)
{
	udd_attach();
}


/**
 * \brief Detaches the device from the bus
 *
 * The driver must remove pull-up on USB line D- or D+.
 */
static inline void udc_detach(void)
{
	udd_detach();
}


/*! \brief The USB driver sends a resume signal called \e "Upstream Resume"
 * This is authorized only when the remote wakeup feature is enabled by host.
 */
static inline void udc_remotewakeup(void)
{
	udd_send_remotewakeup();
}


/**
 * \brief Returns a pointer on the current interface descriptor
 *
 * \return pointer on the current interface descriptor.
 */
usb_iface_desc_t UDC_DESC_STORAGE *udc_get_interface_desc(void);

//@}

/**
 * \ingroup usb_group
 * \defgroup usb_device_group USB Stack Device
 *
 * This module includes USB Stack Device implementation.
 * The stack is divided in three parts:
 * - USB Device Controller (UDC) provides USB chapter 9 compliance
 * - USB Device Interface (UDI) provides USB Class compliance
 * - USB Device Driver (UDD) provides USB Driver for each Atmel MCU

 * Many USB Device applications can be implemented on Atmel MCU.
 * Atmel provides many application notes for different applications:
 * - AVR4900, provides general information about Device Stack
 * - AVR4901, explains how to create a new class
 * - AVR4902, explains how to create a composite device
 * - AVR49xx, all device classes provided in ASF have an application note
 *
 * A basic USB knowledge is required to understand the USB Device
 * Class application notes (HID,MS,CDC,PHDC,...).
 * Then, to create an USB device with
 * only one class provided by ASF, refer directly to the application note
 * corresponding to this USB class. The USB Device application note for
 * New Class and Composite is dedicated to advanced USB users.
 *
 * @{
 */

//! @}

#ifdef __cplusplus
}
#endif

/**
 * \ingroup udc_group
 * \defgroup udc_basic_use_case_setup_prereq USB Device Controller (UDC) - Prerequisites
 * Common prerequisites for all USB devices.
 *
 * This module is based on USB device stack full interrupt driven, and supporting
 * \ref sleepmgr_group sleepmgr. For AVR and SAM3/4 devices the \ref clk_group clock services
 * is supported. For SAMD devices the \ref asfdoc_sam0_system_clock_group clock driver is supported.
 *
 * The following procedure must be executed to setup the project correctly:
 * - Specify the clock configuration:
 *   - XMEGA USB devices need 48MHz clock input.\n
 *     XMEGA USB devices need CPU frequency higher than 12MHz.\n
 *     You can use either an internal RC48MHz auto calibrated by Start of Frames
 *     or an external OSC.
 *   - UC3 and SAM3/4 devices without USB high speed support need 48MHz clock input.\n
 *     You must use a PLL and an external OSC.
 *   - UC3 and SAM3/4 devices with USB high speed support need 12MHz clock input.\n
 *     You must use an external OSC.
 *   - UC3 devices with USBC hardware need CPU frequency higher than 25MHz.
 *   - SAMD devices without USB high speed support need 48MHz clock input.\n
 *     You should use DFLL with USBCRM.
 * - In conf_board.h, the define CONF_BOARD_USB_PORT must be added to enable USB lines.
 * (Not mandatory for all boards)
 * - Enable interrupts
 * - Initialize the clock service
 *
 * The usage of \ref sleepmgr_group sleepmgr service is optional, but recommended to reduce power
 * consumption:
 * - Initialize the sleep manager service
 * - Activate sleep mode when the application is in IDLE state
 *
 * \subpage udc_conf_clock.
 *
 * for AVR and SAM3/4 devices, add to the initialization code:
 * \code
	sysclk_init();
	irq_initialize_vectors();
	cpu_irq_enable();
	board_init();
	sleepmgr_init(); // Optional
\endcode
 *
 * For SAMD devices, add to the initialization code:
 * \code
	system_init();
	irq_initialize_vectors();
	cpu_irq_enable();
	sleepmgr_init(); // Optional
\endcode
 * Add to the main IDLE loop:
 * \code
	sleepmgr_enter_sleep(); // Optional
\endcode
 *
 */

/**
 * \ingroup udc_group
 * \defgroup udc_basic_use_case_setup_code USB Device Controller (UDC) - Example code
 * Common example code for all USB devices.
 *
 * Content of conf_usb.h:
 * \code
	#define USB_DEVICE_VENDOR_ID 0x03EB
	#define USB_DEVICE_PRODUCT_ID 0xXXXX
	#define USB_DEVICE_MAJOR_VERSION 1
	#define USB_DEVICE_MINOR_VERSION 0
	#define USB_DEVICE_POWER 100
	#define USB_DEVICE_ATTR USB_CONFIG_ATTR_BUS_POWERED
\endcode
 *
 * Add to application C-file:
 * \code
	void usb_init(void)
	{
	  udc_start();
	}
\endcode
 */

/**
 * \ingroup udc_group
 * \defgroup udc_basic_use_case_setup_flow USB Device Controller (UDC) - Workflow
 * Common workflow for all USB devices.
 *
 * -# Ensure that conf_usb.h is available and contains the following configuration
 * which is the main USB device configuration:
 *   - \code // Vendor ID provided by USB org (ATMEL 0x03EB)
	#define USB_DEVICE_VENDOR_ID 0x03EB // Type Word
	// Product ID (Atmel PID referenced in usb_atmel.h)
	#define USB_DEVICE_PRODUCT_ID 0xXXXX // Type Word
	// Major version of the device
	#define USB_DEVICE_MAJOR_VERSION 1 // Type Byte
	// Minor version of the device
	#define USB_DEVICE_MINOR_VERSION 0 // Type Byte
	// Maximum device power (mA)
	#define USB_DEVICE_POWER 100 // Type 9-bits
	// USB attributes to enable features
	#define USB_DEVICE_ATTR USB_CONFIG_ATTR_BUS_POWERED // Flags \endcode
 * -# Call the USB device stack start function to enable stack and start USB:
 *   - \code udc_start(); \endcode
 *     \note In case of USB dual roles (Device and Host) managed through USB OTG connector
 * (USB ID pin), the call of udc_start() must be removed and replaced by uhc_start().
 * SeRefer to "AVR4950 section 6.1 Dual roles" for further information about dual roles.
 */

/**
 * \page udc_conf_clock conf_clock.h examples with USB support
 *
 * Content of XMEGA conf_clock.h:
 * \code
	// Configuration based on internal RC:
	// USB clock need of 48Mhz
	#define CONFIG_USBCLK_SOURCE        USBCLK_SRC_RCOSC
	#define CONFIG_OSC_RC32_CAL         48000000UL
	#define CONFIG_OSC_AUTOCAL_RC32MHZ_REF_OSC  OSC_ID_USBSOF
	// CPU clock need of clock > 12MHz to run with USB (Here 24MHz)
	#define CONFIG_SYSCLK_SOURCE     SYSCLK_SRC_RC32MHZ
	#define CONFIG_SYSCLK_PSADIV     SYSCLK_PSADIV_2
	#define CONFIG_SYSCLK_PSBCDIV    SYSCLK_PSBCDIV_1_1
\endcode
 *
 * Content of conf_clock.h for AT32UC3A0, AT32UC3A1, AT32UC3B devices (USBB):
 * \code
	// Configuration based on 12MHz external OSC:
	#define CONFIG_PLL1_SOURCE          PLL_SRC_OSC0
	#define CONFIG_PLL1_MUL             8
	#define CONFIG_PLL1_DIV             2
	#define CONFIG_USBCLK_SOURCE        USBCLK_SRC_PLL1
	#define CONFIG_USBCLK_DIV           1 // Fusb = Fsys/(2 ^ USB_div)
\endcode
 *
 * Content of conf_clock.h for AT32UC3A3, AT32UC3A4 devices (USBB with high speed support):
 * \code
	// Configuration based on 12MHz external OSC:
	#define CONFIG_USBCLK_SOURCE        USBCLK_SRC_OSC0
	#define CONFIG_USBCLK_DIV           1 // Fusb = Fsys/(2 ^ USB_div)
\endcode
 *
 * Content of conf_clock.h for AT32UC3C, ATUCXXD, ATUCXXL3U, ATUCXXL4U devices (USBC):
 * \code
	// Configuration based on 12MHz external OSC:
	#define CONFIG_PLL1_SOURCE          PLL_SRC_OSC0
	#define CONFIG_PLL1_MUL             8
	#define CONFIG_PLL1_DIV             2
	#define CONFIG_USBCLK_SOURCE        USBCLK_SRC_PLL1
	#define CONFIG_USBCLK_DIV           1 // Fusb = Fsys/(2 ^ USB_div)
	// CPU clock need of clock > 25MHz to run with USBC
	#define CONFIG_SYSCLK_SOURCE        SYSCLK_SRC_PLL1
\endcode
 *
 * Content of conf_clock.h for SAM3S, SAM3SD, SAM4S devices (UPD: USB Peripheral Device):
 * \code
	// PLL1 (B) Options   (Fpll = (Fclk * PLL_mul) / PLL_div)
	#define CONFIG_PLL1_SOURCE          PLL_SRC_MAINCK_XTAL
	#define CONFIG_PLL1_MUL             16
	#define CONFIG_PLL1_DIV             2
	// USB Clock Source Options   (Fusb = FpllX / USB_div)
	#define CONFIG_USBCLK_SOURCE        USBCLK_SRC_PLL1
	#define CONFIG_USBCLK_DIV           2
\endcode
 *
 * Content of conf_clock.h for SAM3U device (UPDHS: USB Peripheral Device High Speed):
 * \code
	// USB Clock Source fixed at UPLL.
\endcode
 *
 * Content of conf_clock.h for SAM3X, SAM3A devices (UOTGHS: USB OTG High Speed):
 * \code
	// USB Clock Source fixed at UPLL.
	#define CONFIG_USBCLK_SOURCE        USBCLK_SRC_UPLL
	#define CONFIG_USBCLK_DIV           1
\endcode
 *
 * Content of conf_clocks.h for SAMD devices (USB):
 * \code
	// System clock bus configuration
	#  define CONF_CLOCK_FLASH_WAIT_STATES            2

	// USB Clock Source fixed at DFLL.
	// SYSTEM_CLOCK_SOURCE_DFLL configuration - Digital Frequency Locked Loop
	#  define CONF_CLOCK_DFLL_ENABLE                  true
	#  define CONF_CLOCK_DFLL_LOOP_MODE               SYSTEM_CLOCK_DFLL_LOOP_MODE_USB_RECOVERY
	#  define CONF_CLOCK_DFLL_ON_DEMAND               true

	// Set this to true to configure the GCLK when running clocks_init. 
	// If set to false, none of the GCLK generators will be configured in clocks_init().
	#  define CONF_CLOCK_CONFIGURE_GCLK               true

	// Configure GCLK generator 0 (Main Clock)
	#  define CONF_CLOCK_GCLK_0_ENABLE                true
	#  define CONF_CLOCK_GCLK_0_RUN_IN_STANDBY        true
	#  define CONF_CLOCK_GCLK_0_CLOCK_SOURCE          SYSTEM_CLOCK_SOURCE_DFLL
	#  define CONF_CLOCK_GCLK_0_PRESCALER             1
	#  define CONF_CLOCK_GCLK_0_OUTPUT_ENABLE         false
\endcode
 */

/**
 * \page udc_use_case_1 Change USB speed
 *
 * In this use case, the USB device is used with different USB speeds.
 *
 * \section udc_use_case_1_setup Setup steps
 *
 * Prior to implement this use case, be sure to have already
 * apply the UDI module "basic use case".
 *
 * \section udc_use_case_1_usage Usage steps
 *
 * \subsection udc_use_case_1_usage_code Example code
 * Content of conf_usb.h:
 * \code
	 #if // Low speed
	 #define USB_DEVICE_LOW_SPEED
	 // #define USB_DEVICE_HS_SUPPORT

	 #elif // Full speed
	 // #define USB_DEVICE_LOW_SPEED
	 // #define USB_DEVICE_HS_SUPPORT

	 #elif // High speed
	 // #define USB_DEVICE_LOW_SPEED
	 #define USB_DEVICE_HS_SUPPORT

	 #endif
\endcode
 *
 * \subsection udc_use_case_1_usage_flow Workflow
 * -# Ensure that conf_usb.h is available and contains the following parameters
 * required for a USB device low speed (1.5Mbit/s):
 *   - \code #define USB_DEVICE_LOW_SPEED
	 //#define  USB_DEVICE_HS_SUPPORT \endcode
 * -# Ensure that conf_usb.h contains the following parameters
 * required for a USB device full speed (12Mbit/s):
 *   - \code //#define USB_DEVICE_LOW_SPEED
	 //#define  USB_DEVICE_HS_SUPPORT \endcode
 * -# Ensure that conf_usb.h contains the following parameters
 * required for a USB device high speed (480Mbit/s):
 *   - \code //#define USB_DEVICE_LOW_SPEED
	 #define  USB_DEVICE_HS_SUPPORT \endcode
 */

/**
 * \page udc_use_case_2 Use USB strings
 *
 * In this use case, the usual USB strings is added in the USB device.
 *
 * \section udc_use_case_2_setup Setup steps
 * Prior to implement this use case, be sure to have already
 * apply the UDI module "basic use case".
 *
 * \section udc_use_case_2_usage Usage steps
 *
 * \subsection udc_use_case_2_usage_code Example code
 * Content of conf_usb.h:
 * \code
	#define  USB_DEVICE_MANUFACTURE_NAME      "Manufacture name"
	#define  USB_DEVICE_PRODUCT_NAME          "Product name"
	#define  USB_DEVICE_SERIAL_NAME           "12...EF"
\endcode
 *
 * \subsection udc_use_case_2_usage_flow Workflow
 * -# Ensure that conf_usb.h is available and contains the following parameters
 * required to enable different USB strings:
 *   - \code // Static ASCII name for the manufacture
	#define  USB_DEVICE_MANUFACTURE_NAME "Manufacture name" \endcode
 *   - \code // Static ASCII name for the product
	#define  USB_DEVICE_PRODUCT_NAME "Product name" \endcode
 *   - \code // Static ASCII name to enable and set a serial number
	#define  USB_DEVICE_SERIAL_NAME "12...EF" \endcode
 */

/**
 * \page udc_use_case_3 Use USB remote wakeup feature
 *
 * In this use case, the USB remote wakeup feature is enabled.
 *
 * \section udc_use_case_3_setup Setup steps
 * Prior to implement this use case, be sure to have already
 * apply the UDI module "basic use case".
 *
 * \section udc_use_case_3_usage Usage steps
 *
 * \subsection udc_use_case_3_usage_code Example code
 * Content of conf_usb.h:
 * \code
	#define  USB_DEVICE_ATTR \
	  (USB_CONFIG_ATTR_REMOTE_WAKEUP | USB_CONFIG_ATTR_..._POWERED)
	#define UDC_REMOTEWAKEUP_ENABLE() my_callback_remotewakeup_enable()
	extern void my_callback_remotewakeup_enable(void);
	#define UDC_REMOTEWAKEUP_DISABLE() my_callback_remotewakeup_disable()
	extern void my_callback_remotewakeup_disable(void);
\endcode
 *
 * Add to application C-file:
 * \code
	 void my_callback_remotewakeup_enable(void)
	 {
	    // Enable application wakeup events (e.g. enable GPIO interrupt)
	 }
	 void my_callback_remotewakeup_disable(void)
	 {
	    // Disable application wakeup events (e.g. disable GPIO interrupt)
	 }

	 void my_interrupt_event(void)
	 {
	    udc_remotewakeup();
	 }
\endcode
 *
 * \subsection udc_use_case_3_usage_flow Workflow
 * -# Ensure that conf_usb.h is available and contains the following parameters
 * required to enable remote wakeup feature:
 *   - \code // Authorizes the remote wakeup feature
	     #define  USB_DEVICE_ATTR (USB_CONFIG_ATTR_REMOTE_WAKEUP | USB_CONFIG_ATTR_..._POWERED) \endcode
 *   - \code // Define callback called when the host enables the remotewakeup feature
	#define UDC_REMOTEWAKEUP_ENABLE() my_callback_remotewakeup_enable()
	extern void my_callback_remotewakeup_enable(void); \endcode
 *   - \code // Define callback called when the host disables the remotewakeup feature
	#define UDC_REMOTEWAKEUP_DISABLE() my_callback_remotewakeup_disable()
	extern void my_callback_remotewakeup_disable(void); \endcode
 * -# Send a remote wakeup (USB upstream):
 *   - \code udc_remotewakeup(); \endcode
 */

/**
 * \page udc_use_case_5 Bus power application recommendations
 *
 * In this use case, the USB device BUS power feature is enabled.
 * This feature requires a correct power consumption management.
 *
 * \section udc_use_case_5_setup Setup steps
 * Prior to implement this use case, be sure to have already
 * apply the UDI module "basic use case".
 *
 * \section udc_use_case_5_usage Usage steps
 *
 * \subsection udc_use_case_5_usage_code Example code
 * Content of conf_usb.h:
 * \code
	#define  USB_DEVICE_ATTR (USB_CONFIG_ATTR_BUS_POWERED)
	#define  UDC_SUSPEND_EVENT()         user_callback_suspend_action()
	extern void user_callback_suspend_action(void)
	#define  UDC_RESUME_EVENT()          user_callback_resume_action()
	extern void user_callback_resume_action(void)
\endcode
 *
 * Add to application C-file:
 * \code
	void user_callback_suspend_action(void)
	{
	   // Disable hardware component to reduce power consumption
	}
	void user_callback_resume_action(void)
	{
	   // Re-enable hardware component
	}
\endcode
 *
 * \subsection udc_use_case_5_usage_flow Workflow
 * -# Ensure that conf_usb.h is available and contains the following parameters:
 *   - \code // Authorizes the BUS power feature
	#define  USB_DEVICE_ATTR (USB_CONFIG_ATTR_BUS_POWERED) \endcode
 *   - \code // Define callback called when the host suspend the USB line
	#define UDC_SUSPEND_EVENT() user_callback_suspend_action()
	extern void user_callback_suspend_action(void); \endcode
 *   - \code // Define callback called when the host or device resume the USB line
	#define UDC_RESUME_EVENT() user_callback_resume_action()
	extern void user_callback_resume_action(void); \endcode
 * -# Reduce power consumption in suspend mode (max. 2.5mA on Vbus):
 *   - \code void user_callback_suspend_action(void)
	{
	turn_off_components();
	} \endcode
 */

/**
 * \page udc_use_case_6 USB dynamic serial number
 *
 * In this use case, the USB serial strings is dynamic.
 * For a static serial string refer to \ref udc_use_case_2.
 *
 * \section udc_use_case_6_setup Setup steps
 * Prior to implement this use case, be sure to have already
 * apply the UDI module "basic use case".
 *
 * \section udc_use_case_6_usage Usage steps
 *
 * \subsection udc_use_case_6_usage_code Example code
 * Content of conf_usb.h:
 * \code
	#define  USB_DEVICE_SERIAL_NAME
	#define  USB_DEVICE_GET_SERIAL_NAME_POINTER serial_number
	#define  USB_DEVICE_GET_SERIAL_NAME_LENGTH  12
	extern uint8_t serial_number[];
\endcode
 *
 * Add to application C-file:
 * \code
	 uint8_t serial_number[USB_DEVICE_GET_SERIAL_NAME_LENGTH];

	 void init_build_usb_serial_number(void)
	 {
	 serial_number[0] = 'A';
	 serial_number[1] = 'B';
	 ...
	 serial_number[USB_DEVICE_GET_SERIAL_NAME_LENGTH-1] = 'C';
	 } \endcode
 *
 * \subsection udc_use_case_6_usage_flow Workflow
 * -# Ensure that conf_usb.h is available and contains the following parameters
 * required to enable a USB serial number strings dynamically:
 *   - \code #define  USB_DEVICE_SERIAL_NAME // Define this empty
	#define  USB_DEVICE_GET_SERIAL_NAME_POINTER serial_number // Give serial array pointer
	#define  USB_DEVICE_GET_SERIAL_NAME_LENGTH  12 // Give size of serial array
	extern uint8_t serial_number[]; // Declare external serial array \endcode
 * -# Before start USB stack, initialize the serial array
 *   - \code
	 uint8_t serial_number[USB_DEVICE_GET_SERIAL_NAME_LENGTH];

	 void init_build_usb_serial_number(void)
	 {
	 serial_number[0] = 'A';
	 serial_number[1] = 'B';
	 ...
	 serial_number[USB_DEVICE_GET_SERIAL_NAME_LENGTH-1] = 'C';
	 } \endcode
 */



#endif // _UDC_H_
