#ifndef __LCD_H__
#define __LCD_H__

#include "py/mphal.h"
#include "py/runtime.h"

typedef struct lcd_struct 
{
	uint8_t h;        // height
	uint8_t w;        // width
	uint8_t init_seq;
	uint8_t line_offset[4];
}lcd_t;


extern lcd_t* lcd;

extern bool RAM1_LCD_SONDER;

extern uint8_t lcd_x;

extern uint8_t lcd_y;

void add_to_obuf_more(bool is_control, uint8_t ch);

void lcd_init(uint8_t lcd_type);

#endif