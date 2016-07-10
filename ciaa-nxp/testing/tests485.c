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

void test485UartSetup(void)
{
	mp_hal_rs485_setConfig(9600,1,0);

    // clean buffer
    mp_hal_delay_ms(250);
    while(mp_hal_rs485_getChar()!=-1)
        mp_hal_delay_ms(2);
    mp_hal_rs485_resetRxPacket();

    mp_hal_delay_ms(250);
}

void test485Uart1(void)
{
	char testString[] = "Test string";

	int r = mp_hal_rs485_write((const uint8_t * const)testString,12,0);

	utest_assertEqualsInt(12,r);
}

void test485Uart2(void)
{
    char testString[] = "A";

    mp_hal_rs485_write((const uint8_t * const)testString,1,0);
	mp_hal_delay_ms(150);


	int flagChar = mp_hal_rs485_charAvailable();
	utest_assertEqualsInt(1,flagChar);


	int c = mp_hal_rs485_getChar();
	utest_assertEqualsInt('A',c);
}


void test485Uart3(void)
{
    char testString[] = "Test string";
	char rxBuffer[32];

	mp_hal_rs485_setRxBuffer( (uint8_t*) rxBuffer,32,100,0); // 100ms timeout reception

    mp_hal_rs485_write((const uint8_t * const)testString,12,0);


    int flagNewPacket;
	int i=1000;
	while(i>0) // wait response for 400ms
	{
		flagNewPacket = mp_hal_rs485_isNewPacketAvailable();
		if(flagNewPacket==1)
			break;
		mp_hal_delay_ms(1);
		i--;
	}
	utest_assertEqualsInt(1,flagNewPacket);


	int size = mp_hal_rs485_getPacketSize();
	utest_assertEqualsInt(12,size);

	utest_assertEqualsString(testString,rxBuffer,12);

	mp_hal_rs485_resetRxPacket();
	size = mp_hal_rs232_getPacketSize();
	utest_assertEqualsInt(0,size);
}



