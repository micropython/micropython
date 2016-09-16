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
#include "asf/sam0/drivers/port/port.h"

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
}
