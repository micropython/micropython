
void testDAC1(void)
{
	int32_t r = mp_hal_writeDAC(512);
	utest_assertEqualsInt(0,(int)r);
}

void testDAC2(void)
{
    int32_t r = mp_hal_writeDAC(2000); // invalid value
    utest_assertEqualsInt(-1,(int)r);
}

void testDAC3(void)
{
	const uint16_t TEST_TABLE[4] = {0,8,16,2};

	mp_hal_setSampleRateDAC(10000); // 10Khz

    int32_t r = mp_hal_writeDMADAC((uint16_t*)TEST_TABLE, 4, 1);
	utest_assertEqualsInt(8,(int)r);
}

void testDAC4(void)
{
    int32_t r = mp_hal_writeDMADAC(0, 0, 1);
    utest_assertEqualsInt(-1,(int)r);
}


