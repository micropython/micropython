#define MICROPY_HW_BOARD_NAME "Pimoroni Interstate 75"
#define MICROPY_HW_MCU_NAME "rp2040"

#define MICROPY_HW_R0 (&pin_GPIO0)
#define MICROPY_HW_G0 (&pin_GPIO1)
#define MICROPY_HW_B0 (&pin_GPIO2)
#define MICROPY_HW_R1 (&pin_GPIO3)
#define MICROPY_HW_G1 (&pin_GPIO4)
#define MICROPY_HW_B1 (&pin_GPIO5)

#define MICROPY_HW_ROW_A (&pin_GPIO6)
#define MICROPY_HW_ROW_B (&pin_GPIO7)
#define MICROPY_HW_ROW_C (&pin_GPIO8)
#define MICROPY_HW_ROW_D (&pin_GPIO9)
#define MICROPY_HW_ROW_E (&pin_GPIO10)

#define MICROPY_HW_CLK (&pin_GPIO11)
#define MICROPY_HW_LAT (&pin_GPIO12)
#define MICROPY_HW_OE (&pin_GPIO13)

#define MICROPY_HW_SW_A (&pin_GPIO14)

#define CIRCUITPY_RGB_STATUS_INVERTED_PWM
#define CIRCUITPY_RGB_STATUS_R (&pin_GPIO16)
#define CIRCUITPY_RGB_STATUS_G (&pin_GPIO17)
#define CIRCUITPY_RGB_STATUS_B (&pin_GPIO18)

#define MICROPY_HW_I2C_INT (&pin_GPIO19)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO21)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO20)

#define MICROPY_HW_USER_SW (&pin_GPIO23)

#define MICROPY_HW_CURRENT_SENSE (&pin_GPIO29)

// These pins are unconnected
#define IGNORE_PIN_GPIO15   1
#define IGNORE_PIN_GPIO22   1
#define IGNORE_PIN_GPIO24   1
#define IGNORE_PIN_GPIO25   1
