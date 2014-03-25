typedef enum {
    PYB_USART_NONE = 0,
    PYB_USART_1 = 1,
    PYB_USART_2 = 2,
    PYB_USART_3 = 3,
    PYB_USART_6 = 4,
    PYB_USART_MAX = 4,

    //PYB_USART_XA =  // USART4 on X1, X2 = PA0, PA1
    PYB_USART_XB = 1, // USART1 on X9, X10 = PB6, PB7
    PYB_USART_YA = 4, // USART6 on Y1, Y2 = PC6, PC7
    PYB_USART_YB = 3, // USART3 on Y9, Y10 = PB10, PB11
} pyb_usart_t;

typedef struct _pyb_usart_obj_t pyb_usart_obj_t;

extern pyb_usart_obj_t *pyb_usart_global_debug;
extern const mp_obj_type_t pyb_usart_type;

void usart_init(pyb_usart_obj_t *usart_obj, uint32_t baudrate);
bool usart_rx_any(pyb_usart_obj_t *usart_obj);
int usart_rx_char(pyb_usart_obj_t *usart_obj);
void usart_tx_str(pyb_usart_obj_t *usart_obj, const char *str);
void usart_tx_strn(pyb_usart_obj_t *usart_obj, const char *str, uint len);
void usart_tx_strn_cooked(pyb_usart_obj_t *usart_obj, const char *str, uint len);

