/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string.h>
#include "ciaanxp_mphal.h"
#include "board.h"

static int interrupt_char;

static volatile mp_uint_t tick_ct = 0;

void SysTick_Handler(void) {
	tick_ct++;
	Board_UART_tick_ms();
}


void mp_hal_init(void) {
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/1000);
        Board_Init();
    	Board_Buttons_Init();
}

// Time funcions
mp_uint_t mp_hal_get_milliseconds(void) {
    return tick_ct;
    return tick_ct;
}

void mp_hal_milli_delay(mp_uint_t ms) {
	uint32_t end = tick_ct + ms;
	while(tick_ct < end)
		__WFI();
}

// STD Functions
void mp_hal_set_interrupt_char(int c) {
    interrupt_char = c;
}
int mp_hal_stdin_rx_chr(void) {
   for (;;) {
        int r = Board_UARTGetChar();
        if (r!= EOF) {
            return r;
        }
    }
}
void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    for (; len > 0; --len) {
    	Board_UARTPutChar(*str++);
    }
}
void mp_hal_stdout_tx_strn_cooked(const char *str, mp_uint_t len) {
    for (; len > 0; --len) {
        if (*str == '\n') {
        	Board_UARTPutChar('\r');
        }
        Board_UARTPutChar(*str++);
    }
}


// RS232 Functions
uint32_t mp_hal_rs232_write(uint8_t const * const buffer, uint32_t size,uint32_t delay)
{
	if(delay==0)
		return Board_UART_Write(LPC_USART3,buffer,size);

	uint32_t i=0;
	while(size>0)
	{
		Board_UART_Write(LPC_USART3,&(buffer[i]),1);
		mp_hal_milli_delay(delay);
		size--;
		i++;
	}
	return i;
}

void mp_hal_rs232_setRxBuffer(uint8_t* pBuffer,uint32_t size,uint32_t timeout, uint8_t finalByte)
{
	Board_UART_setRxBuffer(LPC_USART3,pBuffer,size,timeout,finalByte);
}

void mp_hal_rs232_setConfig(int32_t baud,int32_t stopBits,int32_t parity)
{
	Board_UART_setConfig(LPC_USART3,baud,stopBits,parity);
}

int32_t mp_hal_rs232_isNewPacketAvailable(void)
{
	return (int32_t)Board_UART_isNewPacket(LPC_USART3);
}

uint32_t mp_hal_rs232_getPacketSize(void)
{
	return Board_UART_getRxSize(LPC_USART3);
}

int32_t mp_hal_rs232_getChar(void)
{
	return  Board_UART_getChar(LPC_USART3);
}

int32_t mp_hal_rs232_charAvailable(void)
{
	return Board_UART_charAvailable(LPC_USART3);
}

void mp_hal_rs232_resetRxPacket(void)
{
	Board_UART_resetRx(LPC_USART3);
}

// RS485 Functions
uint32_t mp_hal_rs485_write(uint8_t const * const buffer, uint32_t size, uint32_t delay)
{
        if(delay==0)
                return Board_UART_Write(LPC_USART0,buffer,size);

        uint32_t i=0;
        while(size>0)
        {
                Board_UART_Write(LPC_USART0,&(buffer[i]),1);
                mp_hal_milli_delay(delay);
                size--;
                i++;
        }
        return i;
}

void mp_hal_rs485_setRxBuffer(uint8_t* pBuffer,uint32_t size,uint32_t timeout, uint8_t finalByte)
{
        Board_UART_setRxBuffer(LPC_USART0,pBuffer,size,timeout,finalByte);
}

void mp_hal_rs485_setConfig(int32_t baud,int32_t stopBits,int32_t parity)
{
        Board_UART_setConfig(LPC_USART0,baud,stopBits,parity);
}

int32_t mp_hal_rs485_isNewPacketAvailable(void)
{
        return (int32_t)Board_UART_isNewPacket(LPC_USART0);
}

uint32_t mp_hal_rs485_getPacketSize(void)
{
        return Board_UART_getRxSize(LPC_USART0);
}

int32_t mp_hal_rs485_getChar(void)
{
        return  Board_UART_getChar(LPC_USART0);
}

int32_t mp_hal_rs485_charAvailable(void)
{
        return Board_UART_charAvailable(LPC_USART0);
}

void mp_hal_rs485_resetRxPacket(void)
{
        Board_UART_resetRx(LPC_USART0);
}

