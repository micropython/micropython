/* This example demonstrates the following features in a native module:
    - defining a simple function exposed to Python
    - defining a local, helper C function
    - getting and creating integer objects
*/

// Include the header file to get access to the MicroPython API
#include "py/dynruntime.h"
// Include esp-idf include files
#include "esp_system.h"

// This is the function which will be called from Python, as free_heap()
STATIC mp_obj_t free_heap() {
    // calling an ESP-IDF function
    uint32_t f = esp_get_free_heap_size();
    // sample debug printf
    mp_printf(&mp_plat_print, "esp_get_free_heap_size returned %d\n", f);
    // return integer as MP small int object
    return mp_obj_new_int(f);
}
// Define a Python reference to the function above
STATIC MP_DEFINE_CONST_FUN_OBJ_0(free_heap_obj, free_heap);

// This a function that only exists in esp-idf-v4, it will raise in esp-idf-v3.
STATIC mp_obj_t flash_encryption_mode() {
    // calling an ESP-IDF function that doesn't exist in v3
    extern uint32_t esp_get_flash_encryption_mode();
    uint32_t f = esp_get_flash_encryption_mode();
    // return integer as MP small int object
    return mp_obj_new_int(f);
}
// Define a Python reference to the function above
STATIC MP_DEFINE_CONST_FUN_OBJ_0(flash_encryption_mode_obj, flash_encryption_mode);

// This is the entry point and is called when the module is imported
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    // This must be first, it sets up the globals dict and other things
    MP_DYNRUNTIME_INIT_ENTRY

    // Make the functions available in the module's namespace
    mp_store_global(MP_QSTR_free_heap, MP_OBJ_FROM_PTR(&free_heap_obj));
    mp_store_global(MP_QSTR_flash_encryption_mode, MP_OBJ_FROM_PTR(&flash_encryption_mode_obj));

    // This must be last, it restores the globals dict
    MP_DYNRUNTIME_INIT_EXIT
}
