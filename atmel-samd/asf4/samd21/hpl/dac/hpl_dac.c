
/**
* \file
*
* \brief SAM Digital to Analog Converter
*
* Copyright (C) 2016 -2017 Atmel Corporation. All rights reserved.
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

#include <hpl_dac_async.h>
#include <hpl_dac_config.h>
#include <hpl_dac_sync.h>
#include <utils_assert.h>

/** \conf INTERNAL */
static int32_t _dac_init(void *const hw);
static inline void _dac_deinit(void *const hw);
/** \endcond */

/**
 * \brief DAC configuration type
 */
struct dac_configuration {
	hri_dac_ctrla_reg_t  ctrla;   /*!< Control A Register */
	hri_dac_ctrlb_reg_t  ctrlb;   /*!< Control B Register */
	hri_dac_evctrl_reg_t ev_ctrl; /*!< Event Control Register */
};

/**
 * \brief Array of DAC configurations
 */
static struct dac_configuration _dac = {
    (CONF_DAC_RUNSTDBY << DAC_CTRLA_RUNSTDBY_Pos),
    (CONF_DAC_REFSEL << DAC_CTRLB_REFSEL_Pos) | (CONF_DAC_BDWP << DAC_CTRLB_BDWP_Pos)
        | (CONF_DAC_VPD << DAC_CTRLB_VPD_Pos)
        | (CONF_DAC_LEFTADJ << DAC_CTRLB_LEFTADJ_Pos)
        | (CONF_DAC_IOEN << DAC_CTRLB_IOEN_Pos)
        | (CONF_DAC_EOEN << DAC_CTRLB_EOEN_Pos),
    (CONF_DAC_EMPTYEO << DAC_EVCTRL_EMPTYEO_Pos) | (CONF_DAC_STARTEI << DAC_EVCTRL_STARTEI_Pos),
};

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
 *
 * param[in] hw The pointer to DAC hardware instance
 */
static int32_t _dac_init(void *const hw)
{
	hri_dac_wait_for_sync(hw);

	if (hri_dac_get_CTRLA_ENABLE_bit(hw)) {
		return ERR_DENIED;
	}
	hri_dac_set_CTRLA_SWRST_bit(hw);
	hri_dac_wait_for_sync(hw);

	hri_dac_write_EVCTRL_reg(hw, _dac.ev_ctrl);
	hri_dac_write_CTRLB_reg(hw, _dac.ctrlb);
	hri_dac_write_CTRLA_reg(hw, _dac.ctrla);
	return ERR_NONE;
}

/**
 * \brief De-initialize DAC
 *
 * param[in] hw The pointer to DAC hardware instance
 */
static inline void _dac_deinit(void *const hw)
{
	hri_dac_clear_CTRLA_ENABLE_bit(hw);
	hri_dac_set_CTRLA_SWRST_bit(hw);
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
	NVIC_DisableIRQ(DAC_IRQn);
	NVIC_ClearPendingIRQ(DAC_IRQn);
	NVIC_EnableIRQ(DAC_IRQn);

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
	NVIC_DisableIRQ(DAC_IRQn);

	_dac_deinit(device->hw);
}

/**
 * \brief Enable DAC Channel
 */
void _dac_sync_enable_channel(struct _dac_sync_device *const device, const uint8_t ch)
{
	(void)ch;
	hri_dac_set_CTRLA_ENABLE_bit(device->hw);
}

/**
 * \brief Enable DAC Channel
 */
void _dac_async_enable_channel(struct _dac_async_device *const device, const uint8_t ch)
{
	(void)ch;
	hri_dac_set_CTRLA_ENABLE_bit(device->hw);
}

/**
 * \brief Disable DAC Channel
 */
void _dac_sync_disable_channel(struct _dac_sync_device *const device, const uint8_t ch)
{
	(void)ch;
	hri_dac_clear_CTRLA_ENABLE_bit(device->hw);
}

/**
 * \brief Disable DAC Channel
 */
void _dac_async_disable_channel(struct _dac_async_device *const device, const uint8_t ch)
{
	(void)ch;
	hri_dac_clear_CTRLA_ENABLE_bit(device->hw);
}

bool _dac_sync_is_channel_enable(struct _dac_sync_device *const device, const uint8_t ch)
{
	(void)ch;
	return hri_dac_get_CTRLA_ENABLE_bit(device->hw);
}

bool _dac_async_is_channel_enable(struct _dac_async_device *const device, const uint8_t ch)
{
	(void)ch;
	return hri_dac_get_CTRLA_ENABLE_bit(device->hw);
}

/**
 * \brief write synchronous DAC data for output
 */
void _dac_sync_write_data(struct _dac_sync_device *const device, const uint16_t data, const uint8_t ch)
{
	(void)ch;
	hri_dac_write_DATA_reg(device->hw, data);
}

/**
 * \brief write DAC data for output
 */
void _dac_async_write_data(struct _dac_async_device *const device, const uint16_t data, const uint8_t ch)
{
	(void)ch;
	hri_dac_write_DATABUF_reg(device->hw, data);
}

/**
 * \brief Enable/disable DAC interrupt
 *
 * param[in] device The pointer to DAC device instance
 * param[in] type The type of interrupt to disable/enable if applicable
 * param[in] state Enable or disable
 */
void _dac_async_set_irq_state(struct _dac_async_device *const device, const enum _dac_callback_type type,
                              const bool state)
{
	void *hw = device->hw;

	if (DAC_DEVICE_CONVERSION_DONE_CB == type) {
		hri_dac_write_INTEN_EMPTY_bit(hw, state);
	} else if (DAC_DEVICE_ERROR_CB == type) {
		hri_dac_write_INTEN_UNDERRUN_bit(hw, state);
	}
}
