/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Afantor.lan
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

#ifndef LCD_H__
#define LCD_H__

#include "mpconfigboard.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"

//画笔颜色
#define LCD_WHITE         	 0xFFFF
#define LCD_BLACK         	 0x0000	  
#define LCD_BLUE           	 0x001F  
#define LCD_BRED             0XF81F
#define LCD_GRED 			 0XFFE0
#define LCD_GBLUE			 0X07FF
#define LCD_RED           	 0xF800
#define LCD_MAGENTA       	 0xF81F
#define LCD_GREEN         	 0x07E0
#define LCD_CYAN          	 0x7FFF
#define LCD_YELLOW        	 0xFFE0
#define LCD_BROWN 			 0XBC40 //棕色
#define LCD_BRRED 			 0XFC07 //棕红色
#define LCD_GRAY  			 0X8430 //灰色
//GUI颜色

#define LCD_DARKBLUE      	 0X01CF	//深蓝色
#define LCD_LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define LCD_GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LCD_IGHTGREEN     	 0X841F //浅绿色
#define LCD_LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色

#define LCD_LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LCD_LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)


// LCD TFT definitions for Bluefruit52
#define LCD_SDA_PIN  		7
#define LCD_SCL_PIN  		27
#define LCD_RES_PIN  		23
#define LCD_DC_PIN  		16
#define LCD_CS_PIN  		24

/*
#define USE_HORIZONTAL 3  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 135
#define LCD_H 240

#else
#define LCD_W 240
#define LCD_H 135
#endif

*/
//-----------------LCD端口定义---------------- 

#define LCD_SCLK_Clr() nrf_gpio_pin_clear(LCD_SCL_PIN)
#define LCD_SCLK_Set() nrf_gpio_pin_set(LCD_SCL_PIN)

#define LCD_SDIN_Clr() nrf_gpio_pin_clear(LCD_SDA_PIN)//DIN
#define LCD_SDIN_Set() nrf_gpio_pin_set(LCD_SDA_PIN)

#define LCD_RST_Clr() nrf_gpio_pin_clear(LCD_RES_PIN)//RES
#define LCD_RST_Set() nrf_gpio_pin_set(LCD_RES_PIN)

#define LCD_DC_Clr() nrf_gpio_pin_clear(LCD_DC_PIN)//DC
#define LCD_DC_Set() nrf_gpio_pin_set(LCD_DC_PIN)
 		     
#define LCD_CS_Clr() nrf_gpio_pin_clear(LCD_CS_PIN)//CS
#define LCD_CS_Set() nrf_gpio_pin_set(LCD_CS_PIN)

#define LCD_CMD  0	//写命令
#define LCD_DATA 1	//写数据

extern  uint16_t BACK_COLOR;   //背景色

extern uint8_t USE_HORIZONTAL;

extern uint8_t LCD_W;
extern uint8_t LCD_H;

extern const mp_obj_type_t machine_lcd_type;

void lcd_init(uint8_t dir);
void LCD_ShowEnable(void);
void LCD_ShowDisable(void);
void LCD_Writ_Bus(uint8_t dat);
void LCD_WR_DATA8(uint8_t dat);
void LCD_WR_DATA(uint16_t dat);
void LCD_WR_REG(uint8_t dat);
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void LCD_Clear(uint16_t Color);
void LCD_ShowChinese(uint16_t x,uint16_t y,uint8_t index,uint8_t size,uint16_t color);
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color);
void LCD_DrawBigPoint(uint16_t x,uint16_t y,uint16_t colory);
void LCD_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color);
void LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);
void LCD_DrawCircle(uint16_t x0,uint16_t y0,uint8_t r,uint16_t color);
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t mode,uint16_t color);
void LCD_ShowString(uint16_t x,uint16_t y,const uint8_t *p,uint16_t color);
uint32_t mypow(uint8_t m,uint8_t n);
void LCD_ShowNum(uint16_t x,uint16_t y,uint16_t num,uint8_t len,uint16_t color);
void LCD_ShowFloat(uint16_t x,uint16_t y,float num,uint8_t len,uint16_t color);
void LCD_ShowPicture(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void LCD_ShowLogo(void);
void LCD_ShowBLE(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
#endif // LCD_H__
