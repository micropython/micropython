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
    utest_print("starting testings...\r\n");

	utest_init();

	// LEDS Library
	utest_startTest(testLeds1,0,"LEDS: On Off Test");
	utest_startTest(testLeds2,0,"LEDS: Pwm Test");
	utest_startTest(testLeds3,0,"LEDS: Toogle Test");

	// Switches Library
	utest_startTest(testSwitches1,0,"SW: idle status check");

	// Uart Library
	utest_startTest(testUart1,testUartSetup,"UART: TX test");
	utest_startTest(testUart2,testUartSetup,"UART: RX polling test");
	utest_startTest(testUart3,testUartSetup,"UART: RX buffer test");

	// EEPROM Library
	utest_startTest(testEEPROM1,0,"EEPROM: write/read byte Test");
	utest_startTest(testEEPROM2,0,"EEPROM: write invalid address Test");
	utest_startTest(testEEPROM3,0,"EEPROM: read invalid address Test");

	// DAC Library
	utest_startTest(testDAC1,0,"DAC: write value Test");
	utest_startTest(testDAC2,0,"DAC: write invalid value Test");
	utest_startTest(testDAC3,0,"DAC: write table Test");
	utest_startTest(testDAC4,0,"DAC: write invalid table Test");

	// ADC Library
    utest_startTest(testADC1,0,"ADC: enable chn 1 Test");
    utest_startTest(testADC2,0,"ADC: enable chn 2 Test");
    utest_startTest(testADC3,0,"ADC: enable chn 3 Test");
    utest_startTest(testADC4,0,"ADC: enable chn invalid Test");
    utest_startTest(testADC5,0,"ADC: read chn 1 Test");
    utest_startTest(testADC6,0,"ADC: read chn 2 Test");
    utest_startTest(testADC7,0,"ADC: read chn 3 Test");
    utest_startTest(testADC8,0,"ADC: read chn invalid Test");


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


    // rs485 Uart Library
    utest_startTest(test485Uart1,testUartSetup,"485 UART: TX test");
    utest_startTest(test485Uart2,testUartSetup,"485 UART: RX polling test");
    utest_startTest(test485Uart3,testUartSetup,"485 UART: RX buffer test");


	utest_printStatistics();
}


