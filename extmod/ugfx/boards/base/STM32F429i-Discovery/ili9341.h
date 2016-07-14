/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef ILI9341_H
#define ILI9341_H

// ILI9341 commands
#define ILI9341_CMD_NOP                 0x00    /**< No operation.*/
#define ILI9341_CMD_RESET               0x01    /**< Software reset.*/
#define ILI9341_GET_ID_INFO             0x04    /**< Get ID information.*/
#define ILI9341_GET_STATUS              0x09    /**< Get status.*/
#define ILI9341_GET_PWR_MODE            0x0A    /**< Get power mode.*/
#define ILI9341_GET_MADCTL              0x0B    /**< Get MADCTL.*/
#define ILI9341_GET_PIX_FMT             0x0C    /**< Get pixel format.*/
#define ILI9341_GET_IMG_FMT             0x0D    /**< Get image format.*/
#define ILI9341_GET_SIG_MODE            0x0E    /**< Get signal mode.*/
#define ILI9341_GET_SELF_DIAG           0x0F    /**< Get self-diagnostics.*/
#define ILI9341_CMD_SLEEP_ON            0x10    /**< Enter sleep mode.*/
#define ILI9341_CMD_SLEEP_OFF           0x11    /**< Exist sleep mode.*/
#define ILI9341_CMD_PARTIAL_ON          0x12    /**< Enter partial mode.*/
#define ILI9341_CMD_PARTIAL_OFF         0x13    /**< Exit partial mode.*/
#define ILI9341_CMD_INVERT_ON           0x20    /**< Enter inverted mode.*/
#define ILI9341_CMD_INVERT_OFF          0x21    /**< Exit inverted mode.*/
#define ILI9341_SET_GAMMA               0x26    /**< Set gamma params.*/
#define ILI9341_CMD_DISPLAY_OFF         0x28    /**< Disable display.*/
#define ILI9341_CMD_DISPLAY_ON          0x29    /**< Enable display.*/
#define ILI9341_SET_COL_ADDR            0x2A    /**< Set column address.*/
#define ILI9341_SET_PAGE_ADDR           0x2B    /**< Set page address.*/
#define ILI9341_SET_MEM                 0x2C    /**< Set memory.*/
#define ILI9341_SET_COLOR               0x2D    /**< Set color.*/
#define ILI9341_GET_MEM                 0x2E    /**< Get memory.*/
#define ILI9341_SET_PARTIAL_AREA        0x30    /**< Set partial area.*/
#define ILI9341_SET_VSCROLL             0x33    /**< Set vertical scroll def.*/
#define ILI9341_CMD_TEARING_ON          0x34    /**< Tearing line enabled.*/
#define ILI9341_CMD_TEARING_OFF         0x35    /**< Tearing line disabled.*/
#define ILI9341_SET_MEM_ACS_CTL         0x36    /**< Set mem access ctl.*/
#define ILI9341_SET_VSCROLL_ADDR        0x37    /**< Set vscroll start addr.*/
#define ILI9341_CMD_IDLE_OFF            0x38    /**< Exit idle mode.*/
#define ILI9341_CMD_IDLE_ON             0x39    /**< Enter idle mode.*/
#define ILI9341_SET_PIX_FMT             0x3A    /**< Set pixel format.*/
#define ILI9341_SET_MEM_CONT            0x3C    /**< Set memory continue.*/
#define ILI9341_GET_MEM_CONT            0x3E    /**< Get memory continue.*/
#define ILI9341_SET_TEAR_SCANLINE       0x44    /**< Set tearing scanline.*/
#define ILI9341_GET_TEAR_SCANLINE       0x45    /**< Get tearing scanline.*/
#define ILI9341_SET_BRIGHTNESS          0x51    /**< Set brightness.*/
#define ILI9341_GET_BRIGHTNESS          0x52    /**< Get brightness.*/
#define ILI9341_SET_DISPLAY_CTL         0x53    /**< Set display ctl.*/
#define ILI9341_GET_DISPLAY_CTL         0x54    /**< Get display ctl.*/
#define ILI9341_SET_CABC                0x55    /**< Set CABC.*/
#define ILI9341_GET_CABC                0x56    /**< Get CABC.*/
#define ILI9341_SET_CABC_MIN            0x5E    /**< Set CABC min.*/
#define ILI9341_GET_CABC_MIN            0x5F    /**< Set CABC max.*/
#define ILI9341_GET_ID1                 0xDA    /**< Get ID1.*/
#define ILI9341_GET_ID2                 0xDB    /**< Get ID2.*/
#define ILI9341_GET_ID3                 0xDC    /**< Get ID3.*/

// ILI9341 extended commands
#define ILI9341_SET_RGB_IF_SIG_CTL      0xB0    /**< RGB IF signal ctl.*/
#define ILI9341_SET_FRAME_CTL_NORMAL    0xB1    /**< Set frame ctl (normal).*/
#define ILI9341_SET_FRAME_CTL_IDLE      0xB2    /**< Set frame ctl (idle).*/
#define ILI9341_SET_FRAME_CTL_PARTIAL   0xB3    /**< Set frame ctl (partial).*/
#define ILI9341_SET_INVERSION_CTL       0xB4    /**< Set inversion ctl.*/
#define ILI9341_SET_BLANKING_PORCH_CTL  0xB5    /**< Set blanking porch ctl.*/
#define ILI9341_SET_FUNCTION_CTL        0xB6    /**< Set function ctl.*/
#define ILI9341_SET_ENTRY_MODE          0xB7    /**< Set entry mode.*/
#define ILI9341_SET_LIGHT_CTL_1         0xB8    /**< Set backlight ctl 1.*/
#define ILI9341_SET_LIGHT_CTL_2         0xB9    /**< Set backlight ctl 2.*/
#define ILI9341_SET_LIGHT_CTL_3         0xBA    /**< Set backlight ctl 3.*/
#define ILI9341_SET_LIGHT_CTL_4         0xBB    /**< Set backlight ctl 4.*/
#define ILI9341_SET_LIGHT_CTL_5         0xBC    /**< Set backlight ctl 5.*/
#define ILI9341_SET_LIGHT_CTL_7         0xBE    /**< Set backlight ctl 7.*/
#define ILI9341_SET_LIGHT_CTL_8         0xBF    /**< Set backlight ctl 8.*/
#define ILI9341_SET_POWER_CTL_1         0xC0    /**< Set power ctl 1.*/
#define ILI9341_SET_POWER_CTL_2         0xC1    /**< Set power ctl 2.*/
#define ILI9341_SET_VCOM_CTL_1          0xC5    /**< Set VCOM ctl 1.*/
#define ILI9341_SET_VCOM_CTL_2          0xC6    /**< Set VCOM ctl 2.*/
#define ILI9341_SET_NVMEM               0xD0    /**< Set NVMEM data.*/
#define ILI9341_GET_NVMEM_KEY           0xD1    /**< Get NVMEM protect key.*/
#define ILI9341_GET_NVMEM_STATUS        0xD2    /**< Get NVMEM status.*/
#define ILI9341_GET_ID4                 0xD3    /**< Get ID4.*/
#define ILI9341_SET_PGAMMA              0xE0    /**< Set positive gamma.*/
#define ILI9341_SET_NGAMMA              0xE1    /**< Set negative gamma.*/
#define ILI9341_SET_DGAMMA_CTL_1        0xE2    /**< Set digital gamma ctl 1.*/
#define ILI9341_SET_DGAMMA_CTL_2        0xE3    /**< Set digital gamma ctl 2.*/
#define ILI9341_SET_IF_CTL              0xF6    /**< Set interface control.*/

// ILI9341 interface modes
#define ILI9341_IM_3LSI_1               0x5     /**< 3-line serial, mode 1.*/
#define ILI9341_IM_3LSI_2               0xD     /**< 3-line serial, mode 2.*/
#define ILI9341_IM_4LSI_1               0x6     /**< 4-line serial, mode 1.*/
#define ILI9341_IM_4LSI_2               0xE     /**< 4-line serial, mode 2.*/

#endif /* ILI9341_H */
