// Include MicroPython API.
#include "py/runtime.h"

// Declare the function we'll make available in Python as cppexample.cppfunc().
extern mp_obj_t cppfunc(mp_obj_t a_obj, mp_obj_t b_obj);
