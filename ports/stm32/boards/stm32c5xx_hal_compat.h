/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Matt Trentini
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

// Compatibility shim for the CubeMX2-era HAL (STM32C5 and later).
//
// The C5 HAL is a complete rewrite with different type names, function
// signatures, and enum values. This header maps the traditional HAL names
// used throughout MicroPython's STM32 port to their C5 equivalents.

#ifndef MICROPY_INCLUDED_STM32C5XX_HAL_COMPAT_H
#define MICROPY_INCLUDED_STM32C5XX_HAL_COMPAT_H

#include <string.h>

// Include HAL and LL headers needed by MicroPython.
// The C5 HAL uses stm32_hal.h with USE_HAL_*_MODULE guards, but MicroPython
// includes stm32c5xx_hal.h directly, so we pull in the modules here.
#include "stm32c5xx_hal_rcc.h"
#include "stm32c5xx_hal_cortex.h"
#include "stm32c5xx_hal_dma.h"
#include "stm32c5xx_hal_flash.h"
#include "stm32c5xx_hal_gpio.h"
#include "stm32c5xx_hal_i2c.h"
#include "stm32c5xx_hal_icache.h"
#include "stm32c5xx_hal_pwr.h"
#include "stm32c5xx_hal_rtc.h"
#include "stm32c5xx_hal_spi.h"
#include "stm32c5xx_hal_tim.h"
#include "stm32c5xx_hal_uart.h"
#include "stm32c5xx_ll_gpio.h"
#include "stm32c5xx_ll_bus.h"
#include "stm32c5xx_ll_rcc.h"
#include "stm32c5xx_ll_pwr.h"
#include "stm32c5xx_ll_flash.h"
#include "stm32c5xx_ll_usart.h"
#include "stm32c5xx_ll_rtc.h"

// ---------------------------------------------------------------------------
// HAL status type
// ---------------------------------------------------------------------------
// The new HAL uses hal_status_t with different numeric values (HAL_OK=0xEAEAEAEA).
// MicroPython code generally just checks == HAL_OK, so the typedef is enough.
typedef hal_status_t HAL_StatusTypeDef;

// ---------------------------------------------------------------------------
// GPIO compatibility
// ---------------------------------------------------------------------------

// Mode defines used by MicroPython (pin.c, mphalport.h, board configs)
#define GPIO_MODE_INPUT             LL_GPIO_MODE_INPUT
#define GPIO_MODE_OUTPUT_PP         LL_GPIO_MODE_OUTPUT
#define GPIO_MODE_OUTPUT_OD         (LL_GPIO_MODE_OUTPUT | (LL_GPIO_OUTPUT_OPENDRAIN << 4))
#define GPIO_MODE_AF_PP             LL_GPIO_MODE_ALTERNATE
#define GPIO_MODE_AF_OD             (LL_GPIO_MODE_ALTERNATE | (LL_GPIO_OUTPUT_OPENDRAIN << 4))
#define GPIO_MODE_ANALOG            LL_GPIO_MODE_ANALOG

// Interrupt/event mode defines (used in board configs for user switch)
// These encode both the EXTI mode and trigger in a single value.
#define GPIO_MODE_IT_RISING         (0x10110000u)
#define GPIO_MODE_IT_FALLING        (0x10210000u)
#define GPIO_MODE_IT_RISING_FALLING (0x10310000u)
#define GPIO_MODE_EVT_RISING        (0x10120000u)
#define GPIO_MODE_EVT_FALLING       (0x10220000u)
#define GPIO_MODE_EVT_RISING_FALLING (0x10320000u)

// Pull defines
#define GPIO_NOPULL     LL_GPIO_PULL_NO
#define GPIO_PULLUP     LL_GPIO_PULL_UP
#define GPIO_PULLDOWN   LL_GPIO_PULL_DOWN

// Speed defines
#define GPIO_SPEED_FREQ_LOW         LL_GPIO_SPEED_FREQ_LOW
#define GPIO_SPEED_FREQ_MEDIUM      LL_GPIO_SPEED_FREQ_MEDIUM
#define GPIO_SPEED_FREQ_HIGH        LL_GPIO_SPEED_FREQ_HIGH
#define GPIO_SPEED_FREQ_VERY_HIGH   LL_GPIO_SPEED_FREQ_VERY_HIGH

// GPIO_InitTypeDef compatibility struct
typedef struct {
    uint32_t Pin;
    uint32_t Mode;
    uint32_t Pull;
    uint32_t Speed;
    uint32_t Alternate;
} GPIO_InitTypeDef;

// HAL_GPIO_Init compatibility wrapper (implemented as inline using LL)
static inline void HAL_GPIO_Init_Compat(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init) {
    uint32_t position = 0;
    uint32_t pin_mask = GPIO_Init->Pin;
    while (pin_mask) {
        if (pin_mask & 1) {
            LL_GPIO_SetPinMode(GPIOx, 1 << position, GPIO_Init->Mode & 0x3);
            if ((GPIO_Init->Mode & 0x3) == LL_GPIO_MODE_OUTPUT || (GPIO_Init->Mode & 0x3) == LL_GPIO_MODE_ALTERNATE) {
                LL_GPIO_SetPinSpeed(GPIOx, 1 << position, GPIO_Init->Speed);
                LL_GPIO_SetPinOutputType(GPIOx, 1 << position, (GPIO_Init->Mode >> 4) & 1);
            }
            LL_GPIO_SetPinPull(GPIOx, 1 << position, GPIO_Init->Pull);
            if ((GPIO_Init->Mode & 0x3) == LL_GPIO_MODE_ALTERNATE) {
                if (position < 8) {
                    LL_GPIO_SetAFPin_0_7(GPIOx, 1 << position, GPIO_Init->Alternate);
                } else {
                    LL_GPIO_SetAFPin_8_15(GPIOx, 1 << position, GPIO_Init->Alternate);
                }
            }
        }
        position++;
        pin_mask >>= 1;
    }
}
#define HAL_GPIO_Init(gpio, init) HAL_GPIO_Init_Compat(gpio, init)

// HAL_GPIO_DeInit - just reset pin to analog (input) mode
static inline void HAL_GPIO_DeInit_Compat(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin) {
    uint32_t position = 0;
    while (GPIO_Pin) {
        if (GPIO_Pin & 1) {
            LL_GPIO_SetPinMode(GPIOx, 1 << position, LL_GPIO_MODE_ANALOG);
            LL_GPIO_SetPinPull(GPIOx, 1 << position, LL_GPIO_PULL_NO);
        }
        position++;
        GPIO_Pin >>= 1;
    }
}
#define HAL_GPIO_DeInit(gpio, pin) HAL_GPIO_DeInit_Compat(gpio, pin)

// GPIO pin read/write/toggle
#define HAL_GPIO_ReadPin(gpio, pin)     (((gpio)->IDR & (pin)) ? 1 : 0)
#define HAL_GPIO_WritePin(gpio, pin, v) ((v) ? ((gpio)->BSRR = (pin)) : ((gpio)->BSRR = (pin) << 16))
#define HAL_GPIO_TogglePin(gpio, pin)   ((gpio)->ODR ^= (pin))
#define GPIO_PIN_SET    1
#define GPIO_PIN_RESET  0

// GPIO pin defines (GPIO_PIN_0 .. GPIO_PIN_15)
#define GPIO_PIN_0      (1 << 0)
#define GPIO_PIN_1      (1 << 1)
#define GPIO_PIN_2      (1 << 2)
#define GPIO_PIN_3      (1 << 3)
#define GPIO_PIN_4      (1 << 4)
#define GPIO_PIN_5      (1 << 5)
#define GPIO_PIN_6      (1 << 6)
#define GPIO_PIN_7      (1 << 7)
#define GPIO_PIN_8      (1 << 8)
#define GPIO_PIN_9      (1 << 9)
#define GPIO_PIN_10     (1 << 10)
#define GPIO_PIN_11     (1 << 11)
#define GPIO_PIN_12     (1 << 12)
#define GPIO_PIN_13     (1 << 13)
#define GPIO_PIN_14     (1 << 14)
#define GPIO_PIN_15     (1 << 15)

// ---------------------------------------------------------------------------
// GPIO AF compatibility (generated pins_af_const.h uses GPIO_AFn_xxx format)
// ---------------------------------------------------------------------------
#define GPIO_AF1_TIM1       HAL_GPIO_AF_1
#define GPIO_AF1_TIM2       HAL_GPIO_AF_1
#define GPIO_AF1_TIM4       HAL_GPIO_AF_1
#define GPIO_AF1_TIM16      HAL_GPIO_AF_1
#define GPIO_AF1_TIM17      HAL_GPIO_AF_1
#define GPIO_AF2_TIM1       HAL_GPIO_AF_2
#define GPIO_AF2_TIM3       HAL_GPIO_AF_2
#define GPIO_AF2_TIM4       HAL_GPIO_AF_2
#define GPIO_AF2_TIM5       HAL_GPIO_AF_2
#define GPIO_AF4_I2C1       HAL_GPIO_AF_4
#define GPIO_AF4_I2C2       HAL_GPIO_AF_4
#define GPIO_AF4_USART2     HAL_GPIO_AF_4
#define GPIO_AF5_SPI1       HAL_GPIO_AF_5
#define GPIO_AF5_SPI2       HAL_GPIO_AF_5
#define GPIO_AF6_SPI3       HAL_GPIO_AF_6
#define GPIO_AF7_USART1     HAL_GPIO_AF_7
#define GPIO_AF7_USART2     HAL_GPIO_AF_7
#define GPIO_AF7_USART3     HAL_GPIO_AF_7
#define GPIO_AF8_UART4      HAL_GPIO_AF_8
#define GPIO_AF8_UART5      HAL_GPIO_AF_8
#define GPIO_AF9_FDCAN1     HAL_GPIO_AF_9
#define GPIO_AF9_FDCAN2     HAL_GPIO_AF_9
#define GPIO_AF10_USB       HAL_GPIO_AF_10
#define GPIO_AF14_TIM2      HAL_GPIO_AF_14
#define GPIO_AF15_EVENTOUT  HAL_GPIO_AF_15

// ---------------------------------------------------------------------------
// NVIC / Cortex compatibility
// ---------------------------------------------------------------------------
#define NVIC_PRIORITYGROUP_4    (3) // 4 bits for preemption, 0 for subpriority
#define HAL_NVIC_SetPriorityGrouping(x)   NVIC_SetPriorityGrouping(x)
#define HAL_NVIC_SetPriority(irq, pre, sub) NVIC_SetPriority(irq, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), pre, sub))
#define HAL_NVIC_EnableIRQ(irq)           NVIC_EnableIRQ(irq)
#define HAL_NVIC_DisableIRQ(irq)          NVIC_DisableIRQ(irq)
#define HAL_NVIC_SystemReset()            NVIC_SystemReset()

// ---------------------------------------------------------------------------
// Peripheral handle type stubs
// ---------------------------------------------------------------------------
// These are opaque to most MicroPython code; the handles are created/used
// by the HAL but MicroPython just stores pointers to them.

typedef hal_i2c_handle_t I2C_HandleTypeDef;
typedef hal_spi_handle_t SPI_HandleTypeDef;
typedef hal_tim_handle_t TIM_HandleTypeDef;
typedef hal_uart_handle_t UART_HandleTypeDef;

// RTC handle and types
typedef struct {
    RTC_TypeDef *Instance;
} RTC_HandleTypeDef;

typedef struct {
    uint8_t Hours;
    uint8_t Minutes;
    uint8_t Seconds;
    uint8_t TimeFormat;
    uint32_t SubSeconds;
    uint32_t SecondFraction;
    uint32_t DayLightSaving;
    uint32_t StoreOperation;
} RTC_TimeTypeDef;

typedef struct {
    uint8_t WeekDay;
    uint8_t Month;
    uint8_t Date;
    uint8_t Year;
} RTC_DateTypeDef;

#define RTC_FORMAT_BIN  0
#define RTC_FORMAT_BCD  1

// Stub RTC functions - zero-fill results to avoid uninitialized warnings
static inline hal_status_t HAL_RTC_GetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime, uint32_t Format) {
    (void)hrtc;
    (void)Format;
    memset(sTime, 0, sizeof(*sTime));
    return HAL_ERROR; // TODO: implement via LL
}
static inline hal_status_t HAL_RTC_GetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate, uint32_t Format) {
    (void)hrtc;
    (void)Format;
    memset(sDate, 0, sizeof(*sDate));
    return HAL_ERROR; // TODO: implement via LL
}

// ---------------------------------------------------------------------------
// UART defines
// ---------------------------------------------------------------------------
#define UART_WORDLENGTH_8B      HAL_UART_WORD_LENGTH_8_BIT
#define UART_WORDLENGTH_9B      HAL_UART_WORD_LENGTH_9_BIT
#define UART_STOPBITS_1         HAL_UART_STOP_BIT_1
#define UART_STOPBITS_2         HAL_UART_STOP_BIT_2
#define UART_PARITY_NONE        HAL_UART_PARITY_NONE
#define UART_PARITY_EVEN        HAL_UART_PARITY_EVEN
#define UART_PARITY_ODD         HAL_UART_PARITY_ODD
#define UART_HWCONTROL_NONE     LL_USART_HWCONTROL_NONE
#define UART_HWCONTROL_RTS      LL_USART_HWCONTROL_RTS
#define UART_HWCONTROL_CTS      LL_USART_HWCONTROL_CTS

// UART flag defines (ISR register bits)
#define UART_FLAG_RXNE          USART_ISR_RXNE_RXFNE
#define UART_FLAG_IDLE          USART_ISR_IDLE
#define UART_FLAG_TXE           USART_ISR_TXE_TXFNF
#define UART_FLAG_TC            USART_ISR_TC
#define UART_FLAG_ORE           USART_ISR_ORE

// ---------------------------------------------------------------------------
// SPI defines
// ---------------------------------------------------------------------------
#define SPI_FIRSTBIT_MSB        LL_SPI_MSB_FIRST
#define SPI_FIRSTBIT_LSB        LL_SPI_LSB_FIRST

// ---------------------------------------------------------------------------
// ADC compatibility
// ---------------------------------------------------------------------------
// ADC channel defines
#define ADC_CHANNEL_VREFINT     0
#define ADC_CHANNEL_TEMPSENSOR  1
#define __HAL_RCC_ADC1_CLK_ENABLE()  __HAL_RCC_ADC_CLK_ENABLE()
#define __HAL_RCC_ADC2_CLK_ENABLE()  __HAL_RCC_ADC_CLK_ENABLE()
#define __HAL_RCC_ADC3_CLK_ENABLE()  __HAL_RCC_ADC_CLK_ENABLE()

// ---------------------------------------------------------------------------
// RCC register compatibility
// ---------------------------------------------------------------------------
// C5 uses RSR instead of CSR for reset status
#define RCC_CSR_RMVF        RCC_RSR_RMVF
#define RCC_CSR_PINRSTF     RCC_RSR_PINRSTF
#define RCC_CSR_IWDGRSTF    RCC_RSR_IWDGRSTF
#define RCC_CSR_WWDGRSTF    RCC_RSR_WWDGRSTF

// EXTI register name compatibility (C5 uses numbered registers)
#define EXTI_IMR    IMR1
#define EXTI_EMR    EMR1
#define EXTI_RTSR   RTSR1
#define EXTI_FTSR   FTSR1
#define EXTI_SWIER  SWIER1
#define EXTI_PR     RPR1

// IRQ name compatibility
#ifndef EXTI9_5_IRQn
#define EXTI9_5_IRQn        EXTI5_IRQn
#endif
#ifndef EXTI15_10_IRQn
#define EXTI15_10_IRQn      EXTI10_IRQn
#endif
#ifndef PVD_IRQn
#define PVD_IRQn            PVD_AVD_IRQn
#endif

// RTC register compatibility (C5 uses SR instead of ISR)
#define RTC_ISR_WUTF        RTC_SR_WUTF

// SysTick defines
#define SYSTICK_CLKSOURCE_HCLK  SysTick_CTRL_CLKSOURCE_Msk

// GPIO assertion macros (used in pin.c, not defined in C5 HAL)
#define IS_GPIO_MODE(x)     (1)
#define IS_GPIO_PULL(x)     (1)
#define IS_GPIO_AF(x)       (1)

// UART additional defines
#define UART_FLAG_PE            USART_ISR_PE
#define UART_MODE_TX_RX         (USART_CR1_TE | USART_CR1_RE)
#define UART_OVERSAMPLING_16    HAL_UART_OVERSAMPLING_16
#define UART_ONE_BIT_SAMPLE_DISABLE HAL_UART_ONE_BIT_SAMPLE_DISABLE

// UART clock disable/reset macros
#define __HAL_RCC_USART1_CLK_DISABLE()  LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_USART1)
#define __HAL_RCC_USART2_CLK_DISABLE()  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_USART2)
#define __HAL_RCC_USART3_CLK_DISABLE()  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_USART3)
#define __HAL_RCC_UART4_CLK_DISABLE()   LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_UART4)
#define __HAL_RCC_UART5_CLK_DISABLE()   LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_UART5)
#define __HAL_RCC_UART7_CLK_DISABLE()   ((void)0) // UART7 not available on all C5 variants
#define __HAL_RCC_LPUART1_CLK_DISABLE() LL_APB3_GRP1_DisableClock(LL_APB3_GRP1_PERIPH_LPUART1)

// UART reset macros (map to HAL_RCC_*_Reset functions)
#define __HAL_RCC_USART1_FORCE_RESET()   HAL_RCC_USART1_Reset()
#define __HAL_RCC_USART1_RELEASE_RESET() ((void)0)
#define __HAL_RCC_USART2_FORCE_RESET()   HAL_RCC_USART2_Reset()
#define __HAL_RCC_USART2_RELEASE_RESET() ((void)0)
#define __HAL_RCC_USART3_FORCE_RESET()   HAL_RCC_USART3_Reset()
#define __HAL_RCC_USART3_RELEASE_RESET() ((void)0)
#define __HAL_RCC_UART4_FORCE_RESET()    HAL_RCC_UART4_Reset()
#define __HAL_RCC_UART4_RELEASE_RESET()  ((void)0)
#define __HAL_RCC_UART5_FORCE_RESET()    HAL_RCC_UART5_Reset()
#define __HAL_RCC_UART5_RELEASE_RESET()  ((void)0)
#define __HAL_RCC_UART7_FORCE_RESET()    ((void)0)
#define __HAL_RCC_UART7_RELEASE_RESET()  ((void)0)
#define __HAL_RCC_LPUART1_FORCE_RESET()  HAL_RCC_LPUART1_Reset()
#define __HAL_RCC_LPUART1_RELEASE_RESET() ((void)0)

// Function renames / signature compatibility
#define HAL_RCC_GetSysClockFreq HAL_RCC_GetSYSCLKFreq
// C5 HAL_InitTick takes (freq, priority) but MicroPython calls with just (priority)
#define HAL_InitTick(prio) HAL_InitTick(HAL_TICK_FREQ_DEFAULT, prio)

// HAL_GetDEVID / HAL_GetREVID stubs
static inline uint32_t HAL_GetDEVID(void) {
    return DBGMCU->IDCODE & 0xFFFU;
}
static inline uint32_t HAL_GetREVID(void) {
    return (DBGMCU->IDCODE >> 16) & 0xFFFFU;
}

// USART_CR1_SBK doesn't exist on C5 (send break is done differently)
#ifndef USART_CR1_SBK
#define USART_CR1_SBK 0 // no-op
#endif

// ---------------------------------------------------------------------------
// RCC compatibility types
// ---------------------------------------------------------------------------
// Stub for powerctrl.h which references RCC_ClkInitTypeDef.
// C5 doesn't use the traditional PLL/clock init structures.
typedef struct {
    uint32_t ClockType;
    uint32_t SYSCLKSource;
    uint32_t AHBCLKDivider;
    uint32_t APB1CLKDivider;
    uint32_t APB2CLKDivider;
    uint32_t APB3CLKDivider;
} RCC_ClkInitTypeDef;

typedef struct {
    uint32_t OscillatorType;
} RCC_OscInitTypeDef;

// ---------------------------------------------------------------------------
// DMA compatibility
// ---------------------------------------------------------------------------
// Stub DMA_InitTypeDef for dma.c compilation. C5 uses LPDMA with a different
// API; actual DMA support will be added later.
typedef struct {
    uint32_t Request;
    uint32_t Direction;
    uint32_t PeriphInc;
    uint32_t MemInc;
    uint32_t PeriphDataAlignment;
    uint32_t MemDataAlignment;
    uint32_t Mode;
    uint32_t Priority;
} DMA_InitTypeDef;

typedef struct {
    DMA_Channel_TypeDef *Instance;
    DMA_InitTypeDef Init;
} DMA_HandleTypeDef;

// ---------------------------------------------------------------------------
// Flash latency
// ---------------------------------------------------------------------------
#define FLASH_LATENCY_0     LL_FLASH_LATENCY_0WS
#define FLASH_LATENCY_1     LL_FLASH_LATENCY_1WS
#define FLASH_LATENCY_2     LL_FLASH_LATENCY_2WS
#define FLASH_LATENCY_3     LL_FLASH_LATENCY_3WS
#define FLASH_LATENCY_4     LL_FLASH_LATENCY_4WS
#define FLASH_LATENCY_5     LL_FLASH_LATENCY_5WS

#define __HAL_FLASH_SET_LATENCY(lat)    LL_FLASH_SetLatency(FLASH, lat)
#define __HAL_FLASH_GET_LATENCY()       LL_FLASH_GetLatency(FLASH)

// ---------------------------------------------------------------------------
// Flash erase/program compatibility
// ---------------------------------------------------------------------------
// Stub types and functions for flash.c compilation.
// Actual flash operations need adaptation for C5's HAL API.

typedef struct {
    uint32_t TypeErase;
    uint32_t Banks;
    uint32_t Page;
    uint32_t NbPages;
} FLASH_EraseInitTypeDef;

#define FLASH_TYPEERASE_PAGES   0
#ifndef FLASH_BANK_1
#define FLASH_BANK_1            1
#endif
#ifndef FLASH_BANK_2
#define FLASH_BANK_2            2
#endif
#ifndef FLASH_BANK_SIZE
#define FLASH_BANK_SIZE         (FLASH_SIZE / 2)
#endif

// Stub for HAL_FLASHEx_Erase - will need real implementation for filesystem
static inline hal_status_t HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *PageError) {
    (void)pEraseInit;
    (void)PageError;
    return HAL_ERROR; // TODO: implement
}

// Stub for flash programming
#define FLASH_TYPEPROGRAM_QUADWORD 0
static inline hal_status_t HAL_FLASH_Program_Compat(uint32_t TypeProgram, uint32_t Address, uint32_t DataAddress) {
    (void)TypeProgram;
    (void)Address;
    (void)DataAddress;
    return HAL_ERROR; // TODO: implement
}
#define HAL_FLASH_Program HAL_FLASH_Program_Compat

static inline hal_status_t HAL_FLASH_Unlock_Compat(void) {
    return HAL_OK;
}
static inline hal_status_t HAL_FLASH_Lock_Compat(void) {
    return HAL_OK;
}
#define HAL_FLASH_Unlock HAL_FLASH_Unlock_Compat
#define HAL_FLASH_Lock HAL_FLASH_Lock_Compat

#define __HAL_FLASH_CLEAR_FLAG(flags) ((void)0)
#define FLASH_FLAG_ALL_ERRORS 0

// ---------------------------------------------------------------------------
// RCC clock enable macros
// ---------------------------------------------------------------------------
// MicroPython uses __HAL_RCC_GPIOx_CLK_ENABLE() in various drivers.
// Map to the LL bus clock functions.
#define __HAL_RCC_GPIOA_CLK_ENABLE()    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA)
#define __HAL_RCC_GPIOB_CLK_ENABLE()    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB)
#define __HAL_RCC_GPIOC_CLK_ENABLE()    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC)
#define __HAL_RCC_GPIOD_CLK_ENABLE()    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD)
#define __HAL_RCC_GPIOE_CLK_ENABLE()    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOE)
#define __HAL_RCC_GPIOH_CLK_ENABLE()    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOH)

// UART clock enables
#define __HAL_RCC_USART1_CLK_ENABLE()   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1)
#define __HAL_RCC_USART2_CLK_ENABLE()   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2)
#define __HAL_RCC_USART3_CLK_ENABLE()   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3)
#define __HAL_RCC_UART4_CLK_ENABLE()    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART4)
#define __HAL_RCC_UART5_CLK_ENABLE()    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART5)

// SPI clock enables
#define __HAL_RCC_SPI1_CLK_ENABLE()     LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1)
#define __HAL_RCC_SPI2_CLK_ENABLE()     LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2)
#define __HAL_RCC_SPI3_CLK_ENABLE()     LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI3)

// I2C clock enables
#define __HAL_RCC_I2C1_CLK_ENABLE()     LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1)
#define __HAL_RCC_I2C2_CLK_ENABLE()     LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2)

// Timer clock enables
#define __HAL_RCC_TIM1_CLK_ENABLE()     LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1)
#define __HAL_RCC_TIM2_CLK_ENABLE()     LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2)
#define __HAL_RCC_TIM3_CLK_ENABLE()     LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3)
#define __HAL_RCC_TIM4_CLK_ENABLE()     LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4)
#define __HAL_RCC_TIM5_CLK_ENABLE()     LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM5)
#define __HAL_RCC_TIM6_CLK_ENABLE()     LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6)
#define __HAL_RCC_TIM7_CLK_ENABLE()     LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7)

// Other peripheral clock enables
#define __HAL_RCC_CRC_CLK_ENABLE()      LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CRC)
#define __HAL_RCC_RNG_CLK_ENABLE()      LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_RNG)
#define __HAL_RCC_ADC_CLK_ENABLE()      LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12)

// ---------------------------------------------------------------------------
// PWR voltage scaling
// ---------------------------------------------------------------------------
#define PWR_REGULATOR_VOLTAGE_SCALE0    LL_PWR_REGU_VOLTAGE_SCALE0
#define PWR_REGULATOR_VOLTAGE_SCALE1    LL_PWR_REGU_VOLTAGE_SCALE1
#define PWR_REGULATOR_VOLTAGE_SCALE2    LL_PWR_REGU_VOLTAGE_SCALE2

// ---------------------------------------------------------------------------
// Miscellaneous
// ---------------------------------------------------------------------------

// IRQ priority variable expected by HAL
#ifndef uwTickPrio
extern uint32_t uwTickPrio;
#endif

#endif // MICROPY_INCLUDED_STM32C5XX_HAL_COMPAT_H
