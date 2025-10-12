/* Patch rpi_pico zephyr release pinctrl bug */
#if KERNEL_VERSION_NUMBER <= ZEPHYR_VERSION(4, 4, 2)
#define RP2_PINCTRL_GPIO_FUNC_UART_ALT 11
#endif

#include "mpconfig.h"
