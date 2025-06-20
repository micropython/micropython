/*
	File defining the board and the mcu name, clock and memory configuration and which peripherals will be compiled. Some peripherals require a specific "enable" define, others just a specific "pin" define. All constants starting with "pin_" are defined during compilation using the names defined in "pins.csv" + this prefix.
	
	Author: GabSi
	Year: 2022
*/

#define MICROPY_HW_BOARD_NAME       "Tiger_Plus" //Board Name
#define MICROPY_HW_MCU_NAME         "STM32F746" //MCU name

#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0) //in order to use the external flash
#define MICROPY_HW_HAS_SWITCH       (0)  //compiless the switch driver *
#define MICROPY_HW_HAS_FLASH        (0)  //compiles the flash driver **
#define MICROPY_HW_ENABLE_RNG       (1)  //compiles the random number generator
#define MICROPY_HW_ENABLE_RTC       (1)  //compiles the RTC
#define MICROPY_HW_ENABLE_USB       (0)  //compiles the USB driver
#define MICROPY_HW_ENABLE_SDCARD    (0)  //compiles the SD Card driver
#define MICROPY_HW_HAS_LCD          (1)  //compiles the LCD driver
#define MICROPY_HW_HAS_XOR			(1)

//*the switch driver is simply a gpio driver for a switch where the user can read it values and set a callback function to it. TODO: Eliminate it and make it an internal driver for the PC to Run mode
//**the flash driver was not tested yet, check on: https://docs.micropython.org/en/latest/library/pyb.Flash.html, but most likely we will substitute it

//CLOCK DEFINITIONS
// HSE is 25MHz (defined in stm32f7xx_hal_conf.h)
// VCOClock = HSE * PLLN / PLLM = 25 MHz * 384 / 25 = 384 MHz
// SYSCLK = VCOClock / PLLP = 384 MHz / 2 = 192 MHz
// USB/SDMMC/RNG Clock = VCOClock / PLLQ = 384 MHz / 8 = 48 MHz
// Note: SDRAM requires SYSCLK <= 200MHz
// SYSCLK can be increased to 216MHz if SDRAM is disabled
#define MICROPY_HW_CLK_PLLM (20)
#define MICROPY_HW_CLK_PLLN (192)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (5)

// From the reference manual, for 2.7V to 3.6V
//   0-30 MHz  => 0 wait states
//  31-60 MHz  => 1 wait states
//  61-90 MHz  => 2 wait states
//  91-120 MHz => 3 wait states
// 121-150 MHz => 4 wait states
// 151-180 MHz => 4 wait states
// 181-210 MHz => 6 wait states
// 211-216 MHz => 7 wait states
/*
	For Tiger Plus consult STM32F75xxx and STM32F74xxx(RM0385), 
	chapter Flash, Table "Number of wait states according to CPU clock (HCLK) frequency"
*/
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_3 

// REPL config
//TODO: Later in main we need to add an interrupt for the reset switch to actually turn this off and make it available for our future serial drivers
#define MICROPY_HW_UART3_TX         (pin_C10)
#define MICROPY_HW_UART3_RX         (pin_C11)
#define MICROPY_HW_UART_REPL        PYB_UART_3
#define MICROPY_HW_UART_REPL_BAUD   115200
/*
//UART
#define MICROPY_HW_UART2_NAME       "UART"
#define MICROPY_HW_UART2_TX         (pyb_pin_UART_TX)
#define MICROPY_HW_UART2_RX         (pyb_pin_UART_RX)
#define MICROPY_HW_UART2_RTS        (pin_D4)
#define MICROPY_HW_UART2_CTS        (pin_D3)

//I2C
#define MICROPY_HW_I2C4_NAME        "I2C"
#define MICROPY_HW_I2C4_SCL         (pin_H11)
#define MICROPY_HW_I2C4_SDA         (pin_H12)

//SPI
#define MICROPY_HW_SPI2_NAME        "SPI"
#define MICROPY_HW_SPI2_NSS         (pyb_pin_SPI_CS)
#define MICROPY_HW_SPI2_SCK         (pyb_pin_SPI_CLK)
#define MICROPY_HW_SPI2_MISO        (pyb_pin_SPI_MISO)
#define MICROPY_HW_SPI2_MOSI        (pyb_pin_SPI_MOSI)

// CAN buses
#define MICROPY_HW_CAN1_NAME        "CAN"
#define MICROPY_HW_CAN1_TX          (pin_A11)
#define MICROPY_HW_CAN1_RX          (pin_A12)
*/

/*
	The Pins used here are defined by make_pins.py in ports/stm32/boards. These file takes the definitions in pins.csv and add a "pyb_" prefix to it. TODO: Changed it in make_pins AND all the other drivers by "tiger_".
*/

//SPI Flash to be used in python
//TODO: Remove it from here and write a private driver to be used only by our firmware. That is even more simple than write a Micropython module. We could include the .h file in other drivers that would communicate with the flash, like our Tiger Firmware does. The add1ms function in timer.c is a good example
#define MICROPY_HW_SPI6_NAME        "FLASH"
#define MICROPY_HW_SPI6_NSS         (pyb_pin_SPI_FLASH_CS)
#define MICROPY_HW_SPI6_SCK         (pyb_pin_SPI_FLASH_SCK)
#define MICROPY_HW_SPI6_MISO        (pyb_pin_SPI_FLASH_MISO)
#define MICROPY_HW_SPI6_MOSI        (pyb_pin_SPI_FLASH_MOSI)

#define MICROPY_HW_SPIFLASH_SIZE_BITS (64 * 1024 * 1024)

extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;

#define MICROPY_HW_SPIFLASH_ENABLE_CACHE (1)
#define MICROPY_HW_BDEV_SPIFLASH    (&spi_bdev)
#define MICROPY_HW_BDEV_SPIFLASH_CONFIG (&spiflash_config)
#define MICROPY_HW_BDEV_SPIFLASH_SIZE_BYTES (MICROPY_HW_SPIFLASH_SIZE_BITS / 8)
#define MICROPY_HW_BDEV_SPIFLASH_EXTENDED (&spi_bdev) // for extended block protocol

#define MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2 (26)
#define MICROPY_HW_QSPIFLASH_CS             (pyb_pin_QSPI_CS) 
#define MICROPY_HW_QSPIFLASH_SCK            (pyb_pin_QSPI_CLK)
#define MICROPY_HW_QSPIFLASH_IO0            (pyb_pin_QSPI_D0)
#define MICROPY_HW_QSPIFLASH_IO1            (pyb_pin_QSPI_D1)
#define MICROPY_HW_QSPIFLASH_IO2            (pyb_pin_QSPI_D2)
#define MICROPY_HW_QSPIFLASH_IO3            (pyb_pin_QSPI_D3)

// SDRAM
#define MICROPY_HW_SDRAM_SIZE  					(64 / 8 * 1024 * 1024)  // 64 Mbit
#define MICROPY_HW_SDRAM_STARTUP_TEST           (1)
#define MICROPY_HEAP_START              		sdram_start()
#define MICROPY_HEAP_END                		sdram_end()

// Timing configuration for 90 Mhz (11.90ns) of SD clock frequency (180Mhz/2)
#define MICROPY_HW_SDRAM_TIMING_TMRD        (1) //TMRD  -> 1 = 2 Cycles
#define MICROPY_HW_SDRAM_TIMING_TXSR        (7) //TXSR -> 7 = 8 Cyvle
#define MICROPY_HW_SDRAM_TIMING_TRAS        (4) //TRAS -> 4 = 5 Cycles
#define MICROPY_HW_SDRAM_TIMING_TRC         (6) //TRC - > 6 = 7 Cycles
#define MICROPY_HW_SDRAM_TIMING_TWR         (1) //TWR -> 1 = 2 Cycles
#define MICROPY_HW_SDRAM_TIMING_TRP         (2) //TRP -> 2 = 3 Cycles
#define MICROPY_HW_SDRAM_TIMING_TRCD        (2) //TRCD -> 2 = 3 Cycles
#define MICROPY_HW_SDRAM_REFRESH_RATE       (64) // ms REF_PER

#define MICROPY_HW_SDRAM_BURST_LENGTH       1
#define MICROPY_HW_SDRAM_CAS_LATENCY        2     //CAS -> 2 = 2 Cycles
#define MICROPY_HW_SDRAM_COLUMN_BITS_NUM    8     //NC -> 0 = 8 Bits
#define MICROPY_HW_SDRAM_ROW_BITS_NUM       12    //NR -> 1 = 12 Bits
#define MICROPY_HW_SDRAM_MEM_BUS_WIDTH      16    //MWID -> 1 = 16 Bits
#define MICROPY_HW_SDRAM_INTERN_BANKS_NUM   4     //NB -> 1 = 4 Banks
#define MICROPY_HW_SDRAM_CLOCK_PERIOD       2     //SDCLK -> 2 = Two HCLK clock cycle delay
#define MICROPY_HW_SDRAM_RPIPE_DELAY        0     //RPIPE -> 0 = No HCLK clock cycle delay
#define MICROPY_HW_SDRAM_RBURST            (1)    //RBURST -> 1 = single read requests are always managed as bursts
#define MICROPY_HW_SDRAM_WRITE_PROTECTION   (0)   //WP -> 0 = Write accesses allowed
#define MICROPY_HW_SDRAM_AUTOREFRESH_NUM    (8)

#define MICROPY_HW_FMC_SDCKE0   (pin_H2)  //ok
#define MICROPY_HW_FMC_SDNE0    (pin_H3)  //ok
#define MICROPY_HW_FMC_SDCLK    (pin_G8)  //ok
#define MICROPY_HW_FMC_SDNCAS   (pin_G15) //ok
#define MICROPY_HW_FMC_SDNRAS   (pin_F11) //ok
#define MICROPY_HW_FMC_SDNWE    (pin_H5)  //ok
#define MICROPY_HW_FMC_BA0      (pin_G4)  //ok
#define MICROPY_HW_FMC_BA1      (pin_G5)  //ok
#define MICROPY_HW_FMC_NBL0     (pin_E0)  //ok
#define MICROPY_HW_FMC_NBL1     (pin_E1)  //ok
#define MICROPY_HW_FMC_A0       (pin_F0)  //ok
#define MICROPY_HW_FMC_A1       (pin_F1)  //ok
#define MICROPY_HW_FMC_A2       (pin_F2)  //ok
#define MICROPY_HW_FMC_A3       (pin_F3)  //ok
#define MICROPY_HW_FMC_A4       (pin_F4)  //ok
#define MICROPY_HW_FMC_A5       (pin_F5)  //ok
#define MICROPY_HW_FMC_A6       (pin_F12) //ok
#define MICROPY_HW_FMC_A7       (pin_F13) //ok
#define MICROPY_HW_FMC_A8       (pin_F14) //ok
#define MICROPY_HW_FMC_A9       (pin_F15) //ok
#define MICROPY_HW_FMC_A10      (pin_G0)  //ok
#define MICROPY_HW_FMC_A11      (pin_G1)  //ok
#define MICROPY_HW_FMC_A12      (pin_G2)  //ok
#define MICROPY_HW_FMC_D0       (pin_D14) //ok
#define MICROPY_HW_FMC_D1       (pin_D15) //ok
#define MICROPY_HW_FMC_D2       (pin_D0)  //ok
#define MICROPY_HW_FMC_D3       (pin_D1)  //ok
#define MICROPY_HW_FMC_D4       (pin_E7)  //ok
#define MICROPY_HW_FMC_D5       (pin_E8)  //ok
#define MICROPY_HW_FMC_D6       (pin_E9)  //ok
#define MICROPY_HW_FMC_D7       (pin_E10) //ok
#define MICROPY_HW_FMC_D8       (pin_E11) //ok
#define MICROPY_HW_FMC_D9       (pin_E12) //ok
#define MICROPY_HW_FMC_D10      (pin_E13) //ok
#define MICROPY_HW_FMC_D11      (pin_E14) //ok
#define MICROPY_HW_FMC_D12      (pin_E15) //ok
#define MICROPY_HW_FMC_D13      (pin_D8)  //ok
#define MICROPY_HW_FMC_D14      (pin_D9)  //ok
#define MICROPY_HW_FMC_D15      (pin_D10) //ok

