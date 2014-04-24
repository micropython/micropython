typedef enum {
    PYB_UART_NONE = 0,
    PYB_UART_1 = 1,
    PYB_UART_2 = 2,
    PYB_UART_3 = 3,
    PYB_UART_4 = 4,
    PYB_UART_5 = 5,
    PYB_UART_6 = 6,

#if defined(PYBV10)
    PYB_UART_XA = 4, // UART4 on X1, X2 = PA0, PA1
    PYB_UART_XB = 1, // USART1 on X9, X10 = PB6, PB7
    PYB_UART_YA = 6, // USART6 on Y1, Y2 = PC6, PC7
    PYB_UART_YB = 3, // USART3 on Y9, Y10 = PB10, PB11
#endif

} pyb_uart_t;

typedef struct _pyb_uart_obj_t pyb_uart_obj_t;

extern pyb_uart_obj_t *pyb_uart_global_debug;
extern const mp_obj_type_t pyb_uart_type;

bool uart_init(pyb_uart_obj_t *uart_obj, uint32_t baudrate);
bool uart_rx_any(pyb_uart_obj_t *uart_obj);
int uart_rx_char(pyb_uart_obj_t *uart_obj);
void uart_tx_str(pyb_uart_obj_t *uart_obj, const char *str);
void uart_tx_strn(pyb_uart_obj_t *uart_obj, const char *str, uint len);
void uart_tx_strn_cooked(pyb_uart_obj_t *uart_obj, const char *str, uint len);

