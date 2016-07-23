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


