/*
 * @brief LPC43xx basic chip inclusion file
 *
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __CHIP_LPC43XX_H_
#define __CHIP_LPC43XX_H_

#include "lpc_types.h"
#include "sys_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(CORE_M4) && !defined(CORE_M0)
#error CORE_M4 or CORE_M0 is not defined for the LPC43xx architecture
#error CORE_M4 or CORE_M0 should be defined as part of your compiler define list
#endif

#ifndef CHIP_LPC43XX
#error The LPC43XX Chip include path is used for this build, but
#error CHIP_LPC43XX is not defined!
#endif

/** @defgroup PERIPH_43XX_BASE CHIP: LPC43xx Peripheral addresses and register set declarations
 * @ingroup CHIP_18XX_43XX_Drivers
 * @{
 */

#define LPC_SCT_BASE              0x40000000
#define LPC_GPDMA_BASE            0x40002000
#define LPC_SPIFI_BASE            0x40003000
#define LPC_SDMMC_BASE            0x40004000
#define LPC_EMC_BASE              0x40005000
#define LPC_USB0_BASE             0x40006000
#define LPC_USB1_BASE             0x40007000
#define LPC_LCD_BASE              0x40008000
#define LPC_FMCA_BASE             0x4000C000
#define LPC_FMCB_BASE             0x4000D000
#define LPC_ETHERNET_BASE         0x40010000
#define LPC_ATIMER_BASE           0x40040000
#define LPC_REGFILE_BASE          0x40041000
#define LPC_PMC_BASE              0x40042000
#define LPC_CREG_BASE             0x40043000
#define LPC_EVRT_BASE             0x40044000
#define LPC_RTC_BASE              0x40046000
#define LPC_CGU_BASE              0x40050000
#define LPC_CCU1_BASE             0x40051000
#define LPC_CCU2_BASE             0x40052000
#define LPC_RGU_BASE              0x40053000
#define LPC_WWDT_BASE             0x40080000
#define LPC_USART0_BASE           0x40081000
#define LPC_USART2_BASE           0x400C1000
#define LPC_USART3_BASE           0x400C2000
#define LPC_UART1_BASE            0x40082000
#define LPC_SSP0_BASE             0x40083000
#define LPC_SSP1_BASE             0x400C5000
#define LPC_TIMER0_BASE           0x40084000
#define LPC_TIMER1_BASE           0x40085000
#define LPC_TIMER2_BASE           0x400C3000
#define LPC_TIMER3_BASE           0x400C4000
#define LPC_SCU_BASE              0x40086000
#define LPC_PIN_INT_BASE          0x40087000
#define LPC_GPIO_GROUP_INT0_BASE  0x40088000
#define LPC_GPIO_GROUP_INT1_BASE  0x40089000
#define LPC_MCPWM_BASE            0x400A0000
#define LPC_I2C0_BASE             0x400A1000
#define LPC_I2C1_BASE             0x400E0000
#define LPC_I2S0_BASE             0x400A2000
#define LPC_I2S1_BASE             0x400A3000
#define LPC_C_CAN1_BASE           0x400A4000
#define LPC_RITIMER_BASE          0x400C0000
#define LPC_QEI_BASE              0x400C6000
#define LPC_GIMA_BASE             0x400C7000
#define LPC_DAC_BASE              0x400E1000
#define LPC_C_CAN0_BASE           0x400E2000
#define LPC_ADC0_BASE             0x400E3000
#define LPC_ADC1_BASE             0x400E4000
#define LPC_ADCHS_BASE            0x400F0000
#define LPC_GPIO_PORT_BASE        0x400F4000
#define LPC_SPI_BASE              0x40100000
#define LPC_SGPIO_BASE            0x40101000
#define LPC_EEPROM_BASE           0x4000E000
#define LPC_ROM_API_BASE          0x10400100

#define LPC_SCT                   ((LPC_SCT_T              *) LPC_SCT_BASE)
#define LPC_GPDMA                 ((LPC_GPDMA_T            *) LPC_GPDMA_BASE)
#define LPC_SDMMC                 ((LPC_SDMMC_T            *) LPC_SDMMC_BASE)
#define LPC_EMC                   ((LPC_EMC_T              *) LPC_EMC_BASE)
#define LPC_USB0                  ((LPC_USBHS_T            *) LPC_USB0_BASE)
#define LPC_USB1                  ((LPC_USBHS_T            *) LPC_USB1_BASE)
#define LPC_LCD                   ((LPC_LCD_T              *) LPC_LCD_BASE)
#define LPC_ETHERNET              ((LPC_ENET_T             *) LPC_ETHERNET_BASE)
#define LPC_ATIMER                ((LPC_ATIMER_T           *) LPC_ATIMER_BASE)
#define LPC_REGFILE               ((LPC_REGFILE_T          *) LPC_REGFILE_BASE)
#define LPC_PMC                   ((LPC_PMC_T              *) LPC_PMC_BASE)
#define LPC_EVRT                  ((LPC_EVRT_T             *) LPC_EVRT_BASE)
#define LPC_RTC                   ((LPC_RTC_T              *) LPC_RTC_BASE)
#define LPC_CGU                   ((LPC_CGU_T              *) LPC_CGU_BASE)
#define LPC_CCU1                  ((LPC_CCU1_T             *) LPC_CCU1_BASE)
#define LPC_CCU2                  ((LPC_CCU2_T             *) LPC_CCU2_BASE)
#define LPC_CREG                  ((LPC_CREG_T             *) LPC_CREG_BASE)
#define LPC_RGU                   ((LPC_RGU_T              *) LPC_RGU_BASE)
#define LPC_WWDT                  ((LPC_WWDT_T             *) LPC_WWDT_BASE)
#define LPC_USART0                ((LPC_USART_T            *) LPC_USART0_BASE)
#define LPC_USART2                ((LPC_USART_T            *) LPC_USART2_BASE)
#define LPC_USART3                ((LPC_USART_T            *) LPC_USART3_BASE)
#define LPC_UART1                 ((LPC_USART_T            *) LPC_UART1_BASE)
#define LPC_SSP0                  ((LPC_SSP_T              *) LPC_SSP0_BASE)
#define LPC_SSP1                  ((LPC_SSP_T              *) LPC_SSP1_BASE)
#define LPC_TIMER0                ((LPC_TIMER_T            *) LPC_TIMER0_BASE)
#define LPC_TIMER1                ((LPC_TIMER_T            *) LPC_TIMER1_BASE)
#define LPC_TIMER2                ((LPC_TIMER_T            *) LPC_TIMER2_BASE)
#define LPC_TIMER3                ((LPC_TIMER_T            *) LPC_TIMER3_BASE)
#define LPC_SCU                   ((LPC_SCU_T              *) LPC_SCU_BASE)
#define LPC_GPIO_PIN_INT          ((LPC_PIN_INT_T          *) LPC_PIN_INT_BASE)
#define LPC_GPIOGROUP             ((LPC_GPIOGROUPINT_T     *) LPC_GPIO_GROUP_INT0_BASE)
#define LPC_MCPWM                 ((LPC_MCPWM_T            *) LPC_MCPWM_BASE)
#define LPC_I2C0                  ((LPC_I2C_T              *) LPC_I2C0_BASE)
#define LPC_I2C1                  ((LPC_I2C_T              *) LPC_I2C1_BASE)
#define LPC_I2S0                  ((LPC_I2S_T              *) LPC_I2S0_BASE)
#define LPC_I2S1                  ((LPC_I2S_T              *) LPC_I2S1_BASE)
#define LPC_C_CAN1                ((LPC_CCAN_T             *) LPC_C_CAN1_BASE)
#define LPC_RITIMER               ((LPC_RITIMER_T          *) LPC_RITIMER_BASE)
#define LPC_QEI                   ((LPC_QEI_T              *) LPC_QEI_BASE)
#define LPC_GIMA                  ((LPC_GIMA_T             *) LPC_GIMA_BASE)
#define LPC_DAC                   ((LPC_DAC_T              *) LPC_DAC_BASE)
#define LPC_C_CAN0                ((LPC_CCAN_T             *) LPC_C_CAN0_BASE)
#define LPC_ADC0                  ((LPC_ADC_T              *) LPC_ADC0_BASE)
#define LPC_ADC1                  ((LPC_ADC_T              *) LPC_ADC1_BASE)
#define LPC_ADCHS                 ((LPC_HSADC_T            *) LPC_ADCHS_BASE)
#define LPC_GPIO_PORT             ((LPC_GPIO_T             *) LPC_GPIO_PORT_BASE)
#define LPC_SPI                   ((LPC_SPI_T              *) LPC_SPI_BASE)
#define LPC_SGPIO                 ((LPC_SGPIO_T            *) LPC_SGPIO_BASE)
#define LPC_EEPROM                ((LPC_EEPROM_T           *) LPC_EEPROM_BASE)
#define LPC_FMCA                  ((LPC_FMC_T              *) LPC_FMCA_BASE)
#define LPC_FMC                   ((LPC_FMC_T            * *) LPC_FMCA_BASE)
#define LPC_FMCB                  ((LPC_FMC_T              *) LPC_FMCB_BASE)
#define LPC_ROM_API               ((LPC_ROM_API_T          *) LPC_ROM_API_BASE)

/**
 * @}
 */

#include "scu_18xx_43xx.h"
#include "clock_18xx_43xx.h"
#include "rgu_18xx_43xx.h"
#include "creg_18xx_43xx.h"
#include "evrt_18xx_43xx.h"
#include "otp_18xx_43xx.h"
#include "sdif_18xx_43xx.h"
#include "adc_18xx_43xx.h"
#include "hsadc_18xx_43xx.h"
#include "atimer_18xx_43xx.h"
#include "aes_18xx_43xx.h"
#include "ccan_18xx_43xx.h"
#include "dac_18xx_43xx.h"
#include "eeprom_18xx_43xx.h"
#include "emc_18xx_43xx.h"
#include "enet_18xx_43xx.h"
#include "fmc_18xx_43xx.h"
#include "i2c_18xx_43xx.h"
#include "i2s_18xx_43xx.h"
#include "gima_18xx_43xx.h"
#include "gpdma_18xx_43xx.h"
#include "gpio_18xx_43xx.h"
#include "pinint_18xx_43xx.h"
#include "gpiogroup_18xx_43xx.h"
#include "lcd_18xx_43xx.h"
#include "mcpwm_18xx_43xx.h"
#include "pmc_18xx_43xx.h"
#include "qei_18xx_43xx.h"
#include "ritimer_18xx_43xx.h"
#include "rtc_18xx_43xx.h"
#include "sct_18xx_43xx.h"
#include "sct_pwm_18xx_43xx.h"
#include "sdmmc_18xx_43xx.h"
#include "sdio_18xx_43xx.h"
#include "sgpio_18xx_43xx.h"
#include "spi_18xx_43xx.h"
#include "ssp_18xx_43xx.h"
#include "timer_18xx_43xx.h"
#include "uart_18xx_43xx.h"
#include "usbhs_18xx_43xx.h"
#include "wwdt_18xx_43xx.h"
#include "romapi_18xx_43xx.h"
#include "i2cm_18xx_43xx.h"

#if defined(CORE_M4)
#include "fpu_init.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __CHIP_LPC43XX_H_ */
