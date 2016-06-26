/*
	utest : Unit test library for embeded C programs.
    Copyright (C) <2016>  <Ernesto Gigliotti>

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

#include "utest.h"

static char utest_printBuffer[256];
static int utest_flagTestError;
static int utest_lineTestError;
static char* utest_fileTestError;
static int utest_totalTestsCounter;
static int utest_okTestsCounter;
static int utest_varAux;

void utest_init(void)
{
	utest_totalTestsCounter=0;
	utest_okTestsCounter=0;
}

void utest_printStatistics(void)
{
	utest_print("Unit Tests Statistics\r\n");
	if(utest_totalTestsCounter>0)
	{
		int per = (int)(((float)utest_okTestsCounter/(float)utest_totalTestsCounter)*100.0);
		utest_print1("Tests OK : %d %%\r\n",per);
		utest_print2("Tests: %d. Failed Tests: %d\r\n",utest_totalTestsCounter,(utest_totalTestsCounter-utest_okTestsCounter));
	}
	else
		utest_print("No tests executed.\r\n");

}

void utest_startTest(void(*fncTest)(void),void(*fncBefore)(void),char* testName)
{
	if(fncTest!=0)
	{
		utest_flagTestError=0;
		utest_print1("TESTING:%s\r\n",testName);
		if(fncBefore!=0)
			fncBefore();
		utest_totalTestsCounter++;
		fncTest();
		if(utest_flagTestError==1)
		{
			utest_print2("TEST FAILED. FILE:%s LINE:%d\r\n",utest_fileTestError,utest_lineTestError);
		}
		else
		{
			//utest_print("TEST OK\r\n");
			utest_okTestsCounter++;
		}
		//utest_print("________________________________________\r\n");
	}
}



