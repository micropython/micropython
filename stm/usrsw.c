#include <stm_misc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_exti.h>
#include <stm32f4xx_syscfg.h>
#include <stm32f4xx_rcc.h>

#include "misc.h"
#include "mpconfig.h"
#include "obj.h"
#include "usrsw.h"

#define PYB_USRSW_PORT (GPIOA)
#define PYB_USRSW_PIN (GPIO_Pin_13)

void sw_init(void) {
    // make it an input with pull-up
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = PYB_USRSW_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(PYB_USRSW_PORT, &GPIO_InitStructure);

    // the rest does the EXTI interrupt

    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Connect EXTI Line13 to PA13 pin */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource13);

    /* Configure EXTI Line13, rising edge */
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line13;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set EXTI15_10 Interrupt to the lowest priority */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

int sw_get(void) {
    if (PYB_USRSW_PORT->IDR & PYB_USRSW_PIN) {
        // pulled high, so switch is not pressed
        return 0;
    } else {
        // pulled low, so switch is pressed
        return 1;
    }
}

/******************************************************************************/
/* Micro Python bindings                                                      */

mp_obj_t pyb_sw(void) {
    if (sw_get()) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}


