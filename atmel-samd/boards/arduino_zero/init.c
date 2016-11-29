/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "board.h"
#include "conf_board.h"
#include "mpconfigboard.h"
#include "asf/sam0/drivers/sercom/usart/usart.h"
#include "asf/sam0/drivers/port/port.h"

extern struct usart_module usart_instance;

void configure_usart(void)
{
    struct usart_config config_usart;

    usart_get_config_defaults(&config_usart);

    config_usart.baudrate    = 115200;
    config_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
    config_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
    config_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
    config_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
    config_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;

    while (usart_init(&usart_instance, EDBG_CDC_MODULE, &config_usart) != STATUS_OK) {
    }
    usart_enable(&usart_instance);
}

void board_init(void)
{
    /* This function is meant to contain board-specific initialization code
     * for, e.g., the I/O pins. The initialization can rely on application-
     * specific board configuration, found in conf_board.h.
     */
    struct port_config pin_conf;
    port_get_config_defaults(&pin_conf);

    pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
    port_pin_set_config(MICROPY_HW_LED_TX, &pin_conf);
    port_pin_set_output_level(MICROPY_HW_LED_TX, true);

    port_pin_set_config(MICROPY_HW_LED_RX, &pin_conf);
    port_pin_set_output_level(MICROPY_HW_LED_RX, true);

    // TODO(tannewt): Switch to proper pyb based UARTs.
    #ifdef UART_REPL
        configure_usart();
    #endif
}
