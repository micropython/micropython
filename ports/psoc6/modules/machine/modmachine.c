// std includes
#include <stdio.h>
#include <stdlib.h>

// mpy includes
#include "py/obj.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "shared/runtime/pyexec.h"
#include "extmod/modmachine.h"

// MTB includes
#include "cybsp.h"
#include "cyhal.h"
#include "cy_pdl.h"

// port-specific includes
#include "modmachine.h"
#include "mplogger.h"


#if MICROPY_PY_MACHINE

// enums to hold the MPY constants as given in guidelines
enum {MACHINE_PWRON_RESET, MACHINE_HARD_RESET, MACHINE_WDT_RESET, MACHINE_DEEPSLEEP_RESET, MACHINE_SOFT_RESET};

// function to return 64-bit silicon ID of given PSoC microcontroller
// A combined 64-bit unique ID. [63:57] - DIE_YEAR [56:56] - DIE_MINOR [55:48] - DIE_SORT [47:40] - DIE_Y [39:32] - DIE_X [31:24] - DIE_WAFER [23:16] - DIE_LOT[2] [15: 8] - DIE_LOT[1] [ 7: 0] - DIE_LOT[0]
STATIC uint64_t system_get_unique_id(void) {
    return Cy_SysLib_GetUniqueId();
}

// using watchdog timer to count to minimum value (1ms) to trigger reset
// thread-safe way as other methods might interfere with pending interrupts, threads etc.
STATIC void system_reset(void) {
    cyhal_wdt_t wdt_obj;
    cyhal_wdt_init(&wdt_obj, 1); // min 1ms count time
    cyhal_wdt_start(&wdt_obj);
}

// get reset cause of the last system reset
// macros defined here: cy_syslib.h
STATIC uint32_t system_reset_cause(void) {
    return Cy_SysLib_GetResetReason();
}

// helper function to generate random alphanumeric hash
STATIC uint8_t system_rand_hash(uint8_t length) {
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
// see: https://docs.zephyrproject.org/apidoc/latest/group__isr__apis.html#ga19fdde73c3b02fcca6cf1d1e67631228
STATIC uint8_t system_disable_global_irq(void) {
    uint8_t state = system_rand_hash(HASH_CHARS_NUM); // 10 chars long key gen;
    __disable_irq();
    system_irq_key = state;
    return state;
}

// function to enable global IRQs
// uses passed alphanumeric key to verify and enable IRQs
STATIC bool system_enable_global_irq(uint8_t state) {
    if (state == system_irq_key) {
        __enable_irq();
        return true;
    } else {
        return false;
    }
}

// API to return clock freq; Fast CLK (CM4) is the main sys clk
STATIC uint32_t system_get_cpu_freq(void) {
//    return Cy_SysClk_ClkPathMuxGetFrequency(Cy_SysClk_ClkPathGetSource(0UL));
    return Cy_SysClk_ClkFastGetFrequency();
}

// TODO: unused. Required ?
// API to return clock freq divider for Fast CLK (CM4)
// STATIC uint8_t system_get_cpu_freq_div(void) {
//     return Cy_SysClk_ClkFastGetDivider();
// }

void machine_init(void) {
    mplogger_print("machine init\n");

    // TODO: put all module init functions here ?
    // machine_pin_init(); ?
}

void machine_deinit(void) {
    mplogger_print("machine deinit\n");

    // TODO: put all module deinit functions here ?
    // machine_pin_deinit(); ?
}

// machine.info([dump_alloc_table])
// Print out lots of information about the board.
STATIC mp_obj_t machine_info(size_t n_args, const mp_obj_t *args) {
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
// STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_info_obj, machine_info);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_info_obj, 0, 1, machine_info);


// machine.freq()
STATIC mp_obj_t machine_freq(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        mp_printf(&mp_plat_print, "System core freq (CM4): %d Hz\n", system_get_cpu_freq());
        return mp_const_none;
    } else {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("Not implemented!!!\n"));
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_freq_obj, 0, 1, machine_freq);


// machine.unique_id()
STATIC mp_obj_t machine_unique_id(void) {
    uint64_t id = system_get_unique_id();
    byte *id_addr = (byte *)&id;
    mplogger_print("ID_formatted:%02x%02x%02x%02x:%02x%02x%02x%02x\n", id_addr[0], id_addr[1], id_addr[2], id_addr[3], id_addr[4], id_addr[5], id_addr[6], id_addr[7]);
    mplogger_print("RAW_ID_data:");
    return mp_obj_new_bytes(id_addr, 8);
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_unique_id_obj, machine_unique_id);


// machine.reset()
STATIC mp_obj_t machine_reset(void) {
    system_reset();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_obj, machine_reset);

// machine.soft_reset()
STATIC mp_obj_t machine_soft_reset(void) {
    pyexec_system_exit = PYEXEC_FORCED_EXIT;
    mp_raise_type(&mp_type_SystemExit);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_soft_reset_obj, machine_soft_reset);

// machine.reset_cause()
// Note: accurate returns only after one forced hard reset post hex upload.
// Possible reason is that the board does not get (soft) reset after firmware upload
STATIC mp_obj_t machine_reset_cause(void) {
    qstr mp_reset_qstr = MP_QSTR_None;
    uint8_t reset_cause_const = -1;
    uint32_t reset_cause = system_reset_cause();

    if (reset_cause == 0UL) {
        mp_reset_qstr = MP_QSTR_HARD_RESET;
        reset_cause_const = MACHINE_HARD_RESET;
    } else if (reset_cause == CY_SYSLIB_RESET_HWWDT || reset_cause == CY_SYSLIB_RESET_SWWDT0 || reset_cause == CY_SYSLIB_RESET_SWWDT1 || reset_cause == CY_SYSLIB_RESET_SWWDT2 || reset_cause == CY_SYSLIB_RESET_SWWDT3) {
        mp_reset_qstr = MP_QSTR_WDT_RESET;
        reset_cause_const = MACHINE_WDT_RESET;
    } else if (reset_cause == CY_SYSLIB_RESET_DPSLP_FAULT) {
        mp_reset_qstr = MP_QSTR_DEEPSLEEP_RESET;
        reset_cause_const = MACHINE_DEEPSLEEP_RESET;
    } else if (reset_cause == CY_SYSLIB_RESET_SOFT) {
        mp_reset_qstr = MP_QSTR_SOFT_RESET;
        reset_cause_const = MACHINE_SOFT_RESET;
    } else {
        mp_reset_qstr = MP_QSTR_PWRON_RESET;
        reset_cause_const = MACHINE_PWRON_RESET;
    }

    mplogger_print("Reset cause: %q; ", mp_reset_qstr);
    return MP_OBJ_NEW_SMALL_INT(reset_cause_const);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_cause_obj, machine_reset_cause);

// machine.disable_irq()
STATIC mp_obj_t machine_disable_irq(void) {
    uint32_t state = system_disable_global_irq();
    mplogger_print("IRQ State: ");
    return mp_obj_new_int(state);
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_disable_irq_obj, machine_disable_irq);

// machine.enable_irq()
STATIC mp_obj_t machine_enable_irq(mp_obj_t state_in) {
    uint32_t state = mp_obj_get_int(state_in);
    bool result = system_enable_global_irq(state);
    if (result) {
        mplogger_print("Interrupts enabled\n");
    } else {
        mplogger_print("Interrupts not enabled; key mismatch!\n");
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_enable_irq_obj, machine_enable_irq);

// machine.idle()
// This executies a wfi machine instruction which reduces power consumption
// of the MCU until an interrupt occurs, at which point execution continues.
// see: https://www.infineon.com/dgdl/Infineon-AN219528_PSoC_6_MCU_low-power_modes_and_power_reduction_techniques-ApplicationNotes-v06_00-EN.pdf?fileId=8ac78c8c7cdc391c017d0d31efdc659f  pg.7
STATIC mp_obj_t machine_idle(void) {
    __WFI(); // standard ARM instruction
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_idle_obj, machine_idle);


STATIC const mp_rom_map_elem_t machine_module_globals_table[] = {
    // instance functions
    { MP_ROM_QSTR(MP_QSTR___name__),            MP_ROM_QSTR(MP_QSTR_machine) },
    { MP_ROM_QSTR(MP_QSTR_info),                MP_ROM_PTR(&machine_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq),                MP_ROM_PTR(&machine_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_unique_id),           MP_ROM_PTR(&machine_unique_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset),               MP_ROM_PTR(&machine_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_soft_reset),          MP_ROM_PTR(&machine_soft_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_cause),         MP_ROM_PTR(&machine_reset_cause_obj) },

    { MP_ROM_QSTR(MP_QSTR_disable_irq),         MP_ROM_PTR(&machine_disable_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_irq),          MP_ROM_PTR(&machine_enable_irq_obj) },

    { MP_ROM_QSTR(MP_QSTR_idle),                MP_ROM_PTR(&machine_idle_obj) },

    // { MP_ROM_QSTR(MP_QSTR_bootloader),          MP_ROM_PTR(&machine_bootloader_obj) }, // Note: not implemented

    // class constants
    { MP_ROM_QSTR(MP_QSTR_PWRON_RESET),         MP_ROM_INT(MACHINE_PWRON_RESET) },
    { MP_ROM_QSTR(MP_QSTR_HARD_RESET),          MP_ROM_INT(MACHINE_HARD_RESET) },
    { MP_ROM_QSTR(MP_QSTR_WDT_RESET),           MP_ROM_INT(MACHINE_WDT_RESET) },
    { MP_ROM_QSTR(MP_QSTR_DEEPSLEEP_RESET),     MP_ROM_INT(MACHINE_DEEPSLEEP_RESET) },
    { MP_ROM_QSTR(MP_QSTR_SOFT_RESET),          MP_ROM_INT(MACHINE_SOFT_RESET) },

    // TODO: dynamic memory allocation functions/objects. Not yet implemented
    // { MP_ROM_QSTR(MP_QSTR_mem8),                MP_ROM_PTR(&machine_mem8_obj) },
    // { MP_ROM_QSTR(MP_QSTR_mem16),               MP_ROM_PTR(&machine_mem16_obj) },
    // { MP_ROM_QSTR(MP_QSTR_mem32),               MP_ROM_PTR(&machine_mem32_obj) },

    // Modules
    { MP_ROM_QSTR(MP_QSTR_I2C),                 MP_ROM_PTR(&machine_i2c_type) },
    { MP_ROM_QSTR(MP_QSTR_SoftI2C),             MP_ROM_PTR(&mp_machine_soft_i2c_type) },
    { MP_ROM_QSTR(MP_QSTR_Pin),                 MP_ROM_PTR(&machine_pin_type) },
    { MP_ROM_QSTR(MP_QSTR_Signal),              MP_ROM_PTR(&machine_signal_type) },
    { MP_ROM_QSTR(MP_QSTR_RTC),                 MP_ROM_PTR(&machine_rtc_type) },
    { MP_ROM_QSTR(MP_QSTR_PWM),                 MP_ROM_PTR(&machine_pwm_type) },
    { MP_ROM_QSTR(MP_QSTR_SPI),                 MP_ROM_PTR(&machine_spi_type) },
    { MP_ROM_QSTR(MP_QSTR_SoftSPI),             MP_ROM_PTR(&mp_machine_soft_spi_type) },
    { MP_ROM_QSTR(MP_QSTR_Timer),               MP_ROM_PTR(&machine_timer_type) },
    { MP_ROM_QSTR(MP_QSTR_ADC),                 MP_ROM_PTR(&machine_adc_type) },
    { MP_ROM_QSTR(MP_QSTR_ADCBlock),            MP_ROM_PTR(&machine_adcblock_type) },
};
STATIC MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);


const mp_obj_module_t mp_module_machine = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&machine_module_globals,
};

// Register machine module in MPY
MP_REGISTER_MODULE(MP_QSTR_machine, mp_module_machine);

#endif // MICROPY_PY_MACHINE
