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
#include "serial_ex_api.h"
//#include "main.h"

#define UART_TX    PA_7	//PB_5
#define UART_RX    PA_6 //PB_4    

#define SRX_BUF_SZ 100

char rx_buf[SRX_BUF_SZ]={0};
volatile uint32_t tx_busy=0;
volatile uint32_t rx_done=0;

void uart_send_string_done(uint32_t id)
{
    serial_t    *sobj = (void*)id;
    tx_busy = 0;
}

void uart_recv_string_done(uint32_t id)
{
    serial_t    *sobj = (void*)id;
    rx_done = 1;
}

void uart_send_string(serial_t *sobj, char *pstr)
{
    int32_t ret=0;

    if (tx_busy) {
        return;
    }
    
    tx_busy = 1;
    ret = serial_send_stream_dma(sobj, pstr, _strlen(pstr));
    if (ret != 0) {
        DBG_8195A("%s Error(%d)\n", __FUNCTION__, ret);        
        tx_busy = 0;
    }
}

void main(void)
{
    serial_t    sobj;
    int ret;
    int i=0;
    int len;

    serial_init(&sobj,UART_TX,UART_RX);
    serial_baud(&sobj,38400);
    serial_format(&sobj, 8, ParityNone, 1);

    serial_send_comp_handler(&sobj, (void*)uart_send_string_done, (uint32_t) &sobj);
    serial_recv_comp_handler(&sobj, (void*)uart_recv_string_done, (uint32_t) &sobj);

    DBG_8195A("receive 13 bytes\r\n", rx_buf);

    ret = serial_recv_stream_dma(&sobj, rx_buf, 13);
    if (ret) {
        DBG_8195A(" %s: Recv Error(%d)\n", __FUNCTION__, ret);        
        rx_done = 1;
    }

    while (1) {
        if (rx_done) {
            uart_send_string(&sobj, rx_buf);            
            rx_done = 0;
            
            len = (i+4) & 0x0f;
            i++;
            /* Wait for inputing x character to initiate DMA. 
               8 for this example*/
            DBG_8195A("rx_len=%d\r\n", len);
            ret = serial_recv_stream_dma(&sobj, rx_buf, len);
            rx_buf[len] = 0;    // end of string
            if (ret) {
                DBG_8195A(" %s: Recv Error(%d)\n", __FUNCTION__, ret);        
                rx_done = 1;
            }
        }
    }

}


