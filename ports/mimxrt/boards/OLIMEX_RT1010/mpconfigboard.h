#define MICROPY_HW_BOARD_NAME "RT1010-Py-DevKIT"
#define MICROPY_HW_MCU_NAME   "MIMXRT1011DAE5A"
#define MICROPY_HW_USB_STR_MANUF "Olimex Ltd."
#define MICROPY_HW_USB_VID 0x15ba
#define MICROPY_HW_USB_PID 0x0046
#define MICROPY_PY_UOS_DUPTERM_BUILTIN_STREAM (0)

// Olimex RT1010-Py has 1 board LED
#define MICROPY_HW_LED1_PIN (pin_GPIO_11)
#define MICROPY_HW_LED_ON(pin) (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin) (mp_hal_pin_low(pin))

#define MICROPY_HW_NUM_PIN_IRQS (2 * 32)

// Define mapping logical UART # to hardware UART #
// LPUART1 on RX/TX    -> 1
// LPUART4 on D5/D6    -> 2

#define MICROPY_HW_UART_NUM     (sizeof(uart_index_table) / sizeof(uart_index_table)[0])
#define MICROPY_HW_UART_INDEX   { 0, 1, 4 }

#define IOMUX_TABLE_UART \
    { IOMUXC_GPIO_10_LPUART1_TXD }, { IOMUXC_GPIO_09_LPUART1_RXD }, \
    { 0 }, { 0 }, \
    { 0 }, { 0 }, \
    { IOMUXC_GPIO_06_LPUART4_TXD }, { IOMUXC_GPIO_05_LPUART4_RXD },

#define MICROPY_HW_SPI_INDEX { 0, 1, 2 }

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

#define MICROPY_HW_I2C_INDEX   { 0, 1, 2 }

#define IOMUX_TABLE_I2C \
    { IOMUXC_GPIO_AD_14_LPI2C1_SCL }, { IOMUXC_GPIO_AD_13_LPI2C1_SDA }, \
    { IOMUXC_GPIO_AD_08_LPI2C2_SCL }, { IOMUXC_GPIO_AD_07_LPI2C2_SDA },
