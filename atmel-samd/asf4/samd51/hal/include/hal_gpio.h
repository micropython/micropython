/**
 * \file
 *
 * \brief Port
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
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
 */
#ifndef _HAL_GPIO_INCLUDED_
#define _HAL_GPIO_INCLUDED_

#include <hpl_gpio.h>
#include <utils_assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Set gpio pull mode
 *
 * Set pin pull mode, non existing pull modes throws an fatal assert
 *
 * \param[in] pin       The pin number for device
 * \param[in] pull_mode GPIO_PULL_DOWN = Pull pin high with internal
 *                      resistor
 *                      GPIO_PULL_UP   = Pull pin low with internal resistor
 *                      GPIO_PULL_OFF  = Disable pin pull mode
 */
static inline void gpio_set_pin_pull_mode(const uint8_t pin, const enum gpio_pull_mode pull_mode)
{
	_gpio_set_pin_pull_mode((enum gpio_port)GPIO_PORT(pin), pin & 0x1F, pull_mode);
}

/**
 * \brief Set pin function
 *
 * Select which function a pin will be used for
 *
 * \param[in] pin       The pin number for device
 * \param[in] function  The pin function is given by a 32-bit wide bitfield
 *                      found in the header files for the device
 *
 */
static inline void gpio_set_pin_function(const uint32_t pin, uint32_t function)
{
	_gpio_set_pin_function(pin, function);
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] port      Ports are grouped into groups of maximum 32 pins,
 *                      GPIO_PORTA = group 0, GPIO_PORTB = group 1, etc
 * \param[in] mask      Bit mask where 1 means apply direction setting to the
 *                      corresponding pin
 * \param[in] direction GPIO_DIRECTION_IN  = Data direction in
 *                      GPIO_DIRECTION_OUT = Data direction out
 *                      GPIO_DIRECTION_OFF = Disables the pin
 *                      (low power state)
 */
static inline void gpio_set_port_direction(const enum gpio_port port, const uint32_t mask,
                                           const enum gpio_direction direction)
{
	_gpio_set_direction(port, mask, direction);
}

/**
 * \brief Set gpio data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] pin       The pin number for device
 * \param[in] direction GPIO_DIRECTION_IN  = Data direction in
 *                      GPIO_DIRECTION_OUT = Data direction out
 *                      GPIO_DIRECTION_OFF = Disables the pin
 *                      (low power state)
 */
static inline void gpio_set_pin_direction(const uint8_t pin, const enum gpio_direction direction)
{
	_gpio_set_direction((enum gpio_port)GPIO_PORT(pin), 1U << GPIO_PIN(pin), direction);
}

/**
 * \brief Set port level
 *
 * Sets output level on the pins defined by the bit mask
 *
 * \param[in] port  Ports are grouped into groups of maximum 32 pins,
 *                  GPIO_PORTA = group 0, GPIO_PORTB = group 1, etc
 * \param[in] mask  Bit mask where 1 means apply port level to the corresponding
 *                  pin
 * \param[in] level true  = Pin levels set to "high" state
 *                  false = Pin levels set to "low" state
 */
static inline void gpio_set_port_level(const enum gpio_port port, const uint32_t mask, const bool level)
{
	_gpio_set_level(port, mask, level);
}

/**
 * \brief Set gpio level
 *
 * Sets output level on a pin
 *
 * \param[in] pin       The pin number for device
 * \param[in] level true  = Pin level set to "high" state
 *                  false = Pin level set to "low" state
 */
static inline void gpio_set_pin_level(const uint8_t pin, const bool level)
{
	_gpio_set_level((enum gpio_port)GPIO_PORT(pin), 1U << GPIO_PIN(pin), level);
}

/**
 * \brief Toggle out level on pins
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] port  Ports are grouped into groups of maximum 32 pins,
 *                  GPIO_PORTA = group 0, GPIO_PORTB = group 1, etc
 * \param[in] mask  Bit mask where 1 means toggle pin level to the corresponding
 *                  pin
 */
static inline void gpio_toggle_port_level(const enum gpio_port port, const uint32_t mask)
{
	_gpio_toggle_level(port, mask);
}

/**
 * \brief Toggle output level on pin
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] pin       The pin number for device
 */
static inline void gpio_toggle_pin_level(const uint8_t pin)
{
	_gpio_toggle_level((enum gpio_port)GPIO_PORT(pin), 1U << GPIO_PIN(pin));
}

/**
 * \brief Get input level on pins
 *
 * Read the input level on pins connected to a port
 *
 * \param[in] port  Ports are grouped into groups of maximum 32 pins,
 *                  GPIO_PORTA = group 0, GPIO_PORTB = group 1, etc
 */
static inline uint32_t gpio_get_port_level(const enum gpio_port port)
{
	return _gpio_get_level(port);
}

/**
 * \brief Get level on pin
 *
 * Reads the level on pins connected to a port
 *
 * \param[in] pin       The pin number for device
 */
static inline bool gpio_get_pin_level(const uint8_t pin)
{
	return (bool)(_gpio_get_level((enum gpio_port)GPIO_PORT(pin)) & (0x01U << GPIO_PIN(pin)));
}
/**
 * \brief Get current driver version
 */
uint32_t gpio_get_version(void);

#ifdef __cplusplus
}
#endif

#endif
