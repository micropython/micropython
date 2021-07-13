#define MICROPY_HW_BOARD_NAME "Pimoroni Keybow 2040"
#define MICROPY_HW_MCU_NAME "rp2040"

#define MICROPY_HW_SW0 (&pin_GPIO21)
#define MICROPY_HW_SW1 (&pin_GPIO20)
#define MICROPY_HW_SW2 (&pin_GPIO19)
#define MICROPY_HW_SW3 (&pin_GPIO18)
#define MICROPY_HW_SW4 (&pin_GPIO17)
#define MICROPY_HW_SW5 (&pin_GPIO16)
#define MICROPY_HW_SW6 (&pin_GPIO15)
#define MICROPY_HW_SW7 (&pin_GPIO14)
#define MICROPY_HW_SW8 (&pin_GPIO13)
#define MICROPY_HW_SW9 (&pin_GPIO12)
#define MICROPY_HW_SW10 (&pin_GPIO11)
#define MICROPY_HW_SW11 (&pin_GPIO10)
#define MICROPY_HW_SW12 (&pin_GPIO9)
#define MICROPY_HW_SW13 (&pin_GPIO8)
#define MICROPY_HW_SW14 (&pin_GPIO7)
#define MICROPY_HW_SW15 (&pin_GPIO6)

#define MICROPY_HW_USER_SW (&pin_GPIO23)

#define MICROPY_HW_I2C_INT (&pin_GPIO3)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO5)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO4)

#define DEFAULT_UART_BUS_RX (&pin_GPIO1)
#define DEFAULT_UART_BUS_TX (&pin_GPIO0)

// These pins are unconnected
#define IGNORE_PIN_GPIO2    1
#define IGNORE_PIN_GPIO22   1
#define IGNORE_PIN_GPIO24   1
#define IGNORE_PIN_GPIO25   1
#define IGNORE_PIN_GPIO26   1
#define IGNORE_PIN_GPIO27   1
#define IGNORE_PIN_GPIO28   1
#define IGNORE_PIN_GPIO29   1
