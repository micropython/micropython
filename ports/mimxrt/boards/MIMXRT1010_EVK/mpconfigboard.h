#define MICROPY_HW_BOARD_NAME "i.MX RT1010 EVK"
#define MICROPY_HW_MCU_NAME   "MIMXRT1011DAE5A"

#define BOARD_FLASH_SIZE (16 * 1024 * 1024)

// i.MX RT1010 EVK has 1 board LED
#define MICROPY_HW_LED1_PIN (pin_GPIO_11)
#define MICROPY_HW_LED_ON(pin) (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin) (mp_hal_pin_low(pin))
#define BOARD_FLASH_CONFIG_HEADER_H "evkmimxrt1010_flexspi_nor_config.h"

#define MICROPY_HW_NUM_PIN_IRQS (2 * 32)

// Define mapping logical UART # to hardware UART #
// LPUART1 on D0/D1 -> 1
// LPUART3 on A0/D4 -> 3
// LPUART4 on D6/D7 -> 2

#define MICROPY_HW_UART_NUM     (sizeof(uart_index_table) / sizeof(uart_index_table)[0])
#define MICROPY_HW_UART_INDEX   { 0, 1, 4, 3 }

#define IOMUX_TABLE_UART \
    { IOMUXC_GPIO_10_LPUART1_TXD }, { IOMUXC_GPIO_09_LPUART1_RXD }, \
    { 0 }, { 0 }, \
    { IOMUXC_GPIO_08_LPUART3_TXD }, { IOMUXC_GPIO_AD_07_LPUART3_RXD }, \
    { IOMUXC_GPIO_AD_02_LPUART4_TXD }, { IOMUXC_GPIO_AD_01_LPUART4_RXD },

#define MICROPY_HW_SPI_INDEX { 1 }

#define IOMUX_TABLE_SPI \
    { IOMUXC_GPIO_AD_06_LPSPI1_SCK }, { IOMUXC_GPIO_AD_05_LPSPI1_PCS0 }, \
    { IOMUXC_GPIO_AD_04_LPSPI1_SDO }, { IOMUXC_GPIO_AD_03_LPSPI1_SDI },

#define DMA_REQ_SRC_RX { 0, kDmaRequestMuxLPSPI1Rx, kDmaRequestMuxLPSPI2Rx }
#define DMA_REQ_SRC_TX { 0, kDmaRequestMuxLPSPI1Tx, kDmaRequestMuxLPSPI2Tx } 
