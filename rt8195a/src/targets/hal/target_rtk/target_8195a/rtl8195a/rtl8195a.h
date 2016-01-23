/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef _HAL_8195A_H_
#define _HAL_8195A_H_

#include "platform_options.h"
#include "basic_types.h"
#include "section_config.h"
#include "rtl8195a_sys_on.h"
#include "rtl8195a_peri_on.h"
#include "hal_platform.h"
#include "hal_pinmux.h"
#include "hal_api.h"
#include "hal_peri_on.h"
#include "hal_misc.h"
#include "hal_irqn.h"
#include "hal_vector_table.h"
#include "hal_diag.h"
#include "hal_spi_flash.h"
#include "hal_timer.h"
#include "hal_util.h"
#include "hal_efuse.h"
#include "diag.h"

/* ----------------------------------------------------------------------------
   -- Cortex M3 Core Configuration
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup Cortex_Core_Configuration Cortex M0 Core Configuration
 * @{
 */

#define __CM3_REV                      0x0200    /**< Core revision r0p0 */
#define __MPU_PRESENT                  1         /**< Defines if an MPU is present or not */
#define __NVIC_PRIO_BITS               4         /**< Number of priority bits implemented in the NVIC */
#define __Vendor_SysTickConfig         1         /**< Vendor specific implementation of SysTickConfig is defined */

#include "core_cm3.h"

#include "hal_timer.h"

#ifdef CONFIG_GDMA_EN
#include "hal_gdma.h"
#include "rtl8195a_gdma.h"
#endif

#ifdef CONFIG_GPIO_EN
#include "hal_gpio.h"
#include "rtl8195a_gpio.h"
#endif

#ifdef CONFIG_SPI_COM_EN
#include "hal_ssi.h"
#include "rtl8195a_ssi.h"
#endif

#ifdef CONFIG_UART_EN
#include "hal_uart.h"
#include "rtl8195a_uart.h"
#endif

#ifdef CONFIG_I2C_EN
#include "hal_i2c.h"
#include "rtl8195a_i2c.h"
#endif

#ifdef CONFIG_PCM_EN
#include "hal_pcm.h"
#include "rtl8195a_pcm.h"
#endif

#ifdef CONFIG_PWM_EN
#include "hal_pwm.h"
#include "rtl8195a_pwm.h"
#endif

#ifdef CONFIG_I2S_EN
#include "hal_i2s.h"
#include "rtl8195a_i2s.h"
#endif

#ifdef CONFIG_DAC_EN
#include "hal_dac.h"
#include "rtl8195a_dac.h"
#endif

#ifdef CONFIG_ADC_EN
#include "hal_adc.h"
#include "rtl8195a_adc.h"
#endif



#ifdef CONFIG_SDIO_DEVICE_EN
#include "hal_sdio.h"
#endif

#ifdef CONFIG_NFC_EN
#include "hal_nfc.h"
#include "rtl8195a_nfc.h"
#endif


#endif //_HAL_8195A_H_
