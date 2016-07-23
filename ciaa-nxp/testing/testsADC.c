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

