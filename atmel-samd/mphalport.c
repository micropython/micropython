#include <string.h>

#include "asf/common/services/usb/class/cdc/device/udi_cdc.h"
#include "asf/common2/services/delay/delay.h"
#include "asf/sam0/drivers/sercom/usart/usart.h"
#include "py/mphal.h"
#include "py/mpstate.h"

#include "mpconfigboard.h"

static volatile bool mp_cdc_enabled = false;
extern struct usart_module usart_instance;

bool mp_cdc_enable(uint8_t port)
{
	mp_cdc_enabled = true;
	return true;
}

void mp_cdc_disable(uint8_t port)
{
	mp_cdc_enabled = false;
}

int mp_hal_stdin_rx_chr(void) {
  for (;;) {
    #ifdef USB_REPL
    if (mp_cdc_enabled && udi_cdc_is_rx_ready()) {
      return udi_cdc_getc();
    }
    #endif
    #ifdef UART_REPL
    uint16_t temp;
    if (usart_read_wait(&usart_instance, &temp) == STATUS_OK) {
      return temp;
    }
    #endif
    // TODO(tannewt): Figure out how we can sleep while waiting for input and
    // add it here. The current UART implementation doesn't cause a wake.
    //__WFI();
  }
}

//void mp_hal_stdout_tx_str(const char *str) {
//    mp_hal_stdout_tx_strn(str, strlen(str));
//}

void mp_hal_stdout_tx_strn(const char *str, size_t len) {
  #ifdef UART_REPL
  usart_write_buffer_wait(&usart_instance, (uint8_t*) str, len);
  #endif

  #ifdef USB_REPL
  if (mp_cdc_enabled && udi_cdc_is_tx_ready()) {
    udi_cdc_write_buf(str, len);
  }
  #endif
}

void mp_hal_delay_ms(mp_uint_t delay) {
  delay_ms(delay);
}

void mp_hal_delay_us(mp_uint_t delay) {
  delay_us(delay);
}
