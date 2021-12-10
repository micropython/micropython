#include <stdio.h>

#include <stdint.h>
#include "py/mpconfig.h"
#include "py/obj.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "mphalport.h"

void mp_hal_delay_us(mp_uint_t delay) {
}

void mp_hal_disable_all_interrupts(void) {
    common_hal_mcu_disable_interrupts();
}

void mp_hal_enable_all_interrupts(void) {
    common_hal_mcu_enable_interrupts();
}

mp_uint_t cpu_get_regs_and_sp(mp_uint_t *regs) {
    size_t sp = 0;
    __asm__ ("mov %[out], sp" : [out] "=r" (sp));
    __asm__ ("mov %[out], x19" : [out] "=r" (regs[0]));
    __asm__ ("mov %[out], x20" : [out] "=r" (regs[1]));
    __asm__ ("mov %[out], x21" : [out] "=r" (regs[2]));
    __asm__ ("mov %[out], x22" : [out] "=r" (regs[3]));
    __asm__ ("mov %[out], x23" : [out] "=r" (regs[4]));
    __asm__ ("mov %[out], x24" : [out] "=r" (regs[5]));
    __asm__ ("mov %[out], x25" : [out] "=r" (regs[6]));
    __asm__ ("mov %[out], x26" : [out] "=r" (regs[7]));
    __asm__ ("mov %[out], x27" : [out] "=r" (regs[8]));
    __asm__ ("mov %[out], x28" : [out] "=r" (regs[9]));
    return sp;
}
