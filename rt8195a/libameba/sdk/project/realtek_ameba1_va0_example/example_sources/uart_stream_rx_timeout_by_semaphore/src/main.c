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
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define UART_TX    PA_7	//PB_5
#define UART_RX    PA_6	//PB_4    

#define SRX_BUF_SZ 100
#define UART_TIMEOUT_MS   5000    //ms

#define TASK_STACK_SIZE         2048
#define TASK_PRIORITY           (tskIDLE_PRIORITY + 1)

char rx_buf[SRX_BUF_SZ]={0};
volatile uint32_t rx_bytes=0;
SemaphoreHandle_t  UartRxSema;
SemaphoreHandle_t  UartTxSema;


void uart_send_string_done(uint32_t id)
{
    serial_t    *sobj = (void*)id;
    signed portBASE_TYPE xHigherPriorityTaskWoken;
    
    xSemaphoreGiveFromISR(UartTxSema, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void uart_recv_string_done(uint32_t id)
{
    serial_t    *sobj = (void*)id;
    signed portBASE_TYPE xHigherPriorityTaskWoken;
    
    rx_bytes = sobj->rx_len;
    xSemaphoreGiveFromISR(UartRxSema, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void uart_send_string(serial_t *sobj, char *pstr)
{
    int32_t ret=0;

    xSemaphoreTake(UartTxSema, portMAX_DELAY);
    
    ret = serial_send_stream(sobj, pstr, _strlen(pstr));
    if (ret != 0) {
        DBG_8195A("%s Error(%d)\n", __FUNCTION__, ret);        
        xSemaphoreGive( UartTxSema );    
    }
}

void uart_demo(void)
{
    serial_t    sobj;
    int ret;

    serial_init(&sobj,UART_TX,UART_RX);
    serial_baud(&sobj,115200);
    serial_format(&sobj, 8, ParityNone, 1);

    serial_send_comp_handler(&sobj, (void*)uart_send_string_done, (uint32_t) &sobj);
    serial_recv_comp_handler(&sobj, (void*)uart_recv_string_done, (uint32_t) &sobj);

    // Create semaphore for UART RX done(received espected bytes or timeout)
    UartRxSema = xSemaphoreCreateBinary();

    // Create semaphore for UART TX done
    UartTxSema = xSemaphoreCreateBinary();
    xSemaphoreGive( UartTxSema );    // Ready to TX

    while (1) {
        rx_bytes = 0;
#if 1        
        ret = serial_recv_stream(&sobj, rx_buf, 10);    // Interrupt mode
#else        
        ret = serial_recv_stream_dma(&sobj, rx_buf, 10);    // DMA mode
#endif
        if( xSemaphoreTake( UartRxSema, ( TickType_t ) UART_TIMEOUT_MS/portTICK_RATE_MS ) != pdTRUE ) {
            rx_bytes = serial_recv_stream_abort(&sobj);
        }

        if (rx_bytes > 0) {
            rx_buf[rx_bytes] = 0x00; // end of string
            uart_send_string(&sobj, rx_buf);            
        }
    }
}

void main(void)
{
	// create demo Task
	if( xTaskCreate( (TaskFunction_t)uart_demo, "UART DEMO", (TASK_STACK_SIZE/4), NULL, TASK_PRIORITY, NULL) != pdPASS) {
		DBG_8195A("Cannot create demo task\n\r");
	}
	
	#if defined(CONFIG_KERNEL) && !TASK_SCHEDULER_DISABLED
	#ifdef PLATFORM_FREERTOS
	vTaskStartScheduler();
	#endif
#else
	#error !!!Need FREERTOS!!!
#endif
	
	while(1);
}


