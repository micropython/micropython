// arch/sys_arch.h - lwIP sys layer for a NO_SYS=0 build without an RTOS.
//
// MicroPython runs lwIP in its main loop and polls the stack, so the sys
// protection primitives reduce to disabling interrupts for the critical
// section.  This mirrors what ports/stm32/lwip_inc/arch/sys_arch.h does.
#ifndef MICROPY_INCLUDED_GD32_LWIP_ARCH_SYS_ARCH_H
#define MICROPY_INCLUDED_GD32_LWIP_ARCH_SYS_ARCH_H

#include "py/mpconfig.h"
#include "py/mphal.h"

typedef uint32_t sys_prot_t;

#define SYS_ARCH_DECL_PROTECT(lev)  sys_prot_t lev
#define SYS_ARCH_PROTECT(lev)       lev = MICROPY_BEGIN_ATOMIC_SECTION()
#define SYS_ARCH_UNPROTECT(lev)     MICROPY_END_ATOMIC_SECTION(lev)

#endif // MICROPY_INCLUDED_GD32_LWIP_ARCH_SYS_ARCH_H
