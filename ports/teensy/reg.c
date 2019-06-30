#include <stdio.h>
#include <string.h>
#include "py/runtime.h"
#include "reg.h"

#if MICROPY_REG

mp_obj_t reg_cmd(void *base, reg_t *reg, mp_uint_t num_regs, uint n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        // dump all regs

        for (mp_uint_t reg_idx = 0; reg_idx < num_regs; reg_idx++, reg++) {
            printf("  %-8s @0x%08x = 0x%08lx\n",
                   reg->name, (mp_uint_t)base + reg->offset, *(uint32_t *)((uint8_t *)base + reg->offset));
        }
        return mp_const_none;
    }

    mp_uint_t addr = 0;

    if (mp_obj_is_str(args[0])) {
        const char *name = mp_obj_str_get_str(args[0]);
        mp_uint_t reg_idx;
        for (reg_idx = 0; reg_idx < num_regs; reg_idx++, reg++) {
            if (strcmp(name, reg->name) == 0) {
                break;
            }
        }
        if (reg_idx >= num_regs) {
            printf("Unknown register: '%s'\n", name);
            return mp_const_none;
        }
        addr = (mp_uint_t)base + reg->offset;
    } else {
        addr = (mp_uint_t)base + mp_obj_get_int(args[0]);
    }

    if (n_args < 2) {
        // get
        printf("0x%08lx\n", *(uint32_t *)addr);
    } else {
        *(uint32_t *)addr = mp_obj_get_int(args[1]);
    }
    return mp_const_none;
}

#endif
