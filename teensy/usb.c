#include "Arduino.h"

#include "usb.h"
#include "usb_serial.h"

int usb_vcp_is_connected(void)
{
  return usb_configuration && (usb_cdc_line_rtsdtr & (USB_SERIAL_DTR | USB_SERIAL_RTS));
}

int usb_vcp_is_enabled(void)
{
  return 1;
}

void usb_vcp_set_interrupt_char(int c) {
  // The teensy 3.1 usb stack doesn't currently have the notion of generating
  // an exception when a certain character is received. That just means that
  // you can't press Control-C and get your python script to stop.
}

int usb_vcp_rx_num(void) {
  return usb_serial_available();
}

char usb_vcp_rx_get(void)
{
  return usb_serial_getchar();
}

void usb_vcp_send_str(const char* str)
{
  usb_vcp_send_strn(str, strlen(str));
}

void usb_vcp_send_strn(const char* str, int len)
{
  usb_serial_write(str, len);
}

void usb_vcp_send_strn_cooked(const char *str, int len)
{
  for (const char *top = str + len; str < top; str++) {
    if (*str == '\n') {
      usb_serial_putchar('\r');
    }
    usb_serial_putchar(*str);
  }
}
