#include "common-hal/spitarget/SPITarget.h"
#include "common-hal/busio/__init__.h"

#include "shared-bindings/spitarget/SPITarget.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "hpl_sercom_config.h"
#include "peripheral_clk_config.h"

#include "hal/include/hal_gpio.h"
#include "hal/include/hal_spi_m_sync.h"

#include "hpl_sercom_config.h"
#include "samd/sercom.h"

void common_hal_spitarget_spi_target_construct(spitarget_spi_target_obj_t *self,
    const mcu_pin_obj_t *sck, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso, const mcu_pin_obj_t *ss) {
    Sercom *sercom = NULL;
    uint8_t sercom_index;
    uint32_t clock_pinmux = 0;
    uint32_t mosi_pinmux = 0;
    uint32_t miso_pinmux = 0;
    uint32_t ss_pinmux = 0;
    uint8_t clock_pad = 0;
    uint8_t mosi_pad = 0;
    uint8_t miso_pad = 0;
    uint8_t dopo = 255;

    // Ensure the object starts in its deinit state.
    self->clock_pin = NO_PIN;

    // Special case for SAMR21 boards. (feather_radiofruit_zigbee)
    #if defined(PIN_PC19F_SERCOM4_PAD0)
    if (miso == &pin_PC19) {
        if (mosi == &pin_PB30 && sck == &pin_PC18) {
            sercom = SERCOM4;
            sercom_index = 4;
            clock_pinmux = MUX_F;
            mosi_pinmux = MUX_F;
            miso_pinmux = MUX_F;
            clock_pad = 3;
            mosi_pad = 2;
            miso_pad = 0;
            dopo = samd_peripherals_get_spi_dopo(clock_pad, mosi_pad);
        }
        // Error, leave SERCOM unset to throw an exception later.
    } else
    #endif
    {
        for (int i = 0; i < NUM_SERCOMS_PER_PIN; i++) {
            sercom_index = sck->sercom[i].index; // 2 for SERCOM2, etc.
            if (sercom_index >= SERCOM_INST_NUM) {
                continue;
            }
            Sercom *potential_sercom = sercom_insts[sercom_index];
            if (potential_sercom->SPI.CTRLA.bit.ENABLE != 0) {
                continue;
            }
            clock_pinmux = PINMUX(sck->number, (i == 0) ? MUX_C : MUX_D);
            clock_pad = sck->sercom[i].pad;
            if (!samd_peripherals_valid_spi_clock_pad(clock_pad)) {
                continue;
            }
            // find miso_pad first, since it corresponds to dopo which takes limited values
            for (int j = 0; j < NUM_SERCOMS_PER_PIN; j++) {
                if (sercom_index == miso->sercom[j].index) {
                    miso_pinmux = PINMUX(miso->number, (j == 0) ? MUX_C : MUX_D);
                    miso_pad = miso->sercom[j].pad;
                    dopo = samd_peripherals_get_spi_dopo(clock_pad, miso_pad);
                    if (dopo > 0x3) {
                        continue;  // pad combination not possible
                    }
                } else {
                    continue;
                }
                for (int k = 0; k < NUM_SERCOMS_PER_PIN; k++) {
                    if (sercom_index == mosi->sercom[k].index) {
                        mosi_pinmux = PINMUX(mosi->number, (k == 0) ? MUX_C : MUX_D);
                        mosi_pad = mosi->sercom[k].pad;
                        for (int m = 0; m < NUM_SERCOMS_PER_PIN; m++) {
                            if (sercom_index == ss->sercom[m].index) {
                                ss_pinmux = PINMUX(ss->number, (m == 0) ? MUX_C : MUX_D);
                                sercom = potential_sercom;
                                break;
                            }
                        }
                        if (sercom != NULL) {
                            break;
                        }
                    }
                }
                if (sercom != NULL) {
                    break;
                }
            }
            if (sercom != NULL) {
                break;
            }
        }
    }
    if (sercom == NULL) {
        raise_ValueError_invalid_pins();
    }

    // Set up SPI clocks on SERCOM.
    samd_peripherals_sercom_clock_init(sercom, sercom_index);

    if (spi_m_sync_init(&self->spi_desc, sercom) != ERR_NONE) {
        mp_raise_OSError(MP_EIO);
    }

    // Pads must be set after spi_m_sync_init(), which uses default values from
    // the prototypical SERCOM.

    hri_sercomspi_write_CTRLA_MODE_bf(sercom, 2);
    hri_sercomspi_write_CTRLA_DOPO_bf(sercom, dopo);
    hri_sercomspi_write_CTRLA_DIPO_bf(sercom, mosi_pad);
    hri_sercomspi_write_CTRLB_PLOADEN_bit(sercom, 1);

    // Always start at 250khz which is what SD cards need. They are sensitive to
    // SPI bus noise before they are put into SPI mode.
    uint8_t baud_value = samd_peripherals_spi_baudrate_to_baud_reg_value(250000);
    if (spi_m_sync_set_baudrate(&self->spi_desc, baud_value) != ERR_NONE) {
        // spi_m_sync_set_baudrate does not check for validity, just whether the device is
        // busy or not
        mp_raise_OSError(MP_EIO);
    }

    gpio_set_pin_direction(sck->number, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(sck->number, GPIO_PULL_OFF);
    gpio_set_pin_function(sck->number, clock_pinmux);
    claim_pin(sck);
    self->clock_pin = sck->number;

    gpio_set_pin_direction(mosi->number, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(mosi->number, GPIO_PULL_OFF);
    gpio_set_pin_function(mosi->number, mosi_pinmux);
    self->MOSI_pin = mosi->number;
    claim_pin(mosi);

    gpio_set_pin_direction(miso->number, GPIO_DIRECTION_OUT);
    gpio_set_pin_pull_mode(miso->number, GPIO_PULL_OFF);
    gpio_set_pin_function(miso->number, miso_pinmux);
    self->MISO_pin = miso->number;
    claim_pin(miso);

    gpio_set_pin_direction(ss->number, GPIO_DIRECTION_IN);
    gpio_set_pin_pull_mode(ss->number, GPIO_PULL_OFF);
    gpio_set_pin_function(ss->number, ss_pinmux);
    self->SS_pin = ss->number;
    claim_pin(ss);

    self->running_dma.failure = 1; // not started
    self->mosi_packet = NULL;
    self->miso_packet = NULL;

    spi_m_sync_enable(&self->spi_desc);
}

bool common_hal_spitarget_spi_target_deinited(spitarget_spi_target_obj_t *self) {
    return self->clock_pin == NO_PIN;
}

void common_hal_spitarget_spi_target_deinit(spitarget_spi_target_obj_t *self) {
    if (common_hal_spitarget_spi_target_deinited(self)) {
        return;
    }
    spi_m_sync_disable(&self->spi_desc);
    spi_m_sync_deinit(&self->spi_desc);
    reset_pin_number(self->clock_pin);
    reset_pin_number(self->MOSI_pin);
    reset_pin_number(self->MISO_pin);
    reset_pin_number(self->SS_pin);
    self->clock_pin = NO_PIN;
}

void common_hal_spitarget_spi_target_transfer_start(spitarget_spi_target_obj_t *self,
    uint8_t *mosi_packet, const uint8_t *miso_packet, size_t len) {
    if (len == 0) {
        return;
    }
    if (self->running_dma.failure != 1) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Async SPI transfer in progress on this bus, keep awaiting."));
    }

    self->mosi_packet = mosi_packet;
    self->miso_packet = miso_packet;

    Sercom *sercom = self->spi_desc.dev.prvt;
    shared_dma_transfer_start(&self->running_dma, sercom, miso_packet, &sercom->SPI.DATA.reg, &sercom->SPI.DATA.reg, mosi_packet, len, 0);

    // There is an issue where if an unexpected SPI transfer is received before the user calls "end" for the in-progress, expected
    // transfer, the SERCOM has an error and gets confused. This can be detected from INTFLAG.ERROR. I think the code in
    // ports/atmel-samd/peripherals/samd/dma.c at line 277 (as of this commit; it's the part that reads s->SPI.INTFLAG.bit.RXC and
    // s->SPI.DATA.reg) is supposed to fix this, but experimentation seems to show that it does not in fact fix anything. Anyways, if
    // the ERROR bit is set, let's just reset the peripheral and then setup the transfer again -- that seems to work.
    if (hri_sercomspi_get_INTFLAG_ERROR_bit(sercom)) {
        shared_dma_transfer_close(&self->running_dma);

        // disable the sercom
        spi_m_sync_disable(&self->spi_desc);
        hri_sercomspi_wait_for_sync(sercom, SERCOM_SPI_SYNCBUSY_MASK);

        // save configurations
        hri_sercomspi_ctrla_reg_t ctrla_saved_val = hri_sercomspi_get_CTRLA_reg(sercom, -1); // -1 mask is all ones: save all bits
        hri_sercomspi_ctrlb_reg_t ctrlb_saved_val = hri_sercomspi_get_CTRLB_reg(sercom, -1); // -1 mask is all ones: save all bits
        hri_sercomspi_baud_reg_t baud_saved_val = hri_sercomspi_get_BAUD_reg(sercom, -1);    // -1 mask is all ones: save all bits
        // reset
        hri_sercomspi_set_CTRLA_SWRST_bit(sercom);
        hri_sercomspi_wait_for_sync(sercom, SERCOM_SPI_SYNCBUSY_MASK);
        // re-write configurations
        hri_sercomspi_write_CTRLA_reg(sercom, ctrla_saved_val);
        hri_sercomspi_write_CTRLB_reg(sercom, ctrlb_saved_val);
        hri_sercomspi_write_BAUD_reg(sercom, baud_saved_val);
        hri_sercomspi_wait_for_sync(sercom, SERCOM_SPI_SYNCBUSY_MASK);

        // re-enable the sercom
        spi_m_sync_enable(&self->spi_desc);
        hri_sercomspi_wait_for_sync(sercom, SERCOM_SPI_SYNCBUSY_MASK);

        shared_dma_transfer_start(&self->running_dma, sercom, miso_packet, &sercom->SPI.DATA.reg, &sercom->SPI.DATA.reg, mosi_packet, len, 0);
    }
}

bool common_hal_spitarget_spi_target_transfer_is_finished(spitarget_spi_target_obj_t *self) {
    return self->running_dma.failure == 1 || shared_dma_transfer_finished(&self->running_dma);
}

int common_hal_spitarget_spi_target_transfer_close(spitarget_spi_target_obj_t *self) {
    if (self->running_dma.failure == 1) {
        return 0;
    }
    int res = shared_dma_transfer_close(&self->running_dma);
    self->running_dma.failure = 1;

    self->mosi_packet = NULL;
    self->miso_packet = NULL;

    return res;
}
