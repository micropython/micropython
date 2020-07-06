#ifndef MICROPY_INCLUDED_ESP8266_UART_H
#define MICROPY_INCLUDED_ESP8266_UART_H

#include <eagle_soc.h>

#define UART0 (0)
#define UART1 (1)

#define UART0_STATIC_RXBUF_LEN (16)

typedef enum {
    UART_FIVE_BITS = 0x0,
    UART_SIX_BITS = 0x1,
    UART_SEVEN_BITS = 0x2,
    UART_EIGHT_BITS = 0x3
} UartBitsNum4Char;

typedef enum {
    UART_ONE_STOP_BIT             = 0x1,
    UART_ONE_HALF_STOP_BIT        = 0x2,
    UART_TWO_STOP_BIT             = 0x3
} UartStopBitsNum;

typedef enum {
    UART_NONE_BITS  = 0,
    UART_ODD_BITS   = BIT0,
    UART_EVEN_BITS  = 0
} UartParityMode;

typedef enum {
    UART_STICK_PARITY_DIS   = 0,
    UART_STICK_PARITY_EN    = BIT1
} UartExistParity;

typedef enum {
    UART_BIT_RATE_9600     = 9600,
    UART_BIT_RATE_19200   = 19200,
    UART_BIT_RATE_38400   = 38400,
    UART_BIT_RATE_57600   = 57600,
    UART_BIT_RATE_74880   = 74880,
    UART_BIT_RATE_115200 = 115200,
    UART_BIT_RATE_230400 = 230400,
    UART_BIT_RATE_256000 = 256000,
    UART_BIT_RATE_460800 = 460800,
    UART_BIT_RATE_921600 = 921600
} UartBautRate;

typedef enum {
    UART_NONE_CTRL,
    UART_HARDWARE_CTRL,
    UART_XON_XOFF_CTRL
} UartFlowCtrl;

typedef enum {
    UART_EMPTY,
    UART_UNDER_WRITE,
    UART_WRITE_OVER
} RcvMsgBuffState;

typedef struct {
    uint32 RcvBuffSize;
    uint8 *pRcvMsgBuff;
    uint8 *pWritePos;
    uint8 *pReadPos;
    uint8 TrigLvl;      // JLU: may need to pad
    RcvMsgBuffState BuffState;
} RcvMsgBuff;

typedef struct {
    uint32 TrxBuffSize;
    uint8 *pTrxBuff;
} TrxMsgBuff;

typedef enum {
    UART_BAUD_RATE_DET,
    UART_WAIT_SYNC_FRM,
    UART_SRCH_MSG_HEAD,
    UART_RCV_MSG_BODY,
    UART_RCV_ESC_CHAR,
} RcvMsgState;

typedef struct {
    UartBautRate baut_rate;
    UartBitsNum4Char data_bits;
    UartExistParity exist_parity;
    UartParityMode parity;       // chip size in byte
    UartStopBitsNum stop_bits;
    UartFlowCtrl flow_ctrl;
    RcvMsgBuff rcv_buff;
    TrxMsgBuff trx_buff;
    RcvMsgState rcv_state;
    int received;
    int buff_uart_no;                // indicate which uart use tx/rx buffer
} UartDevice;

extern uint8 uart_ringbuf_array[UART0_STATIC_RXBUF_LEN];

void uart_init(UartBautRate uart0_br, UartBautRate uart1_br);
int uart0_rx(void);
bool uart_rx_wait(uint32_t timeout_us);
int uart_rx_char(void);
void uart_tx_one_char(uint8 uart, uint8 TxChar);
void uart_flush(uint8 uart);
void uart_os_config(int uart);
void uart_setup(uint8 uart);
int uart0_get_rxbuf_len(void);
void uart0_set_rxbuf(uint8 *buf, int len);
// check status of rx/tx
int uart_rx_any(uint8 uart);
int uart_tx_any_room(uint8 uart);

#endif // MICROPY_INCLUDED_ESP8266_UART_H
