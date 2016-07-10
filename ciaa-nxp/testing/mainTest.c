/*
    Unit Tests for EDU-CIAA Hardware library.
    Copyright (C) 2016  Ernesto gigliotti. ernestogigliotti@gmail.com

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

#include "utest.h"
#include "utest.c"

#include "testsLeds.c"
#include "testsSwitches.c"
#include "testsUart.c"
#include "testsEEPROM.c"
#include "testsDAC.c"
#include "testsADC.c"
#include "testsGPIO.c"
#include "tests485.c"
#include "testsTimers.c"


/* Hardware connections for testing:

232_RX	->	R100 ohm	->	232_TX
CH1    	->	GNDA
CH2    	->	GNDA
CH3    	->	GNDA
GPIO8	->	R100 ohm	->	GPIO7
RS485	-> echo terminal
*/

void startTesting(void)
{
    utest_print("Starting C testings...\r\n");

	utest_init();


	// LEDS Library
	utest_startTest(testLeds1,0,"LEDS: On Off Test");
	utest_startTest(testLeds2,0,"LEDS: Pwm Test");
	utest_startTest(testLeds3,0,"LEDS: Toogle Test");
    utest_print("____________________\r\n");

	// Switches Library
	utest_startTest(testSwitches1,0,"SW: idle status check");
    utest_print("____________________\r\n");

	// Uart Library
	utest_startTest(testUart1,testUartSetup,"UART: TX test");
	utest_startTest(testUart2,testUartSetup,"UART: RX polling test");
	utest_startTest(testUart3,testUartSetup,"UART: RX buffer test");
    utest_print("____________________\r\n");

	// EEPROM Library
	utest_startTest(testEEPROM1,0,"EEPROM: write/read byte Test");
	utest_startTest(testEEPROM2,0,"EEPROM: write invalid address Test");
	utest_startTest(testEEPROM3,0,"EEPROM: read invalid address Test");
    utest_print("____________________\r\n");

	// DAC Library
	utest_startTest(testDAC1,0,"DAC: write value Test");
	utest_startTest(testDAC2,0,"DAC: write invalid value Test");
	utest_startTest(testDAC3,0,"DAC: write table Test");
	utest_startTest(testDAC4,0,"DAC: write invalid table Test");
    utest_print("____________________\r\n");

	// ADC Library
    utest_startTest(testADC1,0,"ADC: enable chn 1 Test");
    utest_startTest(testADC2,0,"ADC: enable chn 2 Test");
    utest_startTest(testADC3,0,"ADC: enable chn 3 Test");
    utest_startTest(testADC4,0,"ADC: enable chn invalid Test");
    utest_startTest(testADC5,0,"ADC: read chn 1 Test");
    utest_startTest(testADC6,0,"ADC: read chn 2 Test");
    utest_startTest(testADC7,0,"ADC: read chn 3 Test");
    utest_startTest(testADC8,0,"ADC: read chn invalid Test");
    utest_print("____________________\r\n");

	// GPIO
    utest_startTest(testGPIO1,0,"GPIO: configure gpio0 input Test");
    utest_startTest(testGPIO2,0,"GPIO: configure gpio8 input Test");
    utest_startTest(testGPIO3,0,"GPIO: configure invalid gpio Test");
    utest_startTest(testGPIO4,0,"GPIO: configure invalid mode Test");
    utest_startTest(testGPIO5,0,"GPIO: configure invalid pull mode Test");
    utest_startTest(testGPIO6,0,"GPIO: read with pullup Test");
    utest_startTest(testGPIO7,0,"GPIO: read with pulldown Test");
    utest_startTest(testGPIO8,0,"GPIO: read invalid gpio Test");
    utest_startTest(testGPIO9,testGPIOwriteSetup,"GPIO: write 0 Test");
    utest_startTest(testGPIO10,testGPIOwriteSetup,"GPIO: write 1 Test");
    utest_startTest(testGPIO11,testGPIOwriteSetup,"GPIO: write invalid Test");

    utest_startTest(testGPIO12,testGPIOcallbackSetup,"GPIO: enable callback Test");
    utest_startTest(testGPIO13,testGPIOcallbackSetup,"GPIO: interrupt Test");
    utest_startTest(testGPIO14,testGPIOcallbackSetup,"GPIO: enable invalid Test");
    utest_print("____________________\r\n");


    // rs485 Uart Library
    utest_startTest(test485Uart1,test485UartSetup,"485 UART: TX test");
    utest_startTest(test485Uart2,test485UartSetup,"485 UART: RX polling test");
    utest_startTest(test485Uart3,test485UartSetup,"485 UART: RX buffer test");
    utest_print("____________________\r\n");

    // Timers Library
    utest_startTest(testTimers1,0,"TIMERS: configure timer Test");
    utest_startTest(testTimers2,0,"TIMERS: configure invalid timer Test");
    utest_startTest(testTimers3,testTimersSetup,"TIMERS: set callback Test");
    utest_startTest(testTimers4,testTimersSetup,"TIMERS: callback arg Test");
    utest_startTest(testTimers5,testTimersSetup,"TIMERS: set callback invalid Test");
    utest_startTest(testTimers6,testTimersSetup,"TIMERS: disable timer Test");
    utest_startTest(testTimers7,0,"TIMERS: disable timer invalid Test");

    utest_startTest(testTimers8,testTimersSetup,"TIMERS: set counter Test");
    utest_startTest(testTimers9,testTimersSetup,"TIMERS: set counter invalid Test");

    utest_startTest(testTimers10,testTimersSetup,"TIMERS: set prescaler Test");
    utest_startTest(testTimers11,testTimersSetup,"TIMERS: set prescaler invalid Test");

    utest_startTest(testTimers12,testTimersSetup,"TIMERS: get prescaler Test");
    utest_startTest(testTimers13,testTimersSetup,"TIMERS: get prescaler invalid Test");

    utest_startTest(testTimers14,testTimersSetup,"TIMERS: get counter Test");
    utest_startTest(testTimers15,testTimersSetup,"TIMERS: get counter invalid Test");

    utest_startTest(testTimers16,testTimersSetup,"TIMERS: set match Test");
    utest_startTest(testTimers17,testTimersSetup,"TIMERS: set match invalid Test");

    utest_startTest(testTimers18,testTimersSetup,"TIMERS: get match Test");
    utest_startTest(testTimers19,testTimersSetup,"TIMERS: get match invalid Test");

	utest_printStatistics();
}


