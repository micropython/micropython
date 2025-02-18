#define MICROPY_HW_BOARD_NAME "Adafruit Metro M7"
#define MICROPY_HW_MCU_NAME   "MIMXRT1011DAE5A"

// i.MX RT1010 EVK has 1 board LED
#define MICROPY_HW_LED1_PIN (pin_GPIO_03)
#define MICROPY_HW_LED_ON(pin) (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin) (mp_hal_pin_low(pin))

#define MICROPY_HW_NUM_PIN_IRQS (2 * 32)
#define MICROPY_PY_MACHINE_SDCARD    (0)

// Define mapping logical UART # to hardware UART #
// LPUART1 on USB_DBG  -> 0
// LPUART1 on D0/D1    -> 1
// LPUART3 on A0/D4    -> 3
// LPUART4 on D6/D7    -> 2

#define MICROPY_HW_UART_NUM     (sizeof(uart_index_table) / sizeof(uart_index_table)[0])
#define MICROPY_HW_UART_INDEX   { 1, 1, 2, 4 }

#define IOMUX_TABLE_UART \
    { IOMUXC_GPIO_10_LPUART1_TXD }, { IOMUXC_GPIO_09_LPUART1_RXD }, \
    { IOMUXC_GPIO_12_LPUART3_TXD }, { IOMUXC_GPIO_11_LPUART3_RXD }, \
    { 0 }, { 0 }, \
    { IOMUXC_GPIO_AD_02_LPUART4_TXD }, { IOMUXC_GPIO_AD_01_LPUART4_RXD },

#define IOMUX_TABLE_UART_CTS_RTS \
    { IOMUXC_GPIO_08_LPUART1_CTS_B }, { IOMUXC_GPIO_07_LPUART1_RTS_B }, \
    { IOMUXC_GPIO_AD_14_LPUART3_CTS_B }, { 0 }, \
    { 0 }, { 0 }, \
    { IOMUXC_GPIO_AD_14_LPUART4_CTS_B }, { 0 },

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

// Wifi Definitions
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-metro-m7"

#define MICROPY_HW_WIFI_SPI_ID       (0)
#define MICROPY_HW_WIFI_SPI_BAUDRATE (8000000)

#define MICROPY_HW_NINA_ACK          pin_find(MP_OBJ_NEW_QSTR(MP_QSTR_ESP_BUSY))
#define MICROPY_HW_NINA_CS           pin_find(MP_OBJ_NEW_QSTR(MP_QSTR_ESP_CS))
#define MICROPY_HW_NINA_RESET        pin_find(MP_OBJ_NEW_QSTR(MP_QSTR_ESP_RESET))

// BLE definitions
#define MICROPY_PY_BLUETOOTH_NINAW10 (1)

#define MICROPY_HW_BLE_UART_ID       (1)
#define MICROPY_HW_BLE_UART_BAUDRATE (115200)

#define MICROPY_HW_NINA_RTS          pin_find(MP_OBJ_NEW_QSTR(MP_QSTR_MOSI))
#define MICROPY_HW_NINA_CTS          MICROPY_HW_NINA_ACK
