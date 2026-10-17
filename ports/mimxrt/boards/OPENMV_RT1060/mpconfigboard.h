/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2023 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2023 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Board config for OpenMV-RT60.
 */

#define MICROPY_HW_BOARD_NAME   "OpenMV IMXRT1060"
#define MICROPY_HW_MCU_NAME     "MIMXRT1062DVJ6A"
#define MICROPY_HW_FLASH_FS_LABEL   "OPENMV"

// machine.bootloader() enters the OpenMV bootloader, with the firmware image left active.
#define MICROPY_BOARD_ENTER_BOOTLOADER(nargs, args) \
    do { \
        extern void ROM_RunBootloader(void *); \
        uint32_t rom_arg = 0xEB000000; \
        ROM_RunBootloader(&rom_arg); \
    } while (0)

// The UART IRQ_RXIDLE support breaks the Bluetooth HCI UART: the CYW43's
// firmware download times out.  The OpenMV firmware disables it too.
#define MICROPY_PY_MACHINE_UART_IRQ (0)

#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "openmvrt1060"

#define MICROPY_HW_USB_MSC          (1)
#define MICROPY_HW_USB_VID          0x37c5
#ifndef MICROPY_HW_USB_PID
#define MICROPY_HW_USB_PID          0x1060
#endif

#define MICROPY_HW_LED1_PIN         (pin_GPIO_B0_10)
#define MICROPY_HW_LED2_PIN         (pin_GPIO_B0_11)
#define MICROPY_HW_LED3_PIN         (pin_GPIO_B1_14)

#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))

#define MICROPY_HW_NUM_PIN_IRQS     (4 * 32 + 3)

#define MICROPY_HW_SDCARD_SDMMC     (1)

// WiFi SDMMC
#define MICROPY_HW_SDIO_SDMMC       (2)
#define MICROPY_HW_SDIO_CLK         (pin_GPIO_AD_B1_09)
#define MICROPY_HW_SDIO_CMD         (pin_GPIO_AD_B1_08)
#define MICROPY_HW_SDIO_D0          (pin_GPIO_SD_B1_03)
#define MICROPY_HW_SDIO_D1          (pin_GPIO_SD_B1_02)
#define MICROPY_HW_SDIO_D2          (pin_GPIO_AD_B1_06)
#define MICROPY_HW_SDIO_D3          (pin_GPIO_AD_B1_07)

#define MICROPY_HW_WL_REG_ON        (pin_GPIO_B0_14)
#define MICROPY_HW_WL_HOST_WAKE     (pin_GPIO_B0_15)

#define MICROPY_HW_BT_REG_ON        (pin_GPIO_AD_B0_08)
#define MICROPY_HW_BT_HOST_WAKE     (pin_GPIO_AD_B0_14)
#define MICROPY_HW_BT_DEV_WAKE      (pin_GPIO_SD_B1_00)
#define MICROPY_HW_BT_CTS           (pin_GPIO_AD_B1_04)

#define MICROPY_HW_SDIO_CLK_ALT     (6)
#define MICROPY_HW_SDIO_CMD_ALT     (6)
#define MICROPY_HW_SDIO_D0_ALT      (0)
#define MICROPY_HW_SDIO_D1_ALT      (0)
#define MICROPY_HW_SDIO_D2_ALT      (6)
#define MICROPY_HW_SDIO_D3_ALT      (6)

// Bluetooth config.
#define MICROPY_HW_BLE_UART_ID          (0)
#define MICROPY_HW_BLE_UART_BASE        (LPUART3)
#define MICROPY_HW_BLE_UART_BAUDRATE    (115200)
#define MICROPY_HW_BLE_UART_BAUDRATE_SECONDARY (2500000)
#define MICROPY_HW_BLE_UART_BAUDRATE_DOWNLOAD_FIRMWARE (2500000)

// CYW43 config
#define CYW43_WIFI_NVRAM_INCLUDE_FILE "wifi_nvram_1dx.h"

// SDRAM config
#define MICROPY_HW_SDRAM_TIMING_TRC         (60)
#define MICROPY_HW_SDRAM_TIMING_TRP         (18)
#define MICROPY_HW_SDRAM_TIMING_TRCD        (18)
#define MICROPY_HW_SDRAM_TIMING_TWR         (12)
#define MICROPY_HW_SDRAM_TIMING_TRRD        (60)
#define MICROPY_HW_SDRAM_TIMING_TXSR        (67)
#define MICROPY_HW_SDRAM_TIMING_TRAS        (42)
#define MICROPY_HW_SDRAM_TIMING_TREF        (64 * 1000000 / 8192) // 64ms/8192

#define MICROPY_HW_SDRAM_CAS_LATENCY        (kSEMC_LatencyThree)
#define MICROPY_HW_SDRAM_MEM_BUS_WIDTH      (kSEMC_PortSize16Bit)
#define MICROPY_HW_SDRAM_COLUMN_BITS_NUM    (kSEMC_SdramColunm_9bit)
#define MICROPY_HW_SDRAM_BURST_LENGTH       (kSEMC_Sdram_BurstLen8)
#define MICROPY_HW_SDRAM_RBURST_LENGTH      (1)

// Define the mapping hardware UART # to logical UART #
// Bus      HW-UART      Logical UART
// BLE      LPUART3 ->   0
// External LPUART1 ->   1

#define MICROPY_HW_UART_NUM     (sizeof(uart_index_table) / sizeof(uart_index_table)[0])
#define MICROPY_HW_UART_INDEX   { 3, 1 }

#define IOMUX_TABLE_UART \
    { IOMUXC_GPIO_AD_B0_12_LPUART1_TX }, { IOMUXC_GPIO_AD_B0_13_LPUART1_RX }, \
    { 0 }, { 0 }, \
    { IOMUXC_GPIO_B0_08_LPUART3_TX }, { IOMUXC_GPIO_B0_09_LPUART3_RX }, \
    { 0 }, { 0 }, \
    { 0 }, { 0 }, \
    { 0 }, { 0 }, \
    { 0 }, { 0 }, \
    { 0 }, { 0 },

#define IOMUX_TABLE_UART_CTS_RTS \
    { 0 }, { 0 }, \
    { 0 }, { 0 }, \
    { IOMUXC_GPIO_AD_B1_04_LPUART3_CTS_B }, { IOMUXC_GPIO_AD_B1_05_LPUART3_RTS_B }, \
    { 0 }, { 0 }, \
    { 0 }, { 0 }, \
    { 0 }, { 0 }, \
    { 0 }, { 0 }, \
    { 0 }, { 0 },

// Define the mapping hardware SPI # to logical SPI #
// Bus      HW-SPI       Logical SPI
// Camera   LPSPI4 ->    0
// External LPSPI3 ->    1

#define MICROPY_HW_SPI_INDEX { 4, 3 }

#define IOMUX_TABLE_SPI \
    { 0 }, { 0 }, \
    { 0 }, { 0 }, \
    { 0 }, \
    { 0 }, { 0 }, \
    { 0 }, { 0 }, \
    { 0 }, \
    { IOMUXC_GPIO_AD_B0_00_LPSPI3_SCK }, { IOMUXC_GPIO_AD_B0_03_LPSPI3_PCS0 }, \
    { IOMUXC_GPIO_AD_B0_01_LPSPI3_SDO }, { IOMUXC_GPIO_AD_B0_02_LPSPI3_SDI }, \
    { 0 }, \
    { IOMUXC_GPIO_B0_03_LPSPI4_SCK }, { IOMUXC_GPIO_B0_00_LPSPI4_PCS0 }, \
    { IOMUXC_GPIO_B0_02_LPSPI4_SDO }, { IOMUXC_GPIO_B0_01_LPSPI4_SDI }, \
    { 0 },

#define DMA_REQ_SRC_RX { 0, kDmaRequestMuxLPSPI1Rx, kDmaRequestMuxLPSPI2Rx, \
                         kDmaRequestMuxLPSPI3Rx, kDmaRequestMuxLPSPI4Rx }

#define DMA_REQ_SRC_TX { 0, kDmaRequestMuxLPSPI1Tx, kDmaRequestMuxLPSPI2Tx, \
                         kDmaRequestMuxLPSPI3Tx, kDmaRequestMuxLPSPI4Tx }

// Define the mapping hardware I2C # to logical I2C #
// Bus      HW-I2C       Logical I2C
// Camera   LPI2C1 ->    0
// External LPI2C4 ->    1
// Internal LPI2C2 ->    2

#define MICROPY_HW_I2C_INDEX   { 1, 4, 2 }

#define IOMUX_TABLE_I2C \
    { IOMUXC_GPIO_AD_B1_00_LPI2C1_SCL }, { IOMUXC_GPIO_AD_B1_01_LPI2C1_SDA }, \
    { IOMUXC_GPIO_B0_04_LPI2C2_SCL }, { IOMUXC_GPIO_B0_05_LPI2C2_SDA }, \
    { 0 }, { 0 }, \
    { IOMUXC_GPIO_AD_B0_12_LPI2C4_SCL }, { IOMUXC_GPIO_AD_B0_13_LPI2C4_SDA },

// Define the mapping hardware CAN # to logical CAN #
// Bus      HW-CAN       Logical CAN
// External FLEXCAN2 ->  0

#define MICROPY_HW_CAN1_NAME "CAN1"
#define MICROPY_HW_NUM_CAN (1)
#define MICROPY_HW_CAN_INDEX { 2 }
#define MICROPY_HW_NUM_CAN_IRQS (1)

#define IOMUX_TABLE_CAN \
    { 0 }, { 0 }, \
    { IOMUXC_GPIO_AD_B0_02_FLEXCAN2_TX }, { IOMUXC_GPIO_AD_B0_03_FLEXCAN2_RX },

#define USDHC_DUMMY_PIN NULL, 0
#define MICROPY_USDHC1 \
    { \
        .cmd = {GPIO_SD_B0_00_USDHC1_CMD}, \
        .clk = { GPIO_SD_B0_01_USDHC1_CLK }, \
        .cd_b = { GPIO_AD_B1_02_USDHC1_CD_B }, \
        .data0 = { GPIO_SD_B0_02_USDHC1_DATA0 }, \
        .data1 = { GPIO_SD_B0_03_USDHC1_DATA1 }, \
        .data2 = { GPIO_SD_B0_04_USDHC1_DATA2 }, \
        .data3 = { GPIO_SD_B0_05_USDHC1_DATA3 }, \
    }

// --- SEMC --- //
#define MIMXRT_IOMUXC_SEMC_DATA00 IOMUXC_GPIO_EMC_00_SEMC_DATA00
#define MIMXRT_IOMUXC_SEMC_DATA01 IOMUXC_GPIO_EMC_01_SEMC_DATA01
#define MIMXRT_IOMUXC_SEMC_DATA02 IOMUXC_GPIO_EMC_02_SEMC_DATA02
#define MIMXRT_IOMUXC_SEMC_DATA03 IOMUXC_GPIO_EMC_03_SEMC_DATA03
#define MIMXRT_IOMUXC_SEMC_DATA04 IOMUXC_GPIO_EMC_04_SEMC_DATA04
#define MIMXRT_IOMUXC_SEMC_DATA05 IOMUXC_GPIO_EMC_05_SEMC_DATA05
#define MIMXRT_IOMUXC_SEMC_DATA06 IOMUXC_GPIO_EMC_06_SEMC_DATA06
#define MIMXRT_IOMUXC_SEMC_DATA07 IOMUXC_GPIO_EMC_07_SEMC_DATA07
#define MIMXRT_IOMUXC_SEMC_DATA08 IOMUXC_GPIO_EMC_30_SEMC_DATA08
#define MIMXRT_IOMUXC_SEMC_DATA09 IOMUXC_GPIO_EMC_31_SEMC_DATA09
#define MIMXRT_IOMUXC_SEMC_DATA10 IOMUXC_GPIO_EMC_32_SEMC_DATA10
#define MIMXRT_IOMUXC_SEMC_DATA11 IOMUXC_GPIO_EMC_33_SEMC_DATA11
#define MIMXRT_IOMUXC_SEMC_DATA12 IOMUXC_GPIO_EMC_34_SEMC_DATA12
#define MIMXRT_IOMUXC_SEMC_DATA13 IOMUXC_GPIO_EMC_35_SEMC_DATA13
#define MIMXRT_IOMUXC_SEMC_DATA14 IOMUXC_GPIO_EMC_36_SEMC_DATA14
#define MIMXRT_IOMUXC_SEMC_DATA15 IOMUXC_GPIO_EMC_37_SEMC_DATA15

#define MIMXRT_IOMUXC_SEMC_ADDR00 IOMUXC_GPIO_EMC_09_SEMC_ADDR00
#define MIMXRT_IOMUXC_SEMC_ADDR01 IOMUXC_GPIO_EMC_10_SEMC_ADDR01
#define MIMXRT_IOMUXC_SEMC_ADDR02 IOMUXC_GPIO_EMC_11_SEMC_ADDR02
#define MIMXRT_IOMUXC_SEMC_ADDR03 IOMUXC_GPIO_EMC_12_SEMC_ADDR03
#define MIMXRT_IOMUXC_SEMC_ADDR04 IOMUXC_GPIO_EMC_13_SEMC_ADDR04
#define MIMXRT_IOMUXC_SEMC_ADDR05 IOMUXC_GPIO_EMC_14_SEMC_ADDR05
#define MIMXRT_IOMUXC_SEMC_ADDR06 IOMUXC_GPIO_EMC_15_SEMC_ADDR06
#define MIMXRT_IOMUXC_SEMC_ADDR07 IOMUXC_GPIO_EMC_16_SEMC_ADDR07
#define MIMXRT_IOMUXC_SEMC_ADDR08 IOMUXC_GPIO_EMC_17_SEMC_ADDR08
#define MIMXRT_IOMUXC_SEMC_ADDR09 IOMUXC_GPIO_EMC_18_SEMC_ADDR09
#define MIMXRT_IOMUXC_SEMC_ADDR10 IOMUXC_GPIO_EMC_23_SEMC_ADDR10
#define MIMXRT_IOMUXC_SEMC_ADDR11 IOMUXC_GPIO_EMC_19_SEMC_ADDR11
#define MIMXRT_IOMUXC_SEMC_ADDR12 IOMUXC_GPIO_EMC_20_SEMC_ADDR12

#define MIMXRT_IOMUXC_SEMC_BA0 IOMUXC_GPIO_EMC_21_SEMC_BA0
#define MIMXRT_IOMUXC_SEMC_BA1 IOMUXC_GPIO_EMC_22_SEMC_BA1
#define MIMXRT_IOMUXC_SEMC_CAS IOMUXC_GPIO_EMC_24_SEMC_CAS
#define MIMXRT_IOMUXC_SEMC_CKE IOMUXC_GPIO_EMC_27_SEMC_CKE
#define MIMXRT_IOMUXC_SEMC_CLK IOMUXC_GPIO_EMC_26_SEMC_CLK
#define MIMXRT_IOMUXC_SEMC_DM00 IOMUXC_GPIO_EMC_08_SEMC_DM00
#define MIMXRT_IOMUXC_SEMC_DM01 IOMUXC_GPIO_EMC_38_SEMC_DM01
#define MIMXRT_IOMUXC_SEMC_DQS IOMUXC_GPIO_EMC_39_SEMC_DQS
#define MIMXRT_IOMUXC_SEMC_RAS IOMUXC_GPIO_EMC_25_SEMC_RAS
#define MIMXRT_IOMUXC_SEMC_WE IOMUXC_GPIO_EMC_28_SEMC_WE

#define MIMXRT_IOMUXC_SEMC_CS0 IOMUXC_GPIO_EMC_29_SEMC_CS0

// Network definitions
// Transceiver Phy Address
#define ENET_PHY_ADDRESS    (0)
#define ENET_PHY_OPS        phyksz8081_ops

// Etherner PIN definitions
#define ENET_RESET_PIN      &pin_GPIO_B0_13
#define ENET_INT_PIN        &pin_GPIO_B0_12

#define IOMUX_TABLE_ENET \
    { IOMUXC_GPIO_B1_04_ENET_RX_DATA00, 0, 0xB0E9u }, \
    { IOMUXC_GPIO_B1_05_ENET_RX_DATA01, 0, 0xB0E9u }, \
    { IOMUXC_GPIO_B1_06_ENET_RX_EN, 0, 0xB0E9u }, \
    { IOMUXC_GPIO_B1_07_ENET_TX_DATA00, 0, 0xB0E9u }, \
    { IOMUXC_GPIO_B1_08_ENET_TX_DATA01, 0, 0xB0E9u }, \
    { IOMUXC_GPIO_B1_09_ENET_TX_EN, 0, 0xB0E9u }, \
    { IOMUXC_GPIO_B1_10_ENET_REF_CLK, 1, 0x71u }, \
    { IOMUXC_GPIO_B1_11_ENET_RX_ER, 0, 0xB0E9u }, \
    { IOMUXC_GPIO_EMC_41_ENET_MDIO, 0, 0xB0E9u }, \
    { IOMUXC_GPIO_EMC_40_ENET_MDC, 0, 0xB0E9u },

// USB CDC config
#define CFG_TUD_CDC_EP_BUFSIZE  (4096)
#define CFG_TUD_CDC_RX_BUFSIZE  (4096)
#define CFG_TUD_CDC_TX_BUFSIZE  (4096)

// Morse Micro MM8108 802.11ah shield
#if MICROPY_PY_NETWORK_HALOW_MORSE_MICRO
#define MICROPY_HW_HALOW_MORSE_MICRO_SPI_ID                 (1)                     // P0/P1/P2 are LPSPI3, logical SPI 1
#define MICROPY_HW_HALOW_MORSE_MICRO_CS                     (pin_GPIO_AD_B0_03)     // P3
#define MICROPY_HW_HALOW_MORSE_MICRO_IRQ                    (pin_GPIO_B0_07)        // P8
#define MICROPY_PY_NETWORK_HALOW_PIN_IRQ        (1)
#define MICROPY_HW_HALOW_MORSE_MICRO_RESET                  (pin_GPIO_B0_06)        // P7
#define MICROPY_HW_HALOW_MORSE_MICRO_WAKE                   (pin_GPIO_B1_00)        // P9
#define MICROPY_HW_HALOW_MORSE_MICRO_BUSY                   (pin_WAKEUP)            // P11
#define MICROPY_HW_HALOW_MORSE_MICRO_BUSY_INVERTED          (1)                     // shield inverts BUSY
#endif
