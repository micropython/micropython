
/**
 * \file
 *
 * \brief SAM Serial Communication Interface
 *
 * Copyright (C) 2014-2017 Atmel Corporation. All rights reserved.
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
#include <hpl_dma.h>
#include <hpl_i2c_m_async.h>
#include <hpl_i2c_m_sync.h>
#include <hpl_i2c_s_async.h>
#include <hpl_sercom_config.h>
#include <hpl_spi_m_async.h>
#include <hpl_spi_m_sync.h>
#include <hpl_spi_s_async.h>
#include <hpl_spi_s_sync.h>
#include <hpl_usart_async.h>
#include <hpl_usart_sync.h>
#include <utils.h>
#include <utils_assert.h>

#ifndef CONF_SERCOM_0_USART_ENABLE
#define CONF_SERCOM_0_USART_ENABLE 0
#endif
#ifndef CONF_SERCOM_1_USART_ENABLE
#define CONF_SERCOM_1_USART_ENABLE 0
#endif
#ifndef CONF_SERCOM_2_USART_ENABLE
#define CONF_SERCOM_2_USART_ENABLE 0
#endif
#ifndef CONF_SERCOM_3_USART_ENABLE
#define CONF_SERCOM_3_USART_ENABLE 0
#endif
#ifndef CONF_SERCOM_4_USART_ENABLE
#define CONF_SERCOM_4_USART_ENABLE 0
#endif
#ifndef CONF_SERCOM_5_USART_ENABLE
#define CONF_SERCOM_5_USART_ENABLE 0
#endif
#ifndef CONF_SERCOM_6_USART_ENABLE
#define CONF_SERCOM_6_USART_ENABLE 0
#endif
#ifndef CONF_SERCOM_7_USART_ENABLE
#define CONF_SERCOM_7_USART_ENABLE 0
#endif

/** Amount of SERCOM that is used as USART. */
#define SERCOM_USART_AMOUNT                                                                                            \
	(CONF_SERCOM_0_USART_ENABLE + CONF_SERCOM_1_USART_ENABLE + CONF_SERCOM_2_USART_ENABLE + CONF_SERCOM_3_USART_ENABLE \
	 + CONF_SERCOM_4_USART_ENABLE                                                                                      \
	 + CONF_SERCOM_5_USART_ENABLE                                                                                      \
	 + CONF_SERCOM_6_USART_ENABLE                                                                                      \
	 + CONF_SERCOM_7_USART_ENABLE)

/**
 * \brief Macro is used to fill usart configuration structure based on
 * its number
 *
 * \param[in] n The number of structures
 */
#define SERCOM_CONFIGURATION(n)                                                                                        \
	{                                                                                                                  \
		n, SERCOM_USART_CTRLA_MODE(CONF_SERCOM_##n##_USART_MODE)                                                       \
		       | (CONF_SERCOM_##n##_USART_RUNSTDBY << SERCOM_USART_CTRLA_RUNSTDBY_Pos)                                 \
		       | (CONF_SERCOM_##n##_USART_IBON << SERCOM_USART_CTRLA_IBON_Pos)                                         \
		       | SERCOM_USART_CTRLA_SAMPR(CONF_SERCOM_##n##_USART_SAMPR)                                               \
		       | SERCOM_USART_CTRLA_TXPO(CONF_SERCOM_##n##_USART_TXPO)                                                 \
		       | SERCOM_USART_CTRLA_RXPO(CONF_SERCOM_##n##_USART_RXPO)                                                 \
		       | SERCOM_USART_CTRLA_SAMPA(CONF_SERCOM_##n##_USART_SAMPA)                                               \
		       | SERCOM_USART_CTRLA_FORM(CONF_SERCOM_##n##_USART_FORM)                                                 \
		       | (CONF_SERCOM_##n##_USART_CMODE << SERCOM_USART_CTRLA_CMODE_Pos)                                       \
		       | (CONF_SERCOM_##n##_USART_CPOL << SERCOM_USART_CTRLA_CPOL_Pos)                                         \
		       | (CONF_SERCOM_##n##_USART_DORD << SERCOM_USART_CTRLA_DORD_Pos),                                        \
		    SERCOM_USART_CTRLB_CHSIZE(CONF_SERCOM_##n##_USART_CHSIZE)                                                  \
		        | (CONF_SERCOM_##n##_USART_SBMODE << SERCOM_USART_CTRLB_SBMODE_Pos)                                    \
		        | (CONF_SERCOM_##n##_USART_CLODEN << SERCOM_USART_CTRLB_COLDEN_Pos)                                    \
		        | (CONF_SERCOM_##n##_USART_SFDE << SERCOM_USART_CTRLB_SFDE_Pos)                                        \
		        | (CONF_SERCOM_##n##_USART_ENC << SERCOM_USART_CTRLB_ENC_Pos)                                          \
		        | (CONF_SERCOM_##n##_USART_PMODE << SERCOM_USART_CTRLB_PMODE_Pos)                                      \
		        | (CONF_SERCOM_##n##_USART_TXEN << SERCOM_USART_CTRLB_TXEN_Pos)                                        \
		        | (CONF_SERCOM_##n##_USART_RXEN << SERCOM_USART_CTRLB_RXEN_Pos),                                       \
		    (uint16_t)(CONF_SERCOM_##n##_USART_BAUD_RATE), CONF_SERCOM_##n##_USART_FRACTIONAL,                         \
		    CONF_SERCOM_##n##_USART_RECEIVE_PULSE_LENGTH, CONF_SERCOM_##n##_USART_DEBUG_STOP_MODE,                     \
	}

/**
 * \brief SERCOM USART configuration type
 */
struct usart_configuration {
	uint8_t                       number;
	hri_sercomusart_ctrla_reg_t   ctrl_a;
	hri_sercomusart_ctrlb_reg_t   ctrl_b;
	hri_sercomusart_baud_reg_t    baud;
	uint8_t                       fractional;
	hri_sercomusart_rxpl_reg_t    rxpl;
	hri_sercomusart_dbgctrl_reg_t debug_ctrl;
};

#if SERCOM_USART_AMOUNT < 1
/** Dummy array to pass compiling. */
static struct usart_configuration _usarts[1] = {{0}};
#else
/**
 * \brief Array of SERCOM USART configurations
 */
static struct usart_configuration _usarts[] = {
#if CONF_SERCOM_0_USART_ENABLE == 1
    SERCOM_CONFIGURATION(0),
#endif
#if CONF_SERCOM_1_USART_ENABLE == 1
    SERCOM_CONFIGURATION(1),
#endif
#if CONF_SERCOM_2_USART_ENABLE == 1
    SERCOM_CONFIGURATION(2),
#endif
#if CONF_SERCOM_3_USART_ENABLE == 1
    SERCOM_CONFIGURATION(3),
#endif
#if CONF_SERCOM_4_USART_ENABLE == 1
    SERCOM_CONFIGURATION(4),
#endif
#if CONF_SERCOM_5_USART_ENABLE == 1
    SERCOM_CONFIGURATION(5),
#endif
#if CONF_SERCOM_6_USART_ENABLE == 1
    SERCOM_CONFIGURATION(6),
#endif
#if CONF_SERCOM_7_USART_ENABLE == 1
    SERCOM_CONFIGURATION(7),
#endif
};
#endif

static uint8_t _get_sercom_index(const void *const hw);
static uint8_t _sercom_get_irq_num(const void *const hw);
static void _sercom_init_irq_param(const void *const hw, void *dev);
static uint8_t _sercom_get_hardware_index(const void *const hw);

static int32_t _usart_init(void *const hw);
static inline void _usart_deinit(void *const hw);
static uint16_t _usart_calculate_baud_rate(const uint32_t baud, const uint32_t clock_rate, const uint8_t samples,
                                           const enum usart_baud_rate_mode mode, const uint8_t fraction);
static void _usart_set_baud_rate(void *const hw, const uint32_t baud_rate);
static void _usart_set_data_order(void *const hw, const enum usart_data_order order);
static void _usart_set_mode(void *const hw, const enum usart_mode mode);
static void _usart_set_parity(void *const hw, const enum usart_parity parity);
static void _usart_set_stop_bits(void *const hw, const enum usart_stop_bits stop_bits);
static void _usart_set_character_size(void *const hw, const enum usart_character_size size);

/**
 * \brief Initialize synchronous SERCOM USART
 */
int32_t _usart_sync_init(struct _usart_sync_device *const device, void *const hw)
{
	ASSERT(device);

	device->hw = hw;

	return _usart_init(hw);
}

/**
 * \brief Initialize asynchronous SERCOM USART
 */
int32_t _usart_async_init(struct _usart_async_device *const device, void *const hw)
{
	int32_t init_status;

	ASSERT(device);

	init_status = _usart_init(hw);
	if (init_status) {
		return init_status;
	}
	device->hw = hw;
	_sercom_init_irq_param(hw, (void *)device);
	NVIC_DisableIRQ((IRQn_Type)_sercom_get_irq_num(hw));
	NVIC_ClearPendingIRQ((IRQn_Type)_sercom_get_irq_num(hw));
	NVIC_EnableIRQ((IRQn_Type)_sercom_get_irq_num(hw));

	return ERR_NONE;
}

/**
 * \brief De-initialize SERCOM USART
 */
void _usart_sync_deinit(struct _usart_sync_device *const device)
{
	_usart_deinit(device->hw);
}

/**
 * \brief De-initialize SERCOM USART
 */
void _usart_async_deinit(struct _usart_async_device *const device)
{
	NVIC_DisableIRQ((IRQn_Type)_sercom_get_irq_num(device->hw));
	_usart_deinit(device->hw);
}

/**
 * \brief Calculate baud rate register value
 */
uint16_t _usart_sync_calculate_baud_rate(const uint32_t baud, const uint32_t clock_rate, const uint8_t samples,
                                         const enum usart_baud_rate_mode mode, const uint8_t fraction)
{
	return _usart_calculate_baud_rate(baud, clock_rate, samples, mode, fraction);
}

/**
 * \brief Calculate baud rate register value
 */
uint16_t _usart_async_calculate_baud_rate(const uint32_t baud, const uint32_t clock_rate, const uint8_t samples,
                                          const enum usart_baud_rate_mode mode, const uint8_t fraction)
{
	return _usart_calculate_baud_rate(baud, clock_rate, samples, mode, fraction);
}

/**
 * \brief Enable SERCOM module
 */
void _usart_sync_enable(struct _usart_sync_device *const device)
{
	hri_sercomusart_set_CTRLA_ENABLE_bit(device->hw);
}

/**
 * \brief Enable SERCOM module
 */
void _usart_async_enable(struct _usart_async_device *const device)
{
	hri_sercomusart_set_CTRLA_ENABLE_bit(device->hw);
}

/**
 * \brief Disable SERCOM module
 */
void _usart_sync_disable(struct _usart_sync_device *const device)
{
	hri_sercomusart_clear_CTRLA_ENABLE_bit(device->hw);
}

/**
 * \brief Disable SERCOM module
 */
void _usart_async_disable(struct _usart_async_device *const device)
{
	hri_sercomusart_clear_CTRLA_ENABLE_bit(device->hw);
}

/**
 * \brief Set baud rate
 */
void _usart_sync_set_baud_rate(struct _usart_sync_device *const device, const uint32_t baud_rate)
{
	_usart_set_baud_rate(device->hw, baud_rate);
}

/**
 * \brief Set baud rate
 */
void _usart_async_set_baud_rate(struct _usart_async_device *const device, const uint32_t baud_rate)
{
	_usart_set_baud_rate(device->hw, baud_rate);
}

/**
 * \brief Set data order
 */
void _usart_sync_set_data_order(struct _usart_sync_device *const device, const enum usart_data_order order)
{
	_usart_set_data_order(device->hw, order);
}

/**
 * \brief Set data order
 */
void _usart_async_set_data_order(struct _usart_async_device *const device, const enum usart_data_order order)
{
	_usart_set_data_order(device->hw, order);
}

/**
 * \brief Set mode
 */
void _usart_sync_set_mode(struct _usart_sync_device *const device, const enum usart_mode mode)
{
	_usart_set_mode(device->hw, mode);
}

/**
 * \brief Set mode
 */
void _usart_async_set_mode(struct _usart_async_device *const device, const enum usart_mode mode)
{
	_usart_set_mode(device->hw, mode);
}

/**
 * \brief Set parity
 */
void _usart_sync_set_parity(struct _usart_sync_device *const device, const enum usart_parity parity)
{
	_usart_set_parity(device->hw, parity);
}

/**
 * \brief Set parity
 */
void _usart_async_set_parity(struct _usart_async_device *const device, const enum usart_parity parity)
{
	_usart_set_parity(device->hw, parity);
}

/**
 * \brief Set stop bits mode
 */
void _usart_sync_set_stop_bits(struct _usart_sync_device *const device, const enum usart_stop_bits stop_bits)
{
	_usart_set_stop_bits(device->hw, stop_bits);
}

/**
 * \brief Set stop bits mode
 */
void _usart_async_set_stop_bits(struct _usart_async_device *const device, const enum usart_stop_bits stop_bits)
{
	_usart_set_stop_bits(device->hw, stop_bits);
}

/**
 * \brief Set character size
 */
void _usart_sync_set_character_size(struct _usart_sync_device *const device, const enum usart_character_size size)
{
	_usart_set_character_size(device->hw, size);
}

/**
 * \brief Set character size
 */
void _usart_async_set_character_size(struct _usart_async_device *const device, const enum usart_character_size size)
{
	_usart_set_character_size(device->hw, size);
}

/**
 * \brief Retrieve SERCOM usart status
 */
uint32_t _usart_sync_get_status(const struct _usart_sync_device *const device)
{
	return hri_sercomusart_read_STATUS_reg(device->hw);
}

/**
 * \brief Retrieve SERCOM usart status
 */
uint32_t _usart_async_get_status(const struct _usart_async_device *const device)
{
	return hri_sercomusart_read_STATUS_reg(device->hw);
}

/**
 * \brief Write a byte to the given SERCOM USART instance
 */
void _usart_sync_write_byte(struct _usart_sync_device *const device, uint8_t data)
{
	hri_sercomusart_write_DATA_reg(device->hw, data);
}

/**
 * \brief Write a byte to the given SERCOM USART instance
 */
void _usart_async_write_byte(struct _usart_async_device *const device, uint8_t data)
{
	hri_sercomusart_write_DATA_reg(device->hw, data);
}

/**
 * \brief Read a byte from the given SERCOM USART instance
 */
uint8_t _usart_sync_read_byte(const struct _usart_sync_device *const device)
{
	return hri_sercomusart_read_DATA_reg(device->hw);
}

/**
 * \brief Check if USART is ready to send next byte
 */
bool _usart_sync_is_byte_sent(const struct _usart_sync_device *const device)
{
	return hri_sercomusart_get_interrupt_DRE_bit(device->hw);
}

/**
 * \brief Check if USART is ready to send next byte
 */
bool _usart_async_is_byte_sent(const struct _usart_async_device *const device)
{
	return hri_sercomusart_get_interrupt_DRE_bit(device->hw);
}

/**
 * \brief Check if there is data received by USART
 */
bool _usart_sync_is_byte_received(const struct _usart_sync_device *const device)
{
	return hri_sercomusart_get_interrupt_RXC_bit(device->hw);
}

/**
 * \brief Set the state of flow control pins
 */
void _usart_sync_set_flow_control_state(struct _usart_sync_device *const     device,
                                        const union usart_flow_control_state state)
{
	(void)device;
	(void)state;
}

/**
 * \brief Set the state of flow control pins
 */
void _usart_async_set_flow_control_state(struct _usart_async_device *const    device,
                                         const union usart_flow_control_state state)
{
	(void)device;
	(void)state;
}

/**
 * \brief Retrieve the state of flow control pins
 */
union usart_flow_control_state _usart_sync_get_flow_control_state(const struct _usart_sync_device *const device)
{
	(void)device;
	union usart_flow_control_state state;

	state.value           = 0;
	state.bit.unavailable = 1;
	return state;
}

/**
 * \brief Retrieve the state of flow control pins
 */
union usart_flow_control_state _usart_async_get_flow_control_state(const struct _usart_async_device *const device)
{
	(void)device;
	union usart_flow_control_state state;

	state.value           = 0;
	state.bit.unavailable = 1;
	return state;
}

/**
 * \brief Enable data register empty interrupt
 */
void _usart_async_enable_byte_sent_irq(struct _usart_async_device *const device)
{
	hri_sercomusart_set_INTEN_DRE_bit(device->hw);
}

/**
 * \brief Enable transmission complete interrupt
 */
void _usart_async_enable_tx_done_irq(struct _usart_async_device *const device)
{
	hri_sercomusart_set_INTEN_TXC_bit(device->hw);
}

/**
 * \brief Retrieve ordinal number of the given sercom hardware instance
 */
static uint8_t _sercom_get_hardware_index(const void *const hw)
{
#ifdef _UNIT_TEST_
	return ((uint32_t)hw - (uint32_t)SERCOM0) / sizeof(Sercom);
#endif

#if defined __SAML21E18B__ || defined __ATSAML21E18B__ || defined __SAML21G18B__ || defined __ATSAML21G18B__           \
    || defined __SAML21J18B__ || defined __ATSAML21J18B__ || defined __ATSAMR30G18A__ || defined __ATSAMR30E18A__
	if ((uint32_t)SERCOM5 == (uint32_t)hw) {
		return 5;
	}
#endif

	return ((uint32_t)hw - (uint32_t)SERCOM0) >> 10;
}

/**
 * \brief Retrieve ordinal number of the given SERCOM USART hardware instance
 */
uint8_t _usart_sync_get_hardware_index(const struct _usart_sync_device *const device)
{
	return _sercom_get_hardware_index(device->hw);
}

/**
 * \brief Retrieve ordinal number of the given SERCOM USART hardware instance
 */
uint8_t _usart_async_get_hardware_index(const struct _usart_async_device *const device)
{
	return _sercom_get_hardware_index(device->hw);
}

/**
 * \brief Enable/disable USART interrupt
 */
void _usart_async_set_irq_state(struct _usart_async_device *const device, const enum _usart_async_callback_type type,
                                const bool state)
{
	ASSERT(device);

	if (USART_ASYNC_BYTE_SENT == type || USART_ASYNC_TX_DONE == type) {
		hri_sercomusart_write_INTEN_DRE_bit(device->hw, state);
		hri_sercomusart_write_INTEN_TXC_bit(device->hw, state);
	} else if (USART_ASYNC_RX_DONE == type) {
		hri_sercomusart_write_INTEN_RXC_bit(device->hw, state);
	} else if (USART_ASYNC_ERROR == type) {
		hri_sercomusart_write_INTEN_ERROR_bit(device->hw, state);
	}
}

/**
 * \internal Retrieve ordinal number of the given sercom hardware instance
 *
 * \param[in] hw The pointer to hardware instance

 * \return The ordinal number of the given sercom hardware instance
 */
static uint8_t _get_sercom_index(const void *const hw)
{
	uint8_t sercom_offset = _sercom_get_hardware_index(hw);
	uint8_t i;

	for (i = 0; i < ARRAY_SIZE(_usarts); i++) {
		if (_usarts[i].number == sercom_offset) {
			return i;
		}
	}

	ASSERT(false);
	return 0;
}

/**
 * \brief Init irq param with the given sercom hardware instance
 */
static void _sercom_init_irq_param(const void *const hw, void *dev)
{
}

/**
 * \internal Initialize SERCOM USART
 *
 * \param[in] hw The pointer to hardware instance
 *
 * \return The status of initialization
 */
static int32_t _usart_init(void *const hw)
{
	uint8_t i = _get_sercom_index(hw);

	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_SWRST);
	if (hri_sercomusart_get_CTRLA_ENABLE_bit(hw)) {
		return ERR_DENIED;
	}
	hri_sercomusart_set_CTRLA_SWRST_bit(hw);
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_SWRST);

	hri_sercomusart_write_CTRLA_reg(hw, _usarts[i].ctrl_a);
	hri_sercomusart_write_CTRLB_reg(hw, _usarts[i].ctrl_b);
	if ((_usarts[i].ctrl_a & SERCOM_USART_CTRLA_SAMPR(0x1)) || (_usarts[i].ctrl_a & SERCOM_USART_CTRLA_SAMPR(0x3))) {
		((Sercom *)hw)->USART.BAUD.FRAC.BAUD = _usarts[i].baud;
		((Sercom *)hw)->USART.BAUD.FRAC.FP   = _usarts[i].fractional;
	} else {
		hri_sercomusart_write_BAUD_reg(hw, _usarts[i].baud);
	}

	hri_sercomusart_write_RXPL_reg(hw, _usarts[i].rxpl);
	hri_sercomusart_write_DBGCTRL_reg(hw, _usarts[i].debug_ctrl);

	return ERR_NONE;
}

/**
 * \internal De-initialize SERCOM USART
 *
 * \param[in] hw The pointer to hardware instance
 */
static inline void _usart_deinit(void *const hw)
{
	hri_sercomusart_clear_CTRLA_ENABLE_bit(hw);
	hri_sercomusart_set_CTRLA_SWRST_bit(hw);
}

/**
 * \internal Calculate baud rate register value
 *
 * \param[in] baud Required baud rate
 * \param[in] clock_rate SERCOM clock frequency
 * \param[in] samples The number of samples
 * \param[in] mode USART mode
 * \param[in] fraction A fraction value
 *
 * \return Calculated baud rate register value
 */
static uint16_t _usart_calculate_baud_rate(const uint32_t baud, const uint32_t clock_rate, const uint8_t samples,
                                           const enum usart_baud_rate_mode mode, const uint8_t fraction)
{
	if (USART_BAUDRATE_ASYNCH_ARITHMETIC == mode) {
		return 65536 - ((uint64_t)65536 * samples * baud) / clock_rate;
	}

	if (USART_BAUDRATE_ASYNCH_FRACTIONAL == mode) {
		return clock_rate / baud / samples + SERCOM_USART_BAUD_FRACFP_FP(fraction);
	}

	if (USART_BAUDRATE_SYNCH == mode) {
		return clock_rate / baud / 2 - 1;
	}

	return 0;
}

/**
 * \internal Set baud rate
 *
 * \param[in] device The pointer to USART device instance
 * \param[in] baud_rate A baud rate to set
 */
static void _usart_set_baud_rate(void *const hw, const uint32_t baud_rate)
{
	bool enabled = hri_sercomusart_get_CTRLA_ENABLE_bit(hw);

	hri_sercomusart_clear_CTRLA_ENABLE_bit(hw);

	CRITICAL_SECTION_ENTER()
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_ENABLE);
	hri_sercomusart_write_BAUD_reg(hw, baud_rate);
	CRITICAL_SECTION_LEAVE()

	hri_sercomusart_write_CTRLA_ENABLE_bit(hw, enabled);
}

/**
 * \internal Set data order
 *
 * \param[in] device The pointer to USART device instance
 * \param[in] order A data order to set
 */
static void _usart_set_data_order(void *const hw, const enum usart_data_order order)
{
	bool enabled = hri_sercomusart_get_CTRLA_ENABLE_bit(hw);

	hri_sercomusart_clear_CTRLA_ENABLE_bit(hw);

	CRITICAL_SECTION_ENTER()
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_ENABLE);
	hri_sercomusart_write_CTRLA_DORD_bit(hw, order);
	CRITICAL_SECTION_LEAVE()

	hri_sercomusart_write_CTRLA_ENABLE_bit(hw, enabled);
}

/**
 * \internal Set mode
 *
 * \param[in] device The pointer to USART device instance
 * \param[in] mode A mode to set
 */
static void _usart_set_mode(void *const hw, const enum usart_mode mode)
{
	bool enabled = hri_sercomusart_get_CTRLA_ENABLE_bit(hw);

	hri_sercomusart_clear_CTRLA_ENABLE_bit(hw);

	CRITICAL_SECTION_ENTER()
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_ENABLE);
	hri_sercomusart_write_CTRLA_CMODE_bit(hw, mode);
	CRITICAL_SECTION_LEAVE()

	hri_sercomusart_write_CTRLA_ENABLE_bit(hw, enabled);
}

/**
 * \internal Set parity
 *
 * \param[in] device The pointer to USART device instance
 * \param[in] parity A parity to set
 */
static void _usart_set_parity(void *const hw, const enum usart_parity parity)
{
	bool enabled = hri_sercomusart_get_CTRLA_ENABLE_bit(hw);

	hri_sercomusart_clear_CTRLA_ENABLE_bit(hw);

	CRITICAL_SECTION_ENTER()
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_ENABLE);

	if (USART_PARITY_NONE != parity) {
		hri_sercomusart_set_CTRLA_FORM_bf(hw, 1);
	} else {
		hri_sercomusart_clear_CTRLA_FORM_bf(hw, 1);
	}

	hri_sercomusart_write_CTRLB_PMODE_bit(hw, parity);
	CRITICAL_SECTION_LEAVE()

	hri_sercomusart_write_CTRLA_ENABLE_bit(hw, enabled);
}

/**
 * \internal Set stop bits mode
 *
 * \param[in] device The pointer to USART device instance
 * \param[in] stop_bits A stop bits mode to set
 */
static void _usart_set_stop_bits(void *const hw, const enum usart_stop_bits stop_bits)
{
	bool enabled = hri_sercomusart_get_CTRLA_ENABLE_bit(hw);

	hri_sercomusart_clear_CTRLA_ENABLE_bit(hw);

	CRITICAL_SECTION_ENTER()
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_ENABLE);
	hri_sercomusart_write_CTRLB_SBMODE_bit(hw, stop_bits);
	CRITICAL_SECTION_LEAVE()

	hri_sercomusart_write_CTRLA_ENABLE_bit(hw, enabled);
}

/**
 * \internal Set character size
 *
 * \param[in] device The pointer to USART device instance
 * \param[in] size A character size to set
 */
static void _usart_set_character_size(void *const hw, const enum usart_character_size size)
{
	bool enabled = hri_sercomusart_get_CTRLA_ENABLE_bit(hw);

	hri_sercomusart_clear_CTRLA_ENABLE_bit(hw);

	CRITICAL_SECTION_ENTER()
	hri_sercomusart_wait_for_sync(hw, SERCOM_USART_SYNCBUSY_ENABLE);
	hri_sercomusart_write_CTRLB_CHSIZE_bf(hw, size);
	CRITICAL_SECTION_LEAVE()

	if (enabled) {
		hri_sercomusart_set_CTRLA_ENABLE_bit(hw);
	}
}

/* Sercom I2C implementation */

#ifndef CONF_SERCOM_0_I2CM_ENABLE
#define CONF_SERCOM_0_I2CM_ENABLE 0
#endif
#ifndef CONF_SERCOM_1_I2CM_ENABLE
#define CONF_SERCOM_1_I2CM_ENABLE 0
#endif
#ifndef CONF_SERCOM_2_I2CM_ENABLE
#define CONF_SERCOM_2_I2CM_ENABLE 0
#endif
#ifndef CONF_SERCOM_3_I2CM_ENABLE
#define CONF_SERCOM_3_I2CM_ENABLE 0
#endif
#ifndef CONF_SERCOM_4_I2CM_ENABLE
#define CONF_SERCOM_4_I2CM_ENABLE 0
#endif
#ifndef CONF_SERCOM_5_I2CM_ENABLE
#define CONF_SERCOM_5_I2CM_ENABLE 0
#endif
#ifndef CONF_SERCOM_6_I2CM_ENABLE
#define CONF_SERCOM_6_I2CM_ENABLE 0
#endif
#ifndef CONF_SERCOM_7_I2CM_ENABLE
#define CONF_SERCOM_7_I2CM_ENABLE 0
#endif

/** Amount of SERCOM that is used as I2C Master. */
#define SERCOM_I2CM_AMOUNT                                                                                             \
	(CONF_SERCOM_0_I2CM_ENABLE + CONF_SERCOM_1_I2CM_ENABLE + CONF_SERCOM_2_I2CM_ENABLE + CONF_SERCOM_3_I2CM_ENABLE     \
	 + CONF_SERCOM_4_I2CM_ENABLE                                                                                       \
	 + CONF_SERCOM_5_I2CM_ENABLE                                                                                       \
	 + CONF_SERCOM_6_I2CM_ENABLE                                                                                       \
	 + CONF_SERCOM_7_I2CM_ENABLE)

/**
 * \brief Macro is used to fill i2cm configuration structure based on
 * its number
 *
 * \param[in] n The number of structures
 */
#define I2CM_CONFIGURATION(n)                                                                                          \
	{                                                                                                                  \
		(n), (SERCOM_I2CM_CTRLA_MODE_I2C_MASTER) | (CONF_SERCOM_##n##_I2CM_RUNSTDBY << SERCOM_I2CM_CTRLA_RUNSTDBY_Pos) \
		         | (CONF_SERCOM_##n##_I2CM_SPEED << SERCOM_I2CM_CTRLA_SPEED_Pos)                                       \
		         | (CONF_SERCOM_##n##_I2CM_MEXTTOEN << SERCOM_I2CM_CTRLA_MEXTTOEN_Pos)                                 \
		         | (CONF_SERCOM_##n##_I2CM_SEXTTOEN << SERCOM_I2CM_CTRLA_SEXTTOEN_Pos)                                 \
		         | (CONF_SERCOM_##n##_I2CM_INACTOUT << SERCOM_I2CM_CTRLA_INACTOUT_Pos)                                 \
		         | (CONF_SERCOM_##n##_I2CM_LOWTOUT << SERCOM_I2CM_CTRLA_LOWTOUTEN_Pos)                                 \
		         | (CONF_SERCOM_##n##_I2CM_SDAHOLD << SERCOM_I2CM_CTRLA_SDAHOLD_Pos),                                  \
		    SERCOM_I2CM_CTRLB_SMEN, (uint32_t)(CONF_SERCOM_##n##_I2CM_BAUD_RATE),                                      \
		    CONF_SERCOM_##n##_I2CM_DEBUG_STOP_MODE, CONF_SERCOM_##n##_I2CM_TRISE, CONF_GCLK_SERCOM##n##_CORE_FREQUENCY \
	}

#define ERROR_FLAG (1 << 7)
#define SB_FLAG (1 << 1)
#define MB_FLAG (1 << 0)

#define CMD_STOP 0x3
#define I2C_IDLE 0x1
#define I2C_SM 0x0
#define I2C_FM 0x1
#define I2C_HS 0x2
#define TEN_ADDR_FRAME 0x78
#define TEN_ADDR_MASK 0x3ff
#define SEVEN_ADDR_MASK 0x7f

/**
 * \brief SERCOM I2CM configuration type
 */
struct i2cm_configuration {
	uint8_t                      number;
	hri_sercomi2cm_ctrla_reg_t   ctrl_a;
	hri_sercomi2cm_ctrlb_reg_t   ctrl_b;
	hri_sercomi2cm_baud_reg_t    baud;
	hri_sercomi2cm_dbgctrl_reg_t dbgctrl;
	uint16_t                     trise;
	uint32_t                     clk; /* SERCOM peripheral clock frequency */
};

static inline void _i2c_m_enable_implementation(void *hw);
static int32_t _i2c_m_sync_init_impl(struct _i2c_m_service *const service, void *const hw);

#if SERCOM_I2CM_AMOUNT < 1
/** Dummy array to pass compiling. */
static struct i2cm_configuration _i2cms[1] = {{0}};
#else
/**
 * \brief Array of SERCOM I2CM configurations
 */
static struct i2cm_configuration _i2cms[] = {
#if CONF_SERCOM_0_I2CM_ENABLE == 1
    I2CM_CONFIGURATION(0),
#endif
#if CONF_SERCOM_1_I2CM_ENABLE == 1
    I2CM_CONFIGURATION(1),
#endif
#if CONF_SERCOM_2_I2CM_ENABLE == 1
    I2CM_CONFIGURATION(2),
#endif
#if CONF_SERCOM_3_I2CM_ENABLE == 1
    I2CM_CONFIGURATION(3),
#endif
#if CONF_SERCOM_4_I2CM_ENABLE == 1
    I2CM_CONFIGURATION(4),
#endif
#if CONF_SERCOM_5_I2CM_ENABLE == 1
    I2CM_CONFIGURATION(5),
#endif
#if CONF_SERCOM_6_I2CM_ENABLE == 1
    I2CM_CONFIGURATION(6),
#endif
#if CONF_SERCOM_7_I2CM_ENABLE == 1
    I2CM_CONFIGURATION(7),
#endif
};
#endif

/**
 * \internal Retrieve ordinal number of the given sercom hardware instance
 *
 * \param[in] hw The pointer to hardware instance

 * \return The ordinal number of the given sercom hardware instance
 */
static int8_t _get_i2cm_index(const void *const hw)
{
	uint8_t sercom_offset = _sercom_get_hardware_index(hw);
	uint8_t i;

	for (i = 0; i < ARRAY_SIZE(_i2cms); i++) {
		if (_i2cms[i].number == sercom_offset) {
			return i;
		}
	}

	ASSERT(false);
	return -1;
}

static inline void _sercom_i2c_send_stop(void *const hw)
{
	hri_sercomi2cm_set_CTRLB_CMD_bf(hw, CMD_STOP);
}

/**
 * \brief SERCOM I2CM analyze hardware status and transfer next byte
 */
static inline int32_t _sercom_i2c_sync_analyse_flags(void *const hw, uint32_t flags, struct _i2c_m_msg *const msg)
{
	int      sclsm  = hri_sercomi2cm_get_CTRLA_SCLSM_bit(hw);
	uint16_t status = hri_sercomi2cm_read_STATUS_reg(hw);

	if (flags & MB_FLAG) {
		/* tx error */
		if (status & SERCOM_I2CM_STATUS_ARBLOST) {
			hri_sercomi2cm_clear_interrupt_MB_bit(hw);
			msg->flags |= I2C_M_FAIL;
			msg->flags &= ~I2C_M_BUSY;

			if (status & SERCOM_I2CM_STATUS_BUSERR) {
				return I2C_ERR_BUS;
			}

			return I2C_ERR_BAD_ADDRESS;
		} else {
			if (status & SERCOM_I2CM_STATUS_RXNACK) {

				/* Slave rejects to receive more data */
				if (msg->len > 0) {
					msg->flags |= I2C_M_FAIL;
				}

				if (msg->flags & I2C_M_STOP) {
					_sercom_i2c_send_stop(hw);
				}

				msg->flags &= ~I2C_M_BUSY;

				return I2C_NACK;
			}

			if (msg->flags & I2C_M_TEN) {
				hri_sercomi2cm_write_ADDR_reg(hw,
				                              ((((msg->addr & TEN_ADDR_MASK) >> 8) | TEN_ADDR_FRAME) << 1) | I2C_M_RD
				                                  | (hri_sercomi2cm_read_ADDR_reg(hw) & SERCOM_I2CM_ADDR_HS));
				msg->flags &= ~I2C_M_TEN;

				return I2C_OK;
			}

			if (msg->len == 0) {
				if (msg->flags & I2C_M_STOP) {
					_sercom_i2c_send_stop(hw);
				}

				msg->flags &= ~I2C_M_BUSY;
			} else {
				hri_sercomi2cm_write_DATA_reg(hw, *msg->buffer);
				msg->buffer++;
				msg->len--;
			}

			return I2C_OK;
		}
	} else if (flags & SB_FLAG) {
		if ((msg->len) && !(status & SERCOM_I2CM_STATUS_RXNACK)) {
			msg->len--;

			/* last byte, send nack */
			if ((msg->len == 0 && !sclsm) || (msg->len == 1 && sclsm)) {
				hri_sercomi2cm_set_CTRLB_ACKACT_bit(hw);
			}

			if (msg->len == 0) {
				if (msg->flags & I2C_M_STOP) {
					_sercom_i2c_send_stop(hw);
				}

				msg->flags &= ~I2C_M_BUSY;
			}

			/* Accessing DATA.DATA auto-triggers I2C bus operations.
			 * The operation performed depends on the state of
			 * CTRLB.ACKACT, CTRLB.SMEN
			 **/
			*msg->buffer++ = hri_sercomi2cm_read_DATA_reg(hw);
		} else {
			hri_sercomi2cm_clear_interrupt_SB_bit(hw);
			return I2C_NACK;
		}

		hri_sercomi2cm_clear_interrupt_SB_bit(hw);
	}

	return I2C_OK;
}

/**
 * \brief Enable the i2c master module
 *
 * \param[in] i2c_dev The pointer to i2c device
 */
int32_t _i2c_m_async_enable(struct _i2c_m_async_device *const i2c_dev)
{
	ASSERT(i2c_dev);

	_i2c_m_enable_implementation(i2c_dev->hw);

	return ERR_NONE;
}

/**
 * \brief Disable the i2c master module
 *
 * \param[in] i2c_dev The pointer to i2c device
 */
int32_t _i2c_m_async_disable(struct _i2c_m_async_device *const i2c_dev)
{
	void *hw = i2c_dev->hw;

	ASSERT(i2c_dev);
	ASSERT(i2c_dev->hw);

	NVIC_DisableIRQ((IRQn_Type)_sercom_get_irq_num(hw));
	hri_sercomi2cm_clear_CTRLA_ENABLE_bit(hw);

	return ERR_NONE;
}

/**
 * \brief Set baudrate of master
 *
 * \param[in] i2c_dev The pointer to i2c device
 * \param[in] clkrate The clock rate of i2c master, in KHz
 * \param[in] baudrate The baud rate desired for i2c master, in KHz
 */
int32_t _i2c_m_async_set_baudrate(struct _i2c_m_async_device *const i2c_dev, uint32_t clkrate, uint32_t baudrate)
{
	uint32_t tmp;
	void *   hw = i2c_dev->hw;

	if (hri_sercomi2cm_get_CTRLA_ENABLE_bit(hw)) {
		return ERR_DENIED;
	}

	tmp     = _get_i2cm_index(hw);
	clkrate = _i2cms[tmp].clk / 1000;

	if (i2c_dev->service.mode == I2C_STANDARD_MODE) {
		tmp = (uint32_t)((clkrate - 10 * baudrate - baudrate * clkrate * (i2c_dev->service.trise * 0.000000001))
		                 / (2 * baudrate));
		hri_sercomi2cm_write_BAUD_BAUD_bf(hw, tmp);
	} else if (i2c_dev->service.mode == I2C_FASTMODE) {
		tmp = (uint32_t)((clkrate - 10 * baudrate - baudrate * clkrate * (i2c_dev->service.trise * 0.000000001))
		                 / (2 * baudrate));
		hri_sercomi2cm_write_BAUD_BAUD_bf(hw, tmp);
	} else if (i2c_dev->service.mode == I2C_HIGHSPEED_MODE) {
		tmp = (clkrate - 2 * baudrate) / (2 * baudrate);
		hri_sercomi2cm_write_BAUD_HSBAUD_bf(hw, tmp);
	} else {
		/* error baudrate */
		return ERR_INVALID_ARG;
	}

	return ERR_NONE;
}

/**
 * \brief Retrieve IRQ number for the given hardware instance
 */
static uint8_t _sercom_get_irq_num(const void *const hw)
{
	return SERCOM0_IRQn + _sercom_get_hardware_index(hw);
}

/**
 * \brief Initialize sercom i2c module to use in async mode
 *
 * \param[in] i2c_dev The pointer to i2c device
 */
int32_t _i2c_m_async_init(struct _i2c_m_async_device *const i2c_dev, void *const hw)
{
	int32_t init_status;

	ASSERT(i2c_dev);

	i2c_dev->hw = hw;

	init_status = _i2c_m_sync_init_impl(&i2c_dev->service, hw);
	if (init_status) {
		return init_status;
	}

	_sercom_init_irq_param(hw, (void *)i2c_dev);
	NVIC_DisableIRQ((IRQn_Type)_sercom_get_irq_num(hw));
	NVIC_ClearPendingIRQ((IRQn_Type)_sercom_get_irq_num(hw));
	NVIC_EnableIRQ((IRQn_Type)_sercom_get_irq_num(hw));

	return ERR_NONE;
}

/**
 * \brief Deinitialize sercom i2c module
 *
 * \param[in] i2c_dev The pointer to i2c device
 */
int32_t _i2c_m_async_deinit(struct _i2c_m_async_device *const i2c_dev)
{
	ASSERT(i2c_dev);

	hri_sercomi2cm_clear_CTRLA_ENABLE_bit(i2c_dev->hw);
	hri_sercomi2cm_set_CTRLA_SWRST_bit(i2c_dev->hw);

	return ERR_NONE;
}

/**
 * \brief Transfer the slave address to bus, which will start the transfer
 *
 * \param[in] i2c_dev The pointer to i2c device
 */
static int32_t _sercom_i2c_send_address(struct _i2c_m_async_device *const i2c_dev)
{
	void *             hw    = i2c_dev->hw;
	struct _i2c_m_msg *msg   = &i2c_dev->service.msg;
	int                sclsm = hri_sercomi2cm_get_CTRLA_SCLSM_bit(hw);

	ASSERT(i2c_dev);

	if (msg->len == 1 && sclsm) {
		hri_sercomi2cm_set_CTRLB_ACKACT_bit(hw);
	} else {
		hri_sercomi2cm_clear_CTRLB_ACKACT_bit(hw);
	}

	/* ten bit address */
	if (msg->addr & I2C_M_TEN) {
		if (msg->flags & I2C_M_RD) {
			msg->flags |= I2C_M_TEN;
		}

		hri_sercomi2cm_write_ADDR_reg(hw,
		                              ((msg->addr & TEN_ADDR_MASK) << 1) | SERCOM_I2CM_ADDR_TENBITEN
		                                  | (hri_sercomi2cm_read_ADDR_reg(hw) & SERCOM_I2CM_ADDR_HS));
	} else {
		hri_sercomi2cm_write_ADDR_reg(hw,
		                              ((msg->addr & SEVEN_ADDR_MASK) << 1) | (msg->flags & I2C_M_RD ? I2C_M_RD : 0x0)
		                                  | (hri_sercomi2cm_read_ADDR_reg(hw) & SERCOM_I2CM_ADDR_HS));
	}

	return ERR_NONE;
}

/**
 * \brief Transfer data specified by msg
 *
 * \param[in] i2c_dev The pointer to i2c device
 * \param[in] msg The pointer to i2c message
 *
 * \return Transfer status.
 * \retval 0 Transfer success
 * \retval <0 Transfer fail, return the error code
 */
int32_t _i2c_m_async_transfer(struct _i2c_m_async_device *i2c_dev, struct _i2c_m_msg *msg)
{
	int ret;

	ASSERT(i2c_dev);
	ASSERT(i2c_dev->hw);
	ASSERT(msg);

	if (msg->len == 0) {
		return ERR_NONE;
	}

	if (i2c_dev->service.msg.flags & I2C_M_BUSY) {
		return ERR_BUSY;
	}

	msg->flags |= I2C_M_BUSY;
	i2c_dev->service.msg = *msg;

	ret = _sercom_i2c_send_address(i2c_dev);

	if (ret) {
		i2c_dev->service.msg.flags &= ~I2C_M_BUSY;

		return ret;
	}

	return ERR_NONE;
}

/**
 * \brief Set callback to be called in interrupt handler
 *
 * \param[in] i2c_dev The pointer to master i2c device
 * \param[in] type The callback type
 * \param[in] func The callback function pointer
 */
int32_t _i2c_m_async_register_callback(struct _i2c_m_async_device *const i2c_dev, enum _i2c_m_async_callback_type type,
                                       FUNC_PTR func)
{
	switch (type) {
	case I2C_M_ASYNC_DEVICE_ERROR:
		i2c_dev->cb.error = (_i2c_error_cb_t)func;
		break;
	case I2C_M_ASYNC_DEVICE_TX_COMPLETE:
		i2c_dev->cb.tx_complete = (_i2c_complete_cb_t)func;
		break;
	case I2C_M_ASYNC_DEVICE_RX_COMPLETE:
		i2c_dev->cb.rx_complete = (_i2c_complete_cb_t)func;
		break;
	default:
		/* error */
		break;
	}

	return ERR_NONE;
}

/**
 * \brief Set stop condition on I2C
 *
 * \param i2c_dev Pointer to master i2c device
 *
 * \return Operation status
 * \retval I2C_OK Operation was successfull
 */
int32_t _i2c_m_async_send_stop(struct _i2c_m_async_device *const i2c_dev)
{
	void *hw = i2c_dev->hw;

	_sercom_i2c_send_stop(hw);

	return I2C_OK;
}

/**
 * \brief Get number of bytes left in transfer buffer
 *
 * \param i2c_dev Pointer to i2c master device
 *
 * \return Bytes left in buffer
 * \retval =>0 Bytes left in buffer
 */
int32_t _i2c_m_async_get_bytes_left(struct _i2c_m_async_device *const i2c_dev)
{
	if (i2c_dev->service.msg.flags & I2C_M_BUSY) {
		return i2c_dev->service.msg.len;
	}

	return 0;
}

/**
 * \brief Initialize sercom i2c module to use in sync mode
 *
 * \param[in] i2c_dev The pointer to i2c device
 */
int32_t _i2c_m_sync_init(struct _i2c_m_sync_device *const i2c_dev, void *const hw)
{
	ASSERT(i2c_dev);

	i2c_dev->hw = hw;

	return _i2c_m_sync_init_impl(&i2c_dev->service, hw);
}

/**
 * \brief Deinitialize sercom i2c module
 *
 * \param[in] i2c_dev The pointer to i2c device
 */
int32_t _i2c_m_sync_deinit(struct _i2c_m_sync_device *const i2c_dev)
{
	ASSERT(i2c_dev);

	hri_sercomi2cm_clear_CTRLA_ENABLE_bit(i2c_dev->hw);
	hri_sercomi2cm_set_CTRLA_SWRST_bit(i2c_dev->hw);

	return ERR_NONE;
}

/**
 * \brief Enable the i2c master module
 *
 * \param[in] i2c_dev The pointer to i2c device
 */
int32_t _i2c_m_sync_enable(struct _i2c_m_sync_device *const i2c_dev)
{
	ASSERT(i2c_dev);

	_i2c_m_enable_implementation(i2c_dev->hw);

	return ERR_NONE;
}

/**
 * \brief Disable the i2c master module
 *
 * \param[in] i2c_dev The pointer to i2c device
 */
int32_t _i2c_m_sync_disable(struct _i2c_m_sync_device *const i2c_dev)
{
	void *hw = i2c_dev->hw;

	ASSERT(i2c_dev);
	ASSERT(i2c_dev->hw);

	hri_sercomi2cm_clear_CTRLA_ENABLE_bit(hw);

	return ERR_NONE;
}

/**
 * \brief Set baudrate of master
 *
 * \param[in] i2c_dev The pointer to i2c device
 * \param[in] clkrate The clock rate of i2c master, in KHz
 * \param[in] baudrate The baud rate desired for i2c master, in KHz
 */
int32_t _i2c_m_sync_set_baudrate(struct _i2c_m_sync_device *const i2c_dev, uint32_t clkrate, uint32_t baudrate)
{
	uint32_t tmp;
	void *   hw = i2c_dev->hw;

	if (hri_sercomi2cm_get_CTRLA_ENABLE_bit(hw)) {
		return ERR_DENIED;
	}

	tmp     = _get_i2cm_index(hw);
	clkrate = _i2cms[tmp].clk / 1000;

	if (i2c_dev->service.mode == I2C_STANDARD_MODE) {
		tmp = (uint32_t)((clkrate - 10 * baudrate - baudrate * clkrate * (i2c_dev->service.trise * 0.000000001))
		                 / (2 * baudrate));
		hri_sercomi2cm_write_BAUD_BAUD_bf(hw, tmp);
	} else if (i2c_dev->service.mode == I2C_FASTMODE) {
		tmp = (uint32_t)((clkrate - 10 * baudrate - baudrate * clkrate * (i2c_dev->service.trise * 0.000000001))
		                 / (2 * baudrate));
		hri_sercomi2cm_write_BAUD_BAUD_bf(hw, tmp);
	} else if (i2c_dev->service.mode == I2C_HIGHSPEED_MODE) {
		tmp = (clkrate - 2 * baudrate) / (2 * baudrate);
		hri_sercomi2cm_write_BAUD_HSBAUD_bf(hw, tmp);
	} else {
		/* error baudrate */
		return ERR_INVALID_ARG;
	}

	return ERR_NONE;
}

/**
 * \brief Enable/disable I2C master interrupt
 */
void _i2c_m_async_set_irq_state(struct _i2c_m_async_device *const device, const enum _i2c_m_async_callback_type type,
                                const bool state)
{
	if (I2C_M_ASYNC_DEVICE_TX_COMPLETE == type || I2C_M_ASYNC_DEVICE_RX_COMPLETE == type) {
		hri_sercomi2cm_write_INTEN_SB_bit(device->hw, state);
		hri_sercomi2cm_write_INTEN_MB_bit(device->hw, state);
	} else if (I2C_M_ASYNC_DEVICE_ERROR == type) {
		hri_sercomi2cm_write_INTEN_ERROR_bit(device->hw, state);
	}
}

/**
 * \brief Wait for bus response
 *
 * \param[in] i2c_dev The pointer to i2c device
 * \param[in] flags Store the hardware response
 *
 * \return Bus response status.
 * \retval 0 Bus response status OK
 * \retval <0 Bus response fail
 */
inline static int32_t _sercom_i2c_sync_wait_bus(struct _i2c_m_sync_device *const i2c_dev, uint32_t *flags)
{
	uint32_t timeout = 65535;
	void *   hw      = i2c_dev->hw;

	do {
		*flags = hri_sercomi2cm_read_INTFLAG_reg(hw);

		if (timeout-- == 0) {
			return I2C_ERR_BUS;
		}
	} while (!(*flags & MB_FLAG) && !(*flags & SB_FLAG));

	return I2C_OK;
}

/**
 * \brief Send the slave address to bus, which will start the transfer
 *
 * \param[in] i2c_dev The pointer to i2c device
 */
static int32_t _sercom_i2c_sync_send_address(struct _i2c_m_sync_device *const i2c_dev)
{
	void *             hw    = i2c_dev->hw;
	struct _i2c_m_msg *msg   = &i2c_dev->service.msg;
	int                sclsm = hri_sercomi2cm_get_CTRLA_SCLSM_bit(hw);
	uint32_t           flags;

	ASSERT(i2c_dev);

	if (msg->len == 1 && sclsm) {
		hri_sercomi2cm_set_CTRLB_ACKACT_bit(hw);
	} else {
		hri_sercomi2cm_clear_CTRLB_ACKACT_bit(hw);
	}

	/* ten bit address */
	if (msg->addr & I2C_M_TEN) {
		if (msg->flags & I2C_M_RD) {
			msg->flags |= I2C_M_TEN;
		}

		hri_sercomi2cm_write_ADDR_reg(hw,
		                              ((msg->addr & TEN_ADDR_MASK) << 1) | SERCOM_I2CM_ADDR_TENBITEN
		                                  | (hri_sercomi2cm_read_ADDR_reg(hw) & SERCOM_I2CM_ADDR_HS));
	} else {
		hri_sercomi2cm_write_ADDR_reg(hw,
		                              ((msg->addr & SEVEN_ADDR_MASK) << 1) | (msg->flags & I2C_M_RD ? I2C_M_RD : 0x0)
		                                  | (hri_sercomi2cm_read_ADDR_reg(hw) & SERCOM_I2CM_ADDR_HS));
	}

	_sercom_i2c_sync_wait_bus(i2c_dev, &flags);
	return _sercom_i2c_sync_analyse_flags(hw, flags, msg);
}

/**
 * \brief Transfer data specified by msg
 *
 * \param[in] i2c_dev The pointer to i2c device
 * \param[in] msg The pointer to i2c message
 *
 * \return Transfer status.
 * \retval 0 Transfer success
 * \retval <0 Transfer fail or partial fail, return the error code
 */
int32_t _i2c_m_sync_transfer(struct _i2c_m_sync_device *const i2c_dev, struct _i2c_m_msg *msg)
{
	uint32_t flags;
	int      ret;
	void *   hw = i2c_dev->hw;

	ASSERT(i2c_dev);
	ASSERT(i2c_dev->hw);
	ASSERT(msg);

	if (i2c_dev->service.msg.flags & I2C_M_BUSY) {
		return I2C_ERR_BUSY;
	}

	msg->flags |= I2C_M_BUSY;
	i2c_dev->service.msg = *msg;

	ret = _sercom_i2c_sync_send_address(i2c_dev);

	if (ret) {
		i2c_dev->service.msg.flags &= ~I2C_M_BUSY;

		return ret;
	}

	while (i2c_dev->service.msg.flags & I2C_M_BUSY) {
		ret = _sercom_i2c_sync_wait_bus(i2c_dev, &flags);

		if (ret) {
			if (msg->flags & I2C_M_STOP) {
				_sercom_i2c_send_stop(hw);
			}

			i2c_dev->service.msg.flags &= ~I2C_M_BUSY;

			return ret;
		}

		ret = _sercom_i2c_sync_analyse_flags(hw, flags, &i2c_dev->service.msg);
	}

	return ret;
}

int32_t _i2c_m_sync_send_stop(struct _i2c_m_sync_device *const i2c_dev)
{
	void *hw = i2c_dev->hw;

	_sercom_i2c_send_stop(hw);

	return I2C_OK;
}

static inline void _i2c_m_enable_implementation(void *const hw)
{
	int timeout = 65535;

	ASSERT(hw);

	/* Enable interrupts */
	hri_sercomi2cm_set_CTRLA_ENABLE_bit(hw);

	while (hri_sercomi2cm_read_STATUS_BUSSTATE_bf(hw) != I2C_IDLE) {
		timeout--;

		if (timeout <= 0) {
			hri_sercomi2cm_clear_STATUS_reg(hw, SERCOM_I2CM_STATUS_BUSSTATE(I2C_IDLE));
		}
	}
}

static int32_t _i2c_m_sync_init_impl(struct _i2c_m_service *const service, void *const hw)
{
	uint8_t i = _get_i2cm_index(hw);

	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SWRST);
	/* Check if module is enabled. */
	if (hri_sercomi2cm_get_CTRLA_ENABLE_bit(hw)) {
		return ERR_DENIED;
	}
	hri_sercomi2cm_set_CTRLA_SWRST_bit(hw);
	hri_sercomi2cm_wait_for_sync(hw, SERCOM_I2CM_SYNCBUSY_SWRST);

	hri_sercomi2cm_write_CTRLA_reg(hw, _i2cms[i].ctrl_a);
	hri_sercomi2cm_write_CTRLB_reg(hw, _i2cms[i].ctrl_b);
	hri_sercomi2cm_write_BAUD_reg(hw, _i2cms[i].baud);

	service->mode = (_i2cms[i].ctrl_a & SERCOM_I2CM_CTRLA_SPEED_Msk) >> SERCOM_I2CM_CTRLA_SPEED_Pos;
	hri_sercomi2cm_write_ADDR_HS_bit(hw, service->mode < I2C_HS ? 0 : 1);

	service->trise = _i2cms[i].trise;

	return ERR_NONE;
}

/* SERCOM I2C slave */

#ifndef CONF_SERCOM_0_I2CS_ENABLE
#define CONF_SERCOM_0_I2CS_ENABLE 0
#endif
#ifndef CONF_SERCOM_1_I2CS_ENABLE
#define CONF_SERCOM_1_I2CS_ENABLE 0
#endif
#ifndef CONF_SERCOM_2_I2CS_ENABLE
#define CONF_SERCOM_2_I2CS_ENABLE 0
#endif
#ifndef CONF_SERCOM_3_I2CS_ENABLE
#define CONF_SERCOM_3_I2CS_ENABLE 0
#endif
#ifndef CONF_SERCOM_4_I2CS_ENABLE
#define CONF_SERCOM_4_I2CS_ENABLE 0
#endif
#ifndef CONF_SERCOM_5_I2CS_ENABLE
#define CONF_SERCOM_5_I2CS_ENABLE 0
#endif
#ifndef CONF_SERCOM_6_I2CS_ENABLE
#define CONF_SERCOM_6_I2CS_ENABLE 0
#endif
#ifndef CONF_SERCOM_7_I2CS_ENABLE
#define CONF_SERCOM_7_I2CS_ENABLE 0
#endif

/** Amount of SERCOM that is used as I2C Slave. */
#define SERCOM_I2CS_AMOUNT                                                                                             \
	(CONF_SERCOM_0_I2CS_ENABLE + CONF_SERCOM_1_I2CS_ENABLE + CONF_SERCOM_2_I2CS_ENABLE + CONF_SERCOM_3_I2CS_ENABLE     \
	 + CONF_SERCOM_4_I2CS_ENABLE                                                                                       \
	 + CONF_SERCOM_5_I2CS_ENABLE                                                                                       \
	 + CONF_SERCOM_6_I2CS_ENABLE                                                                                       \
	 + CONF_SERCOM_7_I2CS_ENABLE)

/**
 * \brief Macro is used to fill I2C slave configuration structure based on
 * its number
 *
 * \param[in] n The number of structures
 */
#define I2CS_CONFIGURATION(n)                                                                                          \
	{                                                                                                                  \
		n, SERCOM_I2CM_CTRLA_MODE_I2C_SLAVE | (CONF_SERCOM_##n##_I2CS_RUNSTDBY << SERCOM_I2CS_CTRLA_RUNSTDBY_Pos)      \
		       | SERCOM_I2CS_CTRLA_SDAHOLD(CONF_SERCOM_##n##_I2CS_SDAHOLD)                                             \
		       | (CONF_SERCOM_##n##_I2CS_SEXTTOEN << SERCOM_I2CS_CTRLA_SEXTTOEN_Pos)                                   \
		       | (CONF_SERCOM_##n##_I2CS_SPEED << SERCOM_I2CS_CTRLA_SPEED_Pos)                                         \
		       | (CONF_SERCOM_##n##_I2CS_SCLSM << SERCOM_I2CS_CTRLA_SCLSM_Pos)                                         \
		       | (CONF_SERCOM_##n##_I2CS_LOWTOUT << SERCOM_I2CS_CTRLA_LOWTOUTEN_Pos),                                  \
		    SERCOM_I2CS_CTRLB_SMEN | SERCOM_I2CS_CTRLB_AACKEN | SERCOM_I2CS_CTRLB_AMODE(CONF_SERCOM_##n##_I2CS_AMODE), \
		    (CONF_SERCOM_##n##_I2CS_GENCEN << SERCOM_I2CS_ADDR_GENCEN_Pos)                                             \
		        | SERCOM_I2CS_ADDR_ADDR(CONF_SERCOM_##n##_I2CS_ADDRESS)                                                \
		        | (CONF_SERCOM_##n##_I2CS_TENBITEN << SERCOM_I2CS_ADDR_TENBITEN_Pos)                                   \
		        | SERCOM_I2CS_ADDR_ADDRMASK(CONF_SERCOM_##n##_I2CS_ADDRESS_MASK)                                       \
	}

/**
 * \brief Macro to check 10-bit addressing
 */
#define I2CS_7BIT_ADDRESSING_MASK 0x7F

static int32_t _i2c_s_init(void *const hw);
static int8_t _get_i2c_s_index(const void *const hw);
static inline void _i2c_s_deinit(void *const hw);
static int32_t _i2c_s_set_address(void *const hw, const uint16_t address);

/**
 * \brief SERCOM I2C slave configuration type
 */
struct i2cs_configuration {
	uint8_t                    number;
	hri_sercomi2cs_ctrla_reg_t ctrl_a;
	hri_sercomi2cs_ctrlb_reg_t ctrl_b;
	hri_sercomi2cs_addr_reg_t  address;
};

#if SERCOM_I2CS_AMOUNT < 1
/** Dummy array for compiling. */
static struct i2cs_configuration _i2css[1] = {{0}};
#else
/**
 * \brief Array of SERCOM I2C slave configurations
 */
static struct i2cs_configuration _i2css[] = {
#if CONF_SERCOM_0_I2CS_ENABLE == 1
    I2CS_CONFIGURATION(0),
#endif
#if CONF_SERCOM_1_I2CS_ENABLE == 1
    I2CS_CONFIGURATION(1),
#endif
#if CONF_SERCOM_2_I2CS_ENABLE == 1
    I2CS_CONFIGURATION(2),
#endif
#if CONF_SERCOM_3_I2CS_ENABLE == 1
    I2CS_CONFIGURATION(3),
#endif
#if CONF_SERCOM_4_I2CS_ENABLE == 1
    I2CS_CONFIGURATION(4),
#endif
#if CONF_SERCOM_5_I2CS_ENABLE == 1
    I2CS_CONFIGURATION(5),
#endif
};
#endif

/**
 * \brief Initialize synchronous I2C slave
 */
int32_t _i2c_s_sync_init(struct _i2c_s_sync_device *const device, void *const hw)
{
	int32_t status;

	ASSERT(device);

	status = _i2c_s_init(hw);
	if (status) {
		return status;
	}
	device->hw = hw;

	return ERR_NONE;
}

/**
 * \brief Initialize asynchronous I2C slave
 */
int32_t _i2c_s_async_init(struct _i2c_s_async_device *const device, void *const hw)
{
	int32_t init_status;

	ASSERT(device);

	init_status = _i2c_s_init(hw);
	if (init_status) {
		return init_status;
	}

	device->hw = hw;
	_sercom_init_irq_param(hw, (void *)device);
	NVIC_DisableIRQ((IRQn_Type)_sercom_get_irq_num(hw));
	NVIC_ClearPendingIRQ((IRQn_Type)_sercom_get_irq_num(hw));
	NVIC_EnableIRQ((IRQn_Type)_sercom_get_irq_num(hw));

	return ERR_NONE;
}

/**
 * \brief Deinitialize synchronous I2C
 */
int32_t _i2c_s_sync_deinit(struct _i2c_s_sync_device *const device)
{
	_i2c_s_deinit(device->hw);

	return ERR_NONE;
}

/**
 * \brief Deinitialize asynchronous I2C
 */
int32_t _i2c_s_async_deinit(struct _i2c_s_async_device *const device)
{
	NVIC_DisableIRQ((IRQn_Type)_sercom_get_irq_num(device->hw));
	_i2c_s_deinit(device->hw);

	return ERR_NONE;
}

/**
 * \brief Enable I2C module
 */
int32_t _i2c_s_sync_enable(struct _i2c_s_sync_device *const device)
{
	hri_sercomi2cs_set_CTRLA_ENABLE_bit(device->hw);

	return ERR_NONE;
}

/**
 * \brief Enable I2C module
 */
int32_t _i2c_s_async_enable(struct _i2c_s_async_device *const device)
{
	hri_sercomi2cs_set_CTRLA_ENABLE_bit(device->hw);

	return ERR_NONE;
}

/**
 * \brief Disable I2C module
 */
int32_t _i2c_s_sync_disable(struct _i2c_s_sync_device *const device)
{
	hri_sercomi2cs_clear_CTRLA_ENABLE_bit(device->hw);

	return ERR_NONE;
}

/**
 * \brief Disable I2C module
 */
int32_t _i2c_s_async_disable(struct _i2c_s_async_device *const device)
{
	hri_sercomi2cs_clear_CTRLA_ENABLE_bit(device->hw);

	return ERR_NONE;
}

/**
 * \brief Check if 10-bit addressing mode is on
 */
int32_t _i2c_s_sync_is_10bit_addressing_on(const struct _i2c_s_sync_device *const device)
{
	return hri_sercomi2cs_get_ADDR_TENBITEN_bit(device->hw);
}

/**
 * \brief Check if 10-bit addressing mode is on
 */
int32_t _i2c_s_async_is_10bit_addressing_on(const struct _i2c_s_async_device *const device)
{
	return hri_sercomi2cs_get_ADDR_TENBITEN_bit(device->hw);
}

/**
 * \brief Set I2C slave address
 */
int32_t _i2c_s_sync_set_address(struct _i2c_s_sync_device *const device, const uint16_t address)
{
	return _i2c_s_set_address(device->hw, address);
}

/**
 * \brief Set I2C slave address
 */
int32_t _i2c_s_async_set_address(struct _i2c_s_async_device *const device, const uint16_t address)
{
	return _i2c_s_set_address(device->hw, address);
}

/**
 * \brief Write a byte to the given I2C instance
 */
void _i2c_s_sync_write_byte(struct _i2c_s_sync_device *const device, const uint8_t data)
{
	hri_sercomi2cs_write_DATA_reg(device->hw, data);
}

/**
 * \brief Write a byte to the given I2C instance
 */
void _i2c_s_async_write_byte(struct _i2c_s_async_device *const device, const uint8_t data)
{
	hri_sercomi2cs_write_DATA_reg(device->hw, data);
}

/**
 * \brief Read a byte from the given I2C instance
 */
uint8_t _i2c_s_sync_read_byte(const struct _i2c_s_sync_device *const device)
{
	return hri_sercomi2cs_read_DATA_reg(device->hw);
}

/**
 * \brief Check if I2C is ready to send next byt
 */
bool _i2c_s_sync_is_byte_sent(const struct _i2c_s_sync_device *const device)
{
	return hri_sercomi2cs_get_interrupt_DRDY_bit(device->hw);
}

/**
 * \brief Check if there is data received by I2C
 */
bool _i2c_s_sync_is_byte_received(const struct _i2c_s_sync_device *const device)
{
	return hri_sercomi2cs_get_interrupt_DRDY_bit(device->hw);
}

/**
 * \brief Retrieve I2C slave status
 */
i2c_s_status_t _i2c_s_sync_get_status(const struct _i2c_s_sync_device *const device)
{
	return hri_sercomi2cs_read_STATUS_reg(device->hw);
}

/**
 * \brief Retrieve I2C slave status
 */
i2c_s_status_t _i2c_s_async_get_status(const struct _i2c_s_async_device *const device)
{
	return hri_sercomi2cs_read_STATUS_reg(device->hw);
}

/**
 * \brief Abort data transmission
 */
int32_t _i2c_s_async_abort_transmission(const struct _i2c_s_async_device *const device)
{
	hri_sercomi2cs_clear_INTEN_DRDY_bit(device->hw);

	return ERR_NONE;
}

/**
 * \brief Enable/disable I2C slave interrupt
 */
int32_t _i2c_s_async_set_irq_state(struct _i2c_s_async_device *const device, const enum _i2c_s_async_callback_type type,
                                   const bool state)
{
	ASSERT(device);

	if (I2C_S_DEVICE_TX == type || I2C_S_DEVICE_RX_COMPLETE == type) {
		hri_sercomi2cs_write_INTEN_DRDY_bit(device->hw, state);
	} else if (I2C_S_DEVICE_ERROR == type) {
		hri_sercomi2cs_write_INTEN_ERROR_bit(device->hw, state);
	}

	return ERR_NONE;
}

/**
 * \internal Initalize i2c slave hardware
 *
 * \param[in] p The pointer to hardware instance
 *
 *\ return status of initialization
 */
static int32_t _i2c_s_init(void *const hw)
{
	int8_t i = _get_i2c_s_index(hw);
	if (i == -1) {
		return ERR_INVALID_ARG;
	}

	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_CTRLA_SWRST);
	if (hri_sercomi2cs_get_CTRLA_ENABLE_bit(hw)) {
		return ERR_DENIED;
	}
	hri_sercomi2cs_set_CTRLA_SWRST_bit(hw);
	hri_sercomi2cs_wait_for_sync(hw, SERCOM_I2CS_CTRLA_SWRST);

	hri_sercomi2cs_write_CTRLA_reg(hw, _i2css[i].ctrl_a);
	hri_sercomi2cs_write_CTRLB_reg(hw, _i2css[i].ctrl_b);
	hri_sercomi2cs_write_ADDR_reg(hw, _i2css[i].address);

	return ERR_NONE;
}

/**
 * \internal Retrieve ordinal number of the given sercom hardware instance
 *
 * \param[in] hw The pointer to hardware instance
 *
 * \return The ordinal number of the given sercom hardware instance
 */
static int8_t _get_i2c_s_index(const void *const hw)
{
	uint8_t sercom_offset = _sercom_get_hardware_index(hw);
	uint8_t i;

	for (i = 0; i < ARRAY_SIZE(_i2css); i++) {
		if (_i2css[i].number == sercom_offset) {
			return i;
		}
	}

	ASSERT(false);
	return -1;
}

/**
 * \internal De-initialize i2c slave
 *
 * \param[in] hw The pointer to hardware instance
 */
static inline void _i2c_s_deinit(void *const hw)
{
	hri_sercomi2cs_clear_CTRLA_ENABLE_bit(hw);
	hri_sercomi2cs_set_CTRLA_SWRST_bit(hw);
}

/**
 * \internal De-initialize i2c slave
 *
 * \param[in] hw The pointer to hardware instance
 * \param[in] address Address to set
 */
static int32_t _i2c_s_set_address(void *const hw, const uint16_t address)
{
	bool enabled;

	enabled = hri_sercomi2cs_get_CTRLA_ENABLE_bit(hw);

	CRITICAL_SECTION_ENTER()
	hri_sercomi2cs_clear_CTRLA_ENABLE_bit(hw);
	hri_sercomi2cs_write_ADDR_ADDR_bf(hw, address);
	CRITICAL_SECTION_LEAVE()

	if (enabled) {
		hri_sercomi2cs_set_CTRLA_ENABLE_bit(hw);
	}

	return ERR_NONE;
}

/* Sercom SPI implementation */

#ifndef SERCOM_USART_CTRLA_MODE_SPI_SLAVE
#define SERCOM_USART_CTRLA_MODE_SPI_SLAVE (2 << 2)
#endif

#define SPI_DEV_IRQ_MODE 0x8000

#define _SPI_CS_PORT_EXTRACT(cs) (((cs) >> 0) & 0xFF)
#define _SPI_CS_PIN_EXTRACT(cs) (((cs) >> 8) & 0xFF)

COMPILER_PACK_SET(1)
/** Initialization configuration of registers. */
struct sercomspi_regs_cfg {
	uint32_t ctrla;
	uint32_t ctrlb;
	uint32_t addr;
	uint8_t  baud;
	uint8_t  dbgctrl;
	uint16_t dummy_byte;
	uint8_t  n;
};
COMPILER_PACK_RESET()

/** Build configuration from header macros. */
#define SERCOMSPI_REGS(n)                                                                                              \
	{                                                                                                                  \
		((CONF_SERCOM_##n##_SPI_DORD) | (CONF_SERCOM_##n##_SPI_CPOL << SERCOM_SPI_CTRLA_CPOL_Pos)                      \
		 | (CONF_SERCOM_##n##_SPI_CPHA << SERCOM_SPI_CTRLA_CPHA_Pos)                                                   \
		 | (CONF_SERCOM_##n##_SPI_AMODE_EN ? SERCOM_SPI_CTRLA_FORM(2) : SERCOM_SPI_CTRLA_FORM(0))                      \
		 | SERCOM_SPI_CTRLA_DOPO(CONF_SERCOM_##n##_SPI_TXPO)                                                           \
		 | SERCOM_SPI_CTRLA_DIPO(CONF_SERCOM_##n##_SPI_RXPO)                                                           \
		 | (CONF_SERCOM_##n##_SPI_IBON << SERCOM_SPI_CTRLA_IBON_Pos)                                                   \
		 | (CONF_SERCOM_##n##_SPI_RUNSTDBY << SERCOM_SPI_CTRLA_RUNSTDBY_Pos)                                           \
		 | SERCOM_SPI_CTRLA_MODE(CONF_SERCOM_##n##_SPI_MODE)), /* ctrla */                                             \
		    ((CONF_SERCOM_##n##_SPI_RXEN << SERCOM_SPI_CTRLB_RXEN_Pos)                                                 \
		     | (CONF_SERCOM_##n##_SPI_MSSEN << SERCOM_SPI_CTRLB_MSSEN_Pos)                                             \
		     | (CONF_SERCOM_##n##_SPI_SSDE << SERCOM_SPI_CTRLB_SSDE_Pos)                                               \
		     | (CONF_SERCOM_##n##_SPI_PLOADEN << SERCOM_SPI_CTRLB_PLOADEN_Pos)                                         \
		     | SERCOM_SPI_CTRLB_AMODE(CONF_SERCOM_##n##_SPI_AMODE)                                                     \
		     | SERCOM_SPI_CTRLB_CHSIZE(CONF_SERCOM_##n##_SPI_CHSIZE)), /* ctrlb */                                     \
		    (SERCOM_SPI_ADDR_ADDR(CONF_SERCOM_##n##_SPI_ADDR)                                                          \
		     | SERCOM_SPI_ADDR_ADDRMASK(CONF_SERCOM_##n##_SPI_ADDRMASK)),      /* addr */                              \
		    ((uint8_t)CONF_SERCOM_##n##_SPI_BAUD_RATE),                        /* baud */                              \
		    (CONF_SERCOM_##n##_SPI_DBGSTOP << SERCOM_SPI_DBGCTRL_DBGSTOP_Pos), /* dbgctrl */                           \
		    CONF_SERCOM_##n##_SPI_DUMMYBYTE,                                   /* Dummy byte for SPI master mode */    \
		    n                                                                  /* sercom number */                     \
	}

#ifndef CONF_SERCOM_0_SPI_ENABLE
#define CONF_SERCOM_0_SPI_ENABLE 0
#endif
#ifndef CONF_SERCOM_1_SPI_ENABLE
#define CONF_SERCOM_1_SPI_ENABLE 0
#endif
#ifndef CONF_SERCOM_2_SPI_ENABLE
#define CONF_SERCOM_2_SPI_ENABLE 0
#endif
#ifndef CONF_SERCOM_3_SPI_ENABLE
#define CONF_SERCOM_3_SPI_ENABLE 0
#endif
#ifndef CONF_SERCOM_4_SPI_ENABLE
#define CONF_SERCOM_4_SPI_ENABLE 0
#endif
#ifndef CONF_SERCOM_5_SPI_ENABLE
#define CONF_SERCOM_5_SPI_ENABLE 0
#endif
#ifndef CONF_SERCOM_6_SPI_ENABLE
#define CONF_SERCOM_6_SPI_ENABLE 0
#endif
#ifndef CONF_SERCOM_7_SPI_ENABLE
#define CONF_SERCOM_7_SPI_ENABLE 0
#endif

/** Amount of SERCOM that is used as SPI */
#define SERCOM_SPI_AMOUNT                                                                                              \
	(CONF_SERCOM_0_SPI_ENABLE + CONF_SERCOM_1_SPI_ENABLE + CONF_SERCOM_2_SPI_ENABLE + CONF_SERCOM_3_SPI_ENABLE         \
	 + CONF_SERCOM_4_SPI_ENABLE                                                                                        \
	 + CONF_SERCOM_5_SPI_ENABLE                                                                                        \
	 + CONF_SERCOM_6_SPI_ENABLE                                                                                        \
	 + CONF_SERCOM_7_SPI_ENABLE)

#if SERCOM_SPI_AMOUNT < 1
/** Dummy array for compiling. */
static const struct sercomspi_regs_cfg sercomspi_regs[1] = {{0}};
#else
/** The SERCOM SPI configurations of SERCOM that is used as SPI. */
static const struct sercomspi_regs_cfg sercomspi_regs[] = {
#if CONF_SERCOM_0_SPI_ENABLE
    SERCOMSPI_REGS(0),
#endif
#if CONF_SERCOM_1_SPI_ENABLE
    SERCOMSPI_REGS(1),
#endif
#if CONF_SERCOM_2_SPI_ENABLE
    SERCOMSPI_REGS(2),
#endif
#if CONF_SERCOM_3_SPI_ENABLE
    SERCOMSPI_REGS(3),
#endif
#if CONF_SERCOM_4_SPI_ENABLE
    SERCOMSPI_REGS(4),
#endif
#if CONF_SERCOM_5_SPI_ENABLE
    SERCOMSPI_REGS(5),
#endif
#if CONF_SERCOM_6_SPI_ENABLE
    SERCOMSPI_REGS(6),
#endif
#if CONF_SERCOM_7_SPI_ENABLE
    SERCOMSPI_REGS(7),
#endif
};
#endif

/** \internal De-initialize SERCOM SPI
 *
 *  \param[in] hw Pointer to the hardware register base.
 *
 * \return De-initialization status
 */
static int32_t _spi_deinit(void *const hw)
{
	hri_sercomspi_clear_CTRLA_ENABLE_bit(hw);
	hri_sercomspi_set_CTRLA_SWRST_bit(hw);

	return ERR_NONE;
}

/** \internal Enable SERCOM SPI
 *
 *  \param[in] hw Pointer to the hardware register base.
 *
 * \return Enabling status
 */
static int32_t _spi_sync_enable(void *const hw)
{
	if (hri_sercomspi_is_syncing(hw, SERCOM_SPI_SYNCBUSY_SWRST)) {
		return ERR_BUSY;
	}

	hri_sercomspi_set_CTRLA_ENABLE_bit(hw);

	return ERR_NONE;
}

/** \internal Enable SERCOM SPI
 *
 *  \param[in] hw Pointer to the hardware register base.
 *
 * \return Enabling status
 */
static int32_t _spi_async_enable(void *const hw)
{
	_spi_sync_enable(hw);
	NVIC_EnableIRQ(_sercom_get_irq_num(hw));
	NVIC_EnableIRQ(_sercom_get_irq_num(hw));

	return ERR_NONE;
}

/** \internal Disable SERCOM SPI
 *
 *  \param[in] hw Pointer to the hardware register base.
 *
 * \return Disabling status
 */
static int32_t _spi_sync_disable(void *const hw)
{
	if (hri_sercomspi_is_syncing(hw, SERCOM_SPI_SYNCBUSY_SWRST)) {
		return ERR_BUSY;
	}
	hri_sercomspi_clear_CTRLA_ENABLE_bit(hw);

	return ERR_NONE;
}

/** \internal Disable SERCOM SPI
 *
 *  \param[in] hw Pointer to the hardware register base.
 *
 * \return Disabling status
 */
static int32_t _spi_async_disable(void *const hw)
{
	_spi_sync_disable(hw);
	hri_sercomspi_clear_INTEN_reg(
	    hw, SERCOM_SPI_INTFLAG_ERROR | SERCOM_SPI_INTFLAG_RXC | SERCOM_SPI_INTFLAG_TXC | SERCOM_SPI_INTFLAG_DRE);
	NVIC_DisableIRQ(_sercom_get_irq_num(hw));

	return ERR_NONE;
}

/** \internal Set SERCOM SPI mode
 *
 * \param[in] hw Pointer to the hardware register base.
 * \param[in] mode The mode to set
 *
 * \return Setting mode status
 */
static int32_t _spi_set_mode(void *const hw, const enum spi_transfer_mode mode)
{
	uint32_t ctrla;

	if (hri_sercomspi_is_syncing(hw, SERCOM_SPI_SYNCBUSY_SWRST | SERCOM_SPI_SYNCBUSY_ENABLE)) {
		return ERR_BUSY;
	}

	ctrla = hri_sercomspi_read_CTRLA_reg(hw);
	ctrla &= ~(SERCOM_SPI_CTRLA_CPOL | SERCOM_SPI_CTRLA_CPHA);
	ctrla |= (mode & 0x3u) << SERCOM_SPI_CTRLA_CPHA_Pos;
	hri_sercomspi_write_CTRLA_reg(hw, ctrla);

	return ERR_NONE;
}

/** \internal Set SERCOM SPI baudrate
 *
 * \param[in] hw Pointer to the hardware register base.
 * \param[in] baud_val The baudrate to set
 *
 * \return Setting baudrate status
 */
static int32_t _spi_set_baudrate(void *const hw, const uint32_t baud_val)
{
	if (hri_sercomspi_is_syncing(hw, SERCOM_SPI_SYNCBUSY_SWRST)) {
		return ERR_BUSY;
	}

	hri_sercomspi_write_BAUD_reg(hw, baud_val);

	return ERR_NONE;
}

/** \internal Set SERCOM SPI char size
 *
 * \param[in] hw Pointer to the hardware register base.
 * \param[in] baud_val The baudrate to set
 * \param[out] size Stored char size
 *
 * \return Setting char size status
 */
static int32_t _spi_set_char_size(void *const hw, const enum spi_char_size char_size, uint8_t *const size)
{
	/* Only 8-bit or 9-bit accepted */
	if (!(char_size == SPI_CHAR_SIZE_8 || char_size == SPI_CHAR_SIZE_9)) {
		return ERR_INVALID_ARG;
	}

	if (hri_sercomspi_is_syncing(hw, SERCOM_SPI_SYNCBUSY_SWRST | SERCOM_SPI_SYNCBUSY_CTRLB)) {
		return ERR_BUSY;
	}

	hri_sercomspi_write_CTRLB_CHSIZE_bf(hw, char_size);
	*size = (char_size == SPI_CHAR_SIZE_8) ? 1 : 2;

	return ERR_NONE;
}

/** \internal Set SERCOM SPI data order
 *
 * \param[in] hw Pointer to the hardware register base.
 * \param[in] baud_val The baudrate to set
 *
 * \return Setting data order status
 */
static int32_t _spi_set_data_order(void *const hw, const enum spi_data_order dord)
{
	uint32_t ctrla;

	if (hri_sercomspi_is_syncing(hw, SERCOM_SPI_SYNCBUSY_SWRST)) {
		return ERR_BUSY;
	}

	ctrla = hri_sercomspi_read_CTRLA_reg(hw);

	if (dord == SPI_DATA_ORDER_LSB_1ST) {
		ctrla |= SERCOM_SPI_CTRLA_DORD;
	} else {
		ctrla &= ~SERCOM_SPI_CTRLA_DORD;
	}
	hri_sercomspi_write_CTRLA_reg(hw, ctrla);

	return ERR_NONE;
}

/** \brief Load SERCOM registers to init for SPI master mode
 *  The settings will be applied with default master mode, unsupported things
 *  are ignored.
 *  \param[in, out] hw Pointer to the hardware register base.
 *  \param[in] regs Pointer to register configuration values.
 */
static inline void _spi_load_regs_master(void *const hw, const struct sercomspi_regs_cfg *regs)
{
	ASSERT(hw && regs);
	hri_sercomspi_write_CTRLA_reg(
	    hw, regs->ctrla & ~(SERCOM_SPI_CTRLA_IBON | SERCOM_SPI_CTRLA_ENABLE | SERCOM_SPI_CTRLA_SWRST));
	hri_sercomspi_write_CTRLB_reg(
	    hw,
	    (regs->ctrlb
	     & ~(SERCOM_SPI_CTRLB_MSSEN | SERCOM_SPI_CTRLB_AMODE_Msk | SERCOM_SPI_CTRLB_SSDE | SERCOM_SPI_CTRLB_PLOADEN))
	        | (SERCOM_SPI_CTRLB_RXEN));
	hri_sercomspi_write_BAUD_reg(hw, regs->baud);
	hri_sercomspi_write_DBGCTRL_reg(hw, regs->dbgctrl);
}

/** \brief Load SERCOM registers to init for SPI slave mode
 *  The settings will be applied with default slave mode, unsupported things
 *  are ignored.
 *  \param[in, out] hw Pointer to the hardware register base.
 *  \param[in] regs Pointer to register configuration values.
 */
static inline void _spi_load_regs_slave(void *const hw, const struct sercomspi_regs_cfg *regs)
{
	ASSERT(hw && regs);
	hri_sercomspi_write_CTRLA_reg(
	    hw, regs->ctrla & ~(SERCOM_SPI_CTRLA_IBON | SERCOM_SPI_CTRLA_ENABLE | SERCOM_SPI_CTRLA_SWRST));
	hri_sercomspi_write_CTRLB_reg(hw,
	                              (regs->ctrlb & ~(SERCOM_SPI_CTRLB_MSSEN))
	                                  | (SERCOM_SPI_CTRLB_RXEN | SERCOM_SPI_CTRLB_SSDE | SERCOM_SPI_CTRLB_PLOADEN));
	hri_sercomspi_write_ADDR_reg(hw, regs->addr);
	hri_sercomspi_write_DBGCTRL_reg(hw, regs->dbgctrl);
	while (hri_sercomspi_is_syncing(hw, 0xFFFFFFFF))
		;
}

/** \brief Return the pointer to register settings of specific SERCOM
 *  \param[in] hw_addr The hardware register base address.
 *  \return Pointer to register settings of specific SERCOM.
 */
static inline const struct sercomspi_regs_cfg *_spi_get_regs(const uint32_t hw_addr)
{
	uint8_t n = _sercom_get_hardware_index((const void *)hw_addr);
	uint8_t i;

	for (i = 0; i < sizeof(sercomspi_regs) / sizeof(struct sercomspi_regs_cfg); i++) {
		if (sercomspi_regs[i].n == n) {
			return &sercomspi_regs[i];
		}
	}

	return NULL;
}

int32_t _spi_m_sync_init(struct _spi_m_sync_dev *dev, void *const hw)
{
	const struct sercomspi_regs_cfg *regs = _spi_get_regs((uint32_t)hw);

	ASSERT(dev && hw);

	if (regs == NULL) {
		return ERR_INVALID_ARG;
	}

	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_SWRST);
	if (hri_sercomspi_get_CTRLA_ENABLE_bit(hw)) {
		return ERR_DENIED;
	}
	hri_sercomspi_set_CTRLA_SWRST_bit(hw);
	hri_sercomspi_wait_for_sync(hw, SERCOM_SPI_SYNCBUSY_SWRST);
	dev->prvt = hw;

	if ((regs->ctrla & SERCOM_SPI_CTRLA_MODE_Msk) == SERCOM_USART_CTRLA_MODE_SPI_SLAVE) {
		_spi_load_regs_slave(hw, regs);
	} else {
		_spi_load_regs_master(hw, regs);
	}

	/* Load character size from default hardware configuration */
	dev->char_size = ((regs->ctrlb & SERCOM_SPI_CTRLB_CHSIZE_Msk) == 0) ? 1 : 2;

	dev->dummy_byte = regs->dummy_byte;

	return ERR_NONE;
}

int32_t _spi_s_sync_init(struct _spi_s_sync_dev *dev, void *const hw)
{
	return _spi_m_sync_init(dev, hw);
}

int32_t _spi_m_async_init(struct _spi_async_dev *dev, void *const hw)
{
	struct _spi_async_dev *spid = dev;
	/* Do hardware initialize. */
	int32_t rc = _spi_m_sync_init((struct _spi_m_sync_dev *)dev, hw);

	if (rc < 0) {
		return rc;
	}

	_sercom_init_irq_param(hw, (void *)dev);
	/* Initialize callbacks: must use them */
	spid->callbacks.complete = NULL;
	spid->callbacks.rx       = NULL;
	spid->callbacks.tx       = NULL;
	NVIC_DisableIRQ((IRQn_Type)_sercom_get_irq_num(hw));
	NVIC_ClearPendingIRQ((IRQn_Type)_sercom_get_irq_num(hw));

	return ERR_NONE;
}

int32_t _spi_s_async_init(struct _spi_s_async_dev *dev, void *const hw)
{
	return _spi_m_async_init(dev, hw);
}

int32_t _spi_m_async_deinit(struct _spi_async_dev *dev)
{
	NVIC_DisableIRQ(_sercom_get_irq_num(dev->prvt));
	NVIC_ClearPendingIRQ(_sercom_get_irq_num(dev->prvt));

	return _spi_deinit(dev->prvt);
}

int32_t _spi_s_async_deinit(struct _spi_s_async_dev *dev)
{
	NVIC_DisableIRQ(_sercom_get_irq_num(dev->prvt));
	NVIC_ClearPendingIRQ(_sercom_get_irq_num(dev->prvt));

	return _spi_deinit(dev->prvt);
}

int32_t _spi_m_sync_deinit(struct _spi_m_sync_dev *dev)
{
	return _spi_deinit(dev->prvt);
}

int32_t _spi_s_sync_deinit(struct _spi_s_sync_dev *dev)
{
	return _spi_deinit(dev->prvt);
}

int32_t _spi_m_sync_enable(struct _spi_m_sync_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return _spi_sync_enable(dev->prvt);
}

int32_t _spi_s_sync_enable(struct _spi_s_sync_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return _spi_sync_enable(dev->prvt);
}

int32_t _spi_m_async_enable(struct _spi_async_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return _spi_async_enable(dev->prvt);
}

int32_t _spi_s_async_enable(struct _spi_s_async_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return _spi_async_enable(dev->prvt);
}

int32_t _spi_m_sync_disable(struct _spi_m_sync_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return _spi_sync_disable(dev->prvt);
}

int32_t _spi_s_sync_disable(struct _spi_s_sync_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return _spi_sync_disable(dev->prvt);
}

int32_t _spi_m_async_disable(struct _spi_async_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return _spi_async_disable(dev->prvt);
}

int32_t _spi_s_async_disable(struct _spi_s_async_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return _spi_async_disable(dev->prvt);
}

int32_t _spi_m_sync_set_mode(struct _spi_m_sync_dev *dev, const enum spi_transfer_mode mode)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_mode(dev->prvt, mode);
}

int32_t _spi_m_async_set_mode(struct _spi_async_dev *dev, const enum spi_transfer_mode mode)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_mode(dev->prvt, mode);
}

int32_t _spi_s_async_set_mode(struct _spi_s_async_dev *dev, const enum spi_transfer_mode mode)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_mode(dev->prvt, mode);
}

int32_t _spi_s_sync_set_mode(struct _spi_s_sync_dev *dev, const enum spi_transfer_mode mode)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_mode(dev->prvt, mode);
}

int32_t _spi_calc_baud_val(struct spi_dev *dev, const uint32_t clk, const uint32_t baud)
{
	int32_t rc;
	ASSERT(dev);

	/* Not accept 0es */
	if (clk == 0 || baud == 0) {
		return ERR_INVALID_ARG;
	}

	/* Check baudrate range of current assigned clock */
	if (!(baud <= (clk >> 1) && baud >= (clk >> 8))) {
		return ERR_INVALID_ARG;
	}

	rc = ((clk >> 1) / baud) - 1;
	return rc;
}

int32_t _spi_m_sync_set_baudrate(struct _spi_m_sync_dev *dev, const uint32_t baud_val)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_baudrate(dev->prvt, baud_val);
}

int32_t _spi_m_async_set_baudrate(struct _spi_async_dev *dev, const uint32_t baud_val)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_baudrate(dev->prvt, baud_val);
}

int32_t _spi_m_sync_set_char_size(struct _spi_m_sync_dev *dev, const enum spi_char_size char_size)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_char_size(dev->prvt, char_size, &dev->char_size);
}

int32_t _spi_m_async_set_char_size(struct _spi_async_dev *dev, const enum spi_char_size char_size)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_char_size(dev->prvt, char_size, &dev->char_size);
}

int32_t _spi_s_async_set_char_size(struct _spi_s_async_dev *dev, const enum spi_char_size char_size)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_char_size(dev->prvt, char_size, &dev->char_size);
}

int32_t _spi_s_sync_set_char_size(struct _spi_s_sync_dev *dev, const enum spi_char_size char_size)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_char_size(dev->prvt, char_size, &dev->char_size);
}

int32_t _spi_m_sync_set_data_order(struct _spi_m_sync_dev *dev, const enum spi_data_order dord)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_data_order(dev->prvt, dord);
}

int32_t _spi_m_async_set_data_order(struct _spi_async_dev *dev, const enum spi_data_order dord)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_data_order(dev->prvt, dord);
}

int32_t _spi_s_async_set_data_order(struct _spi_s_async_dev *dev, const enum spi_data_order dord)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_data_order(dev->prvt, dord);
}

int32_t _spi_s_sync_set_data_order(struct _spi_s_sync_dev *dev, const enum spi_data_order dord)
{
	ASSERT(dev && dev->prvt);

	return _spi_set_data_order(dev->prvt, dord);
}

/** Wait until SPI bus idle. */
static inline void _spi_wait_bus_idle(void *const hw)
{
	while (!(hri_sercomspi_get_INTFLAG_reg(hw, SERCOM_SPI_INTFLAG_TXC | SERCOM_SPI_INTFLAG_DRE))) {
		;
	}
	hri_sercomspi_clear_INTFLAG_reg(hw, SERCOM_SPI_INTFLAG_TXC | SERCOM_SPI_INTFLAG_DRE);
}

/** Holds run time information for message sync transaction. */
struct _spi_trans_ctrl {
	/** Pointer to transmitting data buffer. */
	uint8_t *txbuf;
	/** Pointer to receiving data buffer. */
	uint8_t *rxbuf;
	/** Count number of data transmitted. */
	uint32_t txcnt;
	/** Count number of data received. */
	uint32_t rxcnt;
	/** Data character size. */
	uint8_t char_size;
};

/** Check interrupt flag of RXC and update transaction runtime information. */
static inline bool _spi_rx_check_and_receive(void *const hw, const uint32_t iflag, struct _spi_trans_ctrl *ctrl)
{
	uint32_t data;

	if (!(iflag & SERCOM_SPI_INTFLAG_RXC)) {
		return false;
	}

	data = hri_sercomspi_read_DATA_reg(hw);

	if (ctrl->rxbuf) {
		*ctrl->rxbuf++ = (uint8_t)data;

		if (ctrl->char_size > 1) {
			*ctrl->rxbuf++ = (uint8_t)(data >> 8);
		}
	}

	ctrl->rxcnt++;

	return true;
}

/** Check interrupt flag of DRE and update transaction runtime information. */
static inline void _spi_tx_check_and_send(void *const hw, const uint32_t iflag, struct _spi_trans_ctrl *ctrl,
                                          uint16_t dummy)
{
	uint32_t data;

	if (!(SERCOM_SPI_INTFLAG_DRE & iflag)) {
		return;
	}

	if (ctrl->txbuf) {
		data = *ctrl->txbuf++;

		if (ctrl->char_size > 1) {
			data |= (*ctrl->txbuf) << 8;
			ctrl->txbuf++;
		}
	} else {
		data = dummy;
	}

	ctrl->txcnt++;
	hri_sercomspi_write_DATA_reg(hw, data);
}

/** Check interrupt flag of ERROR and update transaction runtime information. */
static inline int32_t _spi_err_check(const uint32_t iflag, void *const hw)
{
	if (SERCOM_SPI_INTFLAG_ERROR & iflag) {
		hri_sercomspi_clear_STATUS_reg(hw, ~0);
		hri_sercomspi_clear_INTFLAG_reg(hw, SERCOM_SPI_INTFLAG_ERROR);
		return ERR_OVERFLOW;
	}

	return ERR_NONE;
}

int32_t _spi_m_sync_trans(struct _spi_m_sync_dev *dev, const struct spi_msg *msg)
{
	void *                 hw   = dev->prvt;
	int32_t                rc   = 0;
	struct _spi_trans_ctrl ctrl = {msg->txbuf, msg->rxbuf, 0, 0, dev->char_size};

	ASSERT(dev && hw);

	/* If settings are not applied (pending), we can not go on */
	if (hri_sercomspi_is_syncing(
	        hw, (SERCOM_SPI_SYNCBUSY_SWRST | SERCOM_SPI_SYNCBUSY_ENABLE | SERCOM_SPI_SYNCBUSY_CTRLB))) {
		return ERR_BUSY;
	}

	/* SPI must be enabled to start synchronous transfer */
	if (!hri_sercomspi_get_CTRLA_ENABLE_bit(hw)) {
		return ERR_NOT_INITIALIZED;
	}

	for (;;) {
		uint32_t iflag = hri_sercomspi_read_INTFLAG_reg(hw);

		if (!_spi_rx_check_and_receive(hw, iflag, &ctrl)) {
			/* In master mode, do not start next byte before previous byte received
			 * to make better output waveform */
			if (ctrl.rxcnt >= ctrl.txcnt) {
				_spi_tx_check_and_send(hw, iflag, &ctrl, dev->dummy_byte);
			}
		}

		rc = _spi_err_check(iflag, hw);

		if (rc < 0) {
			break;
		}
		if (ctrl.txcnt >= msg->size && ctrl.rxcnt >= msg->size) {
			rc = ctrl.txcnt;
			break;
		}
	}
	/* Wait until SPI bus idle */
	_spi_wait_bus_idle(hw);

	return rc;
}

int32_t _spi_m_async_enable_tx(struct _spi_async_dev *dev, bool state)
{
	void *hw = dev->prvt;

	ASSERT(dev && hw);

	if (state) {
		hri_sercomspi_set_INTEN_DRE_bit(hw);
	} else {
		hri_sercomspi_clear_INTEN_DRE_bit(hw);
	}

	return ERR_NONE;
}

int32_t _spi_s_async_enable_tx(struct _spi_s_async_dev *dev, bool state)
{
	return _spi_m_async_enable_tx(dev, state);
}

int32_t _spi_m_async_enable_rx(struct _spi_async_dev *dev, bool state)
{
	void *hw = dev->prvt;

	ASSERT(dev);
	ASSERT(hw);

	if (state) {
		hri_sercomspi_set_INTEN_RXC_bit(hw);
	} else {
		hri_sercomspi_clear_INTEN_RXC_bit(hw);
	}

	return ERR_NONE;
}

int32_t _spi_s_async_enable_rx(struct _spi_s_async_dev *dev, bool state)
{
	return _spi_m_async_enable_rx(dev, state);
}

int32_t _spi_m_async_enable_ss_detect(struct _spi_async_dev *dev, bool state)
{
	ASSERT(dev && dev->prvt);

	if (state) {
		hri_sercomspi_set_INTEN_TXC_bit(dev->prvt);
	} else {
		hri_sercomspi_clear_INTEN_TXC_bit(dev->prvt);
	}

	return ERR_NONE;
}

int32_t _spi_s_async_enable_ss_detect(struct _spi_s_async_dev *dev, bool state)
{
	return _spi_m_async_enable_ss_detect(dev, state);
}

int32_t _spi_m_async_write_one(struct _spi_async_dev *dev, uint16_t data)
{
	ASSERT(dev && dev->prvt);

	hri_sercomspi_write_DATA_reg(dev->prvt, data);

	return ERR_NONE;
}

int32_t _spi_s_async_write_one(struct _spi_s_async_dev *dev, uint16_t data)
{
	ASSERT(dev && dev->prvt);

	hri_sercomspi_write_DATA_reg(dev->prvt, data);

	return ERR_NONE;
}

int32_t _spi_s_sync_write_one(struct _spi_s_sync_dev *dev, uint16_t data)
{
	ASSERT(dev && dev->prvt);

	hri_sercomspi_write_DATA_reg(dev->prvt, data);

	return ERR_NONE;
}

uint16_t _spi_m_async_read_one(struct _spi_async_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return hri_sercomspi_read_DATA_reg(dev->prvt);
}

uint16_t _spi_s_async_read_one(struct _spi_s_async_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return hri_sercomspi_read_DATA_reg(dev->prvt);
}

uint16_t _spi_s_sync_read_one(struct _spi_s_sync_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return hri_sercomspi_read_DATA_reg(dev->prvt);
}

int32_t _spi_m_async_register_callback(struct _spi_async_dev *dev, const enum _spi_async_dev_cb_type cb_type,
                                       const FUNC_PTR func)
{
	typedef void (*func_t)(void);
	struct _spi_async_dev *spid = dev;

	ASSERT(dev && (cb_type < SPI_DEV_CB_N));

	func_t *p_ls  = (func_t *)&spid->callbacks;
	p_ls[cb_type] = (func_t)func;

	return ERR_NONE;
}

int32_t _spi_s_async_register_callback(struct _spi_s_async_dev *dev, const enum _spi_s_async_dev_cb_type cb_type,
                                       const FUNC_PTR func)
{
	return _spi_m_async_register_callback(dev, cb_type, func);
}

bool _spi_s_sync_is_tx_ready(struct _spi_s_sync_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return hri_sercomi2cm_get_INTFLAG_reg(dev->prvt, SERCOM_SPI_INTFLAG_DRE);
}

bool _spi_s_sync_is_rx_ready(struct _spi_s_sync_dev *dev)
{
	ASSERT(dev && dev->prvt);

	return hri_sercomi2cm_get_INTFLAG_reg(dev->prvt, SERCOM_SPI_INTFLAG_RXC);
}

bool _spi_s_sync_is_ss_deactivated(struct _spi_s_sync_dev *dev)
{
	void *hw = dev->prvt;

	ASSERT(dev && hw);

	if (hri_sercomi2cm_get_INTFLAG_reg(hw, SERCOM_SPI_INTFLAG_TXC)) {
		hri_sercomspi_clear_INTFLAG_reg(hw, SERCOM_SPI_INTFLAG_TXC);
		return true;
	}
	return false;
}

bool _spi_s_sync_is_error(struct _spi_s_sync_dev *dev)
{
	void *hw = dev->prvt;

	ASSERT(dev && hw);

	if (hri_sercomi2cm_get_INTFLAG_reg(hw, SERCOM_SPI_INTFLAG_ERROR)) {
		hri_sercomspi_clear_STATUS_reg(hw, SERCOM_SPI_STATUS_BUFOVF);
		hri_sercomspi_clear_INTFLAG_reg(hw, SERCOM_SPI_INTFLAG_ERROR);
		return true;
	}
	return false;
}

/**
 * \brief Enable/disable SPI master interrupt
 *
 * param[in] device The pointer to SPI master device instance
 * param[in] type The type of interrupt to disable/enable if applicable
 * param[in] state Enable or disable
 */
void _spi_m_async_set_irq_state(struct _spi_async_dev *const device, const enum _spi_async_dev_cb_type type,
                                const bool state)
{
	ASSERT(device);

	if (SPI_DEV_CB_COMPLETE == type) {
		hri_sercomspi_write_INTEN_ERROR_bit(device->prvt, state);
	}
}
