/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef MICROPY_INCLUDED_STM32N6XX_HAL_CONF_BASE_H
#define MICROPY_INCLUDED_STM32N6XX_HAL_CONF_BASE_H

// Enable various HAL modules
#define HAL_MODULE_ENABLED
#define HAL_ADC_MODULE_ENABLED
#define HAL_BSEC_MODULE_ENABLED
#define HAL_CACHEAXI_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_CRC_MODULE_ENABLED
#define HAL_CRYP_MODULE_ENABLED
#define HAL_CSI_MODULE_ENABLED
#define HAL_DCMI_MODULE_ENABLED
#define HAL_DCMIPP_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_DMA2D_MODULE_ENABLED
#define HAL_DTS_MODULE_ENABLED
#define HAL_ETH_MODULE_ENABLED
#define HAL_EXTI_MODULE_ENABLED
#define HAL_FDCAN_MODULE_ENABLED
#define HAL_GFXMMU_MODULE_ENABLED
#define HAL_GFXTIM_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_GPU2D_MODULE_ENABLED
#define HAL_HASH_MODULE_ENABLED
#define HAL_HCD_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#define HAL_I3C_MODULE_ENABLED
#define HAL_ICACHE_MODULE_ENABLED
#define HAL_IRDA_MODULE_ENABLED
#define HAL_IWDG_MODULE_ENABLED
#define HAL_JPEG_MODULE_ENABLED
#define HAL_LPTIM_MODULE_ENABLED
#define HAL_LTDC_MODULE_ENABLED
#define HAL_MCE_MODULE_ENABLED
#define HAL_MDF_MODULE_ENABLED
#define HAL_MDIOS_MODULE_ENABLED
#define HAL_MMC_MODULE_ENABLED
#define HAL_NAND_MODULE_ENABLED
#define HAL_NOR_MODULE_ENABLED
#define HAL_PCD_MODULE_ENABLED
#define HAL_PKA_MODULE_ENABLED
#define HAL_PSSI_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RAMCFG_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_RIF_MODULE_ENABLED
#define HAL_RNG_MODULE_ENABLED
#define HAL_RTC_MODULE_ENABLED
#define HAL_SAI_MODULE_ENABLED
#define HAL_SD_MODULE_ENABLED
#define HAL_SDRAM_MODULE_ENABLED
#define HAL_SMARTCARD_MODULE_ENABLED
#define HAL_SMBUS_MODULE_ENABLED
#define HAL_SPDIFRX_MODULE_ENABLED
#define HAL_SPI_MODULE_ENABLED
#define HAL_SRAM_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
#define HAL_USART_MODULE_ENABLED
#define HAL_WWDG_MODULE_ENABLED
#define HAL_XSPI_MODULE_ENABLED

// Oscillator values in Hz
#define HSI_VALUE (64000000UL)
#define LSI_VALUE (32000UL)
#define MSI_VALUE (4000000UL)

// SysTick has the highest priority
#define TICK_INT_PRIORITY (0x00)

// Miscellaneous HAL settings
#define VDD_VALUE           3300UL
#define USE_RTOS            0
#define USE_SD_TRANSCEIVER  0
#define USE_SPI_CRC         1

// Disable dynamic callback registration
#define  USE_HAL_ADC_REGISTER_CALLBACKS       0U /* ADC register callback disabled       */
#define  USE_HAL_CACHEAXI_REGISTER_CALLBACKS  0U /* CACHEAXI register callback disabled  */
#define  USE_HAL_CRYP_REGISTER_CALLBACKS      0U /* CRYP register callback disabled      */
#define  USE_HAL_DCMI_REGISTER_CALLBACKS      0U /* DCMI register callback disabled      */
#define  USE_HAL_DCMIPP_REGISTER_CALLBACKS    0U /* DCMIPP register callback disabled    */
#define  USE_HAL_DMA2D_REGISTER_CALLBACKS     0U /* DMA2D register callback disabled     */
#define  USE_HAL_DTS_REGISTER_CALLBACKS       0U /* DTS register callback disabled       */
#define  USE_HAL_ETH_REGISTER_CALLBACKS       0U /* ETH register callback disabled       */
#define  USE_HAL_FDCAN_REGISTER_CALLBACKS     0U /* FDCAN register callback disabled     */
#define  USE_HAL_GFXMMU_REGISTER_CALLBACKS    0U /* GFXMMU register callback disabled    */
#define  USE_HAL_GFXTIM_REGISTER_CALLBACKS    0U /* GFXTIM register callback disabled    */
#define  USE_HAL_HASH_REGISTER_CALLBACKS      0U /* HASH register callback disabled      */
#define  USE_HAL_HCD_REGISTER_CALLBACKS       0U /* HCD register callback disabled       */
#define  USE_HAL_I2C_REGISTER_CALLBACKS       0U /* I2C register callback disabled       */
#define  USE_HAL_I3C_REGISTER_CALLBACKS       0U /* I3C register callback disabled       */
#define  USE_HAL_IWDG_REGISTER_CALLBACKS      0U /* IWDG register callback disabled      */
#define  USE_HAL_IRDA_REGISTER_CALLBACKS      0U /* IRDA register callback disabled      */
#define  USE_HAL_LPTIM_REGISTER_CALLBACKS     0U /* LPTIM register callback disabled     */
#define  USE_HAL_LTDC_REGISTER_CALLBACKS      0U /* LTDC register callback disabled      */
#define  USE_HAL_MCE_REGISTER_CALLBACKS       0U /* MCE register callback disabled       */
#define  USE_HAL_MDF_REGISTER_CALLBACKS       0U /* MDF register callback disabled       */
#define  USE_HAL_MMC_REGISTER_CALLBACKS       0U /* MMC register callback disabled       */
#define  USE_HAL_NAND_REGISTER_CALLBACKS      0U /* NAND register callback disabled      */
#define  USE_HAL_NOR_REGISTER_CALLBACKS       0U /* NOR register callback disabled       */
#define  USE_HAL_PCD_REGISTER_CALLBACKS       0U /* PCD register callback disabled       */
#define  USE_HAL_PKA_REGISTER_CALLBACKS       0U /* PKA register callback disabled       */
#define  USE_HAL_PSSI_REGISTER_CALLBACKS      0U /* PSSI register callback disabled      */
#define  USE_HAL_RAMCFG_REGISTER_CALLBACKS    0U /* RAMCFG register callback disabled    */
#define  USE_HAL_RNG_REGISTER_CALLBACKS       0U /* RNG register callback disabled       */
#define  USE_HAL_RTC_REGISTER_CALLBACKS       0U /* RTC register callback disabled       */
#define  USE_HAL_SAI_REGISTER_CALLBACKS       0U /* SAI register callback disabled       */
#define  USE_HAL_SD_REGISTER_CALLBACKS        0U /* SD register callback disabled        */
#define  USE_HAL_SDRAM_REGISTER_CALLBACKS     0U /* SDRAM register callback disabled     */
#define  USE_HAL_SMARTCARD_REGISTER_CALLBACKS 0U /* SMARTCARD register callback disabled */
#define  USE_HAL_SMBUS_REGISTER_CALLBACKS     0U /* SMBUS register callback disabled     */
#define  USE_HAL_SPDIFRX_REGISTER_CALLBACKS   0U /* SPDIFRX register callback disabled   */
#define  USE_HAL_SPI_REGISTER_CALLBACKS       0U /* SPI register callback disabled       */
#define  USE_HAL_SRAM_REGISTER_CALLBACKS      0U /* SRAM register callback disabled      */
#define  USE_HAL_TIM_REGISTER_CALLBACKS       0U /* TIM register callback disabled       */
#define  USE_HAL_UART_REGISTER_CALLBACKS      0U /* UART register callback disabled      */
#define  USE_HAL_USART_REGISTER_CALLBACKS     0U /* USART register callback disabled     */
#define  USE_HAL_WWDG_REGISTER_CALLBACKS      0U /* WWDG register callback disabled      */
#define  USE_HAL_XSPI_REGISTER_CALLBACKS      0U /* XSPI register callback disabled      */

// Include various HAL modules for convenience
#include "stm32n6xx_hal_rcc.h"
#include "stm32n6xx_hal_gpio.h"
#include "stm32n6xx_hal_rif.h"
#include "stm32n6xx_hal_dma.h"
#include "stm32n6xx_hal_cacheaxi.h"
#include "stm32n6xx_hal_cortex.h"
#include "stm32n6xx_hal_adc.h"
#include "stm32n6xx_hal_bsec.h"
#include "stm32n6xx_hal_crc.h"
#include "stm32n6xx_hal_cryp.h"
#include "stm32n6xx_hal_dcmi.h"
#include "stm32n6xx_hal_dcmipp.h"
#include "stm32n6xx_hal_dma2d.h"
#include "stm32n6xx_hal_dts.h"
#include "stm32n6xx_hal_eth.h"
#include "stm32n6xx_hal_exti.h"
#include "stm32n6xx_hal_fdcan.h"
#include "stm32n6xx_hal_gfxmmu.h"
#include "stm32n6xx_hal_gfxtim.h"
#include "stm32n6xx_hal_gpio.h"
#include "stm32n6xx_hal_gpu2d.h"
#include "stm32n6xx_hal_hash.h"
#include "stm32n6xx_hal_hcd.h"
#include "stm32n6xx_hal_i2c.h"
#include "stm32n6xx_hal_i3c.h"
#include "stm32n6xx_hal_icache.h"
#include "stm32n6xx_hal_irda.h"
#include "stm32n6xx_hal_iwdg.h"
#include "stm32n6xx_hal_jpeg.h"
#include "stm32n6xx_hal_lptim.h"
#include "stm32n6xx_hal_ltdc.h"
#include "stm32n6xx_hal_mce.h"
#include "stm32n6xx_hal_mdf.h"
#include "stm32n6xx_hal_mdios.h"
#include "stm32n6xx_hal_mmc.h"
#include "stm32n6xx_hal_nand.h"
#include "stm32n6xx_hal_nor.h"
#include "stm32n6xx_hal_nand.h"
#include "stm32n6xx_hal_pcd.h"
#include "stm32n6xx_hal_pka.h"
#include "stm32n6xx_hal_pssi.h"
#include "stm32n6xx_hal_pwr.h"
#include "stm32n6xx_hal_ramcfg.h"
#include "stm32n6xx_hal_rng.h"
#include "stm32n6xx_hal_rtc.h"
#include "stm32n6xx_hal_sai.h"
#include "stm32n6xx_hal_sd.h"
#include "stm32n6xx_hal_sdram.h"
#include "stm32n6xx_hal_smartcard.h"
#include "stm32n6xx_hal_smbus.h"
#include "stm32n6xx_hal_spdifrx.h"
#include "stm32n6xx_hal_spi.h"
#include "stm32n6xx_hal_sram.h"
#include "stm32n6xx_hal_tim.h"
#include "stm32n6xx_hal_uart.h"
#include "stm32n6xx_hal_usart.h"
#include "stm32n6xx_hal_wwdg.h"
#include "stm32n6xx_hal_xspi.h"
#include "stm32n6xx_ll_lpuart.h"
#include "stm32n6xx_ll_pwr.h"
#include "stm32n6xx_ll_rtc.h"
#include "stm32n6xx_ll_usart.h"

// HAL parameter assertions are disabled
#define assert_param(expr) ((void)0)

#endif // MICROPY_INCLUDED_STM32N6XX_HAL_CONF_BASE_H
