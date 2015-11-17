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

#include "py/nlr.h"
#include "py/lexer.h"
#include "py/parse.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "py/gc.h"
#include "py/mphal.h"

#include "lib/utils/pyexec.h"
#include "lib/fatfs/ff.h"

#include "systick.h"
#include "pendsv.h"
#include "gccollect.h"
#include "readline.h"
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
#include "rng.h"
#include "accel.h"
#include "servo.h"
#include "dac.h"
#include "can.h"
#include "modnetwork.h"

void SystemClock_Config(void);

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
    mp_hal_stdout_tx_strn("\nFATAL ERROR:\n", 14);
    mp_hal_stdout_tx_strn(msg, strlen(msg));
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

STATIC mp_obj_t pyb_main(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_opt, MP_ARG_INT, {.u_int = 0} }
    };

    if (MP_OBJ_IS_STR(pos_args[0])) {
        MP_STATE_PORT(pyb_config_main) = pos_args[0];

        // parse args
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
        MP_STATE_VM(mp_optimise_value) = args[0].u_int;
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(pyb_main_obj, 1, pyb_main);

static const char fresh_boot_py[] =
"# boot.py -- run on boot-up\r\n"
"# can run arbitrary Python, but best to keep it minimal\r\n"
"\r\n"
"import machine\r\n"
"import pyb\r\n"
"#pyb.main('main.py') # main script to run after this one\r\n"
"#pyb.usb_mode('CDC+MSC') # act as a serial and a storage device\r\n"
"#pyb.usb_mode('CDC+HID') # act as a serial device and a mouse\r\n"
;

static const char fresh_main_py[] =
"# main.py -- put your code here!\r\n"
"while True:\r\n"
"    pass\r\n"
;

static const char fresh_pybcdc_inf[] =
#include "genhdr/pybcdc_inf.h"
;

static const char fresh_readme_txt[] =
"This is a MicroPython board\r\n"
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

// we don't make this function static because it needs a lot of stack and we
// want it to be executed without using stack within main() function
void init_flash_fs(uint reset_mode) {
    // try to mount the flash
    FRESULT res = f_mount(&fatfs0, "/flash", 1);

    if (reset_mode == 3 || res == FR_NO_FILESYSTEM) {
        // no filesystem, or asked to reset it, so create a fresh one

        // LED on to indicate creation of LFS
        led_state(PYB_LED_R2, 1);
        uint32_t start_tick = HAL_GetTick();

        res = f_mkfs("/flash", 0, 0);
        if (res == FR_OK) {
            // success creating fresh LFS
        } else {
            __fatal_error("could not create LFS");
        }

        // set label
        f_setlabel("/flash/pybflash");

        // create empty main.py
        FIL fp;
        f_open(&fp, "/flash/main.py", FA_WRITE | FA_CREATE_ALWAYS);
        UINT n;
        f_write(&fp, fresh_main_py, sizeof(fresh_main_py) - 1 /* don't count null terminator */, &n);
        // TODO check we could write n bytes
        f_close(&fp);

        // create .inf driver file
        f_open(&fp, "/flash/pybcdc.inf", FA_WRITE | FA_CREATE_ALWAYS);
        f_write(&fp, fresh_pybcdc_inf, sizeof(fresh_pybcdc_inf) - 1 /* don't count null terminator */, &n);
        f_close(&fp);

        // create readme file
        f_open(&fp, "/flash/README.txt", FA_WRITE | FA_CREATE_ALWAYS);
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

    // The current directory is used as the boot up directory.
    // It is set to the internal flash filesystem by default.
    f_chdrive("/flash");

    // Make sure we have a /flash/boot.py.  Create it if needed.
    FILINFO fno;
#if _USE_LFN
    fno.lfname = NULL;
    fno.lfsize = 0;
#endif
    res = f_stat("/flash/boot.py", &fno);
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
        f_open(&fp, "/flash/boot.py", FA_WRITE | FA_CREATE_ALWAYS);
        UINT n;
        f_write(&fp, fresh_boot_py, sizeof(fresh_boot_py) - 1 /* don't count null terminator */, &n);
        // TODO check we could write n bytes
        f_close(&fp);

        // keep LED on for at least 200ms
        sys_tick_wait_at_least(start_tick, 200);
        led_state(PYB_LED_R2, 0);
    }
}

STATIC uint update_reset_mode(uint reset_mode) {
#if MICROPY_HW_HAS_SWITCH
    if (switch_get()) {

        // The original method used on the pyboard is appropriate if you have 2
        // or more LEDs.
#if defined(MICROPY_HW_LED2)
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

#elif defined(MICROPY_HW_LED1)

        // For boards with only a single LED, we'll flash that LED the
        // appropriate number of times, with a pause between each one
        for (uint i = 0; i < 10; i++) {
            led_state(1, 0);
            for (uint j = 0; j < reset_mode; j++) {
                if (!switch_get()) {
                    break;
                }
                led_state(1, 1);
                HAL_Delay(100);
                led_state(1, 0);
                HAL_Delay(200);
            }
            HAL_Delay(400);
            if (!switch_get()) {
                break;
            }
            if (++reset_mode > 3) {
                reset_mode = 1;
            }
        }
        // Flash the selected reset mode
        for (uint i = 0; i < 2; i++) {
            for (uint j = 0; j < reset_mode; j++) {
                led_state(1, 1);
                HAL_Delay(100);
                led_state(1, 0);
                HAL_Delay(200);
            }
            HAL_Delay(400);
        }
#else
#error Need a reset mode update method
#endif
    }
#endif
    return reset_mode;
}
#include "irq.h"

// Get the frequency (in Hz) of the source clock for the given timer.
// On STM32F405/407/415/417 there are 2 cases for how the clock freq is set.
// If the APB prescaler is 1, then the timer clock is equal to its respective
// APB clock.  Otherwise (APB prescaler > 1) the timer clock is twice its
// respective APB clock.  See DM00031020 Rev 4, page 115.
static uint32_t my_timer_get_source_freq(uint32_t tim_id) {
    uint32_t source;
    if (tim_id == 1 || (8 <= tim_id && tim_id <= 11)) {
        // TIM{1,8,9,10,11} are on APB2
        source = HAL_RCC_GetPCLK2Freq();
        if ((uint32_t)((RCC->CFGR & RCC_CFGR_PPRE2) >> 3) != RCC_HCLK_DIV1) {
            source *= 2;
        }
    } else {
        // TIM{2,3,4,5,6,7,12,13,14} are on APB1
        source = HAL_RCC_GetPCLK1Freq();
        if ((uint32_t)(RCC->CFGR & RCC_CFGR_PPRE1) != RCC_HCLK_DIV1) {
            source *= 2;
        }
    }
    return source;
}

// TIM3 is set-up for the USB CDC interface
void my_timer_tim3_init(void) {
    // set up the timer for USBD CDC
    __TIM3_CLK_ENABLE();

    TIM3_Handle.Instance = TIM3;
    TIM3_Handle.Init.Period = (USBD_CDC_POLLING_INTERVAL*1000) - 1; // TIM3 fires every USBD_CDC_POLLING_INTERVAL ms
    TIM3_Handle.Init.Prescaler = my_timer_get_source_freq(3) / 1000000 - 1; // TIM3 runs at 1MHz
    TIM3_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    TIM3_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_Base_Init(&TIM3_Handle);

    HAL_NVIC_SetPriority(TIM3_IRQn, IRQ_PRI_TIM3, IRQ_SUBPRI_TIM3);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);

    if (HAL_TIM_Base_Start(&TIM3_Handle) != HAL_OK) {
        /* Starting Error */
    }
}


int main(void) {
    // TODO disable JTAG

    // Stack limit should be less than real stack size, so we have a chance
    // to recover from limit hit.  (Limit is measured in bytes.)
    //mp_stack_set_limit((char*)&_ram_end - (char*)&_heap_end - 1024);

    /* STM32F4xx HAL library initialization:
         - Configure the Flash prefetch, instruction and Data caches
         - Configure the Systick to generate an interrupt each 1 msec
         - Set NVIC Group Priority to 4
         - Global MSP (MCU Support Package) initialization
       */
    HAL_Init();

    // set the system clock to be HSE
    SystemClock_Config();


    my_timer_tim3_init();

    // Initialise low-level sub-systems.  Here we need to very basic things like
    // zeroing out memory and resetting any of the sub-systems.  Following this
    // we can run Python scripts (eg boot.py), but anything that is configurable
    // by boot.py must be set after boot.py is run.


    // init USB device to default setting if it was not already configured
	pyb_usb_dev_init(USBD_VID, USBD_PID_CDC_MSC, USBD_MODE_CDC, NULL);
    while (1)
    {
    	HAL_Delay(250);
    }
}
