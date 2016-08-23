#include <string.h>

//#include "board_driver_usb.h"
#include "asf/common/services/usb/class/cdc/device/udi_cdc.h"
#include "py/mphal.h"

static volatile bool mp_cdc_enabled = false;
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
    // char c;
    if (mp_cdc_enabled && udi_cdc_is_rx_ready()) {
      return udi_cdc_getc();
    }
    // } else if (MP_STATE_PORT(pyb_stdio_uart) != NULL && uart_rx_any(MP_STATE_PORT(pyb_stdio_uart))) {
    //     return uart_rx_char(MP_STATE_PORT(pyb_stdio_uart));
    // }
    __WFI();
  }
}

//void mp_hal_stdout_tx_str(const char *str) {
//    mp_hal_stdout_tx_strn(str, strlen(str));
//}

void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    // if (MP_STATE_PORT(pyb_stdio_uart) != NULL) {
    //     uart_tx_strn(MP_STATE_PORT(pyb_stdio_uart), str, len);
    // }
    if (mp_cdc_enabled && udi_cdc_is_tx_ready()) {
      udi_cdc_write_buf(str, len);
    }
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
