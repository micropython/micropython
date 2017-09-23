
/**
 * \file
 *
 * \brief Generic DMAC related functionality.
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
#include <compiler.h>
#include <hpl_dma.h>
#include <hpl_dmac_config.h>
#include <utils.h>
#include <utils_assert.h>
#include <utils_repeat_macro.h>

#if CONF_DMAC_ENABLE

/* Section containing first descriptors for all DMAC channels */
COMPILER_ALIGNED(16)
static DmacDescriptor _descriptor_section[DMAC_CH_NUM] SECTION_DMAC_DESCRIPTOR;

/* Section containing current descriptors for all DMAC channels */
COMPILER_ALIGNED(16)
static DmacDescriptor _write_back_section[DMAC_CH_NUM] SECTION_DMAC_DESCRIPTOR;

/* Array containing callbacks for DMAC channels */
static struct _dma_resource _resources[DMAC_CH_NUM];

/* This macro DMAC configuration */
#define DMAC_CHANNEL_CFG(i, n)                                                                                         \
	{(CONF_DMAC_ENABLE_##n << DMAC_CHCTRLA_ENABLE_Pos),                                                                \
	 DMAC_CHCTRLB_TRIGACT(CONF_DMAC_TRIGACT_##n) | DMAC_CHCTRLB_TRIGSRC(CONF_DMAC_TRIGSRC_##n)                         \
	     | DMAC_CHCTRLB_LVL(CONF_DMAC_LVL_##n)                                                                         \
	     | (CONF_DMAC_EVOE_##n << DMAC_CHCTRLB_EVOE_Pos)                                                               \
	     | (CONF_DMAC_EVIE_##n << DMAC_CHCTRLB_EVIE_Pos)                                                               \
	     | DMAC_CHCTRLB_EVACT(CONF_DMAC_EVACT_##n),                                                                    \
	 DMAC_BTCTRL_STEPSIZE(CONF_DMAC_STEPSIZE_##n) | (CONF_DMAC_STEPSEL_##n << DMAC_BTCTRL_STEPSEL_Pos)                 \
	     | (CONF_DMAC_DSTINC_##n << DMAC_BTCTRL_DSTINC_Pos)                                                            \
	     | (CONF_DMAC_SRCINC_##n << DMAC_BTCTRL_SRCINC_Pos)                                                            \
	     | DMAC_BTCTRL_BEATSIZE(CONF_DMAC_BEATSIZE_##n)                                                                \
	     | DMAC_BTCTRL_BLOCKACT(CONF_DMAC_BLOCKACT_##n)                                                                \
	     | DMAC_BTCTRL_EVOSEL(CONF_DMAC_EVOSEL_##n)},

/* DMAC channel configuration */
struct dmac_channel_cfg {
	uint8_t  ctrla;
	uint32_t ctrlb;
	uint16_t btctrl;
};

/* DMAC channel configurations */
const static struct dmac_channel_cfg _cfgs[] = {REPEAT_MACRO(DMAC_CHANNEL_CFG, i, DMAC_CH_NUM)};

/**
 * \brief Initialize DMAC
 */
int32_t _dma_init(void)
{
	uint8_t i = 0;

	hri_dmac_clear_CTRL_DMAENABLE_bit(DMAC);
	hri_dmac_clear_CTRL_CRCENABLE_bit(DMAC);
	hri_dmac_set_CHCTRLA_SWRST_bit(DMAC);

	hri_dmac_write_CTRL_reg(DMAC,
	                        (CONF_DMAC_LVLEN0 << DMAC_CTRL_LVLEN0_Pos) | (CONF_DMAC_LVLEN1 << DMAC_CTRL_LVLEN1_Pos)
	                            | (CONF_DMAC_LVLEN2 << DMAC_CTRL_LVLEN2_Pos)
	                            | (CONF_DMAC_LVLEN3 << DMAC_CTRL_LVLEN3_Pos));
	hri_dmac_write_DBGCTRL_DBGRUN_bit(DMAC, CONF_DMAC_DBGRUN);

	hri_dmac_write_PRICTRL0_reg(DMAC,
	                            DMAC_PRICTRL0_LVLPRI0(CONF_DMAC_LVLPRI0) | DMAC_PRICTRL0_LVLPRI1(CONF_DMAC_LVLPRI1)
	                                | DMAC_PRICTRL0_LVLPRI2(CONF_DMAC_LVLPRI2)
	                                | DMAC_PRICTRL0_LVLPRI3(CONF_DMAC_LVLPRI3)
	                                | (CONF_DMAC_RRLVLEN0 << DMAC_PRICTRL0_RRLVLEN0_Pos)
	                                | (CONF_DMAC_RRLVLEN1 << DMAC_PRICTRL0_RRLVLEN1_Pos)
	                                | (CONF_DMAC_RRLVLEN2 << DMAC_PRICTRL0_RRLVLEN2_Pos)
	                                | (CONF_DMAC_RRLVLEN3 << DMAC_PRICTRL0_RRLVLEN3_Pos));
	hri_dmac_write_BASEADDR_reg(DMAC, (uint32_t)_descriptor_section);
	hri_dmac_write_WRBADDR_reg(DMAC, (uint32_t)_write_back_section);

	for (; i < DMAC_CH_NUM; i++) {
		hri_dmac_write_CHID_reg(DMAC, i);

		hri_dmac_write_CHCTRLB_reg(DMAC, _cfgs[i].ctrlb);
		hri_dmacdescriptor_write_BTCTRL_reg(&_descriptor_section[i], _cfgs[i].btctrl);
	}

	NVIC_DisableIRQ(DMAC_IRQn);
	NVIC_ClearPendingIRQ(DMAC_IRQn);
	NVIC_EnableIRQ(DMAC_IRQn);

	hri_dmac_set_CTRL_DMAENABLE_bit(DMAC);

	return ERR_NONE;
}

/**
 * \brief Enable/disable DMA interrupt
 */
void _dma_set_irq_state(const uint8_t channel, const enum _dma_callback_type type, const bool state)
{
	hri_dmac_write_CHID_reg(DMAC, channel);

	if (DMA_TRANSFER_COMPLETE_CB == type) {
		hri_dmac_write_CHINTEN_TCMPL_bit(DMAC, state);
	} else if (DMA_TRANSFER_ERROR_CB == type) {
		hri_dmac_write_CHINTEN_TERR_bit(DMAC, state);
	}
}

int32_t _dma_set_destination_address(const uint8_t channel, const void *const dst)
{
	hri_dmacdescriptor_write_DSTADDR_reg(&_descriptor_section[channel], (uint32_t)dst);

	return ERR_NONE;
}

int32_t _dma_set_source_address(const uint8_t channel, const void *const src)
{
	hri_dmacdescriptor_write_SRCADDR_reg(&_descriptor_section[channel], (uint32_t)src);

	return ERR_NONE;
}

int32_t _dma_srcinc_enable(const uint8_t channel, const bool enable)
{
	hri_dmacdescriptor_write_BTCTRL_SRCINC_bit(&_descriptor_section[channel], enable);

	return ERR_NONE;
}

int32_t _dma_set_data_amount(const uint8_t channel, const uint32_t amount)
{
	uint32_t address   = hri_dmacdescriptor_read_DSTADDR_reg(&_descriptor_section[channel]);
	uint8_t  beat_size = hri_dmacdescriptor_read_BTCTRL_BEATSIZE_bf(&_descriptor_section[channel]);

	if (hri_dmacdescriptor_get_BTCTRL_DSTINC_bit(&_descriptor_section[channel])) {
		hri_dmacdescriptor_write_DSTADDR_reg(&_descriptor_section[channel], address + amount * (1 << beat_size));
	}

	address = hri_dmacdescriptor_read_SRCADDR_reg(&_descriptor_section[channel]);

	if (hri_dmacdescriptor_get_BTCTRL_SRCINC_bit(&_descriptor_section[channel])) {
		hri_dmacdescriptor_write_SRCADDR_reg(&_descriptor_section[channel], address + amount * (1 << beat_size));
	}

	hri_dmacdescriptor_write_BTCNT_reg(&_descriptor_section[channel], amount);

	return ERR_NONE;
}

int32_t _dma_enable_transaction(const uint8_t channel, const bool software_trigger)
{
	hri_dmac_write_CHID_reg(DMAC, channel);
	hri_dmacdescriptor_set_BTCTRL_VALID_bit(&_descriptor_section[channel]);
	hri_dmac_set_CHCTRLA_ENABLE_bit(DMAC);
	if (software_trigger) {
		hri_dmac_set_SWTRIGCTRL_reg(DMAC, 1 << channel);
	}

	return ERR_NONE;
}

int32_t _dma_get_channel_resource(struct _dma_resource **resource, const uint8_t channel)
{
	*resource = &_resources[channel];

	return ERR_NONE;
}

/**
 * \internal DMAC interrupt handler
 */
void DMAC_Handler(void)
{
	uint8_t               channel      = hri_dmac_read_INTPEND_ID_bf(DMAC);
	struct _dma_resource *tmp_resource = &_resources[channel];

	hri_dmac_write_CHID_reg(DMAC, channel);

	if (hri_dmac_get_CHINTFLAG_TERR_bit(DMAC)) {
		hri_dmac_clear_CHINTFLAG_TERR_bit(DMAC);
		tmp_resource->dma_cb.error(tmp_resource);
	} else if (hri_dmac_get_CHINTFLAG_TCMPL_bit(DMAC)) {
		hri_dmac_clear_CHINTFLAG_TCMPL_bit(DMAC);
		tmp_resource->dma_cb.transfer_done(tmp_resource);
	}
}

#endif /* CONF_DMAC_ENABLE */
