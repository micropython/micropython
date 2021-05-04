/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 hathach for Adafruit Industries
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include "supervisor/spi_flash_api.h"

#include <stdint.h>
#include <string.h>

#include "py/mpconfig.h" // for EXTERNAL_FLASH_QSPI_DUAL
#include "nrfx_qspi.h"

#include "shared-bindings/microcontroller/__init__.h"

#include "supervisor/shared/external_flash/common_commands.h"
#include "supervisor/shared/external_flash/qspi_flash.h"
#ifdef NRF_DEBUG_PRINT
#include "supervisor/serial.h" // dbg_printf()
#endif

#ifdef QSPI_FLASH_POWERDOWN
// Parameters for external QSPI Flash power-down
// for W25Q128FV,
//   tDP (nCS high to Power-down mode) = 3us
//   tRES (nCS high to Standby mode)   = 3us
//   sck_delay = max(tDP, tRES) / 62.5ns = 48  -> 50 (w/ margin)
#define DUR_DPM_ENTER  1  // tDP  in (256*62.5ns) units
#define DUR_DPM_EXIT   1  // tRES in (256*62.5ns) units
#define SCK_DELAY      50 // max(tDP, tRES) in (62.5ns) units
//   wait necessary just after DPM enter/exit (cut and try)
#define WAIT_AFTER_DPM_ENTER 10 // usec
#define WAIT_AFTER_DPM_EXIT  50 // usec

static int sck_delay_saved = 0;
#endif

#ifdef NRF_DEBUG_PRINT
extern void dbg_dumpQSPIreg(void);
#else
#define dbg_dumpQSPIreg(...)
#endif

// When USB is disconnected, disable QSPI in sleep mode to save energy
void qspi_disable(void) {
    // If VBUS is detected, no need to disable QSPI
    if (NRF_QSPI->ENABLE && !(NRF_POWER->USBREGSTATUS & POWER_USBREGSTATUS_VBUSDETECT_Msk)) {
        // Keep CS high when QSPI is diabled
        nrf_gpio_cfg_output(MICROPY_QSPI_CS);
        nrf_gpio_pin_write(MICROPY_QSPI_CS, 1);

        // Workaround to disable QSPI according to nRF52840 Revision 1 Errata V1.4 - 3.8
        NRF_QSPI->TASKS_DEACTIVATE = 1;
        *(volatile uint32_t *)0x40029054 = 1;
        NRF_QSPI->ENABLE = 0;
    }
}

void qspi_enable(void) {
    if (NRF_QSPI->ENABLE) {
        return;
    }

    nrf_qspi_enable(NRF_QSPI);

    nrf_qspi_event_clear(NRF_QSPI, NRF_QSPI_EVENT_READY);
    nrf_qspi_task_trigger(NRF_QSPI, NRF_QSPI_TASK_ACTIVATE);

    uint32_t remaining_attempts = 100;
    do {
        if (nrf_qspi_event_check(NRF_QSPI, NRF_QSPI_EVENT_READY)) {
            break;
        }
        NRFX_DELAY_US(10);
    } while (--remaining_attempts);
}

bool spi_flash_command(uint8_t command) {
    qspi_enable();
    nrf_qspi_cinstr_conf_t cinstr_cfg = {
        .opcode = command,
        .length = 1,
        .io2_level = true,
        .io3_level = true,
        .wipwait = false,
        .wren = false
    };
    return nrfx_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL) == NRFX_SUCCESS;
}

bool spi_flash_read_command(uint8_t command, uint8_t *response, uint32_t length) {
    qspi_enable();
    nrf_qspi_cinstr_conf_t cinstr_cfg = {
        .opcode = command,
        .length = length + 1,
        .io2_level = true,
        .io3_level = true,
        .wipwait = false,
        .wren = false
    };
    return nrfx_qspi_cinstr_xfer(&cinstr_cfg, NULL, response) == NRFX_SUCCESS;

}

bool spi_flash_write_command(uint8_t command, uint8_t *data, uint32_t length) {
    qspi_enable();
    nrf_qspi_cinstr_conf_t cinstr_cfg = {
        .opcode = command,
        .length = length + 1,
        .io2_level = true,
        .io3_level = true,
        .wipwait = false,
        .wren = false // We do this manually.
    };
    return nrfx_qspi_cinstr_xfer(&cinstr_cfg, data, NULL) == NRFX_SUCCESS;
}

bool spi_flash_sector_command(uint8_t command, uint32_t address) {
    qspi_enable();
    if (command != CMD_SECTOR_ERASE) {
        return false;
    }
    return nrfx_qspi_erase(NRF_QSPI_ERASE_LEN_4KB, address) == NRFX_SUCCESS;
}

bool spi_flash_write_data(uint32_t address, uint8_t *data, uint32_t length) {
    qspi_enable();
    // TODO: In theory, this also needs to handle unaligned data and
    // non-multiple-of-4 length.  (in practice, I don't think the fat layer
    // generates such writes)
    return nrfx_qspi_write(data, length, address) == NRFX_SUCCESS;
}

bool spi_flash_read_data(uint32_t address, uint8_t *data, uint32_t length) {
    qspi_enable();
    int misaligned = ((intptr_t)data) & 3;
    // If the data is misaligned, we need to read 4 bytes
    // into an aligned buffer, and then copy 1, 2, or 3 bytes from the aligned
    // buffer to data.
    if (misaligned) {
        int sz = 4 - misaligned;
        __attribute__((aligned(4))) uint8_t buf[4];

        if (nrfx_qspi_read(buf, 4, address) != NRFX_SUCCESS) {
            return false;
        }
        memcpy(data, buf, sz);
        data += sz;
        address += sz;
        length -= sz;
    }

    // nrfx_qspi_read works in 4 byte increments, though it doesn't
    // signal an error if sz is not a multiple of 4.  Read (directly into data)
    // all but the last 1, 2, or 3 bytes depending on the (remaining) length.
    uint32_t sz = length & ~(uint32_t)3;
    if (nrfx_qspi_read(data, sz, address) != NRFX_SUCCESS) {
        return false;
    }
    data += sz;
    address += sz;
    length -= sz;

    // Now, if we have any bytes left over, we must do a final read of 4
    // bytes and copy 1, 2, or 3 bytes to data.
    if (length) {
        __attribute__((aligned(4))) uint8_t buf[4];
        if (nrfx_qspi_read(buf, 4, address) != NRFX_SUCCESS) {
            return false;
        }
        memcpy(data, buf, length);
    }

    return true;
}

void spi_flash_init(void) {
    // Init QSPI flash
    nrfx_qspi_config_t qspi_cfg = {
        .xip_offset = 0,
        .pins = {
            .sck_pin = MICROPY_QSPI_SCK,
            .csn_pin = MICROPY_QSPI_CS,
            .io0_pin = MICROPY_QSPI_DATA0,
            .io1_pin = NRF_QSPI_PIN_NOT_CONNECTED,
            .io2_pin = NRF_QSPI_PIN_NOT_CONNECTED,
            .io3_pin = NRF_QSPI_PIN_NOT_CONNECTED,

        },
        .prot_if = {
            .readoc = NRF_QSPI_READOC_FASTREAD,
            .writeoc = NRF_QSPI_WRITEOC_PP,
            .addrmode = NRF_QSPI_ADDRMODE_24BIT,
            #ifdef QSPI_FLASH_POWERDOWN
            .dpmconfig = true
            #else
            .dpmconfig = false
                #endif
        },
        .phy_if = {
            .sck_freq = NRF_QSPI_FREQ_32MDIV16, // Start at a slow 2MHz and speed up once we know what we're talking to.
            .sck_delay = 10,    // min time CS must stay high before going low again. in unit of 62.5 ns
            .spi_mode = NRF_QSPI_MODE_0,
            .dpmen = false
        },
        .irq_priority = 7,
    };

    #if defined(EXTERNAL_FLASH_QSPI_DUAL)
    qspi_cfg.pins.io1_pin = MICROPY_QSPI_DATA1;
    qspi_cfg.prot_if.readoc = NRF_QSPI_READOC_READ2O;
    qspi_cfg.prot_if.writeoc = NRF_QSPI_WRITEOC_PP2O;
    #else
    qspi_cfg.pins.io1_pin = MICROPY_QSPI_DATA1;
    qspi_cfg.pins.io2_pin = MICROPY_QSPI_DATA2;
    qspi_cfg.pins.io3_pin = MICROPY_QSPI_DATA3;
    qspi_cfg.prot_if.readoc = NRF_QSPI_READOC_READ4IO;
    qspi_cfg.prot_if.writeoc = NRF_QSPI_WRITEOC_PP4O;
    #endif

    // No callback for blocking API
    nrfx_qspi_init(&qspi_cfg, NULL, NULL);

    #ifdef QSPI_FLASH_POWERDOWN
    // If pin-reset while flash is in power-down mode,
    // the flash cannot accept any commands. Send CMD_WAKE to release it.
    spi_flash_write_command(CMD_WAKE, NULL, 0);
    NRFX_DELAY_US(WAIT_AFTER_DPM_EXIT);
    #endif
}

void spi_flash_init_device(const external_flash_device *device) {
    check_quad_enable(device);

    // Switch to single output line if the device doesn't support quad programs.
    if (!device->supports_qspi_writes) {
        NRF_QSPI->IFCONFIG0 &= ~QSPI_IFCONFIG0_WRITEOC_Msk;
        NRF_QSPI->IFCONFIG0 |= QSPI_IFCONFIG0_WRITEOC_PP << QSPI_IFCONFIG0_WRITEOC_Pos;
    }

    // Speed up as much as we can.
    // Start at 16 MHz and go down.
    // At 32 MHz GD25Q16C doesn't work reliably on Feather 52840, even though it should work up to 104 MHz.
    // sckfreq = 0 is 32 Mhz
    // sckfreq = 1 is 16 MHz, etc.
    uint8_t sckfreq = 1;
    while (32000000 / (sckfreq + 1) > device->max_clock_speed_mhz * 1000000 && sckfreq < 16) {
        sckfreq += 1;
    }
    NRF_QSPI->IFCONFIG1 &= ~QSPI_IFCONFIG1_SCKFREQ_Msk;
    NRF_QSPI->IFCONFIG1 |= sckfreq << QSPI_IFCONFIG1_SCKFREQ_Pos;
}

void qspi_flash_enter_sleep(void) {
    #ifdef QSPI_FLASH_POWERDOWN
    uint32_t r;
    NRF_QSPI->DPMDUR =
        ((DUR_DPM_ENTER & 0xFFFF) << 16) | (DUR_DPM_EXIT & 0xFFFF);
    // set sck_delay tempolarily
    r = NRF_QSPI->IFCONFIG1;
    sck_delay_saved = (r & QSPI_IFCONFIG1_SCKDELAY_Msk)
        >> QSPI_IFCONFIG1_SCKDELAY_Pos;
    NRF_QSPI->IFCONFIG1
        = (NRF_QSPI->IFCONFIG1 & ~QSPI_IFCONFIG1_SCKDELAY_Msk)
            | (SCK_DELAY << QSPI_IFCONFIG1_SCKDELAY_Pos);

    // enabling IFCONFIG0.DPMENABLE here won't work.
    //  -> do it in spi_flash_init()
    // NRF_QSPI->IFCONFIG0 |= QSPI_IFCONFIG0_DPMENABLE_Msk;
    // dbg_dumpQSPIreg();

    // enter deep power-down mode (DPM)
    NRF_QSPI->IFCONFIG1 |= QSPI_IFCONFIG1_DPMEN_Msk;
    NRFX_DELAY_US(WAIT_AFTER_DPM_ENTER);
    if (!(NRF_QSPI->STATUS & QSPI_STATUS_DPM_Msk)) {
        #ifdef NRF_DEBUG_PRINT
        dbg_printf("qspi flash: DPM failed\r\n");
        #endif
    }
    #endif

    qspi_disable();
    // dbg_dumpQSPIreg();
}

void qspi_flash_exit_sleep(void) {
    qspi_enable();

    #ifdef QSPI_FLASH_POWERDOWN
    if (NRF_QSPI->STATUS & QSPI_STATUS_DPM_Msk) {
        // exit deep power-down mode
        NRF_QSPI->IFCONFIG1 &= ~QSPI_IFCONFIG1_DPMEN_Msk;
        NRFX_DELAY_US(WAIT_AFTER_DPM_EXIT);

        if (NRF_QSPI->STATUS & QSPI_STATUS_DPM_Msk) {
            #ifdef NRF_DEBUG_PRINT
            dbg_printf("qspi flash: exiting DPM failed\r\n");
            #endif
        }
        // restore sck_delay
        if (sck_delay_saved == 0) {
            sck_delay_saved = 10; // default
        }
        NRF_QSPI->IFCONFIG1
            = (NRF_QSPI->IFCONFIG1 & ~QSPI_IFCONFIG1_SCKDELAY_Msk)
                | (sck_delay_saved << QSPI_IFCONFIG1_SCKDELAY_Pos);
    }
    // dbg_dumpQSPIreg();
    #endif
}
