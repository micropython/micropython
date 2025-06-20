#ifndef __KBD_H__
#define __KBD_H__

#include "py/mphal.h"

#define DATA_PORT_TO_INPUT(control_port) *control_port = 0x0000
#define DATA_PORT_TO_OUTPUT(control_port) *control_port = 0x5555

#define TIMINGS_ACLK 11
#define TIMINGS_DCLK 11
#define TIMINGS_INE  17

extern bool RAM1_KEY_CLICK;

bool read_from_kbd_buf(uint8_t *buffer, size_t len);

void kbd_init(void);

void kbd_scan(void);

void kbd_set_key_table(uint8_t *params);

void kbd_set_key_shift_table(uint8_t* params);

void kbd_set_key_attribute_table(uint8_t* params);

void kbd_set_column_addresses(uint8_t* params);

void kbd_set_auto_repeat(uint8_t delay, uint8_t freq);

uint32_t kbd_read_dip(uint8_t* dest, uint8_t from, uint8_t count);

void kbd_set_dip_switches(uint8_t num_scan_cols, uint8_t* col_is_dip);

void kbd_set_echo(uint8_t ja_oder_nein);

void kbd_set_control(uint8_t* params);

//#if MICROPY_HW_HAS_LCD

#endif