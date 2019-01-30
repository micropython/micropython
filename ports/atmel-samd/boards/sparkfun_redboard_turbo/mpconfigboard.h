#define MICROPY_HW_BOARD_NAME "SparkFun RedBoard Turbo"
#define MICROPY_HW_MCU_NAME "samd21g18"


#define MICROPY_HW_LED_TX   &pin_PA27
#define MICROPY_HW_LED_RX   &pin_PA31

#define MICROPY_HW_NEOPIXEL (&pin_PA30)

// Clock rates are off: Salae reads 12MHz which is the limit even though we set it to the safer 8MHz.
#define SPI_FLASH_BAUDRATE  (8000000)

//
// SPI Flash definitions. The flash SPI MOSI, MISO and SCK need to be on the 
// same SERCOM - check the SAMD21 spec to determien which SERCOM to use 
// (post PINs have 2 SERCOMS to work with)
//
#define SPI_FLASH_MOSI_PIN          &pin_PB22
#define SPI_FLASH_MISO_PIN          &pin_PB03
#define SPI_FLASH_SCK_PIN           &pin_PB23
#define SPI_FLASH_CS_PIN            &pin_PA13


// #define SPI_FLASH_MOSI_PIN_FUNCTION PINMUX_PB22D_SERCOM5_PAD2
// #define SPI_FLASH_MISO_PIN_FUNCTION PINMUX_PB03D_SERCOM5_PAD1
// #define SPI_FLASH_SCK_PIN_FUNCTION  PINMUX_PB23D_SERCOM5_PAD3
// #define SPI_FLASH_SERCOM            SERCOM5
// #define SPI_FLASH_SERCOM_INDEX      5
// #define SPI_FLASH_MOSI_PAD          2
// #define SPI_FLASH_MISO_PAD          1
// #define SPI_FLASH_SCK_PAD           3


// // <o> Transmit Data Pinout

// // 
// // The logic for DOPO and DIPO is the following
// // In master SPI operation    
// //    DIPO  <pad #> = MISO
// //    DOPO  - Based on the below table, where 
// //           <DOPO> = [MOSI PAD, SCK PAD]
// //           <0x0>  = PAD[0,1]
// //           <0x1>  = PAD[2,3]
// //           <0x2>  = PAD[3,1]
// //           <0x3>  = PAD[0,3]
// //
// /// For the RedBoard Turbo Board DOPO [2,3] => 1
// #define SPI_FLASH_DOPO              1   
// #define SPI_FLASH_DIPO              1   // same as MISO pad

// These are pins not to reset. 
// This map to the pins as defined above for FLASH. Note the PORT letters - assign A-to-A, B-to-B ..etc
// PA24 and PA25 are for USB

#define MICROPY_PORT_A        ( 0 ) 
#define MICROPY_PORT_B        ( 0 )
#define MICROPY_PORT_C        ( 0 )


// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code.
#define CIRCUITPY_INTERNAL_NVM_SIZE 256

#define BOARD_FLASH_SIZE (0x00040000 - 0x2000 - CIRCUITPY_INTERNAL_NVM_SIZE)


// What external flash device is being used on this board?
// The devices are defined in "exteral_flash/devices.h", which 
// contains a flash struct def and a list of #defines for each
// supported/defined flash module. 
//
// This board - the LUMIDrive - uses a W25Q32FV component. It's 
// definition was added to devices.h, based on the winbond spec sheet.
//
// You can list N number of devices - just update the count define and comma 
// seperate the EXTERNAL_FLASH_DEVICES list of devices.  
                               
// #define EXTERNAL_FLASH_DEVICE_COUNT 1
// #define EXTERNAL_FLASH_DEVICES W25Q32FV


#define BOARD_HAS_CRYSTAL 1


// I2C - also QWIIC
#define DEFAULT_I2C_BUS_SCL (&pin_PA23)
#define DEFAULT_I2C_BUS_SDA (&pin_PA22)


#define DEFAULT_SPI_BUS_SCK (&pin_PB11)
#define DEFAULT_SPI_BUS_MOSI (&pin_PB10)
#define DEFAULT_SPI_BUS_MISO (&pin_PA12)


#define DEFAULT_UART_BUS_RX (&pin_PA11)
#define DEFAULT_UART_BUS_TX (&pin_PA10)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
