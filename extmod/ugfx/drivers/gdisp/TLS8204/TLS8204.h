/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _TLS8204_H
#define _TLS8204_H

// H1H0=xx	=========================================

#define TLS8204_NOP    				0x00		// H1H0=xx: NOP					-  0  0  0  0  0  0  0  0
#define TLS8204_SET_FUNC    		0x20		// H1H0=xx: Function set		-  0  0  1 MX MY PD H1 H0
	#define TLS8204_H0_BIT				0x01
	#define TLS8204_H1_BIT				0x02
	#define TLS8204_PD_BIT				0x04
	#define TLS8204_MX_BIT				0x08
	#define TLS8204_MY_BIT				0x10

// H1H0=00	=========================================

#define TLS8204_SET_VLCD7			0x04		// H1H0=00: Set Contrast bit7 	-  0  0  0  0  0  1  0 V7

#define TLS8204_SET_DISPLAY_OFF		0x08		// H1H0=00: Display Control		-  0  0  0  0  1  D  0  E	(D = 0, E = 0)
#define TLS8204_SET_DISPLAY_FILL 	0x09		// H1H0=00: Display Control		-  0  0  0  0  1  D  0  E	(D = 0, E = 1)
#define TLS8204_SET_DISPLAY_NORMAL	0x0C		// H1H0=00: Display Control		-  0  0  0  0  1  D  0  E	(D = 1, E = 0)
#define TLS8204_SET_DISPLAY_INVERT	0x0D		// H1H0=00: Display Control		-  0  0  0  0  1  D  0  E	(D = 1, E = 1)

#define TLS8204_SET_Y				0x40		// H1H0=00: Set Y/8 (page)		-  0  1  0  0 Y3 Y2 Y1 Y0
#define TLS8204_SET_X				0x80		// H1H0=00: Set X (column)		-  1 X6 X5 X4 X3 X2 X1 X0

// H1H0=01	=========================================

#define TLS8204_SET_STARTLINE_S6	0x04		// H1H0=01: Set Startline S6	-  0  0  0  0  0  1  0 S6
#define TLS8204_SET_STARTLINE_S0_5	0x40		// H1H0=01: Set Startline S0-5	-  0  1 S5 S4 S3 S2 S1 S0

#define TLS8204_SET_Y_LSB_FIRST		0x08		// H1H0=01: Set Y to LSB first	-  0  0  0  0  1 D0  x  x	(D0 = 0)
#define TLS8204_SET_Y_MSB_FIRST		0x0C		// H1H0=01: Set Y to LSB first	-  0  0  0  0  1 D0  x  x	(D0 = 1)

#define TLS8204_SET_BIAS_7			0x10		// H1H0=01: Set Bias 1/100		-  0  0  0  1  0 B2 B1 B0
#define TLS8204_SET_BIAS_6			0x11		// H1H0=01: Set Bias 1/81		-  0  0  0  1  0 B2 B1 B0
#define TLS8204_SET_BIAS_5			0x12		// H1H0=01: Set Bias 1/65,68	-  0  0  0  1  0 B2 B1 B0
#define TLS8204_SET_BIAS_4			0x13		// H1H0=01: Set Bias 1/49		-  0  0  0  1  0 B2 B1 B0
#define TLS8204_SET_BIAS_3			0x14		// H1H0=01: Set Bias 1/40,36	-  0  0  0  1  0 B2 B1 B0
#define TLS8204_SET_BIAS_2			0x15		// H1H0=01: Set Bias 1/24		-  0  0  0  1  0 B2 B1 B0
#define TLS8204_SET_BIAS_1			0x16		// H1H0=01: Set Bias 1/18,16	-  0  0  0  1  0 B2 B1 B0
#define TLS8204_SET_BIAS_0			0x17		// H1H0=01: Set Bias 1/10,9,8	-  0  0  0  1  0 B2 B1 B0

#define TLS8204_SET_VLCD0_6			0x80		// H1H0=01: Set Contrast Bit0-6	-  1 V6 V5 V4 V3 V2 V1 V0 (Real range 36 - 219)

// H1H0=10	=========================================

#define TLS8204_SET_PARTIAL_OFF		0x04		// H1H0=10: Partial Mode Off	-  0  0  0  0  0  1  0 PS (PS = 0)
#define TLS8204_SET_PARTIAL_ON		0x05		// H1H0=10: Partial Mode On		-  0  0  0  0  0  1  0 PS (PS = 1)

#define TLS8204_SET_PARTIAL16		0x08		// H1H0=10: 16 line Partial		-  0  0  0  0  1  0  0 WS (WS = 0)
#define TLS8204_SET_PARTIAL32		0x09		// H1H0=10: 32 line Partial		-  0  0  0  0  1  0  0 WS (WS = 1)

#define TLS8204_SET_PARTIAL_Y		0x10		// H1H0=10: The Partial Y/8 (pg)-  0  0  0  1  0 P2 P1 P0

// H1H0=11	=========================================

#define TLS8204_RESET				0x03		// H1H0=11: Reset				-  0  0  0  0  0  0  1  1

#define TLS8204_SET_FREQ55			0x08		// H1H0=11: Frame Frequency		-  0  0  0  0  1 F2 F1 F0	(F = 0)
#define TLS8204_SET_FREQ65			0x09		// H1H0=11: Frame Frequency		-  0  0  0  0  1 F2 F1 F0	(F = 1)
#define TLS8204_SET_FREQ68			0x0A		// H1H0=11: Frame Frequency		-  0  0  0  0  1 F2 F1 F0	(F = 2)
#define TLS8204_SET_FREQ70			0x0B		// H1H0=11: Frame Frequency		-  0  0  0  0  1 F2 F1 F0	(F = 3)
#define TLS8204_SET_FREQ73			0x0C		// H1H0=11: Frame Frequency		-  0  0  0  0  1 F2 F1 F0	(F = 4)
#define TLS8204_SET_FREQ76			0x0D		// H1H0=11: Frame Frequency		-  0  0  0  0  1 F2 F1 F0	(F = 5)
#define TLS8204_SET_FREQ80			0x0E		// H1H0=11: Frame Frequency		-  0  0  0  0  1 F2 F1 F0	(F = 6)
#define TLS8204_SET_FREQ137			0x0F		// H1H0=11: Frame Frequency		-  0  0  0  0  1 F2 F1 F0	(F = 7)

#define TLS8204_SET_INVERSION		0x40		// H1H0=11: Set line inversion	-  0  1  0 L4 L3 L2 L1 L0	(Lines = 0 or L+2)

#define TLS8204_SET_BOOST			0x50		// H1H0=11: Set Boost			-  1  0  0  1 E1 E0 B1 B0
	#define TLS8204_BOOST_EFFICIENCY1	0x5C	// H1H0=11: Set Boost			-  1  0  0  1 E1 E0 B1 B0	(E1E0 = 3)
	#define TLS8204_BOOST_EFFICIENCY2	0x58	// H1H0=11: Set Boost			-  1  0  0  1 E1 E0 B1 B0	(E1E0 = 2)
	#define TLS8204_BOOST_EFFICIENCY3	0x54	// H1H0=11: Set Boost			-  1  0  0  1 E1 E0 B1 B0	(E1E0 = 1)
	#define TLS8204_BOOST_EFFICIENCY4	0x50	// H1H0=11: Set Boost			-  1  0  0  1 E1 E0 B1 B0	(E1E0 = 0)
	#define TLS8204_BOOST_5X			0x53	// H1H0=11: Set Boost			-  1  0  0  1 E1 E0 B1 B0	(B1B0 = 3)
	#define TLS8204_BOOST_4X			0x52	// H1H0=11: Set Boost			-  1  0  0  1 E1 E0 B1 B0	(B1B0 = 2)
	#define TLS8204_BOOST_3X			0x51	// H1H0=11: Set Boost			-  1  0  0  1 E1 E0 B1 B0	(B1B0 = 1)
	#define TLS8204_BOOST_2X			0x50	// H1H0=11: Set Boost			-  1  0  0  1 E1 E0 B1 B0	(B1B0 = 0)

#endif /* _TLS8204_H */
