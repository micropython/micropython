void testUartSetup(void)
{
	mp_hal_rs232_setConfig(9600,1,0);
}

void testUart1(void)
{
	char testString[] = "Test string";

	int r = mp_hal_rs232_write((const uint8_t * const)testString,12,0);

	utest_assertEqualsInt(12,r);
}

void testUart2(void)
{
    char testString[] = "Test string";
	char rxBuffer[32];

	mp_hal_rs232_setRxBuffer( (uint8_t*) rxBuffer,32,100,0); // 100ms timeout reception

    mp_hal_rs232_write((const uint8_t * const)testString,12,0); // send data


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

	//debug
	utest_print(rxBuffer);
	//_____

	int size = mp_hal_rs232_getPacketSize();
	utest_assertEqualsInt(12,size);


	mp_hal_rs232_resetRxPacket();
	size = mp_hal_rs232_getPacketSize();
	utest_assertEqualsInt(0,size);
}



