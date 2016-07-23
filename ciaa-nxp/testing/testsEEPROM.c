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

void testEEPROM1(void)
{
	const uint8_t values[] = {0x55,0xC3,0x3C,0xFF,0x11};
	int32_t r;

	r = mp_hal_writeByteEEPROM(0x0000,values[0]);
	utest_assertEqualsInt(0,(int)r);
	r = mp_hal_readByteEEPROM(0x0000);
	utest_assertEqualsInt((int)values[0],(int)r);


	r = mp_hal_writeByteEEPROM(0x1000,values[1]);
    utest_assertEqualsInt(0,(int)r);
    r = mp_hal_readByteEEPROM(0x1000);
    utest_assertEqualsInt((int)values[1],(int)r);


	r = mp_hal_writeByteEEPROM(0x2000,values[2]);
    utest_assertEqualsInt(0,(int)r);
    r = mp_hal_readByteEEPROM(0x2000);
    utest_assertEqualsInt((int)values[2],(int)r);


	r = mp_hal_writeByteEEPROM(0x3000,values[3]);
    utest_assertEqualsInt(0,(int)r);
    r = mp_hal_readByteEEPROM(0x3000);
    utest_assertEqualsInt((int)values[3],(int)r);


	r = mp_hal_writeByteEEPROM(0x3FFF,values[4]);
    utest_assertEqualsInt(0,(int)r);
    r = mp_hal_readByteEEPROM(0x3FFF);
    utest_assertEqualsInt((int)values[4],(int)r);
}

void testEEPROM2(void)
{
	int32_t r = mp_hal_writeByteEEPROM(0x8000,0x55); // invalid address
	utest_assertEqualsInt(-1,(int)r);
}

void testEEPROM3(void)
{
    int32_t r = mp_hal_readByteEEPROM(0x8000); // invalid address
    utest_assertEqualsInt(-1,(int)r);
}



