/* This example demonstrates using an ESP32 esp-idf function
*/

// Include the header file to get access to the MicroPython API
#include "py/dynruntime.h"

// ESP-IDF imports
#include <driver/pcnt.h>
#include <esp_err.h>

// This is the function which will be called from Python as counter_init(x)
STATIC mp_obj_t counter_init(mp_obj_t x_obj) {
    // Extract the integer from the MicroPython input object
    mp_int_t x = mp_obj_get_int(x_obj);
    // Call pcnt_unit_config
    pcnt_config_t cfg = {
        .unit = 0,
        .channel = 0,
        .pulse_gpio_num = x,
        .ctrl_gpio_num = -1, // not yet supported
        .counter_h_lim = 0,
        .counter_l_lim = 0,
        .pos_mode = PCNT_COUNT_DIS, // positive edge is no-op
        .neg_mode = PCNT_COUNT_DIS, // negative edge is no-op
        .lctrl_mode = PCNT_MODE_KEEP, // ctrl pin is no-op
        .hctrl_mode = PCNT_MODE_KEEP, // ctrl pin is no-op
    };
    esp_err_t err = pcnt_unit_config(&cfg);
    // Convert the result to a MicroPython integer object and return it
    return mp_obj_new_int(err);
}
// Define a Python reference to the function above
STATIC MP_DEFINE_CONST_FUN_OBJ_1(counter_init_obj, counter_init);

// Another function that calls into the firmware, this time to mp_micropython_mem_info()
// just as a simple demo which prints something to make it easy to see whether it's working or not.
STATIC mp_obj_t mem_info() {
    extern int mp_micropython_mem_info(int, int);
    mp_printf(&mp_plat_print, "mp_fun_table is at 0x%x\n", &mp_fun_table);
    mp_printf(&mp_plat_print, "calling mp_micropython_mem_info\n");
    int i = mp_micropython_mem_info(0, 0);
    mp_printf(&mp_plat_print, "mp_micropython_mem_info: %d/0x%x\n", i, i);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mem_info_obj, mem_info);

// This is the entry point and is called when the module is imported
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    // This must be first, it sets up the globals dict and other things
    MP_DYNRUNTIME_INIT_ENTRY

    // Make the function available in the module's namespace
    mp_store_global(MP_QSTR_counter_init, MP_OBJ_FROM_PTR(&counter_init_obj));
    mp_store_global(MP_QSTR_mem_info, MP_OBJ_FROM_PTR(&mem_info_obj));

    // This must be last, it restores the globals dict
    MP_DYNRUNTIME_INIT_EXIT
}
