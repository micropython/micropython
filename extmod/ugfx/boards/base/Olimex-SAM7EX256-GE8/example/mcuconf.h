/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/

/*
 * AT91SAM7 drivers configuration.
 * The following settings override the default settings present in
 * the various device driver implementation headers.
 * Note that the settings for each driver only have effect if the driver
 * is enabled in halconf.h.
 */

#define HAL_USE_PAL                 TRUE		// Used by all sorts of IO
#define HAL_USE_ICU                 FALSE
#define HAL_USE_TM                  FALSE
#define HAL_USE_EXT                 FALSE
#define HAL_USE_MMC_SPI             FALSE
#define HAL_USE_RTC                 FALSE
#define HAL_USE_SDC                 FALSE
#define HAL_USE_SERIAL_USB          FALSE
#define HAL_USE_USB                 FALSE

/*
 * ADC driver system settings.
 */
#define HAL_USE_ADC                 TRUE		// Used by audio-in

/*
 * CAN driver system settings.
 */
#define HAL_USE_CAN                 FALSE

/*
 * MAC driver system settings.
 */
#define HAL_USE_MAC                 FALSE
#define MAC_TRANSMIT_BUFFERS        2
#define MAC_RECEIVE_BUFFERS         2
#define MAC_BUFFERS_SIZE            1518
#define EMAC_INTERRUPT_PRIORITY     (AT91C_AIC_PRIOR_HIGHEST - 3)

/*
 * PWM driver system settings.
 */
#define HAL_USE_PWM                 TRUE
#define PWM_USE_PWM1				TRUE		// Used by audio-out
#define PWM_USE_PWM2				TRUE		// Used by back-light
#define PWM_USE_PWM3				FALSE
#define PWM_USE_PWM4				FALSE

/*
 * SERIAL driver system settings.
 */
#define HAL_USE_SERIAL              FALSE
#define HAL_USE_UART                FALSE
#define USE_SAM7_USART0             TRUE
#define USE_SAM7_USART1             TRUE
#define SAM7_USART0_PRIORITY        (AT91C_AIC_PRIOR_HIGHEST - 2)
#define SAM7_USART1_PRIORITY        (AT91C_AIC_PRIOR_HIGHEST - 2)

/*
 * SPI driver system settings.
 */
#define USE_AT91SAM7_SPI            FALSE
#define HAL_USE_SPI                 FALSE
#define AT91SAM7_SPI_USE_SPI0       FALSE			// Used by LCD but addressed directly
#define AT91SAM7_SPI_USE_SPI1       FALSE			// Used by lots of UEXT devices
#define AT91SAM7_SPI0_PRIORITY      (AT91C_AIC_PRIOR_HIGHEST - 1)
#define AT91SAM7_SPI1_PRIORITY      (AT91C_AIC_PRIOR_HIGHEST - 1)

/*
 * GPT driver system settings.
 */
#define HAL_USE_GPT                 TRUE
#define AT91_GPT_USE_TC0            FALSE			// used internally by ADC driver
#define AT91_GPT_USE_TC1            TRUE			// uGFX used for audio-out
#define AT91_GPT_USE_TC2            FALSE

/*
 * I2C driver system settings
 */
#define SAM7_I2C_USE_I2C1           TRUE
#define HAL_USE_I2C                 TRUE
