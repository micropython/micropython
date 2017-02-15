/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _HAL_PERI_ON_H_
#define _HAL_PERI_ON_H_

#define MASK_ALLON                          0xFFFFFFFF

#define HAL_PERI_ON_READ32(addr)            HAL_READ32(PERI_ON_BASE, addr)
#define HAL_PERI_ON_WRITE32(addr, value)    HAL_WRITE32(PERI_ON_BASE, addr, value)
#define HAL_PERI_ON_READ16(addr)            HAL_READ16(PERI_ON_BASE, addr)
#define HAL_PERI_ON_WRITE16(addr, value)    HAL_WRITE16(PERI_ON_BASE, addr, value)
#define HAL_PERI_ON_READ8(addr)             HAL_READ8(PERI_ON_BASE, addr)
#define HAL_PERI_ON_WRITE8(addr, value)     HAL_WRITE8(PERI_ON_BASE, addr, value)
#define HAL_PERL_ON_FUNC_CTRL(addr,value,ctrl)  \
        HAL_PERI_ON_WRITE32(addr, ((HAL_PERI_ON_READ32(addr) & (~value))|((MASK_ALLON - ctrl + 1) & value)))
#define HAL_PERL_ON_PIN_SEL(addr,mask,value)  \
        HAL_PERI_ON_WRITE32(addr, ((HAL_PERI_ON_READ32(addr) & (~mask)) | value))

//40 REG_SYS_REGU_CTRL0
#define LDO25M_CTRL(ctrl)       HAL_PERL_ON_FUNC_CTRL(REG_SYS_REGU_CTRL0, BIT_SYS_REGU_LDO25M_EN, ctrl)

//A0 SYS_DEBUG_CTRL
#define DEBUG_PIN_CTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_SYS_DEBUG_CTRL, BIT_SYS_DBG_PIN_EN, ctrl)

//A4 SYS_PINMUX_CTRL
#define SIC_PIN_CTRL(ctrl)      HAL_PERL_ON_FUNC_CTRL(REG_SYS_PINMUX_CTRL, BIT_SIC_PIN_EN, ctrl)
#define EEPROM_PIN_CTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_SYS_PINMUX_CTRL, BIT_EEPROM_PIN_EN, ctrl)


//210 SOV_FUNC_EN
#define LXBUS_FCTRL(ctrl)       HAL_PERL_ON_FUNC_CTRL(REG_SOC_FUNC_EN, BIT_SOC_LXBUS_EN, ctrl)
#define FLASH_FCTRL(ctrl)       { \
                                    if (!ctrl) { \
                                        HAL_READ32(SPI_FLASH_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_FUNC_EN, BIT_SOC_FLASH_EN, ctrl);}

#define MEM_CTRL_FCTRL(ctrl)    { \
                                    if (!ctrl) { \
                                        HAL_READ32(SDR_SDRAM_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_FUNC_EN, BIT_SOC_MEM_CTRL_EN, ctrl);}

#define LOC_UART_FCTRL(ctrl)    { \
                                    if (!ctrl) { \
                                        HAL_READ32(LOG_UART_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_FUNC_EN, BIT_SOC_LOG_UART_EN, ctrl);}

#define GDMA0_FCTRL(ctrl)       { \
                                    if (!ctrl) { \
                                        HAL_READ32(GDMA0_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_FUNC_EN, BIT_SOC_GDMA0_EN, ctrl);}

#define GDMA1_FCTRL(ctrl)       { \
                                    if (!ctrl) { \
                                        HAL_READ32(GDMA1_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_FUNC_EN, BIT_SOC_GDMA1_EN, ctrl);}

#define GTIMER_FCTRL(ctrl)      { \
                                    if (!ctrl) { \
                                        HAL_READ32(TIMER_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_FUNC_EN, BIT_SOC_GTIMER_EN, ctrl);}

#define SECURITY_FCTRL(ctrl)    { \
                                    if (!ctrl) { \
                                        HAL_READ32(CRYPTO_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_FUNC_EN, BIT_SOC_SECURITY_ENGINE_EN, ctrl);}

//214 SOC_HCI_COM_FUNC_EN
#define SDIOD_ON_FCTRL(ctrl)    { \
                                    if (!ctrl) { \
                                        HAL_READ32(SDIO_DEVICE_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_HCI_COM_FUNC_EN, BIT_SOC_HCI_SDIOD_ON_EN, ctrl);}

#define SDIOD_OFF_FCTRL(ctrl)   { \
                                    if (!ctrl) { \
                                        HAL_READ32(SDIO_DEVICE_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_HCI_COM_FUNC_EN, BIT_SOC_HCI_SDIOD_OFF_EN, ctrl);}

#define SDIOH_FCTRL(ctrl)       { \
                                    if (!ctrl) { \
                                        HAL_READ32(SDIO_HOST_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_HCI_COM_FUNC_EN, BIT_SOC_HCI_SDIOH_EN, ctrl);}

#define SDIO_ON_RST_MASK(ctrl)  HAL_PERL_ON_FUNC_CTRL(REG_SOC_HCI_COM_FUNC_EN, BIT_SOC_HCI_SDIOD_ON_RST_MUX, ctrl)
#define OTG_FCTRL(ctrl)         { \
                                    if (!ctrl) { \
                                        HAL_READ32(USB_OTG_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_HCI_COM_FUNC_EN, BIT_SOC_HCI_OTG_EN, ctrl);}

#define OTG_RST_MASK(ctrl)      HAL_PERL_ON_FUNC_CTRL(REG_SOC_HCI_COM_FUNC_EN, BIT_SOC_HCI_OTG_RST_MUX, ctrl)
#define MII_FCTRL(ctrl)         { \
                                    if (!ctrl) { \
                                        HAL_READ32(MII_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_HCI_COM_FUNC_EN, BIT_SOC_HCI_MII_EN, ctrl);}

#define MII_MUX_SEL(ctrl)       HAL_PERL_ON_FUNC_CTRL(REG_SOC_HCI_COM_FUNC_EN, BIT_SOC_HCI_SM_SEL, ctrl)
#define WL_MACON_FCTRL(ctrl)    { \
                                    if (!ctrl) { \
                                        HAL_READ32(WIFI_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_HCI_COM_FUNC_EN, BIT_SOC_HCI_WL_MACON_EN, ctrl);}

//218 SOC_PERI_FUNC0_EN
#define UART0_FCTRL(ctrl)       { \
                                    if (!ctrl) { \
                                        HAL_READ32(UART0_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_PERI_FUNC0_EN, BIT_PERI_UART0_EN, ctrl);}

#define UART1_FCTRL(ctrl)       { \
                                    if (!ctrl) { \
                                        HAL_READ32(UART1_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_PERI_FUNC0_EN, BIT_PERI_UART1_EN, ctrl);}

#define UART2_FCTRL(ctrl)       { \
                                    if (!ctrl) { \
                                        HAL_READ32(UART2_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_PERI_FUNC0_EN, BIT_PERI_UART2_EN, ctrl);}

#define SPI0_FCTRL(ctrl)        { \
                                    if (!ctrl) { \
                                        HAL_READ32(SPI0_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_PERI_FUNC0_EN, BIT_PERI_SPI0_EN, ctrl);}

#define SPI1_FCTRL(ctrl)        { \
                                    if (!ctrl) { \
                                        HAL_READ32(SPI1_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_PERI_FUNC0_EN, BIT_PERI_SPI1_EN, ctrl);}

#define SPI2_FCTRL(ctrl)        { \
                                    if (!ctrl) { \
                                        HAL_READ32(SPI2_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_PERI_FUNC0_EN, BIT_PERI_SPI2_EN, ctrl);}

#define I2C0_FCTRL(ctrl)        { \
                                    if (!ctrl) { \
                                        HAL_READ32(I2C0_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_PERI_FUNC0_EN, BIT_PERI_I2C0_EN, ctrl);}

#define I2C1_FCTRL(ctrl)        { \
                                    if (!ctrl) { \
                                        HAL_READ32(I2C1_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_PERI_FUNC0_EN, BIT_PERI_I2C1_EN, ctrl);}

#define I2C2_FCTRL(ctrl)        { \
                                    if (!ctrl) { \
                                        HAL_READ32(I2C2_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_PERI_FUNC0_EN, BIT_PERI_I2C2_EN, ctrl);}

#define I2C3_FCTRL(ctrl)        { \
                                    if (!ctrl) { \
                                        HAL_READ32(I2C3_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_PERI_FUNC0_EN, BIT_PERI_I2C3_EN, ctrl);}

#define I2S0_FCTRL(ctrl)        { \
                                    if (!ctrl) { \
                                        HAL_READ32(I2S0_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_PERI_FUNC0_EN, BIT_PERI_I2S0_EN, ctrl);}

#define I2S1_FCTRL(ctrl)        { \
                                    if (!ctrl) { \
                                        HAL_READ32(I2S1_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_PERI_FUNC0_EN, BIT_PERI_I2S1_EN, ctrl);}

#define PCM0_FCTRL(ctrl)        { \
                                    if (!ctrl) { \
                                        HAL_READ32(PCM0_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_PERI_FUNC0_EN, BIT_PERI_PCM0_EN, ctrl);}

#define PCM1_FCTRL(ctrl)        { \
                                    if (!ctrl) { \
                                        HAL_READ32(PCM1_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_PERI_FUNC0_EN, BIT_PERI_PCM1_EN, ctrl);}

//21C SOC_PERI_FUNC1_EN
#define ADC0_FCTRL(ctrl)        { \
                                    if (!ctrl) { \
                                        HAL_READ32(ADC_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_PERI_FUNC1_EN, BIT_PERI_ADC0_EN, ctrl);}

#define DAC0_FCTRL(ctrl)        { \
                                    if (!ctrl) { \
                                        HAL_READ32(DAC_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_PERI_FUNC1_EN, BIT_PERI_DAC0_EN, ctrl);}

#define DAC1_FCTRL(ctrl)        { \
                                    if (!ctrl) { \
                                        HAL_READ32(DAC_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_PERI_FUNC1_EN, BIT_PERI_DAC1_EN, ctrl);}

#define GPIO_FCTRL(ctrl)        { \
                                    if (!ctrl) { \
                                        HAL_READ32(GPIO_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_PERI_FUNC1_EN, BIT_PERI_GPIO_EN, ctrl);}

//220 SOC_PERI_BD_FUNC0_EN
#define UART0_BD_FCTRL(ctrl)    { \
                                    if (!ctrl) { \
                                        HAL_READ32(UART0_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_PERI_BD_FUNC0_EN, BIT_PERI_UART0_BD_EN, ctrl);}

#define UART1_BD_FCTRL(ctrl)    { \
                                    if (!ctrl) { \
                                        HAL_READ32(UART1_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_PERI_BD_FUNC0_EN, BIT_PERI_UART1_BD_EN, ctrl);}

#define UART2_BD_FCTRL(ctrl)    { \
                                    if (!ctrl) { \
                                        HAL_READ32(UART2_REG_BASE,0);\
                                    }\
                                    HAL_PERL_ON_FUNC_CTRL(REG_SOC_PERI_BD_FUNC0_EN, BIT_PERI_UART2_BD_EN, ctrl);}

//230 PESOC_CLK_CTRL
#define ACTCK_CPU_CCTRL(ctrl)       HAL_PERL_ON_FUNC_CTRL(REG_PESOC_CLK_CTRL, BIT_SOC_CKE_PLFM, ctrl)
#define ACTCK_TRACE_CCTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_PESOC_CLK_CTRL, BIT_SOC_ACTCK_TRACE_EN, ctrl)
#define SLPCK_TRACE_CCTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_PESOC_CLK_CTRL, BIT_SOC_SLPCK_TRACE_EN, ctrl)
#define ACTCK_VENDOR_CCTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_PESOC_CLK_CTRL, BIT_SOC_ACTCK_VENDOR_REG_EN, ctrl)
#define SLPCK_VENDOR_CCTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_PESOC_CLK_CTRL, BIT_SOC_SLPCK_VENDOR_REG_EN, ctrl)
#define ACTCK_FLASH_CCTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_PESOC_CLK_CTRL, BIT_SOC_ACTCK_FLASH_EN, ctrl)
#define SLPCK_FLASH_CCTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_PESOC_CLK_CTRL, BIT_SOC_SLPCK_FLASH_EN, ctrl)
#define ACTCK_SDR_CCTRL(ctrl)       HAL_PERL_ON_FUNC_CTRL(REG_PESOC_CLK_CTRL, BIT_SOC_ACTCK_SDR_EN, ctrl)
#define SLPCK_SDR_CCTRL(ctrl)       HAL_PERL_ON_FUNC_CTRL(REG_PESOC_CLK_CTRL, BIT_SOC_SLPCK_SDR_EN, ctrl)
#define ACTCK_LOG_UART_CCTRL(ctrl)  HAL_PERL_ON_FUNC_CTRL(REG_PESOC_CLK_CTRL, BIT_SOC_ACTCK_LOG_UART_EN, ctrl)
#define SLPCK_LOG_UART_CCTRL(ctrl)  HAL_PERL_ON_FUNC_CTRL(REG_PESOC_CLK_CTRL, BIT_SOC_SLPCK_LOG_UART_EN, ctrl)
#define ACTCK_TIMER_CCTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_PESOC_CLK_CTRL, BIT_SOC_ACTCK_TIMER_EN, ctrl)
#define SLPCK_TIMER_CCTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_PESOC_CLK_CTRL, BIT_SOC_SLPCK_TIMER_EN, ctrl)
#define ACTCK_GDMA0_CCTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_PESOC_CLK_CTRL, BIT_SOC_ACTCK_GDMA0_EN, ctrl)
#define SLPCK_GDMA0_CCTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_PESOC_CLK_CTRL, BIT_SOC_SLPCK_GDMA0_EN, ctrl)
#define ACTCK_GDMA1_CCTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_PESOC_CLK_CTRL, BIT_SOC_ACTCK_GDMA1_EN, ctrl)
#define SLPCK_GDMA1_CCTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_PESOC_CLK_CTRL, BIT_SOC_SLPCK_GDMA1_EN, ctrl)
#define ACTCK_GPIO_CCTRL(ctrl)      HAL_PERL_ON_FUNC_CTRL(REG_PESOC_CLK_CTRL, BIT_SOC_ACTCK_GPIO_EN, ctrl)
#define SLPCK_GPIO_CCTRL(ctrl)      HAL_PERL_ON_FUNC_CTRL(REG_PESOC_CLK_CTRL, BIT_SOC_SLPCK_GPIO_EN, ctrl)
#define ACTCK_BTCMD_CCTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_PESOC_CLK_CTRL, BIT_SOC_ACTCK_BTCMD_EN, ctrl)
#define SLPCK_BTCMD_CCTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_PESOC_CLK_CTRL, BIT_SOC_SLPCK_BTCMD_EN, ctrl)

//234 PESOC_PERI_CLK_CTRL0
#define ACTCK_UART0_CCTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL0, BIT_SOC_ACTCK_UART0_EN, ctrl)
#define SLPCK_UART0_CCTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL0, BIT_SOC_SLPCK_UART0_EN, ctrl)
#define ACTCK_UART1_CCTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL0, BIT_SOC_ACTCK_UART1_EN, ctrl)
#define SLPCK_UART1_CCTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL0, BIT_SOC_SLPCK_UART1_EN, ctrl)
#define ACTCK_UART2_CCTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL0, BIT_SOC_ACTCK_UART2_EN, ctrl)
#define SLPCK_UART2_CCTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL0, BIT_SOC_SLPCK_UART2_EN, ctrl)
#define ACTCK_SPI0_CCTRL(ctrl)  HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL0, BIT_SOC_ACTCK_SPI0_EN, ctrl)
#define SLPCK_SPI0_CCTRL(ctrl)  HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL0, BIT_SOC_SLPCK_SPI0_EN, ctrl)
#define ACTCK_SPI1_CCTRL(ctrl)  HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL0, BIT_SOC_ACTCK_SPI1_EN, ctrl)
#define SLPCK_SPI1_CCTRL(ctrl)  HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL0, BIT_SOC_SLPCK_SPI1_EN, ctrl)
#define ACTCK_SPI2_CCTRL(ctrl)  HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL0, BIT_SOC_ACTCK_SPI2_EN, ctrl)
#define SLPCK_SPI2_CCTRL(ctrl)  HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL0, BIT_SOC_SLPCK_SPI2_EN, ctrl)

//238 PESOC_PERI_CLK_CTRL1
#define ACTCK_I2C0_CCTRL(ctrl)  HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL1, BIT_SOC_ACTCK_I2C0_EN, ctrl)
#define SLPCK_I2C0_CCTRL(ctrl)  HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL1, BIT_SOC_SLPCK_I2C0_EN, ctrl)
#define ACTCK_I2C1_CCTRL(ctrl)  HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL1, BIT_SOC_ACTCK_I2C1_EN, ctrl)
#define SLPCK_I2C1_CCTRL(ctrl)  HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL1, BIT_SOC_SLPCK_I2C1_EN, ctrl)
#define ACTCK_I2C2_CCTRL(ctrl)  HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL1, BIT_SOC_ACTCK_I2C2_EN, ctrl)
#define SLPCK_I2C2_CCTRL(ctrl)  HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL1, BIT_SOC_SLPCK_I2C2_EN, ctrl)
#define ACTCK_I2C3_CCTRL(ctrl)  HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL1, BIT_SOC_ACTCK_I2C3_EN, ctrl)
#define SLPCK_I2C3_CCTRL(ctrl)  HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL1, BIT_SOC_SLPCK_I2C3_EN, ctrl)
#define ACTCK_I2S_CCTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL1, BIT_SOC_ACTCK_I2S_EN, ctrl)
#define SLPCK_I2S_CCTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL1, BIT_SOC_SLPCK_I2S_EN, ctrl)
#define ACTCK_PCM_CCTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL1, BIT_SOC_ACTCK_PCM_EN, ctrl)
#define SLPCK_PCM_CCTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL1, BIT_SOC_SLPCK_PCM_EN, ctrl)
#define ACTCK_ADC_CCTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL1, BIT_SOC_ACTCK_ADC_EN, ctrl)
#define SLPCK_ADC_CCTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL1, BIT_SOC_SLPCK_ADC_EN, ctrl)
#define ACTCK_DAC_CCTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL1, BIT_SOC_ACTCK_DAC_EN, ctrl)
#define SLPCK_DAC_CCTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CLK_CTRL1, BIT_SOC_SLPCK_DAC_EN, ctrl)

//240 PESOC_HCI_CLK_CTRL0
#define ACTCK_SDIOD_CCTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_PESOC_HCI_CLK_CTRL0, BIT_SOC_ACTCK_SDIO_DEV_EN, ctrl)
#define SLPCK_SDIOD_CCTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_PESOC_HCI_CLK_CTRL0, BIT_SOC_SLPCK_SDIO_DEV_EN, ctrl)
#define ACTCK_SDIOH_CCTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_PESOC_HCI_CLK_CTRL0, BIT_SOC_ACTCK_SDIO_HST_EN, ctrl)
#define SLPCK_SDIOH_CCTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_PESOC_HCI_CLK_CTRL0, BIT_SOC_SLPCK_SDIO_HST_EN, ctrl)
#define ACTCK_OTG_CCTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_PESOC_HCI_CLK_CTRL0, BIT_SOC_ACTCK_OTG_EN, ctrl)
#define SLPCK_OTG_CCTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_PESOC_HCI_CLK_CTRL0, BIT_SOC_SLPCK_OTG_EN, ctrl)
#define ACTCK_MII_MPHY_CCTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_PESOC_HCI_CLK_CTRL0, BIT_SOC_ACTCK_MII_MPHY_EN, ctrl)
#define SLPCK_MII_MPHY_CCTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_PESOC_HCI_CLK_CTRL0, BIT_SOC_SLPCK_MII_MPHY_EN, ctrl)

//244 PESOC_COM_CLK_CTRL1
#define ACTCK_WL_CCTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_PESOC_COM_CLK_CTRL1, BIT_SOC_ACTCK_WL_EN, ctrl)
#define SLPCK_WL_CCTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_PESOC_COM_CLK_CTRL1, BIT_SOC_SLPCK_WL_EN, ctrl)
#define ACTCK_SEC_ENG_CCTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_PESOC_COM_CLK_CTRL1, BIT_SOC_ACTCK_SECURITY_ENG_EN, ctrl)
#define SLPCK_SEC_ENG_CCTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_PESOC_COM_CLK_CTRL1, BIT_SOC_SLPCK_SECURITY_ENG_EN, ctrl)
#define ACTCK_NFC_CCTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_PESOC_COM_CLK_CTRL1, BIT_SOC_ACTCK_NFC_EN, ctrl)
#define SLPCK_NFC_CCTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_PESOC_COM_CLK_CTRL1, BIT_SOC_SLPCK_NFC_EN, ctrl)
#define NFC_CAL_CCTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_PESOC_COM_CLK_CTRL1, BIT_SOC_NFC_CAL_EN, ctrl)

//250 REG_PERI_CLK_SEL
#define TRACE_CLK_SEL(num)      HAL_PERL_ON_PIN_SEL(REG_PESOC_CLK_SEL, (BIT_MASK_PESOC_TRACE_CK_SEL << BIT_SHIFT_PESOC_TRACE_CK_SEL), BIT_PESOC_TRACE_CK_SEL(num))
#define FLASH_CLK_SEL(num)      HAL_PERL_ON_PIN_SEL(REG_PESOC_CLK_SEL, (BIT_MASK_PESOC_FLASH_CK_SEL << BIT_SHIFT_PESOC_FLASH_CK_SEL), BIT_PESOC_FLASH_CK_SEL(num))
#define SDR_CLK_SEL(num)        HAL_PERL_ON_PIN_SEL(REG_PESOC_CLK_SEL, (BIT_MASK_PESOC_SDR_CK_SEL << BIT_SHIFT_PESOC_SDR_CK_SEL), BIT_PESOC_SDR_CK_SEL(num))
#define I2C_SCLK_SEL(num)       HAL_PERL_ON_PIN_SEL(REG_PESOC_CLK_SEL, (BIT_MASK_PESOC_PERI_SCLK_SEL << BIT_SHIFT_PESOC_PERI_SCLK_SEL), BIT_PESOC_PERI_SCLK_SEL(num))

//270 REG_OSC32K_CTRL
#define OSC32K_CKGEN_CTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_OSC32K_CTRL, BIT_32K_POW_CKGEN_EN, ctrl)

//280 REG_UART_MUX_CTRL
#define UART0_PIN_CTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_UART_MUX_CTRL, BIT_UART0_PIN_EN, ctrl)
#define UART0_PIN_SEL(num)      HAL_PERL_ON_PIN_SEL(REG_UART_MUX_CTRL, (BIT_MASK_UART0_PIN_SEL << BIT_SHIFT_UART0_PIN_SEL), BIT_UART0_PIN_SEL(num))
#define UART1_PIN_CTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_UART_MUX_CTRL, BIT_UART1_PIN_EN, ctrl)
#define UART1_PIN_SEL(num)      HAL_PERL_ON_PIN_SEL(REG_UART_MUX_CTRL, (BIT_MASK_UART1_PIN_SEL << BIT_SHIFT_UART1_PIN_SEL), BIT_UART1_PIN_SEL(num))
#define UART2_PIN_CTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_UART_MUX_CTRL, BIT_UART2_PIN_EN, ctrl)
#define UART2_PIN_SEL(num)      HAL_PERL_ON_PIN_SEL(REG_UART_MUX_CTRL, (BIT_MASK_UART2_PIN_SEL << BIT_SHIFT_UART2_PIN_SEL), BIT_UART2_PIN_SEL(num))

//284 REG_SPI_MUX_CTRL
#define SPI0_PIN_CTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_SPI_MUX_CTRL, BIT_SPI0_PIN_EN, ctrl)
#define SPI0_PIN_SEL(num)       HAL_PERL_ON_PIN_SEL(REG_SPI_MUX_CTRL, (BIT_MASK_SPI0_PIN_SEL << BIT_SHIFT_SPI0_PIN_SEL), BIT_SPI0_PIN_SEL(num))
#define SPI1_PIN_CTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_SPI_MUX_CTRL, BIT_SPI1_PIN_EN, ctrl)
#define SPI1_PIN_SEL(num)       HAL_PERL_ON_PIN_SEL(REG_SPI_MUX_CTRL, (BIT_MASK_SPI1_PIN_SEL << BIT_SHIFT_SPI1_PIN_SEL), BIT_SPI1_PIN_SEL(num))
#define SPI2_PIN_CTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_SPI_MUX_CTRL, BIT_SPI2_PIN_EN, ctrl)
#define SPI2_PIN_SEL(num)       HAL_PERL_ON_PIN_SEL(REG_SPI_MUX_CTRL, (BIT_MASK_SPI2_PIN_SEL << BIT_SHIFT_SPI2_PIN_SEL), BIT_SPI2_PIN_SEL(num))
#define SPI0_MULTI_CS_CTRL(ctrl)  HAL_PERL_ON_FUNC_CTRL(REG_SPI_MUX_CTRL, BIT_SPI0_MULTI_CS_EN, ctrl)

//288 REG_I2C_MUX_CTRL
#define I2C0_PIN_CTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_I2C_MUX_CTRL, BIT_I2C0_PIN_EN, ctrl)
#define I2C0_PIN_SEL(num)       HAL_PERL_ON_PIN_SEL(REG_I2C_MUX_CTRL, (BIT_MASK_I2C0_PIN_SEL << BIT_SHIFT_I2C0_PIN_SEL), BIT_I2C0_PIN_SEL(num))
#define I2C1_PIN_CTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_I2C_MUX_CTRL, BIT_I2C1_PIN_EN, ctrl)
#define I2C1_PIN_SEL(num)       HAL_PERL_ON_PIN_SEL(REG_I2C_MUX_CTRL, (BIT_MASK_I2C1_PIN_SEL << BIT_SHIFT_I2C1_PIN_SEL), BIT_I2C1_PIN_SEL(num))
#define I2C2_PIN_CTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_I2C_MUX_CTRL, BIT_I2C2_PIN_EN, ctrl)
#define I2C2_PIN_SEL(num)       HAL_PERL_ON_PIN_SEL(REG_I2C_MUX_CTRL, (BIT_MASK_I2C2_PIN_SEL << BIT_SHIFT_I2C2_PIN_SEL), BIT_I2C2_PIN_SEL(num))
#define I2C3_PIN_CTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_I2C_MUX_CTRL, BIT_I2C3_PIN_EN, ctrl)
#define I2C3_PIN_SEL(num)       HAL_PERL_ON_PIN_SEL(REG_I2C_MUX_CTRL, (BIT_MASK_I2C3_PIN_SEL << BIT_SHIFT_I2C3_PIN_SEL), BIT_I2C3_PIN_SEL(num))

//28C REG_I2S_MUX_CTRL
#define I2S0_PIN_CTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_I2S_MUX_CTRL, BIT_I2S0_PIN_EN, ctrl)
#define I2S0_MCK_CTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_I2S_MUX_CTRL, BIT_I2S0_MCK_EN, ctrl)
#define I2S0_PIN_SEL(num)       HAL_PERL_ON_PIN_SEL(REG_I2S_MUX_CTRL, (BIT_MASK_I2S0_PIN_SEL << BIT_SHIFT_I2S0_PIN_SEL), BIT_I2S0_PIN_SEL(num))
#define I2S1_PIN_CTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_I2S_MUX_CTRL, BIT_I2S1_PIN_EN, ctrl)
#define I2S1_MCK_CTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_I2S_MUX_CTRL, BIT_I2S1_MCK_EN, ctrl)
#define I2S1_PIN_SEL(num)       HAL_PERL_ON_PIN_SEL(REG_I2S_MUX_CTRL, (BIT_MASK_I2S1_PIN_SEL << BIT_SHIFT_I2S1_PIN_SEL), BIT_I2S1_PIN_SEL(num))
#define PCM0_PIN_CTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_I2S_MUX_CTRL, BIT_PCM0_PIN_EN, ctrl)
#define PCM0_PIN_SEL(num)       HAL_PERL_ON_PIN_SEL(REG_I2S_MUX_CTRL, (BIT_MASK_PCM0_PIN_SEL << BIT_SHIFT_PCM0_PIN_SEL), BIT_PCM0_PIN_SEL(num))
#define PCM1_PIN_CTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_I2S_MUX_CTRL, BIT_PCM1_PIN_EN, ctrl)
#define PCM1_PIN_SEL(num)       HAL_PERL_ON_PIN_SEL(REG_I2S_MUX_CTRL, (BIT_MASK_PCM1_PIN_SEL << BIT_SHIFT_PCM1_PIN_SEL), BIT_PCM1_PIN_SEL(num))

//2A0 HCI_PINMUX_CTRL
#define SDIOD_PIN_FCTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_HCI_PINMUX_CTRL, BIT_HCI_SDIOD_PIN_EN, ctrl)
#define SDIOH_PIN_FCTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_HCI_PINMUX_CTRL, BIT_HCI_SDIOH_PIN_EN, ctrl)
#define MII_PIN_FCTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_HCI_PINMUX_CTRL, BIT_HCI_MII_PIN_EN, ctrl)

//2A4 WL_PINMUX_CTRL
#define LED_PIN_FCTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_WL_PINMUX_CTRL, BIT_WL_LED_PIN_EN, ctrl)
#define LED_PIN_SEL(num)        HAL_PERL_ON_PIN_SEL(REG_WL_PINMUX_CTRL, (BIT_MASK_WL_LED_PIN_SEL << BIT_SHIFT_WL_LED_PIN_SEL), BIT_WL_LED_PIN_SEL(num))
#define ANT0_PIN_FCTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_WL_PINMUX_CTRL, BIT_WL_ANT0_PIN_EN, ctrl)
#define ANT1_PIN_FCTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_WL_PINMUX_CTRL, BIT_WL_ANT1_PIN_EN, ctrl)
#define BTCOEX_PIN_FCTRL(ctrl)  HAL_PERL_ON_FUNC_CTRL(REG_WL_PINMUX_CTRL, BIT_WL_BTCOEX_PIN_EN, ctrl)
#define BTCMD_PIN_FCTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_WL_PINMUX_CTRL, BIT_WL_BTCMD_PIN_EN, ctrl)
#define NFC_PIN_FCTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_WL_PINMUX_CTRL, BIT_NFC_PIN_EN, ctrl)

//2AC PWM_PINMUX_CTRL
#define PWM0_PIN_FCTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_PWM_PINMUX_CTRL, BIT_PWM0_PIN_EN, ctrl)
#define PWM0_PIN_SEL(num)       HAL_PERL_ON_PIN_SEL(REG_PWM_PINMUX_CTRL, (BIT_MASK_PWM0_PIN_SEL << BIT_SHIFT_PWM0_PIN_SEL), BIT_PWM0_PIN_SEL(num))
#define PWM1_PIN_FCTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_PWM_PINMUX_CTRL, BIT_PWM1_PIN_EN, ctrl)
#define PWM1_PIN_SEL(num)       HAL_PERL_ON_PIN_SEL(REG_PWM_PINMUX_CTRL, (BIT_MASK_PWM1_PIN_SEL << BIT_SHIFT_PWM1_PIN_SEL), BIT_PWM1_PIN_SEL(num))
#define PWM2_PIN_FCTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_PWM_PINMUX_CTRL, BIT_PWM2_PIN_EN, ctrl)
#define PWM2_PIN_SEL(num)       HAL_PERL_ON_PIN_SEL(REG_PWM_PINMUX_CTRL, (BIT_MASK_PWM2_PIN_SEL << BIT_SHIFT_PWM2_PIN_SEL), BIT_PWM2_PIN_SEL(num))
#define PWM3_PIN_FCTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_PWM_PINMUX_CTRL, BIT_PWM3_PIN_EN, ctrl)
#define PWM3_PIN_SEL(num)       HAL_PERL_ON_PIN_SEL(REG_PWM_PINMUX_CTRL, (BIT_MASK_PWM3_PIN_SEL << BIT_SHIFT_PWM3_PIN_SEL), BIT_PWM3_PIN_SEL(num))
#define ETE0_PIN_FCTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_PWM_PINMUX_CTRL, BIT_ETE0_PIN_EN, ctrl)
#define ETE0_PIN_SEL(num)       HAL_PERL_ON_PIN_SEL(REG_PWM_PINMUX_CTRL, (BIT_MASK_ETE0_PIN_SEL << BIT_SHIFT_ETE0_PIN_SEL), BIT_ETE0_PIN_SEL(num))
#define ETE1_PIN_FCTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_PWM_PINMUX_CTRL, BIT_ETE1_PIN_EN, ctrl)
#define ETE1_PIN_SEL(num)       HAL_PERL_ON_PIN_SEL(REG_PWM_PINMUX_CTRL, (BIT_MASK_ETE1_PIN_SEL << BIT_SHIFT_ETE1_PIN_SEL), BIT_ETE1_PIN_SEL(num))
#define ETE2_PIN_FCTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_PWM_PINMUX_CTRL, BIT_ETE2_PIN_EN, ctrl)
#define ETE2_PIN_SEL(num)       HAL_PERL_ON_PIN_SEL(REG_PWM_PINMUX_CTRL, (BIT_MASK_ETE2_PIN_SEL << BIT_SHIFT_ETE2_PIN_SEL), BIT_ETE2_PIN_SEL(num))
#define ETE3_PIN_FCTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_PWM_PINMUX_CTRL, BIT_ETE3_PIN_EN, ctrl)
#define ETE3_PIN_SEL(num)       HAL_PERL_ON_PIN_SEL(REG_PWM_PINMUX_CTRL, (BIT_MASK_ETE3_PIN_SEL << BIT_SHIFT_ETE3_PIN_SEL), BIT_ETE3_PIN_SEL(num))

//2C0 CPU_PERIPHERAL_CTRL
#define SPI_FLASH_PIN_FCTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_CPU_PERIPHERAL_CTRL, BIT_SPI_FLSH_PIN_EN, ctrl)
#define SPI_FLASH_PIN_SEL(num)       HAL_PERL_ON_PIN_SEL(REG_CPU_PERIPHERAL_CTRL, (BIT_MASK_SPI_FLSH_PIN_SEL << BIT_SHIFT_SPI_FLSH_PIN_SEL), BIT_SPI_FLSH_PIN_SEL(num))
#define SDR_PIN_FCTRL(ctrl)          HAL_PERL_ON_FUNC_CTRL(REG_CPU_PERIPHERAL_CTRL, BIT_SDR_PIN_EN, ctrl)
#define TRACE_PIN_FCTRL(ctrl)        HAL_PERL_ON_FUNC_CTRL(REG_CPU_PERIPHERAL_CTRL, BIT_TRACE_PIN_EN, ctrl)
#define LOG_UART_PIN_FCTRL(ctrl)     HAL_PERL_ON_FUNC_CTRL(REG_CPU_PERIPHERAL_CTRL, BIT_LOG_UART_PIN_EN, ctrl)
#define LOG_UART_IR_FCTRL(ctrl)      HAL_PERL_ON_FUNC_CTRL(REG_CPU_PERIPHERAL_CTRL, BIT_LOG_UART_IR_EN, ctrl)

//300 REG_PESOC_MEM_CTRL
#define SDR_DDL_FCTRL(ctrl)      HAL_PERL_ON_PIN_SEL(REG_PESOC_MEM_CTRL, (BIT_MASK_PESOC_SDR_DDL_CTRL << BIT_SHIFT_PESOC_SDR_DDL_CTRL), BIT_PESOC_SDR_DDL_CTRL(ctrl))
#define FLASH_DDL_FCTRL(ctrl)    HAL_PERL_ON_PIN_SEL(REG_PESOC_MEM_CTRL, (BIT_MASK_PESOC_FLASH_DDL_CTRL << BIT_SHIFT_PESOC_FLASH_DDL_CTRL), BIT_PESOC_FLASH_DDL_CTRL(ctrl))

//304 REG_PESOC_SOC_CTRL
#define SRAM_MUX_CFG(num)        HAL_PERL_ON_PIN_SEL(REG_PESOC_SOC_CTRL, (BIT_MASK_PESOC_SRAM_MUX_CFG << BIT_SHIFT_PESOC_SRAM_MUX_CFG), BIT_PESOC_SRAM_MUX_CFG(num))
#define LX_WL_SWAP_CTRL(ctrl)    HAL_PERL_ON_FUNC_CTRL(REG_PESOC_SOC_CTRL, BIT_PESOC_LX_WL_SWAP_SEL, ctrl)
#define LX_MST_SWAP_CTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_PESOC_SOC_CTRL, BIT_PESOC_LX_MST_SWAP_SEL, ctrl)
#define LX_SLV_SWAP_CTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_PESOC_SOC_CTRL, BIT_PESOC_LX_SLV_SWAP_SEL, ctrl)
#define MII_LX_WRAPPER_FCTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_PESOC_SOC_CTRL, BIT_PESOC_MII_LX_WRAPPER_EN, ctrl)
#define MII_LX_MST_SWAP_CTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_PESOC_SOC_CTRL, BIT_PESOC_MII_LX_MST_SWAP_SEL, ctrl)
#define MII_LX_SLV_SWAP_CTRL(ctrl)   HAL_PERL_ON_FUNC_CTRL(REG_PESOC_SOC_CTRL, BIT_PESOC_MII_LX_SLV_SWAP_SEL, ctrl)
#define GDMA_CFG(num)            HAL_PERL_ON_PIN_SEL(REG_PESOC_SOC_CTRL, (BIT_MASK_PESOC_GDMA_CFG << BIT_SHIFT_PESOC_GDMA_CFG), BIT_PESOC_GDMA_CFG(num))

//308 PESOC_PERI_CTRL
#define SPI_RN_FCTRL(ctrl)       HAL_PERL_ON_FUNC_CTRL(REG_PESOC_PERI_CTRL, BIT_SOC_FUNC_SPI_RN, ctrl)

//320 GPIO_SHTDN_CTRL
#define GPIO_GPA_SHTDN_N_FCTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_GPIO_SHTDN_CTRL, BIT_GPIO_GPA_SHTDN_N, ctrl)
#define GPIO_GPB_SHTDN_N_FCTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_GPIO_SHTDN_CTRL, BIT_GPIO_GPB_SHTDN_N, ctrl)
#define GPIO_GPC_SHTDN_N_FCTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_GPIO_SHTDN_CTRL, BIT_GPIO_GPC_SHTDN_N, ctrl)
#define GPIO_GPD_SHTDN_N_FCTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_GPIO_SHTDN_CTRL, BIT_GPIO_GPD_SHTDN_N, ctrl)
#define GPIO_GPE_SHTDN_N_FCTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_GPIO_SHTDN_CTRL, BIT_GPIO_GPE_SHTDN_N, ctrl)
#define GPIO_GPF_SHTDN_N_FCTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_GPIO_SHTDN_CTRL, BIT_GPIO_GPF_SHTDN_N, ctrl)
#define GPIO_GPG_SHTDN_N_FCTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_GPIO_SHTDN_CTRL, BIT_GPIO_GPG_SHTDN_N, ctrl)
#define GPIO_GPH_SHTDN_N_FCTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_GPIO_SHTDN_CTRL, BIT_GPIO_GPH_SHTDN_N, ctrl)
#define GPIO_GPI_SHTDN_N_FCTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_GPIO_SHTDN_CTRL, BIT_GPIO_GPI_SHTDN_N, ctrl)
#define GPIO_GPJ_SHTDN_N_FCTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_GPIO_SHTDN_CTRL, BIT_GPIO_GPJ_SHTDN_N, ctrl)
#define GPIO_GPK_SHTDN_N_FCTRL(ctrl) HAL_PERL_ON_FUNC_CTRL(REG_GPIO_SHTDN_CTRL, BIT_GPIO_GPK_SHTDN_N, ctrl)

//374
#define EGTIM_FCTRL(ctrl)       HAL_PERL_ON_FUNC_CTRL(REG_PERI_EGTIM_CTRL, BIT_PERI_EGTIM_EN, ctrl)
#define EGTIM_RSIG_SEL(num)     HAL_PERL_ON_PIN_SEL(REG_PERI_EGTIM_CTRL, (BIT_MASK_PERI_EGTIM_REF_SIG_SEL << BIT_SHIFT_PERI_EGTIM_REF_SIG_SEL), BIT_PERI_EGTIM_REF_SIG_SEL(num))
#define EGTIME_PIN_G0_OPT_SEL(num) HAL_PERL_ON_PIN_SEL(REG_PERI_EGTIM_CTRL, (BIT_MASK_PERI_EGTIM_PIN_GROUP0_OPT_SEL << BIT_SHIFT_PERI_EGTIM_PIN_GROUP0_OPT_SEL), BIT_PERI_EGTIM_PIN_GROUP0_OPT_SEL(num))
#define EGTIME_PIN_G1_OPT_SEL(num) HAL_PERL_ON_PIN_SEL(REG_PERI_EGTIM_CTRL, (BIT_MASK_PERI_EGTIM_PIN_GROUP1_OPT_SEL << BIT_SHIFT_PERI_EGTIM_PIN_GROUP1_OPT_SEL), BIT_PERI_EGTIM_PIN_GROUP1_OPT_SEL(num))
#define EGTIME_PIN_G2_OPT_SEL(num) HAL_PERL_ON_PIN_SEL(REG_PERI_EGTIM_CTRL, (BIT_MASK_PERI_EGTIM_PIN_GROUP2_OPT_SEL << BIT_SHIFT_PERI_EGTIM_PIN_GROUP2_OPT_SEL), BIT_PERI_EGTIM_PIN_GROUP2_OPT_SEL(num))


#endif //_HAL_PERI_ON_H_

