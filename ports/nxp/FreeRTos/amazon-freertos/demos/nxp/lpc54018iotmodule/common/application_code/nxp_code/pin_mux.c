/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v3.0
processor: LPC54018
package_id: LPC54018JET180
mcu_data: ksdk2_0
processor_version: 0.0.4
pin_labels:
- {pin_num: A13, pin_signal: PIO3_3/LCD_VD(17)/FC9_TXD_SCL_MISO, label: PWRON, identifier: PWRON}
- {pin_num: E3, pin_signal: PIO3_14/SCT0_OUT4/FC9_RTS_SCL_SSEL1/CTIMER3_MAT1/TRACEDATA(2), label: KFFET, identifier: KFFET}
- {pin_num: H4, pin_signal: PIO3_13/SCT0_OUT9/FC9_CTS_SDA_SSEL0/CTIMER3_CAP1/EMC_FBCK/TRACEDATA(1), label: LED, identifier: LED}
- {pin_num: B10, pin_signal: PIO3_5/LCD_VD(19)/FC8_RTS_SCL_SSEL1/CTIMER4_MAT1, label: PWRON, identifier: LCD_VD19}
- {pin_num: N3, pin_signal: PIO1_0/FC0_RTS_SCL_SSEL1/SD_D(3)/CTIMER0_CAP2/SCT0_GPI4/TRACECLK/ADC0_6, label: IRQ, identifier: IRQ}
- {pin_num: P11, pin_signal: PIO1_22/FC8_RTS_SCL_SSEL1/SD_CMD/CTIMER2_MAT3/SCT0_GPI5/FC4_SSEL3/EMC_CKE(1), label: IRQ, identifier: IRQ}
- {pin_num: C10, pin_signal: PIO3_2/LCD_VD(16)/FC9_RXD_SDA_MOSI/CTIMER1_MAT2, label: IRQ, identifier: IRQ}
- {pin_num: A3, pin_signal: PIO3_10/SCT0_OUT3/CTIMER3_MAT0/EMC_DYCSN(1)/TRACEDATA(0), label: PWRON, identifier: PWRON}
- {pin_num: A14, pin_signal: PIO4_7/CTIMER4_CAP3/USB0_PORTPWRN/USB0_FRAME/SCT0_GPI0, label: PWRON, identifier: PWRON}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

#include "fsl_common.h"
#include "fsl_iocon.h"
#include "pin_mux.h"

/*FUNCTION**********************************************************************
 * 
 * Function Name : BOARD_InitBootPins
 * Description   : Calls initialization functions.
 * 
 *END**************************************************************************/
void BOARD_InitBootPins(void) {
    BOARD_InitPins();
}

#define IOCON_PIO_DIGITAL_EN        0x0100u   /*!< Enables digital function */
#define IOCON_PIO_FUNC1               0x01u   /*!< Selects pin function 1 */
#define IOCON_PIO_INPFILT_OFF       0x0200u   /*!< Input filter disabled */
#define IOCON_PIO_INV_DI              0x00u   /*!< Input function is not inverted */
#define IOCON_PIO_MODE_INACT          0x00u   /*!< No addition pin function */
#define IOCON_PIO_OPENDRAIN_DI        0x00u   /*!< Open drain is disabled */
#define IOCON_PIO_SLEW_STANDARD       0x00u   /*!< Standard mode, output slew rate control is enabled */
#define PIN29_IDX                       29u   /*!< Pin number for pin 29 in a port 0 */
#define PIN30_IDX                       30u   /*!< Pin number for pin 30 in a port 0 */
#define PIO022_DIGIMODE_DIGITAL       0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO022_FUNC_ALT7              0x07u   /*!< Selects pin function.: Alternative connection 7. */
#define PIO022_INVERT_DISABLED        0x00u   /*!< Input polarity.: Disabled. Input function is not inverted. */
#define PIO022_MODE_INACTIVE          0x00u   /*!< Selects function mode (on-chip pull-up/pull-down resistor control).: Inactive. Inactive (no pull-down/pull-up resistor enabled). */
#define PIO022_OD_NORMAL              0x00u   /*!< Controls open-drain mode.: Normal. Normal push-pull output */
#define PIO022_SLEW_STANDARD          0x00u   /*!< Driver slew rate.: Standard mode, output slew rate control is enabled. More outputs can be switched simultaneously. */
#define PORT0_IDX                        0u   /*!< Port index */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitPins:
- options: {callFromInitBoot: 'true', coreID: core0, enableClock: 'true'}
- pin_list:
  - {pin_num: B12, peripheral: USBFSH, signal: USB_VBUS, pin_signal: PIO0_22/FC6_TXD_SCL_MISO_WS/UTICK_CAP1/CTIMER3_CAP3/SCT0_OUT3/USB0_VBUS, mode: inactive, invert: disabled,
    slew_rate: standard, open_drain: disabled}
  - {pin_num: B13, peripheral: FLEXCOMM0, signal: RXD_SDA_MOSI, pin_signal: PIO0_29/FC0_RXD_SDA_MOSI/CTIMER2_MAT3/SCT0_OUT8/TRACEDATA(2), mode: inactive, invert: disabled,
    glitch_filter: disabled, slew_rate: standard, open_drain: disabled}
  - {pin_num: A2, peripheral: FLEXCOMM0, signal: TXD_SCL_MISO, pin_signal: PIO0_30/FC0_TXD_SCL_MISO/CTIMER0_MAT0/SCT0_OUT9/TRACEDATA(1), mode: inactive, invert: disabled,
    glitch_filter: disabled, slew_rate: standard, open_drain: disabled}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitPins
 *
 *END**************************************************************************/
void BOARD_InitPins(void) { /* Function assigned for the Core #0 (ARM Cortex-M4) */
  CLOCK_EnableClock(kCLOCK_Iocon);                           /* Enables the clock for the IOCON block. 0 = Disable; 1 = Enable.: 0x01u */

  IOCON->PIO[0][22] = ((IOCON->PIO[0][22] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_MODE_MASK | IOCON_PIO_INVERT_MASK | IOCON_PIO_DIGIMODE_MASK | IOCON_PIO_SLEW_MASK | IOCON_PIO_OD_MASK))) /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO022_FUNC_ALT7)                     /* Selects pin function.: PORT022 (pin B12) is configured as USB0_VBUS */
      | IOCON_PIO_MODE(PIO022_MODE_INACTIVE)                 /* Selects function mode (on-chip pull-up/pull-down resistor control).: Inactive. Inactive (no pull-down/pull-up resistor enabled). */
      | IOCON_PIO_INVERT(PIO022_INVERT_DISABLED)             /* Input polarity.: Disabled. Input function is not inverted. */
      | IOCON_PIO_DIGIMODE(PIO022_DIGIMODE_DIGITAL)          /* Select Analog/Digital mode.: Digital mode. */
      | IOCON_PIO_SLEW(PIO022_SLEW_STANDARD)                 /* Driver slew rate.: Standard mode, output slew rate control is enabled. More outputs can be switched simultaneously. */
      | IOCON_PIO_OD(PIO022_OD_NORMAL)                       /* Controls open-drain mode.: Normal. Normal push-pull output */
    );
  const uint32_t port0_pin29_config = (
    IOCON_PIO_FUNC1 |                                        /* Pin is configured as FC0_RXD_SDA_MOSI */
    IOCON_PIO_MODE_INACT |                                   /* No addition pin function */
    IOCON_PIO_INV_DI |                                       /* Input function is not inverted */
    IOCON_PIO_DIGITAL_EN |                                   /* Enables digital function */
    IOCON_PIO_INPFILT_OFF |                                  /* Input filter disabled */
    IOCON_PIO_SLEW_STANDARD |                                /* Standard mode, output slew rate control is enabled */
    IOCON_PIO_OPENDRAIN_DI                                   /* Open drain is disabled */
  );
  IOCON_PinMuxSet(IOCON, PORT0_IDX, PIN29_IDX, port0_pin29_config); /* PORT0 PIN29 (coords: B13) is configured as FC0_RXD_SDA_MOSI */
  const uint32_t port0_pin30_config = (
    IOCON_PIO_FUNC1 |                                        /* Pin is configured as FC0_TXD_SCL_MISO */
    IOCON_PIO_MODE_INACT |                                   /* No addition pin function */
    IOCON_PIO_INV_DI |                                       /* Input function is not inverted */
    IOCON_PIO_DIGITAL_EN |                                   /* Enables digital function */
    IOCON_PIO_INPFILT_OFF |                                  /* Input filter disabled */
    IOCON_PIO_SLEW_STANDARD |                                /* Standard mode, output slew rate control is enabled */
    IOCON_PIO_OPENDRAIN_DI                                   /* Open drain is disabled */
  );
  IOCON_PinMuxSet(IOCON, PORT0_IDX, PIN30_IDX, port0_pin30_config); /* PORT0 PIN30 (coords: A2) is configured as FC0_TXD_SCL_MISO */
}


#define PIO10_DIGIMODE_DIGITAL        0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO10_FUNC_ALT0               0x00u   /*!< Selects pin function.: Alternative connection 0. */
#define PIO10_INVERT_DISABLED         0x00u   /*!< Input polarity.: Disabled. Input function is not inverted. */
#define PIO10_MODE_PULL_UP            0x02u   /*!< Selects function mode (on-chip pull-up/pull-down resistor control).: Pull-up. Pull-up resistor enabled. */
#define PIO122_DIGIMODE_DIGITAL       0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO122_FUNC_ALT1              0x01u   /*!< Selects pin function.: Alternative connection 1. */
#define PIO315_DIGIMODE_DIGITAL       0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO315_FUNC_ALT1              0x01u   /*!< Selects pin function.: Alternative connection 1. */
#define PIO316_DIGIMODE_DIGITAL       0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO316_FUNC_ALT1              0x01u   /*!< Selects pin function.: Alternative connection 1. */
#define PIO317_DIGIMODE_DIGITAL       0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO317_FUNC_ALT1              0x01u   /*!< Selects pin function.: Alternative connection 1. */
#define PIO33_DIGIMODE_DIGITAL        0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO33_FUNC_ALT0               0x00u   /*!< Selects pin function.: Alternative connection 0. */
#define PIO33_INVERT_DISABLED         0x00u   /*!< Input polarity.: Disabled. Input function is not inverted. */
#define PIO33_MODE_PULL_DOWN          0x01u   /*!< Selects function mode (on-chip pull-up/pull-down resistor control).: Pull-down. Pull-down resistor enabled. */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitGT202Shield:
- options: {coreID: core0, enableClock: 'true'}
- pin_list:
  - {pin_num: D2, peripheral: FLEXCOMM8, signal: SCK, pin_signal: PIO3_15/FC8_SCK/SD_WR_PRT}
  - {pin_num: K1, peripheral: FLEXCOMM8, signal: TXD_SCL_MISO, pin_signal: PIO3_17/FC8_TXD_SCL_MISO/SD_D(5)}
  - {pin_num: E1, peripheral: FLEXCOMM8, signal: RXD_SDA_MOSI, pin_signal: PIO3_16/FC8_RXD_SDA_MOSI/SD_D(4)}
  - {pin_num: P11, peripheral: FLEXCOMM8, signal: RTS_SCL_SSEL1, pin_signal: PIO1_22/FC8_RTS_SCL_SSEL1/SD_CMD/CTIMER2_MAT3/SCT0_GPI5/FC4_SSEL3/EMC_CKE(1), identifier: ''}
  - {pin_num: A13, peripheral: GPIO, signal: 'PIO3, 3', pin_signal: PIO3_3/LCD_VD(17)/FC9_TXD_SCL_MISO, direction: OUTPUT, mode: pullDown, invert: disabled}
  - {pin_num: N3, peripheral: GPIO, signal: 'PIO1, 0', pin_signal: PIO1_0/FC0_RTS_SCL_SSEL1/SD_D(3)/CTIMER0_CAP2/SCT0_GPI4/TRACECLK/ADC0_6, direction: INPUT, mode: pullUp,
    invert: disabled}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitGT202Shield
 *
 *END**************************************************************************/
void BOARD_InitGT202Shield(void) { /* Function assigned for the Core #0 (ARM Cortex-M4) */
  CLOCK_EnableClock(kCLOCK_Iocon);                           /* Enables the clock for the IOCON block. 0 = Disable; 1 = Enable.: 0x01u */

  IOCON->PIO[1][0] = ((IOCON->PIO[1][0] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_MODE_MASK | IOCON_PIO_INVERT_MASK | IOCON_PIO_DIGIMODE_MASK))) /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO10_FUNC_ALT0)                      /* Selects pin function.: PORT10 (pin N3) is configured as PIO1_0 */
      | IOCON_PIO_MODE(PIO10_MODE_PULL_UP)                   /* Selects function mode (on-chip pull-up/pull-down resistor control).: Pull-up. Pull-up resistor enabled. */
      | IOCON_PIO_INVERT(PIO10_INVERT_DISABLED)              /* Input polarity.: Disabled. Input function is not inverted. */
      | IOCON_PIO_DIGIMODE(PIO10_DIGIMODE_DIGITAL)           /* Select Analog/Digital mode.: Digital mode. */
    );
  IOCON->PIO[1][22] = ((IOCON->PIO[1][22] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))      /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO122_FUNC_ALT1)                     /* Selects pin function.: PORT122 (pin P11) is configured as FC8_RTS_SCL_SSEL1 */
      | IOCON_PIO_DIGIMODE(PIO122_DIGIMODE_DIGITAL)          /* Select Analog/Digital mode.: Digital mode. */
    );
  IOCON->PIO[3][15] = ((IOCON->PIO[3][15] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))      /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO315_FUNC_ALT1)                     /* Selects pin function.: PORT315 (pin D2) is configured as FC8_SCK */
      | IOCON_PIO_DIGIMODE(PIO315_DIGIMODE_DIGITAL)          /* Select Analog/Digital mode.: Digital mode. */
    );
  IOCON->PIO[3][16] = ((IOCON->PIO[3][16] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))      /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO316_FUNC_ALT1)                     /* Selects pin function.: PORT316 (pin E1) is configured as FC8_RXD_SDA_MOSI */
      | IOCON_PIO_DIGIMODE(PIO316_DIGIMODE_DIGITAL)          /* Select Analog/Digital mode.: Digital mode. */
    );
  IOCON->PIO[3][17] = ((IOCON->PIO[3][17] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))      /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO317_FUNC_ALT1)                     /* Selects pin function.: PORT317 (pin K1) is configured as FC8_TXD_SCL_MISO */
      | IOCON_PIO_DIGIMODE(PIO317_DIGIMODE_DIGITAL)          /* Select Analog/Digital mode.: Digital mode. */
    );
  IOCON->PIO[3][3] = ((IOCON->PIO[3][3] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_MODE_MASK | IOCON_PIO_INVERT_MASK | IOCON_PIO_DIGIMODE_MASK))) /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO33_FUNC_ALT0)                      /* Selects pin function.: PORT33 (pin A13) is configured as PIO3_3 */
      | IOCON_PIO_MODE(PIO33_MODE_PULL_DOWN)                 /* Selects function mode (on-chip pull-up/pull-down resistor control).: Pull-down. Pull-down resistor enabled. */
      | IOCON_PIO_INVERT(PIO33_INVERT_DISABLED)              /* Input polarity.: Disabled. Input function is not inverted. */
      | IOCON_PIO_DIGIMODE(PIO33_DIGIMODE_DIGITAL)           /* Select Analog/Digital mode.: Digital mode. */
    );
}

#define IOCON_PIO_DIGITAL_EN        0x0100u   /*!< Enables digital function */
#define IOCON_PIO_FUNC1               0x01u   /*!< Selects pin function 1 */
#define IOCON_PIO_FUNC6               0x06u   /*!< Selects pin function 6 */
#define IOCON_PIO_INPFILT_OFF       0x0200u   /*!< Input filter disabled */
#define IOCON_PIO_INV_DI              0x00u   /*!< Input function is not inverted */
#define IOCON_PIO_MODE_INACT          0x00u   /*!< No addition pin function */
#define IOCON_PIO_MODE_PULLUP         0x20u   /*!< Selects pull-up function */
#define IOCON_PIO_OPENDRAIN_DI        0x00u   /*!< Open drain is disabled */
#define IOCON_PIO_SLEW_STANDARD       0x00u   /*!< Standard mode, output slew rate control is enabled */
#define PIN23_IDX                       23u   /*!< Pin number for pin 23 in a port 0 */
#define PIN24_IDX                       24u   /*!< Pin number for pin 24 in a port 0 */
#define PIN25_IDX                       25u   /*!< Pin number for pin 25 in a port 0 */
#define PIN26_IDX                       26u   /*!< Pin number for pin 26 in a port 0 */
#define PIN27_IDX                       27u   /*!< Pin number for pin 27 in a port 0 */
#define PIN28_IDX                       28u   /*!< Pin number for pin 28 in a port 0 */
#define PIN29_IDX                       29u   /*!< Pin number for pin 29 in a port 0 */
#define PIN30_IDX                       30u   /*!< Pin number for pin 30 in a port 0 */
#define PORT0_IDX                        0u   /*!< Port index */

#define IOCON_PIO_DIGITAL_EN        0x0100u   /*!< Enables digital function */
#define IOCON_PIO_FUNC1               0x01u   /*!< Selects pin function 1 */
#define IOCON_PIO_INPFILT_OFF       0x0200u   /*!< Input filter disabled */
#define IOCON_PIO_INV_DI              0x00u   /*!< Input function is not inverted */
#define IOCON_PIO_MODE_INACT          0x00u   /*!< No addition pin function */
#define IOCON_PIO_OPENDRAIN_DI        0x00u   /*!< Open drain is disabled */
#define IOCON_PIO_SLEW_STANDARD       0x00u   /*!< Standard mode, output slew rate control is enabled */
#define PIN29_IDX                       29u   /*!< Pin number for pin 29 in a port 0 */
#define PIN30_IDX                       30u   /*!< Pin number for pin 30 in a port 0 */
#define PIO022_DIGIMODE_DIGITAL       0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO022_FUNC_ALT7              0x07u   /*!< Selects pin function.: Alternative connection 7. */
#define PIO022_INVERT_DISABLED        0x00u   /*!< Input polarity.: Disabled. Input function is not inverted. */
#define PIO022_MODE_INACTIVE          0x00u   /*!< Selects function mode (on-chip pull-up/pull-down resistor control).: Inactive. Inactive (no pull-down/pull-up resistor enabled). */
#define PIO022_OD_NORMAL              0x00u   /*!< Controls open-drain mode.: Normal. Normal push-pull output */
#define PIO022_SLEW_STANDARD          0x00u   /*!< Driver slew rate.: Standard mode, output slew rate control is enabled. More outputs can be switched simultaneously. */
#define PORT0_IDX                        0u   /*!< Port index */


void BOARD_InitSPIFI(void) { /* Function assigned for the Core #0 (ARM Cortex-M4) */
  CLOCK_EnableClock(kCLOCK_Iocon);                           /* Enables the clock for the IOCON block. 0 = Disable; 1 = Enable.: 0x01u */

  //OK
  const uint32_t port0_pin23_config = (
    IOCON_PIO_FUNC6 |                                        /* Pin is configured as SPIFI_CSN */
    IOCON_PIO_MODE_PULLUP |                                  /* Selects pull-up function */
    IOCON_PIO_INV_DI |                                       /* Input function is not inverted */
    IOCON_PIO_DIGITAL_EN |                                   /* Enables digital function */
    IOCON_PIO_INPFILT_OFF |                                  /* Input filter disabled */
    IOCON_PIO_OPENDRAIN_DI                                   /* Open drain is disabled */
  );
  IOCON_PinMuxSet(IOCON, PORT0_IDX, PIN23_IDX, port0_pin23_config); /* PORT0 PIN23 (coords: N7) is configured as SPIFI_CSN */

  //OK
  const uint32_t port0_pin24_config = (
    IOCON_PIO_FUNC6 |                                        /* Pin is configured as SPIFI_IO(0) */
    IOCON_PIO_MODE_PULLUP |                                  /* Selects pull-up function */
    IOCON_PIO_INV_DI |                                       /* Input function is not inverted */
    IOCON_PIO_DIGITAL_EN |                                   /* Enables digital function */
    IOCON_PIO_INPFILT_OFF |                                  /* Input filter disabled */
    (1 << 10) | //fast mode
    IOCON_PIO_OPENDRAIN_DI                                   /* Open drain is disabled */
  );
  IOCON_PinMuxSet(IOCON, PORT0_IDX, PIN24_IDX, port0_pin24_config); /* PORT0 PIN24 (coords: M7) is configured as SPIFI_IO(0) */

  //OK
  const uint32_t port0_pin25_config = (
    IOCON_PIO_FUNC6 |                                        /* Pin is configured as SPIFI_IO(1) */
    IOCON_PIO_MODE_PULLUP |                                  /* Selects pull-up function */
    IOCON_PIO_INV_DI |                                       /* Input function is not inverted */
    IOCON_PIO_DIGITAL_EN |                                   /* Enables digital function */
    IOCON_PIO_INPFILT_OFF |                                  /* Input filter disabled */
    (1 << 10) | //fast mode
    IOCON_PIO_OPENDRAIN_DI                                   /* Open drain is disabled */
  );
  IOCON_PinMuxSet(IOCON, PORT0_IDX, PIN25_IDX, port0_pin25_config); /* PORT0 PIN25 (coords: K8) is configured as SPIFI_IO(1) */

  //OK
  const uint32_t port0_pin26_config = (
    IOCON_PIO_FUNC6 |                                        /* Pin is configured as SPIFI_CLK */
    IOCON_PIO_MODE_PULLUP |                                  /* Selects pull-up function */
    IOCON_PIO_INV_DI |                                       /* Input function is not inverted */
    IOCON_PIO_DIGITAL_EN |                                   /* Enables digital function */
    IOCON_PIO_INPFILT_OFF |                                  /* Input filter disabled */
    (1 << 10) | //fast mode
    IOCON_PIO_OPENDRAIN_DI                                   /* Open drain is disabled */
  );
  IOCON_PinMuxSet(IOCON, PORT0_IDX, PIN26_IDX, port0_pin26_config); /* PORT0 PIN26 (coords: M13) is configured as SPIFI_CLK */

  //OK
  const uint32_t port0_pin27_config = (
    IOCON_PIO_FUNC6 |                                        /* Pin is configured as SPIFI_IO(3) */
    IOCON_PIO_MODE_PULLUP |                                  /* Selects pull-up function */
    IOCON_PIO_INV_DI |                                       /* Input function is not inverted */
    IOCON_PIO_DIGITAL_EN |                                   /* Enables digital function */
    IOCON_PIO_INPFILT_OFF |                                  /* Input filter disabled */
    (1 << 10) | //fast mode
    IOCON_PIO_OPENDRAIN_DI                                   /* Open drain is disabled */
  );
  IOCON_PinMuxSet(IOCON, PORT0_IDX, PIN27_IDX, port0_pin27_config); /* PORT0 PIN27 (coords: L9) is configured as SPIFI_IO(3) */

  //OK
  const uint32_t port0_pin28_config = (
    IOCON_PIO_FUNC6 |                                        /* Pin is configured as SPIFI_IO(2) */
    IOCON_PIO_MODE_PULLUP |                                  /* Selects pull-up function */
    IOCON_PIO_INV_DI |                                       /* Input function is not inverted */
    IOCON_PIO_DIGITAL_EN |                                   /* Enables digital function */
    IOCON_PIO_INPFILT_OFF |                                  /* Input filter disabled */
    (1 << 10) | //fast mode
    IOCON_PIO_OPENDRAIN_DI                                   /* Open drain is disabled */
  );
  IOCON_PinMuxSet(IOCON, PORT0_IDX, PIN28_IDX, port0_pin28_config); /* PORT0 PIN28 (coords: M9) is configured as SPIFI_IO(2) */
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
