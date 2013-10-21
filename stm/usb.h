void usb_init();
bool usb_vcp_is_enabled();
int usb_vcp_rx_any();
char usb_vcp_rx_get();
void usb_vcp_send_str(const char* str);
void usb_vcp_send_strn(const char* str, int len);
