/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v3.0
processor: MK64FN1M0xxx12
package_id: MK64FN1M0VLL12
mcu_data: ksdk2_0
processor_version: 2.0.0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

#include "fsl_common.h"
#include "fsl_port.h"
#include "pin_mux.h"



#define PIN0_IDX                         0u   /*!< Pin number for pin 0 in a port */
#define PIN1_IDX                         1u   /*!< Pin number for pin 1 in a port */
#define PIN2_IDX                         2u   /*!< Pin number for pin 2 in a port */
#define PIN3_IDX                         3u   /*!< Pin number for pin 3 in a port */
#define PIN4_IDX                         4u   /*!< Pin number for pin 4 in a port */
#define PIN5_IDX                         5u   /*!< Pin number for pin 5 in a port */
#define PIN6_IDX                         6u   /*!< Pin number for pin 6 in a port */
#define PIN16_IDX                       16u   /*!< Pin number for pin 16 in a port */
#define PIN17_IDX                       17u   /*!< Pin number for pin 17 in a port */
#define SOPT5_UART0TXSRC_UART_TX      0x00u   /*!< UART 0 transmit data source select: UART0_TX pin */
#define SOPT5_UART1TXSRC_UART_TX 0x00u /*!<@brief UART 1 transmit data source select: UART1_TX pin */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitPins:
- options: {callFromInitBoot: 'true', coreID: core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '62', peripheral: UART0, signal: RX, pin_signal: PTB16/SPI1_SOUT/UART0_RX/FTM_CLKIN0/FB_AD17/EWM_IN}
  - {pin_num: '63', peripheral: UART0, signal: TX, pin_signal: PTB17/SPI1_SIN/UART0_TX/FTM_CLKIN1/FB_AD16/EWM_OUT_b}
  - {pin_num: '1', peripheral: SDHC, signal: 'DATA, 1', pin_signal: ADC1_SE4a/PTE0/SPI1_PCS1/UART1_TX/SDHC0_D1/TRACE_CLKOUT/I2C1_SDA/RTC_CLKOUT, slew_rate: fast,
    open_drain: disable, drive_strength: high, pull_select: up, pull_enable: enable, passive_filter: disable}
  - {pin_num: '2', peripheral: SDHC, signal: 'DATA, 0', pin_signal: ADC1_SE5a/PTE1/LLWU_P0/SPI1_SOUT/UART1_RX/SDHC0_D0/TRACE_D3/I2C1_SCL/SPI1_SIN, slew_rate: fast,
    open_drain: disable, drive_strength: high, pull_select: up, pull_enable: enable, passive_filter: disable}
  - {pin_num: '3', peripheral: SDHC, signal: DCLK, pin_signal: ADC0_DP2/ADC1_SE6a/PTE2/LLWU_P1/SPI1_SCK/UART1_CTS_b/SDHC0_DCLK/TRACE_D2, slew_rate: fast, open_drain: disable,
    drive_strength: high, pull_select: up, pull_enable: enable, passive_filter: disable}
  - {pin_num: '4', peripheral: SDHC, signal: CMD, pin_signal: ADC0_DM2/ADC1_SE7a/PTE3/SPI1_SIN/UART1_RTS_b/SDHC0_CMD/TRACE_D1/SPI1_SOUT, slew_rate: fast, open_drain: disable,
    drive_strength: high, pull_select: up, pull_enable: enable, passive_filter: disable}
  - {pin_num: '5', peripheral: SDHC, signal: 'DATA, 3', pin_signal: PTE4/LLWU_P2/SPI1_PCS0/UART3_TX/SDHC0_D3/TRACE_D0, slew_rate: fast, open_drain: disable, drive_strength: high,
    pull_select: up, pull_enable: enable, passive_filter: disable}
  - {pin_num: '6', peripheral: SDHC, signal: 'DATA, 2', pin_signal: PTE5/SPI1_PCS2/UART3_RX/SDHC0_D2/FTM3_CH0, slew_rate: fast, open_drain: disable, drive_strength: high,
    pull_select: up, pull_enable: enable, passive_filter: disable}
  - {pin_num: '7', peripheral: GPIOE, signal: 'GPIO, 6', pin_signal: PTE6/SPI1_PCS3/UART3_CTS_b/I2S0_MCLK/FTM3_CH1/USB_SOF_OUT, slew_rate: fast, open_drain: disable,
    drive_strength: low, pull_select: down, pull_enable: enable, passive_filter: disable}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitPins(void) {
  CLOCK_EnableClock(kCLOCK_PortB);                           /* Port B Clock Gate Control: Clock enabled */
  CLOCK_EnableClock(kCLOCK_PortE);                           /* Port E Clock Gate Control: Clock enabled */

  PORT_SetPinMux(PORTB, PIN16_IDX, kPORT_MuxAlt3);           /* PORTB16 (pin 62) is configured as UART0_RX */
  PORT_SetPinMux(PORTB, PIN17_IDX, kPORT_MuxAlt3);           /* PORTB17 (pin 63) is configured as UART0_TX */
  const port_pin_config_t porte0_pin1_config = {
    kPORT_PullUp,                                            /* Internal pull-up resistor is enabled */
    kPORT_FastSlewRate,                                      /* Fast slew rate is configured */
    kPORT_PassiveFilterDisable,                              /* Passive filter is disabled */
    kPORT_OpenDrainDisable,                                  /* Open drain is disabled */
    kPORT_HighDriveStrength,                                 /* High drive strength is configured */
    kPORT_MuxAlt4,                                           /* Pin is configured as SDHC0_D1 */
    kPORT_UnlockRegister                                     /* Pin Control Register fields [15:0] are not locked */
  };
  PORT_SetPinConfig(PORTE, PIN0_IDX, &porte0_pin1_config);   /* PORTE0 (pin 1) is configured as SDHC0_D1 */
  const port_pin_config_t porte1_pin2_config = {
    kPORT_PullUp,                                            /* Internal pull-up resistor is enabled */
    kPORT_FastSlewRate,                                      /* Fast slew rate is configured */
    kPORT_PassiveFilterDisable,                              /* Passive filter is disabled */
    kPORT_OpenDrainDisable,                                  /* Open drain is disabled */
    kPORT_HighDriveStrength,                                 /* High drive strength is configured */
    kPORT_MuxAlt4,                                           /* Pin is configured as SDHC0_D0 */
    kPORT_UnlockRegister                                     /* Pin Control Register fields [15:0] are not locked */
  };
  PORT_SetPinConfig(PORTE, PIN1_IDX, &porte1_pin2_config);   /* PORTE1 (pin 2) is configured as SDHC0_D0 */
  const port_pin_config_t porte2_pin3_config = {
    kPORT_PullUp,                                            /* Internal pull-up resistor is enabled */
    kPORT_FastSlewRate,                                      /* Fast slew rate is configured */
    kPORT_PassiveFilterDisable,                              /* Passive filter is disabled */
    kPORT_OpenDrainDisable,                                  /* Open drain is disabled */
    kPORT_HighDriveStrength,                                 /* High drive strength is configured */
    kPORT_MuxAlt4,                                           /* Pin is configured as SDHC0_DCLK */
    kPORT_UnlockRegister                                     /* Pin Control Register fields [15:0] are not locked */
  };
  PORT_SetPinConfig(PORTE, PIN2_IDX, &porte2_pin3_config);   /* PORTE2 (pin 3) is configured as SDHC0_DCLK */
  const port_pin_config_t porte3_pin4_config = {
    kPORT_PullUp,                                            /* Internal pull-up resistor is enabled */
    kPORT_FastSlewRate,                                      /* Fast slew rate is configured */
    kPORT_PassiveFilterDisable,                              /* Passive filter is disabled */
    kPORT_OpenDrainDisable,                                  /* Open drain is disabled */
    kPORT_HighDriveStrength,                                 /* High drive strength is configured */
    kPORT_MuxAlt4,                                           /* Pin is configured as SDHC0_CMD */
    kPORT_UnlockRegister                                     /* Pin Control Register fields [15:0] are not locked */
  };
  PORT_SetPinConfig(PORTE, PIN3_IDX, &porte3_pin4_config);   /* PORTE3 (pin 4) is configured as SDHC0_CMD */
  const port_pin_config_t porte4_pin5_config = {
    kPORT_PullUp,                                            /* Internal pull-up resistor is enabled */
    kPORT_FastSlewRate,                                      /* Fast slew rate is configured */
    kPORT_PassiveFilterDisable,                              /* Passive filter is disabled */
    kPORT_OpenDrainDisable,                                  /* Open drain is disabled */
    kPORT_HighDriveStrength,                                 /* High drive strength is configured */
    kPORT_MuxAlt4,                                           /* Pin is configured as SDHC0_D3 */
    kPORT_UnlockRegister                                     /* Pin Control Register fields [15:0] are not locked */
  };
  PORT_SetPinConfig(PORTE, PIN4_IDX, &porte4_pin5_config);   /* PORTE4 (pin 5) is configured as SDHC0_D3 */
  const port_pin_config_t porte5_pin6_config = {
    kPORT_PullUp,                                            /* Internal pull-up resistor is enabled */
    kPORT_FastSlewRate,                                      /* Fast slew rate is configured */
    kPORT_PassiveFilterDisable,                              /* Passive filter is disabled */
    kPORT_OpenDrainDisable,                                  /* Open drain is disabled */
    kPORT_HighDriveStrength,                                 /* High drive strength is configured */
    kPORT_MuxAlt4,                                           /* Pin is configured as SDHC0_D2 */
    kPORT_UnlockRegister                                     /* Pin Control Register fields [15:0] are not locked */
  };
  PORT_SetPinConfig(PORTE, PIN5_IDX, &porte5_pin6_config);   /* PORTE5 (pin 6) is configured as SDHC0_D2 */
  const port_pin_config_t porte6_pin7_config = {
    kPORT_PullDown,                                          /* Internal pull-down resistor is enabled */
    kPORT_FastSlewRate,                                      /* Fast slew rate is configured */
    kPORT_PassiveFilterDisable,                              /* Passive filter is disabled */
    kPORT_OpenDrainDisable,                                  /* Open drain is disabled */
    kPORT_LowDriveStrength,                                  /* Low drive strength is configured */
    kPORT_MuxAsGpio,                                         /* Pin is configured as PTE6 */
    kPORT_UnlockRegister                                     /* Pin Control Register fields [15:0] are not locked */
  };
  PORT_SetPinConfig(PORTE, PIN6_IDX, &porte6_pin7_config);   /* PORTE6 (pin 7) is configured as PTE6 */
  SIM->SOPT5 = ((SIM->SOPT5 &
    (~(SIM_SOPT5_UART0TXSRC_MASK)))                          /* Mask bits to zero which are setting */
      | SIM_SOPT5_UART0TXSRC(SOPT5_UART0TXSRC_UART_TX)       /* UART 0 transmit data source select: UART0_TX pin */
    );
}

void DSPI0_InitPins(void)
{
    /* Port A Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortA);
    /* Port C Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortC);

    /* PORTA14 (pin L10) is configured as SPI0_PCS0 */
    PORT_SetPinMux(PORTA, 14U, kPORT_MuxAlt2);

    PORTA->PCR[14] = ((PORTA->PCR[14] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_ODE_MASK | PORT_PCR_ISF_MASK)))

                      /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
                      | PORT_PCR_ODE(kPORT_OpenDrainDisable));

    /* PORTA15 (pin L11) is configured as SPI0_SCK */
    PORT_SetPinMux(PORTA, 15U, kPORT_MuxAlt2);

    PORTA->PCR[15] = ((PORTA->PCR[15] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_ODE_MASK | PORT_PCR_ISF_MASK)))

                      /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
                      | PORT_PCR_ODE(kPORT_OpenDrainDisable));

    /* PORTC6 (pin C8) is configured as SPI0_SOUT */
    PORT_SetPinMux(PORTC, 6U, kPORT_MuxAlt2);

    PORTC->PCR[6] = ((PORTC->PCR[6] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_ODE_MASK | PORT_PCR_ISF_MASK)))

                     /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
                     | PORT_PCR_ODE(kPORT_OpenDrainDisable));

    /* PORTC7 (pin B8) is configured as SPI0_SIN */
    PORT_SetPinMux(PORTC, 7U, kPORT_MuxAlt2);

    PORTC->PCR[7] = ((PORTC->PCR[7] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_ODE_MASK | PORT_PCR_ISF_MASK)))

                     /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
                     | PORT_PCR_ODE(kPORT_OpenDrainDisable));
}

void DSPI0_DeinitPins(void)
{
    /* Port A Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortA);
    /* Port C Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortC);

    /* PORTA14 (pin L10) is disabled */
    PORT_SetPinMux(PORTA, 14U, kPORT_PinDisabledOrAnalog);

    /* PORTA15 (pin L11) is disabled */
    PORT_SetPinMux(PORTA, 15U, kPORT_PinDisabledOrAnalog);

    /* PORTC6 (pin C8) is configured as CMP0_IN0 */
    PORT_SetPinMux(PORTC, 6U, kPORT_PinDisabledOrAnalog);

    /* PORTC7 (pin B8) is configured as CMP0_IN1 */
    PORT_SetPinMux(PORTC, 7U, kPORT_PinDisabledOrAnalog);
}

void DSPI1_InitPins(void)
{
  /* Port B Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortB);
    /* Port D Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortD);
    /* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);

    /* PORTB10 (pin E12) is configured as SPI1_PCS0 */
    PORT_SetPinMux(PORTB, 10U, kPORT_MuxAlt2);

    PORTB->PCR[10] = ((PORTB->PCR[10] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_PS_MASK | PORT_PCR_ISF_MASK)))

                      /* Pull Select: Internal pulldown resistor is enabled on the corresponding pin, if the
                       * corresponding PE field is set. */
                      | PORT_PCR_PS(kPORT_PullDown));

    /* PORTD6 (pin A2) is configured as SPI1_SOUT */
    PORT_SetPinMux(PORTD, 6U, kPORT_MuxAlt7);

    PORTD->PCR[6] = ((PORTD->PCR[6] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_PS_MASK | PORT_PCR_ISF_MASK)))

                     /* Pull Select: Internal pulldown resistor is enabled on the corresponding pin, if the
                      * corresponding PE field is set. */
                     | PORT_PCR_PS(kPORT_PullDown));

    /* PORTE2 (pin D1) is configured as SPI1_SCK */
    PORT_SetPinMux(PORTE, 2U, kPORT_MuxAlt2);

    PORTE->PCR[2] = ((PORTE->PCR[2] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_PS_MASK | PORT_PCR_ISF_MASK)))

                     /* Pull Select: Internal pulldown resistor is enabled on the corresponding pin, if the
                      * corresponding PE field is set. */
                     | PORT_PCR_PS(kPORT_PullDown));

    /* PORTE3 (pin E4) is configured as SPI1_SIN */
    PORT_SetPinMux(PORTE, 3U, kPORT_MuxAlt2);

    PORTE->PCR[3] = ((PORTE->PCR[3] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_PS_MASK | PORT_PCR_ISF_MASK)))

                     /* Pull Select: Internal pulldown resistor is enabled on the corresponding pin, if the
                      * corresponding PE field is set. */
                     | PORT_PCR_PS(kPORT_PullDown));
}

void DSPI1_DeinitPins(void)
{
      /* Port B Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortB);
    /* Port D Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortD);
    /* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);

    /* PORTB10 (pin E12) is configured as PTB10 */
    PORT_SetPinMux(PORTB, 10U, kPORT_MuxAsGpio);

    /* PORTD6 (pin A2) is configured as PTD6 */
    PORT_SetPinMux(PORTD, 6U, kPORT_MuxAsGpio);

    /* PORTE2 (pin D1) is configured as PTE2 */
    PORT_SetPinMux(PORTE, 2U, kPORT_MuxAsGpio);

    /* PORTE3 (pin E4) is configured as PTE3 */
    PORT_SetPinMux(PORTE, 3U, kPORT_MuxAsGpio);
}

void DSPI2_InitPins(void)
{
  /* Port B Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortB);

    /* PORTB20 (pin D10) is configured as SPI2_PCS0 */
    PORT_SetPinMux(PORTB, 20U, kPORT_MuxAlt2);

    PORTB->PCR[20] = ((PORTB->PCR[20] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_ODE_MASK | PORT_PCR_ISF_MASK)))

                      /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
                      | PORT_PCR_ODE(kPORT_OpenDrainDisable));

    /* PORTB21 (pin D9) is configured as SPI2_SCK */
    PORT_SetPinMux(PORTB, 21U, kPORT_MuxAlt2);

    PORTB->PCR[21] = ((PORTB->PCR[21] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_ODE_MASK | PORT_PCR_ISF_MASK)))

                      /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
                      | PORT_PCR_ODE(kPORT_OpenDrainDisable));

    /* PORTB22 (pin C12) is configured as SPI2_SOUT */
    PORT_SetPinMux(PORTB, 22U, kPORT_MuxAlt2);

    PORTB->PCR[22] = ((PORTB->PCR[22] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_ODE_MASK | PORT_PCR_ISF_MASK)))

                      /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
                      | PORT_PCR_ODE(kPORT_OpenDrainDisable));

    /* PORTB23 (pin C11) is configured as SPI2_SIN */
    PORT_SetPinMux(PORTB, 23U, kPORT_MuxAlt2);

    PORTB->PCR[23] = ((PORTB->PCR[23] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_ODE_MASK | PORT_PCR_ISF_MASK)))

                      /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
                      | PORT_PCR_ODE(kPORT_OpenDrainDisable));
}

void DSPI2_DeInitPins(void)
{
    /* Port B Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortB);

    /* PORTB20 (pin D10) is disabled */
    PORT_SetPinMux(PORTB, 20U, kPORT_PinDisabledOrAnalog);

    /* PORTB21 (pin D9) is configured as PTB21 */
    PORT_SetPinMux(PORTB, 21U, kPORT_MuxAsGpio);

    /* PORTB22 (pin C12) is configured as PTB22 */
    PORT_SetPinMux(PORTB, 22U, kPORT_MuxAsGpio);

    /* PORTB23 (pin C11) is disabled */
    PORT_SetPinMux(PORTB, 23U, kPORT_PinDisabledOrAnalog);
}

void UART0_InitPins(void)
{
  /* Port B Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortB);

    const port_pin_config_t portb16_pinE10_config = {/* Internal pull-up/down resistor is disabled */
                                                     kPORT_PullDisable,
                                                     /* Fast slew rate is configured */
                                                     kPORT_FastSlewRate,
                                                     /* Passive filter is disabled */
                                                     kPORT_PassiveFilterDisable,
                                                     /* Open drain is disabled */
                                                     kPORT_OpenDrainDisable,
                                                     /* Low drive strength is configured */
                                                     kPORT_LowDriveStrength,
                                                     /* Pin is configured as UART0_RX */
                                                     kPORT_MuxAlt3,
                                                     /* Pin Control Register fields [15:0] are not locked */
                                                     kPORT_UnlockRegister};
    /* PORTB16 (pin E10) is configured as UART0_RX */
    PORT_SetPinConfig(PORTB, 16U, &portb16_pinE10_config);

    const port_pin_config_t portb17_pinE9_config = {/* Internal pull-up/down resistor is disabled */
                                                    kPORT_PullDisable,
                                                    /* Fast slew rate is configured */
                                                    kPORT_FastSlewRate,
                                                    /* Passive filter is disabled */
                                                    kPORT_PassiveFilterDisable,
                                                    /* Open drain is disabled */
                                                    kPORT_OpenDrainDisable,
                                                    /* Low drive strength is configured */
                                                    kPORT_LowDriveStrength,
                                                    /* Pin is configured as UART0_TX */
                                                    kPORT_MuxAlt3,
                                                    /* Pin Control Register fields [15:0] are not locked */
                                                    kPORT_UnlockRegister};
    /* PORTB17 (pin E9) is configured as UART0_TX */
    PORT_SetPinConfig(PORTB, 17U, &portb17_pinE9_config);

    SIM->SOPT5 = ((SIM->SOPT5 &
                   /* Mask bits to zero which are setting */
                   (~(SIM_SOPT5_UART0TXSRC_MASK)))

                  /* UART 0 transmit data source select: UART0_TX pin. */
                  | SIM_SOPT5_UART0TXSRC(SOPT5_UART0TXSRC_UART_TX));
}

void UART0_DeinitPins(void)
{

}

void UART1_InitPins(void)
{
  /* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);

    /* PORTE0 (pin D3) is configured as UART1_TX */
    PORT_SetPinMux(PORTE, 0U, kPORT_MuxAlt3);

    PORTE->PCR[0] = ((PORTE->PCR[0] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_ODE_MASK | PORT_PCR_ISF_MASK)))

                     /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
                     | PORT_PCR_ODE(kPORT_OpenDrainDisable));

    /* PORTE1 (pin D2) is configured as UART1_RX */
    PORT_SetPinMux(PORTE, 1U, kPORT_MuxAlt3);

    PORTE->PCR[1] = ((PORTE->PCR[1] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_ODE_MASK | PORT_PCR_ISF_MASK)))

                     /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
                     | PORT_PCR_ODE(kPORT_OpenDrainDisable));

    SIM->SOPT5 = ((SIM->SOPT5 &
                   /* Mask bits to zero which are setting */
                   (~(SIM_SOPT5_UART1TXSRC_MASK)))

                  /* UART 1 transmit data source select: UART1_TX pin. */
                  | SIM_SOPT5_UART1TXSRC(SOPT5_UART1TXSRC_UART_TX));
}

void UART1_DeinitPins(void)
{
    /* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);

    /* PORTE0 (pin D3) is configured as PTE0 */
    PORT_SetPinMux(PORTE, 0U, kPORT_MuxAsGpio);

    /* PORTE1 (pin D2) is configured as PTE1 */
    PORT_SetPinMux(PORTE, 1U, kPORT_MuxAsGpio);
}

void UART2_InitPins(void)
{
    /* Port D Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortD);

    /* PORTD2 (pin C4) is configured as UART2_RX */
    PORT_SetPinMux(PORTD, 2U, kPORT_MuxAlt3);

    PORTD->PCR[2] = ((PORTD->PCR[2] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_DSE_MASK | PORT_PCR_ISF_MASK)))

                     /* Drive Strength Enable: Low drive strength is configured on the corresponding pin, if pin
                      * is configured as a digital output. */
                     | PORT_PCR_DSE(kPORT_LowDriveStrength));

    /* PORTD3 (pin B4) is configured as UART2_TX */
    PORT_SetPinMux(PORTD, 3U, kPORT_MuxAlt3);

    PORTD->PCR[3] = ((PORTD->PCR[3] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_DSE_MASK | PORT_PCR_ISF_MASK)))

                     /* Drive Strength Enable: Low drive strength is configured on the corresponding pin, if pin
                      * is configured as a digital output. */
                     | PORT_PCR_DSE(kPORT_LowDriveStrength));
}

void UART2_DeinitPins(void)
{
  /* Port D Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortD);

    /* PORTD2 (pin C4) is disabled */
    PORT_SetPinMux(PORTD, 2U, kPORT_PinDisabledOrAnalog);

    /* PORTD3 (pin B4) is disabled */
    PORT_SetPinMux(PORTD, 3U, kPORT_PinDisabledOrAnalog);
}

void UART3_InitPins(void)
{
    /* Port B Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortB);

    /* PORTB10 (pin E12) is configured as UART3_RX */
    PORT_SetPinMux(PORTB, 10U, kPORT_MuxAlt3);

    PORTB->PCR[10] = ((PORTB->PCR[10] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_ODE_MASK | PORT_PCR_ISF_MASK)))

                      /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
                      | PORT_PCR_ODE(kPORT_OpenDrainDisable));

    /* PORTB11 (pin E11) is configured as UART3_TX */
    PORT_SetPinMux(PORTB, 11U, kPORT_MuxAlt3);

    PORTB->PCR[11] = ((PORTB->PCR[11] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_ODE_MASK | PORT_PCR_ISF_MASK)))

                      /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
                      | PORT_PCR_ODE(kPORT_OpenDrainDisable));
}

void UART3_DeinitPins(void)
{
  /* Port B Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortB);

    /* PORTB10 (pin E12) is configured as ADC1_SE14 */
    PORT_SetPinMux(PORTB, 10U, kPORT_PinDisabledOrAnalog);

    /* PORTB11 (pin E11) is configured as ADC1_SE15 */
    PORT_SetPinMux(PORTB, 11U, kPORT_PinDisabledOrAnalog);
}

void UART4_InitPins(void)
{
    /* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);

    /* PORTE24 (pin M4) is configured as UART4_TX */
    PORT_SetPinMux(PORTE, 24U, kPORT_MuxAlt3);

    PORTE->PCR[24] = ((PORTE->PCR[24] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_ODE_MASK | PORT_PCR_ISF_MASK)))

                      /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
                      | PORT_PCR_ODE(kPORT_OpenDrainDisable));

    /* PORTE25 (pin K5) is configured as UART4_RX */
    PORT_SetPinMux(PORTE, 25U, kPORT_MuxAlt3);

    PORTE->PCR[25] = ((PORTE->PCR[25] &
                       /* Mask bits to zero which are setting */
                       (~(PORT_PCR_ODE_MASK | PORT_PCR_ISF_MASK)))

                      /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
                      | PORT_PCR_ODE(kPORT_OpenDrainDisable));
}

void UART4_DeinitPins(void)
{
  /* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);

    /* PORTE24 (pin M4) is configured as ADC0_SE17 */
    PORT_SetPinMux(PORTE, 24U, kPORT_PinDisabledOrAnalog);

    /* PORTE25 (pin K5) is configured as ADC0_SE18 */
    PORT_SetPinMux(PORTE, 25U, kPORT_PinDisabledOrAnalog);
}

void UART5_InitPins(void)
{
    /* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);

    /* PORTE8 (pin F3) is configured as UART5_TX */
    PORT_SetPinMux(PORTE, 8U, kPORT_MuxAlt3);

    PORTE->PCR[8] = ((PORTE->PCR[8] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_ODE_MASK | PORT_PCR_ISF_MASK)))

                     /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
                     | PORT_PCR_ODE(kPORT_OpenDrainDisable));

    /* PORTE9 (pin F2) is configured as UART5_RX */
    PORT_SetPinMux(PORTE, 9U, kPORT_MuxAlt3);

    PORTE->PCR[9] = ((PORTE->PCR[9] &
                      /* Mask bits to zero which are setting */
                      (~(PORT_PCR_ODE_MASK | PORT_PCR_ISF_MASK)))

                     /* Open Drain Enable: Open drain output is disabled on the corresponding pin. */
                     | PORT_PCR_ODE(kPORT_OpenDrainDisable));
}

void UART5_DeinitPins(void)
{
    /* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);

    /* PORTE8 (pin F3) is disabled */
    PORT_SetPinMux(PORTE, 8U, kPORT_PinDisabledOrAnalog);

    /* PORTE9 (pin F2) is disabled */
    PORT_SetPinMux(PORTE, 9U, kPORT_PinDisabledOrAnalog);
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
