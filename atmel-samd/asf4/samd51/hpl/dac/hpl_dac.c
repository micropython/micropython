
/**
* \file
*
* \brief SAM Digital to Analog Converter
*
* Copyright (C) 2016 - 2017 Atmel Corporation. All rights reserved.
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
* DAMAGES (INCLUDING, BUT NOT LIMIT ED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* \asf_license_stop
*
*/

#include <hpl_dac_config.h>
#include <utils_assert.h>
#include <hpl_dac_sync.h>
#include <hpl_dac_async.h>

/** \conf INTERNAL */
static int32_t _dac_init(void *const hw);
static inline void _dac_deinit(void *const hw);
static inline void _dac_enable_channel(void *const hw, const uint8_t ch);
static inline void _dac_disable_channel(void *const hw, const uint8_t ch);
static inline bool _dac_is_channel_enabled(void *const hw, const uint8_t ch);
/** \endcond */

/**
 * \brief DAC configuration type
 */
struct dac_configuration {
	hri_dac_ctrlb_reg_t   ctrlb;
	hri_dac_evctrl_reg_t  ev_ctrl;
	hri_dac_dacctrl_reg_t dac_ctrl0;
	hri_dac_dacctrl_reg_t dac_ctrl1;
	hri_dac_dbgctrl_reg_t dbg_ctrl;
};

/**
 * \brief Array of DAC configurations
 */
static struct dac_configuration _dac
    = {(CONF_DAC_DIFF << DAC_CTRLB_DIFF_Pos) | DAC_CTRLB_REFSEL(CONF_DAC_REFSEL),
       (CONF_DAC_INVEI0 << DAC_EVCTRL_INVEI0_Pos) | (CONF_DAC_EMPTYEO0 << DAC_EVCTRL_EMPTYEO0_Pos)
           | (CONF_DAC_STARTEI0 << DAC_EVCTRL_STARTEI0_Pos)
           | (CONF_DAC_INVEI1 << DAC_EVCTRL_INVEI1_Pos)
           | (CONF_DAC_EMPTYEO1 << DAC_EVCTRL_EMPTYEO1_Pos)
           | (CONF_DAC_STARTEI1 << DAC_EVCTRL_STARTEI1_Pos),
       (CONF_DAC0_LEFTADJ << DAC_DACCTRL_LEFTADJ_Pos) | DAC_DACCTRL_CCTRL(CONF_DAC0_CCTRL)
           | (CONF_DAC0_RUNSTDBY << DAC_DACCTRL_RUNSTDBY_Pos)
           | (CONF_DAC0_DITHER << DAC_DACCTRL_DITHER_Pos)
           | (CONF_DAC0_REFRESH << DAC_DACCTRL_REFRESH_Pos),
       (CONF_DAC1_LEFTADJ << DAC_DACCTRL_LEFTADJ_Pos) | DAC_DACCTRL_CCTRL(CONF_DAC1_CCTRL)
           | (CONF_DAC1_RUNSTDBY << DAC_DACCTRL_RUNSTDBY_Pos)
           | (CONF_DAC1_DITHER << DAC_DACCTRL_DITHER_Pos)
           | (CONF_DAC1_REFRESH << DAC_DACCTRL_REFRESH_Pos),
       (CONF_DAC_DBGRUN << DAC_DBGCTRL_DBGRUN_Pos)};

/*!< Pointer to hpl device */
static struct _dac_async_device *_dac_dev = NULL;

/**
 * \brief Initialize synchronous DAC
 */
int32_t _dac_sync_init(struct _dac_sync_device *const device, void *const hw)
{
	ASSERT(device);

	device->hw = hw;

	return _dac_init(device->hw);
}

/**
* \brief Initialize DAC
*/
int32_t _dac_async_init(struct _dac_async_device *const device, void *const hw)
{
	int32_t init_status;

	ASSERT(device);

	init_status = _dac_init(hw);
	if (init_status) {
		return init_status;
	}
	device->hw = hw;

	_dac_dev = device;
	NVIC_DisableIRQ(DAC_0_IRQn + 0);
	NVIC_ClearPendingIRQ(DAC_0_IRQn + 0);
	NVIC_EnableIRQ(DAC_0_IRQn + 0);
	NVIC_DisableIRQ(DAC_0_IRQn + 1);
	NVIC_ClearPendingIRQ(DAC_0_IRQn + 1);
	NVIC_EnableIRQ(DAC_0_IRQn + 1);
	NVIC_DisableIRQ(DAC_0_IRQn + 2);
	NVIC_ClearPendingIRQ(DAC_0_IRQn + 2);
	NVIC_EnableIRQ(DAC_0_IRQn + 2);

	return ERR_NONE;
}

/**
 * \brief De-initialize DAC
 */
void _dac_sync_deinit(struct _dac_sync_device *const device)
{
	_dac_deinit(device->hw);
}

/**
 * \brief De-initialize DAC
 */
void _dac_async_deinit(struct _dac_async_device *const device)
{
	NVIC_DisableIRQ(DAC_0_IRQn + 0);
	NVIC_DisableIRQ(DAC_0_IRQn + 1);
	NVIC_DisableIRQ(DAC_0_IRQn + 2);

	_dac_deinit(device->hw);
}

/**
 * \brief Enable DAC Channel
 */
void _dac_sync_enable_channel(struct _dac_sync_device *const device, const uint8_t ch)
{
	_dac_enable_channel(device->hw, ch);
}

/**
 * \brief Enable DAC Channel
 */
void _dac_async_enable_channel(struct _dac_async_device *const device, const uint8_t ch)
{
	_dac_enable_channel(device->hw, ch);
}

/**
 * \brief Disable DAC Channel
 */
void _dac_sync_disable_channel(struct _dac_sync_device *const device, const uint8_t ch)
{
	_dac_disable_channel(device->hw, ch);
}

/**
 * \brief Disable DAC Channel
 */
void _dac_async_disable_channel(struct _dac_async_device *const device, const uint8_t ch)
{
	_dac_disable_channel(device->hw, ch);
}

/**
 * \brief Checks if DAC channel is enabled
 */
bool _dac_sync_is_channel_enable(struct _dac_sync_device *const device, const uint8_t ch)
{
	return _dac_is_channel_enabled(device->hw, ch);
}

/**
 * \brief Checks if DAC channel is enabled
 */
bool _dac_async_is_channel_enable(struct _dac_async_device *const device, const uint8_t ch)
{
	return _dac_is_channel_enabled(device->hw, ch);
}

/**
 * \brief write synchronous DAC data for output
 */
void _dac_sync_write_data(struct _dac_sync_device *const device, const uint16_t data, const uint8_t ch)
{
	hri_dac_write_DATA_reg(device->hw, ch, data);
}

/**
 * \brief write DAC data for output
 */
void _dac_async_write_data(struct _dac_async_device *const device, const uint16_t data, const uint8_t ch)
{
	hri_dac_write_DATABUF_reg(device->hw, ch, data);
}

/**
 * \brief Enable/disable DAC interrupt
 */
void _dac_async_set_irq_state(struct _dac_async_device *const device, const enum _dac_callback_type type,
                              const bool state)
{
	void *hw = device->hw;

	if (DAC_DEVICE_CONVERSION_DONE_CB == type) {
		hri_dac_write_INTEN_EMPTY0_bit(hw, state);
		hri_dac_write_INTEN_EMPTY1_bit(hw, state);
	} else if (DAC_DEVICE_ERROR_CB == type) {
		hri_dac_write_INTEN_UNDERRUN0_bit(hw, state);
		hri_dac_write_INTEN_UNDERRUN1_bit(hw, state);
	}
}

/**
 * \internal Initialize DAC
 *
 * \param[in] hw The pointer to DAC hardware instance
 *
 * \return The status of initialization
 */
static int32_t _dac_init(void *const hw)
{
	ASSERT(hw == DAC);

	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_SWRST);
	if (hri_dac_get_CTRLA_ENABLE_bit(hw)) {
		return ERR_BUSY;
	}
	hri_dac_set_CTRLA_SWRST_bit(hw);
	hri_dac_wait_for_sync(hw, DAC_SYNCBUSY_SWRST);

	hri_dac_write_CTRLB_reg(hw, _dac.ctrlb);
	hri_dac_write_EVCTRL_reg(hw, _dac.ev_ctrl);
	hri_dac_write_DACCTRL_reg(hw, 0, _dac.dac_ctrl0);
	hri_dac_write_DACCTRL_reg(hw, 1, _dac.dac_ctrl1);
	hri_dac_write_DBGCTRL_reg(hw, _dac.dbg_ctrl);

	return ERR_NONE;
}

/**
 * \internal De-initialize DAC
 *
 * \param[in] hw The pointer to DAC hardware instance
 */
static inline void _dac_deinit(void *const hw)
{
	hri_dac_clear_CTRLA_ENABLE_bit(hw);
	hri_dac_set_CTRLA_SWRST_bit(hw);
}

/**
 * \internal Enable DAC channel
 *
 * \param[in] hw The pointer to hardware instance
 * \param[in] ch The channel to enable
 */
static inline void _dac_enable_channel(void *const hw, const uint8_t ch)
{
	ASSERT(ch <= 1);

	hri_dac_clear_CTRLA_ENABLE_bit(hw);
	hri_dac_set_DACCTRL_ENABLE_bit(hw, ch);
	hri_dac_set_CTRLA_ENABLE_bit(hw);

	if (0 == ch) {
		while (!hri_dac_get_STATUS_READY0_bit(hw))
			;
	} else {
		while (!hri_dac_get_STATUS_READY1_bit(hw))
			;
	}
}

/**
 * \internal Disable DAC channel
 *
 * \param[in] hw The pointer to hardware instance
 * \param[in] ch The channel to disable
 */
static inline void _dac_disable_channel(void *const hw, const uint8_t ch)
{
	ASSERT(ch <= 1);

	hri_dac_clear_CTRLA_ENABLE_bit(hw);
	hri_dac_clear_DACCTRL_ENABLE_bit(hw, ch);

	if (hri_dac_get_DACCTRL_ENABLE_bit(hw, !ch)) {
		hri_dac_set_CTRLA_ENABLE_bit(hw);
	}
}

/**
 * \internal Checks if DAC channel is enabled
 *
 * \param[in] hw The pointer to hardware instance
 * \param[in] ch The channel to check
 */
static inline bool _dac_is_channel_enabled(void *const hw, const uint8_t ch)
{
	if (ch <= 1) {
		return hri_dac_get_DACCTRL_ENABLE_bit(hw, ch);
	}

	return false;
}
