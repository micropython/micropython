/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include <string.h>

#include "stm32f4xx_hal.h"

#include "mpconfig.h"
#include "misc.h"
#include "systick.h"
#include "pendsv.h"
#include "qstr.h"
#include "misc.h"
#include "nlr.h"
#include "lexer.h"
#include "parse.h"
#include "obj.h"
#include "runtime.h"
#include "stackctrl.h"
#include "gc.h"
#include "gccollect.h"
#include "pybstdio.h"
#include "readline.h"
#include "pyexec.h"
#include "i2c.h"
#include "spi.h"
#include "uart.h"
#include "timer.h"
#include "led.h"
#include "pin.h"
#include "extint.h"
#include "usrsw.h"
#include "usb.h"
#include "rtc.h"
#include "storage.h"
#include "sdcard.h"
#include "ff.h"
#include "rng.h"
#include "accel.h"
#include "servo.h"
#include "dac.h"
#include "modwlan.h"
#include "modsocket.h"
#include "modselect.h"

void SystemClock_Config(void);

int errno;

static FATFS fatfs0;
#if MICROPY_HW_HAS_SDCARD
static FATFS fatfs1;
#endif

void flash_error(int n) {
    for (int i = 0; i < n; i++) {
        led_state(PYB_LED_R1, 1);
        led_state(PYB_LED_R2, 0);
        HAL_Delay(250);
        led_state(PYB_LED_R1, 0);
        led_state(PYB_LED_R2, 1);
        HAL_Delay(250);
    }
    led_state(PYB_LED_R2, 0);
}

void NORETURN __fatal_error(const char *msg) {
    for (volatile uint delay = 0; delay < 10000000; delay++) {
    }
    led_state(1, 1);
    led_state(2, 1);
    led_state(3, 1);
    led_state(4, 1);
    stdout_tx_strn("\nFATAL ERROR:\n", 14);
    stdout_tx_strn(msg, strlen(msg));
    for (uint i = 0;;) {
        led_toggle(((i++) & 3) + 1);
        for (volatile uint delay = 0; delay < 10000000; delay++) {
        }
        if (i >= 16) {
            // to conserve power
            __WFI();
        }
    }
}

void nlr_jump_fail(void *val) {
    printf("FATAL: uncaught exception %p\n", val);
    __fatal_error("");
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    (void)func;
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("");
}
#endif

STATIC mp_obj_t pyb_config_source_dir = MP_OBJ_NULL;
STATIC mp_obj_t pyb_config_main = MP_OBJ_NULL;
STATIC mp_obj_t pyb_config_usb_mode = MP_OBJ_NULL;

STATIC mp_obj_t pyb_source_dir(mp_obj_t source_dir) {
    if (MP_OBJ_IS_STR(source_dir)) {
        pyb_config_source_dir = source_dir;
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_source_dir_obj, pyb_source_dir);

STATIC mp_obj_t pyb_main(mp_obj_t main) {
    if (MP_OBJ_IS_STR(main)) {
        pyb_config_main = main;
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_main_obj, pyb_main);

STATIC mp_obj_t pyb_usb_mode(mp_obj_t usb_mode) {
    if (MP_OBJ_IS_STR(usb_mode)) {
        pyb_config_usb_mode = usb_mode;
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_usb_mode_obj, pyb_usb_mode);

static const char fresh_boot_py[] =
"# boot.py -- run on boot-up\n"
"# can run arbitrary Python, but best to keep it minimal\n"
"\n"
"import pyb\n"
"#pyb.main('main.py') # main script to run after this one\n"
"#pyb.usb_mode('CDC+MSC') # act as a serial and a storage device\n"
"#pyb.usb_mode('CDC+HID') # act as a serial device and a mouse\n"
;

static const char fresh_main_py[] =
"# main.py -- put your code here!\n"
;

static const char fresh_pybcdc_inf[] =
#include "genhdr/pybcdc_inf.h"
;

static const char fresh_readme_txt[] =
"This is a Micro Python board\r\n"
"\r\n"
"You can get started right away by writing your Python code in 'main.py'.\r\n"
"\r\n"
"For a serial prompt:\r\n"
" - Windows: you need to go to 'Device manager', right click on the unknown device,\r\n"
"   then update the driver software, using the 'pybcdc.inf' file found on this drive.\r\n"
"   Then use a terminal program like Hyperterminal or putty.\r\n"
" - Mac OS X: use the command: screen /dev/tty.usbmodem*\r\n"
" - Linux: use the command: screen /dev/ttyACM0\r\n"
"\r\n"
"Please visit http://micropython.org/help/ for further help.\r\n"
;

int main(void) {
    // TODO disable JTAG

    // Stack limit should be less than real stack size, so we
    // had chance to recover from limit hit.
    mp_stack_set_limit(&_ram_end - &_heap_end - 512);

    /* STM32F4xx HAL library initialization:
         - Configure the Flash prefetch, instruction and Data caches
         - Configure the Systick to generate an interrupt each 1 msec
         - Set NVIC Group Priority to 4
         - Global MSP (MCU Support Package) initialization
       */
    HAL_Init();

    // set the system clock to be HSE
    SystemClock_Config();

    // enable GPIO clocks
    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();

    // enable the CCM RAM
    __CCMDATARAMEN_CLK_ENABLE();

#if 0
#if defined(NETDUINO_PLUS_2)
    {
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

#if MICROPY_HW_HAS_SDCARD
        // Turn on the power enable for the sdcard (PB1)
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET);
#endif

        // Turn on the power for the 5V on the expansion header (PB2)
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        GPIO_WriteBit(GPIOB, GPIO_Pin_2, Bit_SET);
    }
#endif
#endif

    // basic sub-system init
    pendsv_init();
    timer_tim3_init();
    led_init();
#if MICROPY_HW_HAS_SWITCH
    switch_init0();
#endif

    int first_soft_reset = true;

soft_reset:

    // check if user switch held to select the reset mode
    led_state(1, 0);
    led_state(2, 1);
    led_state(3, 0);
    led_state(4, 0);
    uint reset_mode = 1;

#if MICROPY_HW_HAS_SWITCH
    if (switch_get()) {
        for (uint i = 0; i < 3000; i++) {
            if (!switch_get()) {
                break;
            }
            HAL_Delay(20);
            if (i % 30 == 29) {
                if (++reset_mode > 3) {
                    reset_mode = 1;
                }
                led_state(2, reset_mode & 1);
                led_state(3, reset_mode & 2);
                led_state(4, reset_mode & 4);
            }
        }
        // flash the selected reset mode
        for (uint i = 0; i < 6; i++) {
            led_state(2, 0);
            led_state(3, 0);
            led_state(4, 0);
            HAL_Delay(50);
            led_state(2, reset_mode & 1);
            led_state(3, reset_mode & 2);
            led_state(4, reset_mode & 4);
            HAL_Delay(50);
        }
        HAL_Delay(400);
    }
#endif

#if MICROPY_HW_ENABLE_RTC
    if (first_soft_reset) {
        rtc_init();
    }
#endif

    // more sub-system init
#if MICROPY_HW_HAS_SDCARD
    if (first_soft_reset) {
        sdcard_init();
    }
#endif
    if (first_soft_reset) {
        storage_init();
    }

    // GC init
    gc_init(&_heap_start, &_heap_end);

    // Change #if 0 to #if 1 if you want REPL on UART_6 (or another uart)
    // as well as on USB VCP
#if 0
    {
        mp_obj_t args[2] = {
            MP_OBJ_NEW_SMALL_INT(PYB_UART_6),
            MP_OBJ_NEW_SMALL_INT(115200),
        };
        pyb_uart_global_debug = pyb_uart_type.make_new((mp_obj_t)&pyb_uart_type,
                                                       MP_ARRAY_SIZE(args),
                                                       0, args);
    }
#else
    pyb_uart_global_debug = NULL;
#endif

    // Micro Python init
    qstr_init();
    mp_init();
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_0_colon__slash_));
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_0_colon__slash_lib));
    mp_obj_list_init(mp_sys_argv, 0);

    readline_init();

    pin_init();
    extint_init();

    // local filesystem init
    {
        // try to mount the flash
        FRESULT res = f_mount(&fatfs0, "0:", 1);
        if (reset_mode == 3 || res == FR_NO_FILESYSTEM) {
            // no filesystem, or asked to reset it, so create a fresh one

            // LED on to indicate creation of LFS
            led_state(PYB_LED_R2, 1);
            uint32_t start_tick = HAL_GetTick();

            res = f_mkfs("0:", 0, 0);
            if (res == FR_OK) {
                // success creating fresh LFS
            } else {
                __fatal_error("could not create LFS");
            }

            // create empty main.py
            FIL fp;
            f_open(&fp, "0:/main.py", FA_WRITE | FA_CREATE_ALWAYS);
            UINT n;
            f_write(&fp, fresh_main_py, sizeof(fresh_main_py) - 1 /* don't count null terminator */, &n);
            // TODO check we could write n bytes
            f_close(&fp);

            // create .inf driver file
            f_open(&fp, "0:/pybcdc.inf", FA_WRITE | FA_CREATE_ALWAYS);
            f_write(&fp, fresh_pybcdc_inf, sizeof(fresh_pybcdc_inf) - 1 /* don't count null terminator */, &n);
            f_close(&fp);

            // create readme file
            f_open(&fp, "0:/README.txt", FA_WRITE | FA_CREATE_ALWAYS);
            f_write(&fp, fresh_readme_txt, sizeof(fresh_readme_txt) - 1 /* don't count null terminator */, &n);
            f_close(&fp);

            // keep LED on for at least 200ms
            sys_tick_wait_at_least(start_tick, 200);
            led_state(PYB_LED_R2, 0);
        } else if (res == FR_OK) {
            // mount sucessful
        } else {
            __fatal_error("could not access LFS");
        }
    }

    // make sure we have a 0:/boot.py
    {
        FILINFO fno;
#if _USE_LFN
        fno.lfname = NULL;
        fno.lfsize = 0;
#endif
        FRESULT res = f_stat("0:/boot.py", &fno);
        if (res == FR_OK) {
            if (fno.fattrib & AM_DIR) {
                // exists as a directory
                // TODO handle this case
                // see http://elm-chan.org/fsw/ff/img/app2.c for a "rm -rf" implementation
            } else {
                // exists as a file, good!
            }
        } else {
            // doesn't exist, create fresh file

            // LED on to indicate creation of boot.py
            led_state(PYB_LED_R2, 1);
            uint32_t start_tick = HAL_GetTick();

            FIL fp;
            f_open(&fp, "0:/boot.py", FA_WRITE | FA_CREATE_ALWAYS);
            UINT n;
            f_write(&fp, fresh_boot_py, sizeof(fresh_boot_py) - 1 /* don't count null terminator */, &n);
            // TODO check we could write n bytes
            f_close(&fp);

            // keep LED on for at least 200ms
            sys_tick_wait_at_least(start_tick, 200);
            led_state(PYB_LED_R2, 0);
        }
    }

    // root device defaults to internal flash filesystem
    uint root_device = 0;

#if defined(USE_DEVICE_MODE)
    usb_storage_medium_t usb_medium = USB_STORAGE_MEDIUM_FLASH;
#endif

#if MICROPY_HW_HAS_SDCARD
    // if an SD card is present then mount it on 1:/
    if (reset_mode == 1 && sdcard_is_present()) {
        FRESULT res = f_mount(&fatfs1, "1:", 1);
        if (res != FR_OK) {
            printf("[SD] could not mount SD card\n");
        } else {
            // use SD card as root device
            root_device = 1;

            if (first_soft_reset) {
                // use SD card as medium for the USB MSD
#if defined(USE_DEVICE_MODE)
                usb_medium = USB_STORAGE_MEDIUM_SDCARD;
#endif
            }
        }
    }
#else
    // Get rid of compiler warning if no SDCARD is configured.
    (void)first_soft_reset;
#endif

    // run <root>:/boot.py, if it exists
    if (reset_mode == 1) {
        const char *boot_file;
        if (root_device == 0) {
            boot_file = "0:/boot.py";
        } else {
            boot_file = "1:/boot.py";
        }
        FRESULT res = f_stat(boot_file, NULL);
        if (res == FR_OK) {
            if (!pyexec_file(boot_file)) {
                flash_error(4);
            }
        }
    }

    // turn boot-up LEDs off
    led_state(2, 0);
    led_state(3, 0);
    led_state(4, 0);

#if defined(USE_HOST_MODE)
    // USB host
    pyb_usb_host_init();
#elif defined(USE_DEVICE_MODE)
    // USB device
    if (reset_mode == 1) {
        usb_device_mode_t usb_mode = USB_DEVICE_MODE_CDC_MSC;
        if (pyb_config_usb_mode != MP_OBJ_NULL) {
            if (strcmp(mp_obj_str_get_str(pyb_config_usb_mode), "CDC+HID") == 0) {
                usb_mode = USB_DEVICE_MODE_CDC_HID;
            }
        }
        pyb_usb_dev_init(usb_mode, usb_medium);
    } else {
        pyb_usb_dev_init(USB_DEVICE_MODE_CDC_MSC, usb_medium);
    }
#endif

    timer_init0();

#if MICROPY_HW_ENABLE_RNG
    rng_init0();
#endif

    i2c_init0();
    spi_init0();

#if MICROPY_HW_HAS_MMA7660
    // MMA accel: init and reset
    accel_init();
#endif

#if MICROPY_HW_ENABLE_SERVO
    // servo
    servo_init();
#endif

#if MICROPY_HW_ENABLE_DAC
    // DAC
    dac_init();
#endif

    // now that everything is initialised, run main script
    if (reset_mode == 1 && pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL) {
        vstr_t *vstr = vstr_new();
        vstr_printf(vstr, "%d:/", root_device);
        if (pyb_config_main == MP_OBJ_NULL) {
            vstr_add_str(vstr, "main.py");
        } else {
            vstr_add_str(vstr, mp_obj_str_get_str(pyb_config_main));
        }
        FRESULT res = f_stat(vstr_str(vstr), NULL);
        if (res == FR_OK) {
            if (!pyexec_file(vstr_str(vstr))) {
                flash_error(3);
            }
        }
        vstr_free(vstr);
    }

#if MICROPY_HW_ENABLE_CC3K
    // wifi using the CC3000 driver
    modwlan_init0();
    modsocket_init0();
    modselect_init0();
#endif

    // enter REPL
    // REPL mode can change, or it can request a soft reset
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

    printf("PYB: sync filesystems\n");
    storage_flush();

    printf("PYB: soft reboot\n");
    timer_deinit();

    first_soft_reset = false;
    goto soft_reset;
}

STATIC NORETURN mp_obj_t mp_sys_exit(uint n_args, const mp_obj_t *args) {
    int rc = 0;
    if (n_args > 0) {
        rc = mp_obj_get_int(args[0]);
    }
    nlr_raise(mp_obj_new_exception_arg1(&mp_type_SystemExit, mp_obj_new_int(rc)));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_sys_exit_obj, 0, 1, mp_sys_exit);
