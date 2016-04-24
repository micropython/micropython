#include "utest.h"
#include "utest.c"

#include "testsLeds.c"
#include "testsSwitches.c"
#include "testsUart.c"
#include "testsEEPROM.c"
#include "testsDAC.c"
#include "testsADC.c"

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



	utest_printStatistics();
}


