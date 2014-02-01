#define VCP_CHAR_NONE   (0)
#define VCP_CHAR_CTRL_C (3)
#define VCP_CHAR_CTRL_D (4)

void pyb_usb_dev_init(void);
bool usb_vcp_is_enabled(void);
bool usb_vcp_is_connected(void);
void usb_vcp_set_interrupt_char(int c);
int usb_vcp_rx_any(void);
char usb_vcp_rx_get(void);
void usb_vcp_send_str(const char* str);
void usb_vcp_send_strn(const char* str, int len);
void usb_vcp_send_strn_cooked(const char *str, int len);
void usb_hid_send_report(uint8_t *buf); // 4 bytes for mouse: ?, x, y, ?

void pyb_usb_host_init(void);
void pyb_usb_host_process(void);
uint pyb_usb_host_get_keyboard(void);
