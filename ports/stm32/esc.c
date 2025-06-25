#include "esc.h"
#include "beep.h"
#include "kbd.h"
#include "lcd.h"
#include "ti-2def2.h"

#include "charset0.h"

#if MICROPY_HW_HAS_LCD

#define SMASK (LEN - 1)

static uint8_t special_charset = 0xff; // 0..15
static uint8_t charset_table[8*8*16];

void set_special_charset(uint8_t charset)
{
	charset &= 0xf;
	
	if(special_charset == charset)
    {
        return;
    }
	
    special_charset = charset;

    add_to_obuf_more(true, 0x40); // initial CGRAM address

    // send 8 * 8 bytes
    uint8_t* ptr = charset_table + special_charset*8*8;
    uint32_t i = 0;
    for(; i<8*8; ++i)
    {
        add_to_obuf_more(false, *ptr++);
    }

    add_to_obuf_more(true, calc_pos());  // back to display mode
}

void esc_init()
{
    uint32_t i;
    //  DDR_INST_LOG("Setting default special charset\n");
    // create the charset to use
	
    for(i=0; i<8*8*16; ++i)
    {
        charset_table[i] = charset0[i];
    }

    set_special_charset(0);
}

static void set_custom_charset(uint8_t charset, uint8_t *chars)
{
	charset &= 0xf;
	
    uint8_t* dest = charset_table+charset*8*8;

    uint32_t i = 0;
    for(; i<8*8; ++i)
    {
        dest[i] = chars[i];
    }

    special_charset = 0xff; // force re
    set_special_charset(charset);
}

static void reset_charset(uint8_t charset)
{
    charset &= 0xf;
	
    uint8_t* dest = charset_table+charset*8*8;
    uint8_t* src = (uint8_t*)charset0+charset*8*8;

    uint32_t i = 0;
    for(; i<8*8; ++i)
    {
        dest[i] = src[i];
    }

    special_charset = 0xff; // force reload
    set_special_charset(charset);
}

// 'M' esc
static void select_menu_entry(uint8_t index)
{
    if (o_buf.cnt == 0) {
        return;
    }

    uint8_t sep = o_buf.buf[o_buf.head++];
    o_buf.head &= SMASK;
    o_buf.cnt--;

    uint8_t default_len = 0;
    uint8_t current_index = 0;
    bool capturing = (index == 0);
    bool prev_sep = false;
    bool wrote = false;
    bool found = false;

    while (o_buf.cnt > 0) {
        uint8_t ch = o_buf.buf[o_buf.head++];
        o_buf.head &= SMASK;
        o_buf.cnt--;

        if (ch == sep) {
            if (prev_sep) {
                break;
            }

            prev_sep = true;

            if (capturing && wrote) {
                found = true;
            }

            current_index++;
            capturing = (current_index == index);
            wrote = false;
            continue;
        }

        prev_sep = false;

        if (current_index == 0) {
            default_len++;
        }

        if (capturing) {
            uint8_t out = ch;
            if (out >= 0x80) {
                out &= 0x7;
            }
            add_to_obuf_more(false, out);
            inc_pos();
            wrote = true;
        }
    }

    if (capturing && wrote) {
        found = true;
    }

    if (!found) {
        for (uint8_t k = 0; k < default_len; ++k) {
            add_to_obuf_more(false, ' ');
            inc_pos();
        }
    }
}


bool in_esc = false;


#define ESC_MODE_NEED_CMD       0
#define ESC_MODE_NEED_PARAM     1
#define ESC_MODE_NEED_EOS       2
static uint8_t esc_mode = ESC_MODE_NEED_CMD;
static uint8_t esc_cmd;
static uint8_t param_bytes_needed;
static uint8_t param_buffer_len;
static uint8_t param_buffer[128];
bool process_esc_char(uint8_t *ch)
{
    // must be 27 then
    if(!(in_esc))
    {
        esc_mode = ESC_MODE_NEED_CMD;
        in_esc = true;
        return false;
    }

    switch(esc_mode)
    {
        case ESC_MODE_NEED_CMD:
        {
            esc_mode = ESC_MODE_NEED_PARAM;
            esc_cmd = *ch;
            param_buffer_len = 0;

            switch(esc_cmd)
            {
                case 'A':
                case 'r':
                {
                    param_bytes_needed = 2;
                    return false;
                }
                case 'B':
                {
                    param_bytes_needed = 22;
                    return false;
                }
                case 'C':
                case 'S':
                case 'R':
                case 'M':
                case 'c':
                case 'K':
                case 's':
                {
                    param_bytes_needed = 1;
                    return false;
                }
                case 'O':
                {
                    param_bytes_needed = 4;
                    return false;
                }
                case 'L':
                {
                    param_bytes_needed = 1+64;
                    return false;
                }
                case 'Z':
                case 'z':
                case 'a':
                {
                    param_bytes_needed = 128;
                    return false;
                }
                case 'D':
                {
                    param_bytes_needed = 1 + 16;
                    return false;
                }
                case 'k':
                {
                    param_bytes_needed = 16;
                    return false;
                }
                default:
                {
                    in_esc = false;
                    return false;
                }
            }
        }

        case ESC_MODE_NEED_PARAM:
        {
            param_buffer[param_buffer_len++] = *ch;
            if(param_buffer_len == param_bytes_needed)
            {
                esc_mode = ESC_MODE_NEED_EOS;
            }
            return false;
        }

        case ESC_MODE_NEED_EOS:
        {
			in_esc = false;
		

			switch(esc_cmd)
			{
				case 'A': // cursor position: x, y
				{
					if(param_buffer[0] >= lcd->w || param_buffer[1] >= lcd->h)
					{
						return false;
					}
					lcd_x = param_buffer[0];
					lcd_y = param_buffer[1];
					*ch = calc_pos();
					return true;
				}
				case 'B': // set sound parameters
				{
					beep_set_params((uint16_t*)param_buffer);
					return false;
				}
				case 'C': // turn off sound
				{
					beep_off(param_buffer[0] - 1);
					return false;
				}
				case 'S': // turn on charset
				{
					set_special_charset(param_buffer[0]);
					return false;
				}
				case 'L': // load custom charset
				{
					set_custom_charset(param_buffer[0], &(param_buffer[1]));
					return false;
				}
				case 'R': // reset custom charset
				{
					reset_charset(param_buffer[0]);
					return false;
				}
                                case 'M': // menu entry selection
                                {
                                        select_menu_entry(param_buffer[0]);
                                        return false;
                                }
				case 'c': // cursor: 0:off, 1:on, else:on+blinking
				{
					switch(param_buffer[0])
					{
						case 0: *ch = 0x0c; return true;
						case 1: *ch = 0x0c | 0x2; return true;
					}
					*ch = 0x0c | 0x3;
					return true;
				}
				case 'r': // set keybard auto repeat params
				{
					kbd_set_auto_repeat(param_buffer[0], param_buffer[1]);
					break;
				}
				case 'Z': // keyboard key table
				{
					kbd_set_key_table(param_buffer);
					return false;
				}
				case 'z': // keyboard key shift table
				{
					kbd_set_key_shift_table(param_buffer);
					return false;
				}
				case 'a': // keyboard key attribute table
				{
					kbd_set_key_attribute_table(param_buffer);
					return false;
				}
				case 'O': // keyboard CTRL function
				{
					kbd_set_control(param_buffer);
					return false;
				}
				case 'K': // sound on keyboard input			; ist es "K"-Kommando ?
				{
					RAM1_KEY_CLICK = (param_buffer[0] == 0 ? true : false);
					return false;
				}
				case 'k': // keyboard column addresses
				{
					kbd_set_column_addresses(param_buffer);
					return false;
				}
				case 'D': // dip switches
				{
					kbd_set_dip_switches(param_buffer[0], &(param_buffer[1]));
					return false;
				}
				case 's': // disable special chars
				{
					lcd_disable_special_charset(param_buffer[0]);
					return false;
				}
			}

			return false;
		}
    }

    return false;
}




#endif