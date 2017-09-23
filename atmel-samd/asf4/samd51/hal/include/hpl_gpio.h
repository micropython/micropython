/**
 * \file
 *
 * \brief Port related functionality declaration.
 *
 * Copyright (C) 2014-2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _HPL_GPIO_H_INCLUDED
#define _HPL_GPIO_H_INCLUDED

/**
 * \addtogroup HPL Port
 *
 * \section hpl_port_rev Revision History
 * - v1.0.0 Initial Release
 *
 *@{
 */

#include <compiler.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * \brief Macros for the pin and port group, lower 5
 * bits stands for pin number in the group, higher 3
 * bits stands for port group
 */
#define GPIO_PIN(n) (((n)&0x1Fu) << 0)
#define GPIO_PORT(n) ((n) >> 5)
#define GPIO(port, pin) ((((port)&0x7u) << 5) + ((pin)&0x1Fu))
#define GPIO_PIN_FUNCTION_OFF 0xffffffff

/**
 * \brief PORT pull mode settings
 */
enum gpio_pull_mode { GPIO_PULL_OFF, GPIO_PULL_UP, GPIO_PULL_DOWN };

/**
 * \brief PORT direction settins
 */
enum gpio_direction { GPIO_DIRECTION_OFF, GPIO_DIRECTION_IN, GPIO_DIRECTION_OUT };

/**
 * \brief PORT group abstraction
 */

enum gpio_port { GPIO_PORTA, GPIO_PORTB, GPIO_PORTC, GPIO_PORTD, GPIO_PORTE };

/**
 * \name HPL functions
 */
//@{
/**
 * \brief Port initialization function
 *
 * Port initialization function should setup the port module based
 * on a static configuration file, this function should normally
 * not be called directly, but is a part of hal_init()
 */
void _gpio_init(void);

/**
 * \brief Set direction on port with mask
 *
 * Set data direction for each pin, or disable the pin
 *
 * \param[in] port      Ports are grouped into groups of maximum 32 pins,
 *                      GPIO_PORTA = group 0, GPIO_PORTB = group 1, etc
 * \param[in] mask      Bit mask where 1 means apply direction setting to the
 *                      corresponding pin
 * \param[in] direction GPIO_DIRECTION_OFF  = set pin direction to input
 *                      and disable input buffer to disable the pin
 *                      GPIO_DIRECTION_IN   = set pin direction to input
 *                      and enable input buffer to enable the pin
 *                      GPIO_DIRECTION_OUT  = set pin direction to output
 *                      and disable input buffer
 */
static inline void _gpio_set_direction(const enum gpio_port port, const uint32_t mask,
                                       const enum gpio_direction direction);

/**
 * \brief Set output level on port with mask
 *
 * Sets output state on pin to high or low with pin masking
 *
 * \param[in] port  Ports are grouped into groups of maximum 32 pins,
 *                  GPIO_PORTA = group 0, GPIO_PORTB = group 1, etc
 * \param[in] mask  Bit mask where 1 means apply direction setting to
 *                  the corresponding pin
 * \param[in] level true  = pin level is set to 1
 *                  false = pin level is set to 0
 */
static inline void _gpio_set_level(const enum gpio_port port, const uint32_t mask, const bool level);

/**
 * \brief Change output level to the opposite with mask
 *
 * Change pin output level to the opposite with pin masking
 *
 * \param[in] port  Ports are grouped into groups of maximum 32 pins,
 *                  GPIO_PORTA = group 0, GPIO_PORTB = group 1, etc
 * \param[in] mask  Bit mask where 1 means apply direction setting to
 *                  the corresponding pin
 */
static inline void _gpio_toggle_level(const enum gpio_port port, const uint32_t mask);

/**
 * \brief Get input levels on all port pins
 *
 * Get input level on all port pins, will read IN register if configured to
 * input and OUT register if configured as output
 *
 * \param[in] port  Ports are grouped into groups of maximum 32 pins,
 *                  GPIO_PORTA = group 0, GPIO_PORTB = group 1, etc
 */
static inline uint32_t _gpio_get_level(const enum gpio_port port);

/**
 * \brief Set pin pull mode
 *
 * Set pull mode on a single pin
 *
 * \notice This function will automatically change pin direction to input
 *
 * \param[in] port      Ports are grouped into groups of maximum 32 pins,
 *                      GPIO_PORTA = group 0, GPIO_PORTB = group 1, etc
 * \param[in] pin       The pin in the group that pull mode should be selected
 *                      for
 * \param[in] pull_mode GPIO_PULL_OFF  = pull resistor on pin is disabled
 *                      GPIO_PULL_DOWN = pull resistor on pin will pull pin
 *                      level to ground level
 *                      GPIO_PULL_UP   = pull resistor on pin will pull pin
 *                      level to VCC
 */
static inline void _gpio_set_pin_pull_mode(const enum gpio_port port, const uint8_t pin,
                                           const enum gpio_pull_mode pull_mode);

/**
 * \brief Set gpio function
 *
 * Select which function a gpio is used for
 *
 * \param[in] gpio     The gpio to set function for
 * \param[in] function The gpio function is given by a 32-bit wide bitfield
 *                     found in the header files for the device
 *
 */
static inline void _gpio_set_pin_function(const uint32_t gpio, const uint32_t function);

#include <hpl_gpio_base.h>
//@}

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* _HPL_GPIO_H_INCLUDED */
