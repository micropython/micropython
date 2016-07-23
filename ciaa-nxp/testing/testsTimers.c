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

void testTimers1(void)
{
    int32_t r = mp_hal_enableTimerAsTimer(0, 1000,1000,0); // timer0

	utest_assertEqualsInt(0,(int)r);
}


void testTimers2(void)
{
    int32_t r = mp_hal_enableTimerAsTimer(99, 1000,1000,0); // invalid timer number

    utest_assertEqualsInt(-1,(int)r);
}


// Aux functions
void testTimersSetup(void)
{
    mp_hal_disableTimer(0);
    mp_hal_enableTimerAsTimer(0, 1000,1000,0); // timer0
}

static int testTimersCounter;
void testTimersCallback(void* arg)
{
    testTimersCounter++;
}
void testTimersCallback2(void* arg)
{
    testTimersCounter = *((int*)arg);
}
//_______________



void testTimers3(void)
{
    testTimersCounter=0;
    int32_t r = mp_hal_setTimerCallback(0,testTimersCallback,(void*)0x11223344);
    utest_assertEqualsInt(0,(int)r);
    mp_hal_delay_ms(100);
    utest_assertGTInt(testTimersCounter,0);
}

void testTimers4(void)
{
    testTimersCounter=0;
    int test = 0x11223344;
    int32_t r = mp_hal_setTimerCallback(0,testTimersCallback2,&test);
    utest_assertEqualsInt(0,(int)r);
    mp_hal_delay_ms(100);
    utest_assertEqualsInt(testTimersCounter,0x11223344);
}

void testTimers5(void)
{
    int32_t r = mp_hal_setTimerCallback(99,testTimersCallback,(void*)0x11223344); // invalid timer
    utest_assertEqualsInt(-1,(int)r);
}

void testTimers6(void)
{
    testTimersCounter=0;
    mp_hal_setTimerCallback(0,testTimersCallback,(void*)0x11223344);
    mp_hal_delay_ms(100);

    int32_t r = mp_hal_disableTimer(0);
    utest_assertEqualsInt(0,(int)r);

    testTimersCounter=0;
    mp_hal_delay_ms(100);
    utest_assertEqualsInt(testTimersCounter,0);
}

void testTimers7(void)
{
    int32_t r = mp_hal_disableTimer(99); // invalid timer
    utest_assertEqualsInt(-1,(int)r);

}


void testTimers8(void)
{
    int32_t r = mp_hal_setTimerCounter(0,0);
    utest_assertEqualsInt(0,(int)r);
}

void testTimers9(void)
{
    int32_t r = mp_hal_setTimerCounter(99,0);
    utest_assertEqualsInt(-1,(int)r);
}

void testTimers10(void)
{
    int32_t r = mp_hal_setTimerPrescaler(0,1000);
    utest_assertEqualsInt(0,(int)r);
}

void testTimers11(void)
{
    int32_t r = mp_hal_setTimerPrescaler(99,1000);
    utest_assertEqualsInt(-1,(int)r);
}


void testTimers12(void)
{
    mp_hal_setTimerPrescaler(0,1000);

    int32_t r = mp_hal_getTimerPrescaler(0);
    utest_assertGTInt(1000,(int)r);
}
void testTimers13(void)
{
    int32_t r = mp_hal_getTimerPrescaler(99);
    utest_assertEqualsInt(0,(int)r);
}

void testTimers14(void)
{
    mp_hal_setTimerCounter(0,1000);
    int32_t r = mp_hal_getTimerCounter(0);
    utest_assertEqualsInt(1000,(int)r);
}
void testTimers15(void)
{
    int32_t r = mp_hal_getTimerCounter(99);
    utest_assertEqualsInt(0,(int)r);
}

void testTimers16(void)
{
    int32_t r = mp_hal_setTimerMatch(0,1200);
    utest_assertEqualsInt(0,(int)r);
}

void testTimers17(void)
{
    int32_t r = mp_hal_setTimerMatch(99,1200);
    utest_assertEqualsInt(-1,(int)r);
}

void testTimers18(void)
{
    mp_hal_setTimerMatch(0,1200);
    int32_t r = mp_hal_getTimerMatch(0);
    utest_assertEqualsInt(1200,(int)r);
}
void testTimers19(void)
{
    int32_t r = mp_hal_getTimerMatch(99);
    utest_assertEqualsInt(0,(int)r);

    mp_hal_disableTimer(0);
}


