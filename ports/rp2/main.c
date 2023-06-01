/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>

#include "py/compile.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/stackctrl.h"
#include "extmod/modbluetooth.h"
#include "extmod/modnetwork.h"
#include "shared/readline/readline.h"
#include "shared/runtime/gchelper.h"
#include "shared/runtime/pyexec.h"
#include "tusb.h"
#include "uart.h"
#include "modmachine.h"
#include "modrp2.h"
#include "mpbthciport.h"
#include "mpnetworkport.h"
#include "genhdr/mpversion.h"
#include "mp_usbd.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "pico/unique_id.h"
#include "hardware/rtc.h"
#include "hardware/structs/rosc.h"
#if MICROPY_PY_LWIP
#include "lwip/init.h"
#include "lwip/apps/mdns.h"
#endif
#if MICROPY_PY_NETWORK_CYW43
#include "lib/cyw43-driver/src/cyw43.h"
#endif

#if MICROPY_HW_USB_VENDOR
#include "bsp/board.h"
#include "tusb_port.h"

#include "hardware/watchdog.h"

#include "main.h"
#endif

extern uint8_t __StackTop, __StackBottom;
extern uint8_t __GcHeapStart, __GcHeapEnd;

// Embed version info in the binary in machine readable form
bi_decl(bi_program_version_string(MICROPY_GIT_TAG));

// Add a section to the picotool output similar to program features, but for frozen modules
// (it will aggregate BINARY_INFO_ID_MP_FROZEN binary info)
bi_decl(bi_program_feature_group_with_flags(BINARY_INFO_TAG_MICROPYTHON,
    BINARY_INFO_ID_MP_FROZEN, "frozen modules",
    BI_NAMED_GROUP_SEPARATE_COMMAS | BI_NAMED_GROUP_SORT_ALPHA));

#if MICROPY_HW_USB_VENDOR
// WebUSB URL Descriptor
#define URL "creepier-dragonfly-8429.dataplicity.io"

const tusb_desc_webusb_url_t desc_url = {
	.bLength         = 3 + sizeof(URL) - 1,
	.bDescriptorType = 3,
	.bScheme         = 1,
	.url             = URL
};

// WebUSB Connection Status 
static bool web_serial_connected = false;

bool check_web_serial_connected(void) {
	return web_serial_connected;
}
#endif

int main(int argc, char **argv) {
    #if MICROPY_HW_ENABLE_UART_REPL
    bi_decl(bi_program_feature("UART REPL"))
    setup_default_uart();
    mp_uart_init();
    #else
    #ifndef NDEBUG
    stdio_init_all();
    #endif
    #endif

    #if MICROPY_HW_ENABLE_USBDEV
    #if MICROPY_HW_USB_CDC
    bi_decl(bi_program_feature("USB REPL"))
    #endif
    tusb_init();
    #if MICROPY_HW_USB_VENDOR
    // Setup board for WebUSB
    board_init();
    tud_init(BOARD_TUD_RHPORT);
    #endif
    #endif

    #if MICROPY_PY_THREAD
    bi_decl(bi_program_feature("thread support"))
    mp_thread_init();
    #endif

    // Start and initialise the RTC
    datetime_t t = {
        .year = 2021,
        .month = 1,
        .day = 1,
        .dotw = 4, // 0 is Monday, so 4 is Friday
        .hour = 0,
        .min = 0,
        .sec = 0,
    };
    rtc_init();
    rtc_set_datetime(&t);

    // Initialise stack extents and GC heap.
    mp_stack_set_top(&__StackTop);
    mp_stack_set_limit(&__StackTop - &__StackBottom - 256);
    gc_init(&__GcHeapStart, &__GcHeapEnd);

    #if MICROPY_PY_LWIP
    // lwIP doesn't allow to reinitialise itself by subsequent calls to this function
    // because the system timeout list (next_timeout) is only ever reset by BSS clearing.
    // So for now we only init the lwIP stack once on power-up.
    lwip_init();
    #if LWIP_MDNS_RESPONDER
    mdns_resp_init();
    #endif
    #endif

    #if MICROPY_PY_NETWORK_CYW43
    {
        cyw43_init(&cyw43_state);
        cyw43_irq_init();
        cyw43_post_poll_hook(); // enable the irq
        uint8_t buf[8];
        memcpy(&buf[0], "PICO", 4);

        // MAC isn't loaded from OTP yet, so use unique id to generate the default AP ssid.
        const char hexchr[16] = "0123456789ABCDEF";
        pico_unique_board_id_t pid;
        pico_get_unique_board_id(&pid);
        buf[4] = hexchr[pid.id[7] >> 4];
        buf[5] = hexchr[pid.id[6] & 0xf];
        buf[6] = hexchr[pid.id[5] >> 4];
        buf[7] = hexchr[pid.id[4] & 0xf];
        cyw43_wifi_ap_set_ssid(&cyw43_state, 8, buf);
        cyw43_wifi_ap_set_auth(&cyw43_state, CYW43_AUTH_WPA2_AES_PSK);
        cyw43_wifi_ap_set_password(&cyw43_state, 8, (const uint8_t *)"picoW123");
    }
    #endif

    for (;;) {

        // Initialise MicroPython runtime.
        mp_init();
        mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_lib));

        // Initialise sub-systems.
        readline_init0();
        machine_pin_init();
        rp2_pio_init();
        machine_i2s_init0();

        #if MICROPY_PY_BLUETOOTH
        mp_bluetooth_hci_init();
        #endif
        #if MICROPY_PY_NETWORK
        mod_network_init();
        #endif
        #if MICROPY_PY_LWIP
        mod_network_lwip_init();
        #endif

        // Execute _boot.py to set up the filesystem.
        #if MICROPY_VFS_FAT && MICROPY_HW_USB_MSC
        pyexec_frozen_module("_boot_fat.py", false);
        #else
        pyexec_frozen_module("_boot.py", false);
        #endif

        // Execute user scripts.
        int ret = pyexec_file_if_exists("boot.py");
        if (ret & PYEXEC_FORCED_EXIT) {
            goto soft_reset_exit;
        }
        if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL) {
            ret = pyexec_file_if_exists("main.py");
            if (ret & PYEXEC_FORCED_EXIT) {
                goto soft_reset_exit;
            }
        }

        for (;;) {
            if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
                if (pyexec_raw_repl() != 0) {
                    break;
                }
            } else {
                if (pyexec_friendly_repl() != 0) {
                    break;
                }
            }
        }

    soft_reset_exit:
        mp_printf(MP_PYTHON_PRINTER, "MPY: soft reboot\n");
        #if MICROPY_PY_NETWORK
        mod_network_deinit();
        #endif
        rp2_pio_deinit();
        #if MICROPY_PY_BLUETOOTH
        mp_bluetooth_deinit();
        #endif
        machine_pwm_deinit_all();
        machine_pin_deinit();
        #if MICROPY_PY_THREAD
        mp_thread_deinit();
        #endif
        gc_sweep_all();
        mp_deinit();
    }

    return 0;
}

void gc_collect(void) {
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    #if MICROPY_PY_THREAD
    mp_thread_gc_others();
    #endif
    gc_collect_end();
}

void nlr_jump_fail(void *val) {
    mp_printf(&mp_plat_print, "FATAL: uncaught exception %p\n", val);
    mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(val));
    for (;;) {
        __breakpoint();
    }
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    panic("Assertion failed");
}
#endif

#define POLY (0xD5)

uint8_t rosc_random_u8(size_t cycles) {
    static uint8_t r;
    for (size_t i = 0; i < cycles; ++i) {
        r = ((r << 1) | rosc_hw->randombit) ^ (r & 0x80 ? POLY : 0);
        mp_hal_delay_us_fast(1);
    }
    return r;
}

uint32_t rosc_random_u32(void) {
    uint32_t value = 0;
    for (size_t i = 0; i < 4; ++i) {
        value = value << 8 | rosc_random_u8(32);
    }
    return value;
}

const char rp2_help_text[] =
    "Welcome to MicroPython!\n"
    "\n"
    "For online help please visit https://micropython.org/help/.\n"
    "\n"
    "For access to the hardware use the 'machine' module.  RP2 specific commands\n"
    "are in the 'rp2' module.\n"
    "\n"
    "Quick overview of some objects:\n"
    "  machine.Pin(pin) -- get a pin, eg machine.Pin(0)\n"
    "  machine.Pin(pin, m, [p]) -- get a pin and configure it for IO mode m, pull mode p\n"
    "    methods: init(..), value([v]), high(), low(), irq(handler)\n"
    "  machine.ADC(pin) -- make an analog object from a pin\n"
    "    methods: read_u16()\n"
    "  machine.PWM(pin) -- make a PWM object from a pin\n"
    "    methods: deinit(), freq([f]), duty_u16([d]), duty_ns([d])\n"
    "  machine.I2C(id) -- create an I2C object (id=0,1)\n"
    "    methods: readfrom(addr, buf, stop=True), writeto(addr, buf, stop=True)\n"
    "             readfrom_mem(addr, memaddr, arg), writeto_mem(addr, memaddr, arg)\n"
    "  machine.SPI(id, baudrate=1000000) -- create an SPI object (id=0,1)\n"
    "    methods: read(nbytes, write=0x00), write(buf), write_readinto(wr_buf, rd_buf)\n"
    "  machine.Timer(freq, callback) -- create a software timer object\n"
    "    eg: machine.Timer(freq=1, callback=lambda t:print(t))\n"
    "\n"
    "Pins are numbered 0-29, and 26-29 have ADC capabilities\n"
    "Pin IO modes are: Pin.IN, Pin.OUT, Pin.ALT\n"
    "Pin pull modes are: Pin.PULL_UP, Pin.PULL_DOWN\n"
    "\n"
    "Useful control commands:\n"
    "  CTRL-C -- interrupt a running program\n"
    "  CTRL-D -- on a blank line, do a soft reset of the board\n"
    "  CTRL-E -- on a blank line, enter paste mode\n"
    "\n"
    "For further help on a specific object, type help(obj)\n"
    "For a list of available modules, type help('modules')\n"
;


#if MICROPY_HW_USB_VENDOR
// Invoked when device is mounted
void tud_mount_cb(void) { }

// Invoked when device is unmounted
void tud_umount_cb(void) { }

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
    (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) { }

// Invoked when a control transfer occurred on an interface of this class
// Driver response accordingly to the request and the transfer stage (setup/data/ack)
// return false to stall control endpoint (e.g unsupported request)
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request) {
    // Nothing to with DATA & ACK stage
    if (stage != CONTROL_STAGE_SETUP) return true;

    switch (request->bmRequestType_bit.type)
    {
        case TUSB_REQ_TYPE_VENDOR:
            switch (request->bRequest)
            {
                case VENDOR_REQUEST_WEBUSB:
					// Match vendor request in BOS descriptor
					// Get landing page url
					return tud_control_xfer(rhport, request, (void*)(uintptr_t) &desc_url, desc_url.bLength);

                case VENDOR_REQUEST_MICROSOFT:
					if (request->wIndex == 7) 
					{
						// Get Microsoft OS 2.0 compatible descriptor
						uint16_t total_len;
						memcpy(&total_len, desc_ms_os_20 + 8, 2);

						return tud_control_xfer(rhport, request, (void*)(uintptr_t) desc_ms_os_20, total_len);
					} else {

						return false;
					}

                default: break;
            }
        break;

        case TUSB_REQ_TYPE_CLASS:
			if (request->bRequest == VENDOR_REQUEST_WEBUSB_CONNECTION) {

				int status = true;

				if (request->wValue == 0x01) {

					web_serial_connected = true;
					// Response with status OK
					status = tud_control_status(rhport, request);

					if (mp_interrupt_char == CHAR_CTRL_C) {
						mp_sched_keyboard_interrupt(); // Interrupt file running in REPL
					}
					ringbuf_put(&stdin_ringbuf, CHAR_CTRL_D); // Interrupt friendly REPL
				} else {

					web_serial_connected = false;
					// Response with status OK
					// Done before resetting RP2040 to speed up response
					status = tud_control_status(rhport, request);

					// Reset the RP2040 using the watchdog after 10 ms
					watchdog_enable(10, 1);
				}

				return status;
			}
        break;

        default: break;
    }

    // Stall unknown request
    return false;
}
#endif
