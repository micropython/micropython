#ifndef __UART_SOCKET_H_
#define __UART_SOCKET_H_

#include "osdep_api.h"
#include "serial_api.h"
#include "serial_ex_api.h"

#define UART_SEND_BUFFER_LEN	256
#define UART_RECV_BUFFER_LEN	1024
#define UART_MAX_DELAY_TIME   20

typedef struct _uart_set_str 
{ 
    char UartName[8];    // the name of uart 
    int BaudRate;    //The baud rate 
    char number;     //The number of data bits 
    char parity;     //The parity(default NONE) 
    char StopBits;      //The number of stop bits 
    char FlowControl;    //support flow control is 1 
}uart_set_str;

typedef struct _uart_socket_t
{
	serial_t sobj;
	int fd;

	/* Used for UART RX */
	u32 rx_start;
	//u32 rx_bytes;
	u32 prxread;
	u32 prxwrite;
	u32 rxoverlap;
	u32 last_update;  //tick count when rx byte
	u8 recv_buf[UART_RECV_BUFFER_LEN];

	u32 tx_start;
	u32 tx_bytes;
	u8 send_buf[UART_SEND_BUFFER_LEN];
	_Sema tx_sema;
	_Sema dma_tx_sema;	

	_Sema action_sema;
}uart_socket_t;

uart_socket_t* uart_open(uart_set_str *puartpara);
int uart_close(uart_socket_t *u);
int uart_read(uart_socket_t *u, void *read_buf, size_t size);
int uart_write(uart_socket_t *u, void *pbuf, size_t size);

#endif //__UART_SOCKET_H_
