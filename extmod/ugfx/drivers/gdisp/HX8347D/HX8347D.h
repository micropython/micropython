/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _HX8347D_H
#define _HX8347D_H

/* HX8347D registers */

/* page 0 registers */
#define HX8347D_REG_HID                 0x00    /* Himax ID */
#define HX8347D_REG_DMODE               0x01    /* Display mode control */
#define HX8347D_REG_SCH                 0x02    /* Column address start high */
#define HX8347D_REG_SCL                 0x03    /* Column address start low */
#define HX8347D_REG_ECH                 0x04    /* Column address end high */
#define HX8347D_REG_ECL                 0x05    /* Column address end low */
#define HX8347D_REG_SPH                 0x06    /* Row address start high */
#define HX8347D_REG_SPL                 0x07    /* Row address start low */
#define HX8347D_REG_EPH                 0x08    /* Row address end high */
#define HX8347D_REG_EPL                 0x09    /* Row address end low */
#define HX8347D_REG_PSLH                0x0a    /* Partial area start row high */
#define HX8347D_REG_PSLL                0x0b    /* Partial area start row low */
#define HX8347D_REG_PELH                0x0c    /* Partial area end row high */
#define HX8347D_REG_PELL                0x0d    /* Partial area end row low */
#define HX8347D_REG_TFAH                0x0e    /* Vertical scroll top fixed area high */
#define HX8347D_REG_TFAL                0x0f    /* Vertical scroll top fixed area low */

#define HX8347D_REG_VSAH                0x10    /* Vertical scroll height area high */
#define HX8347D_REG_VSAL                0x11    /* Vertical scroll height area low */
#define HX8347D_REG_BFAH                0x12    /* Vertical scroll button area high */
#define HX8347D_REG_BFAL                0x13    /* Vertical scroll button area low */
#define HX8347D_REG_VSPH                0x14    /* Vertical scroll start address high */
#define HX8347D_REG_VSPL                0x15    /* Vertical scroll start address low */
#define HX8347D_REG_MAC                 0x16    /* Memory access control */
#define HX8347D_REG_COLMOD              0x17    /* COLMOD */
#define HX8347D_REG_OSCCH               0x18    /* OSC control 2 */
#define HX8347D_REG_OSCCL               0x19    /* OSC control 1 */
#define HX8347D_REG_PWC1                0x1a    /* Power control 1 */
#define HX8347D_REG_PWC2                0x1b    /* Power control 2 */
#define HX8347D_REG_PWC3                0x1c    /* Power control 3 */
#define HX8347D_REG_PWC4                0x1d    /* Power control 4 */
#define HX8347D_REG_PWC5                0x1e    /* Power control 5 */
#define HX8347D_REG_PWC6                0x1f    /* Power control 6 */

#define HX8347D_REG_SRAMWC              0x22    /* SRAM write control */
#define HX8347D_REG_VMF                 0x23    /* VCOM control 1 */
#define HX8347D_REG_VMH                 0x24    /* VCOM control 2 */
#define HX8347D_REG_VML                 0x25    /* VCOM control 3 */
#define HX8347D_REG_DC1                 0x26    /* Display control 1 */
#define HX8347D_REG_DC2                 0x27    /* Display control 2 */
#define HX8347D_REG_DC3                 0x28    /* Display control 3 */
#define HX8347D_REG_FRC1                0x29    /* Frame rate control 1 */
#define HX8347D_REG_FRC2                0x2a    /* Frame rate control 2 */
#define HX8347D_REG_FRC3                0x2b    /* Frame rate control 3 */
#define HX8347D_REG_FRC4                0x2c    /* Frame rate control 4 */
#define HX8347D_REG_GDON                0x2d    /* Cycle control 1 */
#define HX8347D_REG_GDOF                0x2e    /* Cycle control 2 */
#define HX8347D_REG_DINV                0x2f    /* Display inversion */

#define HX8347D_REG_RGB1                0x31    /* RGB interface control 1 */
#define HX8347D_REG_RGB2                0x32    /* RGB interface control 2 */
#define HX8347D_REG_RGB3                0x33    /* RGB interface control 3 */
#define HX8347D_REG_RGB4                0x34    /* RGB interface control 4 */
#define HX8347D_REG_PCH                 0x36    /* Panel characteristic */
#define HX8347D_REG_OTP1                0x38    /* OTP control 1 */
#define HX8347D_REG_OTP2                0x39    /* OTP control 2 */
#define HX8347D_REG_OTP3                0x3a    /* OTP control 3 */
#define HX8347D_REG_CABC1               0x3c    /* CABC control 1 */
#define HX8347D_REG_CABC2               0x3d    /* CABC control 2 */
#define HX8347D_REG_CABC3               0x3e    /* CABC control 3 */
#define HX8347D_REG_CABC4               0x3f    /* CABC control 4 */

#define HX8347D_REG_VRP0                0x40    /* Gamma control 1 */
#define HX8347D_REG_VRP1                0x41    /* Gamma control 2 */
#define HX8347D_REG_VRP2                0x42    /* Gamma control 3 */
#define HX8347D_REG_VRP3                0x43    /* Gamma control 4 */
#define HX8347D_REG_VRP4                0x44    /* Gamma control 5 */
#define HX8347D_REG_VRP5                0x45    /* Gamma control 6 */
#define HX8347D_REG_PRP0                0x46    /* Gamma control 7 */
#define HX8347D_REG_PRP1                0x47    /* Gamma control 8 */
#define HX8347D_REG_PKP0                0x48    /* Gamma control 9 */
#define HX8347D_REG_PKP1                0x49    /* Gamma control 10 */
#define HX8347D_REG_PKP2                0x4a    /* Gamma control 11 */
#define HX8347D_REG_PKP3                0x4b    /* Gamma control 12 */
#define HX8347D_REG_PKP4                0x4c    /* Gamma control 13 */

#define HX8347D_REG_VRN0                0x50    /* Gamma control 14 */
#define HX8347D_REG_VRN1                0x51    /* Gamma control 15 */
#define HX8347D_REG_VRN2                0x52    /* Gamma control 16 */
#define HX8347D_REG_VRN3                0x53    /* Gamma control 17 */
#define HX8347D_REG_VRN4                0x54    /* Gamma control 18 */
#define HX8347D_REG_VRN5                0x55    /* Gamma control 19 */
#define HX8347D_REG_PRN0                0x56    /* Gamma control 20 */
#define HX8347D_REG_PRN1                0x57    /* Gamma control 21 */
#define HX8347D_REG_PKN0                0x58    /* Gamma control 22 */
#define HX8347D_REG_PKN1                0x59    /* Gamma control 23 */
#define HX8347D_REG_PKN2                0x5a    /* Gamma control 24 */
#define HX8347D_REG_PKN3                0x5b    /* Gamma control 25 */
#define HX8347D_REG_PKN4                0x5c    /* Gamma control 26 */
#define HX8347D_REG_CGM                 0x5d    /* Gamma control 27 */

#define HX8347D_REG_TEC                 0x60    /* TE control */

#define HX8347D_REG_PS1                 0xe4    /* Power saving 1 */
#define HX8347D_REG_PS2                 0xe5    /* Power saving 2 */
#define HX8347D_REG_PS3                 0xe6    /* Power saving 3 */
#define HX8347D_REG_PS4                 0xe7    /* Power saving 4 */
#define HX8347D_REG_OPONN               0xe8    /* Source OP control normal */
#define HX8347D_REG_OPONI               0xe9    /* Source OP control idle */
#define HX8347D_REG_STBAH               0xea    /* Power control internal use 1 */
#define HX8347D_REG_STBAL               0xeb    /* Power control internal use 2 */
#define HX8347D_REG_PTBAH               0xec    /* Source control internal use 1 */
#define HX8347D_REG_PTBAL               0xed    /* Source control internal use 2 */

/* page 1 registers */
#define HX8347D_REG_CABC5               0xc3    /* CABC control 5 */
#define HX8347D_REG_CABC6               0xc5    /* CABC control 6 */
#define HX8347D_REG_CABC7               0xc7    /* CABC control 7 */
#define HX8347D_REG_DBG0                0xcb    /* Gain select register 0 */
#define HX8347D_REG_DBG1                0xcc    /* Gain select register 1 */
#define HX8347D_REG_DBG2                0xcd    /* Gain select register 2 */
#define HX8347D_REG_DBG3                0xce    /* Gain select register 3 */
#define HX8347D_REG_DBG4                0xcf    /* Gain select register 4 */
#define HX8347D_REG_DBG5                0xd0    /* Gain select register 5 */
#define HX8347D_REG_DBG6                0xd1    /* Gain select register 6 */
#define HX8347D_REG_DBG7                0xd2    /* Gain select register 7 */
#define HX8347D_REG_DBG8                0xd3    /* Gain select register 8 */

#define HX8347D_REG_PGSEL               0xff    /* Page select */

#endif /* _HX8347D_H */
