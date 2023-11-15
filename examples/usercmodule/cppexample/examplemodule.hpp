#ifndef EXAMPLEMODULE_HPP
#define EXAMPLEMODULE_HPP

// Include MicroPython API.
extern "C" { // so imported symbols don't get mangled, otherwise linker undefined reference errors
  #include "py/runtime.h"
}

// Declare the function we'll make available in Python as cppexample.cppfunc().
extern mp_obj_t cppusermod_func(const mp_obj_t o_in);

#endif
