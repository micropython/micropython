#include <string.h>

#include "asf/common/services/usb/class/cdc/device/udi_cdc.h"
#include "asf/sam0/drivers/sercom/usart/usart.h"
#include "py/mphal.h"
#include "py/mpstate.h"

#include "mpconfigboard.h"

#include "asf/common2/services/delay/delay.h"
#include "asf/sam0/drivers/port/port.h"
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
  usart_write_buffer_wait(&usart_instance, (uint8_t*) str, len);
  // if (mp_cdc_enabled && udi_cdc_is_tx_ready()) {
  //   udi_cdc_write_buf(str, len);
  // }
}

//void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    // send stdout to UART and USB CDC VCP
    // if (MP_STATE_PORT(pyb_stdio_uart) != NULL) {
    //     uart_tx_strn_cooked(MP_STATE_PORT(pyb_stdio_uart), str, len);
    // }
    // if (usb_vcp_is_enabled()) {
    //     usb_vcp_send_strn_cooked(str, len);
    // }
//}
//
// void mp_hal_gpio_clock_enable(GPIO_TypeDef *gpio) {
// }
//
// void mp_hal_gpio_config(GPIO_TypeDef *gpio, uint32_t pin, uint32_t mode, uint32_t pull, uint32_t alt) {
//     //mp_hal_gpio_clock_enable(gpio);
// }
//
// bool mp_hal_gpio_set_af(const pin_obj_t *pin, GPIO_InitTypeDef *init, uint8_t fn, uint8_t unit) {
//     //mp_hal_gpio_clock_enable(pin->gpio);
//
// }
