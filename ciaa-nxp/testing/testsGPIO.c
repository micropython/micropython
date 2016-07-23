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

void testGPIO1(void)
{
	int32_t r = mp_hal_configureGPIOs(0,GPIO_MODE_INPUT, GPIO_NOPULL);
    utest_assertEqualsInt(0,(int)r);
}

void testGPIO2(void)
{
    int32_t r = mp_hal_configureGPIOs(8,GPIO_MODE_INPUT, GPIO_NOPULL);
    utest_assertEqualsInt(0,(int)r);
}

void testGPIO3(void)
{
    int32_t r = mp_hal_configureGPIOs(9,GPIO_MODE_INPUT, GPIO_NOPULL); // invalid gpio
    utest_assertEqualsInt(-1,(int)r);
}

void testGPIO4(void)
{
    int32_t r = mp_hal_configureGPIOs(0,99, GPIO_NOPULL); // invalid mode
    utest_assertEqualsInt(-1,(int)r);
}

void testGPIO5(void)
{
    int32_t r = mp_hal_configureGPIOs(0,GPIO_MODE_INPUT, 99); // invalid pullup mode
    utest_assertEqualsInt(-1,(int)r);
}

void testGPIO6(void)
{
    mp_hal_configureGPIOs(0,GPIO_MODE_INPUT, GPIO_PULLUP);
	mp_hal_delay_ms(10);
	int32_t r = mp_hal_readGPIO(0);
	utest_assertEqualsInt(1,(int)r);
}

void testGPIO7(void)
{
    mp_hal_configureGPIOs(0,GPIO_MODE_INPUT, GPIO_PULLDOWN);
	mp_hal_delay_ms(10);
    int32_t r = mp_hal_readGPIO(0);
    utest_assertEqualsInt(0,(int)r);
}

void testGPIO8(void)
{
    mp_hal_configureGPIOs(0,GPIO_MODE_INPUT, GPIO_PULLDOWN);
    int32_t r = mp_hal_readGPIO(99); // invalid gpio
    utest_assertEqualsInt(-1,(int)r);
}


//________________ Write test____________________________________
void testGPIOwriteSetup(void)
{
	mp_hal_configureGPIOs(7,GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);
	mp_hal_delay_ms(1);
	mp_hal_configureGPIOs(8,GPIO_MODE_INPUT, GPIO_NOPULL);
	mp_hal_delay_ms(1);
}

void testGPIO9(void)
{
	int32_t r = mp_hal_writeGPIO(7,0);
	utest_assertEqualsInt(0,(int)r);

	mp_hal_delay_ms(10);

    r = mp_hal_readGPIO(8);
    utest_assertEqualsInt(0,(int)r);
}

void testGPIO10(void)
{
    mp_hal_writeGPIO(7,1);
	mp_hal_delay_ms(10);
    int32_t r = mp_hal_readGPIO(8);
    utest_assertEqualsInt(1,(int)r);
}

void testGPIO11(void)
{
    int32_t r = mp_hal_writeGPIO(99,1);
    utest_assertEqualsInt(-1,(int)r);
}


//_______________________________________________________________


//________________ interrupt callback test_______________________
#define POINTER_GPIO_TEST	0x11223344
int32_t flagInt;

void testGPIOcallbackSetup(void)
{
	flagInt=0;
	mp_hal_disableIntCallbackGPIO(7);
	mp_hal_disableIntCallbackGPIO(8);
    mp_hal_configureGPIOs(7,GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);
    mp_hal_delay_ms(1);
    mp_hal_configureGPIOs(8,GPIO_MODE_INPUT, GPIO_NOPULL);
    mp_hal_delay_ms(1);
	mp_hal_writeGPIO(7,0);
	mp_hal_delay_ms(1);
}

void testGPIOcallback(void* arg)
{
	utest_assertEqualsPointer((void*)POINTER_GPIO_TEST,arg);
	flagInt=1;
}

void testGPIO12(void)
{
	int32_t r = mp_hal_enableIntCallbackGPIO(8,testGPIOcallback,(void*)POINTER_GPIO_TEST, 1, 1); // edge-high
    utest_assertEqualsInt(1,(int)r);
}

void testGPIO13(void)
{
    mp_hal_enableIntCallbackGPIO(8,testGPIOcallback,(void*)POINTER_GPIO_TEST, 1, 1); // edge-high

	// generate interrupt
	mp_hal_writeGPIO(7,1);
	mp_hal_delay_ms(10);

	utest_assertEqualsInt(1,(int)flagInt);
}

void testGPIO14(void)
{
    int32_t r = mp_hal_enableIntCallbackGPIO(99,testGPIOcallback,(void*)POINTER_GPIO_TEST, 1, 1); // invalid gpio number
    utest_assertEqualsInt(0,(int)r);
}




