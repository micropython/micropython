// pin_defs.h - GD32 pin encoding helpers
#ifndef MICROPY_INCLUDED_GD32_PIN_DEFS_H
#define MICROPY_INCLUDED_GD32_PIN_DEFS_H

#include <stdint.h>
#include "py/obj.h"
#include "gd32f4xx.h"

// A pin is encoded as: port index (0=A .. 8=I) << 4 | pin number (0..15)
#define PIN_PORT_SHIFT  (4)
#define PIN_NUM_MASK    (0x0f)
#define PIN_PORT(p)     ((uint8_t)((p) >> PIN_PORT_SHIFT))
#define PIN_NUM(p)      ((uint8_t)((p) & PIN_NUM_MASK))
#define PIN_ENC(port, num)  (((port) << PIN_PORT_SHIFT) | (num))
#define PIN_INVALID     (0xff)

typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    uint8_t pin;
} machine_pin_obj_t;

uint32_t pin_port_base(uint8_t port_idx);
uint32_t pin_port_rcu(uint8_t port_idx);
uint32_t pin_hal_mask(uint8_t num);

extern const machine_pin_obj_t machine_pin_obj_table[];
extern const size_t machine_pin_board_pins_locals_dict_table_len;
extern const mp_rom_map_elem_t machine_pin_board_pins_locals_dict_table[];
extern const mp_obj_type_t machine_pin_type;
// Used by extmod via MICROPY_PY_MACHINE_PIN_MAKE_NEW (e.g. machine.Signal).
mp_obj_t machine_pin_make_new(const mp_obj_type_t *type, size_t n_args,
                              size_t n_kw, const mp_obj_t *args);

// ---- mp_hal pin abstraction (for SoftSPI/SoftI2C/Signal) ----
// py/mphal.h tests "#ifndef mp_hal_pin_obj_t" to decide whether the port
// supplies its own pin API, so this must be a macro rather than a typedef.
struct _mp_hal_pin_obj_t {
    uint8_t pin;        // encoded port<<4 | num
};
#define mp_hal_pin_obj_t struct _mp_hal_pin_obj_t

// Fetch a pin from a Python Pin object or an int encoding.
mp_hal_pin_obj_t mp_hal_get_pin_obj(mp_obj_t obj);
// Human-readable pin name, used by MP_HAL_PIN_FMT.
const char *mp_hal_pin_name(mp_hal_pin_obj_t pin);


#endif
