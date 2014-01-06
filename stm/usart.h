void usart_init(void);
MP_BOOL usart_is_enabled(void);
MP_BOOL usart_rx_any(void);
int usart_rx_char(void);
void usart_tx_char(int c);
void usart_tx_str(const char *str);
void usart_tx_strn_cooked(const char *str, int len);
