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
#include "py/objstr.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "extmod/machine_mem.h"
#include "extmod/machine_signal.h"
#include "extmod/machine_pulse.h"
#include "extmod/machine_i2c.h"
#include "lib/utils/pyexec.h"
#include "lib/oofatfs/ff.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "extint.h"
#include "gccollect.h"
#include "irq.h"
#include "powerctrl.h"
#include "pybthread.h"
#include "storage.h"
#include "pin.h"
#include "pin_remap.h"
#include "timer.h"
#include "usb.h"
#include "rtc.h"
#include "i2c.h"
#include "spi.h"
#include "sdcard.h"
#include "uart.h"
#include "wdt.h"
#include "can.h"
#include "adc.h"
#include "dac.h"

// 复位来源定义
#define PYB_RESET_SOFT      (0) // 软件复位
#define PYB_RESET_POWER_ON  (1) // 上电/掉电复位
#define PYB_RESET_HARD      (2) // 通过复位引脚NRST复位
#define PYB_RESET_WDT       (3) // 看门狗复位
#define PYB_RESET_DEEPSLEEP (4) // 待机模式复位

STATIC uint32_t reset_cause;

void machine_init(void) {
    if (BIT_BAND(PWR->CSR, PWR_CSR_SBF_Pos)) { // 待机模式
        reset_cause = PYB_RESET_DEEPSLEEP;
        // PWR->CR |= PWR_CR_CSBF;   
        BIT_BAND_SET(PWR->CR, PWR_CR_CSBF_Pos); // 清除待机标识
    } else {
        // 从RCC里获取复位原因
        uint32_t state = RCC->CSR;
        if (state & RCC_CSR_IWDGRSTF || state & RCC_CSR_WWDGRSTF) {
            reset_cause = PYB_RESET_WDT;
        } else if (state & RCC_CSR_PORRSTF) {
            reset_cause = PYB_RESET_POWER_ON;
        } else if (state & RCC_CSR_PINRSTF) {
            reset_cause = PYB_RESET_HARD;
        } else {
            // default is soft reset
            reset_cause = PYB_RESET_SOFT;
        }
    }
    // clear RCC reset flags
    // RCC->CSR |= RCC_CSR_RMVF;
    BIT_BAND_SET(RCC->CSR, RCC_CSR_RMVF_Pos); // 清除待机标识
}

void machine_deinit(void) {
    // we are doing a soft-reset so change the reset_cause
    reset_cause = PYB_RESET_SOFT;
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
    // SYSCLK=72MHz, HCLK=72MHz, PCLK1=36MHz, PCLK2=72MHz
    {
        printf("S=%u\nH=%u\nP1=%u\nP2=%u\n",
               (unsigned int)HAL_RCC_GetSysClockFreq(),
               (unsigned int)HAL_RCC_GetHCLKFreq(),
               (unsigned int)HAL_RCC_GetPCLK1Freq(),
               (unsigned int)HAL_RCC_GetPCLK2Freq());
    }

    // to print info about memory
    {
        printf("_etext=%p\n", &_etext);
        printf("_sidata=%p\n", &_sidata);
        printf("_sdata=%p\n", &_sdata);
        printf("_edata=%p\n", &_edata);
        printf("_sbss=%p\n", &_sbss);
        printf("_ebss=%p\n", &_ebss);
        printf("_sstack=%p\n", &_sstack);
        printf("_estack=%p\n", &_estack);
        printf("_ram_start=%p\n", &_ram_start);
        printf("_heap_start=%p\n", &_heap_start);
        printf("_heap_end=%p\n", &_heap_end);
        printf("_ram_end=%p\n", &_ram_end);
    }

    // qstr info
    {
        size_t n_pool, n_qstr, n_str_data_bytes, n_total_bytes;
        qstr_pool_info(&n_pool, &n_qstr, &n_str_data_bytes, &n_total_bytes);
        printf("qstr:\n  n_pool=%u\n  n_qstr=%u\n  n_str_data_bytes=%u\n  n_total_bytes=%u\n", n_pool, n_qstr, n_str_data_bytes, n_total_bytes);
    }

    // GC info
    {
        gc_info_t info;
        gc_info(&info);
        printf("GC:\n");
        printf("  %u total\n", info.total);
        printf("  %u : %u\n", info.used, info.free);
        printf("  1=%u 2=%u m=%u\n", info.num_1block, info.num_2block, info.max_block);
    }

    // free space on flash
    {
        #if MICROPY_VFS_FAT
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
        #endif
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
    powerctrl_mcu_reset();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_obj, machine_reset);

STATIC mp_obj_t machine_soft_reset(void) {
    pyexec_system_exit = PYEXEC_FORCED_EXIT;
    nlr_raise(mp_obj_new_exception(&mp_type_SystemExit));
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_soft_reset_obj, machine_soft_reset);

// Activate the bootloader without BOOT* pins.
STATIC NORETURN mp_obj_t machine_bootloader(size_t n_args, const mp_obj_t *args) {
    #if MICROPY_HW_ENABLE_USB
    pyb_usb_dev_deinit();
    #endif

    #if MICROPY_HW_ENABLE_STORAGE
    storage_flush();
    #endif

    #if MICROPY_HW_USES_BOOTLOADER
    if (n_args == 0 || !mp_obj_is_true(args[0])) {
        // By default, with no args given, we enter the custom bootloader (mboot)
        powerctrl_enter_bootloader(0x70ad0000, 0x08000000);
    }

    if (n_args == 1 && mp_obj_is_str_or_bytes(args[0])) {
        // With a string/bytes given, pass its data to the custom bootloader
        size_t len;
        const char *data = mp_obj_str_get_data(args[0], &len);
        void *mboot_region = (void*)*((volatile uint32_t*)0x08000000);
        memmove(mboot_region, data, len);
        powerctrl_enter_bootloader(0x70ad0080, 0x08000000);
    }
    #endif

    powerctrl_enter_bootloader(0, 0x00000000);

    while (1);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_bootloader_obj, 0, 1, machine_bootloader);

// get or set the MCU frequencies
STATIC mp_obj_t machine_freq(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        // get
        mp_obj_t tuple[] = {
           mp_obj_new_int(HAL_RCC_GetSysClockFreq()),
           mp_obj_new_int(HAL_RCC_GetHCLKFreq()),
           mp_obj_new_int(HAL_RCC_GetPCLK1Freq()),
           mp_obj_new_int(HAL_RCC_GetPCLK2Freq()),
        };
        return mp_obj_new_tuple(MP_ARRAY_SIZE(tuple), tuple);
    } else {
        // set
        mp_int_t sysclk = mp_obj_get_int(args[0]);
        mp_int_t ahb = sysclk;
        mp_int_t apb1 = ahb / 2; // max 36MHz
        mp_int_t apb2 = ahb / 1; // max 72MHz
        if (n_args > 1) {
            ahb = mp_obj_get_int(args[1]);
            if (n_args > 2) {
                apb1 = mp_obj_get_int(args[2]);
                if (n_args > 3) {
                    apb2 = mp_obj_get_int(args[3]);
                }
            }
        }

        int ret = powerctrl_set_sysclk(sysclk, ahb, apb1, apb2);
        if (ret == -MP_EINVAL) {
            mp_raise_ValueError("invalid freq");
        } else if (ret < 0) {
            void NORETURN __fatal_error(const char *msg);
            __fatal_error("can't change freq");
        }
        return mp_const_none;
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_freq_obj, 0, 4, machine_freq);

STATIC mp_obj_t machine_lightsleep(size_t n_args, const mp_obj_t *args) {
    if (n_args != 0) {
        mp_obj_t args2[2] = {MP_OBJ_NULL, args[0]};
        pyb_rtc_wakeup(2, args2);
    }
    powerctrl_enter_stop_mode();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_lightsleep_obj, 0, 1, machine_lightsleep);

STATIC mp_obj_t machine_deepsleep(size_t n_args, const mp_obj_t *args) {
    if (n_args != 0) {
        mp_obj_t args2[2] = {MP_OBJ_NULL, args[0]};
        pyb_rtc_wakeup(2, args2);
    }
    powerctrl_enter_standby_mode();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_deepsleep_obj, 0, 1, machine_deepsleep);

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
    
    { MP_ROM_QSTR(MP_QSTR_idle),                MP_ROM_PTR(&pyb_wfi_obj) },
    { MP_ROM_QSTR(MP_QSTR_lightsleep),          MP_ROM_PTR(&machine_lightsleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_deepsleep),           MP_ROM_PTR(&machine_deepsleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_cause),         MP_ROM_PTR(&machine_reset_cause_obj) },

    { MP_ROM_QSTR(MP_QSTR_disable_irq),         MP_ROM_PTR(&pyb_disable_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_irq),          MP_ROM_PTR(&pyb_enable_irq_obj) },

    { MP_ROM_QSTR(MP_QSTR_time_pulse_us),       MP_ROM_PTR(&machine_time_pulse_us_obj) },

    { MP_ROM_QSTR(MP_QSTR_mem8),                MP_ROM_PTR(&machine_mem8_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem16),               MP_ROM_PTR(&machine_mem16_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem32),               MP_ROM_PTR(&machine_mem32_obj) },

    { MP_ROM_QSTR(MP_QSTR_Pin),                 MP_ROM_PTR(&pin_type) },
    { MP_ROM_QSTR(MP_QSTR_PinRemap),            MP_ROM_PTR(&pin_remap_type) },
    { MP_ROM_QSTR(MP_QSTR_ExtInt),              MP_ROM_PTR(&extint_type) },
    { MP_ROM_QSTR(MP_QSTR_Signal),              MP_ROM_PTR(&machine_signal_type) },
    { MP_ROM_QSTR(MP_QSTR_Timer),               MP_ROM_PTR(&pyb_timer_type) },

#if MICROPY_HW_ENABLE_RTC
    { MP_ROM_QSTR(MP_QSTR_RTC), MP_ROM_PTR(&pyb_rtc_type) },
#endif

#if MICROPY_PY_MACHINE_I2C
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&machine_i2c_type) },
#endif

#if MICROPY_HW_ENABLE_CAN
    { MP_ROM_QSTR(MP_QSTR_CAN), MP_ROM_PTR(&pyb_can_type) },
#endif

#if MICROPY_HW_ENABLE_ADC
    { MP_ROM_QSTR(MP_QSTR_ADC), MP_ROM_PTR(&pyb_adc_type) },
    { MP_ROM_QSTR(MP_QSTR_ADCAll), MP_ROM_PTR(&pyb_adc_all_type) },
#endif

#if MICROPY_HW_ENABLE_DAC
    { MP_ROM_QSTR(MP_QSTR_DAC), MP_ROM_PTR(&pyb_dac_type) },
#endif

#if MICROPY_HW_HAS_FLASH
    { MP_ROM_QSTR(MP_QSTR_Flash), MP_ROM_PTR(&pyb_flash_type) },
#endif

#if MICROPY_HW_ENABLE_SDCARD
    { MP_ROM_QSTR(MP_QSTR_SDCard), MP_ROM_PTR(&pyb_sdcard_type) },
#endif

    { MP_ROM_QSTR(MP_QSTR_SPI),                 MP_ROM_PTR(&machine_hard_spi_type) },
    { MP_ROM_QSTR(MP_QSTR_UART),                MP_ROM_PTR(&machine_uart_type) },
    { MP_ROM_QSTR(MP_QSTR_WDT),                 MP_ROM_PTR(&pyb_wdt_type) },

    { MP_ROM_QSTR(MP_QSTR_PWRON_RESET),         MP_ROM_INT(PYB_RESET_POWER_ON) },
    { MP_ROM_QSTR(MP_QSTR_HARD_RESET),          MP_ROM_INT(PYB_RESET_HARD) },
    { MP_ROM_QSTR(MP_QSTR_WDT_RESET),           MP_ROM_INT(PYB_RESET_WDT) },
    { MP_ROM_QSTR(MP_QSTR_DEEPSLEEP_RESET),     MP_ROM_INT(PYB_RESET_DEEPSLEEP) },
    { MP_ROM_QSTR(MP_QSTR_SOFT_RESET),          MP_ROM_INT(PYB_RESET_SOFT) },
};

STATIC MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t machine_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&machine_module_globals,
};
