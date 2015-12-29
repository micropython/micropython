/*
    Poncho UI for EDU-CIAA library.
    Copyright (C) 2015  Ernesto gigliotti. ernestogigliotti@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
     
*/

#include "chip.h"
#include "LCD4Bit.h"

//================================================[LCD Management]========================================================
typedef struct
{
    int8_t port;
    int8_t portBit;
    int8_t gpio;
    int8_t gpioBit;
    int8_t func;

}LCDPinInfo;

static LCDPinInfo lcdPinInfo[]={
        {4,4,2,4,SCU_MODE_FUNC0},{4,5,2,5,SCU_MODE_FUNC0},{4,6,2,6,SCU_MODE_FUNC0},{4,10,5,14,SCU_MODE_FUNC4},{4,8,5,12,SCU_MODE_FUNC4},{4,9,5,13,SCU_MODE_FUNC4}
};

// LCD4Bit lib function redefinitions
void LCD4Bit_digitalWrite(int pin,int state)
{
	if(state==1)
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, lcdPinInfo[pin].gpio, lcdPinInfo[pin].gpioBit);
	else
		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, lcdPinInfo[pin].gpio, lcdPinInfo[pin].gpioBit);
}
void LCD4Bit_delayMicroseconds(int t)
{
	volatile int t2 = t;
	while(t2>0) {
		volatile int i=0;
		while(i<17){
			i++;
		}
		t2--;
	}
}
void LCD4Bit_delay(int t)
{
	while(t>0) {
		LCD4Bit_delayMicroseconds(1000);
		t--;
	}	
}
//___________________________________


void Board_LCD_Init(uint8_t lines,uint8_t dotFormat)
{
	for(int32_t i=0 ; i<6; i++)
	{
		Chip_SCU_PinMuxSet(lcdPinInfo[i].port, lcdPinInfo[i].portBit, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | lcdPinInfo[i].func));
		Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, lcdPinInfo[i].gpio, lcdPinInfo[i].gpioBit);
		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, lcdPinInfo[i].gpio, lcdPinInfo[i].gpioBit);
	}

	LCD4Bit_init(lines,dotFormat);

}

void Board_LCD_clear(void)
{
	 LCD4Bit_clear();
}
void Board_LCD_printString(const char* text)
{
	LCD4Bit_printIn(text);
}
void Board_LCD_gotoXY(uint8_t x,uint8_t y)
{
	LCD4Bit_cursorTo(y, x);
}
void Board_LCD_configCursor(uint8_t onOff,uint8_t blinkOnOff)
{
	LCD4Bit_configureCursor(onOff,blinkOnOff);
}
//===========================================================================================================================





//================================================[Keyboard Management]========================================================
#define KEYBOARD_MAX_ROWS       4
#define KEYBOARD_MAX_COLUMNS    4

#define KEYBOARD_SCAN_STATE_START	0
#define KEYBOARD_SCAN_STATE_COL_LOW	1
#define KEYBOARD_SCAN_STATE_READ_ROWS	2

typedef struct
{
    int8_t port;
    int8_t portBit;
    int8_t gpio;
    int8_t gpioBit;
    int8_t func;

}KeyboardPinInfo;

typedef struct {
	int rows;
	int columns;
	int enable;
	int stateMatrix[KEYBOARD_MAX_ROWS][KEYBOARD_MAX_COLUMNS];
	int currentCol;
	int state;
}KeyboardInfo;

static KeyboardPinInfo keyboardRowsPinInfo[]={
        {4,0,2,0,SCU_MODE_FUNC0},{4,1,2,1,SCU_MODE_FUNC0},{4,2,2,2,SCU_MODE_FUNC0},{4,3,2,3,SCU_MODE_FUNC0}
};

static KeyboardPinInfo keyboardColsPinInfo[]={
        {1,5,1,8,SCU_MODE_FUNC0},{7,4,3,12,SCU_MODE_FUNC0},{7,5,3,13,SCU_MODE_FUNC0},{6,12,2,8,SCU_MODE_FUNC0}
};

static volatile KeyboardInfo keyboardInfo;


void Board_KEYBOARD_Init(uint8_t rows, uint8_t columns)
{
	keyboardInfo.enable=0;

	if(rows>KEYBOARD_MAX_ROWS)
		rows=KEYBOARD_MAX_ROWS;
	if(columns>KEYBOARD_MAX_COLUMNS)
		columns=KEYBOARD_MAX_COLUMNS;

	keyboardInfo.rows=rows;
	keyboardInfo.columns=columns;

        // configure Rows (Input)
        for(int32_t i=0 ; i<rows; i++)
        {
            Chip_SCU_PinMuxSet(keyboardRowsPinInfo[i].port, keyboardRowsPinInfo[i].portBit, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | keyboardRowsPinInfo[i].func));
            Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, keyboardRowsPinInfo[i].gpio, keyboardRowsPinInfo[i].gpioBit);
        }


	// configure Columns (output)
        for(int32_t i=0 ; i<columns; i++)
        {
            Chip_SCU_PinMuxSet(keyboardColsPinInfo[i].port, keyboardColsPinInfo[i].portBit, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | keyboardColsPinInfo[i].func));
            Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, keyboardColsPinInfo[i].gpio, keyboardColsPinInfo[i].gpioBit);

	    Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, keyboardColsPinInfo[i].gpio, keyboardColsPinInfo[i].gpioBit);
        }

	keyboardInfo.state=KEYBOARD_SCAN_STATE_START;
	for(uint32_t r=0; r<KEYBOARD_MAX_ROWS; r++)
	{
	    for(uint32_t c=0; c<KEYBOARD_MAX_COLUMNS; c++)
		keyboardInfo.stateMatrix[r][c]=1;
	}
	keyboardInfo.enable=1;
}

void Board_KEYBOARD_tick_ms(void)
{
	if(keyboardInfo.enable==1)
	{
		switch(keyboardInfo.state)
		{
			case KEYBOARD_SCAN_STATE_START:
			{
				keyboardInfo.currentCol=0;
				keyboardInfo.state=KEYBOARD_SCAN_STATE_COL_LOW;
				break;
			}
			case KEYBOARD_SCAN_STATE_COL_LOW:
			{
				if(keyboardInfo.currentCol>=keyboardInfo.columns)
				{
					keyboardInfo.state=KEYBOARD_SCAN_STATE_START;
				}
				else
				{
					// set col low
	                        	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, keyboardColsPinInfo[keyboardInfo.currentCol].gpio, keyboardColsPinInfo[keyboardInfo.currentCol].gpioBit);
					keyboardInfo.state=KEYBOARD_SCAN_STATE_READ_ROWS;
				}
				break;
			}
			case KEYBOARD_SCAN_STATE_READ_ROWS:
			{
				// read rows
                        	int row;
                        	for(row=0; row<keyboardInfo.rows;row++)
                        	{
                                	int rowVal = Chip_GPIO_GetPinState(LPC_GPIO_PORT, keyboardRowsPinInfo[row].gpio, keyboardRowsPinInfo[row].gpioBit);
                                	keyboardInfo.stateMatrix[row][keyboardInfo.currentCol] = rowVal;
                        	}
                                // set col high
                                Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, keyboardColsPinInfo[keyboardInfo.currentCol].gpio, keyboardColsPinInfo[keyboardInfo.currentCol].gpioBit);
				keyboardInfo.currentCol++;
				keyboardInfo.state=KEYBOARD_SCAN_STATE_COL_LOW;
				break;
			}
		}
	}
}

int Board_KEYBOARD_readMatrix(uint8_t row,uint8_t col)
{
	return keyboardInfo.stateMatrix[row][col];
}

void Board_KEYBOARD_enable(void)
{
	keyboardInfo.enable=1;
}
void Board_KEYBOARD_disable(void)
{
	keyboardInfo.enable=0;
}
//===========================================================================================================================

