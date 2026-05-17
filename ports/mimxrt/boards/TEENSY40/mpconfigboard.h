#define MICROPY_HW_BOARD_NAME "Teensy 4.0"
#define MICROPY_HW_MCU_NAME   "MIMXRT1062DVJ6A"

// Teensy 4.0 has 1 board LED
#define MICROPY_HW_LED1_PIN (pin_GPIO_B0_03)
#define MICROPY_HW_LED_ON(pin) (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin) (mp_hal_pin_low(pin))

#define MICROPY_HW_NUM_PIN_IRQS (4 * 32 + 3)

// UART config: 7 UARTs at the pins for Teensy 4.0
#define MICROPY_HW_UART_NUM     (sizeof(uart_index_table) / sizeof(uart_index_table)[0])
#define MICROPY_HW_UART_INDEX   { 0, 6, 4, 2, 3, 8, 1, 7 }

#define IOMUX_TABLE_UART \
    { IOMUXC_GPIO_AD_B0_12_LPUART1_TX }, { IOMUXC_GPIO_AD_B0_13_LPUART1_RX }, \
    { IOMUXC_GPIO_AD_B1_02_LPUART2_TX }, { IOMUXC_GPIO_AD_B1_03_LPUART2_RX }, \
    { IOMUXC_GPIO_AD_B1_06_LPUART3_TX }, { IOMUXC_GPIO_AD_B1_07_LPUART3_RX }, \
    { IOMUXC_GPIO_B1_00_LPUART4_TX }, { IOMUXC_GPIO_B1_01_LPUART4_RX }, \
    { 0 }, { 0 }, \
    { IOMUXC_GPIO_AD_B0_02_LPUART6_TX }, { IOMUXC_GPIO_AD_B0_03_LPUART6_RX }, \
    { IOMUXC_GPIO_EMC_31_LPUART7_TX }, { IOMUXC_GPIO_EMC_32_LPUART7_RX }, \
    { IOMUXC_GPIO_AD_B1_10_LPUART8_TX }, { IOMUXC_GPIO_AD_B1_11_LPUART8_RX },

#define IOMUX_TABLE_UART_CTS_RTS \
    { IOMUXC_GPIO_AD_B0_14_LPUART1_CTS_B }, { IOMUXC_GPIO_AD_B0_15_LPUART1_RTS_B }, \
    { IOMUXC_GPIO_AD_B1_00_LPUART2_CTS_B }, { IOMUXC_GPIO_AD_B1_01_LPUART2_RTS_B }, \
    { IOMUXC_GPIO_AD_B1_04_LPUART3_CTS_B }, { IOMUXC_GPIO_AD_B1_05_LPUART3_RTS_B }, \
    { IOMUXC_GPIO_EMC_17_LPUART4_CTS_B }, { IOMUXC_GPIO_EMC_18_LPUART4_RTS_B }, \
    { 0 }, { 0 }, \
    { IOMUXC_GPIO_EMC_30_LPUART6_CTS_B }, { IOMUXC_GPIO_EMC_29_LPUART6_RTS_B }, \
    { IOMUXC_GPIO_SD_B1_06_LPUART7_CTS_B }, { IOMUXC_GPIO_SD_B1_07_LPUART7_RTS_B }, \
    { IOMUXC_GPIO_SD_B0_02_LPUART8_CTS_B }, { IOMUXC_GPIO_SD_B0_03_LPUART8_RTS_B },

#define MICROPY_HW_SPI_INDEX { 4, 3}

#define IOMUX_TABLE_SPI \
    { 0 }, { 0 }, \
    { 0 }, { 0 }, \
    { 0 }, \
    { 0 }, { 0 }, \
    { 0 }, { 0 }, \
    { 0 }, \
    { IOMUXC_GPIO_AD_B1_15_LPSPI3_SCK }, { IOMUXC_GPIO_AD_B0_03_LPSPI3_PCS0 }, \
    { IOMUXC_GPIO_AD_B1_14_LPSPI3_SDO }, { IOMUXC_GPIO_AD_B0_02_LPSPI3_SDI }, \
    { 0 }, \
    { IOMUXC_GPIO_B0_03_LPSPI4_SCK }, { IOMUXC_GPIO_B0_00_LPSPI4_PCS0 }, \
    { IOMUXC_GPIO_B0_02_LPSPI4_SDO }, { IOMUXC_GPIO_B0_01_LPSPI4_SDI }, \
    { 0 },

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

#define MICROPY_PY_MACHINE_I2S (1)
#define MICROPY_HW_I2S_NUM (2)
#define I2S_CLOCK_MUX { 0, kCLOCK_Sai1Mux, kCLOCK_Sai2Mux }
#define I2S_CLOCK_PRE_DIV { 0, kCLOCK_Sai1PreDiv, kCLOCK_Sai2PreDiv }
#define I2S_CLOCK_DIV { 0, kCLOCK_Sai1Div, kCLOCK_Sai2Div }
#define I2S_IOMUXC_GPR_MODE { 0, kIOMUXC_GPR_SAI1MClkOutputDir, kIOMUXC_GPR_SAI2MClkOutputDir }
#define I2S_DMA_REQ_SRC_RX { 0, kDmaRequestMuxSai1Rx, kDmaRequestMuxSai2Rx }
#define I2S_DMA_REQ_SRC_TX { 0, kDmaRequestMuxSai1Tx, kDmaRequestMuxSai2Tx }
#define I2S_AUDIO_PLL_CLOCK (2U)

#define I2S_GPIO(_hwid, _fn, _mode, _pin, _iomux) \
    { \
        .hw_id = _hwid, \
        .fn = _fn, \
        .mode = _mode, \
        .name = MP_QSTR_##_pin, \
        .iomux = {_iomux}, \
    }

#define I2S_GPIO_MAP \
    { \
        I2S_GPIO(1, SCK, RX, GPIO_AD_B1_11, IOMUXC_GPIO_AD_B1_11_SAI1_RX_BCLK), /* pin 21 */ \
        I2S_GPIO(1, WS, RX, GPIO_AD_B1_10, IOMUXC_GPIO_AD_B1_10_SAI1_RX_SYNC), /* pin 20 */ \
        I2S_GPIO(1, SD, RX, GPIO_B1_00, IOMUXC_GPIO_B1_00_SAI1_RX_DATA00), /* pin 8 */ \
        I2S_GPIO(1, SCK, TX, GPIO_AD_B1_14, IOMUXC_GPIO_AD_B1_14_SAI1_TX_BCLK), /* pin 26 */ \
        I2S_GPIO(1, SCK, TX, GPIO_B1_02, IOMUXC_GPIO_B1_02_SAI1_TX_BCLK), /* pin 36 */ \
        I2S_GPIO(1, WS, TX, GPIO_AD_B1_15, IOMUXC_GPIO_AD_B1_15_SAI1_TX_SYNC), /* pin 27 */ \
        I2S_GPIO(1, WS, TX, GPIO_B1_03, IOMUXC_GPIO_B1_03_SAI1_TX_SYNC), /* pin 37 */ \
        I2S_GPIO(1, SD, TX, GPIO_B1_01, IOMUXC_GPIO_B1_01_SAI1_TX_DATA00), /* pin 7 */ \
        I2S_GPIO(1, MCK, TX, GPIO_AD_B1_09, IOMUXC_GPIO_AD_B1_09_SAI1_MCLK), /* pin 23 */ \
        I2S_GPIO(2, SCK, TX, GPIO_EMC_06, IOMUXC_GPIO_EMC_06_SAI2_TX_BCLK), /* pin 4 */ \
        I2S_GPIO(2, WS, TX, GPIO_EMC_05, IOMUXC_GPIO_EMC_05_SAI2_TX_SYNC), /* pin 3 */ \
        I2S_GPIO(2, SD, TX, GPIO_EMC_04, IOMUXC_GPIO_EMC_04_SAI2_TX_DATA), /* pin 2 */ \
        I2S_GPIO(2, MCK, TX, GPIO_EMC_07, IOMUXC_GPIO_EMC_07_SAI2_MCLK) /* pin 33 */ \
    }

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
