/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _STMPE811_H
#define _STMPE811_H

// Identification registers
#define STMPE811_REG_CHP_ID         0x00 // 16-bit
#define STMPE811_REG_ID_VER         0x02

// System registers
#define STMPE811_REG_SYS_CTRL1      0x03
#define STMPE811_REG_SYS_CTRL2      0x04
#define STMPE811_REG_SPI_CFG        0x08

// Interrupt control registers
#define STMPE811_REG_INT_CTRL       0x09
#define STMPE811_REG_INT_EN         0x0A
#define STMPE811_REG_INT_STA        0x0B
#define STMPE811_REG_GPIO_INT_EN    0x0C
#define STMPE811_REG_GPIO_INT_STA   0x0D
#define STMPE811_REG_ADC_INT_EN     0x0E
#define STMPE811_REG_ADC_INT_STA    0x0F

// GPIO registers
#define STMPE811_REG_GPIO_SET_PIN   0x10
#define STMPE811_REG_GPIO_CLR_PIN   0x11
#define STMPE811_REG_GPIO_MP_STA    0x12
#define STMPE811_REG_GPIO_DIR       0x13
#define STMPE811_REG_GPIO_ED        0x14
#define STMPE811_REG_GPIO_RE        0x15
#define STMPE811_REG_GPIO_FE        0x16
#define STMPE811_REG_GPIO_AF        0x17

// ADC registers
#define STMPE811_REG_ADC_CTRL1      0x20
#define STMPE811_REG_ADC_CTRL2      0x21
#define STMPE811_REG_ADC_CAPT       0x22
#define STMPE811_REG_ADC_DATA_CH0   0x30 // 16-bit
#define STMPE811_REG_ADC_DATA_CH1   0x32 // 16-bit
#define STMPE811_REG_ADC_DATA_CH2   0x34 // 16-bit
#define STMPE811_REG_ADC_DATA_CH3   0x36 // 16-bit
#define STMPE811_REG_ADC_DATA_CH4   0x38 // 16-bit
#define STMPE811_REG_ADC_DATA_CH5   0x3A // 16-bit
#define STMPE811_REG_ADC_DATA_CH6   0x3B // 16-bit
#define STMPE811_REG_ADC_DATA_CH7   0x3C // 16-bit

// Touchscreen registers
#define STMPE811_REG_TSC_CTRL       0x40
#define STMPE811_REG_TSC_CFG        0x41
#define STMPE811_REG_WDW_TR_X       0x42 // 16-bit
#define STMPE811_REG_WDW_TR_Y       0x44 // 16-bit
#define STMPE811_REG_WDW_BL_X       0x46 // 16-bit
#define STMPE811_REG_WDW_BL_Y       0x48 // 16-bit
#define STMPE811_REG_FIFO_TH        0x4A
#define STMPE811_REG_FIFO_STA       0x4B
#define STMPE811_REG_FIFO_SIZE      0x4C
#define STMPE811_REG_TSC_DATA_X     0x4D // 16-bit
#define STMPE811_REG_TSC_DATA_Y     0x4F // 16-bit
#define STMPE811_REG_TSC_DATA_Z     0x51
#define STMPE811_REG_TSC_DATA_XYZ   0x52 // 32-bit
#define STMPE811_REG_TSC_FRACT_XYZ  0x56
#define STMPE811_REG_TSC_DATA       0x57
#define STMPE811_REG_TSC_DATA_AI    0xD7
#define STMPE811_REG_TSC_I_DRIVE    0x58
#define STMPE811_REG_TSC_SHIELD     0x59

// Temperature sensor registers
#define STMPE811_REG_TEMP_CTRL      0x60
#define STMPE811_REG_TEMP_DATA      0x61
#define STMPE811_REG_TEMP_TH        0x62

#endif /* _STMPE811_H */
