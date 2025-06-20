#ifndef __ESC_H__
#define __ESC_H__

#include "py/mphal.h"
#include "py/runtime.h"

void esc_init();

extern struct fifo_descriptor o_buf;
extern bool in_esc;

void inc_pos();
uint8_t calc_pos();


bool process_esc_char(uint8_t* ch);

void lcd_disable_special_charset(uint8_t param);

void set_special_charset(uint8_t charset);

#endif