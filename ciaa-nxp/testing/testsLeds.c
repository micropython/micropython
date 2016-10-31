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

void testLeds1(void)
{
	// Led 0
	mp_hal_setLed(0, 1); // On
	utest_assertEqualsInt(1,mp_hal_testLed(0));

	// Led 1
	mp_hal_setLed(1, 0); // Off
	utest_assertEqualsInt(0,mp_hal_testLed(1));

	// Led 2
	mp_hal_setLed(2, 1); // On
	utest_assertEqualsInt(1,mp_hal_testLed(2));
}

void testLeds2(void)
{
	// Led 3
	mp_hal_setPwmRGBValue(0,8); // Value : 8
	utest_assertEqualsInt(8,mp_hal_getPwmRGBValue(0));

	// Led 4
    mp_hal_setPwmRGBValue(1,5); // Value : 5
    utest_assertEqualsInt(5,mp_hal_getPwmRGBValue(1));

	// Led 5
    mp_hal_setPwmRGBValue(2,9); // Value : 9
    utest_assertEqualsInt(9,mp_hal_getPwmRGBValue(2));
}

void testLeds3(void)
{
    mp_hal_setLed(0, 1); // On
    mp_hal_setLed(1, 0); // Off
    mp_hal_setLed(2, 0); // Off

	mp_hal_toggleLed(0);
	utest_assertEqualsInt(0,mp_hal_testLed(0));
	utest_assertEqualsInt(0,mp_hal_testLed(1));
	utest_assertEqualsInt(0,mp_hal_testLed(2));

    mp_hal_toggleLed(1);
    utest_assertEqualsInt(0,mp_hal_testLed(0));
    utest_assertEqualsInt(1,mp_hal_testLed(1));
    utest_assertEqualsInt(0,mp_hal_testLed(2));

    mp_hal_toggleLed(2);
    utest_assertEqualsInt(0,mp_hal_testLed(0));
    utest_assertEqualsInt(1,mp_hal_testLed(1));
    utest_assertEqualsInt(1,mp_hal_testLed(2));
}


