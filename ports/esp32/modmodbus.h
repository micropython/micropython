#ifndef MICROPY_INCLUDED_ESP32_MODMODBUS_H
#define MICROPY_INCLUDED_ESP32_MODMODBUS_H

#include "modnetwork2.h"

#include "esp_modbus_common.h"
#include "esp_modbus_master.h"
#include "esp_modbus_slave.h"

#include "py/runtime.h"
#include "py/obj.h"

#if ESP_IDF_VERSION_MINOR >= 3

bool slave_set;
bool master_set;
bool slave_running;
bool master_running;
bool register_area_set;

char area_names[4][9];
char parity_names[4][6];
char serial_mode_names[2][5];
char data_type_names[5][6];

int cid_counter;

mp_obj_t param_descriptions;

uint8_t get_length_of_type_in_registers(uint8_t type);

void raise_VauleError_if_outside_area(int16_t area_start_register, int16_t area_length,
    int16_t var_start_register, int16_t var_length);

uint32_t get_uint_checked(mp_obj_t data, uint8_t type, bool raise_exception);

typedef struct _modbus_register_area_obj_t {
    mp_obj_base_t base;
    uint8_t type;               // HOLDING, COIL, INPUT, DISCRETE
    uint16_t reg_start_offset;  // start register
    uint16_t reg_size;          // size in registers
    void *address;              // address in memory
} modbus_register_area_obj_t;

const mp_obj_type_t modbus_register_area_type;

typedef struct _modbus_parameter_descriptor_obj_t {
    mp_obj_base_t base;
    uint16_t cid;                   // must be unique
    mp_obj_t param_key;             // must be unique
    mp_obj_t param_unit;            // just for documentation
    uint8_t mb_slave_addr;          // between 1 and 127
    mb_param_type_t mb_param_type;  // HOLDING, COIL, INPUT, DISCRETE
    uint16_t mb_reg_start;          // start register
    uint16_t mb_size;               // size in registers
    uint16_t param_offset;          // not used
    mb_descr_type_t param_type;     // U8, U16, U32, FLOAT, ASCII
    mb_descr_size_t param_size;     // relevant for strings
    int min;                        // currently not used
    int max;                        // currently not used
    int step;                       // currently not used
    mb_param_perms_t access;        // currently not used
} modbus_parameter_descriptor_obj_t;

const mp_obj_type_t modbus_parameter_descriptor_type;

typedef struct _modbus_serial_slave_obj_t {
    mp_obj_base_t base;
    uint8_t slave_address;
    uint8_t uart_port;
    uint32_t baudrate;
    uint8_t parity;
    uint8_t tx;
    uint8_t rx;
    uint8_t rts;
    uint8_t serial_mode;
    TaskHandle_t callbak_task_handle;
} modbus_serial_slave_obj_t;

const mp_obj_type_t modbus_serial_slave_type;

typedef struct _modbus_serial_master_obj_t {
    mp_obj_base_t base;
    uint8_t uart_port;
    uint32_t baudrate;
    uint8_t parity;
    uint8_t tx;
    uint8_t rx;
    uint8_t rts;
    uint8_t serial_mode;
} modbus_serial_master_obj_t;

const mp_obj_type_t modbus_serial_master_type;

typedef struct _modbus_tcp_slave_obj_t {
    mp_obj_base_t base;
    uint16_t port;
    mp_obj_t network_if;
} modbus_tcp_slave_obj_t;

const mp_obj_type_t modbus_tcp_slave_type;

typedef struct _modbus_tcp_master_obj_t {
    mp_obj_base_t base;
    mp_obj_t ips;
    uint16_t port;
    mp_obj_t network_if;
} modbus_tcp_master_obj_t;

const mp_obj_type_t modbus_tcp_master_type;

#endif
#endif
