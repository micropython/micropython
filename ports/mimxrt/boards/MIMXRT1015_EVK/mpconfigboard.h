#define MICROPY_HW_BOARD_NAME "i.MX RT1015 EVK"
#define MICROPY_HW_MCU_NAME   "MIMXRT1015DAF5A"

// i.MX RT1015 EVK has 3 board LED
// Todo: think about replacing the define with searching in the generated pins?
#define MICROPY_HW_LED1_PIN (pin_GPIO_SD_B1_00)
#define MICROPY_HW_LED2_PIN (pin_GPIO_SD_B1_01)
#define MICROPY_HW_LED3_PIN (pin_GPIO_SD_B1_02)
#define MICROPY_HW_LED_ON(pin) (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin) (mp_hal_pin_high(pin))

#define MICROPY_HW_NUM_PIN_IRQS (3 * 32)

// Define mapping logical UART # to hardware UART #
// RX/TX      HW-UART    Logical UART
// DEBUG USB  LPUART1 -> 0
// D3/D5      LPUART1
// D0/D1      LPUART2 -> 1
// D6/D9      LPUART3 -> 2
// A0/A1      LPUART4 -> 5

#define MICROPY_HW_UART_NUM     (sizeof(uart_index_table) / sizeof(uart_index_table)[0])
#define MICROPY_HW_UART_INDEX   { 1, 4, 3 }

#define IOMUX_TABLE_UART \
    { IOMUXC_GPIO_AD_B0_06_LPUART1_TX }, { IOMUXC_GPIO_AD_B0_07_LPUART1_RX }, \
    { 0 }, { 0 }, \
    { IOMUXC_GPIO_AD_B0_14_LPUART3_TX }, { IOMUXC_GPIO_AD_B0_15_LPUART3_RX }, \
    { IOMUXC_GPIO_EMC_32_LPUART4_TX },   { IOMUXC_GPIO_EMC_33_LPUART4_RX }, \

#define MICROPY_HW_SPI_INDEX { 1 }

#define IOMUX_TABLE_SPI \
    { IOMUXC_GPIO_AD_B0_10_LPSPI1_SCK }, { IOMUXC_GPIO_AD_B0_11_LPSPI1_PCS0 }, \
    { IOMUXC_GPIO_AD_B0_12_LPSPI1_SDO }, { IOMUXC_GPIO_AD_B0_13_LPSPI1_SDI }, \
    { 0 }

#define DMA_REQ_SRC_RX { 0, kDmaRequestMuxLPSPI1Rx, kDmaRequestMuxLPSPI2Rx }

#define DMA_REQ_SRC_TX { 0, kDmaRequestMuxLPSPI1Tx, kDmaRequestMuxLPSPI2Tx }

// Define mapping hardware I2C # to logical I2C #
// SDA/SCL  HW-I2C    Logical I2C
// D14/D15  LPI2C4 ->    0
// A4/A5    LPI2C1 ->    1
// D0/D1    LPI2C2 ->    2

#define MICROPY_HW_I2C_INDEX   { 1, 2 }

#define IOMUX_TABLE_I2C \
    { IOMUXC_GPIO_AD_B1_14_LPI2C1_SCL }, { IOMUXC_GPIO_AD_B1_15_LPI2C1_SDA }, \
    { IOMUXC_GPIO_EMC_19_LPI2C2_SCL },   { IOMUXC_GPIO_EMC_18_LPI2C2_SDA },

#define MICROPY_PY_MACHINE_I2S (1)
#define MICROPY_HW_I2S_NUM (1)
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
        I2S_GPIO(1, MCK, TX, GPIO_EMC_20, IOMUXC_GPIO_EMC_20_SAI1_MCLK), \
        I2S_GPIO(1, SCK, RX, GPIO_EMC_19, IOMUXC_GPIO_EMC_19_SAI1_RX_BCLK), \
        I2S_GPIO(1, WS, RX, GPIO_EMC_18, IOMUXC_GPIO_EMC_18_SAI1_RX_SYNC), \
        I2S_GPIO(1, SD, RX, GPIO_EMC_21, IOMUXC_GPIO_EMC_21_SAI1_RX_DATA00),  \
        I2S_GPIO(1, SCK, TX, GPIO_EMC_26, IOMUXC_GPIO_EMC_26_SAI1_TX_BCLK), \
        I2S_GPIO(1, WS, TX, GPIO_EMC_27, IOMUXC_GPIO_EMC_27_SAI1_TX_SYNC),  \
        I2S_GPIO(1, SD, TX, GPIO_EMC_25, IOMUXC_GPIO_EMC_25_SAI1_TX_DATA00), \
    }
