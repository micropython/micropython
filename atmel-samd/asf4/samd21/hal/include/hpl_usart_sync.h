/**
 * \file
 *
 * \brief USART related functionality declaration.
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
 *
 */

#ifndef _HPL_SYNC_USART_H_INCLUDED
#define _HPL_SYNC_USART_H_INCLUDED

/**
 * \addtogroup HPL USART SYNC
 *
 * \section hpl_usart_sync_rev Revision History
 * - v1.0.0 Initial Release
 *
 *@{
 */

#include <hpl_usart.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief USART descriptor device structure
 */
struct _usart_sync_device {
	void *hw;
};

/**
 * \name HPL functions
 */
//@{
/**
 * \brief Initialize synchronous USART
 *
 * This function does low level USART configuration.
 *
 * \param[in] device The pointer to USART device instance
 * \param[in] hw The pointer to hardware instance
 *
 * \return Initialization status
 */
int32_t _usart_sync_init(struct _usart_sync_device *const device, void *const hw);

/**
 * \brief Deinitialize USART
 *
 * This function closes the given USART by disabling its clock.
 *
 * \param[in] device The pointer to USART device instance
 */
void _usart_sync_deinit(struct _usart_sync_device *const device);

/**
 * \brief Enable usart module
 *
 * This function will enable the usart module
 *
 * \param[in] device The pointer to USART device instance
 */
void _usart_sync_enable(struct _usart_sync_device *const device);

/**
 * \brief Disable usart module
 *
 * This function will disable the usart module
 *
 * \param[in] device The pointer to USART device instance
 */
void _usart_sync_disable(struct _usart_sync_device *const device);

/**
 * \brief Calculate baud rate register value
 *
 * \param[in] baud Required baud rate
 * \param[in] clock_rate clock frequency
 * \param[in] samples The number of samples
 * \param[in] mode USART mode
 * \param[in] fraction A fraction value
 *
 * \return Calculated baud rate register value
 */
uint16_t _usart_sync_calculate_baud_rate(const uint32_t baud, const uint32_t clock_rate, const uint8_t samples,
                                         const enum usart_baud_rate_mode mode, const uint8_t fraction);

/**
 * \brief Set baud rate
 *
 * \param[in] device The pointer to USART device instance
 * \param[in] baud_rate A baud rate to set
 */
void _usart_sync_set_baud_rate(struct _usart_sync_device *const device, const uint32_t baud_rate);

/**
 * \brief Set data order
 *
 * \param[in] device The pointer to USART device instance
 * \param[in] order A data order to set
 */
void _usart_sync_set_data_order(struct _usart_sync_device *const device, const enum usart_data_order order);

/**
 * \brief Set mode
 *
 * \param[in] device The pointer to USART device instance
 * \param[in] mode A mode to set
 */
void _usart_sync_set_mode(struct _usart_sync_device *const device, const enum usart_mode mode);

/**
 * \brief Set parity
 *
 * \param[in] device The pointer to USART device instance
 * \param[in] parity A parity to set
 */
void _usart_sync_set_parity(struct _usart_sync_device *const device, const enum usart_parity parity);

/**
 * \brief Set stop bits mode
 *
 * \param[in] device The pointer to USART device instance
 * \param[in] stop_bits A stop bits mode to set
 */
void _usart_sync_set_stop_bits(struct _usart_sync_device *const device, const enum usart_stop_bits stop_bits);

/**
 * \brief Set character size
 *
 * \param[in] device The pointer to USART device instance
 * \param[in] size A character size to set
 */
void _usart_sync_set_character_size(struct _usart_sync_device *const device, const enum usart_character_size size);

/**
 * \brief Retrieve usart status
 *
 * \param[in] device The pointer to USART device instance
 */
uint32_t _usart_sync_get_status(const struct _usart_sync_device *const device);

/**
 * \brief Write a byte to the given USART instance
 *
 * \param[in] device The pointer to USART device instance
 * \param[in] data Data to write
 */
void _usart_sync_write_byte(struct _usart_sync_device *const device, uint8_t data);

/**
 * \brief Read a byte from the given USART instance
 *
 * \param[in] device The pointer to USART device instance
 * \param[in] data Data to write
 *
 * \return Data received via USART interface.
 */
uint8_t _usart_sync_read_byte(const struct _usart_sync_device *const device);

/**
 * \brief Check if USART is ready to send next byte
 *
 * \param[in] device The pointer to USART device instance
 *
 * \return Status of the ready check.
 * \retval true if the USART is ready to send next byte
 * \retval false if the USART is not ready to send next byte
 */
bool _usart_sync_is_byte_sent(const struct _usart_sync_device *const device);

/**
 * \brief Check if there is data received by USART
 *
 * \param[in] device The pointer to USART device instance
 *
 * \return Status of the data received check.
 * \retval true if the USART has received a byte
 * \retval false if the USART has not received a byte
 */
bool _usart_sync_is_byte_received(const struct _usart_sync_device *const device);

/**
 * \brief Set the state of flow control pins
 *
 * \param[in] device The pointer to USART device instance
 * \param[in] state - A state of flow control pins to set
 */
void _usart_sync_set_flow_control_state(struct _usart_sync_device *const     device,
                                        const union usart_flow_control_state state);

/**
 * \brief Retrieve the state of flow control pins
 *
 * This function retrieves the of flow control pins.
 *
 * \return USART_FLOW_CONTROL_STATE_UNAVAILABLE.
 */
union usart_flow_control_state _usart_sync_get_flow_control_state(const struct _usart_sync_device *const device);

/**
 * \brief Retrieve ordinal number of the given USART hardware instance
 *
 * \param[in] device The pointer to USART device instance
 *
 * \return The ordinal number of the given USART hardware instance
 */
uint8_t _usart_sync_get_hardware_index(const struct _usart_sync_device *const device);
//@}

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* _HPL_SYNC_USART_H_INCLUDED */
