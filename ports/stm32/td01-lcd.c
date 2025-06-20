/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */



#include "py/mphal.h"
#include "py/runtime.h"

#include "lcd.h"

#if MICROPY_HW_HAS_LCD

#include <stdio.h>
#include <string.h>

#include "pin.h"
#include "timer.h"
#include "bufhelper.h"

#include "ti-2def2.h"

#include "td01-lcd.h"
#include "esc.h"
#include "beep.h"
#include "kbd.h"


#define DEFAULT_PORT_BUS        GPIOH
#define PORT_BUS_8        		GPIOJ

#define CLR 0x01
#define HOME 0x02
#define FS 0x05
#define BS 0x08
#define LF 0x0A
#define D0 LF
#define UP 0x0B
#define FF 0x0C
#define CR 0x0D

#define SMASK			LEN - 1

#ifdef LCD4BIT
#define DEFAULT_LCD_TYPE        14
#else
#define DEFAULT_LCD_TYPE        17
#endif


#define portOut(port, first_half)         (volatile uint8_t *)(((uint32_t)&(port->ODR))      + (first_half?1:0)) 
#define portIn(port, first_half)          (volatile uint8_t *)(((uint32_t)&(port->IDR))      + (first_half?1:0))
#define portControl(port, first_half) 	  (volatile uint16_t*)(((uint32_t)&(port->MODER))    + (first_half?2:0))

typedef struct _tiger_lcd_obj_t 
{
    mp_obj_base_t base;
} tiger_lcd_obj_t;

static tiger_lcd_obj_t tiger_lcd_obj;

static void set_isep_tab_10(bool flag, uint8_t min, uint8_t max);

struct fifo_descriptor o_buf;
struct fifo_descriptor o_buf_more;
struct fifo_descriptor kbd_buf;

static volatile bool inst_ok = false;
static volatile uint8_t interrupt = 0xFF;

static bool lcd_on = true;
static bool kbd_on = true;
static bool bp_on = false;

bool xports_flg = false;
int8_t eport_phys_offs;

uint8_t RAM1_ISEP_TAB[256];

uint8_t RAM1_SLED_BMASK = 1; 
uint8_t RAM1_SLED_PORT = 0x10;
bool RAM1_SLED_TFFLG = true;

uint8_t RAM1_BELL_TFFLG = true; // need 1 or 0 on the pin to beep

const machine_pin_obj_t *pin_e;
const machine_pin_obj_t *pin_rs;

volatile uint8_t*  portData = 0;
volatile uint8_t*  portDataIn = 0;
volatile uint16_t* portDataControl = 0;

const machine_pin_obj_t *pin_beep;

const machine_pin_obj_t *xport_aclk;
const machine_pin_obj_t *xport_dclk;
const machine_pin_obj_t *xport_ine;

pin_gpio_t *gpioPortData;


static lcd_t lcd_table[] = {
	{0, 0,    0,  {0, 0, 0, 0}},  //  0:	Dummy  -  LCD-Typ 0 gibt es nicht !!
	{1, 8, 0x38,  {0,20,40,60}},  //  1:	1-Zeilig,   8-Zeichen			Hitachi HD 44780
	{1,12, 0x38,  {0,20,40,60}},  //  2:	1-Zeilig,  12-Zeichen			Hitachi HD 44780
	{1,16, 0x38,  {0,20,40,60}},  //  3:	1-Zeilig,  16-Zeichen			Hitachi HD 44780
	{1,20, 0x38,  {0,20,40,60}},  //  4:	1-Zeilig,  20-Zeichen			Hitachi HD 44780
	{1,40, 0x38,  {0,20,40,60}},  //  5:	1-Zeilig,  40-Zeichen			Hitachi HD 44780

	{2, 8, 0x38,  {0,20,40,60}},  //  6:	2-Zeilig,   8-Zeichen			Hitachi HD 44780
	{2,12, 0x38,  {0,20,40,60}},  //  7:	2-Zeilig,  12-Zeichen			Hitachi HD 44780
	{2,16, 0x38,  {0,20,40,60}},  //  8:	2-Zeilig,  16-Zeichen			Hitachi HD 44780
	{2,20, 0x38,  {0,20,40,60}},  //  9:	2-Zeilig,  20-Zeichen			Hitachi HD 44780
	{2,40, 0x38,  {0,64, 0, 0}},  // 10:	2-Zeilig,  40-Zeichen			Hitachi HD 44780

	{4,20, 0x38,  {0,20,40,60}},  // 11:	4-Zeilig,  20-Zeichen			Hitachi HD 44780

	{2, 8, 0x38,  {0,64, 0, 0}},  // 12:	2-Zeilig,   8-Zeichen			Hitachi HD 44780
	{2,12, 0x38,  {0,64, 0, 0}},  // 13:	2-Zeilig,  12-Zeichen			Hitachi HD 44780
	{2,16, 0x38,  {0,64, 0, 0}},  // 14:	2-Zeilig,  16-Zeichen			Hitachi HD 44780
	{2,20, 0x38,  {0,64, 0, 0}},  // 15:	2-Zeilig,  20-Zeichen			Hitachi HD 44780
	{2,40, 0x38,  {0,64, 0, 0}},  // 16:	2-Zeilig,  40-Zeichen			Hitachi HD 44780

	{4,20, 0x38,  {0,64,20,84}},  // 17:	4-Zeilig,  20-Zeichen			Hitachi HD 44780
	{2,24, 0x38,  {0,64, 0, 0}},  // 18:  2-zeilig,  24-Zeichen    (Kunde: Bosch)	Hitachi HD 44780
	{4,16, 0x38,  {0,64,16,80}},  // 19:  4-zeilig,  16-Zeichen	 (Kunde: Graf)	Hitachi HD 44780

    {4,24, 0x39,  {0,32,64,96}},  // 20:	4 x 24             	 (Kunde: Wilke)  NJU 6426  <===  ab. 28-SEPT-2000  / Vers. Vers. 1.01h
	{2, 8, 0x38,  {0,64, 0, 0}},  // 21:  2 x  8 bzw. 1 x 16  NJU 6426  (New Japan Radio Co. Ltd.)   <===  ab. 28-SEPT-2000  / Vers. Vers. 1.01h
	{2,16, 0x30,  {0,64, 0, 0}},  // 22:  2 x 16              NJU 6426  (New Japan Radio Co. Ltd.)   <===  ab. 28-SEPT-2000  / Vers. Vers. 1.01h
	{2,24, 0x31,  {0,64, 0, 0}},  // 23:  2 x 24              NJU 6426  (New Japan Radio Co. Ltd.)   <===  ab. 28-SEPT-2000  / Vers. Vers. 1.01h
	{2,32, 0x32,  {0,64, 0, 0}},  // 24:  2 x 32              NJU 6426  (New Japan Radio Co. Ltd.)   <===  ab. 28-SEPT-2000  / Vers. Vers. 1.01h
	{2,40, 0x33,  {0,64, 0, 0}},  // 25:  2 x 40              NJU 6426  (New Japan Radio Co. Ltd.)   <===  ab. 28-SEPT-2000  / Vers. Vers. 1.01h
	{4, 8, 0x38,  {0,32,64,96}},  // 26:  4 x  8              NJU 6426  (New Japan Radio Co. Ltd.)   <===  ab. 28-SEPT-2000  / Vers. Vers. 1.01h
	{4,16, 0x38,  {0,32,64,96}},  // 27:  4 x 16              NJU 6426  (New Japan Radio Co. Ltd.)   <===  ab. 28-SEPT-2000  / Vers. Vers. 1.01h
	{4,20, 0x38,  {0,32,64,96}},  // 28:  4 x 20              NJU 6426  (New Japan Radio Co. Ltd.)   <===  ab. 28-SEPT-2000  / Vers. Vers. 1.01h




//		;  N u r    f u e r     T e s t s
//		;  N u r    f u e r     T e s t s
	{4,32, 0x30,  {0,32,64,96}},  // 29:  4 x 32   N=0, E1=0, E0=0    NJU 6426  (New Japan Radio Co)  Tests mit neuen LCDs
	{4,32, 0x31,  {0,32,64,96}},  // 30:  4 x 32   N=0, E1=0, E0=1    NJU 6426  (New Japan Radio Co)  Tests mit neuen LCDs
	{4,32, 0x32,  {0,32,64,96}},  // 31:  4 x 32   N=0, E1=1, E0=0    NJU 6426  (New Japan Radio Co)  Tests mit neuen LCDs
	{4,32, 0x33,  {0,32,64,96}},  // 32:  4 x 32   N=0, E1=1, E0=1    NJU 6426  (New Japan Radio Co)  Tests mit neuen LCDs
	{4,32, 0x38,  {0,32,64,96}},  // 33:  4 x 32   N=1, E1=0, E0=0    NJU 6426  (New Japan Radio Co)  Tests mit neuen LCDs
	{4,32, 0x39,  {0,32,64,96}},  // 34:  4 x 32   N=1, E1=0, E0=1    NJU 6426  (New Japan Radio Co)  Tests mit neuen LCDs
	{4,32, 0x3a,  {0,32,64,96}},  // 35:  4 x 32   N=1, E1=1, E0=0    NJU 6426  (New Japan Radio Co)  Tests mit neuen LCDs
	{4,32, 0x3b,  {0,32,64,96}},  // 36:  4 x 32   N=1, E1=1, E0=1    NJU 6426  (New Japan Radio Co)  Tests mit neuen LCDs
//		;  N u r    f u e r     T e s t s
//		;  N u r    f u e r     T e s t s


	{4,20, 0x38,  {0,32,64,96}},  // 37:	4 x 20    1:??    EA DIP204-4 Electronic Assembly  <===  ab. 17-Feb-2004

	{3,16, 0x39,  {0,16,32, 0}},  // 38:	3 x 16    EA DOGM163x-A  8-Bit  5V    <=== ab 08-Dez-2004   Anpassung fuer Kunde  Engel
	{4,20, 0,     {0,32,64,96}},  // 39:	4 x 20    EA DIP203B-4NLW mit SSD1803 Controller  <===  ab. 09-Dez-2014
};
#define MAX_LCD_DISP	(sizeof(lcd_table)/sizeof(lcd_t))

static uint8_t lcd_type = DEFAULT_LCD_TYPE; // 4x20
lcd_t* lcd = &lcd_table[DEFAULT_LCD_TYPE];

void set_isep_tab_10(bool flag, uint8_t min, uint8_t max)
{
    if(max < min){return;}
    
    uint8_t len = max - min;
    len++;

    do
    {
        RAM1_ISEP_TAB[min++] = flag;
        len--;
    }while(len != 0);
}


void add_to_obuf_more(bool is_control, uint8_t ch)
{
	if(o_buf_more.cnt + 2 > LEN)
	{
		return;
	}
	
	o_buf_more.buf[ o_buf_more.tail++ ] = (uint8_t)is_control;
	o_buf_more.tail &= SMASK;
	
	o_buf_more.buf[ o_buf_more.tail++ ] = (uint8_t)ch;
	o_buf_more.tail &= SMASK;
	o_buf_more.cnt += 2;
}

static void add_to_obuf(const void *data, size_t n_bytes)
{
	
	o_buf.busy = true;

	if( n_bytes > LEN - o_buf.cnt)
	{
		o_buf.busy = false;
		mp_printf(&mp_plat_print,"Data len: %d, Buff Cnt: %d, Buff Head: %d, Buff Tail: %d\n",n_bytes, o_buf.cnt, o_buf.head, o_buf.tail );
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Buffer is full"));
	}
	
	size_t free = LEN - o_buf.tail;
	
	const char *temp_data = (const char *)data;
	
	if(free >= n_bytes)
	{
		
		memcpy(o_buf.buf + o_buf.tail , temp_data, n_bytes);
		
		o_buf.tail += n_bytes;
		
	}
	else
	{
		
		memcpy(o_buf.buf + o_buf.tail , data, free);
		
		o_buf.tail = 0;
		
		memcpy(o_buf.buf + o_buf.tail , temp_data + free, n_bytes-free);
		o_buf.tail += (n_bytes-free);
	
	}
	o_buf.tail &= SMASK;
	
	
	mp_uint_t irq_state = MICROPY_BEGIN_ATOMIC_SECTION();
	o_buf.cnt += n_bytes;
	MICROPY_END_ATOMIC_SECTION(irq_state);

	o_buf.busy = false;
}

bool check_only_1_bit_is_set(uint8_t val)
{
	switch(val)
	{
		case 0x01:
		case 0x02:
		case 0x04:
		case 0x08:
		case 0x10:
		case 0x20:
		case 0x40:
		case 0x80:
		return true;
	}
	return false;
}

bool chk_1bit_mask_or0(uint8_t val)
{
	if(val == 0) return true;
	else return check_only_1_bit_is_set(val);
}


#define INSTALLED	0x1A	//; Flag: "Installed"
#define NOT_INSTALLED	0xAE	//; Flag: "Not Installed"

static bool process_control_char(uint8_t* ch)
{
	if(!lcd) return false;
	
    switch(*ch)
    {
        case 0x00: // click sound
            beep_on(BEEP_CLICK);
            return false;
        case 0x01: // clear
        case 0x02: // home
            lcd_x = lcd_y = 0;
            return true;
        case 0x05: // right
            inc_pos();
            *ch = calc_pos();
            return true;
        case 0x07: // beep sound
            beep_on(BEEP_BEEP);
            return false;
        case 0x08: // bs
            if(lcd_x==0)
            {
                lcd_x = lcd->w - 1;
                if(lcd_y==0)
                {
                    lcd_y = lcd->h - 1;
                }
                else
                {
                    --(lcd_y);
                }
            }
            else
            {
                --(lcd_x);
            }
            *ch = calc_pos();
            return true;
        case 0x0a: // down
            if(++lcd_y >= lcd->h)
            {
                lcd_y = 0;
            }
            *ch = calc_pos();
            return true;
        case 0x0b: // up
            if(lcd_y == 0)
            {
                return false;
            }
            --lcd_y;
            *ch = calc_pos();
            return true;
        case 0x0c: // ff = clear screen
            lcd_x = lcd_y = 0;
            *ch = 0x01;
            return true;
        case 0x0d: // cr
            if(lcd_x == 0)
            {
                return false;
            }
            lcd_x = 0;
            *ch = calc_pos();
            return true;
        case 0x14: // alarm sound
            beep_on(BEEP_ALARM);
            return false;
    }

    return false;
}

static uint8_t bell_counter = 9;
static uint8_t lcd_busy_cnt = 1;    // BUSY-Count-Down
#define _LCD_EXE_SLOW 4 
	
static void ddr_int(void)
{
	uint8_t ch = 0;
	
	if(kbd_on)
	{
		kbd_scan();
	}
	
	if(bp_on && ++bell_counter == 10)
	{
        if(beep_process() == OUT_HIGH)
        {
            if(RAM1_BELL_TFFLG == true)
            {
                 mp_hal_pin_low(pin_beep);
            }
            else
            {
                mp_hal_pin_high(pin_beep);
            }
        }
        else
        {
            if(RAM1_BELL_TFFLG == true)
            {
                mp_hal_pin_high(pin_beep);
            }
            else
            {
                mp_hal_pin_low(pin_beep);
            }
        }
    
        bell_counter = 0;		
	}
	
	
	if(--lcd_busy_cnt == 0)
	{
		lcd_busy_cnt = 1;
		if( o_buf_more.cnt > 1 )
		{	
			uint8_t is_control = o_buf_more.buf[o_buf_more.head++];
			ch 				   = o_buf_more.buf[o_buf_more.head++];

			o_buf_more.head &= SMASK;
			o_buf_more.cnt -= 2;
			
			if(is_control)
            {
                if(pin_rs != NULL)
                {
                    mp_hal_pin_write(pin_rs, 0);
                }
                if(ch < _LCD_EXE_SLOW)
                {
                    lcd_busy_cnt = 5;
                }
            }
            else
            {
                if(pin_rs != NULL)
                {
                    mp_hal_pin_write(pin_rs, 1); // data
                }
            }
			
		}
		else if(o_buf.cnt > 0 && o_buf.busy == false)
		{
			ch = o_buf.buf[o_buf.head++];
			o_buf.head &= SMASK;
			o_buf.cnt--;
			if(ch == 27 || in_esc) // esc
			{
				if(!process_esc_char(&ch))
				{
					return;
				}

				if(pin_rs != NULL)
				{
					mp_hal_pin_write(pin_rs, 0);
				}
			} 
			else if(ch < ' ') // control chars
			{
				if(!process_control_char(&ch))
				{
					return;
				}
				if(pin_rs != NULL)
				{
					mp_hal_pin_write(pin_rs, 0);
				}
				if(ch < _LCD_EXE_SLOW)
				{
					lcd_busy_cnt = 5;
				}
			}
			else // regular char
			{
				if(pin_rs != NULL)
				{
					mp_hal_pin_write(pin_rs, 1);
				}
				inc_pos();

				// special charsets
				if(ch >= 0x80 && RAM1_LCD_SONDER)
				{
					set_special_charset(ch >> 3);
					ch &= 0x07;
				}
			}
		}
		else if(o_buf.busy == true)
		{
			mp_printf(&mp_plat_print,"BUSY\n" );
		}
		else
		{
			return;
		}
		if(lcd_on)
		{		
			mp_uint_t irq_state = MICROPY_BEGIN_ATOMIC_SECTION();
			
			DELAY_CONFIG(TIM10);
			
			*portData = ch;
				
			DELAY_SET(TIM10, TIMINGS_DCLK);
			
			mp_hal_pin_toggle(pin_e);	
			
			DELAY_WAIT(TIM10);
			
			mp_hal_pin_toggle(pin_e);	
			
			MICROPY_END_ATOMIC_SECTION(irq_state);
			
		}
		
	}
	
	
}

static mp_obj_t tiger_lcd1_reset(mp_obj_t self_in) 
{
	lcd_init(lcd_type);
	lcd_init(lcd_type);
	
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(tiger_lcd1_reset_obj, tiger_lcd1_reset);

static mp_obj_t tiger_lcd1_write_string(mp_obj_t self_in, mp_obj_t data) 
{
	if(mp_obj_is_str(data))
	{
		size_t len;
	
		const char* str = mp_obj_str_get_data(data, &len);
	
		add_to_obuf(str,len);
		
	}
	else if(mp_obj_is_type(data, &mp_type_bytes))
	{
		uint8_t temp[1];
		
		mp_buffer_info_t bufinfo;
		
		pyb_buf_get_for_send(data, &bufinfo, temp);
		
		add_to_obuf(bufinfo.buf, bufinfo.len);
	}	
	else
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Parameter must be a string"));
	}
	
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(tiger_lcd1_write_string_obj, tiger_lcd1_write_string);

static mp_obj_t tiger_lcd1_clear(mp_obj_t self_in) 
{
	uint8_t cmd = 0x01;
	add_to_obuf(&cmd,1);
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(tiger_lcd1_clear_obj, tiger_lcd1_clear);

static mp_obj_t tiger_lcd1_cursor_home(mp_obj_t self_in) 
{
	uint8_t cmd = 0x02;
	add_to_obuf(&cmd,1);
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(tiger_lcd1_cursor_home_obj, tiger_lcd1_cursor_home);

static mp_obj_t tiger_lcd1_cursor_right(mp_obj_t self_in) 
{
	uint8_t cmd = 0x05;
	add_to_obuf(&cmd,1);
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(tiger_lcd1_cursor_right_obj, tiger_lcd1_cursor_right);

static mp_obj_t tiger_lcd1_cursor_left(mp_obj_t self_in) 
{
	uint8_t cmd = 0x08;
	add_to_obuf(&cmd,1);
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(tiger_lcd1_cursor_left_obj, tiger_lcd1_cursor_left);

static mp_obj_t tiger_lcd1_cursor_down(mp_obj_t self_in) 
{
	uint8_t cmd = 0x0A;
	add_to_obuf(&cmd,1);
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(tiger_lcd1_cursor_down_obj, tiger_lcd1_cursor_down);

static mp_obj_t tiger_lcd1_cursor_up(mp_obj_t self_in) 
{
	uint8_t cmd = 0x0B;
	add_to_obuf(&cmd,1);
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(tiger_lcd1_cursor_up_obj, tiger_lcd1_cursor_up);

static mp_obj_t tiger_lcd1_cursor_position(mp_obj_t self_in, mp_obj_t x, mp_obj_t y) 
{
	uint8_t pos_x = mp_obj_int_get_uint_checked(x);
	uint8_t pos_y = mp_obj_int_get_uint_checked(y);
	
	uint8_t cmd[] = {0x1B,'A', pos_x, pos_y, 0xF0};
	
	add_to_obuf(cmd, 5);
	
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(tiger_lcd1_cursor_position_obj, tiger_lcd1_cursor_position);

static mp_obj_t tiger_lcd1_activate_special(mp_obj_t self_in, mp_obj_t n) 
{
	const uint8_t ch = mp_obj_int_get_uint_checked(n);
	
	if(ch > 15)
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Charset must be a number between 0 and 15"));
	}
	
	const uint8_t cmd[4] = {0x1B,'S', ch, 0xF0};
	
	add_to_obuf(cmd, 4);
	
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(tiger_lcd1_activate_special_obj, tiger_lcd1_activate_special);

static mp_obj_t tiger_lcd1_load_special(mp_obj_t self_in, mp_obj_t n, mp_obj_t cset) 
{
	uint8_t ch = mp_obj_int_get_uint_checked(n);
	
	if(ch > 15)
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Charset must be a number between 0 and 15"));
	}
	
	if(!mp_obj_is_type(cset, &mp_type_bytes))
	{
		mp_raise_msg_varg(&mp_type_TypeError, MP_ERROR_TEXT("Parameter must be of type bytes"));
	}

	uint8_t data[1];
	
	mp_buffer_info_t bufinfo;
	
	pyb_buf_get_for_send(cset, &bufinfo, data);
	
	if(bufinfo.len != 64)
	{
		mp_raise_msg_varg(&mp_type_MemoryError, MP_ERROR_TEXT("Charset must have 64 bytes"));
	}
	
	uint8_t cmd[4] = {0x1B, 'L',ch, 0xF0};
	
	add_to_obuf(cmd, 3);
	add_to_obuf(bufinfo.buf, 64);
	add_to_obuf(&(cmd[3]), 1);
	
	
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(tiger_lcd1_load_special_obj, tiger_lcd1_load_special);

static mp_obj_t tiger_lcd1_reset_special(mp_obj_t self_in, mp_obj_t n) 
{
	uint8_t ch = mp_obj_int_get_uint_checked(n);
	
	uint8_t cmd[] = {0x1B,'R', ch, 0xF0};
	
	add_to_obuf(cmd, 4);
	
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(tiger_lcd1_reset_special_obj, tiger_lcd1_reset_special);

/*
static mp_obj_t tiger_lcd1_menu(mp_obj_t self_in, mp_obj_t n) 
{
	tiger_lcd_obj_t *l = MP_OBJ_TO_PTR(self_in);
	
	uint8_t ch = mp_obj_int_get_uint_checked(n);
	
	if(ch > 15)
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Special Charset must be between 0 and 15"));
	}
	
	uint8_t cmd[] = {0x1B,'L', ch, 0xF0};
	
	add_to_obuf(cmd, 4);
	
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(tiger_lcd1_menu_obj, tiger_lcd1_menu);
*/

static mp_obj_t tiger_lcd1_cursor_off(mp_obj_t self_in) 
{
	const uint8_t cmd[] = {0x1B,'c',0,0xF0};
	
	add_to_obuf(cmd, 4);
	
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(tiger_lcd1_cursor_off_obj, tiger_lcd1_cursor_off);

static mp_obj_t tiger_lcd1_cursor_on(mp_obj_t self_in) 
{
	uint8_t cmd[] = {0x1B,'c',1,0xF0};
	
	add_to_obuf(cmd, 4);
	
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(tiger_lcd1_cursor_on_obj, tiger_lcd1_cursor_on);

static mp_obj_t tiger_lcd1_cursor_blink(mp_obj_t self_in) 
{
	uint8_t cmd[] = {0x1B,'c',2,0xF0};
	
	add_to_obuf(cmd, 4);
	
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(tiger_lcd1_cursor_blink_obj, tiger_lcd1_cursor_blink);

static mp_obj_t tiger_lcd1_enable_special(mp_obj_t self_in) 
{
	uint8_t cmd[] = {0x1B,'s',1,0xF0};
	
	add_to_obuf(cmd, 4);
	
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(tiger_lcd1_enable_special_obj, tiger_lcd1_enable_special);

static mp_obj_t tiger_lcd1_disable_special(mp_obj_t self_in) 
{
	uint8_t cmd[] = {0x1B,'s',0,0xF0};
	
	add_to_obuf(cmd, 4);
	
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(tiger_lcd1_disable_special_obj, tiger_lcd1_disable_special);

static void tiger_lcd1_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
	mp_printf(print, "LCD1(RS=%q, E=%q)\n", mp_hal_pin_name(pin_rs), mp_hal_pin_name(pin_e));
	uint8_t data_port = gpioPortData == GPIOH? 6:8;
	mp_printf(print, "Data line:");
	for(uint8_t i = 0; i < 8; i++)
	{
		mp_printf(print, " L%d%d", data_port,i);
	}
	mp_printf(print, "\n");
	if(bp_on)
	{
		mp_printf(print, "Beep Pin: %q\n", mp_hal_pin_name(pin_beep));
	}
	if(xports_flg)
	{
		mp_printf(print, "XPort Adress Clock: %q\n", mp_hal_pin_name(xport_aclk));
		mp_printf(print, "XPort Data Clock: %q\n", mp_hal_pin_name(xport_dclk));
		mp_printf(print, "XPort Input Enable: %q\n", mp_hal_pin_name(xport_ine));
		mp_printf(print, "Led Port: %x, Mask: %x\n", RAM1_SLED_PORT, RAM1_SLED_BMASK);
	}
	
}

static mp_obj_t tiger_lcd1_set_key_table(mp_obj_t self_in, mp_obj_t table) 
{
	if(!mp_obj_is_type(table, &mp_type_bytes))
	{
		mp_raise_msg_varg(&mp_type_TypeError, MP_ERROR_TEXT("Parameter must be of type bytes"));
	}

	uint8_t data[1];
	
	mp_buffer_info_t bufinfo;
	
	pyb_buf_get_for_send(table, &bufinfo, data);
	
	if(bufinfo.len != 128)
	{
		mp_raise_msg_varg(&mp_type_MemoryError, MP_ERROR_TEXT("Table must have 128 bytes"));
	}
	
	uint8_t cmd[] = {0x1B, 'Z', 0xF0};
	
	
	add_to_obuf(cmd, 2);
	add_to_obuf(bufinfo.buf, 128);
	add_to_obuf(&(cmd[2]), 1);	
	

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(tiger_lcd1_set_key_table_obj, tiger_lcd1_set_key_table);

static mp_obj_t tiger_lcd1_set_key_shift_table(mp_obj_t self_in, mp_obj_t table) 
{
	if(!mp_obj_is_type(table, &mp_type_bytes))
	{
		mp_raise_msg_varg(&mp_type_TypeError, MP_ERROR_TEXT("Parameter must be of type bytes"));
	}

	uint8_t data[1];
	
	mp_buffer_info_t bufinfo;
	
	pyb_buf_get_for_send(table, &bufinfo, data);
	
	if(bufinfo.len != 128)
	{
		mp_raise_msg_varg(&mp_type_MemoryError, MP_ERROR_TEXT("Table must have 128 bytes"));
	}
	
	uint8_t cmd[] = {0x1B, 'z', 0xF0};
	
	
	add_to_obuf(cmd, 2);
	add_to_obuf(bufinfo.buf, 128);
	add_to_obuf(&(cmd[2]), 1);	
	
	
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(tiger_lcd1_set_key_shift_table_obj, tiger_lcd1_set_key_shift_table);

static mp_obj_t tiger_lcd1_set_key_attr(mp_obj_t self_in, mp_obj_t table) 
{
	if(!mp_obj_is_type(table, &mp_type_bytes))
	{
		mp_raise_msg_varg(&mp_type_TypeError, MP_ERROR_TEXT("Parameter must be of type bytes"));
	}

	uint8_t data[1];
	
	mp_buffer_info_t bufinfo;
	
	pyb_buf_get_for_send(table, &bufinfo, data);
	
	if(bufinfo.len != 128)
	{
		mp_raise_msg_varg(&mp_type_MemoryError, MP_ERROR_TEXT("Table must have 128 bytes"));
	}
	
	uint8_t cmd[] = {0x1B, 'a', 0xF0};
	
	
	add_to_obuf(cmd, 2);
	add_to_obuf(bufinfo.buf, 128);
	add_to_obuf(&(cmd[2]), 1);	
	
	
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(tiger_lcd1_set_key_attr_obj, tiger_lcd1_set_key_attr);

static mp_obj_t tiger_lcd1_key_click_on(mp_obj_t self_in) 
{
	uint8_t cmd[] = {0x1B,'K', 0x00, 0xF0};
	
	
	add_to_obuf(cmd, 4);
	
	
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(tiger_lcd1_key_click_on_obj, tiger_lcd1_key_click_on);

static mp_obj_t tiger_lcd1_key_click_off(mp_obj_t self_in) 
{
	uint8_t cmd[] = {0x1B,'K', 0xFF, 0xF0};
	
	
	add_to_obuf(cmd, 4);
	
	
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(tiger_lcd1_key_click_off_obj, tiger_lcd1_key_click_off);

static mp_obj_t tiger_lcd1_set_keyboard_repeat(mp_obj_t self_in, mp_obj_t n1, mp_obj_t n2) 
{
	uint8_t delay_n1 = mp_obj_int_get_uint_checked(n1);
	uint8_t delay_n2 = mp_obj_int_get_uint_checked(n2);
	
	uint8_t cmd[] = {0x1B,'r', delay_n1, delay_n2, 0xF0};
	
	
	add_to_obuf(cmd, 5);
	
	
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(tiger_lcd1_set_keyboard_repeat_obj, tiger_lcd1_set_keyboard_repeat);

static mp_obj_t tiger_lcd1_set_dip_switches(mp_obj_t self_in, mp_obj_t dips) 
{
	if (!mp_obj_is_type(dips, &mp_type_list)) 
	{
        mp_raise_msg_varg(&mp_type_TypeError, MP_ERROR_TEXT("Parameter must be a list"));
    }
	
	mp_obj_list_t *list = MP_OBJ_TO_PTR(dips);
	
	if (list->len != 16) 
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Parameter must have a len of 16"));
    }
	
    uint8_t col[16];
    for (size_t i = 0; i < 16; ++i)
    {
            if (!mp_obj_is_int(list->items[i]))
            {
                    mp_raise_msg_varg(&mp_type_TypeError, MP_ERROR_TEXT("Parameter must be a list of integer"));
            }
            col[i] = mp_obj_get_int(list->items[i]);
    }

	
	uint8_t cmd[4] = {0x1B, 'D', 16, 0xF0};
	
	
	add_to_obuf(cmd, 3);
	add_to_obuf(col, 16);
	add_to_obuf(&(cmd[3]), 1);	
	

    return mp_const_none;
} 
static MP_DEFINE_CONST_FUN_OBJ_2(tiger_lcd1_set_dip_switches_obj, tiger_lcd1_set_dip_switches);

static mp_obj_t tiger_lcd1_get_kbd_buf_size(mp_obj_t self_in) 
{
    return MP_OBJ_NEW_SMALL_INT(kbd_buf.cnt);
} 
static MP_DEFINE_CONST_FUN_OBJ_1(tiger_lcd1_get_kbd_buf_size_obj, tiger_lcd1_get_kbd_buf_size);

static mp_obj_t tiger_lcd1_read_kbd(mp_obj_t self_in, mp_obj_t bytes) 
{
	
	uint8_t len = mp_obj_int_get_uint_checked(bytes);
	if(kbd_buf.cnt == 0)
	{
		return mp_const_none;
	}
	if(kbd_buf.cnt < len)
	{
		len = kbd_buf.cnt;
	}
	
	kbd_buf.busy = true;
	
	
    vstr_t vstr;
    vstr_init(&vstr, len);

    if (!read_from_kbd_buf((uint8_t *)vstr.buf, len)) {
            mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed buffer operation"));
    }
    vstr.len = len;
    kbd_buf.busy = false;
	
  
	
    return mp_obj_new_bytes_from_vstr(&vstr);
    
} 
static MP_DEFINE_CONST_FUN_OBJ_2(tiger_lcd1_read_kbd_obj, tiger_lcd1_read_kbd);



/// \classmethod \constructor(skin_position)
///
/// Construct an LCD object in the given skin position.  `skin_position` can be 'X' or 'Y', and
/// should match the position where the LCD pyskin is plugged in.
static mp_obj_t tiger_lcd_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) 
{
      
	enum { ARG_type, ARG_data_port, ARG_e, ARG_rs, ARG_bp, ARG_bp_pull, ARG_led_mask, ARG_led_port, ARG_led_pull, ARG_bp_on, ARG_kbd_on, ARG_lcd_on, ARG_pin_aclk, ARG_pin_ine, ARG_pin_dclk };
	
	
	static const mp_arg_t allowed_args[] = {
	{ MP_QSTR_type,        					  MP_ARG_INT, {.u_int = DEFAULT_LCD_TYPE}    },
	{ MP_QSTR_data_port,    				  MP_ARG_INT, {.u_int = 6}                   },
	{ MP_QSTR_pin_e,      					  MP_ARG_OBJ, {.u_rom_obj = pyb_pin_LCD1_E}  },
	{ MP_QSTR_pin_rs,    					  MP_ARG_OBJ, {.u_rom_obj = pyb_pin_LCD1_RS} },
	{ MP_QSTR_pin_beep,       				  MP_ARG_OBJ, {.u_rom_obj = pyb_pin_L42}	 },
	{ MP_QSTR_beep_pull_up,                   MP_ARG_BOOL,{.u_bool = true} 			     },
	{ MP_QSTR_led_mask,     				  MP_ARG_INT, {.u_int = 1} 				     },
	{ MP_QSTR_led_port,     				  MP_ARG_INT, {.u_int = 0x10} 				 },
	{ MP_QSTR_led_pull_up,                    MP_ARG_BOOL,{.u_bool = true} 			     },
	{ MP_QSTR_beep_on,                        MP_ARG_BOOL,{.u_bool = false}			     },
	{ MP_QSTR_kbd_on,                         MP_ARG_BOOL,{.u_bool = true} 			     },
	{ MP_QSTR_lcd_on,                         MP_ARG_BOOL,{.u_bool = true} 			     },
	{ MP_QSTR_pin_aclk,      			      MP_ARG_OBJ, {.u_rom_obj = pyb_pin_XP_ACLK} },
	{ MP_QSTR_pin_ine,    					  MP_ARG_OBJ, {.u_rom_obj = pyb_pin_XP_INE}  },
	{ MP_QSTR_pin_dclk,       				  MP_ARG_OBJ, {.u_rom_obj = pyb_pin_XP_DCLK} },
    };

	//mp_arg_check_num(n_args, n_kw, 0, MP_ARRAY_SIZE(allowed_args), true); Is it needed?
	
	mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
	
	if(inst_ok)
	{
		del1mInt(interrupt);
	}
	
	lcd_type = args[ARG_type].u_int;
	
	mp_printf(&mp_plat_print, "args: %d, kwargs: %d, arg  pin:%d \n", n_args, n_kw, ARG_pin_aclk);

	if( lcd_type == 0 || lcd_type >= MAX_LCD_DISP )
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Wrong LCD type!"));
	}
	
	lcd = &(lcd_table[lcd_type]);
	lcd_x = 0;
	lcd_y = 0;
	
	
	switch(args[ARG_data_port].u_int)
	{
		case 6:
				gpioPortData = DEFAULT_PORT_BUS;
				break;
		case 8:
				gpioPortData = PORT_BUS_8;
				break;
		default:
				mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("port must be 6 or 8!"));
	}
	
	if(n_args < ARG_pin_aclk) kbd_on = false;
	
	mp_hal_gpio_clock_enable(gpioPortData);
	portData = portOut(gpioPortData, true);
	portDataIn = portIn(gpioPortData, true);
	portDataControl = portControl(gpioPortData, true);
	
	
	mp_printf(&mp_plat_print, "portOut: %x\n", portData);
	mp_printf(&mp_plat_print, "portDataIn: %x\n", portDataIn);
	mp_printf(&mp_plat_print, "portDataControl: %x\n", portDataControl);
	DATA_PORT_TO_OUTPUT(portDataControl);
	

	mp_obj_t pin_e_obj  = args[ARG_e].u_obj;
	mp_obj_t pin_rs_obj = args[ARG_rs].u_obj;
	mp_obj_t pin_bp_obj = args[ARG_bp].u_obj;
	

	if(mp_obj_is_str(pin_e_obj))
	{
		pin_e = pin_find_named_pin(&machine_pin_board_pins_locals_dict, pin_e_obj);
	}
	else
	{
		pin_e = pin_find(pin_e_obj);
	}
	

	if(mp_obj_is_str(pin_rs_obj))
	{
		pin_rs = pin_find_named_pin(&machine_pin_board_pins_locals_dict, pin_rs_obj);
	}
	else
	{
		pin_rs = pin_find(pin_rs_obj);
	}
	

	if(mp_obj_is_str(pin_bp_obj))
	{
		pin_beep = pin_find_named_pin(&machine_pin_board_pins_locals_dict, pin_bp_obj);
	}
	else
	{
		pin_beep = pin_find(pin_bp_obj);
	}
	
	RAM1_BELL_TFFLG = args[ARG_bp_pull].u_bool;
	
	if(!chk_1bit_mask_or0(args[ARG_led_mask].u_int))
	{
		mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Invalid LED mask"));
	}
	
	RAM1_SLED_BMASK = args[ARG_led_mask].u_int;
	
	RAM1_SLED_PORT = args[ARG_led_port].u_int;
	
	RAM1_SLED_TFFLG = args[ARG_led_pull].u_bool;
	
	kbd_on = args[ARG_kbd_on].u_bool;
	
	lcd_on = args[ARG_lcd_on].u_bool;
	
	bp_on = args[ARG_bp_on].u_bool;
	 

	mp_obj_t pin_aclk_obj  = args[ARG_pin_aclk].u_obj;
	mp_obj_t pin_ine_obj = args[ARG_pin_ine].u_obj;
	mp_obj_t pin_dclk_obj = args[ARG_pin_dclk].u_obj;
	

	if(mp_obj_is_str(pin_aclk_obj))
	{
		xport_aclk = pin_find_named_pin(&machine_pin_board_pins_locals_dict, pin_aclk_obj);
	}
	else
	{
		xport_aclk = pin_find(pin_aclk_obj);
	}

	if(mp_obj_is_str(pin_ine_obj))
	{
		xport_ine = pin_find_named_pin(&machine_pin_board_pins_locals_dict, pin_ine_obj);
	}
	else
	{
		xport_ine = pin_find(pin_ine_obj);
	}
	

	if(mp_obj_is_str(pin_dclk_obj))
	{
		xport_dclk = pin_find_named_pin(&machine_pin_board_pins_locals_dict, pin_dclk_obj);
	}
	else
	{
		xport_dclk = pin_find(pin_dclk_obj);
	}
	
	
	if(lcd_on)
	{
		mp_hal_pin_output(pin_e);
		mp_hal_pin_low(pin_e);
		mp_hal_pin_output(pin_rs);
	}
	
	if(bp_on)
	{
		mp_hal_pin_output(pin_beep);
	
		RAM1_BELL_TFFLG ? mp_hal_pin_low(pin_beep) : mp_hal_pin_high(pin_beep); //it might be inverted here
	}
	
	set_isep_tab_10(false, 0, 0xFF);
    set_isep_tab_10(true, 0, 0x1F);
    set_isep_tab_10(1,',',',');
	
	
	if(kbd_on)
	{
		mp_hal_pin_output(xport_aclk);
		mp_hal_pin_output(xport_dclk);
		mp_hal_pin_output(xport_ine);
		mp_hal_pin_high(xport_ine);
	}	

	memset(o_buf.buf, 0, LEN);
	
	o_buf.head = 0;
	o_buf.tail = 0;
	o_buf.cnt = 0;
	o_buf.busy = false;
	
	memset(o_buf_more.buf, 0, LEN);
	
    o_buf_more.head = 0;
	o_buf_more.tail = 0;
	o_buf_more.cnt = 0;
	o_buf_more.busy = false;
	
	memset(kbd_buf.buf, 0, LEN);
	
    kbd_buf.head = 0;
	kbd_buf.tail = 0;
	kbd_buf.cnt = 0;
	kbd_buf.busy = false;

 
	kbd_init();
	
	if(lcd_on)
	{
		lcd_init(lcd_type);
		lcd_init(lcd_type);
	}	
	
	if(bp_on)
	{
		beep_init();
	}
	
	esc_init();
	
	eport_phys_offs = 0xF8;
	
	tiger_lcd_obj.base.type = &tiger_lcd_type;
	
	if(!inst_ok)
	{
		interrupt = add1msInt(ddr_int);
	}
	
    return MP_OBJ_FROM_PTR(&tiger_lcd_obj);
}



static const mp_rom_map_elem_t tiger_lcd1_locals_dict_table[] = {
    // instance methods
	{ MP_ROM_QSTR(MP_QSTR_writeString), MP_ROM_PTR(&tiger_lcd1_write_string_obj)                       },
	{ MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&tiger_lcd1_reset_obj)                                    },
	{ MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&tiger_lcd1_clear_obj) 	                               },
	{ MP_ROM_QSTR(MP_QSTR_cursorHome), MP_ROM_PTR(&tiger_lcd1_cursor_home_obj)                         },
	{ MP_ROM_QSTR(MP_QSTR_cursorRight), MP_ROM_PTR(&tiger_lcd1_cursor_right_obj)                       },
	{ MP_ROM_QSTR(MP_QSTR_cursorLeft), MP_ROM_PTR(&tiger_lcd1_cursor_left_obj)                         },
	{ MP_ROM_QSTR(MP_QSTR_cursorDown), MP_ROM_PTR(&tiger_lcd1_cursor_down_obj)                         },
	{ MP_ROM_QSTR(MP_QSTR_cursorUp), MP_ROM_PTR(&tiger_lcd1_cursor_up_obj)                             },
	{ MP_ROM_QSTR(MP_QSTR_cursorPosition), MP_ROM_PTR(&tiger_lcd1_cursor_position_obj)                 },
	{ MP_ROM_QSTR(MP_QSTR_activateSpecialCharset), MP_ROM_PTR(&tiger_lcd1_activate_special_obj)        },
	{ MP_ROM_QSTR(MP_QSTR_resetSpecialCharset), MP_ROM_PTR(&tiger_lcd1_reset_special_obj)              },
	{ MP_ROM_QSTR(MP_QSTR_cursorOff), MP_ROM_PTR(&tiger_lcd1_cursor_off_obj)                           },
	{ MP_ROM_QSTR(MP_QSTR_cursorOn), MP_ROM_PTR(&tiger_lcd1_cursor_on_obj)                             },
	{ MP_ROM_QSTR(MP_QSTR_cursorBlink), MP_ROM_PTR(&tiger_lcd1_cursor_blink_obj)                       },
	{ MP_ROM_QSTR(MP_QSTR_enableSpecialCharset), MP_ROM_PTR(&tiger_lcd1_enable_special_obj)            },
	{ MP_ROM_QSTR(MP_QSTR_disableSpecialCharset), MP_ROM_PTR(&tiger_lcd1_disable_special_obj)          },
	{ MP_ROM_QSTR(MP_QSTR_loadSpecialCharset), MP_ROM_PTR(&tiger_lcd1_load_special_obj)                },
	{ MP_ROM_QSTR(MP_QSTR_keyboardSetKeyTable), MP_ROM_PTR(&tiger_lcd1_set_key_table_obj)              },
	{ MP_ROM_QSTR(MP_QSTR_keyboardSetKeyShiftTable), MP_ROM_PTR(&tiger_lcd1_set_key_shift_table_obj)   },
	{ MP_ROM_QSTR(MP_QSTR_keyboardSetKeyAttributeCode),MP_ROM_PTR(&tiger_lcd1_set_key_attr_obj)        },
	{ MP_ROM_QSTR(MP_QSTR_keyboardKeyClickOn),MP_ROM_PTR(&tiger_lcd1_key_click_on_obj)                 },
	{ MP_ROM_QSTR(MP_QSTR_keyboardKeyClickOff),MP_ROM_PTR(&tiger_lcd1_key_click_off_obj)               },
	{ MP_ROM_QSTR(MP_QSTR_keyboardSetRepeat),MP_ROM_PTR(&tiger_lcd1_set_keyboard_repeat_obj)           },
	{ MP_ROM_QSTR(MP_QSTR_keyboardSetDipSwitch),MP_ROM_PTR(&tiger_lcd1_set_dip_switches_obj)           },
	{ MP_ROM_QSTR(MP_QSTR_getKeyboardBufferSize),MP_ROM_PTR(&tiger_lcd1_get_kbd_buf_size_obj)          },
	{ MP_ROM_QSTR(MP_QSTR_readKeyboard),MP_ROM_PTR(&tiger_lcd1_read_kbd_obj)              			   }
	
	
   /* { MP_ROM_QSTR(MP_QSTR_command), MP_ROM_PTR(&pyb_lcd_command_obj) },
    { MP_ROM_QSTR(MP_QSTR_contrast), MP_ROM_PTR(&pyb_lcd_contrast_obj) },
    { MP_ROM_QSTR(MP_QSTR_light), MP_ROM_PTR(&pyb_lcd_light_obj) },
    tiger_lcd1_activate_special
    { MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&pyb_lcd_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_get), MP_ROM_PTR(&pyb_lcd_get_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&pyb_lcd_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&pyb_lcd_text_obj) },
    { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&pyb_lcd_show_obj) },*/
};

static MP_DEFINE_CONST_DICT(tiger_lcd1_locals_dict, tiger_lcd1_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    tiger_lcd_type, //the new type defined
    MP_QSTR_LCD1,	//MP_QSTR_ + the name of the module
    MP_TYPE_FLAG_NONE, 
    make_new, tiger_lcd_make_new, //the constructor of the module
	print, tiger_lcd1_print, //the behavior of the print function towards the object
    locals_dict, &tiger_lcd1_locals_dict //dictionary the constants and methods of the module
    );

#endif // MICROPY_HW_HAS_LCD
