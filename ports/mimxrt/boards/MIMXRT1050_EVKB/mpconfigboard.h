#define MICROPY_HW_BOARD_NAME "i.MX RT1050 EVKB"
#define MICROPY_HW_MCU_NAME   "MIMXRT1052DVL6B"

#define BOARD_FLASH_SIZE (64 * 1024 * 1024)

// MIMXRT1050_EVKB has 1 user LED
#define MICROPY_HW_LED1_PIN (pin_GPIO_AD_B0_09)
#define MICROPY_HW_LED_ON(pin) (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin) (mp_hal_pin_high(pin))
#define BOARD_FLASH_CONFIG_HEADER_H "evkbmimxrt1050_flexspi_nor_config.h"
#define BOARD_FLASH_OPS_HEADER_H "hal/flexspi_hyper_flash.h"

#define MICROPY_HW_NUM_PIN_IRQS (4 * 32 + 3)

// Define mapping logical UART # to hardware UART #
// LPUART3 on D0/D1  -> 1
// LPUART2 on D7/D6  -> 2
// LPUART6 on D8/D9  -> 3
// LPUART8 on A1/A0  -> 4

#define MICROPY_HW_UART_NUM     (sizeof(uart_index_table) / sizeof(uart_index_table)[0])
#define MICROPY_HW_UART_INDEX   { 0, 3, 2, 6, 8 }

#define IOMUX_TABLE_UART \
    { 0 }, { 0 }, \
    { IOMUXC_GPIO_AD_B1_02_LPUART2_TX }, { IOMUXC_GPIO_AD_B1_03_LPUART2_RX }, \
    { IOMUXC_GPIO_AD_B1_06_LPUART3_TX }, { IOMUXC_GPIO_AD_B1_07_LPUART3_RX }, \
    { 0 }, { 0 }, \
    { 0 }, { 0 }, \
    { IOMUXC_GPIO_AD_B0_02_LPUART6_TX }, { IOMUXC_GPIO_AD_B0_03_LPUART6_RX }, \
    { 0 }, { 0 }, \
    { IOMUXC_GPIO_AD_B1_10_LPUART8_TX }, { IOMUXC_GPIO_AD_B1_11_LPUART8_RX },

#define MICROPY_HW_SPI_INDEX { 1 }

#define IOMUX_TABLE_SPI \
    { IOMUXC_GPIO_SD_B0_00_LPSPI1_SCK }, { IOMUXC_GPIO_SD_B0_01_LPSPI1_PCS0 }, \
    { IOMUXC_GPIO_SD_B0_02_LPSPI1_SDO }, { IOMUXC_GPIO_SD_B0_03_LPSPI1_SDI },

#define DMA_REQ_SRC_RX { 0, kDmaRequestMuxLPSPI1Rx, kDmaRequestMuxLPSPI2Rx, \
                            kDmaRequestMuxLPSPI3Rx, kDmaRequestMuxLPSPI4Rx }

#define DMA_REQ_SRC_TX { 0, kDmaRequestMuxLPSPI1Tx, kDmaRequestMuxLPSPI2Tx, \
                            kDmaRequestMuxLPSPI3Tx, kDmaRequestMuxLPSPI4Tx } 

// Define the mapping hardware I2C # to logical I2C #
// SDA/SCL  HW-I2C    Logical I2C
// D14/D15  LPI2C1 ->    0
// D1/D0    LPI2C3 ->    1

#define MICROPY_HW_I2C_INDEX   { 1, 3 }

#define IOMUX_TABLE_I2C \
    { IOMUXC_GPIO_AD_B1_00_LPI2C1_SCL }, { IOMUXC_GPIO_AD_B1_01_LPI2C1_SDA }, \
    { 0 }, { 0 }, \
    { IOMUXC_GPIO_AD_B1_07_LPI2C3_SCL }, { IOMUXC_GPIO_AD_B1_06_LPI2C3_SDA },

#define USDHC_DUMMY_PIN NULL , 0

#define MICROPY_USDHC1 \
    { \
        .cmd = {GPIO_SD_B0_00_USDHC1_CMD}, \
        .clk = { GPIO_SD_B0_01_USDHC1_CLK }, \
        .cd_b = { GPIO_B1_12_USDHC1_CD_B },\
        .data0 = { GPIO_SD_B0_02_USDHC1_DATA0 },\
        .data1 = { GPIO_SD_B0_03_USDHC1_DATA1 },\
        .data2 = { GPIO_SD_B0_04_USDHC1_DATA2 },\
        .data3 = { GPIO_SD_B0_05_USDHC1_DATA3 },\
    }
