/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "device.h"
#include "serial_api.h"
#include "main.h"

#define UART_TX    PA_7
#define UART_RX    PA_6

volatile char rc=0;

void uart_send_string(serial_t *sobj, char *pstr)
{
    unsigned int i=0;

    while (*(pstr+i) != 0) {
        serial_putc(sobj, *(pstr+i));
        i++;
    }
}

void uart_irq(uint32_t id, SerialIrq event)
{
    serial_t    *sobj = (void*)id;

    if(event == RxIrq) {
        rc = serial_getc(sobj);
        serial_putc(sobj, rc);
    }

    if(event == TxIrq && rc!=0){
        uart_send_string(sobj, "\r\n8195a$");
        rc = 0;
    }
}

void main(void)
{
    // sample text
    serial_t    sobj;

    // mbed uart test
    serial_init(&sobj,UART_TX,UART_RX);
    serial_baud(&sobj,38400);
    serial_format(&sobj, 8, ParityNone, 1);

    uart_send_string(&sobj, "UART IRQ API Demo...\r\n");
    uart_send_string(&sobj, "Hello World!!\n");
    uart_send_string(&sobj, "\r\n8195a$");
    serial_irq_handler(&sobj, uart_irq, (uint32_t)&sobj);
    serial_irq_set(&sobj, RxIrq, 1);
    serial_irq_set(&sobj, TxIrq, 1);

    while(1);

}


