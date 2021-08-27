#define MICROPY_HW_BOARD_NAME "i.MX RT1020 EVK"
#define MICROPY_HW_MCU_NAME   "MIMXRT1021DAG5A"

#define BOARD_FLASH_SIZE (8 * 1024 * 1024)

// i.MX RT1020 EVK has 1 board LED
// Todo: think about replacing the define with searching in the generated pins?
#define MICROPY_HW_LED1_PIN (pin_GPIO_AD_B0_05)
#define MICROPY_HW_LED_ON(pin) (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin) (mp_hal_pin_high(pin))
#define BOARD_FLASH_CONFIG_HEADER_H "evkmimxrt1020_flexspi_nor_config.h"
#define BOARD_FLASH_OPS_HEADER_H "hal/flexspi_nor_flash.h"

#define MICROPY_HW_NUM_PIN_IRQS (3 * 32)

// Define mapping logical UART # to hardware UART #
// RX/TX   HW-UART    Logical UART
// D3/D5   LPUART1    Not usable, Since D3 is blocked.
// D0/D1   LPUART2 -> 1
// D6/D9   LPUART3 -> 2
// D10/D12 LPUART5 -> 3
// D14/D15 LPUART8 -> 4
// A0/A1   LPUART4 -> 5

#define MICROPY_HW_UART_NUM     (sizeof(uart_index_table) / sizeof(uart_index_table)[0])
#define MICROPY_HW_UART_INDEX   { 0, 2, 3, 5, 8, 4 }

#define IOMUX_TABLE_UART \
    { IOMUXC_GPIO_AD_B0_06_LPUART1_TX }, { IOMUXC_GPIO_AD_B0_07_LPUART1_RX }, \
    { IOMUXC_GPIO_AD_B1_08_LPUART2_TX }, { IOMUXC_GPIO_AD_B1_09_LPUART2_RX }, \
    { IOMUXC_GPIO_AD_B0_14_LPUART3_TX }, { IOMUXC_GPIO_AD_B0_15_LPUART3_RX }, \
    { IOMUXC_GPIO_AD_B1_10_LPUART4_TX }, { IOMUXC_GPIO_AD_B1_11_LPUART4_RX }, \
    { IOMUXC_GPIO_AD_B0_10_LPUART5_TX }, { IOMUXC_GPIO_AD_B0_11_LPUART5_RX }, \
    { 0 }, { 0 }, \
    { 0 }, { 0 }, \
    { IOMUXC_GPIO_SD_B1_02_LPUART8_TX }, { IOMUXC_GPIO_SD_B1_03_LPUART8_RX },

#define MICROPY_HW_SPI_INDEX { 1, 3 }

#define IOMUX_TABLE_SPI \
    { IOMUXC_GPIO_AD_B0_10_LPSPI1_SCK }, { IOMUXC_GPIO_AD_B0_11_LPSPI1_PCS0 }, \
    { IOMUXC_GPIO_AD_B0_12_LPSPI1_SDO }, { IOMUXC_GPIO_AD_B0_13_LPSPI1_SDI }, \
    { 0 }, { 0 }, \
    { 0 }, { 0 }, \
    { IOMUXC_GPIO_AD_B1_12_LPSPI3_SCK }, { IOMUXC_GPIO_AD_B1_13_LPSPI3_PCS0 }, \
    { IOMUXC_GPIO_AD_B1_14_LPSPI3_SDO }, { IOMUXC_GPIO_AD_B1_15_LPSPI3_SDI }, 

#define DMA_REQ_SRC_RX { 0, kDmaRequestMuxLPSPI1Rx, kDmaRequestMuxLPSPI2Rx, \
                            kDmaRequestMuxLPSPI3Rx, kDmaRequestMuxLPSPI4Rx }

#define DMA_REQ_SRC_TX { 0, kDmaRequestMuxLPSPI1Tx, kDmaRequestMuxLPSPI2Tx, \
                            kDmaRequestMuxLPSPI3Tx, kDmaRequestMuxLPSPI4Tx } 

// Define mapping hardware I2C # to logical I2C #
// SDA/SCL  HW-I2C    Logical I2C
// D14/D15  LPI2C4 ->    0
// A4/A5    LPI2C1 ->    1
// D0/D1    LPI2C2 ->    2

#define MICROPY_HW_I2C_INDEX   { 4, 1, 2 }

#define IOMUX_TABLE_I2C \
    { IOMUXC_GPIO_AD_B1_14_LPI2C1_SCL }, { IOMUXC_GPIO_AD_B1_15_LPI2C1_SDA }, \
    { IOMUXC_GPIO_AD_B1_08_LPI2C2_SCL }, { IOMUXC_GPIO_AD_B1_09_LPI2C2_SDA }, \
    { 0 }, { 0 }, \
    { IOMUXC_GPIO_SD_B1_02_LPI2C4_SCL }, { IOMUXC_GPIO_SD_B1_03_LPI2C4_SDA },
