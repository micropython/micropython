/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2024 Infineon Technologies AG
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

// std includes
#include <stdio.h>
#include <stdlib.h>

// mpy includes
#include "py/obj.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "shared/runtime/pyexec.h"
#include "extmod/modnetwork.h"

// MTB includes
#include "cybsp.h"
#include "cyhal.h"
#include "cy_pdl.h"
#include "cy_retarget_io.h"

// port-specific includes
#include "modmachine.h"
#include "mplogger.h"
#include "modpsoc6.h"
#if MICROPY_PY_MACHINE

#define clock_assert_raise_val(msg, ret)   if (ret != CY_RSLT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

// enums to hold the MPY constants as given in guidelines
enum {
    MACHINE_PWRON_RESET,
    MACHINE_HARD_RESET,
    MACHINE_WDT_RESET,
    MACHINE_DEEPSLEEP_RESET,
    MACHINE_SOFT_RESET
};

uint32_t reset_cause;
enum clock_freq_type freq_peri;
enum clock_freq_type PLL0_freq;

// function to return 64-bit silicon ID of given PSoC microcontroller
// A combined 64-bit unique ID. [63:57] - DIE_YEAR [56:56] - DIE_MINOR [55:48] - DIE_SORT [47:40] - DIE_Y [39:32] - DIE_X [31:24] - DIE_WAFER [23:16] - DIE_LOT[2] [15: 8] - DIE_LOT[1] [ 7: 0] - DIE_LOT[0]
static uint64_t system_get_unique_id(void) {
    return Cy_SysLib_GetUniqueId();
}

// helper function to generate random alphanumeric hash
static uint8_t system_rand_hash(uint8_t length) {
    uint8_t hash_sum = 0;
    char charset[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'}; // hash can be made stronger but
                                                                         // uint8_t can only hold <=255

    while (length-- > 0) {
        uint8_t idx = rand() % sizeof(charset);
        hash_sum = hash_sum + (int)charset[idx];
    }
    return hash_sum;
}

// global var to store current irq state/hash
static uint8_t system_irq_key;

// defines how strong the hash for enable/disable interrupt is, how many chars long
#define HASH_CHARS_NUM 10

// function to disable global IRQs
// returns alphanumeric hash to enable IRQs later
static uint8_t system_disable_global_irq(void) {
    uint8_t state = system_rand_hash(HASH_CHARS_NUM); // 10 chars long key gen;
    __disable_irq();
    system_irq_key = state;
    return state;
}

// function to enable global IRQs
// uses passed alphanumeric key to verify and enable IRQs
static bool system_enable_global_irq(uint8_t state) {
    if (state == system_irq_key) {
        __enable_irq();
        return true;
    } else {
        return false;
    }
}

// API to return clock freq; Fast CLK (CM4) is the main sys clk
static uint32_t system_get_cpu_freq(void) {
    return Cy_SysClk_ClkFastGetFrequency();
}

void machine_init(void) {
    mplogger_print("machine init\n");
    // TODO: put all module init functions here ?
    // machine_pin_init(); ?
}

void machine_deinit(void) {
    // we are doing a soft-reset so change the reset_cause
    reset_cause = MACHINE_SOFT_RESET;
    mplogger_print("machine deinit\n");
    mod_wdt_deinit();
    mod_pin_deinit();
    mod_adcblock_deinit();
    mod_i2c_deinit();
    mod_pwm_deinit();
    mod_spi_deinit();
    mod_rtc_deinit();
    mod_timer_deinit();
    #if MICROPY_ENABLE_SD_CARD
    mod_sdcard_deinit();
    #endif
}

// machine.info([dump_alloc_table])
// Print out lots of information about the board.
static mp_obj_t machine_info(size_t n_args, const mp_obj_t *args) {
    mp_printf(&mp_plat_print, "\nmachine info :\n");

    // qstr info
    {
        size_t n_pool, n_qstr, n_str_data_bytes, n_total_bytes;
        qstr_pool_info(&n_pool, &n_qstr, &n_str_data_bytes, &n_total_bytes);
        mp_printf(&mp_plat_print, "\n  qstr :\n    n_pool = %u\n    n_qstr = %u\n    n_str_data_bytes = %u\n    n_total_bytes = %u\n", n_pool, n_qstr, n_str_data_bytes, n_total_bytes);
    }

    // GC info
    {
        gc_info_t info;
        gc_info(&info);
        mp_printf(&mp_plat_print, "\n  GC heap :\n");
        mp_printf(&mp_plat_print, "    total : 0x%x (%u)\n", info.total, info.total);
        mp_printf(&mp_plat_print, "    used  : 0x%x (%u)\n", info.used, info.used);
        mp_printf(&mp_plat_print, "    free  : 0x%x (%u)\n", info.free, info.free);
        mp_printf(&mp_plat_print, "    #1-blocks = 0x%x   #2-blocks = 0x%x   max-block = 0x%x\n", info.num_1block, info.num_2block, info.max_block);


        extern uint8_t __StackTop, __StackLimit;
        mp_printf(&mp_plat_print, "\n  GC stack :\n");
        printf("    __StackTop     : 0x%x (%u)\n", (mp_uint_t)&__StackTop, (mp_uint_t)&__StackTop);
        printf("    __StackLimit   : 0x%x (%u)\n", (mp_uint_t)&__StackLimit, (mp_uint_t)&__StackLimit);
        printf("    GC stack limit : 0x%x (%u)\n\n", (mp_uint_t)&__StackTop - (mp_uint_t)&__StackLimit, (mp_uint_t)&__StackTop - (mp_uint_t)&__StackLimit);

        printf("    GC heap size   : 0x%x (%u)\n", MICROPY_GC_HEAP_SIZE, MICROPY_GC_HEAP_SIZE);
    }

    // flash info
    {
        mp_printf(&mp_plat_print, "\n  flash memory map :\n");

        cyhal_flash_t cyhal_flash_obj;
        cyhal_flash_info_t flash_info;

        // Initialize flash object
        cyhal_flash_init(&cyhal_flash_obj);

        // Get flash characteristics
        cyhal_flash_get_info(&cyhal_flash_obj, &flash_info);

        // Wait for 100ms for the flash write to complete
        uint32_t timeout = 100;

        // Wait for the command to finish execution
        while ((true != cyhal_flash_is_operation_complete(&cyhal_flash_obj)) && (0 < timeout)) {
            timeout--;
            cyhal_system_delay_ms(1); // delay one millisecond each iteration
        }

        const cyhal_flash_block_info_t *block_info = 0;

        if (0 != timeout) {
            for (int index = 0; index < flash_info.block_count; index++)
            {
                block_info = flash_info.blocks;
                block_info += index;

                mp_printf(&mp_plat_print, "    block_info->start_address : 0x%lx\n", block_info->start_address);
                mp_printf(&mp_plat_print, "    block_info->size          : 0x%lx\n", block_info->size);
                mp_printf(&mp_plat_print, "    block_info->sector_size   : 0x%lx\n", block_info->sector_size);
                mp_printf(&mp_plat_print, "    block_info->page_size     : 0x%lx\n", block_info->page_size);
                mp_printf(&mp_plat_print, "    block_info->erase_value   : 0x%x\n\n", block_info->erase_value);
            }
        }
    }


    mp_printf(&mp_plat_print, "\n");
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_info_obj, 0, 1, machine_info);


static mp_obj_t mp_machine_get_freq(void) {
    return MP_OBJ_NEW_SMALL_INT(system_get_cpu_freq());
}

void cm4_set_frequency(uint32_t freq) {
    cyhal_clock_t clock_fast;
    cyhal_clock_t clock_slow;
    cyhal_clock_t clock_pll;
    cyhal_clock_t clock_hf0;
    cyhal_clock_t clock_peri;

    // deinitialize retarget-io before changing the clock frequency
    cy_retarget_io_deinit();

    /* Initialize, take ownership of PLL0/PLL */
    cyhal_clock_reserve(&clock_pll, &CYHAL_CLOCK_PLL[0]);


    /* Set the PLL0/PLL frequency to PLL_CLOCK_HZ =150 MHZ*/
    cy_rslt_t result = cyhal_clock_set_frequency(&clock_pll,  freq, NULL);
    clock_assert_raise_val("PLL clock reserve failed with error code: %lx", result);

    /* If the PLL0/PLL clock is not already enabled, enable it */
    if (!cyhal_clock_is_enabled(&clock_pll)) {
        result = cyhal_clock_set_enabled(&clock_pll, true, true);
        clock_assert_raise_val("PLL clock enable failed with error code: %lx", result);
    }

    // HF0
    /* Initialize, take ownership of CLK_HF0 */
    result = cyhal_clock_reserve(&clock_hf0, &CYHAL_CLOCK_HF[0]);
    clock_assert_raise_val("HF0 clock reserve failed with error code: %lx", result);

    /* Source the (CLK_HF0) from PLL0/PLL */
    result = cyhal_clock_set_source(&clock_hf0, &clock_pll);
    clock_assert_raise_val("HF0 clock source failed with error code: %lx", result);

    /* Set the divider for (CLK_HF0) */
    result = cyhal_clock_set_divider(&clock_hf0, 1);
    clock_assert_raise_val("HF0 clock set divider failed with error code: %lx", result);

    /*  (CLK_HF0) is not already enabled, enable it */
    if (!cyhal_clock_is_enabled(&clock_hf0)) {
        result = cyhal_clock_set_enabled(&clock_hf0, true, true);
        clock_assert_raise_val("HF0 clock enable failed with error code: %lx", result);
    }
    // HF0

    // Fast clock
    result = cyhal_clock_reserve(&clock_fast, &CYHAL_CLOCK_FAST);
    clock_assert_raise_val("Fast clock reserve failed with error code: %lx", result);

    result = cyhal_clock_set_divider(&clock_fast, 1);
    clock_assert_raise_val("Fast clock set divider failed with error code: %lx", result);



    if (!cyhal_clock_is_enabled(&clock_fast)) {
        result = cyhal_clock_set_enabled(&clock_fast, true, true);
        clock_assert_raise_val("Fast clock enable failed with error code: %lx", result);
    }
    // Fast clock

    // Peri clock
    result = cyhal_clock_reserve(&clock_peri, &CYHAL_CLOCK_PERI);
    clock_assert_raise_val("Peri clock reserve failed with error code: %lx", result);

    result = cyhal_clock_set_divider(&clock_peri, 2);

    if (!cyhal_clock_is_enabled(&clock_peri)) {
        result = cyhal_clock_set_enabled(&clock_peri, true, true);
        clock_assert_raise_val("Peri clock enable failed with error code: %lx", result);
    }
    // peri clock


    // slow clock
    result = cyhal_clock_reserve(&clock_slow, &CYHAL_CLOCK_SLOW);
    clock_assert_raise_val("Slow clock reserve failed with error code: %lx", result);

    result = cyhal_clock_set_divider(&clock_slow, 1);
    clock_assert_raise_val("Slow clock set divider failed with error code: %lx", result);

    if (!cyhal_clock_is_enabled(&clock_slow)) {
        result = cyhal_clock_set_enabled(&clock_slow, true, true);
        clock_assert_raise_val("Slow clock enable failed with error code: %lx", result);
    }
    // slow clock

    cyhal_clock_free(&clock_fast);
    cyhal_clock_free(&clock_slow);
    cyhal_clock_free(&clock_pll);
    cyhal_clock_free(&clock_hf0);
    cyhal_clock_free(&clock_peri);

    // Initialize retarget-io to use the debug UART port
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS) {
        mp_raise_ValueError(MP_ERROR_TEXT("cy_retarget_io_init failed !\n"));
    }

}

void cm4_fll_set_frequency(uint32_t freq) {
    cyhal_clock_t clock_fast;
    cyhal_clock_t clock_slow;
    cyhal_clock_t clock_fll;
    cyhal_clock_t clock_hf0;
    cyhal_clock_t clock_peri;

    // deinitialize retarget-io before changing the clock frequency
    cy_retarget_io_deinit();

    /* Initialize, take ownership of FLL */
    cyhal_clock_reserve(&clock_fll, &CYHAL_CLOCK_FLL);

    /* Set the FLL frequency */
    cy_rslt_t result = cyhal_clock_set_frequency(&clock_fll,  freq, NULL);
    clock_assert_raise_val("FLL clock reserve failed with error code: %lx", result);

    /* If the FLL clock is not already enabled, enable it */
    if (!cyhal_clock_is_enabled(&clock_fll)) {
        result = cyhal_clock_set_enabled(&clock_fll, true, true);
        clock_assert_raise_val("FLL clock enable failed with error code: %lx", result);
    }

    // HF0
    /* Initialize, take ownership of CLK_HF0 */
    result = cyhal_clock_reserve(&clock_hf0, &CYHAL_CLOCK_HF[0]);
    clock_assert_raise_val("HF0 clock reserve failed with error code: %lx", result);

    /* Source the (CLK_HF0) from FLL */
    result = cyhal_clock_set_source(&clock_hf0, &clock_fll);
    clock_assert_raise_val("HF0 clock source failed with error code: %lx", result);

    /* Set the divider for (CLK_HF0) */
    result = cyhal_clock_set_divider(&clock_hf0, 1);
    clock_assert_raise_val("HF0 clock set divider failed with error code: %lx", result);

    /*  (CLK_HF0) is not already enabled, enable it */
    if (!cyhal_clock_is_enabled(&clock_hf0)) {
        result = cyhal_clock_set_enabled(&clock_hf0, true, true);
        clock_assert_raise_val("HF0 clock enable failed with error code: %lx", result);
    }
    // HF0

    // Fast clock
    result = cyhal_clock_reserve(&clock_fast, &CYHAL_CLOCK_FAST);
    clock_assert_raise_val("Fast clock reserve failed with error code: %lx", result);

    result = cyhal_clock_set_divider(&clock_fast, 1);
    clock_assert_raise_val("Fast clock set divider failed with error code: %lx", result);



    if (!cyhal_clock_is_enabled(&clock_fast)) {
        result = cyhal_clock_set_enabled(&clock_fast, true, true);
        clock_assert_raise_val("Fast clock enable failed with error code: %lx", result);
    }
    // Fast clock

    // Peri clock
    result = cyhal_clock_reserve(&clock_peri, &CYHAL_CLOCK_PERI);
    clock_assert_raise_val("Peri clock reserve failed with error code: %lx", result);

    result = cyhal_clock_set_divider(&clock_peri, 2);

    if (!cyhal_clock_is_enabled(&clock_peri)) {
        result = cyhal_clock_set_enabled(&clock_peri, true, true);
        clock_assert_raise_val("Peri clock enable failed with error code: %lx", result);
    }
    // peri clock


    // slow clock
    result = cyhal_clock_reserve(&clock_slow, &CYHAL_CLOCK_SLOW);
    clock_assert_raise_val("Slow clock reserve failed with error code: %lx", result);

    result = cyhal_clock_set_divider(&clock_slow, 1);
    clock_assert_raise_val("Slow clock set divider failed with error code: %lx", result);

    if (!cyhal_clock_is_enabled(&clock_slow)) {
        result = cyhal_clock_set_enabled(&clock_slow, true, true);
        clock_assert_raise_val("Slow clock enable failed with error code: %lx", result);
    }
    // slow clock

    cyhal_clock_free(&clock_fast);
    cyhal_clock_free(&clock_slow);
    cyhal_clock_free(&clock_fll);
    cyhal_clock_free(&clock_hf0);
    cyhal_clock_free(&clock_peri);

    // Initialize retarget-io to use the debug UART port
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS) {
        mp_raise_ValueError(MP_ERROR_TEXT("cy_retarget_io_init failed !\n"));
    }
}

void audio_i2s_set_frequency(uint32_t freq) {

    if (freq != 98000000 && freq != 90000000) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Invalid PLL0 frequency %lu"), freq);
    }
    cyhal_clock_t clock_pll;
    cy_rslt_t result;

    static bool clock_set_i2s = false;

    result = cyhal_clock_reserve(&clock_pll, &CYHAL_CLOCK_PLL[0]);
    clock_assert_raise_val("PLL clock reserve failed with error code: %lx", result);

    uint32_t pll_source_clock_freq_hz = cyhal_clock_get_frequency(&clock_pll);
    if (freq != pll_source_clock_freq_hz) {
        clock_set_i2s = false;
        pll_source_clock_freq_hz = freq;
    }

    if (!clock_set_i2s) {
        result = cyhal_clock_set_frequency(&clock_pll,  pll_source_clock_freq_hz, NULL);
        clock_assert_raise_val("Set PLL clock frequency failed with error code: %lx", result);
        if (!cyhal_clock_is_enabled(&clock_pll)) {
            result = cyhal_clock_set_enabled(&clock_pll, true, true);
            clock_assert_raise_val("PLL clock enable failed with error code: %lx", result);
        }

        result = cyhal_clock_reserve(&audio_clock, &CYHAL_CLOCK_HF[1]);
        clock_assert_raise_val("HF1 clock reserve failed with error code: %lx", result);
        result = cyhal_clock_set_source(&audio_clock, &clock_pll);
        clock_assert_raise_val("HF1 clock sourcing failed with error code: %lx", result);
        result = cyhal_clock_set_divider(&audio_clock, 2);
        clock_assert_raise_val("HF1 clock set divider failed with error code: %lx", result);
        if (!cyhal_clock_is_enabled(&audio_clock)) {
            result = cyhal_clock_set_enabled(&audio_clock, true, true);
            clock_assert_raise_val("HF1 clock enable failed with error code: %lx", result);
        }
        cyhal_clock_free(&audio_clock);

        clock_set_i2s = true;
    }

    cyhal_clock_free(&clock_pll);

    cyhal_system_delay_ms(1);

}

void audio_pdm_set_frequency(uint32_t freq) {

    if (freq != 24576000 && freq != 22579000) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Invalid PLL0 frequency %lu"), freq);
    }
    cyhal_clock_t pll_clock;
    cy_rslt_t result;

    static bool clock_set_pdm = false;

    result = cyhal_clock_reserve(&pll_clock, &CYHAL_CLOCK_PLL[0]);
    clock_assert_raise_val("PLL clock reserve failed with error code: %lx", result);

    uint32_t pll_source_clock_freq_hz = cyhal_clock_get_frequency(&pll_clock);

    if (freq != pll_source_clock_freq_hz) {
        clock_set_pdm = false;
        pll_source_clock_freq_hz = freq;
    }

    if (!clock_set_pdm) {
        result = cyhal_clock_set_frequency(&pll_clock,  pll_source_clock_freq_hz, NULL);
        clock_assert_raise_val("Set PLL clock frequency failed with error code: %lx", result);
        if (!cyhal_clock_is_enabled(&pll_clock)) {
            result = cyhal_clock_set_enabled(&pll_clock, true, true);
            clock_assert_raise_val("PLL clock enable failed with error code: %lx", result);
        }

        result = cyhal_clock_reserve(&pdm_pcm_audio_clock, &CYHAL_CLOCK_HF[1]);
        clock_assert_raise_val("HF1 clock reserve failed with error code: %lx", result);
        result = cyhal_clock_set_source(&pdm_pcm_audio_clock, &pll_clock);
        clock_assert_raise_val("HF1 clock sourcing failed with error code: %lx", result);

        result = cyhal_clock_set_enabled(&pdm_pcm_audio_clock, true, true);
        clock_assert_raise_val("HF1 clock enable failed with error code: %lx", result);

        cyhal_clock_free(&pdm_pcm_audio_clock); // free the clock object

        clock_set_pdm = true;
    }

    cyhal_clock_free(&pll_clock);
    cyhal_system_delay_ms(1);

}


static void mp_machine_set_freq(size_t n_args, const mp_obj_t *args) {
    freq_peri = mp_obj_get_int(args[0]); // Assuming the enum values are used as integers
    switch (freq_peri) {
        case AUDIO_I2S_90_MHZ:
            PLL0_freq = AUDIO_I2S_90_MHZ;
            audio_i2s_set_frequency(freq_peri);     // i2s audio fz
            break;
        case AUDIO_I2S_98_MHZ:
            PLL0_freq = AUDIO_I2S_98_MHZ;
            audio_i2s_set_frequency(freq_peri);     // i2s audio fz
            break;
        case AUDIO_PDM_22_579_000_HZ:
            PLL0_freq = AUDIO_PDM_22_579_000_HZ;
            audio_pdm_set_frequency(freq_peri);     // pdm audio fz
            break;
        case AUDIO_PDM_24_576_000_HZ:
            PLL0_freq = AUDIO_PDM_24_576_000_HZ;
            audio_pdm_set_frequency(freq_peri);     // pdm audio fz
            break;
        case CM4_FLL:
            cm4_fll_set_frequency(mp_obj_get_int(args[1]));     // core m4 fz sourced by FLL
            break;
        case CM4:
            PLL0_freq = CM4;
            cm4_set_frequency(mp_obj_get_int(args[1]));     // core m4 fz sourced by PLL(default condition)
            break;
        default:
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Invalid frequency type %lu"), freq_peri);
            break;
    }
}


// Sleep Modes Not working. Might be because of the REPL always running in background. Need to evaluate
static void mp_machine_lightsleep(size_t n_args, const mp_obj_t *args) {
    // cy_rslt_t result;
    // if (n_args != 0) {
    //     uint32_t expiry = mp_obj_get_int(args[0]);
    //     cyhal_lptimer_t obj;
    //     uint32_t actual_ms;
    //     result = cyhal_syspm_tickless_sleep(&obj, expiry, &actual_ms);
    //     if (result != CY_RSLT_SUCCESS) {
    //         mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Light sleeep failed %lx !"), result);
    //     }
    // } else {
    //     result = cyhal_syspm_sleep();
    //     if (result != CY_RSLT_SUCCESS) {
    //         mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Light sleeep failed %lx !"), result);
    //     }
    // }
    mp_raise_NotImplementedError(MP_ERROR_TEXT("Not implemented!!!\n"));
}

NORETURN static void mp_machine_deepsleep(size_t n_args, const mp_obj_t *args) {
    // cy_rslt_t result;
    // if (n_args != 0) {
    //     uint32_t expiry = mp_obj_get_int(args[0]);
    //     cyhal_lptimer_t obj;
    //     uint32_t actual_ms;
    //     result = cyhal_syspm_tickless_deepsleep(&obj, expiry, &actual_ms);
    //     if (result != CY_RSLT_SUCCESS) {
    //         mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Deep sleeep failed %lx !"), result);
    //     }
    // } else {
    //     result = cyhal_syspm_deepsleep();
    //     if (result != CY_RSLT_SUCCESS) {
    //         mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Deep sleeep failed %lx !"), result);
    //     }
    // }
    // for (;;)
    // {

    // }
    mp_raise_NotImplementedError(MP_ERROR_TEXT("Not implemented!!!\n"));
}

// machine.idle()
// This executies a wfi machine instruction which reduces power consumption
// of the MCU until an interrupt occurs, at which point execution continues.
// see: https://www.infineon.com/dgdl/Infineon-AN219528_PSoC_6_MCU_low-power_modes_and_power_reduction_techniques-ApplicationNotes-v06_00-EN.pdf?fileId=8ac78c8c7cdc391c017d0d31efdc659f  pg.7
static void mp_machine_idle(void) {
    __WFI(); // standard ARM instruction
}

// machine.unique_id()
static mp_obj_t mp_machine_unique_id(void) {
    uint64_t id = system_get_unique_id();
    byte *id_addr = (byte *)&id;
    // printf("ID_formatted:%02x%02x%02x%02x:%02x%02x%02x%02x\n", id_addr[0], id_addr[1], id_addr[2], id_addr[3], id_addr[4], id_addr[5], id_addr[6], id_addr[7]);
    return mp_obj_new_bytes(id_addr, 8);
}

// machine.reset()
// using watchdog timer to count to minimum value (1ms) to trigger reset
// thread-safe way as other methods might interfere with pending interrupts, threads etc.
NORETURN static void mp_machine_reset(void) {
    cyhal_wdt_t wdt_obj;
    cyhal_wdt_init(&wdt_obj, 1); // min 1ms count time
    cyhal_wdt_start(&wdt_obj);
    while (true) {
    }
    ;
}

// This function is called from MPY side and is for addressing soft reset from micropython side. This does not indicate a system level soft reset.
static mp_int_t mp_machine_reset_cause(void) {
    if (reset_cause == MACHINE_SOFT_RESET) {
        return MACHINE_SOFT_RESET;
    } else {
        mp_obj_t sys_reset = system_reset_cause();
        uint32_t sys_reset_cause = mp_obj_get_int(sys_reset);
        if (sys_reset_cause == SYSTEM_RESET_NONE) {
            reset_cause = MACHINE_PWRON_RESET;
        } else if (sys_reset_cause == SYSTEM_RESET_WDT) {
            reset_cause = MACHINE_WDT_RESET;
        } else if (sys_reset_cause == SYSTEM_RESET_DEEPSLEEP_FAULT) {
            reset_cause = MACHINE_DEEPSLEEP_RESET;
        } else {
            reset_cause = MACHINE_HARD_RESET;
        }
    }
    return reset_cause;
}

// machine.disable_irq()
static mp_obj_t machine_disable_irq(void) {
    uint32_t state = system_disable_global_irq();
    return mp_obj_new_int(state);
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_disable_irq_obj, machine_disable_irq);

// machine.enable_irq()
static mp_obj_t machine_enable_irq(mp_obj_t state_in) {
    uint32_t state = mp_obj_get_int(state_in);
    bool result = system_enable_global_irq(state);
    if (!result) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Interrupt enabling failed!"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_enable_irq_obj, machine_enable_irq);

static mp_obj_t machine_rng(void) {
    uint32_t rnd_num;
    cyhal_trng_t trng_obj;
    /* Initialize the true random number generator block */
    cy_rslt_t rslt = cyhal_trng_init(&trng_obj);
    if (rslt != CY_RSLT_SUCCESS) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Random Number generator failed %lx !"), rslt);
    }
    /* Generate a true random number */
    rnd_num = cyhal_trng_generate(&trng_obj);
    rnd_num &= 0xFFFFFF;
    /* Release the true random number generator block */
    cyhal_trng_free(&trng_obj);
    return mp_obj_new_int(rnd_num);
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_rng_obj, machine_rng);

#ifdef MICROPY_PY_SD_CARD
#define MICROPY_PY_MACHINE_SD_CARD_ENTRY { MP_ROM_QSTR(MP_QSTR_SDCard),              MP_ROM_PTR(&machine_sdcard_type) },
#else
#define MICROPY_PY_MACHINE_SD_CARD_ENTRY
#endif

#if MICROPY_PY_MACHINE_SPI_SLAVE
#define MICROPY_PY_MACHINE_SPI_SLAVE_ENTRY { MP_ROM_QSTR(MP_QSTR_SPISlave),            MP_ROM_PTR(&machine_spi_slave_type) },
#else
#define MICROPY_PY_MACHINE_SPI_SLAVE_ENTRY
#endif

#define MICROPY_PY_MACHINE_EXTRA_GLOBALS \
    { MP_ROM_QSTR(MP_QSTR_info),                MP_ROM_PTR(&machine_info_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_reset_cause),         MP_ROM_PTR(&machine_reset_cause_obj) },  \
    { MP_ROM_QSTR(MP_QSTR_rng),                 MP_ROM_PTR(&machine_rng_obj) },  \
    \
    { MP_ROM_QSTR(MP_QSTR_disable_irq),         MP_ROM_PTR(&machine_disable_irq_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_enable_irq),          MP_ROM_PTR(&machine_enable_irq_obj) }, \
    \
    /* class constants */ \
    { MP_ROM_QSTR(MP_QSTR_HARD_RESET),          MP_ROM_INT(MACHINE_HARD_RESET) },  \
    { MP_ROM_QSTR(MP_QSTR_WDT_RESET),           MP_ROM_INT(MACHINE_WDT_RESET) },  \
    { MP_ROM_QSTR(MP_QSTR_DEEPSLEEP_RESET),     MP_ROM_INT(MACHINE_DEEPSLEEP_RESET) }, \
    { MP_ROM_QSTR(MP_QSTR_SOFT_RESET),          MP_ROM_INT(MACHINE_SOFT_RESET) }, \
    { MP_ROM_QSTR(MP_QSTR_PWRON_RESET),         MP_ROM_INT(MACHINE_PWRON_RESET) }, \
    { MP_ROM_QSTR(MP_QSTR_AUDIO_I2S_98_MHZ),    MP_ROM_INT(AUDIO_I2S_98_MHZ) }, \
    { MP_ROM_QSTR(MP_QSTR_AUDIO_I2S_90_MHZ),    MP_ROM_INT(AUDIO_I2S_90_MHZ) }, \
    { MP_ROM_QSTR(MP_QSTR_AUDIO_PDM_24_576_000_HZ),    MP_ROM_INT(AUDIO_PDM_24_576_000_HZ) }, \
    { MP_ROM_QSTR(MP_QSTR_AUDIO_PDM_22_579_000_HZ),    MP_ROM_INT(AUDIO_PDM_22_579_000_HZ) }, \
    { MP_ROM_QSTR(MP_QSTR_CM4),                 MP_ROM_INT(CM4) }, \
    { MP_ROM_QSTR(MP_QSTR_CM4_FLL),             MP_ROM_INT(CM4_FLL) }, \
    \
    /* Modules */ \
    { MP_ROM_QSTR(MP_QSTR_I2CSlave),            MP_ROM_PTR(&machine_i2c_slave_type) }, \
    { MP_ROM_QSTR(MP_QSTR_PDM_PCM),             MP_ROM_PTR(&machine_pdm_pcm_type) }, \
    { MP_ROM_QSTR(MP_QSTR_Pin),                 MP_ROM_PTR(&machine_pin_type) }, \
    { MP_ROM_QSTR(MP_QSTR_RTC),                 MP_ROM_PTR(&machine_rtc_type) }, \
    \
    { MP_ROM_QSTR(MP_QSTR_Timer),               MP_ROM_PTR(&machine_timer_type) }, \
    { MP_ROM_QSTR(MP_QSTR_ADC),                 MP_ROM_PTR(&machine_adc_type) }, \
    { MP_ROM_QSTR(MP_QSTR_ADCBlock),            MP_ROM_PTR(&machine_adcblock_type) }, \
    MICROPY_PY_MACHINE_SPI_SLAVE_ENTRY \
    MICROPY_PY_MACHINE_SD_CARD_ENTRY \

#endif // MICROPY_PY_MACHINE
