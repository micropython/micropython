// This file contains default configuration settings for MicroPython.
// You can override any of these options using mpconfigport.h file located
// in a directory of your port.

#include <mpconfigport.h>

// Any options not explicitly set in mpconfigport.h will get default
// values below.

// Whether to collect memory allocation stats
#ifndef MICROPY_MEM_STATS
#define MICROPY_MEM_STATS (1)
#endif
