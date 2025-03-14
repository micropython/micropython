#define MICROPY_HW_BOARD_NAME "iMX_RT1011_Nano_Kit"
#define MICROPY_HW_MCU_NAME   "MIMXRT1011DAE5A"
#define MICROPY_HW_USB_MANUFACTURER_STRING "Makerdiary"
#define MICROPY_HW_USB_VID 0xf055
#define MICROPY_HW_USB_PID 0x9802
#define MICROPY_PY_OS_DUPTERM_BUILTIN_STREAM (0)

// RT1011-Nanokit has 1 board LED
#define MICROPY_HW_LED1_PIN (pin_GPIO_SD_04)
#define MICROPY_HW_LED_ON(pin) (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin) (mp_hal_pin_low(pin))

#define MICROPY_HW_NUM_PIN_IRQS (2 * 32)
#define MICROPY_PY_MACHINE_SDCARD    (0)


#define MICROPY_HW_UART_NUM     (sizeof(uart_index_table) / sizeof(uart_index_table)[0])
#define MICROPY_HW_UART_INDEX   { 0, 1, 2, 3, 4 }

#define IOMUX_TABLE_UART \
    { IOMUXC_GPIO_10_LPUART1_TXD }, { IOMUXC_GPIO_09_LPUART1_RXD }, \
    { IOMUXC_GPIO_AD_00_LPUART2_TXD }, { IOMUXC_GPIO_13_LPUART2_RXD }, \
    { IOMUXC_GPIO_12_LPUART3_TXD }, { IOMUXC_GPIO_11_LPUART3_RXD }, \
    { IOMUXC_GPIO_AD_02_LPUART4_TXD }, { IOMUXC_GPIO_AD_01_LPUART4_RXD },

#define IOMUX_TABLE_UART_CTS_RTS \
    { IOMUXC_GPIO_08_LPUART1_CTS_B }, { IOMUXC_GPIO_07_LPUART1_RTS_B }, \
    { IOMUXC_GPIO_AD_08_LPUART2_CTS_B }, { IOMUXC_GPIO_AD_07_LPUART2_RTS_B }, \
    { IOMUXC_GPIO_AD_14_LPUART3_CTS_B }, { IOMUXC_GPIO_AD_13_LPUART3_RTS_B }, \
    { IOMUXC_GPIO_AD_14_LPUART4_CTS_B }, { IOMUXC_GPIO_AD_13_LPUART4_RTS_B },

#define MICROPY_HW_SPI_INDEX { 1, 2 }

#define IOMUX_TABLE_SPI \
    { IOMUXC_GPIO_AD_06_LPSPI1_SCK }, { IOMUXC_GPIO_AD_05_LPSPI1_PCS0 }, \
    { IOMUXC_GPIO_AD_04_LPSPI1_SDO }, { IOMUXC_GPIO_AD_03_LPSPI1_SDI }, \
    { IOMUXC_GPIO_AD_02_LPSPI1_PCS1 }, \
    { IOMUXC_GPIO_AD_12_LPSPI2_SCK }, { IOMUXC_GPIO_AD_11_LPSPI2_PCS0 }, \
    { IOMUXC_GPIO_AD_10_LPSPI2_SDO }, { IOMUXC_GPIO_AD_09_LPSPI2_SDI }, \
    { IOMUXC_GPIO_AD_01_LPSPI2_PCS1 }

#define DMA_REQ_SRC_RX { 0, kDmaRequestMuxLPSPI1Rx, kDmaRequestMuxLPSPI2Rx }
#define DMA_REQ_SRC_TX { 0, kDmaRequestMuxLPSPI1Tx, kDmaRequestMuxLPSPI2Tx }

// Define mapping hardware I2C # to logical I2C #
// SDA/SCL    HW-I2C    Logical I2C
// SDA1/SCL1  LPI2C1 ->    0
// SDA2/SCL2  LPI2C2 ->    1

#define MICROPY_HW_I2C_INDEX   { 1, 2 }

#define IOMUX_TABLE_I2C \
    { IOMUXC_GPIO_02_LPI2C1_SCL }, { IOMUXC_GPIO_01_LPI2C1_SDA }, \
    { IOMUXC_GPIO_AD_08_LPI2C2_SCL }, { IOMUXC_GPIO_AD_07_LPI2C2_SDA },

#define MICROPY_PY_MACHINE_I2S (1)
#define MICROPY_HW_I2S_NUM (3)
#define I2S_CLOCK_MUX { 0, kCLOCK_Sai1Mux, 0, kCLOCK_Sai3Mux }
#define I2S_CLOCK_PRE_DIV { 0, kCLOCK_Sai1PreDiv, 0, kCLOCK_Sai3PreDiv }
#define I2S_CLOCK_DIV { 0, kCLOCK_Sai1Div, 0, kCLOCK_Sai3Div }
#define I2S_IOMUXC_GPR_MODE { 0, kIOMUXC_GPR_SAI1MClkOutputDir, 0, kIOMUXC_GPR_SAI3MClkOutputDir }
#define I2S_DMA_REQ_SRC_RX { 0, kDmaRequestMuxSai1Rx, 0, kDmaRequestMuxSai3Rx }
#define I2S_DMA_REQ_SRC_TX { 0, kDmaRequestMuxSai1Tx, 0, kDmaRequestMuxSai3Tx }
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
        I2S_GPIO(1, MCK, TX, GPIO_08, IOMUXC_GPIO_08_SAI1_MCLK), /* pin D8 */ \
        I2S_GPIO(1, SCK, RX, GPIO_01, IOMUXC_GPIO_01_SAI1_RX_BCLK), /* pin D1 */ \
        I2S_GPIO(1, WS, RX, GPIO_02, IOMUXC_GPIO_02_SAI1_RX_SYNC), /* pin D2 */ \
        I2S_GPIO(1, SD, RX, GPIO_03, IOMUXC_GPIO_03_SAI1_RX_DATA00), /* pin D3 */ \
        I2S_GPIO(1, SCK, TX, GPIO_06, IOMUXC_GPIO_06_SAI1_TX_BCLK), /* pin D6 */ \
        I2S_GPIO(1, WS, TX, GPIO_07, IOMUXC_GPIO_07_SAI1_TX_SYNC), /* pin D7 */ \
        I2S_GPIO(1, SD, TX, GPIO_04, IOMUXC_GPIO_04_SAI1_TX_DATA00), /* pin D4 */ \
        I2S_GPIO(3, SCK, TX, GPIO_SD_01, IOMUXC_GPIO_SD_01_SAI3_TX_BCLK), /* pin SD1 */ \
        I2S_GPIO(3, WS, TX, GPIO_SD_00, IOMUXC_GPIO_SD_00_SAI3_TX_SYNC), /* pin SD0 */ \
        I2S_GPIO(3, SD, TX, GPIO_SD_02, IOMUXC_GPIO_SD_02_SAI3_TX_DATA) /* pin SD2 */ \
    }
