#ifndef SSD1331_H
#define SSD1331_H

#define SSD1331_COLUMN_ADDRESS					0x15		// 2 data: startx endx
#define SSD1331_ROW_ADDRESS         	        0x75		// 2 data: starty endy
#define SSD1331_CONTRAST_A						0x81		// 1 data: 0 - 255 default=128
#define SSD1331_CONTRAST_B						0x82		// 1 data: 0 - 255 default=128
#define SSD1331_CONTRAST_C						0x83		// 1 data: 0 - 255 default=128
#define SSD1331_BRIGHTNESS						0x87		// 1 data: 0 - 15  default=15
#define SSD1331_PRECHARGE_A						0x8A		// 1 data: 0 - 255 default=128
#define SSD1331_PRECHARGE_B						0x8B		// 1 data: 0 - 255 default=128
#define SSD1331_PRECHARGE_C						0x8C		// 1 data: 0 - 255 default=128
#define SSD1331_MODE							0xA0		// 1 data:-        default=0x40
	#define SSD1331_MODE_VINC						0x01
	#define SSD1331_MODE_COLUMN_REVERSE				0x02
	#define SSD1331_MODE_BGR						0x04		// if 1 then A=Blue,B=Green,C=Red
	#define SSD1331_MODE_COM_SWAPLR					0x08
	#define SSD1331_MODE_COM_REVERSE				0x10
	#define SSD1331_MODE_COM_SPLIT					0x20
	#define SSD1331_MODE_8_BIT						0x00
	#define SSD1331_MODE_16_BIT						0x40
	#define SSD1331_MODE_16_BIT2					0x80
#define SSD1331_START_LINE						0xA1		// 1 data: liney default=0
#define SSD1331_COM_OFFSET						0xA2		// 1 data: liney default=0
#define SSD1331_PIXELS_NORMAL					0xA4		// 0 data - default
#define SSD1331_PIXELS_FILL						0xA5		// 0 data
#define SSD1331_PIXELS_OFF						0xA6		// 0 data
#define SSD1331_DISPLAY_INVERSE					0xA7		// 0 data
#define SSD1331_MULTIPLEX						0xA8		// 1 data: 15-63 default=63
#define SSD1331_DIM								0xAB		// 5 data:
															//		0x00 - Reserved
															//		ContrastA 0 - 255 default=0?
															//		ContrastB 0 - 255 default=0?
															//		ContrastC 0 - 255 default=0?
															//		Precharge 0 - 31 default=0?
#define SSD1331_RESET							0xAD		// 1 data:
	#define SSD1331_RESET_ON						0x8F	//		Reset - default
	#define SSD1331_RESET_OFF						0x8E	//		Normal
#define SSD1331_DISPLAY_OFF						0xAE		// 0 data - default
#define SSD1331_DISPLAY_DIM						0xAC		// 0 data
#define SSD1331_DISPLAY_ON						0xAF		// 0 data
#define SSD1331_POWER							0xB0		// 1 data:
	#define SSD1331_POWER_ON						0x0B	//		Normal Operation
	#define SSD1331_POWER_OFF						0x1A	//		Power Save - default
#define SSD1331_PHASE_PERIOD					0xB1		// 1 data: Phase 1 bit0-3 1-15, Phase 2 bit4-7 1-15 default=0x74
#define SSD1331_CLOCKS							0xB3		// 1 data: Divide bit0-3 + 1, Freq bits4-7 default=0xD0
#define SSD1331_GRAYSCALE						0xB8		// 32 data: each 1-125 default=1,5,9...121,125
#define SSD1331_GRAYSCALE_LINEAR				0xB9		// 0 data
#define SSD1331_PRECHARGE_VOLTAGE				0xBB		// 1 data: 0 - 62 (even only) default=62
#define SSD1331_NOP								0xBC		// 0 data
#define SSD1331_NOP1							0xBD		// 0 data
#define SSD1331_NOP2							0xE3		// 0 data
#define SSD1331_DESELECT_VOLTAGE				0xBE		// 1 data: 0 - 62 (even only) default=62
#define SSD1331_LOCK							0xFD		// 1 data:
	#define SSD1331_LOCK_ON							0x16	//		Lock MCU
	#define SSD1331_LOCK_OFF						0x12	//		Unlock MCU - default

#define SSD1331_DRAW_LINE						0x21		// 7 data:
															//		startx, starty,
															//		endx, endy,
															//		C, B, A			- 6 bits each
#define SSD1331_DRAW_RECT						0x22		// 10 data:
															//		startx, starty,
															//		endx, endy,
															//		C, B, A			- line color 6 bits each
															//		C, B, A			- fill color 6 bits each
#define SSD1331_DRAW_COPY						0x23		// 6 data:
															//		startx, starty,
															//		endx, endy,
															//		newx, newy
#define SSD1331_DIM_AREA						0x24		// 4 data:
															//		startx, starty,
															//		endx, endy
#define SSD1331_DRAW_CLEAR						0x25		// 4 data:
															//		startx, starty,
															//		endx, endy
#define SSD1331_DRAW_MODE						0x26		// 1 data:
	#define SSD1331_DRAW_FILLRECT					0x01	//		Fill Rectangle Draws
	#define SSD1331_COPY_REVERSE					0x10	//		Reverse colors when copying
#define SSD1331_SCROLL							0x27		// 5 data:
															//		0 - 95	columns horizontal scroll
															//		starty
															//		cy
															//		0 - 63 rows vertical scroll
															//		time 0 = 6 frames, 1 = 10 frames, 2 = 100 frames, 3 = 200 frames
#define SSD1331_SCROLL_STOP						0x2E		// 0 data
#define SSD1331_SCROLL_START					0x2F		// 0 data

#endif // SSD1331_H
