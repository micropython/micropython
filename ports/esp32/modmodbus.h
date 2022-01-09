#ifndef MICROPY_INCLUDED_ESP32_MODNETWORK2_H
#define MICROPY_INCLUDED_ESP32_MODNETWORK2_H

#include "py/runtime.h"
#include "py/obj.h"

typedef struct _modbus_register_area_obj_t {
    mp_obj_base_t base;
    uint8_t type;
    uint32_t start_offset;
    uint32_t size;
    void* address;
} modbus_register_area_obj_t;

typedef struct _modbus_slave_obj_t {
    mp_obj_base_t base;
    uint8_t slave_address;
    uint8_t uart_port;
    uint32_t baudrate;
    uint8_t parity;
    uint8_t tx;
    uint8_t rx;
    uint8_t rts;
    uint8_t serial_mode;
} modbus_slave_obj_t;

#endif