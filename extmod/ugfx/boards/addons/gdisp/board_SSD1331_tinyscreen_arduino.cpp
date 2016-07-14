#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include "board_SSD1331.h"

#define LCD_BOARD_ID		0		// 0 or 1 - set by the position of a resistor near SX1505 (see schematic and board design)

// GPIO Pins
#define GPIO_DC				0x01
#define GPIO_CS				0x02
#define GPIO_RES			0x08
#define GPIO_BTN1			0x10
#define GPIO_BTN2			0x20
#define GPIO_BTN3			0x40
#define GPIO_BTN4			0x80
#define GPIO_CMD_START		~(GPIO_CS|GPIO_DC)
#define GPIO_DATA_START		~GPIO_CS
#define GPIO_TRANSFER_END	GPIO_CS

//GPIO Registers
#define GPIO_RegData		0x00
#define GPIO_RegDir			0x01
#define GPIO_RegPullUp		0x02

// Wire address of the SX1505 chip
#define GPIO_ADDR			0x20

static void writeGPIO(uint8_t regAddr, uint8_t regData)
{
  uint8_t oldTWBR=TWBR;
  TWBR=0;
  Wire.beginTransmission(GPIO_ADDR + LCD_BOARD_ID);
  Wire.write(regAddr); 
  Wire.write(regData);
  Wire.endTransmission();
  TWBR=oldTWBR;
}

static bool isDataMode = false;
static bool isCmdMode = false;

void SSD1331_init_board(void) {
	//setup GPIO, reset SSD1331
	writeGPIO(GPIO_RegData,~GPIO_RES);//reset low, CS/other pins high
	writeGPIO(GPIO_RegDir,~GPIO_RES);//set reset to output
	delay(5);
	writeGPIO(GPIO_RegDir,~(GPIO_CS|GPIO_DC));//reset to input, CS/DC output
	writeGPIO(GPIO_RegPullUp,GPIO_BTN1|GPIO_BTN2|GPIO_BTN3|GPIO_BTN4);//button pullup enable
	//init SPI
	SPI.begin();
	SPI.setDataMode(SPI_MODE0);//wrong mode, works because we're only writing. this mode is compatible with SD cards.
	SPI.setClockDivider(SPI_CLOCK_DIV2);
	
	isDataMode = isCmdMode = false;
}

void SSD1331_setpin_reset(int state) {
	// Ignore this as we did it in the init_board
}

void SSD1331_aquirebus(void) {
	// Do nothing as we do it in the data transfer
}

void SSD1331_releasebus(void) {
	if (!isDataMode && !isCmdMode) return;
	writeGPIO(GPIO_RegData,GPIO_TRANSFER_END);
	isDataMode = isCmdMode = false;
}

void SSD1331_write_cmd(uint8_t cmd) {
	if (!isCmdMode) {
		writeGPIO(GPIO_RegData,GPIO_CMD_START);
		isDataMode = false;
		isCmdMode = true;
	}
	SPI.transfer(cmd);
}

void SSD1331_write_data(uint8_t data) {
	if (!isDataMode) {
		writeGPIO(GPIO_RegData,GPIO_DATA_START);
		isDataMode = true;
		isCmdMode = false;
	}
	SPI.transfer(data);
}
