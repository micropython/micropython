#ifndef MICROPY_INCLUDED_TEENSY_REG_H
#define MICROPY_INCLUDED_TEENSY_REG_H

typedef struct {
    const char *name;
    mp_uint_t   offset;
} reg_t;

#define REG_ENTRY(st, name) { #name, offsetof(st, name) }

mp_obj_t reg_cmd(void *base, reg_t *reg, mp_uint_t num_reg, uint n_args, const mp_obj_t *args);

#endif // MICROPY_INCLUDED_TEENSY_REG_H
