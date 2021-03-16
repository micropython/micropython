#define MICROPY_HW_BOARD_NAME "SparkFun LUMIDrive"
#define MICROPY_HW_MCU_NAME "samd21g18"

// Clock rates are off: Salae reads 12MHz which is the limit even though we set it to the safer 8MHz.
#define SPI_FLASH_BAUDRATE  (8000000)

#define SPI_FLASH_MOSI_PIN   &pin_PA08
#define SPI_FLASH_MISO_PIN   &pin_PA14
#define SPI_FLASH_SCK_PIN    &pin_PA09
#define SPI_FLASH_CS_PIN     &pin_PA13

#define MICROPY_PORT_A    (0)
#define MICROPY_PORT_B    (0)
#define MICROPY_PORT_C    (0)

#define BOARD_HAS_CRYSTAL 1

// I2C
#define DEFAULT_I2C_BUS_SCL (&pin_PA23)
#define DEFAULT_I2C_BUS_SDA (&pin_PA22)

// SPI
#define DEFAULT_SPI_BUS_SCK (&pin_PA19)
#define DEFAULT_SPI_BUS_MOSI (&pin_PA18)
#define DEFAULT_SPI_BUS_MISO (&pin_PA21)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1

#define IGNORE_PIN_PA02     1
#define IGNORE_PIN_PA03     1
#define IGNORE_PIN_PA10     1
#define IGNORE_PIN_PA11     1
#define IGNORE_PIN_PA12     1
#define IGNORE_PIN_PA15     1
#define IGNORE_PIN_PA16     1
#define IGNORE_PIN_PA27     1
#define IGNORE_PIN_PA28     1

#define IGNORE_PIN_PB02     1
#define IGNORE_PIN_PB03     1
#define IGNORE_PIN_PB08     1
#define IGNORE_PIN_PB09     1
#define IGNORE_PIN_PB10     1
#define IGNORE_PIN_PB11     1
#define IGNORE_PIN_PB22     1
#define IGNORE_PIN_PB23     1
