/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _ST7565_H
#define _ST7565_H

#define ST7565_CONTRAST             0x81
#define ST7565_ALLON_NORMAL         0xA4
#define ST7565_ALLON                0xA5
#define ST7565_POSITIVE_DISPLAY     0xA6
#define ST7565_INVERT_DISPLAY       0xA7
#define ST7565_DISPLAY_OFF          0xAE
#define ST7565_DISPLAY_ON           0xAF

#define ST7565_LCD_BIAS_7           0xA3
#define ST7565_LCD_BIAS_9           0xA2

#define ST7565_ADC_NORMAL           0xA0
#define ST7565_ADC_REVERSE          0xA1

#define ST7565_COM_SCAN_INC         0xC0
#define ST7565_COM_SCAN_DEC         0xC8

#define ST7565_START_LINE           0x40
#define ST7565_PAGE                 0xB0
#define ST7565_COLUMN_MSB           0x10
#define ST7565_COLUMN_LSB           0x00
#define ST7565_RMW                  0xE0

#define ST7565_RESISTOR_RATIO       0x20
#define ST7565_POWER_CONTROL        0x28

#endif /* _ST7565_H */
