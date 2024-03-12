/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2023 Damien P. George
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

// This file is never compiled standalone, it's included directly from
// extmod/modmachine.c via MICROPY_PY_MACHINE_INCLUDEFILE.

#include <string.h>
#include "modmachine.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "lib/oofatfs/ff.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "gccollect.h"
#include "irq.h"
#include "powerctrl.h"
#include "boardctrl.h"
#include "pybthread.h"
#include "rng.h"
#include "storage.h"
#include "pin.h"
#include "timer.h"
#include "usb.h"
#include "rtc.h"
#include "i2c.h"
#include "spi.h"

#if defined(STM32G0)
// G0 has BOR and POR combined
#define RCC_CSR_BORRSTF RCC_CSR_PWRRSTF
#define RCC_CSR_PORRSTF RCC_CSR_PWRRSTF
#endif

#if defined(STM32H5)
#define RCC_SR          RSR
#define RCC_SR_IWDGRSTF RCC_RSR_IWDGRSTF
#define RCC_SR_WWDGRSTF RCC_RSR_WWDGRSTF
#define RCC_SR_BORRSTF  RCC_RSR_BORRSTF
#define RCC_SR_PINRSTF  RCC_RSR_PINRSTF
#define RCC_SR_RMVF     RCC_RSR_RMVF
#elif defined(STM32H7)
#define RCC_SR          RSR
#define RCC_SR_IWDGRSTF RCC_RSR_IWDG1RSTF
#define RCC_SR_WWDGRSTF RCC_RSR_WWDG1RSTF
#define RCC_SR_PORRSTF  RCC_RSR_PORRSTF
#define RCC_SR_BORRSTF  RCC_RSR_BORRSTF
#define RCC_SR_PINRSTF  RCC_RSR_PINRSTF
#define RCC_SR_RMVF     RCC_RSR_RMVF
#else
#define RCC_SR          CSR
#define RCC_SR_IWDGRSTF RCC_CSR_IWDGRSTF
#define RCC_SR_WWDGRSTF RCC_CSR_WWDGRSTF
#if defined(RCC_CSR_PORRSTF)
#define RCC_SR_PORRSTF  RCC_CSR_PORRSTF
#endif
#if defined(RCC_CSR_BORRSTF)
#define RCC_SR_BORRSTF  RCC_CSR_BORRSTF
#endif
#define RCC_SR_PINRSTF  RCC_CSR_PINRSTF
#define RCC_SR_RMVF     RCC_CSR_RMVF
#endif

#define PYB_RESET_SOFT      (0)
#define PYB_RESET_POWER_ON  (1)
#define PYB_RESET_HARD      (2)
#define PYB_RESET_WDT       (3)
#define PYB_RESET_DEEPSLEEP (4)

#if MICROPY_HW_ENABLE_RNG
#define MICROPY_PY_MACHINE_RNG_ENTRY { MP_ROM_QSTR(MP_QSTR_rng), MP_ROM_PTR(&pyb_rng_get_obj) },
#else
#define MICROPY_PY_MACHINE_RNG_ENTRY
#endif

#define MICROPY_PY_MACHINE_EXTRA_GLOBALS \
    { MP_ROM_QSTR(MP_QSTR_info),                MP_ROM_PTR(&machine_info_obj) }, \
    MICROPY_PY_MACHINE_RNG_ENTRY \
    { MP_ROM_QSTR(MP_QSTR_sleep),               MP_ROM_PTR(&machine_lightsleep_obj) }, \
    \
    { MP_ROM_QSTR(MP_QSTR_disable_irq),         MP_ROM_PTR(&machine_disable_irq_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_enable_irq),          MP_ROM_PTR(&machine_enable_irq_obj) }, \
    \
    { MP_ROM_QSTR(MP_QSTR_Pin),                 MP_ROM_PTR(&pin_type) }, \
    \
    { MP_ROM_QSTR(MP_QSTR_RTC),                 MP_ROM_PTR(&pyb_rtc_type) }, \
    { MP_ROM_QSTR(MP_QSTR_Timer),               MP_ROM_PTR(&machine_timer_type) }, \
    \
    { MP_ROM_QSTR(MP_QSTR_PWRON_RESET),         MP_ROM_INT(PYB_RESET_POWER_ON) }, \
    { MP_ROM_QSTR(MP_QSTR_HARD_RESET),          MP_ROM_INT(PYB_RESET_HARD) }, \
    { MP_ROM_QSTR(MP_QSTR_WDT_RESET),           MP_ROM_INT(PYB_RESET_WDT) }, \
    { MP_ROM_QSTR(MP_QSTR_DEEPSLEEP_RESET),     MP_ROM_INT(PYB_RESET_DEEPSLEEP) }, \
    { MP_ROM_QSTR(MP_QSTR_SOFT_RESET),          MP_ROM_INT(PYB_RESET_SOFT) }, \

static uint32_t reset_cause;

void machine_init(void) {
    #if defined(STM32F4)
    if (PWR->CSR & PWR_CSR_SBF) {
        // came out of standby
        reset_cause = PYB_RESET_DEEPSLEEP;
        PWR->CR |= PWR_CR_CSBF;
    } else
    #elif defined(STM32F7)
    if (PWR->CSR1 & PWR_CSR1_SBF) {
        // came out of standby
        reset_cause = PYB_RESET_DEEPSLEEP;
        PWR->CR1 |= PWR_CR1_CSBF;
    } else
    #elif defined(STM32H5)
    if (PWR->PMSR & PWR_PMSR_STOPF || PWR->PMSR & PWR_PMSR_SBF) {
        // came out of standby or stop mode
        reset_cause = PYB_RESET_DEEPSLEEP;
        PWR->PMCR |= PWR_PMCR_CSSF;
    } else
    #elif defined(STM32H7)
    if (PWR->CPUCR & PWR_CPUCR_SBF || PWR->CPUCR & PWR_CPUCR_STOPF) {
        // came out of standby or stop mode
        reset_cause = PYB_RESET_DEEPSLEEP;
        PWR->CPUCR |= PWR_CPUCR_CSSF;
    } else
    #elif defined(STM32L4)
    if (PWR->SR1 & PWR_SR1_SBF) {
        // came out of standby
        reset_cause = PYB_RESET_DEEPSLEEP;
        PWR->SCR |= PWR_SCR_CSBF;
    } else
    #elif defined(STM32WB)
    if (PWR->EXTSCR & PWR_EXTSCR_C1SBF) {
        // came out of standby
        reset_cause = PYB_RESET_DEEPSLEEP;
        PWR->EXTSCR |= PWR_EXTSCR_C1CSSF;
    } else
    #endif
    {
        // get reset cause from RCC flags
        uint32_t state = RCC->RCC_SR;
        if (state & RCC_SR_IWDGRSTF || state & RCC_SR_WWDGRSTF) {
            reset_cause = PYB_RESET_WDT;
        } else if (0
                   #if defined(RCC_SR_PORRSTF)
                   || (state & RCC_SR_PORRSTF)
                   #endif
                   #if defined(RCC_SR_BORRSTF)
                   || (state & RCC_SR_BORRSTF)
                   #endif
                   ) {
            reset_cause = PYB_RESET_POWER_ON;
        } else if (state & RCC_SR_PINRSTF) {
            reset_cause = PYB_RESET_HARD;
        } else {
            // default is soft reset
            reset_cause = PYB_RESET_SOFT;
        }
    }
    // clear RCC reset flags
    RCC->RCC_SR |= RCC_SR_RMVF;
}

void machine_deinit(void) {
    // we are doing a soft-reset so change the reset_cause
    reset_cause = PYB_RESET_SOFT;
}

// machine.info([dump_alloc_table])
// Print out lots of information about the board.
static mp_obj_t machine_info(size_t n_args, const mp_obj_t *args) {
    const mp_print_t *print = &mp_plat_print;

    // get and print unique id; 96 bits
    {
        byte *id = (byte *)MP_HAL_UNIQUE_ID_ADDRESS;
        mp_printf(print, "ID=%02x%02x%02x%02x:%02x%02x%02x%02x:%02x%02x%02x%02x\n", id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7], id[8], id[9], id[10], id[11]);
    }

    mp_printf(print, "DEVID=0x%04x\nREVID=0x%04x\n", (unsigned int)HAL_GetDEVID(), (unsigned int)HAL_GetREVID());

    // get and print clock speeds
    // SYSCLK=168MHz, HCLK=168MHz, PCLK1=42MHz, PCLK2=84MHz
    {
        #if defined(STM32F0) || defined(STM32G0)
        mp_printf(print, "S=%u\nH=%u\nP1=%u\n",
            (unsigned int)HAL_RCC_GetSysClockFreq(),
            (unsigned int)HAL_RCC_GetHCLKFreq(),
            (unsigned int)HAL_RCC_GetPCLK1Freq());
        #else
        mp_printf(print, "S=%u\nH=%u\nP1=%u\nP2=%u\n",
            (unsigned int)HAL_RCC_GetSysClockFreq(),
            (unsigned int)HAL_RCC_GetHCLKFreq(),
            (unsigned int)HAL_RCC_GetPCLK1Freq(),
            (unsigned int)HAL_RCC_GetPCLK2Freq());
        #endif
    }

    // to print info about memory
    {
        mp_printf(print, "_etext=%p\n", &_etext);
        mp_printf(print, "_sidata=%p\n", &_sidata);
        mp_printf(print, "_sdata=%p\n", &_sdata);
        mp_printf(print, "_edata=%p\n", &_edata);
        mp_printf(print, "_sbss=%p\n", &_sbss);
        mp_printf(print, "_ebss=%p\n", &_ebss);
        mp_printf(print, "_sstack=%p\n", &_sstack);
        mp_printf(print, "_estack=%p\n", &_estack);
        mp_printf(print, "_ram_start=%p\n", &_ram_start);
        mp_printf(print, "_heap_start=%p\n", &_heap_start);
        mp_printf(print, "_heap_end=%p\n", &_heap_end);
        mp_printf(print, "_ram_end=%p\n", &_ram_end);
    }

    // qstr info
    {
        size_t n_pool, n_qstr, n_str_data_bytes, n_total_bytes;
        qstr_pool_info(&n_pool, &n_qstr, &n_str_data_bytes, &n_total_bytes);
        mp_printf(print, "qstr:\n  n_pool=%u\n  n_qstr=%u\n  n_str_data_bytes=%u\n  n_total_bytes=%u\n", n_pool, n_qstr, n_str_data_bytes, n_total_bytes);
    }

    // GC info
    {
        gc_info_t info;
        gc_info(&info);
        mp_printf(print, "GC:\n");
        mp_printf(print, "  %u total\n", info.total);
        mp_printf(print, "  %u : %u\n", info.used, info.free);
        mp_printf(print, "  1=%u 2=%u m=%u\n", info.num_1block, info.num_2block, info.max_block);
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
                mp_printf(print, "LFS free: %u bytes\n", (uint)(nclst * vfs_fat->fatfs.csize * 512));
                break;
            }
        }
        #endif
    }

    #if MICROPY_PY_THREAD
    pyb_thread_dump(print);
    #endif

    if (n_args == 1) {
        // arg given means dump gc allocation table
        gc_dump_alloc_table(print);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_info_obj, 0, 1, machine_info);

// Returns a string of 12 bytes (96 bits), which is the unique ID for the MCU.
static mp_obj_t mp_machine_unique_id(void) {
    byte *id = (byte *)MP_HAL_UNIQUE_ID_ADDRESS;
    return mp_obj_new_bytes(id, 12);
}

// Resets the pyboard in a manner similar to pushing the external RESET button.
NORETURN static void mp_machine_reset(void) {
    powerctrl_mcu_reset();
}

// Activate the bootloader without BOOT* pins.
NORETURN void mp_machine_bootloader(size_t n_args, const mp_obj_t *args) {
    #if MICROPY_HW_ENABLE_USB
    pyb_usb_dev_deinit();
    #endif
    #if MICROPY_HW_ENABLE_STORAGE
    storage_flush();
    #endif

    __disable_irq();

    MICROPY_BOARD_ENTER_BOOTLOADER(n_args, args);

    #if defined(STM32F7) || defined(STM32H7)
    powerctrl_enter_bootloader(0, 0x1ff00000);
    #elif defined(STM32H5)
    powerctrl_enter_bootloader(0, 0x0bf97000);
    #else
    powerctrl_enter_bootloader(0, 0x00000000);
    #endif

    while (1) {
        ;
    }
}

// get or set the MCU frequencies
static mp_obj_t mp_machine_get_freq(void) {
    mp_obj_t tuple[] = {
        mp_obj_new_int(HAL_RCC_GetSysClockFreq()),
        mp_obj_new_int(HAL_RCC_GetHCLKFreq()),
        mp_obj_new_int(HAL_RCC_GetPCLK1Freq()),
        #if !defined(STM32F0) && !defined(STM32G0)
        mp_obj_new_int(HAL_RCC_GetPCLK2Freq()),
        #endif
    };
    return mp_obj_new_tuple(MP_ARRAY_SIZE(tuple), tuple);
}

static void mp_machine_set_freq(size_t n_args, const mp_obj_t *args) {
    #if defined(STM32F0) || defined(STM32L0) || defined(STM32L1) || defined(STM32L4) || defined(STM32G0)
    mp_raise_NotImplementedError(MP_ERROR_TEXT("machine.freq set not supported yet"));
    #else
    mp_int_t sysclk = mp_obj_get_int(args[0]);
    mp_int_t ahb = sysclk;
    #if defined(STM32H7)
    if (ahb > 200000000) {
        ahb /= 2;
    }
    #endif
    #if defined(STM32WB)
    mp_int_t apb1 = ahb;
    mp_int_t apb2 = ahb;
    #else
    mp_int_t apb1 = ahb / 4;
    mp_int_t apb2 = ahb / 2;
    #endif
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
        mp_raise_ValueError(MP_ERROR_TEXT("invalid freq"));
    } else if (ret < 0) {
        MICROPY_BOARD_FATAL_ERROR("can't change freq");
    }
    #endif
}

// idle()
// This executies a wfi machine instruction which reduces power consumption
// of the MCU until an interrupt occurs, at which point execution continues.
static void mp_machine_idle(void) {
    __WFI();
}

static void mp_machine_lightsleep(size_t n_args, const mp_obj_t *args) {
    if (n_args != 0) {
        mp_obj_t args2[2] = {MP_OBJ_NULL, args[0]};
        pyb_rtc_wakeup(2, args2);
    }
    powerctrl_enter_stop_mode();
}

static void mp_machine_deepsleep(size_t n_args, const mp_obj_t *args) {
    if (n_args != 0) {
        mp_obj_t args2[2] = {MP_OBJ_NULL, args[0]};
        pyb_rtc_wakeup(2, args2);
    }
    powerctrl_enter_standby_mode();
}

static mp_int_t mp_machine_reset_cause(void) {
    return reset_cause;
}
