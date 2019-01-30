#define MICROPY_HW_BOARD_NAME "SparkFun LUMIDrive"
#define MICROPY_HW_MCU_NAME "samd21g18"

//#define MICROPY_HW_NEOPIXEL (&pin_PA06)

// Clock rates are off: Salae reads 12MHz which is the limit even though we set it to the safer 8MHz.
#define SPI_FLASH_BAUDRATE  (8000000)

#define SPI_FLASH_MOSI_PIN   &PIN_PA08
#define SPI_FLASH_MISO_PIN   &PIN_PA14
#define SPI_FLASH_SCK_PIN    &PIN_PA09
#define SPI_FLASH_CS_PIN     &PIN_PA13

// All port(pin) definitions should be deleted unless PA06 -- SERCOM0/PAD[2]
// Will need to scour the repo to figure out why this was done. 
// I'm guessing these ports are defined elsewehere by default rather than needing to
// repeatedly define them. Saves time to name outliers than participants in
// some cases. 
#define MICROPY_PORT_A    ( 0 )
#define MICROPY_PORT_B    ( 0 )
#define MICROPY_PORT_C    ( 0 )

// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code.
#define CIRCUITPY_INTERNAL_NVM_SIZE 256

#define BOARD_FLASH_SIZE (0x00040000 - 0x2000 - CIRCUITPY_INTERNAL_NVM_SIZE)

#define BOARD_HAS_CRYSTAL 1


// The LUMIDrive board exposed pins are limited, and I2C, TX/RX, and SPI are not 
// really exposed but if they are not define (and included in pins.h), Python 
// won't start/operate. So define here:

//I2C
#define DEFAULT_I2C_BUS_SCL (&pin_PA23)
#define DEFAULT_I2C_BUS_SDA (&pin_PA22)

//SPI
#define DEFAULT_SPI_BUS_SCK (&pin_PA19)
#define DEFAULT_SPI_BUS_MOSI (&pin_PA18)
#define DEFAULT_SPI_BUS_MISO (&pin_PA21)

//UART
#define DEFAULT_UART_BUS_RX (&pin_PA11)
#define DEFAULT_UART_BUS_TX (&pin_PA10)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
