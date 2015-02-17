bool usb_vcp_is_connected(void);
bool usb_vcp_is_enabled(void);
int usb_vcp_rx_num(void);
int usb_vcp_recv_byte(uint8_t *ptr);
void usb_vcp_send_str(const char* str);
void usb_vcp_send_strn(const char* str, int len);
void usb_vcp_send_strn_cooked(const char *str, int len);
