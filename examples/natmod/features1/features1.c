/* This example demonstrates the following features in a native module:
    - defining simple functions exposed to Python
    - defining local, helper C functions
    - defining constant integers and strings exposed to Python
    - getting and creating integer objects
    - creating Python lists
    - raising exceptions
    - allocating memory
    - BSS and constant data (rodata)
    - relocated pointers in rodata
*/

// Include the header file to get access to the MicroPython API
#include "py/dynruntime.h"

// BSS (zero) data
uint16_t data16[4];

// Constant data (rodata)
const uint8_t table8[] = { 0, 1, 1, 2, 3, 5, 8, 13 };
const uint16_t table16[] = { 0x1000, 0x2000 };

// Constant data pointing to BSS/constant data
uint16_t *const table_ptr16a[] = { &data16[0], &data16[1], &data16[2], &data16[3] };
const uint16_t *const table_ptr16b[] = { &table16[0], &table16[1] };

// A simple function that adds its 2 arguments (must be integers)
STATIC mp_obj_t add(mp_obj_t x_in, mp_obj_t y_in) {
    mp_int_t x = mp_obj_get_int(x_in);
    mp_int_t y = mp_obj_get_int(y_in);
    return mp_obj_new_int(x + y);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(add_obj, add);

// A local helper function (not exposed to Python)
STATIC mp_int_t fibonacci_helper(mp_int_t x) {
    if (x < MP_ARRAY_SIZE(table8)) {
        return table8[x];
    } else {
        return fibonacci_helper(x - 1) + fibonacci_helper(x - 2);
    }
}

// A function which computes Fibonacci numbers
STATIC mp_obj_t fibonacci(mp_obj_t x_in) {
    mp_int_t x = mp_obj_get_int(x_in);
    if (x < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("can't compute negative Fibonacci number"));
    }
    return mp_obj_new_int(fibonacci_helper(x));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fibonacci_obj, fibonacci);

// A function that accesses the BSS data
STATIC mp_obj_t access(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        // Create a list holding all items from data16
        mp_obj_list_t *lst = MP_OBJ_TO_PTR(mp_obj_new_list(MP_ARRAY_SIZE(data16), NULL));
        for (int i = 0; i < MP_ARRAY_SIZE(data16); ++i) {
            lst->items[i] = mp_obj_new_int(data16[i]);
        }
        return MP_OBJ_FROM_PTR(lst);
    } else if (n_args == 1) {
        // Get one item from data16
        mp_int_t idx = mp_obj_get_int(args[0]) & 3;
        return mp_obj_new_int(data16[idx]);
    } else {
        // Set one item in data16 (via table_ptr16a)
        mp_int_t idx = mp_obj_get_int(args[0]) & 3;
        *table_ptr16a[idx] = mp_obj_get_int(args[1]);
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(access_obj, 0, 2, access);

// A function that allocates memory and creates a bytearray
STATIC mp_obj_t make_array(void) {
    uint16_t *ptr = m_new(uint16_t, MP_ARRAY_SIZE(table_ptr16b));
    for (int i = 0; i < MP_ARRAY_SIZE(table_ptr16b); ++i) {
        ptr[i] = *table_ptr16b[i];
    }
    return mp_obj_new_bytearray_by_ref(sizeof(uint16_t) * MP_ARRAY_SIZE(table_ptr16b), ptr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(make_array_obj, make_array);

// This is the entry point and is called when the module is imported
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    // This must be first, it sets up the globals dict and other things
    MP_DYNRUNTIME_INIT_ENTRY

    // Messages can be printed as usualy
    mp_printf(&mp_plat_print, "initialising module self=%p\n", self);

    // Make the functions available in the module's namespace
    mp_store_global(MP_QSTR_add, MP_OBJ_FROM_PTR(&add_obj));
    mp_store_global(MP_QSTR_fibonacci, MP_OBJ_FROM_PTR(&fibonacci_obj));
    mp_store_global(MP_QSTR_access, MP_OBJ_FROM_PTR(&access_obj));
    mp_store_global(MP_QSTR_make_array, MP_OBJ_FROM_PTR(&make_array_obj));

    // Add some constants to the module's namespace
    mp_store_global(MP_QSTR_VAL, MP_OBJ_NEW_SMALL_INT(42));
    mp_store_global(MP_QSTR_MSG, MP_OBJ_NEW_QSTR(MP_QSTR_HELLO_MICROPYTHON));

    // This must be last, it restores the globals dict
    MP_DYNRUNTIME_INIT_EXIT
}
