#include <jtag.h>
#include <gpio.h>

const int pios_jtag_tck = 25;
const int pios_jtag_tdi = 4;
const int pios_jtag_tdo = 24;
const int pios_jtag_tms = 27;
const int pios_jtag_trst = 22;

int pios_jtag_init ()
{
    uint32_t p[2] = {0xff, 0xff};
    pios_gpio_pullBulk ( p, PIOS_GPIO_PULL_OFF );
    
    pios_gpio_pinmode ( pios_jtag_tck, PIOS_GPIO_ALT4 );
    pios_gpio_pinmode ( pios_jtag_tdi, PIOS_GPIO_ALT5 );
    pios_gpio_pinmode ( pios_jtag_tdo, PIOS_GPIO_ALT4 );
    pios_gpio_pinmode ( pios_jtag_tms, PIOS_GPIO_ALT4 );
    pios_gpio_pinmode ( pios_jtag_trst, PIOS_GPIO_ALT4 );
    
    return 0;
}
