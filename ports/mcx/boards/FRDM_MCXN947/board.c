/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 NXP
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

#include <stdio.h>

#include "board.h"

#include "fsl_spc.h"

#include "clock_config.h"
#include "pin_mux.h"

typedef struct mcxn947_irq_table_type {
    mcx_board_isr_t irq_gpio[5];
    void *irq_gpio_params[5];
    mcx_board_isr_t irq_uart[10];
    void *irq_uart_params[10];
    mcx_board_isr_t irq_ostimer[1];
    void *irq_ostimer_params[1];
    mcx_board_isr_t irq_usb[2];
    void *irq_usb_params[2];
} mcxn947_irq_table_t;

typedef struct mcxn947_irqn_prio_type {
    IRQn_Type irqn;
    uint8_t priority;
} mcxn947_irqn_prio_t;

static volatile mcxn947_irq_table_t s_mcxn947_irq_table;

static ADC_Type *const s_mcxn947_adc_inst_map[] = ADC_BASE_PTRS;

static LPUART_Type *const s_mcxn947_uart_inst_map[] = LPUART_BASE_PTRS;
static LPI2C_Type *const s_mcxn947_i2c_inst_map[] = LPI2C_BASE_PTRS;
static LPSPI_Type *const s_mcxn947_spi_inst_map[] = LPSPI_BASE_PTRS;

static CTIMER_Type *const s_mcxn947_ctimer_inst_map[] = CTIMER_BASE_PTRS;

static OSTIMER_Type *const s_mcxn947_ostimer_inst_map[] = OSTIMER_BASE_PTRS;

static PORT_Type *const s_mcxn947_port_inst_map[] = PORT_BASE_PTRS;
static GPIO_Type *const s_mcxn947_gpio_inst_map[] = GPIO_BASE_PTRS;

static clock_ip_name_t const s_mcxn947_port_clk_map[] = {
    kCLOCK_Port0,
    kCLOCK_Port1,
    kCLOCK_Port2,
    kCLOCK_Port3,
    kCLOCK_Port4,
    kCLOCK_None, /* Special: PORT5 is in VBAT domain */
};

static clock_ip_name_t const s_mcxn947_gpio_clk_map[] = {
    kCLOCK_Gpio0,
    kCLOCK_Gpio1,
    kCLOCK_Gpio2,
    kCLOCK_Gpio3,
    kCLOCK_Gpio4,
    kCLOCK_None, /* Special: GPIO is in VBAT domain */
};

static clock_attach_id_t const s_mcxn947_adc_clk_att_map[] = {
    kFRO12M_to_ADC0,
    kFRO12M_to_ADC1,
};

static clock_div_name_t const s_mcxn947_adc_clk_div_map[] = {
    kCLOCK_DivAdc0Clk,
    kCLOCK_DivAdc1Clk,
};

static clock_attach_id_t const s_mcxn947_flexcomm_clk_att_map[] = {
    kPLL_DIV_to_FLEXCOMM0,
    kPLL_DIV_to_FLEXCOMM1,
    kPLL_DIV_to_FLEXCOMM2,
    kPLL_DIV_to_FLEXCOMM3,
    kPLL_DIV_to_FLEXCOMM4,
    kPLL_DIV_to_FLEXCOMM5,
    kPLL_DIV_to_FLEXCOMM6,
    kPLL_DIV_to_FLEXCOMM7,
    kPLL_DIV_to_FLEXCOMM8,
    kPLL_DIV_to_FLEXCOMM9,
};

static clock_div_name_t const s_mcxn947_flexcomm_clk_div_map[] = {
    kCLOCK_DivFlexcom0Clk,
    kCLOCK_DivFlexcom1Clk,
    kCLOCK_DivFlexcom2Clk,
    kCLOCK_DivFlexcom3Clk,
    kCLOCK_DivFlexcom4Clk,
    kCLOCK_DivFlexcom5Clk,
    kCLOCK_DivFlexcom6Clk,
    kCLOCK_DivFlexcom7Clk,
    kCLOCK_DivFlexcom8Clk,
    kCLOCK_DivFlexcom9Clk,
};

static clock_attach_id_t const s_mcxn947_ctimer_clk_att_map[] = {
    kPLL0_to_CTIMER0,
    kPLL0_to_CTIMER1,
    kPLL0_to_CTIMER2,
    kPLL0_to_CTIMER3,
    kPLL0_to_CTIMER4,
};

static clock_div_name_t const s_mcxn947_ctimer_clk_div_map[] = {
    kCLOCK_DivCtimer0Clk,
    kCLOCK_DivCtimer1Clk,
    kCLOCK_DivCtimer2Clk,
    kCLOCK_DivCtimer3Clk,
    kCLOCK_DivCtimer4Clk,
};

static clock_attach_id_t const s_mcxn947_ostimer_clk_att_map[] = {
    kCLK_1M_to_OSTIMER,
};

static mcxn947_irqn_prio_t const s_mcxn947_gpio_irqn_map[] = {
    {.irqn = GPIO00_IRQn, .priority = 3}, /* TODO: Only IRQ A in use for now. */
    {.irqn = GPIO10_IRQn, .priority = 3},
    {.irqn = GPIO20_IRQn, .priority = 3},
    {.irqn = GPIO30_IRQn, .priority = 3},
    {.irqn = GPIO40_IRQn, .priority = 3},
    {.irqn = GPIO50_IRQn, .priority = 3},
};

static mcxn947_irqn_prio_t const s_mcxn947_flexcomm_irqn_map[] = {
    {.irqn = LP_FLEXCOMM0_IRQn, .priority = 3},
    {.irqn = LP_FLEXCOMM1_IRQn, .priority = 3},
    {.irqn = LP_FLEXCOMM2_IRQn, .priority = 3},
    {.irqn = LP_FLEXCOMM3_IRQn, .priority = 3},
    {.irqn = LP_FLEXCOMM4_IRQn, .priority = 3},
    {.irqn = LP_FLEXCOMM5_IRQn, .priority = 3},
    {.irqn = LP_FLEXCOMM6_IRQn, .priority = 3},
    {.irqn = LP_FLEXCOMM7_IRQn, .priority = 3},
    {.irqn = LP_FLEXCOMM8_IRQn, .priority = 3},
    {.irqn = LP_FLEXCOMM9_IRQn, .priority = 3},
};

static mcxn947_irqn_prio_t const s_mcxn947_usb_irqn_map[] = {
    {.irqn = USB0_FS_IRQn, .priority = 2},
    {.irqn = USB1_HS_IRQn, .priority = 2},
};

void MCX_BoardEarlyInit(void) {
    BOARD_InitBootClocks();
    BOARD_InitBootPins();

    /* Use PLLDIV clock (50MHz) to clock most of the peripherals. */
    CLOCK_AttachClk(kPLL0_to_PLLCLKDIV);
    CLOCK_SetClkDiv(kCLOCK_DivPllClk, 3U);
}

int MCX_BoardGetUniqueID(uint64_t *uuid) {
    /* TODO: Implement this */

    *uuid = 0xAA55BEEF0066CCFF;

    return 0;
}

void *MCX_BoardGetPORTInstance(uint8_t id) {
    if (id < ARRAY_SIZE(s_mcxn947_port_inst_map)) {
        return s_mcxn947_port_inst_map[id];
    }

    return NULL;
}

void *MCX_BoardGetGPIOInstance(uint8_t id) {
    if (id < ARRAY_SIZE(s_mcxn947_gpio_inst_map)) {
        return s_mcxn947_gpio_inst_map[id];
    }

    return NULL;
}

int MCX_BoardConfigurePORTClock(uint8_t id) {
    if (id < ARRAY_SIZE(s_mcxn947_port_inst_map)) {
        CLOCK_EnableClock(s_mcxn947_port_clk_map[id]);

        return 0;
    }

    return -1;
}

int MCX_BoardConfigureGPIOClock(uint8_t id) {
    if (id < ARRAY_SIZE(s_mcxn947_gpio_inst_map)) {
        CLOCK_EnableClock(s_mcxn947_gpio_clk_map[id]);

        return 0;
    }

    return -1;
}

int MCX_BoardConfigureGPIOISR(uint8_t id, mcx_board_isr_t isr, void *param) {
    if (id >= ARRAY_SIZE(s_mcxn947_irq_table.irq_gpio)) {
        return -1;
    }

    s_mcxn947_irq_table.irq_gpio[id] = isr;
    s_mcxn947_irq_table.irq_gpio_params[id] = param;

    if (isr == NULL) {
        DisableIRQ(s_mcxn947_gpio_irqn_map[id].irqn);
    } else {
        NVIC_SetPriority(s_mcxn947_gpio_irqn_map[id].irqn, s_mcxn947_gpio_irqn_map[id].priority);
        EnableIRQ(s_mcxn947_gpio_irqn_map[id].irqn);
    }

    return 0;
}

void MCX_BoardGenericGPIOISR(uint8_t id) {
    if (id >= ARRAY_SIZE(s_mcxn947_irq_table.irq_gpio)) {
        return;
    }

    mcx_board_isr_t isr = s_mcxn947_irq_table.irq_gpio[id];

    if (isr == NULL) {
        return;
    }

    isr(s_mcxn947_irq_table.irq_gpio_params[id]);
}

void *MCX_BoardGetADCInstance(uint8_t id) {
    if (id < ARRAY_SIZE(s_mcxn947_adc_inst_map)) {
        return s_mcxn947_adc_inst_map[id];
    }

    return NULL;
}

int MCX_BoardConfigureADCClock(uint8_t id) {
    clock_attach_id_t attach_id = kNONE_to_NONE;
    clock_div_name_t div_name;

    if (id >= ARRAY_SIZE(s_mcxn947_adc_clk_att_map)) {
        return -1;
    }

    attach_id = s_mcxn947_adc_clk_att_map[id];
    div_name = s_mcxn947_adc_clk_div_map[id];

    CLOCK_AttachClk(attach_id);
    CLOCK_SetClkDiv(div_name, 1U);

    return CLOCK_GetAdcClkFreq(id);
}

void *MCX_BoardGetUARTInstance(uint8_t id) {
    if (id < ARRAY_SIZE(s_mcxn947_uart_inst_map)) {
        return s_mcxn947_uart_inst_map[id];
    }

    /* No such UART */
    return NULL;
}

void *MCX_BoardGetI2CInstance(uint8_t id) {
    if (id < ARRAY_SIZE(s_mcxn947_i2c_inst_map)) {
        return s_mcxn947_i2c_inst_map[id];
    }

    /* No such I2C */
    return NULL;
}

void *MCX_BoardGetSPIInstance(uint8_t id) {
    if (id < ARRAY_SIZE(s_mcxn947_spi_inst_map)) {
        return s_mcxn947_spi_inst_map[id];
    }

    /* No such SPI */
    return NULL;
}

static int MCX_BoardConfigureLPFCClock(uint8_t id) {
    clock_attach_id_t attach_id = kNONE_to_NONE;
    clock_div_name_t div_name;

    if (id >= ARRAY_SIZE(s_mcxn947_flexcomm_clk_att_map)) {
        return -1;
    }

    attach_id = s_mcxn947_flexcomm_clk_att_map[id];
    div_name = s_mcxn947_flexcomm_clk_div_map[id];

    CLOCK_AttachClk(attach_id);
    CLOCK_SetClkDiv(div_name, 1U);

    return CLOCK_GetLPFlexCommClkFreq(id);
}

int MCX_BoardConfigureUARTClock(uint8_t id) {
    return MCX_BoardConfigureLPFCClock(id);
}

int MCX_BoardConfigureI2CClock(uint8_t id) {
    return MCX_BoardConfigureLPFCClock(id);
}

int MCX_BoardConfigureSPIClock(uint8_t id) {
    return MCX_BoardConfigureLPFCClock(id);
}

int MCX_BoardConfigureUARTISR(uint8_t id, mcx_board_isr_t isr, void *param) {
    if (id >= ARRAY_SIZE(s_mcxn947_irq_table.irq_uart)) {
        return -1;
    }

    s_mcxn947_irq_table.irq_uart[id] = isr;
    s_mcxn947_irq_table.irq_uart_params[id] = param;

    /* Pass NULL as ISR to disable IRQ. */
    if (isr == NULL) {
        DisableIRQ(s_mcxn947_flexcomm_irqn_map[id].irqn);
    } else {
        NVIC_SetPriority(s_mcxn947_flexcomm_irqn_map[id].irqn, s_mcxn947_flexcomm_irqn_map[id].priority);
        EnableIRQ(s_mcxn947_flexcomm_irqn_map[id].irqn);
    }

    return 0;
}

void MCX_BoardGenericUARTISR(uint8_t id) {
    if (id >= ARRAY_SIZE(s_mcxn947_irq_table.irq_uart)) {
        return;
    }

    mcx_board_isr_t isr = s_mcxn947_irq_table.irq_uart[id];

    if (isr == NULL) {
        return;
    }

    isr(s_mcxn947_irq_table.irq_uart_params[id]);
}

void *MCX_BoardGetCT32Instance(uint8_t id) {
    if (id < ARRAY_SIZE(s_mcxn947_ctimer_inst_map)) {
        return s_mcxn947_ctimer_inst_map[id];
    }

    /* No such CTIMER */
    return NULL;
}

int MCX_BoardConfigureCT32Clock(uint8_t id) {
    clock_attach_id_t attach_id = kNONE_to_NONE;
    clock_div_name_t div_name;

    if (id >= ARRAY_SIZE(s_mcxn947_ctimer_clk_att_map)) {
        return -1;
    }

    attach_id = s_mcxn947_ctimer_clk_att_map[id];
    div_name = s_mcxn947_ctimer_clk_div_map[id];

    CLOCK_AttachClk(attach_id);
    CLOCK_SetClkDiv(div_name, 1U);

    return CLOCK_GetCTimerClkFreq(id);
}

void *MCX_BoardGetOSTimerInstance(uint8_t id) {
    if (id < ARRAY_SIZE(s_mcxn947_ostimer_inst_map)) {
        return s_mcxn947_ostimer_inst_map[id];
    }

    /* No such OSTimer */
    return NULL;
}

int MCX_BoardConfigureUSBClock(uint8_t id) {
    if (id >= ARRAY_SIZE(s_mcxn947_usb_irqn_map)) {
        return -1;
    }

    switch (id) {
        case 0: {
            /* TODO: Configure FS USB here. */
            break;
        }

        case 1: {
            CLOCK_SetupExtClocking(24000000);

            CLOCK_SetupClockCtrl(kCLOCK_CLKIN_ENA_FM_USBH_LPT);

            CLOCK_EnableClock(kCLOCK_UsbHs);
            CLOCK_EnableClock(kCLOCK_UsbHsPhy);

            CLOCK_EnableUsbhsPhyPllClock(kCLOCK_Usbphy480M, 24000000);
            CLOCK_EnableUsbhsClock();

            break;
        }

        default:
            break;
    }

    return 0;
}

int MCX_BoardConfigureUSBPHY(uint8_t id) {
    if (id >= ARRAY_SIZE(s_mcxn947_gpio_irqn_map)) {
        return -1;
    }

    switch (id) {
        case 0: {
            /* TODO: Implement this */
            break;
        }

        case 1: {
            USBPHY->CTRL |= USBPHY_CTRL_SET_ENUTMILEVEL2_MASK | USBPHY_CTRL_SET_ENUTMILEVEL3_MASK;
            USBPHY->PWD = 0;

            break;
        }

        default:
            break;
    }

    return 0;
}

int MCX_BoardConfigureUSBISR(uint8_t id, mcx_board_isr_t isr, void *param) {
    if (id >= ARRAY_SIZE(s_mcxn947_irq_table.irq_usb)) {
        return -1;
    }

    s_mcxn947_irq_table.irq_usb[id] = isr;
    s_mcxn947_irq_table.irq_usb_params[id] = param;

    /* Pass NULL as ISR to disable IRQ. */
    if (isr == NULL) {
        DisableIRQ(s_mcxn947_usb_irqn_map[id].irqn);
    } else {
        NVIC_SetPriority(s_mcxn947_usb_irqn_map[id].irqn, s_mcxn947_usb_irqn_map[id].priority);
        EnableIRQ(s_mcxn947_usb_irqn_map[id].irqn);
    }

    return 0;
}

void MCX_BoardGenericUSBISR(uint8_t id) {
    if (id >= ARRAY_SIZE(s_mcxn947_irq_table.irq_usb)) {
        return;
    }

    mcx_board_isr_t isr = s_mcxn947_irq_table.irq_usb[id];

    if (isr == NULL) {
        return;
    }

    isr(s_mcxn947_irq_table.irq_usb_params[id]);
}

int MCX_BoardConfigureOSTimerClock(uint8_t id) {
    clock_attach_id_t attach_id = kNONE_to_NONE;

    if (id >= ARRAY_SIZE(s_mcxn947_ostimer_clk_att_map)) {
        return -1;
    }

    attach_id = s_mcxn947_ostimer_clk_att_map[id];

    CLOCK_AttachClk(attach_id);

    return CLOCK_GetOstimerClkFreq();
}

int MCX_BoardConfigureOSTimerISR(uint8_t id, mcx_board_isr_t isr, void *param) {
    if (id >= ARRAY_SIZE(s_mcxn947_irq_table.irq_ostimer)) {
        return -1;
    }

    s_mcxn947_irq_table.irq_ostimer[id] = isr;
    s_mcxn947_irq_table.irq_ostimer_params[id] = param;

    return 0;
}

void MCX_BoardGenericOSTimerISR(uint8_t id) {
    if (id >= ARRAY_SIZE(s_mcxn947_irq_table.irq_ostimer)) {
        return;
    }

    mcx_board_isr_t isr = s_mcxn947_irq_table.irq_ostimer[id];

    if (isr == NULL) {
        return;
    }

    isr(s_mcxn947_irq_table.irq_ostimer_params[id]);
}
