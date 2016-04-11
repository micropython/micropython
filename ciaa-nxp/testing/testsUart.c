void testUartSetup(void)
{
	mp_hal_rs232_setConfig(9600,1,0);

    // clean buffer
    while(mp_hal_rs232_getChar()!=-1)
        mp_hal_delay_ms(1);
    mp_hal_rs232_resetRxPacket();

}

void testUart1(void)
{
	char testString[] = "Test string";

	int r = mp_hal_rs232_write((const uint8_t * const)testString,12,0);

	utest_assertEqualsInt(12,r);
}

void testUart2(void)
{
    char testString[] = "A";

    mp_hal_rs232_write((const uint8_t * const)testString,1,0);
	mp_hal_delay_ms(10);


	int flagChar = mp_hal_rs232_charAvailable();
	utest_assertEqualsInt(1,flagChar);


	int c = mp_hal_rs232_getChar();
	utest_assertEqualsInt('A',c);
}


void testUart3(void)
{
    char testString[] = "Test string";
	char rxBuffer[32];

	mp_hal_rs232_setRxBuffer( (uint8_t*) rxBuffer,32,100,0); // 100ms timeout reception

    mp_hal_rs232_write((const uint8_t * const)testString,12,10); // send data 10ms between bytes


    int flagNewPacket;
	int i=150;
	while(i>0) // wait response for 150ms
	{
		flagNewPacket = mp_hal_rs232_isNewPacketAvailable();
		if(flagNewPacket==1)
			break;
		mp_hal_delay_ms(1);
		i--;
	}
	utest_assertEqualsInt(1,flagNewPacket);


	int size = mp_hal_rs232_getPacketSize();
	utest_assertEqualsInt(12,size);

	utest_assertEqualsString(testString,rxBuffer,12);

	mp_hal_rs232_resetRxPacket();
	size = mp_hal_rs232_getPacketSize();
	utest_assertEqualsInt(0,size);
}



