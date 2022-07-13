#define MICROPY_HW_BOARD_NAME "i.MX RT1010 EVK"
#define MICROPY_HW_MCU_NAME   "MIMXRT1011DAE5A"

// i.MX RT1010 EVK has 1 board LED
#define MICROPY_HW_LED1_PIN (pin_GPIO_11)
#define MICROPY_HW_LED_ON(pin) (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin) (mp_hal_pin_low(pin))

#define MICROPY_HW_NUM_PIN_IRQS (2 * 32)

// Define mapping logical UART # to hardware UART #
// LPUART1 on USB_DBG  -> 0
// LPUART1 on D0/D1    -> 1
// LPUART3 on A0/D4    -> 3
// LPUART4 on D6/D7    -> 2

#define MICROPY_HW_UART_NUM     (sizeof(uart_index_table) / sizeof(uart_index_table)[0])
#define MICROPY_HW_UART_INDEX   { 1, 1, 4, 3 }

#define IOMUX_TABLE_UART \
    { IOMUXC_GPIO_10_LPUART1_TXD }, { IOMUXC_GPIO_09_LPUART1_RXD }, \
    { 0 }, { 0 }, \
    { 0 }, { 0 }, \
    { IOMUXC_GPIO_AD_02_LPUART4_TXD }, { IOMUXC_GPIO_AD_01_LPUART4_RXD },

#define MICROPY_HW_SPI_INDEX { 1 }

#define IOMUX_TABLE_SPI \
    { IOMUXC_GPIO_AD_06_LPSPI1_SCK }, { IOMUXC_GPIO_AD_05_LPSPI1_PCS0 }, \
    { IOMUXC_GPIO_AD_04_LPSPI1_SDO }, { IOMUXC_GPIO_AD_03_LPSPI1_SDI }, \
    { IOMUXC_GPIO_AD_02_LPSPI1_PCS1 }

#define DMA_REQ_SRC_RX { 0, kDmaRequestMuxLPSPI1Rx, kDmaRequestMuxLPSPI2Rx }
#define DMA_REQ_SRC_TX { 0, kDmaRequestMuxLPSPI1Tx, kDmaRequestMuxLPSPI2Tx }

// Define mapping hardware I2C # to logical I2C #
// SDA/SCL  HW-I2C    Logical I2C
// D14/D15  LPI2C1 ->    0
// D0/D1    LPI2C2 ->    1
// D6/D7    LPI2C2 ->    1   Alternatively possible GPIO_AD_01, GPIO_AD_02

#define MICROPY_HW_I2C_INDEX   { 1, 2 }

#define IOMUX_TABLE_I2C \
    { IOMUXC_GPIO_02_LPI2C1_SCL }, { IOMUXC_GPIO_01_LPI2C1_SDA }, \
    { IOMUXC_GPIO_10_LPI2C2_SCL }, { IOMUXC_GPIO_09_LPI2C2_SDA },

#define MICROPY_PY_MACHINE_I2S (1)
#define MICROPY_HW_I2S_NUM (1)
#define I2S_CLOCK_MUX { 0, kCLOCK_Sai1Mux }
#define I2S_CLOCK_PRE_DIV { 0, kCLOCK_Sai1PreDiv }
#define I2S_CLOCK_DIV { 0, kCLOCK_Sai1Div }
#define I2S_IOMUXC_GPR_MODE { 0, kIOMUXC_GPR_SAI1MClkOutputDir }
#define I2S_DMA_REQ_SRC_RX { 0, kDmaRequestMuxSai1Rx }
#define I2S_DMA_REQ_SRC_TX { 0, kDmaRequestMuxSai1Tx }
#define I2S_WM8960_RX_MODE  (1)

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
        I2S_GPIO(1, MCK, TX, GPIO_08, IOMUXC_GPIO_08_SAI1_MCLK),  \
        I2S_GPIO(1, SCK, RX, GPIO_01, IOMUXC_GPIO_01_SAI1_RX_BCLK),  \
        I2S_GPIO(1, WS, RX, GPIO_02, IOMUXC_GPIO_02_SAI1_RX_SYNC),  \
        I2S_GPIO(1, SD, RX, GPIO_03, IOMUXC_GPIO_03_SAI1_RX_DATA00),  \
        I2S_GPIO(1, SCK, TX, GPIO_06, IOMUXC_GPIO_06_SAI1_TX_BCLK),  \
        I2S_GPIO(1, WS, TX, GPIO_07, IOMUXC_GPIO_07_SAI1_TX_SYNC),  \
        I2S_GPIO(1, SD, TX, GPIO_04, IOMUXC_GPIO_04_SAI1_TX_DATA00),  \
    }

#define MICROPY_BOARD_ROOT_POINTERS \
    struct _machine_i2s_obj_t *machine_i2s_obj[MICROPY_HW_I2S_NUM];
