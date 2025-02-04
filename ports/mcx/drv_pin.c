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

#include "board.h"

/* SDK drivers */
#include "fsl_gpio.h"
#include "fsl_port.h"

#include "drv_pin.h"

int drv_pin_init(drv_pin_t *pin) {
    if (MCX_BoardConfigurePORTClock(pin->port) != 0) {
        return -ENOENT;
    }

    if (MCX_BoardConfigureGPIOClock(pin->port) != 0) {
        return -ENOENT;
    }

    return 0;
}

/**
 * Initialize pin to specific AF or analog function
*/
int drv_pin_config_set(drv_pin_t *pin, drv_pin_config_t *cfg) {
    int ret = 0;

    ret = drv_pin_mode_set(pin, cfg->mode, cfg->af);
    if (ret < 0) {
        return ret;
    }

    ret = drv_pin_pull_set(pin, cfg->pull);
    if (ret < 0) {
        return ret;
    }

    ret = drv_pin_drive_set(pin, cfg->drive);
    if (ret < 0) {
        return ret;
    }

    return ret;
}

int drv_pin_config_get(drv_pin_t *pin, drv_pin_config_t *cfg) {
    int ret = 0;

    ret = drv_pin_mode_get(pin, &cfg->mode, &cfg->af);
    if (ret < 0) {
        return ret;
    }

    ret = drv_pin_pull_get(pin, &cfg->pull);
    if (ret < 0) {
        return ret;
    }

    ret = drv_pin_drive_get(pin, &cfg->drive);
    if (ret < 0) {
        return ret;
    }

    return ret;
}

int drv_pin_drive_set(drv_pin_t *pin, drv_pin_drive_t drive) {
    PORT_Type *port = MCX_BoardGetPORTInstance(pin->port);
    uint8_t pin_num = pin->pin;

    /* Sanity checks */
    if (port == NULL) {
        return -ENOENT;
    }
    if (pin_num >= 32) {
        return -EINVAL;
    }

    uint32_t pcr = port->PCR[pin_num];

    pcr &= ~(PORT_PCR_DSE_MASK);

    switch (drive) {
        case DRV_PIN_DRIVE_HIGH:
            pcr |= PORT_PCR_DSE(1U);
            break;

        case DRV_PIN_DRIVE_LOW:
        default:
            break;
    }

    port->PCR[pin_num] = pcr;

    return 0;
}

int drv_pin_drive_get(drv_pin_t *pin, drv_pin_drive_t *drive) {
    PORT_Type *port = MCX_BoardGetPORTInstance(pin->port);
    uint8_t pin_num = pin->pin;

    /* Sanity checks */
    if (port == NULL) {
        return -ENOENT;
    }
    if (pin_num >= 32) {
        return -EINVAL;
    }

    uint32_t pcr = port->PCR[pin_num];

    /* Drive */
    if (pcr & PORT_PCR_DSE_MASK) {
        *drive = DRV_PIN_DRIVE_HIGH;
    } else {
        *drive = DRV_PIN_DRIVE_LOW;
    }

    return 0;
}

int drv_pin_pull_set(drv_pin_t *pin, drv_pin_pull_type_t pull) {
    PORT_Type *port = MCX_BoardGetPORTInstance(pin->port);
    uint8_t pin_num = pin->pin;

    /* Sanity checks */
    if (port == NULL) {
        return -ENOENT;
    }
    if (pin_num >= 32) {
        return -EINVAL;
    }

    uint32_t pcr = port->PCR[pin_num];

    pcr &= ~(PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);

    switch (pull) {
        case DRV_PIN_PULL_UP:
            pcr |= (PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
            break;

        case DRV_PIN_PULL_DOWN:
            pcr |= PORT_PCR_PE_MASK;
            break;

        default:
        case DRV_PIN_PULL_NONE:
            break;
    }

    port->PCR[pin_num] = pcr;

    return 0;
}

int drv_pin_pull_get(drv_pin_t *pin, drv_pin_pull_type_t *pull) {
    PORT_Type *port = MCX_BoardGetPORTInstance(pin->port);
    uint8_t pin_num = pin->pin;

    /* Sanity checks */
    if (port == NULL) {
        return -ENOENT;
    }
    if (pin_num >= 32) {
        return -EINVAL;
    }

    uint32_t pcr = port->PCR[pin_num];

    /* Pull enabled */
    if (pcr & PORT_PCR_PE_MASK) {
        if (pcr & PORT_PCR_PS_MASK) {
            *pull = DRV_PIN_PULL_UP;
        } else {
            *pull = DRV_PIN_PULL_DOWN;
        }
    } else {
        *pull = DRV_PIN_PULL_NONE;
    }

    return 0;
}

int drv_pin_mode_set(drv_pin_t *pin, drv_pin_mode_t mode, uint8_t af) {
    PORT_Type *port = MCX_BoardGetPORTInstance(pin->port);
    GPIO_Type *gpio = MCX_BoardGetGPIOInstance(pin->port);
    uint8_t pin_num = pin->pin;

    /* Sanity checks */
    if (port == NULL) {
        return -ENOENT;
    }
    if (gpio == NULL) {
        return -ENOENT;
    }
    if (pin_num >= 32) {
        return -EINVAL;
    }

    uint32_t pcr = port->PCR[pin_num];

    pcr &= ~(PORT_PCR_MUX_MASK | PORT_PCR_ODE_MASK);
    pcr |= PORT_PCR_IBE(1U);

    gpio_pin_config_t gpio_cfg = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic = 0U,
    };

    uint8_t alt_id = af;

    switch (mode) {
        case DRV_PIN_MODE_INPUT:
            alt_id = kPORT_MuxAlt0;
            break;

        case DRV_PIN_MODE_AF_PP:
            break;

        case DRV_PIN_MODE_OUTPUT_PP:
            gpio_cfg.pinDirection = kGPIO_DigitalOutput;
            GPIO_PinInit(gpio, pin_num, &gpio_cfg);

            alt_id = kPORT_MuxAlt0;
            break;

        case DRV_PIN_MODE_OUTPUT_OD:
            gpio_cfg.pinDirection = kGPIO_DigitalOutput;
            GPIO_PinInit(gpio, pin_num, &gpio_cfg);

            pcr |= PORT_PCR_ODE(1U);
            alt_id = kPORT_MuxAlt0;
            break;

        case DRV_PIN_MODE_AF_OD:
            pcr |= PORT_PCR_ODE(1U);
            break;

        case DRV_PIN_MODE_ANALOG:
        default:
            /* Clear IBE for analog functions */
            pcr &= ~(PORT_PCR_IBE_MASK);
            break;
    }

    /* Apply corresponding AF */
    pcr |= PORT_PCR_MUX(alt_id);

    port->PCR[pin_num] = pcr;

    return 0;
}

int drv_pin_mode_get(drv_pin_t *pin, drv_pin_mode_t *mode, uint8_t *af) {
    PORT_Type *port = MCX_BoardGetPORTInstance(pin->port);
    GPIO_Type *gpio = MCX_BoardGetGPIOInstance(pin->port);
    uint8_t pin_num = pin->pin;

    /* Sanity checks */
    if (port == NULL) {
        return -ENOENT;
    }
    if (gpio == NULL) {
        return -ENOENT;
    }
    if (pin_num >= 32) {
        return -EINVAL;
    }

    uint32_t pcr = port->PCR[pin_num];

    uint8_t mux = (pcr & PORT_PCR_MUX_MASK) >> PORT_PCR_MUX_SHIFT;
    bool od = (pcr & PORT_PCR_ODE_MASK) >> PORT_PCR_ODE_SHIFT;

    if (mux == 0) { /* GPIO or Analog mode */
        if ((pcr & PORT_PCR_IBE_MASK) == 0U) {
            *mode = DRV_PIN_MODE_ANALOG;
        } else {
            bool gpio_mode = (gpio->PDDR & (1U << pin_num)) ? true : false;
            if (gpio_mode) {
                if (od) {
                    *mode = DRV_PIN_MODE_OUTPUT_OD;
                } else {
                    *mode = DRV_PIN_MODE_OUTPUT_PP;
                }
            } else {
                *mode = DRV_PIN_MODE_INPUT;
            }
        }
    } else {
        if (od) {
            *mode = DRV_PIN_MODE_AF_OD;
        } else {
            *mode = DRV_PIN_MODE_AF_PP;
        }

        *af = mux;
    }

    return 0;
}

int drv_pin_read(drv_pin_t *pin) {
    GPIO_Type *gpio = MCX_BoardGetGPIOInstance(pin->port);
    if (gpio == NULL) {
        return -ENOENT;
    }

    uint32_t gpio_value = GPIO_PinRead(gpio, pin->pin);

    return gpio_value ? 1 : 0;
}

int drv_pin_write(drv_pin_t *pin, bool value) {
    GPIO_Type *gpio = MCX_BoardGetGPIOInstance(pin->port);
    if (gpio == NULL) {
        return -ENOENT;
    }

    uint8_t val = value ? 1 : 0;

    GPIO_PinWrite(gpio, pin->pin, val);

    return 0;
}
