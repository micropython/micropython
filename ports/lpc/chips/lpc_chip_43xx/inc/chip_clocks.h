/*
 * @brief	LPC18xx/43xx chip clock list used by CGU and CCU drivers
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
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
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __CHIP_CLOCKS_H_
#define __CHIP_CLOCKS_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup CLOCK_18XX_43XX
 * @{
 */

/**
 * @brief CGU clock input list
 * These are possible input clocks for the CGU and can come
 * from both external (crystal) and internal (PLL) sources. These
 * clock inputs can be routed to the base clocks (@ref CHIP_CGU_BASE_CLK_T).
 */
typedef enum CHIP_CGU_CLKIN {
	CLKIN_32K,		/*!< External 32KHz input */
	CLKIN_IRC,		/*!< Internal IRC (12MHz) input */
	CLKIN_ENET_RX,	/*!< External ENET_RX pin input */
	CLKIN_ENET_TX,	/*!< External ENET_TX pin input */
	CLKIN_CLKIN,	/*!< External GPCLKIN pin input */
	CLKIN_RESERVED1,
	CLKIN_CRYSTAL,	/*!< External (main) crystal pin input */
	CLKIN_USBPLL,	/*!< Internal USB PLL input */
	CLKIN_AUDIOPLL,	/*!< Internal Audio PLL input */
	CLKIN_MAINPLL,	/*!< Internal Main PLL input */
	CLKIN_RESERVED2,
	CLKIN_RESERVED3,
	CLKIN_IDIVA,	/*!< Internal divider A input */
	CLKIN_IDIVB,	/*!< Internal divider B input */
	CLKIN_IDIVC,	/*!< Internal divider C input */
	CLKIN_IDIVD,	/*!< Internal divider D input */
	CLKIN_IDIVE,	/*!< Internal divider E input */
	CLKINPUT_PD		/*!< External 32KHz input */
} CHIP_CGU_CLKIN_T;

/**
 * @brief CGU base clocks
 * CGU base clocks are clocks that are associated with a single input clock
 * and are routed out to 1 or more peripherals. For example, the CLK_BASE_PERIPH
 * clock can be configured to use the CLKIN_MAINPLL input clock, which will in
 * turn route that clock to the CLK_PERIPH_BUS, CLK_PERIPH_CORE, and
 * CLK_PERIPH_SGPIO periphral clocks.
 */
typedef enum CHIP_CGU_BASE_CLK {
	CLK_BASE_SAFE,		/*!< Base clock for WDT oscillator, IRC input only */
	CLK_BASE_USB0,		/*!< Base USB clock for USB0, USB PLL input only */
#if defined(CHIP_LPC43XX)
	CLK_BASE_PERIPH,	/*!< Base clock for SGPIO */
#else
	CLK_BASE_RESERVED1,
#endif
	CLK_BASE_USB1,		/*!< Base USB clock for USB1 */
	CLK_BASE_MX,		/*!< Base clock for CPU core */
	CLK_BASE_SPIFI,		/*!< Base clock for SPIFI */
#if defined(CHIP_LPC43XX)
	CLK_BASE_SPI,		/*!< Base clock for SPI */
#else
	CLK_BASE_RESERVED2,
#endif
	CLK_BASE_PHY_RX,	/*!< Base clock for PHY RX */
	CLK_BASE_PHY_TX,	/*!< Base clock for PHY TX */
	CLK_BASE_APB1,		/*!< Base clock for APB1 group */
	CLK_BASE_APB3,		/*!< Base clock for APB3 group */
	CLK_BASE_LCD,		/*!< Base clock for LCD pixel clock */
#if defined(CHIP_LPC43XX)
	CLK_BASE_ADCHS,		/*!< Base clock for ADCHS */
#else
	CLK_BASE_RESERVED3,
#endif
	CLK_BASE_SDIO,		/*!< Base clock for SDIO */
	CLK_BASE_SSP0,		/*!< Base clock for SSP0 */
	CLK_BASE_SSP1,		/*!< Base clock for SSP1 */
	CLK_BASE_UART0,		/*!< Base clock for UART0 */
	CLK_BASE_UART1,		/*!< Base clock for UART1 */
	CLK_BASE_UART2,		/*!< Base clock for UART2 */
	CLK_BASE_UART3,		/*!< Base clock for UART3 */
	CLK_BASE_OUT,		/*!< Base clock for CLKOUT pin */
	CLK_BASE_RESERVED4,
	CLK_BASE_RESERVED5,
	CLK_BASE_RESERVED6,
	CLK_BASE_RESERVED7,
	CLK_BASE_APLL,		/*!< Base clock for audio PLL */
	CLK_BASE_CGU_OUT0,	/*!< Base clock for CGUOUT0 pin */
	CLK_BASE_CGU_OUT1,	/*!< Base clock for CGUOUT1 pin */
	CLK_BASE_LAST,
	CLK_BASE_NONE = CLK_BASE_LAST
} CHIP_CGU_BASE_CLK_T;

/**
 * @brief CGU dividers
 * CGU dividers provide an extra clock state where a specific clock can be
 * divided before being routed to a peripheral group. A divider accepts an
 * input clock and then divides it. To use the divided clock for a base clock
 * group, use the divider as the input clock for the base clock (for example,
 * use CLKIN_IDIVB, where CLKIN_MAINPLL might be the input into the divider).
 */
typedef enum CHIP_CGU_IDIV {
	CLK_IDIV_A,		/*!< CGU clock divider A */
	CLK_IDIV_B,		/*!< CGU clock divider B */
	CLK_IDIV_C,		/*!< CGU clock divider A */
	CLK_IDIV_D,		/*!< CGU clock divider D */
	CLK_IDIV_E,		/*!< CGU clock divider E */
	CLK_IDIV_LAST
} CHIP_CGU_IDIV_T;

#define CHIP_CGU_IDIV_MASK(x)  ("\x03\x0F\x0F\x0F\xFF"[x])

/**
 * @brief Peripheral clocks
 * Peripheral clocks are individual clocks routed to peripherals. Although
 * multiple peripherals may share a same base clock, each peripheral's clock
 * can be enabled or disabled individually. Some peripheral clocks also have
 * additional dividers associated with them.
 */
typedef enum CHIP_CCU_CLK {
	/* CCU1 clocks */
	CLK_APB3_BUS,		/*!< APB3 bus clock from base clock CLK_BASE_APB3 */
	CLK_APB3_I2C1,		/*!< I2C1 register/perigheral clock from base clock CLK_BASE_APB3 */
	CLK_APB3_DAC,		/*!< DAC peripheral clock from base clock CLK_BASE_APB3 */
	CLK_APB3_ADC0,		/*!< ADC0 register/perigheral clock from base clock CLK_BASE_APB3 */
	CLK_APB3_ADC1,		/*!< ADC1 register/perigheral clock from base clock CLK_BASE_APB3 */
	CLK_APB3_CAN0,		/*!< CAN0 register/perigheral clock from base clock CLK_BASE_APB3 */
	CLK_APB1_BUS = 32,	/*!< APB1 bus clock clock from base clock CLK_BASE_APB1 */
	CLK_APB1_MOTOCON,	/*!< Motor controller register/perigheral clock from base clock CLK_BASE_APB1 */
	CLK_APB1_I2C0,		/*!< I2C0 register/perigheral clock from base clock CLK_BASE_APB1 */
	CLK_APB1_I2S,		/*!< I2S register/perigheral clock from base clock CLK_BASE_APB1 */
	CLK_APB1_CAN1,		/*!< CAN1 register/perigheral clock from base clock CLK_BASE_APB1 */
	CLK_SPIFI = 64,		/*!< SPIFI SCKI input clock from base clock CLK_BASE_SPIFI */
	CLK_MX_BUS = 96,	/*!< M3/M4 BUS core clock from base clock CLK_BASE_MX */
	CLK_MX_SPIFI,		/*!< SPIFI register clock from base clock CLK_BASE_MX */
	CLK_MX_GPIO,		/*!< GPIO register clock from base clock CLK_BASE_MX */
	CLK_MX_LCD,			/*!< LCD register clock from base clock CLK_BASE_MX */
	CLK_MX_ETHERNET,	/*!< ETHERNET register clock from base clock CLK_BASE_MX */
	CLK_MX_USB0,		/*!< USB0 register clock from base clock CLK_BASE_MX */
	CLK_MX_EMC,			/*!< EMC clock from base clock CLK_BASE_MX */
	CLK_MX_SDIO,		/*!< SDIO register clock from base clock CLK_BASE_MX */
	CLK_MX_DMA,			/*!< DMA register clock from base clock CLK_BASE_MX */
	CLK_MX_MXCORE,		/*!< M3/M4 CPU core clock from base clock CLK_BASE_MX */
	RESERVED_ALIGN = CLK_MX_MXCORE + 3,
	CLK_MX_SCT,			/*!< SCT register clock from base clock CLK_BASE_MX */
	CLK_MX_USB1,		/*!< USB1 register clock from base clock CLK_BASE_MX */
	CLK_MX_EMC_DIV,		/*!< ENC divider clock from base clock CLK_BASE_MX */
	CLK_MX_FLASHA,		/*!< FLASHA bank clock from base clock CLK_BASE_MX */
	CLK_MX_FLASHB,		/*!< FLASHB bank clock from base clock CLK_BASE_MX */
#if defined(CHIP_LPC43XX)
	CLK_M4_M0APP,		/*!< M0 app CPU core clock from base clock CLK_BASE_MX */
	CLK_MX_ADCHS,		/*!< ADCHS clock from base clock CLK_BASE_ADCHS */
#else
	CLK_RESERVED1,
	CLK_RESERVED2,
#endif
	CLK_MX_EEPROM,		/*!< EEPROM clock from base clock CLK_BASE_MX */
	CLK_MX_WWDT = 128,	/*!< WWDT register clock from base clock CLK_BASE_MX */
	CLK_MX_UART0,		/*!< UART0 register clock from base clock CLK_BASE_MX */
	CLK_MX_UART1,		/*!< UART1 register clock from base clock CLK_BASE_MX */
	CLK_MX_SSP0,		/*!< SSP0 register clock from base clock CLK_BASE_MX */
	CLK_MX_TIMER0,		/*!< TIMER0 register/perigheral clock from base clock CLK_BASE_MX */
	CLK_MX_TIMER1,		/*!< TIMER1 register/perigheral clock from base clock CLK_BASE_MX */
	CLK_MX_SCU,			/*!< SCU register/perigheral clock from base clock CLK_BASE_MX */
	CLK_MX_CREG,		/*!< CREG clock from base clock CLK_BASE_MX */
	CLK_MX_RITIMER = 160,	/*!< RITIMER register/perigheral clock from base clock CLK_BASE_MX */
	CLK_MX_UART2,		/*!< UART3 register clock from base clock CLK_BASE_MX */
	CLK_MX_UART3,		/*!< UART4 register clock from base clock CLK_BASE_MX */
	CLK_MX_TIMER2,		/*!< TIMER2 register/perigheral clock from base clock CLK_BASE_MX */
	CLK_MX_TIMER3,		/*!< TIMER3 register/perigheral clock from base clock CLK_BASE_MX */
	CLK_MX_SSP1,		/*!< SSP1 register clock from base clock CLK_BASE_MX */
	CLK_MX_QEI,			/*!< QEI register/perigheral clock from base clock CLK_BASE_MX */
#if defined(CHIP_LPC43XX)
	CLK_PERIPH_BUS = 192,	/*!< Peripheral bus clock from base clock CLK_BASE_PERIPH */
	CLK_RESERVED3,
	CLK_PERIPH_CORE,	/*!< Peripheral core clock from base clock CLK_BASE_PERIPH */
	CLK_PERIPH_SGPIO,	/*!< SGPIO clock from base clock CLK_BASE_PERIPH */
#else
	CLK_RESERVED3 = 192,
	CLK_RESERVED3A,
	CLK_RESERVED4,
	CLK_RESERVED5,
#endif
	CLK_USB0 = 224,			/*!< USB0 clock from base clock CLK_BASE_USB0 */
	CLK_USB1 = 256,			/*!< USB1 clock from base clock CLK_BASE_USB1 */
#if defined(CHIP_LPC43XX)
	CLK_SPI = 288,			/*!< SPI clock from base clock CLK_BASE_SPI */
	CLK_ADCHS = 320,		/*!< ADCHS clock from base clock CLK_BASE_ADCHS */
#else
	CLK_RESERVED7 = 320,
	CLK_RESERVED8,
#endif
	CLK_CCU1_LAST,

	/* CCU2 clocks */
	CLK_CCU2_START,
	CLK_APLL = CLK_CCU2_START,	/*!< Audio PLL clock from base clock CLK_BASE_APLL */
	RESERVED_ALIGNB = CLK_CCU2_START + 31,
	CLK_APB2_UART3,			/*!< UART3 clock from base clock CLK_BASE_UART3 */
	RESERVED_ALIGNC = CLK_CCU2_START + 63,
	CLK_APB2_UART2,			/*!< UART2 clock from base clock CLK_BASE_UART2 */
	RESERVED_ALIGND = CLK_CCU2_START + 95,
	CLK_APB0_UART1,			/*!< UART1 clock from base clock CLK_BASE_UART1 */
	RESERVED_ALIGNE = CLK_CCU2_START + 127,
	CLK_APB0_UART0,			/*!< UART0 clock from base clock CLK_BASE_UART0 */
	RESERVED_ALIGNF = CLK_CCU2_START + 159,
	CLK_APB2_SSP1,			/*!< SSP1 clock from base clock CLK_BASE_SSP1 */
	RESERVED_ALIGNG = CLK_CCU2_START + 191,
	CLK_APB0_SSP0,			/*!< SSP0 clock from base clock CLK_BASE_SSP0 */
	RESERVED_ALIGNH = CLK_CCU2_START + 223,
	CLK_APB2_SDIO,			/*!< SDIO clock from base clock CLK_BASE_SDIO */
	CLK_CCU2_LAST
} CHIP_CCU_CLK_T;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __CHIP_CLOCKS_H_ */






