#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "../stm/usart.h"

pyb_usart_t pyb_usart_global_debug = PYB_USART_NONE;

void usart_init(pyb_usart_t usart_id, uint32_t baudrate)
{
    (void)usart_id;
    (void)baudrate;
}

bool usart_rx_any(pyb_usart_t usart_id)
{
    (void)usart_id;
    return false;
}

int usart_rx_char(pyb_usart_t usart_id)
{
    (void)usart_id;
    return 0;
}

void usart_tx_str(pyb_usart_t usart_id, const char *str)
{
    (void)usart_id;
    (void)str;
}

void usart_tx_strn_cooked(pyb_usart_t usart_id, const char *str, int len)
{
  (void)usart_id;
  (void)str;
  (void)len;
}
