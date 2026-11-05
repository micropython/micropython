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

#endif
