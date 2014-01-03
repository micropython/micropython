// This file contains default configuration settings for MicroPython.
// You can override any of these options using mpconfigport.h file located
// in a directory of your port.

#include <mpconfigport.h>

#ifndef INT_FMT
// printf format spec to use for machine_int_t and friends
#ifdef __LP64__
// Archs where machine_int_t == long, long != int
#define UINT_FMT "%lu"
#define INT_FMT "%ld"
#else
// Archs where machine_int_t == int
#define UINT_FMT "%u"
#define INT_FMT "%d"
#endif
#endif //INT_FMT


// Any options not explicitly set in mpconfigport.h will get default
// values below.

// Whether to collect memory allocation stats
#ifndef MICROPY_MEM_STATS
#define MICROPY_MEM_STATS (1)
#endif
