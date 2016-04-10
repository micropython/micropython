#include "utest.h"
#include "utest.c"

#include "testsLeds.c"
#include "testsSwitches.c"
#include "testsUart.c"

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
	utest_startTest(testUart2,testUartSetup,"UART: RX buffer test");


	utest_printStatistics();
}


