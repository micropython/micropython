#include "utest.h"
#include "utest.c"

void test1(void)
{
	utest_assertEqualsInt(5,5);
	utest_assertNotEqualsInt(5,7);

	utest_assertNull(0x0);
	utest_assertNotNull(0x99);

	utest_assertEqualsFloat(5.8,5.8);
}

void test2(void)
{
    utest_assertEqualsInt(5,5);
    utest_assertNotEqualsInt(5,7);

    utest_assertNull(0x01);
    utest_assertNotNull(0x99);

    utest_assertEqualsFloat(5.8,5.8);
}



void startTesting(void)
{
    utest_print("starting testings...\r\n");

	utest_init();

	utest_startTest(test1,0,"test1");
	utest_startTest(test2,0,"test2");

	utest_printStatistics();
}


