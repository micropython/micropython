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

void uart_send_string(serial_t *sobj, char *pstr)
{
    unsigned int i=0;

    while (*(pstr+i) != 0) {
        serial_putc(sobj, *(pstr+i));
        i++;
    }
}

void main(void)
{
    // sample text
    char rc;
    serial_t    sobj;

    // mbed uart test
    serial_init(&sobj,UART_TX,UART_RX);
    serial_baud(&sobj,38400);
    serial_format(&sobj, 8, ParityNone, 1);

    uart_send_string(&sobj, "UART API Demo...\r\n");
    uart_send_string(&sobj, "Hello World!!\r\n");
    while(1){
        uart_send_string(&sobj, "\r\n8195a$");
        rc = serial_getc(&sobj);
        serial_putc(&sobj, rc);
    }
}

