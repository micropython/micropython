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
	Board_LED_Set(0, 1); // On
	utest_assertEqualsInt(1,Board_LED_Test(0));

	// Led 1
	Board_LED_Set(1, 0); // Off
	utest_assertEqualsInt(0,Board_LED_Test(1));

	// Led 2
	Board_LED_Set(2, 1); // On
	utest_assertEqualsInt(1,Board_LED_Test(2));
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
    Board_LED_Set(0, 1); // On
    Board_LED_Set(1, 0); // Off
    Board_LED_Set(2, 0); // Off

	Board_LED_Toggle(0);
	utest_assertEqualsInt(0,Board_LED_Test(0));
	utest_assertEqualsInt(0,Board_LED_Test(1));
	utest_assertEqualsInt(0,Board_LED_Test(2));

    Board_LED_Toggle(1);
    utest_assertEqualsInt(0,Board_LED_Test(0));
    utest_assertEqualsInt(1,Board_LED_Test(1));
    utest_assertEqualsInt(0,Board_LED_Test(2));

    Board_LED_Toggle(2);
    utest_assertEqualsInt(0,Board_LED_Test(0));
    utest_assertEqualsInt(1,Board_LED_Test(1));
    utest_assertEqualsInt(1,Board_LED_Test(2));
}


