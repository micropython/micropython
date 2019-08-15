#ifndef _PYB_PINMUX_H_
#define _PYB_PINMUX_H_

typedef struct _pinmux_obj_t {
    mp_obj_base_t base;
    qstr pinmux_name;
    const uint32_t muxRegister;
    const uint32_t muxMode;
    const uint32_t inputRegister;
    const uint32_t inputDaisy;
    const uint32_t configRegister;
}pinmux_obj_t;

typedef struct _port_t{
    mp_obj_base_t base;
    qstr port_name;
    uint8_t pinmux_num;
    const pinmux_obj_t *pinmux;
}port_obj_t;

#include "port-evkmimxrt1064.h"

extern const mp_obj_type_t pinmux_type;
extern const mp_obj_type_t port_type;
extern const mp_obj_type_t board_ports_obj_type;

extern const mp_obj_dict_t port_board_pinmux_locals_dict;
#endif