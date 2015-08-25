/*
 * File	: uart.c
 * Copyright (C) 2013 - 2016, Espressif Systems
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of version 3 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "ets_sys.h"
#include "osapi.h"
#include "driver/uart.h"
#include "osapi.h"
#include "driver/uart_register.h"
#include "mem.h"
#include "os_type.h"

// UartDev is defined and initialized in rom code.
extern UartDevice    UartDev;

LOCAL struct UartBuffer* pTxBuffer = NULL;
LOCAL struct UartBuffer* pRxBuffer = NULL;

/*uart demo with a system task, to output what uart receives*/
/*this is a example to process uart data from task,please change the priority to fit your application task if exists*/
/*it might conflict with your task, if so,please arrange the priority of different task,  or combine it to a different event in the same task. */
#define uart_recvTaskPrio        0
#define uart_recvTaskQueueLen    10
os_event_t    uart_recvTaskQueue[uart_recvTaskQueueLen];

#define DBG  
#define DBG1 uart1_sendStr_no_wait
#define DBG2 os_printf


LOCAL void uart0_rx_intr_handler(void *para);

/******************************************************************************
 * FunctionName : uart_config
 * Description  : Internal used function
 *                UART0 used for data TX/RX, RX buffer size is 0x100, interrupt enabled
 *                UART1 just used for debug output
 * Parameters   : uart_no, use UART0 or UART1 defined ahead
 * Returns      : NONE
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR
uart_config(uint8 uart_no)
{
    if (uart_no == UART1){
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_U1TXD_BK);
    }else{
        /* rcv_buff size if 0x100 */
        ETS_UART_INTR_ATTACH(uart0_rx_intr_handler,  &(UartDev.rcv_buff));
        PIN_PULLUP_DIS(PERIPHS_IO_MUX_U0TXD_U);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD);
	#if UART_HW_RTS
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_U0RTS);   //HW FLOW CONTROL RTS PIN
        #endif
	#if UART_HW_CTS
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_U0CTS);   //HW FLOW CONTROL CTS PIN
        #endif
    }
    uart_div_modify(uart_no, UART_CLK_FREQ / (UartDev.baut_rate));//SET BAUDRATE
    
    WRITE_PERI_REG(UART_CONF0(uart_no), ((UartDev.exist_parity & UART_PARITY_EN_M)  <<  UART_PARITY_EN_S) //SET BIT AND PARITY MODE
                                                                        | ((UartDev.parity & UART_PARITY_M)  <<UART_PARITY_S )
                                                                        | ((UartDev.stop_bits & UART_STOP_BIT_NUM) << UART_STOP_BIT_NUM_S)
                                                                        | ((UartDev.data_bits & UART_BIT_NUM) << UART_BIT_NUM_S));
    
    //clear rx and tx fifo,not ready
    SET_PERI_REG_MASK(UART_CONF0(uart_no), UART_RXFIFO_RST | UART_TXFIFO_RST);    //RESET FIFO
    CLEAR_PERI_REG_MASK(UART_CONF0(uart_no), UART_RXFIFO_RST | UART_TXFIFO_RST);
    
    if (uart_no == UART0){
        //set rx fifo trigger
        WRITE_PERI_REG(UART_CONF1(uart_no),
        ((100 & UART_RXFIFO_FULL_THRHD) << UART_RXFIFO_FULL_THRHD_S) |
        #if UART_HW_RTS
        ((110 & UART_RX_FLOW_THRHD) << UART_RX_FLOW_THRHD_S) |
        UART_RX_FLOW_EN |   //enbale rx flow control
        #endif
        (0x02 & UART_RX_TOUT_THRHD) << UART_RX_TOUT_THRHD_S |
        UART_RX_TOUT_EN|
        ((0x10 & UART_TXFIFO_EMPTY_THRHD)<<UART_TXFIFO_EMPTY_THRHD_S));//wjl 
        #if UART_HW_CTS
        SET_PERI_REG_MASK( UART_CONF0(uart_no),UART_TX_FLOW_EN);  //add this sentense to add a tx flow control via MTCK( CTS )
        #endif
        SET_PERI_REG_MASK(UART_INT_ENA(uart_no), UART_RXFIFO_TOUT_INT_ENA |UART_FRM_ERR_INT_ENA);
    }else{
        WRITE_PERI_REG(UART_CONF1(uart_no),((UartDev.rcv_buff.TrigLvl & UART_RXFIFO_FULL_THRHD) << UART_RXFIFO_FULL_THRHD_S));//TrigLvl default val == 1
    }
    //clear all interrupt
    WRITE_PERI_REG(UART_INT_CLR(uart_no), 0xffff);
    //enable rx_interrupt
    SET_PERI_REG_MASK(UART_INT_ENA(uart_no), UART_RXFIFO_FULL_INT_ENA|UART_RXFIFO_OVF_INT_ENA);
}

/******************************************************************************
 * FunctionName : uart1_tx_one_char
 * Description  : Internal used function
 *                Use uart1 interface to transfer one char
 * Parameters   : uint8 TxChar - character to tx
 * Returns      : OK
*******************************************************************************/
 STATUS uart_tx_one_char(uint8 uart, uint8 TxChar)
{
    while (true){
        uint32 fifo_cnt = READ_PERI_REG(UART_STATUS(uart)) & (UART_TXFIFO_CNT<<UART_TXFIFO_CNT_S);
        if ((fifo_cnt >> UART_TXFIFO_CNT_S & UART_TXFIFO_CNT) < 126) {
            break;
        }
    }
    WRITE_PERI_REG(UART_FIFO(uart) , TxChar);
    return OK;
}

/******************************************************************************
 * FunctionName : uart1_write_char
 * Description  : Internal used function
 *                Do some special deal while tx char is '\r' or '\n'
 * Parameters   : char c - character to tx
 * Returns      : NONE
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR
uart1_write_char(char c)
{
    if (c == '\n'){
        uart_tx_one_char(UART1, '\r');
        uart_tx_one_char(UART1, '\n');
    }else if (c == '\r'){
    
    }else{
        uart_tx_one_char(UART1, c);
    }
}

//os_printf output to fifo or to the tx buffer
LOCAL void ICACHE_FLASH_ATTR
uart0_write_char_no_wait(char c)
{
#if UART_BUFF_EN    //send to uart0 fifo but do not wait 
    uint8 chr;
    if (c == '\n'){
        chr = '\r';
        tx_buff_enq(&chr, 1);
        chr = '\n';
        tx_buff_enq(&chr, 1);
    }else if (c == '\r'){
    
    }else{
        tx_buff_enq(&c,1);
    }
#else //send to uart tx buffer
    if (c == '\n'){
        uart_tx_one_char_no_wait(UART0, '\r');
        uart_tx_one_char_no_wait(UART0, '\n');
    }else if (c == '\r'){
    
    }
    else{
        uart_tx_one_char_no_wait(UART0, c);
    }
#endif
}

/******************************************************************************
 * FunctionName : uart0_tx_buffer
 * Description  : use uart0 to transfer buffer
 * Parameters   : uint8 *buf - point to send buffer
 *                uint16 len - buffer len
 * Returns      :
*******************************************************************************/
void ICACHE_FLASH_ATTR
uart0_tx_buffer(uint8 *buf, uint16 len)
{
    uint16 i;
    for (i = 0; i < len; i++)
    {
        uart_tx_one_char(UART0, buf[i]);
    }
}

/******************************************************************************
 * FunctionName : uart0_sendStr
 * Description  : use uart0 to transfer buffer
 * Parameters   : uint8 *buf - point to send buffer
 *                uint16 len - buffer len
 * Returns      :
*******************************************************************************/
void ICACHE_FLASH_ATTR
uart0_sendStr(const char *str)
{
    while(*str){
        uart_tx_one_char(UART0, *str++);
    }
}
void at_port_print(const char *str) __attribute__((alias("uart0_sendStr")));
/******************************************************************************
 * FunctionName : uart0_rx_intr_handler
 * Description  : Internal used function
 *                UART0 interrupt handler, add self handle code inside
 * Parameters   : void *para - point to ETS_UART_INTR_ATTACH's arg
 * Returns      : NONE
*******************************************************************************/
LOCAL void
uart0_rx_intr_handler(void *para)
{
    /* uart0 and uart1 intr combine togther, when interrupt occur, see reg 0x3ff20020, bit2, bit0 represents
    * uart1 and uart0 respectively
    */
    uint8 RcvChar;
    uint8 uart_no = UART0;//UartDev.buff_uart_no;
    uint8 fifo_len = 0;
    uint8 buf_idx = 0;
    uint8 temp,cnt;
    //RcvMsgBuff *pRxBuff = (RcvMsgBuff *)para;
    
    	/*ATTENTION:*/
	/*IN NON-OS VERSION SDK, DO NOT USE "ICACHE_FLASH_ATTR" FUNCTIONS IN THE WHOLE HANDLER PROCESS*/
	/*ALL THE FUNCTIONS CALLED IN INTERRUPT HANDLER MUST BE DECLARED IN RAM */
	/*IF NOT , POST AN EVENT AND PROCESS IN SYSTEM TASK */
    if(UART_FRM_ERR_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_FRM_ERR_INT_ST)){
        DBG1("FRM_ERR\r\n");
        WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_FRM_ERR_INT_CLR);
    }else if(UART_RXFIFO_FULL_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_RXFIFO_FULL_INT_ST)){
        DBG("f");
        uart_rx_intr_disable(UART0);
        WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR);
        system_os_post(uart_recvTaskPrio, 0, 0);
    }else if(UART_RXFIFO_TOUT_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_RXFIFO_TOUT_INT_ST)){
        DBG("t");
        uart_rx_intr_disable(UART0);
        WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_TOUT_INT_CLR);
        system_os_post(uart_recvTaskPrio, 0, 0);
    }else if(UART_TXFIFO_EMPTY_INT_ST == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_TXFIFO_EMPTY_INT_ST)){
        DBG("e");
	/* to output uart data from uart buffer directly in empty interrupt handler*/
	/*instead of processing in system event, in order not to wait for current task/function to quit */
	/*ATTENTION:*/
	/*IN NON-OS VERSION SDK, DO NOT USE "ICACHE_FLASH_ATTR" FUNCTIONS IN THE WHOLE HANDLER PROCESS*/
	/*ALL THE FUNCTIONS CALLED IN INTERRUPT HANDLER MUST BE DECLARED IN RAM */
	CLEAR_PERI_REG_MASK(UART_INT_ENA(UART0), UART_TXFIFO_EMPTY_INT_ENA);
	#if UART_BUFF_EN
		tx_start_uart_buffer(UART0);
	#endif
        //system_os_post(uart_recvTaskPrio, 1, 0);
        WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_TXFIFO_EMPTY_INT_CLR);
        
    }else if(UART_RXFIFO_OVF_INT_ST  == (READ_PERI_REG(UART_INT_ST(uart_no)) & UART_RXFIFO_OVF_INT_ST)){
        WRITE_PERI_REG(UART_INT_CLR(uart_no), UART_RXFIFO_OVF_INT_CLR);
        DBG1("RX OVF!!\r\n");
    }

}

/******************************************************************************
 * FunctionName : uart_init
 * Description  : user interface for init uart
 * Parameters   : UartBautRate uart0_br - uart0 bautrate
 *                UartBautRate uart1_br - uart1 bautrate
 * Returns      : NONE
*******************************************************************************/
#if UART_SELFTEST&UART_BUFF_EN
os_timer_t buff_timer_t;
void ICACHE_FLASH_ATTR
uart_test_rx()
{
    uint8 uart_buf[128]={0};
    uint16 len = 0;
    len = rx_buff_deq(uart_buf, 128 );
    tx_buff_enq(uart_buf,len);
}
#endif

LOCAL void ICACHE_FLASH_ATTR ///////
uart_recvTask(os_event_t *events)
{
    if(events->sig == 0){
    #if  UART_BUFF_EN  
        Uart_rx_buff_enq();
    #else
        uint8 fifo_len = (READ_PERI_REG(UART_STATUS(UART0))>>UART_RXFIFO_CNT_S)&UART_RXFIFO_CNT;
        uint8 d_tmp = 0;
        uint8 idx=0;
        for(idx=0;idx<fifo_len;idx++) {
            d_tmp = READ_PERI_REG(UART_FIFO(UART0)) & 0xFF;
            uart_tx_one_char(UART0, d_tmp);
        }
        WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR|UART_RXFIFO_TOUT_INT_CLR);
        uart_rx_intr_enable(UART0);
    #endif
    }else if(events->sig == 1){
    #if UART_BUFF_EN
	 //already move uart buffer output to uart empty interrupt
        //tx_start_uart_buffer(UART0);
    #else 
    
    #endif
    }
}

void ICACHE_FLASH_ATTR
uart_init(UartBautRate uart0_br, UartBautRate uart1_br)
{
    /*this is a example to process uart data from task,please change the priority to fit your application task if exists*/
    system_os_task(uart_recvTask, uart_recvTaskPrio, uart_recvTaskQueue, uart_recvTaskQueueLen);  //demo with a task to process the uart data
    
    UartDev.baut_rate = uart0_br;
    uart_config(UART0);
    UartDev.baut_rate = uart1_br;
    uart_config(UART1);
    ETS_UART_INTR_ENABLE();
    
    #if UART_BUFF_EN
    pTxBuffer = Uart_Buf_Init(UART_TX_BUFFER_SIZE);
    pRxBuffer = Uart_Buf_Init(UART_RX_BUFFER_SIZE);
    #endif


    /*option 1: use default print, output from uart0 , will wait some time if fifo is full */
    //do nothing...

    /*option 2: output from uart1,uart1 output will not wait , just for output debug info */
    /*os_printf output uart data via uart1(GPIO2)*/
    //os_install_putc1((void *)uart1_write_char);    //use this one to output debug information via uart1 //

    /*option 3: output from uart0 will skip current byte if fifo is full now... */
    /*see uart0_write_char_no_wait:you can output via a buffer or output directly */
    /*os_printf output uart data via uart0 or uart buffer*/
    //os_install_putc1((void *)uart0_write_char_no_wait);  //use this to print via uart0
    
    #if UART_SELFTEST&UART_BUFF_EN
    os_timer_disarm(&buff_timer_t);
    os_timer_setfn(&buff_timer_t, uart_test_rx , NULL);   //a demo to process the data in uart rx buffer
    os_timer_arm(&buff_timer_t,10,1);
    #endif
}

void ICACHE_FLASH_ATTR
uart_reattach()
{
    uart_init(BIT_RATE_115200, BIT_RATE_115200);
}

/******************************************************************************
 * FunctionName : uart_tx_one_char_no_wait
 * Description  : uart tx a single char without waiting for fifo 
 * Parameters   : uint8 uart - uart port
 *                uint8 TxChar - char to tx
 * Returns      : STATUS
*******************************************************************************/
STATUS uart_tx_one_char_no_wait(uint8 uart, uint8 TxChar)
{
    uint8 fifo_cnt = (( READ_PERI_REG(UART_STATUS(uart))>>UART_TXFIFO_CNT_S)& UART_TXFIFO_CNT);
    if (fifo_cnt < 126) {
        WRITE_PERI_REG(UART_FIFO(uart) , TxChar);
    }
    return OK;
}

STATUS uart0_tx_one_char_no_wait(uint8 TxChar)
{
    uint8 fifo_cnt = (( READ_PERI_REG(UART_STATUS(UART0))>>UART_TXFIFO_CNT_S)& UART_TXFIFO_CNT);
    if (fifo_cnt < 126) {
        WRITE_PERI_REG(UART_FIFO(UART0) , TxChar);
    }
    return OK;
}


/******************************************************************************
 * FunctionName : uart1_sendStr_no_wait
 * Description  : uart tx a string without waiting for every char, used for print debug info which can be lost
 * Parameters   : const char *str - string to be sent
 * Returns      : NONE
*******************************************************************************/
void uart1_sendStr_no_wait(const char *str)
{
    while(*str){
        uart_tx_one_char_no_wait(UART1, *str++);
    }
}


#if UART_BUFF_EN
/******************************************************************************
 * FunctionName : Uart_Buf_Init
 * Description  : tx buffer enqueue: fill a first linked buffer 
 * Parameters   : char *pdata - data point  to be enqueue
 * Returns      : NONE
*******************************************************************************/
struct UartBuffer* ICACHE_FLASH_ATTR
Uart_Buf_Init(uint32 buf_size)
{
    uint32 heap_size = system_get_free_heap_size();
    if(heap_size <=buf_size){
        DBG1("no buf for uart\n\r");
        return NULL;
    }else{
        DBG("test heap size: %d\n\r",heap_size);
        struct UartBuffer* pBuff = (struct UartBuffer* )os_malloc(sizeof(struct UartBuffer));
        pBuff->UartBuffSize = buf_size;
        pBuff->pUartBuff = (uint8*)os_malloc(pBuff->UartBuffSize);
        pBuff->pInPos = pBuff->pUartBuff;
        pBuff->pOutPos = pBuff->pUartBuff;
        pBuff->Space = pBuff->UartBuffSize;
        pBuff->BuffState = OK;
        pBuff->nextBuff = NULL;
        pBuff->TcpControl = RUN;
        return pBuff;
    }
}


//copy uart buffer
LOCAL void Uart_Buf_Cpy(struct UartBuffer* pCur, char* pdata , uint16 data_len)
{
    if(data_len == 0) return ;
    
    uint16 tail_len = pCur->pUartBuff + pCur->UartBuffSize - pCur->pInPos ;
    if(tail_len >= data_len){  //do not need to loop back  the queue
        os_memcpy(pCur->pInPos , pdata , data_len );
        pCur->pInPos += ( data_len );
        pCur->pInPos = (pCur->pUartBuff +  (pCur->pInPos - pCur->pUartBuff) % pCur->UartBuffSize );
        pCur->Space -=data_len;
    }else{
        os_memcpy(pCur->pInPos, pdata, tail_len);
        pCur->pInPos += ( tail_len );
        pCur->pInPos = (pCur->pUartBuff +  (pCur->pInPos - pCur->pUartBuff) % pCur->UartBuffSize );
        pCur->Space -=tail_len;
        os_memcpy(pCur->pInPos, pdata+tail_len , data_len-tail_len);
        pCur->pInPos += ( data_len-tail_len );
        pCur->pInPos = (pCur->pUartBuff +  (pCur->pInPos - pCur->pUartBuff) % pCur->UartBuffSize );
        pCur->Space -=( data_len-tail_len);
    }
    
}

/******************************************************************************
 * FunctionName : uart_buf_free
 * Description  : deinit of the tx buffer
 * Parameters   : struct UartBuffer* pTxBuff - tx buffer struct pointer
 * Returns      : NONE
*******************************************************************************/
void ICACHE_FLASH_ATTR
uart_buf_free(struct UartBuffer* pBuff)
{
    os_free(pBuff->pUartBuff);
    os_free(pBuff);
}


//rx buffer dequeue
uint16 ICACHE_FLASH_ATTR
rx_buff_deq(char* pdata, uint16 data_len )
{
    uint16 buf_len =  (pRxBuffer->UartBuffSize- pRxBuffer->Space);
    uint16 tail_len = pRxBuffer->pUartBuff + pRxBuffer->UartBuffSize - pRxBuffer->pOutPos ;
    uint16 len_tmp = 0;
    len_tmp = ((data_len > buf_len)?buf_len:data_len);
    if(pRxBuffer->pOutPos <= pRxBuffer->pInPos){
        os_memcpy(pdata, pRxBuffer->pOutPos,len_tmp);
        pRxBuffer->pOutPos+= len_tmp;
        pRxBuffer->Space += len_tmp;
    }else{
        if(len_tmp>tail_len){
            os_memcpy(pdata, pRxBuffer->pOutPos, tail_len);
            pRxBuffer->pOutPos += tail_len;
            pRxBuffer->pOutPos = (pRxBuffer->pUartBuff +  (pRxBuffer->pOutPos- pRxBuffer->pUartBuff) % pRxBuffer->UartBuffSize );
            pRxBuffer->Space += tail_len;
            
            os_memcpy(pdata+tail_len , pRxBuffer->pOutPos, len_tmp-tail_len);
            pRxBuffer->pOutPos+= ( len_tmp-tail_len );
            pRxBuffer->pOutPos= (pRxBuffer->pUartBuff +  (pRxBuffer->pOutPos- pRxBuffer->pUartBuff) % pRxBuffer->UartBuffSize );
            pRxBuffer->Space +=( len_tmp-tail_len);                
        }else{
            //os_printf("case 3 in rx deq\n\r");
            os_memcpy(pdata, pRxBuffer->pOutPos, len_tmp);
            pRxBuffer->pOutPos += len_tmp;
            pRxBuffer->pOutPos = (pRxBuffer->pUartBuff +  (pRxBuffer->pOutPos- pRxBuffer->pUartBuff) % pRxBuffer->UartBuffSize );
            pRxBuffer->Space += len_tmp;
        }
    }
    if(pRxBuffer->Space >= UART_FIFO_LEN){
        uart_rx_intr_enable(UART0);
    }
    return len_tmp; 
}


//move data from uart fifo to rx buffer
void Uart_rx_buff_enq()
{
    uint8 fifo_len,buf_idx;
    uint8 fifo_data;
    #if 1
    fifo_len = (READ_PERI_REG(UART_STATUS(UART0))>>UART_RXFIFO_CNT_S)&UART_RXFIFO_CNT;
    if(fifo_len >= pRxBuffer->Space){
        os_printf("buf full!!!\n\r");            
    }else{
        buf_idx=0;
        while(buf_idx < fifo_len){
            buf_idx++;
            fifo_data = READ_PERI_REG(UART_FIFO(UART0)) & 0xFF;
            *(pRxBuffer->pInPos++) = fifo_data;
            if(pRxBuffer->pInPos == (pRxBuffer->pUartBuff + pRxBuffer->UartBuffSize)){
                pRxBuffer->pInPos = pRxBuffer->pUartBuff;
            }            
        }
        pRxBuffer->Space -= fifo_len ;
        if(pRxBuffer->Space >= UART_FIFO_LEN){
            //os_printf("after rx enq buf enough\n\r");
            uart_rx_intr_enable(UART0);
        }
    }
    #endif
}


//fill the uart tx buffer
void ICACHE_FLASH_ATTR
tx_buff_enq(char* pdata, uint16 data_len )
{
    CLEAR_PERI_REG_MASK(UART_INT_ENA(UART0), UART_TXFIFO_EMPTY_INT_ENA);

    if(pTxBuffer == NULL){
        DBG1("\n\rnull, create buffer struct\n\r");
        pTxBuffer = Uart_Buf_Init(UART_TX_BUFFER_SIZE);
        if(pTxBuffer!= NULL){
            Uart_Buf_Cpy(pTxBuffer ,  pdata,  data_len );
        }else{
            DBG1("uart tx MALLOC no buf \n\r");
        }
    }else{
        if(data_len <= pTxBuffer->Space){
        Uart_Buf_Cpy(pTxBuffer ,  pdata,  data_len);
        }else{
            DBG1("UART TX BUF FULL!!!!\n\r");
        }
    }
    #if 0
    if(pTxBuffer->Space <= URAT_TX_LOWER_SIZE){
	    set_tcp_block();        
    }
    #endif
    SET_PERI_REG_MASK(UART_CONF1(UART0), (UART_TX_EMPTY_THRESH_VAL & UART_TXFIFO_EMPTY_THRHD)<<UART_TXFIFO_EMPTY_THRHD_S);
    SET_PERI_REG_MASK(UART_INT_ENA(UART0), UART_TXFIFO_EMPTY_INT_ENA);
}



//--------------------------------
LOCAL void tx_fifo_insert(struct UartBuffer* pTxBuff, uint8 data_len,  uint8 uart_no)
{
    uint8 i;
    for(i = 0; i<data_len;i++){
        WRITE_PERI_REG(UART_FIFO(uart_no) , *(pTxBuff->pOutPos++));
        if(pTxBuff->pOutPos == (pTxBuff->pUartBuff + pTxBuff->UartBuffSize)){
            pTxBuff->pOutPos = pTxBuff->pUartBuff;
        }
    }
    pTxBuff->pOutPos = (pTxBuff->pUartBuff +  (pTxBuff->pOutPos - pTxBuff->pUartBuff) % pTxBuff->UartBuffSize );
    pTxBuff->Space += data_len;
}


/******************************************************************************
 * FunctionName : tx_start_uart_buffer
 * Description  : get data from the tx buffer and fill the uart tx fifo, co-work with the uart fifo empty interrupt
 * Parameters   : uint8 uart_no - uart port num
 * Returns      : NONE
*******************************************************************************/
void tx_start_uart_buffer(uint8 uart_no)
{
    uint8 tx_fifo_len = (READ_PERI_REG(UART_STATUS(uart_no))>>UART_TXFIFO_CNT_S)&UART_TXFIFO_CNT;
    uint8 fifo_remain = UART_FIFO_LEN - tx_fifo_len ;
    uint8 len_tmp;
    uint16 tail_ptx_len,head_ptx_len,data_len;
    //struct UartBuffer* pTxBuff = *get_buff_prt();
    
    if(pTxBuffer){      
        data_len = (pTxBuffer->UartBuffSize - pTxBuffer->Space);
        if(data_len > fifo_remain){
            len_tmp = fifo_remain;
            tx_fifo_insert( pTxBuffer,len_tmp,uart_no);
            SET_PERI_REG_MASK(UART_INT_ENA(UART0), UART_TXFIFO_EMPTY_INT_ENA);
        }else{
            len_tmp = data_len;
            tx_fifo_insert( pTxBuffer,len_tmp,uart_no);
        }
    }else{
        DBG1("pTxBuff null \n\r");
    }
}

#endif


void uart_rx_intr_disable(uint8 uart_no)
{
#if 1
    CLEAR_PERI_REG_MASK(UART_INT_ENA(uart_no), UART_RXFIFO_FULL_INT_ENA|UART_RXFIFO_TOUT_INT_ENA);
#else
    ETS_UART_INTR_DISABLE();
#endif
}

void uart_rx_intr_enable(uint8 uart_no)
{
#if 1
    SET_PERI_REG_MASK(UART_INT_ENA(uart_no), UART_RXFIFO_FULL_INT_ENA|UART_RXFIFO_TOUT_INT_ENA);
#else
    ETS_UART_INTR_ENABLE();
#endif
}


//========================================================
LOCAL void
uart0_write_char(char c)
{
    if (c == '\n') {
        uart_tx_one_char(UART0, '\r');
        uart_tx_one_char(UART0, '\n');
    } else if (c == '\r') {
    } else {
        uart_tx_one_char(UART0, c);
    }
}

void ICACHE_FLASH_ATTR
UART_SetWordLength(uint8 uart_no, UartBitsNum4Char len) 
{
    SET_PERI_REG_BITS(UART_CONF0(uart_no),UART_BIT_NUM,len,UART_BIT_NUM_S);
}

void ICACHE_FLASH_ATTR
UART_SetStopBits(uint8 uart_no, UartStopBitsNum bit_num) 
{
    SET_PERI_REG_BITS(UART_CONF0(uart_no),UART_STOP_BIT_NUM,bit_num,UART_STOP_BIT_NUM_S);
}

void ICACHE_FLASH_ATTR
UART_SetLineInverse(uint8 uart_no, UART_LineLevelInverse inverse_mask) 
{
    CLEAR_PERI_REG_MASK(UART_CONF0(uart_no), UART_LINE_INV_MASK);
    SET_PERI_REG_MASK(UART_CONF0(uart_no), inverse_mask);
}

void ICACHE_FLASH_ATTR
UART_SetParity(uint8 uart_no, UartParityMode Parity_mode) 
{
    CLEAR_PERI_REG_MASK(UART_CONF0(uart_no), UART_PARITY |UART_PARITY_EN);
    if(Parity_mode==NONE_BITS){
    }else{
        SET_PERI_REG_MASK(UART_CONF0(uart_no), Parity_mode|UART_PARITY_EN);
    }
}

void ICACHE_FLASH_ATTR
UART_SetBaudrate(uint8 uart_no,uint32 baud_rate)
{
    uart_div_modify(uart_no, UART_CLK_FREQ /baud_rate);
}

void ICACHE_FLASH_ATTR
UART_SetFlowCtrl(uint8 uart_no,UART_HwFlowCtrl flow_ctrl,uint8 rx_thresh)
{
    if(flow_ctrl&USART_HardwareFlowControl_RTS){
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_U0RTS);
        SET_PERI_REG_BITS(UART_CONF1(uart_no),UART_RX_FLOW_THRHD,rx_thresh,UART_RX_FLOW_THRHD_S);
        SET_PERI_REG_MASK(UART_CONF1(uart_no), UART_RX_FLOW_EN);
    }else{
        CLEAR_PERI_REG_MASK(UART_CONF1(uart_no), UART_RX_FLOW_EN);
    }
    if(flow_ctrl&USART_HardwareFlowControl_CTS){
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_UART0_CTS);
        SET_PERI_REG_MASK(UART_CONF0(uart_no), UART_TX_FLOW_EN);
    }else{
        CLEAR_PERI_REG_MASK(UART_CONF0(uart_no), UART_TX_FLOW_EN);
    }
}

void ICACHE_FLASH_ATTR
UART_WaitTxFifoEmpty(uint8 uart_no , uint32 time_out_us) //do not use if tx flow control enabled
{
    uint32 t_s = system_get_time();
    while (READ_PERI_REG(UART_STATUS(uart_no)) & (UART_TXFIFO_CNT << UART_TXFIFO_CNT_S)){
		
        if(( system_get_time() - t_s )> time_out_us){
            break;
        }
	WRITE_PERI_REG(0X60000914, 0X73);//WTD

    }
}


bool ICACHE_FLASH_ATTR
UART_CheckOutputFinished(uint8 uart_no, uint32 time_out_us)
{
    uint32 t_start = system_get_time();
    uint8 tx_fifo_len;
    uint32 tx_buff_len;
    while(1){
        tx_fifo_len =( (READ_PERI_REG(UART_STATUS(uart_no))>>UART_TXFIFO_CNT_S)&UART_TXFIFO_CNT);
        if(pTxBuffer){
            tx_buff_len = ((pTxBuffer->UartBuffSize)-(pTxBuffer->Space));
        }else{
            tx_buff_len = 0;
        }
		
        if( tx_fifo_len==0 && tx_buff_len==0){
            return TRUE;
        }
        if( system_get_time() - t_start > time_out_us){
            return FALSE;
        }
	WRITE_PERI_REG(0X60000914, 0X73);//WTD
    }    
}


void ICACHE_FLASH_ATTR
UART_ResetFifo(uint8 uart_no)
{
    SET_PERI_REG_MASK(UART_CONF0(uart_no), UART_RXFIFO_RST | UART_TXFIFO_RST);
    CLEAR_PERI_REG_MASK(UART_CONF0(uart_no), UART_RXFIFO_RST | UART_TXFIFO_RST);
}

void ICACHE_FLASH_ATTR
UART_ClearIntrStatus(uint8 uart_no,uint32 clr_mask)
{
    WRITE_PERI_REG(UART_INT_CLR(uart_no), clr_mask);
}

void ICACHE_FLASH_ATTR
UART_SetIntrEna(uint8 uart_no,uint32 ena_mask)
{
    SET_PERI_REG_MASK(UART_INT_ENA(uart_no), ena_mask);
}


void ICACHE_FLASH_ATTR
UART_SetPrintPort(uint8 uart_no)
{
    if(uart_no==1){
        os_install_putc1(uart1_write_char);
    }else{
        /*option 1: do not wait if uart fifo is full,drop current character*/
        os_install_putc1(uart0_write_char_no_wait);
	/*option 2: wait for a while if uart fifo is full*/
	os_install_putc1(uart0_write_char);
    }
}


//========================================================


/*test code*/
void ICACHE_FLASH_ATTR
uart_init_2(UartBautRate uart0_br, UartBautRate uart1_br)
{
    // rom use 74880 baut_rate, here reinitialize
    UartDev.baut_rate = uart0_br;
    UartDev.exist_parity = STICK_PARITY_EN;
    UartDev.parity = EVEN_BITS;
    UartDev.stop_bits = ONE_STOP_BIT;
    UartDev.data_bits = EIGHT_BITS;
	
    uart_config(UART0);
    UartDev.baut_rate = uart1_br;
    uart_config(UART1);
    ETS_UART_INTR_ENABLE();

    // install uart1 putc callback
    os_install_putc1((void *)uart1_write_char);//print output at UART1
}


