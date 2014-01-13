typedef enum {
    PYB_USART_NONE = 0,
    PYB_USART_1 = 1,
    PYB_USART_2 = 2,
    PYB_USART_3 = 3,
    PYB_USART_6 = 4,
    PYB_USART_MAX = 4,
} pyb_usart_t;

extern pyb_usart_t pyb_usart_global_debug;

void usart_init(pyb_usart_t usart_id, uint32_t baudrate);
bool usart_rx_any(pyb_usart_t usart_id);
int usart_rx_char(pyb_usart_t usart_id);
void usart_tx_str(pyb_usart_t usart_id, const char *str);
void usart_tx_strn_cooked(pyb_usart_t usart_id, const char *str, int len);

mp_obj_t pyb_Usart(mp_obj_t usart_id, mp_obj_t baudrate);
