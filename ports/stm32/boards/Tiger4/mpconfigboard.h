#define MICROPY_HW_BOARD_NAME       "Tiger 4"
#define MICROPY_HW_MCU_NAME         "STM32F746NE"

// 1 = use internal flash (512 KByte)
// 0 = use onboard QSPI flash (8 MByte) Winbond W25Q64FV
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)
#define Enable_SPI (1) //!(not in used)
#define Enable_SDRAM (0)

#define MICROPY_HW_HAS_SWITCH       (0)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_ENABLE_SDCARD    (1)  // it has a sd card, but no detect pin

#define MICROPY_BOARD_EARLY_INIT                STM32F746NE_board_early_init
#define MICROPY_SET_CS_PIN_HIGH                 MICROPY_HW_ENABLE_EXTERNAL_FLASH_STORAGE
#define MICROPY_SET_SDCARD_DETECT_PIN_LOW       MICROPY_RESET_SDCARD_DETECT_PIN
#define MICROPY_SET_ETH_PWR_LOW                 MICROPY_RESET_ETH_PWR
#define MICROPY_SET_SDRAM_PWR_LOW               MICROPY_RESET_SDRAM_PWR
void STM32F746NE_board_early_init(void); 
void MICROPY_HW_ENABLE_EXTERNAL_FLASH_STORAGE(void);
void MICROPY_RESET_SDCARD_DETECT_PIN(void);
void MICROPY_RESET_SDRAM_PWR(void);
void MICROPY_RESET_ETH_PWR(void);

// HSE is 25MHz
// VCOClock = HSE * PLLN / PLLM = 25 MHz * 384 / 25 = 384 MHz
// SYSCLK = VCOClock / PLLP = 384 MHz / 2 = 192 MHz
// USB/SDMMC/RNG Clock = VCOClock / PLLQ = 384 MHz / 8 = 48 MHz
// Note: SDRAM requires SYSCLK <= 200MHz
// SYSCLK can be increased to 216MHz if SDRAM is disabled
#define MICROPY_HW_CLK_PLLM (25)
#define MICROPY_HW_CLK_PLLN (400)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (2)

// From the reference manual, for 2.7V to 3.6V
// 151-180 MHz => 5 wait states
// 181-210 MHz => 6 wait states
// 211-216 MHz => 7 wait states
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_6 // 181-210 MHz => 6 wait states

// UART config
#define MICROPY_HW_UART1_NAME       "UART1_"
#define MICROPY_HW_UART1_TX         (pin_A9)
#define MICROPY_HW_UART1_RX         (pin_B7)
#define MICROPY_HW_UART2_NAME       "UART2_" //Ser0
#define MICROPY_HW_UART2_TX         (pyb_pin_UART2_TX)
#define MICROPY_HW_UART2_RX         (pyb_pin_UART2_RX)
// #define MICROPY_HW_UART2_RTS        (pyb_pin_UART2_RTS)
// #define MICROPY_HW_UART2_CTS        (pyb_pin_UART2_CTS)
#define MICROPY_HW_UART6_NAME       "UART6_"
#define MICROPY_HW_UART6_TX         (pin_C6)
#define MICROPY_HW_UART6_RX         (pin_C7)
#define MICROPY_HW_UART7_NAME       "UART7_"
#define MICROPY_HW_UART7_TX         (pin_F6)
#define MICROPY_HW_UART7_RX         (pin_F7)
#define MICROPY_HW_UART_REPL        PYB_UART_2
#define MICROPY_HW_UART_REPL_BAUD   115200

//I2C buses
#define MICROPY_HW_I2C3_NAME        "I2C3_H"
#define MICROPY_HW_I2C3_SCL         (pyb_pin_I2C3_SCL)
#define MICROPY_HW_I2C3_SDA         (pyb_pin_I2C3_SDA)
#define MICROPY_HW_I2C4_NAME        "I2C4_H"
#define MICROPY_HW_I2C4_SCL         (pyb_pin_I2C4_SCL)
#define MICROPY_HW_I2C4_SDA         (pyb_pin_I2C4_SDA)

// SPI buses
#define MICROPY_HW_SPI1_NAME        "SPI1_" //SPI_Flash
#define MICROPY_HW_SPI1_NSS         (pyb_pin_SPI1_CS)
#define MICROPY_HW_SPI1_SCK         (pyb_pin_SPI1_SCK)
#define MICROPY_HW_SPI1_MISO        (pyb_pin_SPI1_MISO)
#define MICROPY_HW_SPI1_MOSI        (pyb_pin_SPI1_MOSI)

#define MICROPY_HW_SPI5_NAME        "SPI5_"
#define MICROPY_HW_SPI5_NSS         (pyb_pin_SPI5_CS)
#define MICROPY_HW_SPI5_SCK         (pyb_pin_SPI5_SCK)
#define MICROPY_HW_SPI5_MISO        (pyb_pin_SPI5_MISO)
#define MICROPY_HW_SPI5_MOSI        (pyb_pin_SPI5_MOSI)

// CAN buses
#define MICROPY_HW_CAN1_NAME        "CAN1_"
#define MICROPY_HW_CAN1_TX          (pin_H13)
#define MICROPY_HW_CAN1_RX          (pin_I9)

// LEDs
#define MICROPY_HW_LED1             (pin_G6) // Red(Type4)
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// SD card
#define MICROPY_HW_SD_NAME                  "SD_"
#define MICROPY_HW_SDCARD_SDMMC             (1)
#define MICROPY_HW_SDCARD_CK                (pyb_pin_SD_CLK)
#define MICROPY_HW_SDCARD_CMD               (pyb_pin_SD_CMD)
#define MICROPY_HW_SDCARD_D0                (pyb_pin_SD_D0)
#define MICROPY_HW_SDCARD_D1                (pyb_pin_SD_D1)
#define MICROPY_HW_SDCARD_D2                (pyb_pin_SD_D2)
#define MICROPY_HW_SDCARD_D3                (pyb_pin_SD_D3)
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_I1) // dummy, no switch at this pin(needs to pull low manually)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)
#define MICROPY_HW_SDCARD_MOUNT_AT_BOOT     (0)

// SPI/QSPI flash size
#define MICROPY_HW_SPIFLASH_SIZE_BITS       (64 * 1024 * 1024)

//QSPI flash
#define MICROPY_HW_QSPI_PRESCALER           (2) // 100MHz
#define MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2 (26)
#define MICROPY_HW_QSPIFLASH_CS             (pyb_pin_QSPI1_CS)
#define MICROPY_HW_QSPIFLASH_SCK            (pyb_pin_QSPI1_CLK)
#define MICROPY_HW_QSPIFLASH_IO0            (pyb_pin_QSPI1_D0)
#define MICROPY_HW_QSPIFLASH_IO1            (pyb_pin_QSPI1_D1)
#define MICROPY_HW_QSPIFLASH_IO2            (pyb_pin_QSPI1_D2)
#define MICROPY_HW_QSPIFLASH_IO3            (pyb_pin_QSPI1_D3)

#if !MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;
#if !BUILDING_MBOOT
#define MICROPY_HW_SPIFLASH_ENABLE_CACHE (1)
#endif
#define MICROPY_HW_BDEV_SPIFLASH    (&spi_bdev)
#define MICROPY_HW_BDEV_SPIFLASH_CONFIG (&spiflash_config)
#define MICROPY_HW_BDEV_SPIFLASH_SIZE_BYTES (MICROPY_HW_SPIFLASH_SIZE_BITS / 8)
#define MICROPY_HW_BDEV_SPIFLASH_EXTENDED (&spi_bdev) // for extended block protocol
#endif

#if !Enable_SPI //!(not in used)
// Winbond W25Q64 SPI Flash = 64 Mbit (2 MByte)
#define MICROPY_HW_SPIFLASH_CS      (pin_A4)
#define MICROPY_HW_SPIFLASH_SCK     (pin_A5)
#define MICROPY_HW_SPIFLASH_MISO    (pin_B4)
#define MICROPY_HW_SPIFLASH_MOSI    (pin_B5)

extern const struct _mp_spiflash_config_t spiflash2_config;
extern struct _spi_bdev_t spi_bdev2;
#endif

// USB config 
#define MICROPY_HW_USB_FS              (1)
#define MICROPY_HW_USB_OTG_ID_PIN      (pin_A10)

// Ethernet via RMII
#define MICROPY_HW_RTH_PHY_RMII (1)
#define MICROPY_HW_ETH_NAME            "ETH_RMII_"
#define MICROPY_HW_ETH_MDC             (pin_C1)
#define MICROPY_HW_ETH_MDIO            (pin_A2)
#define MICROPY_HW_ETH_RMII_REF_CLK    (pin_A1)
#define MICROPY_HW_ETH_RMII_CRS_DV     (pin_A7)
#define MICROPY_HW_ETH_RMII_RXD0       (pin_C4)
#define MICROPY_HW_ETH_RMII_RXD1       (pin_C5)
#define MICROPY_HW_ETH_RMII_TX_EN      (pin_G11)
#define MICROPY_HW_ETH_RMII_TXD0       (pin_G13)
#define MICROPY_HW_ETH_RMII_TXD1       (pin_G14)
//!add SD_NBL0
// SDRAM 
#if !Enable_SDRAM
#define MICROPY_HW_SDRAM_SIZE  (64 / 8 * 1024 * 1024)  // 64 Mbit
#define MICROPY_HW_SDRAM_STARTUP_TEST             (1)
#define MICROPY_HEAP_START              sdram_start()
#define MICROPY_HEAP_END                sdram_end()

// Timing configuration for 100 Mhz (10ns) of SD clock frequency (133Mhz/2) W9864G6KH-6I
// #define MICROPY_HW_SDRAM_TIMING_TMRD        (2) //TRSC
// #define MICROPY_HW_SDRAM_TIMING_TXSR        (8)
// #define MICROPY_HW_SDRAM_TIMING_TRAS        (5)
// #define MICROPY_HW_SDRAM_TIMING_TRC         (7)
// #define MICROPY_HW_SDRAM_TIMING_TWR         (2)//2
// #define MICROPY_HW_SDRAM_TIMING_TRP         (2)
// #define MICROPY_HW_SDRAM_TIMING_TRCD        (2)
// #define MICROPY_HW_SDRAM_REFRESH_RATE       (64) //TREF(ms)
#define MICROPY_HW_SDRAM_TIMING_TMRD        (1) //TRSC
#define MICROPY_HW_SDRAM_TIMING_TXSR        (7)
#define MICROPY_HW_SDRAM_TIMING_TRAS        (4)
#define MICROPY_HW_SDRAM_TIMING_TRC         (6)
#define MICROPY_HW_SDRAM_TIMING_TWR         (1)//2
#define MICROPY_HW_SDRAM_TIMING_TRP         (2)
#define MICROPY_HW_SDRAM_TIMING_TRCD        (2)
#define MICROPY_HW_SDRAM_REFRESH_RATE       (64) //TREF(ms)

#define MICROPY_HW_SDRAM_BURST_LENGTH       1
#define MICROPY_HW_SDRAM_CAS_LATENCY        2
#define MICROPY_HW_SDRAM_COLUMN_BITS_NUM    8
#define MICROPY_HW_SDRAM_ROW_BITS_NUM       12 //13
#define MICROPY_HW_SDRAM_MEM_BUS_WIDTH      16
#define MICROPY_HW_SDRAM_INTERN_BANKS_NUM   4
#define MICROPY_HW_SDRAM_CLOCK_PERIOD       2 //SDRAM common clock
#define MICROPY_HW_SDRAM_RPIPE_DELAY        0 //SDRAM common read pipe delay
#define MICROPY_HW_SDRAM_RBURST             (1)
#define MICROPY_HW_SDRAM_WRITE_PROTECTION   (0)
#define MICROPY_HW_SDRAM_AUTOREFRESH_NUM    (8)

#define MICROPY_HW_FMC_SDCKE0  (pyb_pin_SDRAM_SDCKE0)
#define MICROPY_HW_FMC_SDNE0   (pyb_pin_SDRAM_SDNE0)
#define MICROPY_HW_FMC_SDCLK   (pyb_pin_SDRAM_SDCLK)
#define MICROPY_HW_FMC_SDNCAS  (pyb_pin_SDRAM_SDNCAS)
#define MICROPY_HW_FMC_SDNRAS  (pyb_pin_SDRAM_SDNRAS)
#define MICROPY_HW_FMC_SDNWE   (pyb_pin_SDRAM_SDNWE)
#define MICROPY_HW_FMC_BA0     (pyb_pin_SDRAM_BA0)
#define MICROPY_HW_FMC_BA1     (pyb_pin_SDRAM_BA1)
#define MICROPY_HW_FMC_NBL0    (pyb_pin_SDRAM_NBL0)
#define MICROPY_HW_FMC_NBL1    (pyb_pin_SDRAM_NBL1)
#define MICROPY_HW_FMC_A0      (pyb_pin_SDRAM_A0)
#define MICROPY_HW_FMC_A1      (pyb_pin_SDRAM_A1)
#define MICROPY_HW_FMC_A2      (pyb_pin_SDRAM_A2)
#define MICROPY_HW_FMC_A3      (pyb_pin_SDRAM_A3)
#define MICROPY_HW_FMC_A4      (pyb_pin_SDRAM_A4)
#define MICROPY_HW_FMC_A5      (pyb_pin_SDRAM_A5)
#define MICROPY_HW_FMC_A6      (pyb_pin_SDRAM_A6)
#define MICROPY_HW_FMC_A7      (pyb_pin_SDRAM_A7)
#define MICROPY_HW_FMC_A8      (pyb_pin_SDRAM_A8)
#define MICROPY_HW_FMC_A9      (pyb_pin_SDRAM_A9)
#define MICROPY_HW_FMC_A10     (pyb_pin_SDRAM_A10)
#define MICROPY_HW_FMC_A11     (pyb_pin_SDRAM_A11)
#define MICROPY_HW_FMC_A12     (pyb_pin_SDRAM_A12)//!Not in used
#define MICROPY_HW_FMC_D0      (pyb_pin_SDRAM_D0)
#define MICROPY_HW_FMC_D1      (pyb_pin_SDRAM_D1)
#define MICROPY_HW_FMC_D2      (pyb_pin_SDRAM_D2)
#define MICROPY_HW_FMC_D3      (pyb_pin_SDRAM_D3)
#define MICROPY_HW_FMC_D4      (pyb_pin_SDRAM_D4)
#define MICROPY_HW_FMC_D5      (pyb_pin_SDRAM_D5)
#define MICROPY_HW_FMC_D6      (pyb_pin_SDRAM_D6)
#define MICROPY_HW_FMC_D7      (pyb_pin_SDRAM_D7)
#define MICROPY_HW_FMC_D8      (pyb_pin_SDRAM_D8)
#define MICROPY_HW_FMC_D9      (pyb_pin_SDRAM_D9)
#define MICROPY_HW_FMC_D10     (pyb_pin_SDRAM_D10)
#define MICROPY_HW_FMC_D11     (pyb_pin_SDRAM_D11)
#define MICROPY_HW_FMC_D12     (pyb_pin_SDRAM_D12)
#define MICROPY_HW_FMC_D13     (pyb_pin_SDRAM_D13)
#define MICROPY_HW_FMC_D14     (pyb_pin_SDRAM_D14)
#define MICROPY_HW_FMC_D15     (pyb_pin_SDRAM_D15)
#endif



