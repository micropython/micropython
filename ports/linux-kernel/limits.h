#ifndef MICROPY_INCLUDED_LINUX_KERNEL_LIMITS_H
#define MICROPY_INCLUDED_LINUX_KERNEL_LIMITS_H

// Linux doesn't have this define. it has LONG_MAX which is equivalent for x86-64, and it's defined
// just this way, but with a cast included.
// MicroPython uses this define in preprocessor #ifs, and the preprocessor doesn't handle casts.
// so just redefine it here.
#define SSIZE_MAX (~0UL >> 1)

#endif // MICROPY_INCLUDED_LINUX_KERNEL_LIMITS_H
