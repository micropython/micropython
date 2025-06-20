#include "lcd.h"

#if MICROPY_HW_HAS_LCD

bool RAM1_LCD_SONDER = true;
uint8_t lcd_x = 0;
uint8_t lcd_y = 0;

void lcd_disable_special_charset(uint8_t param)
{	
	RAM1_LCD_SONDER = (param == 0 ? false : true);
}

 uint8_t calc_pos(void)
{
	
	return 0x80 + lcd_x + lcd->line_offset[lcd_y];
}	

 void inc_pos(void)
{
	if(++(lcd_x) >= lcd->w)
	{	
		lcd_x = 0;
		
		if(++(lcd_y) >= lcd->h)
		{	
			lcd_y = 0;
		}

		// new line
		add_to_obuf_more(true, calc_pos());
	}
}

void lcd_init(uint8_t type)
{
	if(!lcd) return;
	
	uint8_t init_commands_default[] = {lcd->init_seq, lcd->init_seq, 0x0F, 0x06, 0x01, 0x00}; 
	uint8_t init_commands_lcd_37[]  = {0x34, 0x09, 0x30, 0x0F, 0x01, 0x06, 0x00};
	uint8_t init_commands_lcd_38[] = {0x39, 0x1d, 0x50, 0x6c, 0x7c, 0x0f, 0x01, 0x06, 0x00};
	uint8_t init_commands_lcd_39[] = {0x3c, 0x09, 0x38, 0x02, 0x0f, 0x01, 0x3c, 0x06, 0x38, 0x00};

	uint8_t* cmd = init_commands_default;
	

	switch(type)
	{
		case 37:
			cmd = init_commands_lcd_37;
			break;
		case 38:
			cmd = init_commands_lcd_38;
			break;
		case 39:
			cmd = init_commands_lcd_39;
			break;
	}

	while(*cmd != 0x00)
	{
#ifdef LCD4BIT
		if(*cmd & 0x30)
		{
			*cmd &= ~0x10;
		}
#endif
		
		add_to_obuf_more(true, *cmd++);
	}
}

#endif