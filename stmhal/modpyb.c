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

#include <stdint.h>
#include <stdio.h>

#include STM32_HAL_H

#include "py/mpstate.h"
#include "py/nlr.h"
#include "py/obj.h"
#include "py/gc.h"
#include "lib/fatfs/ff.h"
#include "lib/fatfs/diskio.h"
#include "gccollect.h"
#include "irq.h"
#include "systick.h"
#include "pyexec.h"
#include "led.h"
#include "pin.h"
#include "timer.h"
#include "extint.h"
#include "usrsw.h"
#include "rng.h"
#include "rtc.h"
#include "i2c.h"
#include "spi.h"
#include "uart.h"
#include "can.h"
#include "adc.h"
#include "storage.h"
#include "sdcard.h"
#include "accel.h"
#include "servo.h"
#include "dac.h"
#include "lcd.h"
#include "usb.h"
#include "fsusermount.h"
#include "portmodules.h"

/// \module pyb - functions related to the pyboard
///
/// The `pyb` module contains specific functions related to the pyboard.

/// \function bootloader()
/// Activate the bootloader without BOOT* pins.
STATIC NORETURN mp_obj_t pyb_bootloader(void) {
    pyb_usb_dev_deinit();
    storage_flush();

    HAL_RCC_DeInit();
    HAL_DeInit();

#if defined(MCU_SERIES_F7)
    // arm-none-eabi-gcc 4.9.0 does not correctly inline this
    // MSP function, so we write it out explicitly here.
    //__set_MSP(*((uint32_t*) 0x1FF00000));
    __ASM volatile ("movw r3, #0x0000\nmovt r3, #0x1FF0\nldr r3, [r3, #0]\nMSR msp, r3\n" : : : "r3", "sp");

    ((void (*)(void)) *((uint32_t*) 0x1FF00004))();
#else
    __HAL_REMAPMEMORY_SYSTEMFLASH();

    // arm-none-eabi-gcc 4.9.0 does not correctly inline this
    // MSP function, so we write it out explicitly here.
    //__set_MSP(*((uint32_t*) 0x00000000));
    __ASM volatile ("movs r3, #0\nldr r3, [r3, #0]\nMSR msp, r3\n" : : : "r3", "sp");

    ((void (*)(void)) *((uint32_t*) 0x00000004))();
#endif

    while (1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_bootloader_obj, pyb_bootloader);

/// \function hard_reset()
/// Resets the pyboard in a manner similar to pushing the external RESET
/// button.
STATIC mp_obj_t pyb_hard_reset(void) {
    NVIC_SystemReset();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_hard_reset_obj, pyb_hard_reset);

/// \function info([dump_alloc_table])
/// Print out lots of information about the board.
STATIC mp_obj_t pyb_info(mp_uint_t n_args, const mp_obj_t *args) {
    // get and print unique id; 96 bits
    {
        byte *id = (byte*)0x1fff7a10;
        printf("ID=%02x%02x%02x%02x:%02x%02x%02x%02x:%02x%02x%02x%02x\n", id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7], id[8], id[9], id[10], id[11]);
    }

    // get and print clock speeds
    // SYSCLK=168MHz, HCLK=168MHz, PCLK1=42MHz, PCLK2=84MHz
    {
        printf("S=%lu\nH=%lu\nP1=%lu\nP2=%lu\n",
               HAL_RCC_GetSysClockFreq(),
               HAL_RCC_GetHCLKFreq(),
               HAL_RCC_GetPCLK1Freq(),
               HAL_RCC_GetPCLK2Freq());
    }

    // to print info about memory
    {
        printf("_etext=%p\n", &_etext);
        printf("_sidata=%p\n", &_sidata);
        printf("_sdata=%p\n", &_sdata);
        printf("_edata=%p\n", &_edata);
        printf("_sbss=%p\n", &_sbss);
        printf("_ebss=%p\n", &_ebss);
        printf("_estack=%p\n", &_estack);
        printf("_ram_start=%p\n", &_ram_start);
        printf("_heap_start=%p\n", &_heap_start);
        printf("_heap_end=%p\n", &_heap_end);
        printf("_ram_end=%p\n", &_ram_end);
    }

    // qstr info
    {
        mp_uint_t n_pool, n_qstr, n_str_data_bytes, n_total_bytes;
        qstr_pool_info(&n_pool, &n_qstr, &n_str_data_bytes, &n_total_bytes);
        printf("qstr:\n  n_pool=" UINT_FMT "\n  n_qstr=" UINT_FMT "\n  n_str_data_bytes=" UINT_FMT "\n  n_total_bytes=" UINT_FMT "\n", n_pool, n_qstr, n_str_data_bytes, n_total_bytes);
    }

    // GC info
    {
        gc_info_t info;
        gc_info(&info);
        printf("GC:\n");
        printf("  " UINT_FMT " total\n", info.total);
        printf("  " UINT_FMT " : " UINT_FMT "\n", info.used, info.free);
        printf("  1=" UINT_FMT " 2=" UINT_FMT " m=" UINT_FMT "\n", info.num_1block, info.num_2block, info.max_block);
    }

    // free space on flash
    {
        DWORD nclst;
        FATFS *fatfs;
        f_getfree("/flash", &nclst, &fatfs);
        printf("LFS free: %u bytes\n", (uint)(nclst * fatfs->csize * 512));
    }

    if (n_args == 1) {
        // arg given means dump gc allocation table
        gc_dump_alloc_table();
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_info_obj, 0, 1, pyb_info);

/// \function unique_id()
/// Returns a string of 12 bytes (96 bits), which is the unique ID for the MCU.
STATIC mp_obj_t pyb_unique_id(void) {
    byte *id = (byte*)0x1fff7a10;
    return mp_obj_new_bytes(id, 12);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_unique_id_obj, pyb_unique_id);

// get or set the MCU frequencies
STATIC mp_uint_t pyb_freq_calc_ahb_div(mp_uint_t wanted_div) {
    if (wanted_div <= 1) { return RCC_SYSCLK_DIV1; }
    else if (wanted_div <= 2) { return RCC_SYSCLK_DIV2; }
    else if (wanted_div <= 4) { return RCC_SYSCLK_DIV4; }
    else if (wanted_div <= 8) { return RCC_SYSCLK_DIV8; }
    else if (wanted_div <= 16) { return RCC_SYSCLK_DIV16; }
    else if (wanted_div <= 64) { return RCC_SYSCLK_DIV64; }
    else if (wanted_div <= 128) { return RCC_SYSCLK_DIV128; }
    else if (wanted_div <= 256) { return RCC_SYSCLK_DIV256; }
    else { return RCC_SYSCLK_DIV512; }
}
STATIC mp_uint_t pyb_freq_calc_apb_div(mp_uint_t wanted_div) {
    if (wanted_div <= 1) { return RCC_HCLK_DIV1; }
    else if (wanted_div <= 2) { return RCC_HCLK_DIV2; }
    else if (wanted_div <= 4) { return RCC_HCLK_DIV4; }
    else if (wanted_div <= 8) { return RCC_HCLK_DIV8; }
    else { return RCC_SYSCLK_DIV16; }
}
STATIC mp_obj_t pyb_freq(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        // get
        mp_obj_t tuple[4] = {
           mp_obj_new_int(HAL_RCC_GetSysClockFreq()),
           mp_obj_new_int(HAL_RCC_GetHCLKFreq()),
           mp_obj_new_int(HAL_RCC_GetPCLK1Freq()),
           mp_obj_new_int(HAL_RCC_GetPCLK2Freq()),
        };
        return mp_obj_new_tuple(4, tuple);
    } else {
        // set
        mp_int_t wanted_sysclk = mp_obj_get_int(args[0]) / 1000000;

        // default PLL parameters that give 48MHz on PLL48CK
        uint32_t m = HSE_VALUE / 1000000, n = 336, p = 2, q = 7;
        uint32_t sysclk_source;

        // the following logic assumes HSE < HSI
        if (HSE_VALUE / 1000000 <= wanted_sysclk && wanted_sysclk < HSI_VALUE / 1000000) {
            // use HSE as SYSCLK
            sysclk_source = RCC_SYSCLKSOURCE_HSE;
        } else if (HSI_VALUE / 1000000 <= wanted_sysclk && wanted_sysclk < 24) {
            // use HSI as SYSCLK
            sysclk_source = RCC_SYSCLKSOURCE_HSI;
        } else {
            // search for a valid PLL configuration that keeps USB at 48MHz
            for (; wanted_sysclk > 0; wanted_sysclk--) {
                for (p = 2; p <= 8; p += 2) {
                    // compute VCO_OUT
                    mp_uint_t vco_out = wanted_sysclk * p;
                    // make sure VCO_OUT is between 192MHz and 432MHz
                    if (vco_out < 192 || vco_out > 432) {
                        continue;
                    }
                    // make sure Q is an integer
                    if (vco_out % 48 != 0) {
                        continue;
                    }
                    // solve for Q to get PLL48CK at 48MHz
                    q = vco_out / 48;
                    // make sure Q is in range
                    if (q < 2 || q > 15) {
                        continue;
                    }
                    // make sure N/M is an integer
                    if (vco_out % (HSE_VALUE / 1000000) != 0) {
                        continue;
                    }
                    // solve for N/M
                    mp_uint_t n_by_m = vco_out / (HSE_VALUE / 1000000);
                    // solve for M, making sure VCO_IN (=HSE/M) is between 1MHz and 2MHz
                    m = 192 / n_by_m;
                    while (m < (HSE_VALUE / 2000000) || n_by_m * m < 192) {
                        m += 1;
                    }
                    if (m > (HSE_VALUE / 1000000)) {
                        continue;
                    }
                    // solve for N
                    n = n_by_m * m;
                    // make sure N is in range
                    if (n < 192 || n > 432) {
                        continue;
                    }

                    // found values!
                    sysclk_source = RCC_SYSCLKSOURCE_PLLCLK;
                    goto set_clk;
                }
            }
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "can't make valid freq"));
        }

    set_clk:
        //printf("%lu %lu %lu %lu %lu\n", sysclk_source, m, n, p, q);

        // let the USB CDC have a chance to process before we change the clock
        HAL_Delay(USBD_CDC_POLLING_INTERVAL + 2);

        // desired system clock source is in sysclk_source
        RCC_ClkInitTypeDef RCC_ClkInitStruct;
        RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
        if (sysclk_source == RCC_SYSCLKSOURCE_PLLCLK) {
            // set HSE as system clock source to allow modification of the PLL configuration
            // we then change to PLL after re-configuring PLL
            RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
        } else {
            // directly set the system clock source as desired
            RCC_ClkInitStruct.SYSCLKSource = sysclk_source;
        }
        wanted_sysclk *= 1000000;
        if (n_args >= 2) {
            // note: AHB freq required to be >= 14.2MHz for USB operation
            RCC_ClkInitStruct.AHBCLKDivider = pyb_freq_calc_ahb_div(wanted_sysclk / mp_obj_get_int(args[1]));
        } else {
            RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
        }
        if (n_args >= 3) {
            RCC_ClkInitStruct.APB1CLKDivider = pyb_freq_calc_apb_div(wanted_sysclk / mp_obj_get_int(args[2]));
        } else {
            RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
        }
        if (n_args >= 4) {
            RCC_ClkInitStruct.APB2CLKDivider = pyb_freq_calc_apb_div(wanted_sysclk / mp_obj_get_int(args[3]));
        } else {
            RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
        }
        if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
            goto fail;
        }

        // re-configure PLL
        // even if we don't use the PLL for the system clock, we still need it for USB, RNG and SDIO
        RCC_OscInitTypeDef RCC_OscInitStruct;
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
        RCC_OscInitStruct.HSEState = RCC_HSE_ON;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
        RCC_OscInitStruct.PLL.PLLM = m;
        RCC_OscInitStruct.PLL.PLLN = n;
        RCC_OscInitStruct.PLL.PLLP = p;
        RCC_OscInitStruct.PLL.PLLQ = q;
        if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
            goto fail;
        }

        // set PLL as system clock source if wanted
        if (sysclk_source == RCC_SYSCLKSOURCE_PLLCLK) {
            RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
            RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
            if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
                goto fail;
            }
        }

        // re-init TIM3 for USB CDC rate
        timer_tim3_init();

        return mp_const_none;

    fail:;
        void NORETURN __fatal_error(const char *msg);
        __fatal_error("can't change freq");
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_freq_obj, 0, 4, pyb_freq);

/// \function millis()
/// Returns the number of milliseconds since the board was last reset.
///
/// The result is always a micropython smallint (31-bit signed number), so
/// after 2^30 milliseconds (about 12.4 days) this will start to return
/// negative numbers.
STATIC mp_obj_t pyb_millis(void) {
    // We want to "cast" the 32 bit unsigned into a small-int.  This means
    // copying the MSB down 1 bit (extending the sign down), which is
    // equivalent to just using the MP_OBJ_NEW_SMALL_INT macro.
    return MP_OBJ_NEW_SMALL_INT(HAL_GetTick());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_millis_obj, pyb_millis);

/// \function elapsed_millis(start)
/// Returns the number of milliseconds which have elapsed since `start`.
///
/// This function takes care of counter wrap, and always returns a positive
/// number. This means it can be used to measure periods upto about 12.4 days.
///
/// Example:
///     start = pyb.millis()
///     while pyb.elapsed_millis(start) < 1000:
///         # Perform some operation
STATIC mp_obj_t pyb_elapsed_millis(mp_obj_t start) {
    uint32_t startMillis = mp_obj_get_int(start);
    uint32_t currMillis = HAL_GetTick();
    return MP_OBJ_NEW_SMALL_INT((currMillis - startMillis) & 0x3fffffff);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_elapsed_millis_obj, pyb_elapsed_millis);

/// \function micros()
/// Returns the number of microseconds since the board was last reset.
///
/// The result is always a micropython smallint (31-bit signed number), so
/// after 2^30 microseconds (about 17.8 minutes) this will start to return
/// negative numbers.
STATIC mp_obj_t pyb_micros(void) {
    // We want to "cast" the 32 bit unsigned into a small-int.  This means
    // copying the MSB down 1 bit (extending the sign down), which is
    // equivalent to just using the MP_OBJ_NEW_SMALL_INT macro.
    return MP_OBJ_NEW_SMALL_INT(sys_tick_get_microseconds());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_micros_obj, pyb_micros);

/// \function elapsed_micros(start)
/// Returns the number of microseconds which have elapsed since `start`.
///
/// This function takes care of counter wrap, and always returns a positive
/// number. This means it can be used to measure periods upto about 17.8 minutes.
///
/// Example:
///     start = pyb.micros()
///     while pyb.elapsed_micros(start) < 1000:
///         # Perform some operation
STATIC mp_obj_t pyb_elapsed_micros(mp_obj_t start) {
    uint32_t startMicros = mp_obj_get_int(start);
    uint32_t currMicros = sys_tick_get_microseconds();
    return MP_OBJ_NEW_SMALL_INT((currMicros - startMicros) & 0x3fffffff);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_elapsed_micros_obj, pyb_elapsed_micros);

/// \function delay(ms)
/// Delay for the given number of milliseconds.
STATIC mp_obj_t pyb_delay(mp_obj_t ms_in) {
    mp_int_t ms = mp_obj_get_int(ms_in);
    if (ms >= 0) {
        HAL_Delay(ms);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_delay_obj, pyb_delay);

/// \function udelay(us)
/// Delay for the given number of microseconds.
STATIC mp_obj_t pyb_udelay(mp_obj_t usec_in) {
    mp_int_t usec = mp_obj_get_int(usec_in);
    if (usec > 0) {
        sys_tick_udelay(usec);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_udelay_obj, pyb_udelay);

/// \function stop()
STATIC mp_obj_t pyb_stop(void) {
    // takes longer to wake but reduces stop current
    HAL_PWREx_EnableFlashPowerDown();

    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

    // reconfigure the system clock after waking up

    // enable HSE
    __HAL_RCC_HSE_CONFIG(RCC_HSE_ON);
    while (!__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY)) {
    }

    // enable PLL
    __HAL_RCC_PLL_ENABLE();
    while (!__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY)) {
    }

    // select PLL as system clock source
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_SYSCLKSOURCE_PLLCLK);
    while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_CFGR_SWS_PLL) {
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(pyb_stop_obj, pyb_stop);

/// \function standby()
STATIC mp_obj_t pyb_standby(void) {
#if defined(MCU_SERIES_F7)
    printf("pyb.standby not supported yet\n");
#else
    // We need to clear the PWR wake-up-flag before entering standby, since
    // the flag may have been set by a previous wake-up event.  Furthermore,
    // we need to disable the wake-up sources while clearing this flag, so
    // that if a source is active it does actually wake the device.
    // See section 5.3.7 of RM0090.

    // Note: we only support RTC ALRA, ALRB, WUT and TS.
    // TODO support TAMP and WKUP (PA0 external pin).
    uint32_t irq_bits = RTC_CR_ALRAIE | RTC_CR_ALRBIE | RTC_CR_WUTIE | RTC_CR_TSIE;

    // save RTC interrupts
    uint32_t save_irq_bits = RTC->CR & irq_bits;

    // disable RTC interrupts
    RTC->CR &= ~irq_bits;

    // clear RTC wake-up flags
    RTC->ISR &= ~(RTC_ISR_ALRAF | RTC_ISR_ALRBF | RTC_ISR_WUTF | RTC_ISR_TSF);

    // clear global wake-up flag
    PWR->CR |= PWR_CR_CWUF;

    // enable previously-enabled RTC interrupts
    RTC->CR |= save_irq_bits;

    // enter standby mode
    HAL_PWR_EnterSTANDBYMode();
    // we never return; MCU is reset on exit from standby
#endif
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(pyb_standby_obj, pyb_standby);

/// \function repl_uart(uart)
/// Get or set the UART object that the REPL is repeated on.
STATIC mp_obj_t pyb_repl_uart(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        if (MP_STATE_PORT(pyb_stdio_uart) == NULL) {
            return mp_const_none;
        } else {
            return MP_STATE_PORT(pyb_stdio_uart);
        }
    } else {
        if (args[0] == mp_const_none) {
            MP_STATE_PORT(pyb_stdio_uart) = NULL;
        } else if (mp_obj_get_type(args[0]) == &pyb_uart_type) {
            MP_STATE_PORT(pyb_stdio_uart) = args[0];
        } else {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "need a UART object"));
        }
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_repl_uart_obj, 0, 1, pyb_repl_uart);

MP_DECLARE_CONST_FUN_OBJ(pyb_main_obj); // defined in main.c

STATIC const mp_map_elem_t pyb_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_pyb) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_bootloader), (mp_obj_t)&pyb_bootloader_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_hard_reset), (mp_obj_t)&pyb_hard_reset_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_info), (mp_obj_t)&pyb_info_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_unique_id), (mp_obj_t)&pyb_unique_id_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_freq), (mp_obj_t)&pyb_freq_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_repl_info), (mp_obj_t)&pyb_set_repl_info_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_wfi), (mp_obj_t)&pyb_wfi_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disable_irq), (mp_obj_t)&pyb_disable_irq_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable_irq), (mp_obj_t)&pyb_enable_irq_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_stop), (mp_obj_t)&pyb_stop_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_standby), (mp_obj_t)&pyb_standby_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_main), (mp_obj_t)&pyb_main_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_repl_uart), (mp_obj_t)&pyb_repl_uart_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_usb_mode), (mp_obj_t)&pyb_usb_mode_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_hid_mouse), (mp_obj_t)&pyb_usb_hid_mouse_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_hid_keyboard), (mp_obj_t)&pyb_usb_hid_keyboard_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_USB_VCP), (mp_obj_t)&pyb_usb_vcp_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_USB_HID), (mp_obj_t)&pyb_usb_hid_type },
    // these 2 are deprecated; use USB_VCP.isconnected and USB_HID.send instead
    { MP_OBJ_NEW_QSTR(MP_QSTR_have_cdc), (mp_obj_t)&pyb_have_cdc_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_hid), (mp_obj_t)&pyb_hid_send_report_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_millis), (mp_obj_t)&pyb_millis_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_elapsed_millis), (mp_obj_t)&pyb_elapsed_millis_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_micros), (mp_obj_t)&pyb_micros_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_elapsed_micros), (mp_obj_t)&pyb_elapsed_micros_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_delay), (mp_obj_t)&pyb_delay_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_udelay), (mp_obj_t)&pyb_udelay_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sync), (mp_obj_t)&mod_os_sync_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mount), (mp_obj_t)&pyb_mount_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_Timer), (mp_obj_t)&pyb_timer_type },

#if MICROPY_HW_ENABLE_RNG
    { MP_OBJ_NEW_QSTR(MP_QSTR_rng), (mp_obj_t)&pyb_rng_get_obj },
#endif

#if MICROPY_HW_ENABLE_RTC
    { MP_OBJ_NEW_QSTR(MP_QSTR_RTC), (mp_obj_t)&pyb_rtc_type },
#endif

    { MP_OBJ_NEW_QSTR(MP_QSTR_Pin), (mp_obj_t)&pin_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ExtInt), (mp_obj_t)&extint_type },

#if MICROPY_HW_ENABLE_SERVO
    { MP_OBJ_NEW_QSTR(MP_QSTR_pwm), (mp_obj_t)&pyb_pwm_set_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_servo), (mp_obj_t)&pyb_servo_set_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Servo), (mp_obj_t)&pyb_servo_type },
#endif

#if MICROPY_HW_HAS_SWITCH
    { MP_OBJ_NEW_QSTR(MP_QSTR_Switch), (mp_obj_t)&pyb_switch_type },
#endif

#if MICROPY_HW_HAS_SDCARD
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD), (mp_obj_t)&pyb_sdcard_obj },
#endif

#if defined(MICROPY_HW_LED1)
    { MP_OBJ_NEW_QSTR(MP_QSTR_LED), (mp_obj_t)&pyb_led_type },
#endif
    { MP_OBJ_NEW_QSTR(MP_QSTR_I2C), (mp_obj_t)&pyb_i2c_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SPI), (mp_obj_t)&pyb_spi_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_UART), (mp_obj_t)&pyb_uart_type },
#if MICROPY_HW_ENABLE_CAN
    { MP_OBJ_NEW_QSTR(MP_QSTR_CAN), (mp_obj_t)&pyb_can_type },
#endif

    { MP_OBJ_NEW_QSTR(MP_QSTR_ADC), (mp_obj_t)&pyb_adc_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ADCAll), (mp_obj_t)&pyb_adc_all_type },

#if MICROPY_HW_ENABLE_DAC
    { MP_OBJ_NEW_QSTR(MP_QSTR_DAC), (mp_obj_t)&pyb_dac_type },
#endif

#if MICROPY_HW_HAS_MMA7660
    { MP_OBJ_NEW_QSTR(MP_QSTR_Accel), (mp_obj_t)&pyb_accel_type },
#endif

#if MICROPY_HW_HAS_LCD
    { MP_OBJ_NEW_QSTR(MP_QSTR_LCD), (mp_obj_t)&pyb_lcd_type },
#endif
};

STATIC MP_DEFINE_CONST_DICT(pyb_module_globals, pyb_module_globals_table);

const mp_obj_module_t pyb_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_pyb,
    .globals = (mp_obj_dict_t*)&pyb_module_globals,
};
