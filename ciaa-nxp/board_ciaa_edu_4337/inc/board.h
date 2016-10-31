/*
 * @brief EDU-CIAA-NXP board file
 *
 * @note
 * Copyright(C) Ernesto Gigliotti <ernestogiglotti@gmail.com>, 2016
 * Copyright(C) Marin Ribelotta, 2015
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __BOARD_H_
#define __BOARD_H_

#include "chip.h"
/* board_api.h is included at the bottom of this file after DEBUG setup */

#ifndef __SECTION_EXT
#define __SECTION_EXT(type, bank, name) __attribute__ ((section("." #type ".$" #bank "." #name)))
#endif

#ifndef __SECTION
#define __SECTION(type, bank) __attribute__ ((section("." #type ".$" #bank)))
#endif

#ifndef __DATA_EXT
#define __DATA_EXT(bank, name) __SECTION_EXT(data, bank, name)
#endif

#ifndef __DATA
#define __DATA(bank) __SECTION(data, bank)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** Define DEBUG_ENABLE to enable IO via the DEBUGSTR, DEBUGOUT, and
    DEBUGIN macros. If not defined, DEBUG* functions will be optimized
	out of the code at build time.
 */
#define DEBUG_ENABLE

/** Define DEBUG_SEMIHOSTING along with DEBUG_ENABLE to enable IO support
    via semihosting. You may need to use a C library that supports
	semihosting with this option.
 */
//#define DEBUG_SEMIHOSTING

/** Board UART used for debug output and input using the DEBUG* macros. This
    is also the port used for Board_UARTPutChar, Board_UARTGetChar, and
	Board_UARTPutSTR functions. */
#define DEBUG_UART LPC_USART2

/**
 * @}
 */

/* Board name */
#define BOARD_CIAA_EDU_NXP_4337




/* GPIOs */
#define BOARD_GPIO_MODE_INPUT		0
#define BOARD_GPIO_MODE_OUTPUT_PP	1
#define BOARD_GPIO_MODE_OUTPUT_OD	2

#define BOARD_GPIO_NOPULL		0
#define BOARD_GPIO_PULLUP		1
#define BOARD_GPIO_PULLDOWN		2

#define GPIO_MAX_NUMBER		8


/* Build for RMII interface */
#define USE_RMII
#define BOARD_ENET_PHY_ADDR	0x00


#define BUTTONS_BUTTON1_GPIO_PORT_NUM   0
#define BUTTONS_BUTTON1_GPIO_BIT_NUM    4

#define BUTTONS_BUTTON2_GPIO_PORT_NUM   0
#define BUTTONS_BUTTON2_GPIO_BIT_NUM    8

#define BUTTONS_BUTTON3_GPIO_PORT_NUM   0
#define BUTTONS_BUTTON3_GPIO_BIT_NUM    9

#define BUTTONS_BUTTON4_GPIO_PORT_NUM   1
#define BUTTONS_BUTTON4_GPIO_BIT_NUM    9

#define LED1_GPIO_PORT_NUM              1
#define LED1_GPIO_BIT_NUM               11
#define LED2_GPIO_PORT_NUM              1
#define LED2_GPIO_BIT_NUM               12

/* USB1 VBUS Enable GPIO pins */
#define USB1_VBUS_PORT_NUM          2
#define USB1_VBUS_PIN_NUM           5
#define USB1_VBUS_PIN_CFG           (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC4)
#define USB1_VBUS_GPIO_PORT_NUM     5
#define USB1_VBUS_GPIO_BIT_NUM      5




/**
 * @brief   Initializes all board periferals
 * @return  Nothing
 */
void Board_Init(void);





/**
 * @brief	Sets up board specific I2C interface
 * @param	id	: I2C Peripheral ID (I2C0, I2C1)
 * @return	Nothing
 */
void Board_I2C_Init(I2C_ID_T id);

/**
 * @brief	Sets up I2C Fast Plus mode
 * @param	id	: Must always be I2C0
 * @return	Nothing
 * @note	This function must be called before calling
 *          Chip_I2C_SetClockRate() to set clock rates above
 *          normal range 100KHz to 400KHz. Only I2C0 supports
 *          this mode.
 */
STATIC INLINE void Board_I2C_EnableFastPlus(I2C_ID_T id)
{
	Chip_SCU_I2C0PinConfig(I2C0_FAST_MODE_PLUS);
}

/**
 * @brief	Disable I2C Fast Plus mode and enables default mode
 * @param	id	: Must always be I2C0
 * @return	Nothing
 * @sa		Board_I2C_EnableFastPlus()
 */
STATIC INLINE void Board_I2C_DisableFastPlus(I2C_ID_T id)
{
	Chip_SCU_I2C0PinConfig(I2C0_STANDARD_FAST_MODE);
}

/**
 * @brief	Enable VBUS to USB1 port in Host mode
 * @return	Nothing
 * @sa		Board_USB1_DisableVbus()
 */
__STATIC_INLINE void Board_USB1_EnableVbus(void)
{
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, USB1_VBUS_GPIO_PORT_NUM, USB1_VBUS_GPIO_BIT_NUM);
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, USB1_VBUS_GPIO_PORT_NUM, USB1_VBUS_GPIO_BIT_NUM);
}

/**
 * @brief	Disable VBUS to USB1 port
 * @return	Nothing
 * @sa		Board_USB1_EnableVbus()
 */
__STATIC_INLINE void Board_USB1_DisableVbus(void)
{
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, USB1_VBUS_GPIO_PORT_NUM, USB1_VBUS_GPIO_BIT_NUM);
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, USB1_VBUS_GPIO_PORT_NUM, USB1_VBUS_GPIO_BIT_NUM);
}

/**
 * @brief	Initializes board specific GPIO Interrupt
 * @return	Nothing
 */
void Board_GPIO_Int_Init(void);

/**
 * @brief	Returns the MAC address assigned to this board
 * @param	mcaddr : Pointer to 6-byte character array to populate with MAC address
 * @return	Nothing
 */
void Board_ENET_GetMacADDR(uint8_t *mcaddr);

/**
 * @brief	Initialize pin muxing for a UART
 * @param	pUART	: Pointer to UART register block for UART pins to init
 * @return	Nothing
 */
void Board_UART_Init(LPC_USART_T *pUART);

/**
 * @brief	Initialize pin muxing for SDMMC interface
 * @return	Nothing
 */
void Board_SDMMC_Init(void);

/**
 * @brief	Initialize button(s) interface on board
 * @return	Nothing
 */
void Board_Buttons_Init(void);

/**
 * @brief       Configure button's callback
 * @param       buttonNumber : 0 to 3
 * @param       function : calback to be called. prototype: void function(void* arg);
 * @param       arg : argument passed to callback function when it is called
 * @return      Nothing
 */
void Board_Buttons_configureCallback(int buttonNumber,void(*function)(void*),void* arg);

/**
 * @brief       Returns Switch status
 * @param       BUTTONNumber    : number of switch from 1 to 4
 * @return      switch state - 1: pressed 0: no-pressed
 */
int Board_Buttons_GetStatusByNumber(int BUTTONNumber);


/**
 * @brief	Initialize I2S interface for the board and UDA1380
 * @param	pI2S	: Pointer to I2S register interface used on this board
 * @param	micIn	: If 1 MIC will be used as input, if 0 LINE_IN will be used
 * @return	Nothing
 */
void Board_Audio_Init(LPC_I2S_T *pI2S, int micIn);


/**
 * @brief	Initialize Pinmuxing for the LCD interface
 * @return	Nothing
 */
//void Board_LCD_Init(void);

/**
 * @brief	Write given data to LCD module
 * @param	data	: data to be written
 * @param	size	: number of data items
 * @return	Nothing
 */
void Board_LCD_WriteData(const uint8_t *data, uint16_t size);


/**
 * @brief       Write over UART device
 * @param       pUART    : LPC_USART0 or LPC_USART3
 * @param       buffer   : buffer to be written
 * @param       size     : amount of bytes to be written
 * @return      amount of bytes written
 */
uint32_t Board_UART_Write(LPC_USART_T *pUART, uint8_t const * const buffer, uint32_t const size);


/**
 * @brief       Sets RX buffer
 * @param       pUART    : LPC_USART0 or LPC_USART3
 * @param       pBuffer  : Pointer to RX buffer
 * @param       size     : Size of RX buffer
 * @param       timeout     : timeout in milliseconds before rx notification
 * @param       finalByte     : final byte value for rx notification
 * @return      void
 */
void Board_UART_setRxBuffer(LPC_USART_T *pUART,uint8_t* pBuffer,uint32_t size,uint32_t timeout, uint8_t finalByte);


/**
 * @brief       Resets RX buffer configuration
 * @return      void
 */
void Board_UART_resetRxBufferConfig(LPC_USART_T *pUART);


/**
 * @brief       This function must be called each millisecond for uart rx timeout calculation
 * @return      void
 */
void Board_UART_tick_ms(void);

/**
 * @brief       return 1 if new packet is available in rx buffer
 * @param       pUART    : LPC_USART0 or LPC_USART3
 * @return      return 1 if new packet is available in rx buffer
 */
uint32_t Board_UART_isNewPacket(LPC_USART_T *pUART);

/**
 * @brief       Prepare rx buffer for new reception
 * @param       pUART    : LPC_USART0 or LPC_USART3
 * @return      void
 */
void Board_UART_resetRx(LPC_USART_T *pUART);


/**
 * @brief       Returns packet size
 * @param       pUART    : LPC_USART0 or LPC_USART3
 * @return      Packet size
 */
uint32_t Board_UART_getRxSize(LPC_USART_T *pUART);

/**
 * @brief       Configura UART
 * @param       pUART    : LPC_USART0 or LPC_USART3
 * @param       baud     : Baudrate
 * @param       dataBits : Amount of bits of data
 * @param       stopBits : Stop bits (1 or 2)
 * @param       parity   : 0: disable - 1: odd - 2: even - 3: forced 0 - 4: forced 1
 * @return      void
 */
void Board_UART_setConfig(LPC_USART_T *pUART,int32_t baud, int32_t stopBits,int32_t parity);

/**
 * @brief       Returns received char
 * @param       pUART    : LPC_USART0 or LPC_USART3
 * @return      Received char or -1
 */
int32_t Board_UART_getChar(LPC_USART_T *pUART);

/**
 * @brief       Returns if there is a char available for read
 * @param       pUART    : LPC_USART0 or LPC_USART3
 * @return      0: there is not a char available
 */
int32_t Board_UART_charAvailable(LPC_USART_T *pUART);


/**
 * @brief       Initializes GPIOs as input.
 * @return      void
 */
void Board_GPIOs_Init(void);

/**
 * @brief       Configures a specific gpio.
 * @param       gpioNumber : 0 to 8
 * @param       mode : BOARD_GPIO_MODE_INPUT, BOARD_GPIO_MODE_OUTPUT_PP, BOARD_GPIO_MODE_OUTPUT_OD
 * @param       pullup : BOARD_GPIO_NOPULL, BOARD_GPIO_PULLUP, BOARD_GPIO_PULLDOWN
 * @return      0: OK. -1: error.
 */
int32_t Board_GPIOs_configure(int32_t gpioNumber,int32_t mode, int32_t pullup);


/**
 * @brief       Reads gpio's value.
 * @param       gpioNumber : 0 to 8
 * @return      0 or 1
 */
uint32_t Board_GPIOs_readValue(int32_t gpioNumber);


/**
 * @brief       Writes gpio's value.
 * @param       gpioNumber : 0 to 8
 * @param       value : 0 or 1
 * @return      0:OK. -1: Error
 */
int32_t Board_GPIOs_writeValue(int32_t gpioNumber,uint8_t value);


/**
 * @brief       Enables interrupt channel available to GPIO. There are only 4 channels available.
 * @param       gpioNumber : 0 to 8
 * @param       function : calback to be called. prototype: void function(void* arg);
 * @param       arg : argument passed to callback function when it is called
 * @param       flagEdgeLevel : 1: edge mode - 0: level mode
 * @param       flagHighLow : 1: high level/rise edge - 0: low level/fall edge
 * @return      1 if int was enabled
 */
bool Board_GPIOs_enableIntCallback(int gpioNumber,void(*function)(void*),void* arg, uint8_t flagEdgeLevel, uint8_t flagHighLow);

/**
 * @brief       Disables interrupt from GPIO
 * @param       gpioNumber : 0 to 8
 * @return      void
 */
void Board_GPIOs_disableIntCallback(int gpioNumber);



/**
 * @brief       Writes a 10-bit-value to DAC output
 * @param       value
 * @return      0: OK. -1: value out of range.
 */
int32_t Board_DAC_writeValue(uint32_t value);

/**
 * @brief       Sets sampless' frecuency
 * @param       value between 3112 and 1000000
 * @return      void
 */
void Board_DAC_setSampleRate(uint32_t freq);

/**
 * @brief       Configure and start a DMA transfer from a buffer
 * @param       buffer : Buffer with samples (10-bit value)
 * @param       size : Number of samples
 * @param       flagCyclic : 1: cyclic mode, 0: just one write
 * @return      Amount of bytes transferred
 */
int32_t Board_DAC_writeDMA(uint16_t* buffer, uint32_t size, bool flagCyclic);


/**
 * @brief       Sets the led state
 * @param       LEDNumber : number of led : 0 to 2
 * @param       on : 0: led on. 1: led off
 * @return      void
 */
void Board_LED_Set(uint8_t LEDNumber, bool On);

/**
 * @brief       Returns the led state
 * @param       LEDNumber : number of led : 0 to 2
 * @return      led state. 0:on. 1:off
 */
bool Board_LED_Test(uint8_t LEDNumber);


/**
 * @brief       Inverts the led state
 * @param       LEDNumber : number of led : 0 to 2
 * @return      void
 */
void Board_LED_Toggle(uint8_t LEDNumber);


/**
 * @brief       This function must be called every 1ms for RGB PWM management
 * @return      void
 */
void Board_LED_PWM_tick_ms(void);


/**
 * @brief       sets a PWM value between 0 and 15
 * @param       pwmNumber : 0: red, 1: green, 2: blue
 * @param       value : PWM duty cycle (0 to 15)
 * @return      void
 */
void Board_LED_PWM_SetValue(uint8_t pwmNumber,uint8_t value);


/**
 * @brief       get pwm's value
 * @param       pwmNumber : 0: red, 1: green, 2: blue
 * @return      PWM duty cycle (0 to 15)
 */
uint8_t Board_LED_PWM_GetValue(uint8_t pwmNumber);



/**
 * @brief       Initializes TIMERs modules.
 * @return      void
 */
void Board_TIMER_Init(void);

/**
 * @brief       Init timer module working as timer.
 * @param       timerNum : Number of timer (0 to 3)
 * @param       presc : Prescaler value (0 to 0xFFFFFFFF)
 * @param       matchValue : Match value (0 to 0xFFFFFFFF). Timer's interrupt will happen when timer counter equals this value.
 * @param       flagOnce : 1: when timer reaches the match value, it stops. 0: Timer keeps counting
 * @return      0:OK. -1:ERROR
 */
int32_t Board_TIMER_EnableTimerAsTimer(uint8_t timerNum, uint32_t presc,uint32_t matchValue,bool flagOnce);

/**
 * @brief       Disable timer module.
 * @param       timerNum : Number of timer (0 to 3)
 * @return      0:OK. -1:ERROR
 */
int32_t Board_TIMER_DisableTimer(uint8_t timerNum);

/**
 * @brief       Sets the function that will be called by timer's interupt
 * @param       timerNum : Number of timer (0 to 3)
 * @param       function : calback to be called. prototype: void function(void* arg);
 * @param       arg : argument passed to callback function when it is called
 * @return      0:OK. -1:ERROR
 */
int32_t Board_TIMER_SetCallback(uint8_t timerNum,void(*function)(void*),void* arg);

/**
 * @brief       Returns Timer's base clock frequency in Hertz.
 * @return      Frequency in Hertz
 */
uint32_t Board_TIMER_getClockFrequency(void);

/**
 * @brief       Load current timer's counter value
 * @param       timerNum : Number of timer (0 to 3)
 * @param       value : Value to be loaded in Counter register
 * @return      0:OK. -1:ERROR
 */
int32_t Board_TIMER_SetTimerCounter(uint8_t timerNum,uint32_t value);


/**
 * @brief       Returns Timer's counter value.
 * @param       timerNum : Number of timer (0 to 3)
 * @return      Counter register
 */
uint32_t Board_TIMER_GetTimerCounter(uint8_t timerNum);

/**
 * @brief       Load current timer's prescaller value
 * @param       timerNum : Number of timer (0 to 3)
 * @param       value : Value to be loaded in Prescaler register
 * @return      0:OK. -1:ERROR
 */
int32_t Board_TIMER_SetTimerPrescaler(uint8_t timerNum,uint32_t value);

/**
 * @brief       Returns Timer's prescaler value.
 * @param       timerNum : Number of timer (0 to 3)
 * @return      Prescaler register
 */
uint32_t Board_TIMER_GetTimerPrescaler(uint8_t timerNum);

/**
 * @brief       Load current timer's match value
 * @param       timerNum : Number of timer (0 to 3)
 * @param       value : Value to be loaded in Match register
 * @return      0:OK. -1:ERROR
 */
int32_t Board_TIMER_SetTimerMatch(uint8_t timerNum,uint32_t value);

/**
 * @brief       Returns Timer's match value.
 * @param       timerNum : Number of timer (0 to 3)
 * @return      Match register
 */
uint32_t Board_TIMER_GetTimerMatch(uint8_t timerNum);


/**
 * @brief       Configure the global frequency for all pwm outs
 * @param       freq : Frequency in Hz
 * @return      void
 */
void Board_PWM_SetFrequency(uint32_t freq);

/**
 * @brief       Enable PWM out (11 available)
 * @param       outNumber : Number of PWM out enabled (0 to 10)
 * @return      void
 */
void Board_PWM_ConfigureOut(uint8_t outNumber);

/**
 * @brief       Set duty cycle for the specified PWM outs
 * @param       outNumber : Number of PWM out (0 to 10)
 * @param       duty : Duty cycle (0 to 100)
 * @return      void
 */
void Board_PWM_SetDutyCycle(uint8_t outNumber, uint8_t duty);



/**
 * @brief       Initializes ADC Module
 * @return      void
 */
void Board_ADC_Init(void);

/**
 * @brief       Enable ADC channel mapped to ADC0 module
 * @param       channelNumber : Number of input channel (1, 2 or 3)
 * @return      0: OK. -1: invalid channel
 */
int32_t Board_ADC_EnableChannel(uint8_t channelNumber);


/**
 * @brief       Start a ADC conversion and wait until is finished
 * @return      void
 */
void Board_ADC_StartConversion(void);

/**
 * @brief       Read an ADC channel after a conversion
 * @param       channelNumber : Number of input channel (1, 2 or 3)
 * @return      A 10bit analog value from the specified channel
 */
uint16_t Board_ADC_readValue(uint8_t channelNumber);


/**
 * @brief       Write a byte in EEPROM memory
 * @param       addr : Address relative to eeprom start (0x0000 to 0x3F7F)
 * @param       value : Byte to be written in specified address
 * @return      0: OK, -1: invalid address
 */
int32_t Board_EEPROM_writeByte(uint32_t addr,uint8_t value);

/**
 * @brief       Read a byte from EEPROM memory
 * @param       addr : Address relative to eeprom start (0x0000 to 0x3F7F)
 * @return      byte value read or -1 if address is invalid
 */
int32_t Board_EEPROM_readByte(uint32_t addr);



/**
 * @brief       Initializes SPI Module in Master mode
 * @return      void
 */
void Board_SSP_Init(void);

/**
 * @brief       Configure SPI module
 * @param       bits : bits for each spi frame (4, 8 or 16)
 * @param       clockMode : SPI Mode : 0,1,2 or 3
 * @param       bitrate : SPI clock frequency in Hz
 * @return      void
 */
void Board_SSP_config(uint8_t bits, uint8_t clockMode, uint32_t bitrate);

/**
 * @brief       Send data using SPI module
 * @param       buffer : buffer to send
 * @param       bufferLen : Amount of frames to be sended
 * @return      Amount of frames sended
 */
uint32_t Board_SSP_writeBuffer(const uint8_t *buffer, uint32_t bufferLen);

/**
 * @brief       Receive data using SPI module
 * @param       buffer : buffer for reception
 * @param       bufferLen : Amount of frames to be received
 * @return      Amount of frames received
 */
uint32_t Board_SSP_readBuffer(uint8_t *buffer, uint32_t bufferLen);

/**
 * @brief       Send and receive at the same time data using SPI module
 * @param       bufferTx : buffer to send
 * @param       bufferRx : buffer for reception
 * @param       bufferLen : Amount of frames to be sended
 * @return      Amount of frames sended and received
 */
uint32_t Board_SSP_transfer(uint8_t *bufferTx, uint8_t *bufferRx, uint32_t bufferLen);


/**
 * @brief       Initialize RTC module
 * @return      void
 */
void Board_RTC_Init(void);

/**
 * @brief       Calibrate RTC module
 * @param       value : Calibration value, should be in range from -131,072 to 131,072
 * @return      void
 */
void Board_RTC_calibration(uint32_t value);

/**
 * @brief       Return current RTC module datetime
 * @param       hr : Pointer to variable where Hour (24hs format) will be stored
 * @param       min : Pointer to variable where Minutes will be stored
 * @param       sec : Pointer to variable where Seconds will be stored
 * @param       day : Pointer to variable where Day of Month will be stored
 * @param       mon : Pointer to variable where Month will be stored
 * @param       yr : Pointer to variable where Year will be stored
 * @param       dayOfWeek : Pointer to variable where Day of Week will be stored
 * @return      void
 */
void Board_RTC_getTime(uint32_t* hr,uint32_t* min, uint32_t* sec, uint32_t* day, uint32_t* mon, uint32_t* yr,uint32_t* dayOfWeek);


/**
 * @brief       Set current RTC module datetime
 * @param       hr : Hour (24hs format)
 * @param       min : Minutes
 * @param       sec : Seconds
 * @param       day : Day
 * @param       mon : Month
 * @param       yr : Year
 * @param       dayOfWeek : Day of Week
 * @return      void
 */
void Board_RTC_setTime(uint32_t hr,uint32_t min, uint32_t sec, uint32_t day, uint32_t mon, uint32_t yr,uint32_t dayOfWeek);


/**
 * @brief       Write a value in RTC Backup registers space
 * @param       address : Back up register address (o to 63)
 * @param       value : 32-bit value to be written
 * @return      void
 */
void Board_RTC_writeBkpRegister(uint8_t address,uint32_t value);

/**
 * @brief       Read a value from RTC Backup registers space
 * @param       address : Back up register address (o to 63)
 * @return      32-bit value
 */
uint32_t Board_RTC_readBkpRegister(uint8_t address);


/**
 * @brief       Set Alarm for RTC module
 * @param       hr : Hour (24hs format)
 * @param       min : Minutes
 * @param       sec : Seconds
 * @param       day : Day
 * @param       mon : Month
 * @param       yr : Year
 * @param       dayOfWeek : Day of Week
 * @param       alarmMask :  Or'ed bit values for time types:
 * 				bit 0 : second
 * 				bit 1 : minute
 * 				bit 2 : hour
 * 				bit 3 : day of month
 * 				bit 4 : day of week
 * 				bit 5 : always 0 (not implemented)
 * 				bit 6 : month
 * 				bit 7 : year
 * 
 * @return      void
 */
void Board_RTC_setAlarmTime(uint32_t hr,uint32_t min, uint32_t sec, uint32_t day, uint32_t mon, uint32_t yr,uint32_t dayOfWeek,uint32_t alarmMask);

/**
 * @brief       Return Alarm RTC module datetime
 * @param       hr : Pointer to variable where Hour (24hs format) will be stored
 * @param       min : Pointer to variable where Minutes will be stored
 * @param       sec : Pointer to variable where Seconds will be stored
 * @param       day : Pointer to variable where Day of Month will be stored
 * @param       mon : Pointer to variable where Month will be stored
 * @param       yr : Pointer to variable where Year will be stored
 * @param       dayOfWeek : Pointer to variable where Day of Week will be stored
 * @return      void
 */
void Board_RTC_getAlarmTime(uint32_t* hr,uint32_t* min, uint32_t* sec, uint32_t* day, uint32_t* mon, uint32_t* yr,uint32_t* dayOfWeek);

/**
 * @brief       Disable RTC Alarm
 * @return      void
 */
void Board_RTC_disableAlarm(void);

/**
 * @brief       Sets the function that will be called by RTC Alarm interupt
 * @param       function : calback to be called. prototype: void function(void* arg);
 * @param       arg : argument passed to callback function when it is called
 * @return      void
 */
void Board_RTC_setAlarmCallback(void(*function)(void*),void* arg);



void Board_I2C_Master_SetSpeed(uint32_t speed);
uint32_t Board_I2C_Master_Read(uint8_t slaveAddr, uint8_t *buff, int len);
uint32_t Board_I2C_Master_Write(uint8_t slaveAddr, uint8_t *buff, int len);
uint32_t Board_I2C_Master_WriteCmdAdnRead(uint8_t slaveAddr, uint8_t cmd,uint8_t *buff, int len);



#include "../src/ponchos/poncho_ui.h"


/**
 * @}
 */

#include "board_api.h"

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H_ */
