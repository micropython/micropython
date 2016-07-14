/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _PCF8812_H
#define _PCF8812_H

#define PCF8812_SET_FUNC				0x20  // Function set
#define PCF8812_H 					0x01
#define PCF8812_V 					0x02
#define PCF8812_PD					0x04

#define PCF8812_SET_DISPLAY				0x08
#define PCF8812_DISPLAY_MODE_BLANK			0x00  // bit D = 0, E = 0
#define PCF8812_DISPLAY_MODE_FILL			0x01  // bit D = 0, E = 1
#define PCF8812_DISPLAY_MODE_NORMAL			0x04  // bit D = 1, E = 0
#define PCF8812_DISPLAY_MODE_INVERT			0x05  // bit D = 1, E = 1

#define PCF8812_SET_PRS					0x10  // Set Vop range, VLCD programming range select

#define PCF8812_SET_Y					0x40  // 0 0  1  0  0 Y3 Y2 Y1 Y0
#define PCF8812_SET_X					0x80  // 0 1 X6 X5 X4 X3 X2 X1 X0

// =========================================

#define PCF8812_SET_TEMP				0x04  // set temperature coefﬁcient (TCx)
#define PCF8812_TEMP_MODE_0				0x00  // TC1 = 0, TC0 = 0
#define PCF8812_TEMP_MODE_1				0x01  // TC1 = 0, TC0 = 1
#define PCF8812_TEMP_MODE_2				0x02  // TC1 = 1, TC0 = 0
#define PCF8812_TEMP_MODE_3				0x03  // TC1 = 1, TC0 = 1

#define PCF8812_SET_VMULT				0x08  // Set voltage multiplier factor
#define PCF8812_VMULT_MODE_0				0x00  // S1 = 0, S0 = 0 - 2 × voltage multiplier
#define PCF8812_VMULT_MODE_1				0x01  // S1 = 0, S0 = 1 - 3 × voltage multiplier
#define PCF8812_VMULT_MODE_2				0x02  // S1 = 1, S0 = 0 - 4 × voltage multiplier
#define PCF8812_VMULT_MODE_3				0x03  // S1 = 1, S0 = 1 - 5 × voltage multiplier
 
#define PCF8812_SET_BIAS				0x10  // set bias system (BSx)
#define PCF8812_BIAS_MODE_7				0x00  // 1 to 100
#define PCF8812_BIAS_MODE_6				0x01  // 1 to 80
#define PCF8812_BIAS_MODE_5				0x02  // 1 to 65
#define PCF8812_BIAS_MODE_4				0x03  // 1 to 48
#define PCF8812_BIAS_MODE_3				0x04  // 1 to 40 or 1 to 34
#define PCF8812_BIAS_MODE_2				0x05  // 1 to 24
#define PCF8812_BIAS_MODE_1				0x06  // 1 to 18 or 1 to 16
#define PCF8812_BIAS_MODE_0				0x07  // 1 to 10 or 1 to 9 or 1 to 8

#define PCF8812_SET_VOP					0x80  // write VOP to register, 1 VOP6 VOP5 VOP4 VOP3 VOP2 VOP1 VOP0

#endif /* _PCF8812_H */
