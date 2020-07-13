#include <stdint.h>
#include <mk20dx128.h>
#include "teensy_hal.h"

#define GPIO_NUMBER 32

void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init) {
    /* Check the parameters */
    assert_param(IS_GPIO_PIN(GPIO_Init->Pin));
    assert_param(IS_GPIO_MODE(GPIO_Init->Mode));
    assert_param(IS_GPIO_PULL(GPIO_Init->Pull));

    /* Configure the port pins */
    for (uint32_t position = 0; position < GPIO_NUMBER; position++) {
        uint32_t bitmask = 1 << position;
        if ((GPIO_Init->Pin & bitmask) == 0) {
            continue;
        }
        volatile uint32_t *port_pcr = GPIO_PIN_TO_PORT_PCR(GPIOx, position);

        /*--------------------- GPIO Mode Configuration ------------------------*/
        /* In case of Alternate function mode selection */
        if ((GPIO_Init->Mode == GPIO_MODE_AF_PP) || (GPIO_Init->Mode == GPIO_MODE_AF_OD)) {
            /* Check the Alternate function parameter */
            assert_param(IS_GPIO_AF(GPIO_Init->Alternate));
        } else if (GPIO_Init->Mode == GPIO_MODE_ANALOG) {
            GPIO_Init->Alternate = 0;
        } else {
            GPIO_Init->Alternate = 1;
        }

        /* Configure Alternate function mapped with the current IO */
        *port_pcr &= ~PORT_PCR_MUX_MASK;
        *port_pcr |= PORT_PCR_MUX(GPIO_Init->Alternate);

        /* Configure IO Direction mode (Input, Output, Alternate or Analog) */
        if (GPIO_Init->Mode == GPIO_MODE_INPUT || GPIO_Init->Mode == GPIO_MODE_ANALOG) {
            GPIOx->PDDR &= ~bitmask;
        } else {
            GPIOx->PDDR |= bitmask;
        }

        /* In case of Output or Alternate function mode selection */
        if ((GPIO_Init->Mode == GPIO_MODE_OUTPUT_PP) || (GPIO_Init->Mode == GPIO_MODE_AF_PP) ||
            (GPIO_Init->Mode == GPIO_MODE_OUTPUT_OD) || (GPIO_Init->Mode == GPIO_MODE_AF_OD)) {
            /* Check the Speed parameter */
            assert_param(IS_GPIO_SPEED(GPIO_Init->Speed));

            *port_pcr |= PORT_PCR_DSE;

            /* Configure the IO Speed */
            if (GPIO_Init->Speed > GPIO_SPEED_FREQ_MEDIUM) {
                *port_pcr &= ~PORT_PCR_SRE;
            } else {
                *port_pcr |= PORT_PCR_SRE;
            }

            /* Configure the IO Output Type */
            if (GPIO_Init->Mode & GPIO_OUTPUT_TYPE) {
                *port_pcr |= PORT_PCR_ODE;  // OD
            } else {
                *port_pcr &= ~PORT_PCR_ODE; // PP
            }
        } else {
            *port_pcr &= ~PORT_PCR_DSE;
        }

        /* Activate the Pull-up or Pull down resistor for the current IO */
        if (GPIO_Init->Pull == GPIO_NOPULL) {
            *port_pcr &= ~PORT_PCR_PE;
        } else {
            *port_pcr |= PORT_PCR_PE;
            if (GPIO_Init->Pull == GPIO_PULLDOWN) {
                *port_pcr &= ~PORT_PCR_PS;
            } else {
                *port_pcr |= PORT_PCR_PS;
            }
        }

        #if 0
        /*--------------------- EXTI Mode Configuration ------------------------*/
        /* Configure the External Interrupt or event for the current IO */
        if ((GPIO_Init->Mode & EXTI_MODE) == EXTI_MODE) {
            /* Enable SYSCFG Clock */
            __SYSCFG_CLK_ENABLE();

            temp = ((uint32_t)0x0F) << (4 * (position & 0x03));
            SYSCFG->EXTICR[position >> 2] &= ~temp;
            SYSCFG->EXTICR[position >> 2] |= ((uint32_t)(__HAL_GET_GPIO_SOURCE(GPIOx)) << (4 * (position & 0x03)));

            /* Clear EXTI line configuration */
            EXTI->IMR &= ~((uint32_t)iocurrent);
            EXTI->EMR &= ~((uint32_t)iocurrent);

            if ((GPIO_Init->Mode & GPIO_MODE_IT) == GPIO_MODE_IT) {
                EXTI->IMR |= iocurrent;
            }
            if ((GPIO_Init->Mode & GPIO_MODE_EVT) == GPIO_MODE_EVT) {
                EXTI->EMR |= iocurrent;
            }

            /* Clear Rising Falling edge configuration */
            EXTI->RTSR &= ~((uint32_t)iocurrent);
            EXTI->FTSR &= ~((uint32_t)iocurrent);

            if ((GPIO_Init->Mode & RISING_EDGE) == RISING_EDGE) {
                EXTI->RTSR |= iocurrent;
            }
            if ((GPIO_Init->Mode & FALLING_EDGE) == FALLING_EDGE) {
                EXTI->FTSR |= iocurrent;
            }
        }
        #endif
    }
}
