/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string.h>
#include "ciaanxp_mphal.h"
#include "board.h"

static int interrupt_char;

static volatile mp_uint_t tick_ct = 0;

void SysTick_Handler(void) {
	tick_ct++;
	Board_UART_tick_ms();
	Board_LED_PWM_tick_ms();
	Board_KEYBOARD_tick_ms();
}


void mp_hal_init(void) {
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/1000);
        Board_Init();
    	Board_Buttons_Init();
}



// Time funcions
mp_uint_t mp_hal_get_milliseconds(void) {
    return tick_ct;
}

// Time funcions
mp_uint_t mp_hal_ticks_ms(void) {
    return mp_hal_get_milliseconds();
}

void mp_hal_milli_delay(mp_uint_t ms) {
	uint32_t end = tick_ct + ms;
	while(tick_ct < end)
		__WFI();
}

void mp_hal_delay_ms(mp_uint_t ms) { mp_hal_milli_delay(ms); }

// STD Functions
void mp_hal_set_interrupt_char(int c) {
    interrupt_char = c;
}

int mp_hal_stdin_rx_chr(void) {
   for (;;) {
        int r = Board_UARTGetChar();
        if (r!= EOF) {
            return r;
        } else {
            __WFI();
        }
    }
}

void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    for (; len > 0; --len) {
    	Board_UARTPutChar(*str++);
    }
}

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}

void mp_hal_stdout_tx_strn_cooked(const char *str, mp_uint_t len) {
    for (; len > 0; --len) {
        if (*str == '\n') {
        	Board_UARTPutChar('\r');
        }
        Board_UARTPutChar(*str++);
    }
}


// RS232 Functions
uint32_t mp_hal_rs232_write(uint8_t const * const buffer, uint32_t size,uint32_t delay)
{
	if(delay==0)
		return Board_UART_Write(LPC_USART3,buffer,size);

	uint32_t i=0;
	while(size>0)
	{
		Board_UART_Write(LPC_USART3,&(buffer[i]),1);
		mp_hal_milli_delay(delay);
		size--;
		i++;
	}
	return i;
}

void mp_hal_rs232_setRxBuffer(uint8_t* pBuffer,uint32_t size,uint32_t timeout, uint8_t finalByte)
{
	Board_UART_setRxBuffer(LPC_USART3,pBuffer,size,timeout,finalByte);
}

void mp_hal_rs232_setConfig(int32_t baud,int32_t stopBits,int32_t parity)
{
	Board_UART_setConfig(LPC_USART3,baud,stopBits,parity);
}

int32_t mp_hal_rs232_isNewPacketAvailable(void)
{
	return (int32_t)Board_UART_isNewPacket(LPC_USART3);
}

uint32_t mp_hal_rs232_getPacketSize(void)
{
	return Board_UART_getRxSize(LPC_USART3);
}

int32_t mp_hal_rs232_getChar(void)
{
	return  Board_UART_getChar(LPC_USART3);
}

int32_t mp_hal_rs232_charAvailable(void)
{
	return Board_UART_charAvailable(LPC_USART3);
}

void mp_hal_rs232_resetRxPacket(void)
{
	Board_UART_resetRx(LPC_USART3);
}

// RS485 Functions
uint32_t mp_hal_rs485_write(uint8_t const * const buffer, uint32_t size, uint32_t delay)
{
        if(delay==0)
                return Board_UART_Write(LPC_USART0,buffer,size);

        uint32_t i=0;
        while(size>0)
        {
                Board_UART_Write(LPC_USART0,&(buffer[i]),1);
                mp_hal_milli_delay(delay);
                size--;
                i++;
        }
        return i;
}

void mp_hal_rs485_setRxBuffer(uint8_t* pBuffer,uint32_t size,uint32_t timeout, uint8_t finalByte)
{
        Board_UART_setRxBuffer(LPC_USART0,pBuffer,size,timeout,finalByte);
}

void mp_hal_rs485_setConfig(int32_t baud,int32_t stopBits,int32_t parity)
{
        Board_UART_setConfig(LPC_USART0,baud,stopBits,parity);
}

int32_t mp_hal_rs485_isNewPacketAvailable(void)
{
        return (int32_t)Board_UART_isNewPacket(LPC_USART0);
}

uint32_t mp_hal_rs485_getPacketSize(void)
{
        return Board_UART_getRxSize(LPC_USART0);
}

int32_t mp_hal_rs485_getChar(void)
{
        return  Board_UART_getChar(LPC_USART0);
}

int32_t mp_hal_rs485_charAvailable(void)
{
        return Board_UART_charAvailable(LPC_USART0);
}

void mp_hal_rs485_resetRxPacket(void)
{
        Board_UART_resetRx(LPC_USART0);
}

// Buttons
void mp_hal_configureButtonCallback(int buttonNumber,void(*function)(void*),void* arg)
{
	Board_Buttons_configureCallback(buttonNumber,function,arg);
}

// GPIOs
void mp_hal_configureGPIOs(int32_t gpioNumber,int32_t mode, int32_t pullup)
{
	Board_GPIOs_configure(gpioNumber,mode,pullup);
}

int32_t mp_hal_readGPIO(int32_t gpioNumber)
{
	return Board_GPIOs_readValue(gpioNumber);
}

void mp_hal_writeGPIO(int32_t gpioNumber, uint8_t value)
{
	Board_GPIOs_writeValue(gpioNumber,value);
}

bool mp_hal_enableIntCallbackGPIO(int gpioNumber,void(*function)(void*),void* arg, uint8_t flagEdgeLevel, uint8_t flagHighLow)
{
	return Board_GPIOs_enableIntCallback(gpioNumber,function,arg,flagEdgeLevel,flagHighLow);
}

void mp_hal_disableIntCallbackGPIO(int gpioNumber)
{
	Board_GPIOs_disableIntCallback(gpioNumber);
}

//DAC
void mp_hal_writeDAC(uint32_t value)
{
	Board_DAC_writeValue(value);
}

void mp_hal_setSampleRateDAC(uint32_t freq)
{
	Board_DAC_setSampleRate(freq);
}

int32_t mp_hal_writeDMADAC(uint16_t* buffer, uint32_t size, bool flagCyclic)
{
	return Board_DAC_writeDMA(buffer,size,flagCyclic);
}

// LEDS
void mp_hal_setPwmRGBValue(uint8_t pwmNumber,uint8_t value)
{
	if(value<=15)
		Board_LED_PWM_SetValue(pwmNumber,value);
}

uint8_t mp_hal_getPwmRGBValue(uint8_t pwmNumber)
{
	return Board_LED_PWM_GetValue(pwmNumber);
}

//TIMERs
void mp_hal_enableTimerAsTimer(uint8_t timerNum, uint32_t presc,uint32_t matchValue,bool flagOnce)
{
	Board_TIMER_EnableTimerAsTimer(timerNum,presc,matchValue,flagOnce);
}

void mp_hal_disableTimer(uint8_t timerNum)
{
	Board_TIMER_DisableTimer(timerNum);
}

void mp_hal_setTimerCallback(uint8_t timerNum,void(*function)(void*),void* arg)
{
	Board_TIMER_SetCallback(timerNum,function,arg);
}

uint32_t mp_hal_getTimerClockFrequency(void)
{
	return Board_TIMER_getClockFrequency();
}

void mp_hal_setTimerCounter(uint8_t timerNum,uint32_t value)
{
	Board_TIMER_SetTimerCounter(timerNum,value);
}

uint32_t mp_hal_getTimerCounter(uint8_t timerNum)
{
	return Board_TIMER_GetTimerCounter(timerNum);
}

void mp_hal_setTimerPrescaler(uint8_t timerNum,uint32_t value)
{
	Board_TIMER_SetTimerPrescaler(timerNum,value);
}

uint32_t mp_hal_getTimerPrescaler(uint8_t timerNum)
{
	return Board_TIMER_GetTimerPrescaler(timerNum);
}

void mp_hal_setTimerMatch(uint8_t timerNum,uint32_t value)
{
	Board_TIMER_SetTimerMatch(timerNum,value);
}

uint32_t mp_hal_getTimerMatch(uint8_t timerNum)
{
	return Board_TIMER_GetTimerMatch(timerNum);
}


// PWM
void mp_hal_setPWMFequency(uint32_t freq)
{
	Board_PWM_SetFrequency(freq);
}

void mp_hal_configurePWMOut(uint8_t outNumber)
{
	Board_PWM_ConfigureOut(outNumber);
}

void mp_hal_setPWMDutyCycle(uint8_t outNumber, uint8_t duty)
{
	Board_PWM_SetDutyCycle(outNumber, duty);
}

//ADC
void mp_hal_enableADCchannel(uint8_t channelNumber)
{
	Board_ADC_EnableChannel(channelNumber);
}

void mp_hal_startADCconversion(void)
{
	Board_ADC_StartConversion();
}

uint16_t mp_hal_readADCchannel(uint8_t channelNumber)
{
	return Board_ADC_readValue(channelNumber);
}


// Keyboard
void mp_hal_initKeyboard(uint8_t rows, uint8_t columns)
{
	Board_KEYBOARD_Init(rows, columns);
}
uint8_t mp_hal_readMatrixKeyboard(uint8_t row, uint8_t col)
{
	return Board_KEYBOARD_readMatrix(row,col);
}

//LCD
void mp_hal_initLCD(uint8_t lines,uint8_t dotFormat)
{
	Board_LCD_Init(lines,dotFormat);
}
void mp_hal_clearLCD(void)
{
	Board_LCD_clear();
}
void mp_hal_printStringLCD(const char* text)
{
	Board_LCD_printString(text);
}
void mp_hal_gotoXYLCD(uint8_t x,uint8_t y)
{
	Board_LCD_gotoXY(x,y);
}
void mp_hal_configCursorLCD(uint8_t onOff,uint8_t blinkOnOff)
{
	Board_LCD_configCursor(onOff,blinkOnOff);
}

//EEPROM
void mp_hal_writeByteEEPROM(uint32_t addr,uint8_t value)
{
	Board_EEPROM_writeByte(addr,value);
}

uint8_t mp_hal_readByteEEPROM(uint32_t addr)
{
	return Board_EEPROM_readByte(addr);
}

//SPI
void mp_hal_configSPI(uint8_t bits, uint8_t clockMode, uint32_t bitrate)
{
	Board_SSP_config(bits, clockMode, bitrate);
}

uint32_t mp_hal_writeSPI(const uint8_t *buffer, uint32_t bufferLen)
{
	return Board_SSP_writeBuffer(buffer, bufferLen);
}

uint32_t mp_hal_readSPI(uint8_t *buffer, uint32_t bufferLen)
{
	return Board_SSP_readBuffer(buffer, bufferLen);
}

//RTC
void mp_hal_initRTC(void)
{
	Board_RTC_Init();
}

void mp_hal_setTimeRTC(uint32_t hr,uint32_t min, uint32_t sec, uint32_t day, uint32_t mon, uint32_t yr,uint32_t dayOfWeek)
{
	Board_RTC_setTime(hr,min, sec, day, mon, yr,dayOfWeek);
}

void mp_hal_getTimeRTC(uint32_t* hr,uint32_t* min, uint32_t* sec, uint32_t* day, uint32_t* mon, uint32_t* yr,uint32_t* dayOfWeek)
{
	Board_RTC_getTime(hr,min, sec, day, mon, yr,dayOfWeek);
}

void mp_hal_setCalibRTC(uint32_t value)
{
	Board_RTC_calibration(value);
}

void mp_hal_writeBkpRegisterRTC(uint8_t address,uint32_t value)
{
	Board_RTC_writeBkpRegister(address,value);
}

uint32_t mp_hal_readBkpRegisterRTC(uint8_t address)
{
	return Board_RTC_readBkpRegister(address);
}


void mp_hal_setAlarmTimeRTC(uint32_t hr,uint32_t min, uint32_t sec, uint32_t day, uint32_t mon, uint32_t yr,uint32_t dayOfWeek,uint32_t alarmMask)
{
	Board_RTC_setAlarmTime(hr,min, sec, day, mon, yr,dayOfWeek, alarmMask);
}
void mp_hal_getAlarmTimeRTC(uint32_t* hr,uint32_t* min, uint32_t* sec, uint32_t* day, uint32_t* mon, uint32_t* yr,uint32_t* dayOfWeek)
{
	Board_RTC_getAlarmTime(hr,min, sec, day, mon, yr,dayOfWeek);
}
void mp_hal_disableAlarmRTC(void)
{
	Board_RTC_disableAlarm();
}
void mp_hal_setCallbackRTC(void(*function)(void*),void* arg)
{
	Board_RTC_setAlarmCallback(function,arg);
}

//I2C
void mp_hal_setSpeedI2C(uint32_t speed)
{
	Board_I2C_Master_SetSpeed(speed);
}
uint32_t mp_hal_masterReadI2C(uint8_t slaveAddr, uint8_t *buff, int len)
{
	return Board_I2C_Master_Read(slaveAddr,buff,len);
}
uint32_t mp_hal_masterWrite(uint8_t slaveAddr, uint8_t *buff, int len)
{
	return Board_I2C_Master_Write(slaveAddr,buff,len);
}
uint32_t mp_hal_masterWriteCmdAdnRead(uint8_t slaveAddr, uint8_t cmd,uint8_t *buff, int len)
{
	return Board_I2C_Master_WriteCmdAdnRead(slaveAddr,cmd,buff,len);
}

