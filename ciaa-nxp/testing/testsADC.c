
void testADC1(void)
{
	int32_t r = mp_hal_enableADCchannel(1);

    utest_assertEqualsInt(0,(int)r);
}

void testADC2(void)
{
    int32_t r = mp_hal_enableADCchannel(2);

    utest_assertEqualsInt(0,(int)r);
}

void testADC3(void)
{
    int32_t r = mp_hal_enableADCchannel(3);

    utest_assertEqualsInt(0,(int)r);
}

void testADC4(void)
{
    int32_t r = mp_hal_enableADCchannel(56); // invalid channel

    utest_assertEqualsInt(-1,(int)r);
}

void testADC5(void)
{
    mp_hal_enableADCchannel(1);

	mp_hal_startADCconversion();

	uint16_t v = mp_hal_readADCchannel(1);

    utest_assertEqualsInt(0,(int)v);
}

void testADC6(void)
{
    mp_hal_enableADCchannel(2);

    mp_hal_startADCconversion();

    uint16_t v = mp_hal_readADCchannel(2);

    utest_assertEqualsInt(0,(int)v);
}

void testADC7(void)
{
    mp_hal_enableADCchannel(3);

    mp_hal_startADCconversion();

    uint16_t v = mp_hal_readADCchannel(3);

    utest_assertEqualsInt(0,(int)v);
}

void testADC8(void)
{
    mp_hal_enableADCchannel(1);

    mp_hal_startADCconversion();

    uint16_t v = mp_hal_readADCchannel(99); // invalid channel

    utest_assertEqualsInt(65535,(int)v);
}

