/*
 * This file is subject to the terms of the GPL License.
 */

#ifndef _SSD1848_H
#define _SSD1848_H

/*
 * Set the start column address by X5X4X3X2X1X0
 * Set the end column address by Y5Y4Y3Y2Y1Y0
 * Column address = 00000000b (POR)
 * Column address is in a range of 0~32 (0x00~0x20).
 */
#define SSD1848_HV_COLUMN_ADDRESS         0x15

/*
 * Set the start page address by X7X6X5X4X3X2X1X0
 * Set the end page address by Y7Y6Y5Y4Y3Y2Y1Y0
 * Page address = 00000000b (POR)
 * Page address is in a range of 0~129 (0x00~0x81).
 */
#define SSD1848_HV_PAGE_ADDRESS           0x75

/*
 * X2 X1 X0 ROW0…ROW64 ROW65…ROW129
 * 0 0 0 COM0 ->COM64 COM65-> COM129(POR)
 * 0 0 1 COM0->COM64 COM129<-COM65
 * 0 1 0 COM64<-COM0 COM65->COM129
 * 0 1 1 COM64<-COM0 COM129<-COM65
 */
#define SSD1848_COM_OUTPUT_SCAN_DIR       0xBB

/*
 * a) Normal or Reverse page/column/RAM access/scan
 * directions
 * P10 = 0: set page address to normal display (POR)
 * P10 = 1: set page address to inverse display
 * P11 = 0: set column address to normal rotation (POR)
 * P11 = 1: set column address to inverse rotation
 * P12 = 0: set scan direction to column scan(POR)
 * P12 = 1: set scan direction to page scan
 * P13 = 0: set normal scan direction (POR)
 * P13 = 1: set inverse scan direction
 *
 * b) Gray-scale setting
 * X = Light gray PWM count (POR 5 counts)
 * Y = Dark gray PWM count (POR 10 counts)
 * P22P21P20 = X -1 (POR 100)
 * P25P24P23 = Y - X - 1 (POR 100)
 * Remark: Y-X≤8
 *
 * # Remarks: The PWM count for White and Black are 0 and 15 respectively.
 *
 * P30 = 0: PWM (POR)
 * P34 = 0:
 * White | Light Gray | Dark Gray | Black
 *    0% |        33% |       66% |  100%
 *
 * P34 = 1:
 * White | Light Gray | Dark Gray | Black
 *    0% |       X/15 |      Y/15 |  100%
 *
 * P30 = 1: FRC
 * P31 = 0: 3-frame FRC (POR)
 * White | Light Gray | Dark Gray | Black
 *    0% |        33% |       66% |  100%
 *
 * P31 = 1: 4-frame FRC
 * P33 P32 | White | Light Gray | Dark Gray | Black
 * 00(POR) |    0% |        25% |       75% |  100%
 *      01 |    0% |        50% |       75% |  100%
 *      10 |    0% |        25% |       50% |  100%
 *      11 |           Reserved
 */
#define SSD1848_DATA_OUTPUT_SCAN_DIR      0xBC

/* @
 * Driver duty selection
 * Select driver duty from 1/16 to 1/128. As Y5Y4Y3Y2Y1Y0 is increased
 * from 000011b to 011111b, the number of display lines, N is increased
 * at the same rating. To specify the Y5Y4Y3Y2Y1Y0 = (N/4)-1 where 1/N
 * is the driver duty.
 *
 * Y5Y4Y3Y2Y1Y0 = 100000b for 1/130 duty.
 */
#define SSD1848_SETDISP_CTRL              0xCA

/* @
 * X0=0: turns off the reference voltage generator (POR)
 * X0=1: turns on the reference voltage generator
 * X1=0: turns off the internal regulator and voltage follower (POR)
 * X1=1: turns on the internal regulator and voltage follower
 *
 * Select booster level
 * X4 X3 X2    Boost level
 *  0  0  0    4X
 *  0  0  1    5X
 *  0  1  0    6X (POR)
 *  0  1  1    7X
 */
#define SSD1848_SETPOWER_CTRL             0x20

/*
 * a) Select contrast level from 64 contrast steps Contrast increases
 * as X5X4X3X2X1X0 is increased from 000000b to 111111b.
 * X5X4X3X2X1X0 = 100000b (POR)
 *
 * b) The internal regulator gain (1+R2/R1) VOUT increases as Y2Y1Y0
 * is increased from 000b to 111b.
 *
 * The factor, 1+R2/R1, is given by:
 * Y2Y1Y0 = 000: 3.38 (POR)
 * Y2Y1Y0 = 001: 4.41
 * Y2Y1Y0 = 010: 5.44
 * Y2Y1Y0 = 011: 6.47
 * Y2Y1Y0 = 100: 7.50
 * Y2Y1Y0 = 101: 8.52
 * Y2Y1Y0 = 110: 9.55
 * Y2Y1Y0 = 111: 10.58
 */
#define SSD1848_SETCONTRAST               0x81

/*
 * X7X6X5X4X3X2X1X0 : End COM Address = 00000000b (POR)
 */
#define SSD1848_ENABLE_PARTIAL_DISP       0xA8
#define SSD1848_EXIT_PARTIAL_DISP         0xA9

#define SSD1848_DISPLAYOFF                0xAE
#define SSD1848_DISPLAYON                 0xAF

#define SSD1848_EXITSLEEP                 0x94
#define SSD1848_ENTERSLEEP                0x95

#define SSD1848_ENABLE_INTERNAL_CLOCK     0xD1
#define SSD1848_DISABLE_INTERNAL_CLOCK    0xD2



/*
 * VOUT average temperature gradients
 * X1 X0    Average Temperature
 *          Gradient [%/oC]
 *  0  0    -0.01 (POR)
 *  0  1    -0.06
 */
#define SSD1848_TEMP_COMPENSATION          0x82

/*
 * Enter the "write display data mode" by executing
 * the command 01011100b. The following byte is
 * used to specify the data byte to be written to the
 * GDDRAM directly.
 * The D/C bit should be stated at logic "1" during the
 * display data is written to the GDDRAM.
 */
#define SSD1848_WRITE_DISP_DATA            0x5C

/*
 * Allow user to set bias from 1/ 4 to 1/13
 * B3B2B1B0    Bias ratio
 * 1 0 0 1     1/4 bias
 * 1 0 0 0     1/5 bias
 * 0 1 1 1     1/6 bias
 * 0 1 1 0     1/7 bias
 * 0 1 0 1     1/8 bias
 * 0 1 0 0     1/9 bias
 * 0 0 1 1     1/10 bias
 * 0 0 1 0     1/11 bias
 * 0 0 0 1     1/12 bias
 * 0 0 0 0     1/13 bias (POR)
 * L0          Lock and unlock Cmd
 * 0 unlock (POR)
 * 1 lock and no more cmd/data is written to driver
 * The 2nd byte is sent as Cmd if L0 is set to 1
 */
#define SSD1848_SET_BIASING_LOCK           0xFB

/*
 * This command uses to change the frame
 * frequency; set the N-line inversion and N-line
 * inversion mode
 * X0 = 1 (POR)            X0 = 0
 * F4F3F2F1F0
 * 00000 : 56.4 Hz (POR)   64Hz
 * 00111 : +10.1%          +11.8%
 * 01000 : +10.7%          +15.2%
 * 01001 : +12.5%          +15.2%
 * 01010 : +14.1%          +20.6%
 * 01011 : +16.1%          +20.6%
 * 01100 : +17.4%          +25.9%
 * 01101 : +19.5%          +25.9%
 * 01110 : +21.4%          +32.9%
 * 01111 : +23.7%          +32.9%
 * 10000 : +24.6%          +37.4%
 * 10001 : +27.1%          +37.4%
 * 10010 : +29.2%          +46.0%
 * 10011 : +31.8%          +46.0%
 * 10100 : +33.6%          +54.6%
 * 10101 : +36.5%          +54.6%
 * 10110 : +39.0%          +66.9%
 * 10111 : +42.2%          +66.9%
 * 11000 : +43.2%          +75.8%
 * 11001 : +46.6%          +75.8%
 * 11010 : +49.7%          +94.0%
 *
 * Remark: The frame frequency is typical value
 * for 130mux and PWM mode.
 * The second byte data N5N4N3N2N1N0 sets the nline
 * inversion register from 2 to 64 lines to
 * reduce display crosstalk. Register values from
 * 000001b to 111111b are mapped to 2 lines to 64
 * lines respectively. Value 00000b disables the Nline
 * inversion. 010000 is the POR value. To
 * avoid a fix polarity at some lines, it should be
 * noted that the total number of mux should NOT
 * be a multiple of the lines of inversion (n).
 * N6
 * 0 – reset n-line counter per frame (POR)
 * 1 – will not reset n-line counter per frame
 */
#define SSD1848_SET_FRAME_FREQ_LINEINVE    0xF2


#define SSD1848_DUAL_OPT_SET               0xF6
#define SSD1848_SET_BLACK_WHITE            0xF7
#define SSD1848_OTP                        0xF8


#define SSD1848_NORMALDISPLAY              0xA6
#define SSD1848_INVERTDISPLAY              0xA7

/*
 * This command specifies 1st Com line function. Byte A specifies the first
 * display line which the graphic start to display. At POR, the 1st Com line
 * is set to 00000000b (0 lines).
 */
#define SSD1848_SETSTARTLINE               0x44




/* Scrolling #defines */
/*
 * a) Top Block Address
 * X7X6X5X4X3X2X1X0 is used to specify the row
 * address at the top of the scrolling area.
 * Top row address = 00000000b (POR)
 *
 * b) Bottom Block Address
 * Y7Y6Y5Y4Y3Y2Y1Y0 is used to specify the row
 * address at the bottom of the scrolling area.
 * Bottom row address = 00000000b (POR)
 *
 * c) Number of specified Blocks
 * The number of specified blocks = Number of (Top
 * fixed area + Scroll area) blocks –1. If bottom scroll
 * or whole screen scroll mode is chosen, the number
 * of specified blocks is set to Z7Z6Z5Z4Z3Z2Z1Z0
 * Number of specified blocks = 00000000b (POR)
 *
 * d) Area Scroll Mode
 * There are four types of area scroll.
 * P41 P40   Types of Area Scroll
 *   0   0   Center Screen Scroll
 *   0   1   Top Screen Scroll
 *   1   0   Bottom Screen Scroll
 *   1   1   Whole Screen Scroll
 * Type of area scroll = Whole Screen Scroll (POR)
 */
#define SSD1848_SET_AREA_SCROLL    0xAA

/*
 * X5X4X3X2X1X0 specify the start row address
 * of area scrolling.
 * Start block address = 00000000b (POR)
 */
#define SSD1848_SCROLL_START       0xAB

/*
 * Enter the "read display data mode" by executing
 * the command 01011101b. The next byte is a
 * dummy data. The GDDRAM data will be read
 * form the second byte. The GDDRAM column
 * address pointer will be increased by one
 * automatically after each 2-bytes data read.
 */
#define SSD1848_READ_DISP_DATA     0x5D

#endif /* _SSD1848_H */
