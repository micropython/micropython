/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2015 Damien P. George
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

#include "modmachine.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/machine_mem.h"
#include "extmod/machine_signal.h"
#include "extmod/machine_pulse.h"
#include "extmod/machine_i2c.h"
#include "lib/utils/pyexec.h"
#include "lib/oofatfs/ff.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "gccollect.h"
#include "irq.h"
#include "pybthread.h"
#include "rng.h"
#include "storage.h"
#include "pin.h"
#include "timer.h"
#include "usb.h"
#include "rtc.h"
#include "i2c.h"
#include "spi.h"
#include "uart.h"
#include "wdt.h"
#include "genhdr/pllfreqtable.h"

#if defined(MCU_SERIES_L4)
// L4 does not have a POR, so use BOR instead
#define RCC_CSR_PORRSTF RCC_CSR_BORRSTF
#endif

#define PYB_RESET_SOFT      (0)
#define PYB_RESET_POWER_ON  (1)
#define PYB_RESET_HARD      (2)
#define PYB_RESET_WDT       (3)
#define PYB_RESET_DEEPSLEEP (4)

STATIC uint32_t reset_cause;

void machine_init(void) {
    #if defined(MCU_SERIES_F4)
    if (PWR->CSR & PWR_CSR_SBF) {
        // came out of standby
        reset_cause = PYB_RESET_DEEPSLEEP;
        PWR->CR |= PWR_CR_CSBF;
    } else
    #elif defined(MCU_SERIES_F7)
    if (PWR->CSR1 & PWR_CSR1_SBF) {
        // came out of standby
        reset_cause = PYB_RESET_DEEPSLEEP;
        PWR->CR1 |= PWR_CR1_CSBF;
    } else
    #endif
    {
        // get reset cause from RCC flags
        uint32_t state = RCC->CSR;
        if (state & RCC_CSR_IWDGRSTF || state & RCC_CSR_WWDGRSTF) {
            reset_cause = PYB_RESET_WDT;
        } else if (state & RCC_CSR_PORRSTF || state & RCC_CSR_BORRSTF) {
            reset_cause = PYB_RESET_POWER_ON;
        } else if (state & RCC_CSR_PINRSTF) {
            reset_cause = PYB_RESET_HARD;
        } else {
            // default is soft reset
            reset_cause = PYB_RESET_SOFT;
        }
    }
    // clear RCC reset flags
    RCC->CSR |= RCC_CSR_RMVF;
}

// machine.info([dump_alloc_table])
// Print out lots of information about the board.
STATIC mp_obj_t machine_info(size_t n_args, const mp_obj_t *args) {
    // get and print unique id; 96 bits
    {
        byte *id = (byte*)MP_HAL_UNIQUE_ID_ADDRESS;
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
        for (mp_vfs_mount_t *vfs = MP_STATE_VM(vfs_mount_table); vfs != NULL; vfs = vfs->next) {
            if (strncmp("/flash", vfs->str, vfs->len) == 0) {
                // assumes that it's a FatFs filesystem
                fs_user_mount_t *vfs_fat = MP_OBJ_TO_PTR(vfs->obj);
                DWORD nclst;
                f_getfree(&vfs_fat->fatfs, &nclst);
                printf("LFS free: %u bytes\n", (uint)(nclst * vfs_fat->fatfs.csize * 512));
                break;
            }
        }
    }

    #if MICROPY_PY_THREAD
    pyb_thread_dump();
    #endif

    if (n_args == 1) {
        // arg given means dump gc allocation table
        gc_dump_alloc_table();
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_info_obj, 0, 1, machine_info);

// Returns a string of 12 bytes (96 bits), which is the unique ID for the MCU.
STATIC mp_obj_t machine_unique_id(void) {
    byte *id = (byte*)MP_HAL_UNIQUE_ID_ADDRESS;
    return mp_obj_new_bytes(id, 12);
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_unique_id_obj, machine_unique_id);

// Resets the pyboard in a manner similar to pushing the external RESET button.
STATIC mp_obj_t machine_reset(void) {
    NVIC_SystemReset();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_obj, machine_reset);

STATIC mp_obj_t machine_soft_reset(void) {
    pyexec_system_exit = PYEXEC_FORCED_EXIT;
    nlr_raise(mp_obj_new_exception(&mp_type_SystemExit));
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_soft_reset_obj, machine_soft_reset);

// Activate the bootloader without BOOT* pins.
STATIC NORETURN mp_obj_t machine_bootloader(void) {
    pyb_usb_dev_deinit();
    storage_flush();

    HAL_RCC_DeInit();
    HAL_DeInit();

    #if (__MPU_PRESENT == 1)
    // MPU must be disabled for bootloader to function correctly
    HAL_MPU_Disable();
    #endif

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
MP_DEFINE_CONST_FUN_OBJ_0(machine_bootloader_obj, machine_bootloader);

// get or set the MCU frequencies
STATIC mp_uint_t machine_freq_calc_ahb_div(mp_uint_t wanted_div) {
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
STATIC mp_uint_t machine_freq_calc_apb_div(mp_uint_t wanted_div) {
    if (wanted_div <= 1) { return RCC_HCLK_DIV1; }
    else if (wanted_div <= 2) { return RCC_HCLK_DIV2; }
    else if (wanted_div <= 4) { return RCC_HCLK_DIV4; }
    else if (wanted_div <= 8) { return RCC_HCLK_DIV8; }
    else { return RCC_SYSCLK_DIV16; }
}
STATIC mp_obj_t machine_freq(size_t n_args, const mp_obj_t *args) {
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

        #if defined(MCU_SERIES_L4)
        mp_raise_NotImplementedError("machine.freq set not supported yet");
        #endif

        // default PLL parameters that give 48MHz on PLL48CK
        uint32_t m = HSE_VALUE / 1000000, n = 336, p = 2, q = 7;
        uint32_t sysclk_source;

        // search for a valid PLL configuration that keeps USB at 48MHz
        for (const uint16_t *pll = &pll_freq_table[MP_ARRAY_SIZE(pll_freq_table) - 1]; pll >= &pll_freq_table[0]; --pll) {
            uint32_t sys = *pll & 0xff;
            if (sys <= wanted_sysclk) {
                m = (*pll >> 10) & 0x3f;
                p = ((*pll >> 7) & 0x6) + 2;
                if (m == 0) {
                    // special entry for using HSI directly
                    sysclk_source = RCC_SYSCLKSOURCE_HSI;
                    goto set_clk;
                } else if (m == 1) {
                    // special entry for using HSE directly
                    sysclk_source = RCC_SYSCLKSOURCE_HSE;
                    goto set_clk;
                } else {
                    // use PLL
                    sysclk_source = RCC_SYSCLKSOURCE_PLLCLK;
                    uint32_t vco_out = sys * p;
                    n = vco_out * m / (HSE_VALUE / 1000000);
                    q = vco_out / 48;
                    goto set_clk;
                }
            }
        }
        mp_raise_ValueError("can't make valid freq");

    set_clk:
        //printf("%lu %lu %lu %lu %lu\n", sysclk_source, m, n, p, q);

        // let the USB CDC have a chance to process before we change the clock
        mp_hal_delay_ms(5);

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
            RCC_ClkInitStruct.AHBCLKDivider = machine_freq_calc_ahb_div(wanted_sysclk / mp_obj_get_int(args[1]));
        } else {
            RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
        }
        if (n_args >= 3) {
            RCC_ClkInitStruct.APB1CLKDivider = machine_freq_calc_apb_div(wanted_sysclk / mp_obj_get_int(args[2]));
        } else {
            RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
        }
        if (n_args >= 4) {
            RCC_ClkInitStruct.APB2CLKDivider = machine_freq_calc_apb_div(wanted_sysclk / mp_obj_get_int(args[3]));
        } else {
            RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
        }
        #if defined(MICROPY_HW_CLK_LAST_FREQ) && MICROPY_HW_CLK_LAST_FREQ
        uint32_t h = RCC_ClkInitStruct.AHBCLKDivider >> 4;
        uint32_t b1 = RCC_ClkInitStruct.APB1CLKDivider >> 10;
        uint32_t b2 = RCC_ClkInitStruct.APB2CLKDivider >> 10;
        #endif
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
            #if defined(MCU_SERIES_F7)
            // if possible, scale down the internal voltage regulator to save power
            uint32_t volt_scale;
            if (wanted_sysclk <= 151000000) {
                volt_scale = PWR_REGULATOR_VOLTAGE_SCALE3;
            } else if (wanted_sysclk <= 180000000) {
                volt_scale = PWR_REGULATOR_VOLTAGE_SCALE2;
            } else {
                volt_scale = PWR_REGULATOR_VOLTAGE_SCALE1;
            }
            if (HAL_PWREx_ControlVoltageScaling(volt_scale) != HAL_OK) {
                goto fail;
            }
            #endif

            #if !defined(MICROPY_HW_FLASH_LATENCY)
            #define MICROPY_HW_FLASH_LATENCY FLASH_LATENCY_5
            #endif
            RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
            RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
            if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, MICROPY_HW_FLASH_LATENCY) != HAL_OK) {
                goto fail;
            }
        }

        #if defined(MICROPY_HW_CLK_LAST_FREQ) && MICROPY_HW_CLK_LAST_FREQ
        #if defined(MCU_SERIES_F7)
        #define FREQ_BKP BKP31R
        #else
        #define FREQ_BKP BKP19R
        #endif
        // qqqqqqqq pppppppp nnnnnnnn nnmmmmmm
        // qqqqQQQQ ppppppPP nNNNNNNN NNMMMMMM
        // 222111HH HHQQQQPP nNNNNNNN NNMMMMMM
        p = (p / 2) - 1;
        RTC->FREQ_BKP = m
            | (n << 6) | (p << 16) | (q << 18)
            | (h << 22)
            | (b1 << 26)
            | (b2 << 29);
        #endif

        return mp_const_none;

    fail:;
        void NORETURN __fatal_error(const char *msg);
        __fatal_error("can't change freq");
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_freq_obj, 0, 4, machine_freq);

STATIC mp_obj_t machine_sleep(void) {
    #if defined(MCU_SERIES_L4)

    // Enter Stop 1 mode
    __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_MSI);
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

    // reconfigure system clock after wakeup
    // Enable Power Control clock
    __HAL_RCC_PWR_CLK_ENABLE();

    // Get the Oscillators configuration according to the internal RCC registers
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    HAL_RCC_GetOscConfig(&RCC_OscInitStruct);
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    // Get the Clocks configuration according to the internal RCC registers
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    uint32_t pFLatency = 0;
    HAL_RCC_GetClockConfig(&RCC_ClkInitStruct, &pFLatency);

    // Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clock dividers
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
    RCC_ClkInitStruct.SYSCLKSource  = RCC_SYSCLKSOURCE_PLLCLK;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, pFLatency);

    #else

    // takes longer to wake but reduces stop current
    HAL_PWREx_EnableFlashPowerDown();

    # if defined(MCU_SERIES_F7)
    HAL_PWR_EnterSTOPMode((PWR_CR1_LPDS | PWR_CR1_LPUDS | PWR_CR1_FPDS | PWR_CR1_UDEN), PWR_STOPENTRY_WFI);
    # else
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    #endif

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

    #endif

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_sleep_obj, machine_sleep);

STATIC mp_obj_t machine_deepsleep(void) {
    rtc_init_finalise();

#if defined(MCU_SERIES_L4)
    printf("machine.deepsleep not supported yet\n");
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

    #if defined(MCU_SERIES_F7)
    // disable wake-up flags
    PWR->CSR2 &= ~(PWR_CSR2_EWUP6 | PWR_CSR2_EWUP5 | PWR_CSR2_EWUP4 | PWR_CSR2_EWUP3 | PWR_CSR2_EWUP2 | PWR_CSR2_EWUP1);
    // clear global wake-up flag
    PWR->CR2 |= PWR_CR2_CWUPF6 | PWR_CR2_CWUPF5 | PWR_CR2_CWUPF4 | PWR_CR2_CWUPF3 | PWR_CR2_CWUPF2 | PWR_CR2_CWUPF1;
    #else
    // clear global wake-up flag
    PWR->CR |= PWR_CR_CWUF;
    #endif

    // enable previously-enabled RTC interrupts
    RTC->CR |= save_irq_bits;

    // enter standby mode
    HAL_PWR_EnterSTANDBYMode();
    // we never return; MCU is reset on exit from standby
#endif
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_deepsleep_obj, machine_deepsleep);

STATIC mp_obj_t machine_reset_cause(void) {
    return MP_OBJ_NEW_SMALL_INT(reset_cause);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_cause_obj, machine_reset_cause);

STATIC const mp_rom_map_elem_t machine_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),            MP_ROM_QSTR(MP_QSTR_umachine) },
    { MP_ROM_QSTR(MP_QSTR_info),                MP_ROM_PTR(&machine_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_unique_id),           MP_ROM_PTR(&machine_unique_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset),               MP_ROM_PTR(&machine_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_soft_reset),          MP_ROM_PTR(&machine_soft_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_bootloader),          MP_ROM_PTR(&machine_bootloader_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq),                MP_ROM_PTR(&machine_freq_obj) },
#if MICROPY_HW_ENABLE_RNG
    { MP_ROM_QSTR(MP_QSTR_rng),                 MP_ROM_PTR(&pyb_rng_get_obj) },
#endif
    { MP_ROM_QSTR(MP_QSTR_idle),                MP_ROM_PTR(&pyb_wfi_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep),               MP_ROM_PTR(&machine_sleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_deepsleep),           MP_ROM_PTR(&machine_deepsleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_cause),         MP_ROM_PTR(&machine_reset_cause_obj) },
#if 0
    { MP_ROM_QSTR(MP_QSTR_wake_reason),         MP_ROM_PTR(&machine_wake_reason_obj) },
#endif

    { MP_ROM_QSTR(MP_QSTR_disable_irq),         MP_ROM_PTR(&pyb_disable_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_irq),          MP_ROM_PTR(&pyb_enable_irq_obj) },

    { MP_ROM_QSTR(MP_QSTR_time_pulse_us),       MP_ROM_PTR(&machine_time_pulse_us_obj) },

    { MP_ROM_QSTR(MP_QSTR_mem8),                MP_ROM_PTR(&machine_mem8_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem16),               MP_ROM_PTR(&machine_mem16_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem32),               MP_ROM_PTR(&machine_mem32_obj) },

    { MP_ROM_QSTR(MP_QSTR_Pin),                 MP_ROM_PTR(&pin_type) },
    { MP_ROM_QSTR(MP_QSTR_Signal),              MP_ROM_PTR(&machine_signal_type) },

#if 0
    { MP_ROM_QSTR(MP_QSTR_RTC),                 MP_ROM_PTR(&pyb_rtc_type) },
    { MP_ROM_QSTR(MP_QSTR_ADC),                 MP_ROM_PTR(&pyb_adc_type) },
#endif
#if MICROPY_PY_MACHINE_I2C
    { MP_ROM_QSTR(MP_QSTR_I2C),                 MP_ROM_PTR(&machine_i2c_type) },
#endif
    { MP_ROM_QSTR(MP_QSTR_SPI),                 MP_ROM_PTR(&machine_hard_spi_type) },
    { MP_ROM_QSTR(MP_QSTR_UART),                MP_ROM_PTR(&pyb_uart_type) },
    { MP_ROM_QSTR(MP_QSTR_WDT),                 MP_ROM_PTR(&pyb_wdt_type) },
#if 0
    { MP_ROM_QSTR(MP_QSTR_Timer),               MP_ROM_PTR(&pyb_timer_type) },
    { MP_ROM_QSTR(MP_QSTR_HeartBeat),           MP_ROM_PTR(&pyb_heartbeat_type) },
    { MP_ROM_QSTR(MP_QSTR_SD),                  MP_ROM_PTR(&pyb_sd_type) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_IDLE),                MP_ROM_INT(PYB_PWR_MODE_ACTIVE) },
    { MP_ROM_QSTR(MP_QSTR_SLEEP),               MP_ROM_INT(PYB_PWR_MODE_LPDS) },
    { MP_ROM_QSTR(MP_QSTR_DEEPSLEEP),           MP_ROM_INT(PYB_PWR_MODE_HIBERNATE) },
#endif
    { MP_ROM_QSTR(MP_QSTR_PWRON_RESET),         MP_ROM_INT(PYB_RESET_POWER_ON) },
    { MP_ROM_QSTR(MP_QSTR_HARD_RESET),          MP_ROM_INT(PYB_RESET_HARD) },
    { MP_ROM_QSTR(MP_QSTR_WDT_RESET),           MP_ROM_INT(PYB_RESET_WDT) },
    { MP_ROM_QSTR(MP_QSTR_DEEPSLEEP_RESET),     MP_ROM_INT(PYB_RESET_DEEPSLEEP) },
    { MP_ROM_QSTR(MP_QSTR_SOFT_RESET),          MP_ROM_INT(PYB_RESET_SOFT) },
#if 0
    { MP_ROM_QSTR(MP_QSTR_WLAN_WAKE),           MP_ROM_INT(PYB_SLP_WAKED_BY_WLAN) },
    { MP_ROM_QSTR(MP_QSTR_PIN_WAKE),            MP_ROM_INT(PYB_SLP_WAKED_BY_GPIO) },
    { MP_ROM_QSTR(MP_QSTR_RTC_WAKE),            MP_ROM_INT(PYB_SLP_WAKED_BY_RTC) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t machine_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&machine_module_globals,
};

