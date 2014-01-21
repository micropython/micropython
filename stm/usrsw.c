#include <stm_misc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_exti.h>
#include <stm32f4xx_syscfg.h>
#include <stm32f4xx_rcc.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "usrsw.h"

#if defined (PYBOARD)
    #define USRSW_PORT          (GPIOA)
    #define USRSW_PIN           (GPIO_Pin_13)
    #define USRSW_PUPD          (GPIO_PuPd_UP)
    #define USRSW_EXTI_PIN      (EXTI_PinSource13)
    #define USRSW_EXTI_PORT     (EXTI_PortSourceGPIOA)
    #define USRSW_EXTI_LINE     (EXTI_Line13)
    #define USRSW_EXTI_IRQN     (EXTI15_10_IRQn)
    #define USRSW_EXTI_EDGE     (EXTI_Trigger_Rising)
#elif defined (PYBOARD4)
    #define USRSW_PORT          (GPIOB)
    #define USRSW_PIN           (GPIO_Pin_3)
    #define USRSW_PUPD          (GPIO_PuPd_UP)
    #define USRSW_EXTI_PIN      (EXTI_PinSource3)
    #define USRSW_EXTI_PORT     (EXTI_PortSourceGPIOB)
    #define USRSW_EXTI_LINE     (EXTI_Line3)
    #define USRSW_EXTI_IRQN     (EXTI15_10_IRQn)
    #define USRSW_EXTI_EDGE     (EXTI_Trigger_Rising)
#elif defined (STM32F4DISC)
    #define USRSW_PORT          (GPIOA)
    #define USRSW_PIN           (GPIO_Pin_0)
    #define USRSW_PUPD          (GPIO_PuPd_NOPULL)
    #define USRSW_EXTI_PIN      (EXTI_PinSource0)
    #define USRSW_EXTI_PORT     (EXTI_PortSourceGPIOA)
    #define USRSW_EXTI_LINE     (EXTI_Line0)
    #define USRSW_EXTI_IRQN     (EXTI0_IRQn)
    #define USRSW_EXTI_EDGE     (EXTI_Trigger_Falling)
#endif

void switch_init(void) {
    // make it an input with pull-up
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin  = USRSW_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = USRSW_PUPD;
    GPIO_Init(USRSW_PORT, &GPIO_InitStructure);

    // the rest does the EXTI interrupt

    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Connect EXTI Line to GPIO pin */
    SYSCFG_EXTILineConfig(USRSW_EXTI_PORT, USRSW_EXTI_PIN);

    /* Configure EXTI Line */
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = USRSW_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = USRSW_EXTI_EDGE;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set EXTI15_10 Interrupt to the lowest priority */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USRSW_EXTI_IRQN;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

int switch_get(void) {
#if defined (PYBOARD) || defined (PYBOARD4)
    if (USRSW_PORT->IDR & USRSW_PIN) {
        // pulled high, so switch is not pressed
        return 0;
    } else {
        // pulled low, so switch is pressed
        return 1;
    }
#elif defined (STM32F4DISC)
    /* switch pulled down */
    if (USRSW_PORT->IDR & USRSW_PIN) {
        // pulled high, so switch is pressed
        return 1;
    } else {
        // pulled low, so switch is not pressed
        return 0;
    }
#endif
}

/******************************************************************************/
/* Micro Python bindings                                                      */

static mp_obj_t pyb_switch(void) {
    if (switch_get()) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}

MP_DEFINE_CONST_FUN_OBJ_0(pyb_switch_obj, pyb_switch);
