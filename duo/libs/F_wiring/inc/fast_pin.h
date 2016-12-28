/*
 ******************************************************************************
 *  Copyright (c) 2015 Particle Industries, Inc.  All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */


#ifndef FAST_PIN_H
#define	FAST_PIN_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "pinmap_hal.h"
#include "pinmap_impl.h"

/* Disabling USE_BIT_BAND since bitbanding is much slower! as per comment
 * by @pkourany on PR: https://github.com/spark/firmware/pull/556 */
#define USE_BIT_BAND 0

#if USE_BIT_BAND
/* Use CortexM3 Bit-Band access to perform GPIO atomic read-modify-write */

//Below is defined in stm32fxxx.h
//PERIPH_BASE     = 0x40000000 /* Peripheral base address in the alias region */
//PERIPH_BB_BASE  = 0x42000000 /* Peripheral base address in the bit-band region */

/* A mapping formula shows how to reference each word in the alias region to a
   corresponding bit in the bit-band region. The mapping formula is:
   bit_word_addr = bit_band_base + (byte_offset x 32) + (bit_number + 4)

where:
   - bit_word_addr: is the address of the word in the alias memory region that
                    maps to the targeted bit.
   - bit_band_base is the starting address of the alias region
   - byte_offset is the number of the byte in the bit-band region that contains
     the targeted bit
   - bit_number is the bit position of the targeted bit
*/

#define  GPIO_ResetBit_BB(Addr, Bit)    \
          (*(__IO uint32_t *) (PERIPH_BB_BASE | ((Addr - PERIPH_BASE) << 5) | ((Bit) << 2)) = 0)

#define GPIO_SetBit_BB(Addr, Bit)       \
          (*(__IO uint32_t *) (PERIPH_BB_BASE | ((Addr - PERIPH_BASE) << 5) | ((Bit) << 2)) = 1)

#define GPIO_GetBit_BB(Addr, Bit)       \
          (*(__IO uint32_t *) (PERIPH_BB_BASE | ((Addr - PERIPH_BASE) << 5) | ((Bit) << 2)))

static STM32_Pin_Info* pin_map = HAL_Pin_Map();

inline void pinSetFast(pin_t _pin) __attribute__((always_inline));
inline void pinResetFast(pin_t _pin) __attribute__((always_inline));
inline int32_t pinReadFast(pin_t _pin) __attribute__((always_inline));

inline void pinSetFast(pin_t _pin)
{
    GPIO_SetBit_BB((__IO uint32_t)&pin_map[_pin].gpio_peripheral->ODR, pin_map[_pin].gpio_pin_source);
}

inline void pinResetFast(pin_t _pin)
{
    GPIO_ResetBit_BB((__IO uint32_t)&pin_map[_pin].gpio_peripheral->ODR, pin_map[_pin].gpio_pin_source);
}

inline int32_t pinReadFast(pin_t _pin)
{
    return GPIO_GetBit_BB((__IO uint32_t)&pin_map[_pin].gpio_peripheral->IDR, pin_map[_pin].gpio_pin_source);
}

#else

#ifndef STM32F10X
    #if defined(STM32F10X_MD) || defined(STM32F10X_HD)
        #define STM32F10X
    #endif
#endif

#ifdef STM32F10X

inline void pinSetFast(pin_t _pin) __attribute__((always_inline));
inline void pinResetFast(pin_t _pin) __attribute__((always_inline));
inline int32_t pinReadFast(pin_t _pin) __attribute__((always_inline));

inline void pinSetFast(pin_t _pin)
{
    PIN_MAP[_pin].gpio_peripheral->BSRR = PIN_MAP[_pin].gpio_pin;
}

inline void pinResetFast(pin_t _pin)
{
    PIN_MAP[_pin].gpio_peripheral->BRR = PIN_MAP[_pin].gpio_pin;
}

inline int32_t pinReadFast(pin_t _pin)
{
	return ((PIN_MAP[_pin].gpio_peripheral->IDR & PIN_MAP[_pin].gpio_pin) == 0 ? LOW : HIGH);
}
#elif defined(STM32F2XX)
static STM32_Pin_Info* PIN_MAP = HAL_Pin_Map();

inline void pinSetFast(pin_t _pin) __attribute__((always_inline));
inline void pinResetFast(pin_t _pin) __attribute__((always_inline));
inline int32_t pinReadFast(pin_t _pin) __attribute__((always_inline));

inline void pinSetFast(pin_t _pin)
{
    PIN_MAP[_pin].gpio_peripheral->BSRRL = PIN_MAP[_pin].gpio_pin;
}

inline void pinResetFast(pin_t _pin)
{
    PIN_MAP[_pin].gpio_peripheral->BSRRH = PIN_MAP[_pin].gpio_pin;
}

inline int32_t pinReadFast(pin_t _pin)
{
	return ((PIN_MAP[_pin].gpio_peripheral->IDR & PIN_MAP[_pin].gpio_pin) == 0 ? LOW : HIGH);
}
#elif PLATFORM_ID==3

// make them unresolved symbols so attempted use will result in a linker error
void pinResetFast(pin_t _pin);
void pinSetFast(pin_t _pin);
void pinReadFast(pin_t _pin);
#else
    #warning "*** MCU architecture not supported by the fastPin library. ***"
    #define pinSetFast(pin) digitalWrite(pin, HIGH)
    #define pinResetFast(pin) digitalWrite(pin, LOW)
#endif

#endif //USE_BIT_BAND

inline void digitalWriteFast(pin_t pin, uint8_t value)
{
    if (value)
        pinSetFast(pin);
    else
        pinResetFast(pin);
}

#ifdef	__cplusplus
}
#endif

#endif	/* FAST_PIN_H */

