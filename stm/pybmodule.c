#include <stdint.h>
#include <stdio.h>

#include <stm32f4xx.h>
#include <stm32f4xx_rcc.h>

#include "misc.h"
#include "ff.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "gc.h"
#include "gccollect.h"
#include "systick.h"
#include "rtc.h"
#include "pyexec.h"
#include "servo.h"
#include "storage.h"
#include "usb.h"
#include "usrsw.h"
#include "sdcard.h"
#include "accel.h"
#include "led.h"
#include "i2c.h"
#include "usart.h"
#include "adc.h"
#include "audio.h"
#include "pin.h"
#include "gpio.h"
#include "exti.h"
#include "pybmodule.h"

// get lots of info about the board
STATIC mp_obj_t pyb_info(void) {
    // get and print unique id; 96 bits
    {
        byte *id = (byte*)0x1fff7a10;
        printf("ID=%02x%02x%02x%02x:%02x%02x%02x%02x:%02x%02x%02x%02x\n", id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7], id[8], id[9], id[10], id[11]);
    }

    // get and print clock speeds
    // SYSCLK=168MHz, HCLK=168MHz, PCLK1=42MHz, PCLK2=84MHz
    {
        RCC_ClocksTypeDef rcc_clocks;
        RCC_GetClocksFreq(&rcc_clocks);
        printf("S=%lu\nH=%lu\nP1=%lu\nP2=%lu\n", rcc_clocks.SYSCLK_Frequency, rcc_clocks.HCLK_Frequency, rcc_clocks.PCLK1_Frequency, rcc_clocks.PCLK2_Frequency);
    }

    // to print info about memory
    {
        printf("_text_end=%p\n", &_text_end);
        printf("_data_start_init=%p\n", &_data_start_init);
        printf("_data_start=%p\n", &_data_start);
        printf("_data_end=%p\n", &_data_end);
        printf("_bss_start=%p\n", &_bss_start);
        printf("_bss_end=%p\n", &_bss_end);
        printf("_stack_end=%p\n", &_stack_end);
        printf("_ram_start=%p\n", &_ram_start);
        printf("_heap_start=%p\n", &_heap_start);
        printf("_heap_end=%p\n", &_heap_end);
        printf("_ram_end=%p\n", &_ram_end);
    }

    // qstr info
    {
        uint n_pool, n_qstr, n_str_data_bytes, n_total_bytes;
        qstr_pool_info(&n_pool, &n_qstr, &n_str_data_bytes, &n_total_bytes);
        printf("qstr:\n  n_pool=%u\n  n_qstr=%u\n  n_str_data_bytes=%u\n  n_total_bytes=%u\n", n_pool, n_qstr, n_str_data_bytes, n_total_bytes);
    }

    // GC info
    {
        gc_info_t info;
        gc_info(&info);
        printf("GC:\n");
        printf("  %lu total\n", info.total);
        printf("  %lu : %lu\n", info.used, info.free);
        printf("  1=%lu 2=%lu m=%lu\n", info.num_1block, info.num_2block, info.max_block);
    }

    // free space on flash
    {
        DWORD nclst;
        FATFS *fatfs;
        f_getfree("0:", &nclst, &fatfs);
        printf("LFS free: %u bytes\n", (uint)(nclst * fatfs->csize * 512));
    }

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_info_obj, pyb_info);

// sync all file systems
STATIC mp_obj_t pyb_sync(void) {
    storage_flush();
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_sync_obj, pyb_sync);

STATIC mp_obj_t pyb_millis(void) {
    return mp_obj_new_int(sys_tick_counter);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_millis_obj, pyb_millis);

STATIC mp_obj_t pyb_delay(mp_obj_t count) {
    sys_tick_delay_ms(mp_obj_get_int(count));
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_delay_obj, pyb_delay);

STATIC mp_obj_t pyb_udelay(mp_obj_t usec) {
    uint32_t count = 0;
    const uint32_t utime = (168 * mp_obj_get_int(usec) / 5);
    for (;;) {
        if (++count > utime) {
            return mp_const_none;
        }
    }
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_udelay_obj, pyb_udelay);

STATIC mp_obj_t pyb_rng_get(void) {
    return mp_obj_new_int(RNG_GetRandomNumber() >> 16);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_rng_get_obj, pyb_rng_get);

STATIC void SYSCLKConfig_STOP(void) {
    /* After wake-up from STOP reconfigure the system clock */
    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);

    /* Wait till HSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET) {
    }

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x08) {
    }
}

STATIC mp_obj_t pyb_stop(void) {
    PWR_EnterSTANDBYMode();
    //PWR_FlashPowerDownCmd(ENABLE); don't know what the logic is with this

    /* Enter Stop Mode */
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

    /* Configures system clock after wake-up from STOP: enable HSE, PLL and select 
     *        PLL as system clock source (HSE and PLL are disabled in STOP mode) */
    SYSCLKConfig_STOP();

    //PWR_FlashPowerDownCmd(DISABLE);

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_0(pyb_stop_obj, pyb_stop);

STATIC mp_obj_t pyb_standby(void) {
    PWR_EnterSTANDBYMode();
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_0(pyb_standby_obj, pyb_standby);

STATIC mp_obj_t pyb_hid_send_report(mp_obj_t arg) {
    mp_obj_t *items;
    mp_obj_get_array_fixed_n(arg, 4, &items);
    uint8_t data[4];
    data[0] = mp_obj_get_int(items[0]);
    data[1] = mp_obj_get_int(items[1]);
    data[2] = mp_obj_get_int(items[2]);
    data[3] = mp_obj_get_int(items[3]);
    usb_hid_send_report(data);
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(pyb_hid_send_report_obj, pyb_hid_send_report);

MP_DEFINE_CONST_FUN_OBJ_2(pyb_I2C_obj, pyb_I2C); // TODO put this in i2c.c

MP_DECLARE_CONST_FUN_OBJ(pyb_source_dir_obj); // defined in main.c
MP_DECLARE_CONST_FUN_OBJ(pyb_main_obj); // defined in main.c

STATIC const mp_map_elem_t pyb_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_pyb) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_info), (mp_obj_t)&pyb_info_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_gc), (mp_obj_t)&pyb_gc_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_repl_info), (mp_obj_t)&pyb_set_repl_info_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_stop), (mp_obj_t)&pyb_stop_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_standby), (mp_obj_t)&pyb_standby_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_source_dir), (mp_obj_t)&pyb_source_dir_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_main), (mp_obj_t)&pyb_main_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_millis), (mp_obj_t)&pyb_millis_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_delay), (mp_obj_t)&pyb_delay_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_udelay), (mp_obj_t)&pyb_udelay_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sync), (mp_obj_t)&pyb_sync_obj },

#if MICROPY_HW_ENABLE_RNG
    { MP_OBJ_NEW_QSTR(MP_QSTR_rand), (mp_obj_t)&pyb_rng_get_obj },
#endif

#if MICROPY_HW_ENABLE_RTC
    { MP_OBJ_NEW_QSTR(MP_QSTR_time), (mp_obj_t)&pyb_rtc_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_rtc_info), (mp_obj_t)&pyb_rtc_info_obj },
#endif

#if MICROPY_HW_ENABLE_SERVO
    { MP_OBJ_NEW_QSTR(MP_QSTR_pwm), (mp_obj_t)&pyb_pwm_set_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_servo), (mp_obj_t)&pyb_servo_set_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Servo), (mp_obj_t)&pyb_Servo_obj },
#endif

#if MICROPY_HW_HAS_SWITCH
    { MP_OBJ_NEW_QSTR(MP_QSTR_switch), (mp_obj_t)&pyb_switch_obj },
#endif

#if MICROPY_HW_HAS_SDCARD
    { MP_OBJ_NEW_QSTR(MP_QSTR_SD), (mp_obj_t)&pyb_sdcard_obj },
#endif

#if MICROPY_HW_HAS_MMA7660
    { MP_OBJ_NEW_QSTR(MP_QSTR_accel), (mp_obj_t)&pyb_accel_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_accel_read), (mp_obj_t)&pyb_accel_read_all_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_accel_mode), (mp_obj_t)&pyb_accel_write_mode_obj },
#endif

    { MP_OBJ_NEW_QSTR(MP_QSTR_hid), (mp_obj_t)&pyb_hid_send_report_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_Led), (mp_obj_t)&pyb_Led_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_I2C), (mp_obj_t)&pyb_I2C_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Usart), (mp_obj_t)&pyb_Usart_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ADC_all), (mp_obj_t)&pyb_ADC_all_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ADC), (mp_obj_t)&pyb_ADC_obj },

#if MICROPY_HW_ENABLE_AUDIO
    { MP_OBJ_NEW_QSTR(MP_QSTR_Audio), (mp_obj_t)&pyb_Audio_obj },
#endif

    // pin mapper
    { MP_OBJ_NEW_QSTR(MP_QSTR_Pin), (mp_obj_t)&pin_map_obj },

    // GPIO bindings
    { MP_OBJ_NEW_QSTR(MP_QSTR_gpio), (mp_obj_t)&pyb_gpio_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_gpio_in), (mp_obj_t)&pyb_gpio_input_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_gpio_out), (mp_obj_t)&pyb_gpio_output_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PULL_NONE), MP_OBJ_NEW_SMALL_INT(GPIO_PuPd_NOPULL) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PULL_UP), MP_OBJ_NEW_SMALL_INT(GPIO_PuPd_UP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PULL_DOWN), MP_OBJ_NEW_SMALL_INT(GPIO_PuPd_DOWN) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PUSH_PULL), MP_OBJ_NEW_SMALL_INT(GPIO_OType_PP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OPEN_DRAIN), MP_OBJ_NEW_SMALL_INT(GPIO_OType_OD) },

    // EXTI bindings
    { MP_OBJ_NEW_QSTR(MP_QSTR_Exti), (mp_obj_t)&exti_obj_type },
};

STATIC const mp_map_t pyb_module_globals = {
    .all_keys_are_qstrs = 1,
    .table_is_fixed_array = 1,
    .used = sizeof(pyb_module_globals_table) / sizeof(mp_map_elem_t),
    .alloc = sizeof(pyb_module_globals_table) / sizeof(mp_map_elem_t),
    .table = (mp_map_elem_t*)pyb_module_globals_table,
};

const mp_obj_module_t pyb_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_pyb,
    .globals = (mp_map_t*)&pyb_module_globals,
};
