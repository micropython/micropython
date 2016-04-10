void testSwitches1(void)
{
	utest_assertEqualsInt(0,Buttons_GetStatusByNumber(0)); // sw1 released
	utest_assertEqualsInt(0,Buttons_GetStatusByNumber(1)); // sw2 released
	utest_assertEqualsInt(0,Buttons_GetStatusByNumber(2)); // sw3 released
	utest_assertEqualsInt(0,Buttons_GetStatusByNumber(3)); // sw4 released

	utest_assertEqualsInt(-1,Buttons_GetStatusByNumber(999)); // invalid sw returns -1
}






