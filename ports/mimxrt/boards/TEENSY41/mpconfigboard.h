#define MICROPY_HW_BOARD_NAME "Teensy 4.1"
#define MICROPY_HW_MCU_NAME   "MIMXRT1062DVJ6A"

// Teensy 4.1 has 1 board LED
#define MICROPY_HW_LED1_PIN (pin_GPIO_B0_03)
#define MICROPY_HW_LED_ON(pin) (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin) (mp_hal_pin_low(pin))

#define MICROPY_HW_NUM_PIN_IRQS (4 * 32 + 3)

// UART config: 8 UARTs at the pins for Teensy 4.1
#define MICROPY_HW_UART_NUM     (sizeof(uart_index_table) / sizeof(uart_index_table)[0])
#define MICROPY_HW_UART_INDEX   { 0, 6, 4, 2, 3, 8, 1, 7, 5 }

#define IOMUX_TABLE_UART \
    { IOMUXC_GPIO_AD_B0_12_LPUART1_TX }, { IOMUXC_GPIO_AD_B0_13_LPUART1_RX }, \
    { IOMUXC_GPIO_AD_B1_02_LPUART2_TX }, { IOMUXC_GPIO_AD_B1_03_LPUART2_RX }, \
    { IOMUXC_GPIO_AD_B1_06_LPUART3_TX }, { IOMUXC_GPIO_AD_B1_07_LPUART3_RX }, \
    { IOMUXC_GPIO_B1_00_LPUART4_TX }, { IOMUXC_GPIO_B1_01_LPUART4_RX }, \
    { IOMUXC_GPIO_B1_12_LPUART5_TX }, { IOMUXC_GPIO_B1_13_LPUART5_RX }, \
    { IOMUXC_GPIO_AD_B0_02_LPUART6_TX }, { IOMUXC_GPIO_AD_B0_03_LPUART6_RX }, \
    { IOMUXC_GPIO_EMC_31_LPUART7_TX }, { IOMUXC_GPIO_EMC_32_LPUART7_RX }, \
    { IOMUXC_GPIO_AD_B1_10_LPUART8_TX }, { IOMUXC_GPIO_AD_B1_11_LPUART8_RX },

#define MICROPY_HW_SPI_INDEX { 4, 3, 1 }

#define IOMUX_TABLE_SPI \
    { IOMUXC_GPIO_SD_B0_00_LPSPI1_SCK }, { IOMUXC_GPIO_SD_B0_01_LPSPI1_PCS0 }, \
    { IOMUXC_GPIO_SD_B0_02_LPSPI1_SDO }, { IOMUXC_GPIO_SD_B0_03_LPSPI1_SDI }, \
    { 0 }, { 0 }, \
    { 0 }, { 0 }, \
    { IOMUXC_GPIO_AD_B1_15_LPSPI3_SCK }, { IOMUXC_GPIO_AD_B1_12_LPSPI3_PCS0 }, \
    { IOMUXC_GPIO_AD_B1_14_LPSPI3_SDO }, { IOMUXC_GPIO_AD_B1_13_LPSPI3_SDI }, \
    { IOMUXC_GPIO_B0_03_LPSPI4_SCK }, { IOMUXC_GPIO_B0_00_LPSPI4_PCS0 }, \
    { IOMUXC_GPIO_B0_02_LPSPI4_SDO }, { IOMUXC_GPIO_B0_01_LPSPI4_SDI },

#define DMA_REQ_SRC_RX { 0, kDmaRequestMuxLPSPI1Rx, kDmaRequestMuxLPSPI2Rx, \
                         kDmaRequestMuxLPSPI3Rx, kDmaRequestMuxLPSPI4Rx }

#define DMA_REQ_SRC_TX { 0, kDmaRequestMuxLPSPI1Tx, kDmaRequestMuxLPSPI2Tx, \
                         kDmaRequestMuxLPSPI3Tx, kDmaRequestMuxLPSPI4Tx }

// Define mapping hardware I2C # to logical I2C #
// SDA/SCL  HW-I2C    Logical I2C
// 17/16    LPI2C3 ->    0
// 18/19    LPI2C1 ->    1
// 25/24    LPI2C4 ->    2

#define MICROPY_HW_I2C_INDEX   { 1, 3, 4 }

#define IOMUX_TABLE_I2C \
    { IOMUXC_GPIO_AD_B1_00_LPI2C1_SCL }, { IOMUXC_GPIO_AD_B1_01_LPI2C1_SDA }, \
    { 0 }, { 0 }, \
    { IOMUXC_GPIO_AD_B1_07_LPI2C3_SCL }, { IOMUXC_GPIO_AD_B1_06_LPI2C3_SDA }, \
    { IOMUXC_GPIO_AD_B0_12_LPI2C4_SCL }, { IOMUXC_GPIO_AD_B0_13_LPI2C4_SDA },

#define USDHC_DUMMY_PIN NULL, 0
#define MICROPY_USDHC1 \
    { \
        .cmd = {GPIO_SD_B0_00_USDHC1_CMD}, \
        .clk = { GPIO_SD_B0_01_USDHC1_CLK }, \
        .cd_b = { USDHC_DUMMY_PIN }, \
        .data0 = { GPIO_SD_B0_02_USDHC1_DATA0 }, \
        .data1 = { GPIO_SD_B0_03_USDHC1_DATA1 }, \
        .data2 = { GPIO_SD_B0_04_USDHC1_DATA2 }, \
        .data3 = { GPIO_SD_B0_05_USDHC1_DATA3 }, \
    }
