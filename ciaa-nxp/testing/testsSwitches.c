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

void testSwitches1(void)
{
	utest_assertEqualsInt(0,Buttons_GetStatusByNumber(0)); // sw1 released
	utest_assertEqualsInt(0,Buttons_GetStatusByNumber(1)); // sw2 released
	utest_assertEqualsInt(0,Buttons_GetStatusByNumber(2)); // sw3 released
	utest_assertEqualsInt(0,Buttons_GetStatusByNumber(3)); // sw4 released

	utest_assertEqualsInt(-1,Buttons_GetStatusByNumber(999)); // invalid sw returns -1
}






