/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _HAL_PLATFORM_
#define _HAL_PLATFORM_

#define ROMVERSION 0x03
#define ROMINFORMATION (ROMVERSION)

#define SYSTEM_CLK                  PLATFORM_CLOCK

#define SDR_SDRAM_BASE              0x30000000
#define SYSTEM_CTRL_BASE            0x40000000
#define PERI_ON_BASE                0x40000000
#define VENDOR_REG_BASE             0x40002800
#define SPI_FLASH_BASE              0x98000000
#define SDR_CTRL_BASE               0x40005000

#define PERIPHERAL_IRQ_STATUS       0x04
#define PERIPHERAL_IRQ_MODE         0x08
#define PERIPHERAL_IRQ_EN           0x0C
#define LP_PERI_EXT_IRQ_STATUS      0x24
#define LP_PERI_EXT_IRQ_MODE        0x28
#define LP_PERI_EXT_IRQ_EN          0x2C

#define PERIPHERAL_IRQ_ALL_LEVEL    0

#define TIMER_CLK                   32*1000

//3 Peripheral IP Base Address
#define GPIO_REG_BASE               0x40001000
#define TIMER_REG_BASE              0x40002000
#define NFC_INTERFACE_BASE          0x40002400
#define LOG_UART_REG_BASE           0x40003000
#define I2C2_REG_BASE               0x40003400
#define I2C3_REG_BASE               0x40003800
#define SPI_FLASH_CTRL_BASE         0x40006000
#define ADC_REG_BASE                0x40010000
#define DAC_REG_BASE                0x40011000
#define UART0_REG_BASE              0x40040000
#define UART1_REG_BASE              0x40040400
#define UART2_REG_BASE              0x40040800
#define SPI0_REG_BASE               0x40042000
#define SPI1_REG_BASE               0x40042400
#define SPI2_REG_BASE               0x40042800
#define I2C0_REG_BASE               0x40044000
#define I2C1_REG_BASE               0x40044400
#define SDIO_DEVICE_REG_BASE        0x40050000
#define MII_REG_BASE                0x40050000
#define SDIO_HOST_REG_BASE          0x40058000
#define GDMA0_REG_BASE              0x40060000
#define GDMA1_REG_BASE              0x40061000
#define I2S0_REG_BASE               0x40062000
#define I2S1_REG_BASE               0x40063000
#define PCM0_REG_BASE               0x40064000
#define PCM1_REG_BASE               0x40065000
#define CRYPTO_REG_BASE             0x40070000
#define WIFI_REG_BASE               0x40080000
#define USB_OTG_REG_BASE            0x400C0000

#define GDMA1_REG_OFF               0x1000
#define I2S1_REG_OFF                0x1000
#define PCM1_REG_OFF                0x1000
#define SSI_REG_OFF                 0x400
#define RUART_REG_OFF               0x400

#define CPU_CLK_TYPE_NO             6

enum _BOOT_TYPE_ {
    BOOT_FROM_FLASH = 0,
    BOOT_FROM_SDIO  = 1,        
    BOOT_FROM_USB   = 2,
    BOOT_FROM_RSVD  = 3,    
};

enum _EFUSE_CPU_CLK_ {
    #if 1
    CLK_200M   = 0,
    CLK_100M   = 1,
    CLK_50M    = 2,
    CLK_25M    = 3,
    CLK_12_5M  = 4,    
    CLK_4M     = 5,
    #else
    CLK_25M    = 0,
    CLK_200M   = 1,
    CLK_100M   = 2,        
    CLK_50M    = 3,
    CLK_12_5M  = 4,    
    CLK_4M     = 5,    
    #endif
};


#endif //_HAL_PLATFORM_
