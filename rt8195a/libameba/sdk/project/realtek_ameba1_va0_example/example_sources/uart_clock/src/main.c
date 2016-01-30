/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "device.h"
#include "serial_api.h"
#include "serial_ex_api.h"
#include "main.h"

#define UART_TX    PA_7    
#define UART_RX    PA_6    

/* 100 bytes data, 500 clocks, provide buadrate/2 frequency */
#define SREAM_LEN 128
char sim_clock[SREAM_LEN+1];


volatile uint32_t is_stop = 0;
static serial_t    sobj_clk;

void uart_clock_send_string(serial_t *sobj, char *pstr)
{
    int32_t ret=0;

    ret = serial_send_stream_dma(sobj, pstr, _strlen(pstr));
    if (ret != 0) {
        DBG_8195A("%s Error(%d)\n", __FUNCTION__, ret);        
    }
}

void uart_clock_send_string_done(uint32_t id)
{
    serial_t    *sobj = (void*)id;
	if(!is_stop)
		uart_clock_send_string(sobj, sim_clock);
}

void uart_clock_deinit(void)
{
	is_stop = 1;
	serial_free(&sobj_clk);
}

void uart_clock_init(int rate)
{
    //serial_t    sobj;
    int ret;
    int i;

    for (i=0;i<SREAM_LEN;i++) {
        sim_clock[i] = 0x55;
    }
    sim_clock[i] = 0;
    
    serial_init(&sobj_clk,UART_TX,UART_RX);
	serial_baud(&sobj_clk, rate*2);
    
    serial_format(&sobj_clk, 8, ParityNone, 0);

    serial_send_comp_handler(&sobj_clk, (void*)uart_clock_send_string_done, (uint32_t) &sobj_clk);
}

void uart_clock_on(void)
{
	is_stop = 0;
	uart_clock_send_string(&sobj_clk, sim_clock);
}

void uart_clock_off(void)
{
	is_stop = 1;
    serial_send_stream_abort(&sobj_clk);
}

void main(void)
{
	// only support 33kHz, 36kHz, 36.7kHz 38kHz, 40kHz, 56kHz
	uart_clock_init(38000);
	
	while(1) {
        uart_clock_on();
        wait_ms(5000);
        uart_clock_off();
        wait_ms(5000);        
    }

}


