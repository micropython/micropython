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

// port-specific includes
#include "modmachine.h"
#include "mplogger.h"

#if MICROPY_PY_MACHINE

// enums to hold the MPY constants as given in guidelines
enum {MACHINE_PWRON_RESET, MACHINE_HARD_RESET, MACHINE_WDT_RESET, MACHINE_DEEPSLEEP_RESET, MACHINE_SOFT_RESET};

uint32_t reset_cause;

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
    reset_cause = CYHAL_SYSTEM_RESET_SOFT;
    mplogger_print("machine deinit\n");
    mod_wdt_deinit();
    mod_pin_deinit();
    mod_adcblock_deinit();
    mod_i2c_deinit();
    mod_pwm_deinit();
    mod_spi_deinit();
    mod_rtc_deinit();
    mod_pin_phy_deinit();
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

static void mp_machine_set_freq(size_t n_args, const mp_obj_t *args) {
    mp_raise_NotImplementedError(MP_ERROR_TEXT("Not implemented!!!\n"));
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

static mp_int_t mp_machine_reset_cause(void) {
    if (reset_cause == CYHAL_SYSTEM_RESET_SOFT) {
        return MACHINE_SOFT_RESET;
    } else {
        uint32_t reset_cause = cyhal_system_get_reset_reason();
        if (reset_cause == 0UL) {
            return MACHINE_HARD_RESET;
        } else if (reset_cause == CYHAL_SYSTEM_RESET_WDT) {
            return MACHINE_WDT_RESET;
        } else if (reset_cause == CYHAL_SYSTEM_RESET_DEEPSLEEP_FAULT) {
            return MACHINE_DEEPSLEEP_RESET;
        }
    }
    return MACHINE_PWRON_RESET;
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
    \
    /* Modules */ \
    { MP_ROM_QSTR(MP_QSTR_I2CSlave),            MP_ROM_PTR(&machine_i2c_slave_type) }, \
    { MP_ROM_QSTR(MP_QSTR_Pin),                 MP_ROM_PTR(&machine_pin_type) }, \
    { MP_ROM_QSTR(MP_QSTR_RTC),                 MP_ROM_PTR(&machine_rtc_type) }, \
    { MP_ROM_QSTR(MP_QSTR_SPISlave),            MP_ROM_PTR(&machine_spi_slave_type) }, \
    { MP_ROM_QSTR(MP_QSTR_Timer),               MP_ROM_PTR(&machine_timer_type) }, \
    { MP_ROM_QSTR(MP_QSTR_ADC),                 MP_ROM_PTR(&machine_adc_type) }, \
    { MP_ROM_QSTR(MP_QSTR_ADCBlock),            MP_ROM_PTR(&machine_adcblock_type) }, \

#endif // MICROPY_PY_MACHINE
