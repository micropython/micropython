/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
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

#include "bindings/rp2pio/StateMachine.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Processor.h"
#include "shared-bindings/usb_host/Port.h"
#include "supervisor/usb.h"

#include "src/common/pico_time/include/pico/time.h"
#include "src/rp2040/hardware_structs/include/hardware/structs/mpu.h"
#include "src/rp2_common/cmsis/stub/CMSIS/Device/RaspberryPi/RP2040/Include/RP2040.h"
#include "src/rp2_common/hardware_dma/include/hardware/dma.h"
#include "src/rp2_common/pico_multicore/include/pico/multicore.h"

#include "py/runtime.h"

#include "tusb.h"

#include "lib/Pico-PIO-USB/src/pio_usb.h"
#include "lib/Pico-PIO-USB/src/pio_usb_configuration.h"

#include "supervisor/serial.h"

usb_host_port_obj_t usb_host_instance;

STATIC PIO pio_instances[2] = {pio0, pio1};
volatile bool _core1_ready = false;

static void __not_in_flash_func(core1_main)(void) {
    // The MPU is reset before this starts.
    SysTick->LOAD = (uint32_t)((common_hal_mcu_processor_get_frequency() / 1000) - 1UL);
    SysTick->VAL = 0UL;   /* Load the SysTick Counter Value */
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |  // Processor clock.
        SysTick_CTRL_ENABLE_Msk;

    // Turn off flash access. After this, it will hard fault. Better than messing
    // up CIRCUITPY.
    MPU->CTRL = MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_ENABLE_Msk;
    MPU->RNR = 6; // 7 is used by pico-sdk stack protection.
    MPU->RBAR = XIP_MAIN_BASE | MPU_RBAR_VALID_Msk;
    MPU->RASR = MPU_RASR_XN_Msk | // Set execute never and everything else is restricted.
        MPU_RASR_ENABLE_Msk |
        (0x1b << MPU_RASR_SIZE_Pos);         // Size is 0x10000000 which masks up to SRAM region.
    MPU->RNR = 7;

    _core1_ready = true;

    while (true) {
        pio_usb_host_frame();
        if (tuh_task_event_ready()) {
            // Queue the tinyusb background task.
            usb_background_schedule();
        }
        // Wait for systick to reload.
        while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0) {
        }
    }
}

STATIC uint8_t _sm_free_count(uint8_t pio_index) {
    PIO pio = pio_instances[pio_index];
    uint8_t free_count = 0;
    for (size_t j = 0; j < NUM_PIO_STATE_MACHINES; j++) {
        if (!pio_sm_is_claimed(pio, j)) {
            free_count++;
        }
    }
    return free_count;
}

STATIC bool _has_program_room(uint8_t pio_index, uint8_t program_size) {
    PIO pio = pio_instances[pio_index];
    pio_program_t program_struct = {
        .instructions = NULL,
        .length = program_size,
        .origin = -1
    };
    return pio_can_add_program(pio, &program_struct);
}

usb_host_port_obj_t *common_hal_usb_host_port_construct(const mcu_pin_obj_t *dp, const mcu_pin_obj_t *dm) {
    if (dp->number + 1 != dm->number) {
        raise_ValueError_invalid_pins();
    }
    usb_host_port_obj_t *self = &usb_host_instance;

    // Return the singleton if given the same pins.
    if (self->dp != NULL) {
        if (self->dp != dp || self->dm != dm) {
            mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("%q in use"), MP_QSTR_usb_host);
        }
        return self;
    }

    assert_pin_free(dp);
    assert_pin_free(dm);

    pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
    pio_cfg.skip_alarm_pool = true;
    pio_cfg.pin_dp = dp->number;
    // Allocating the peripherals like this works on Pico W, where the
    // "preferred PIO" for the cyw43 wifi chip is PIO 1.
    pio_cfg.pio_tx_num = 1; // uses 22 instructions and 1 SM
    pio_cfg.pio_rx_num = 0; // uses 31 instructions and 2 SM.
    if (!_has_program_room(pio_cfg.pio_tx_num, 22) || _sm_free_count(pio_cfg.pio_tx_num) < 1 ||
        !_has_program_room(pio_cfg.pio_rx_num, 31) || _sm_free_count(pio_cfg.pio_rx_num) < 2) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("All state machines in use"));
    }
    pio_cfg.tx_ch = dma_claim_unused_channel(false); // DMA channel
    if (pio_cfg.tx_ch < 0) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("All dma channels in use"));
    }

    self->base.type = &usb_host_port_type;
    self->dp = dp;
    self->dm = dm;

    PIO tx_pio = pio_instances[pio_cfg.pio_tx_num];
    pio_cfg.sm_tx = pio_claim_unused_sm(tx_pio, false);
    PIO rx_pio = pio_instances[pio_cfg.pio_rx_num];
    pio_cfg.sm_rx = pio_claim_unused_sm(rx_pio, false);
    pio_cfg.sm_eop = pio_claim_unused_sm(rx_pio, false);

    // Unclaim everything so that the library can.
    dma_channel_unclaim(pio_cfg.tx_ch);
    pio_sm_unclaim(tx_pio, pio_cfg.sm_tx);
    pio_sm_unclaim(rx_pio, pio_cfg.sm_rx);
    pio_sm_unclaim(rx_pio, pio_cfg.sm_eop);

    // Set all of the state machines to never reset.
    rp2pio_statemachine_never_reset(tx_pio, pio_cfg.sm_tx);
    rp2pio_statemachine_never_reset(rx_pio, pio_cfg.sm_rx);
    rp2pio_statemachine_never_reset(rx_pio, pio_cfg.sm_eop);

    common_hal_never_reset_pin(dp);
    common_hal_never_reset_pin(dm);

    // Core 1 will run the SOF interrupt directly.
    _core1_ready = false;
    multicore_launch_core1(core1_main);
    while (!_core1_ready) {
    }

    tuh_configure(TUH_OPT_RHPORT, TUH_CFGID_RPI_PIO_USB_CONFIGURATION, &pio_cfg);
    tuh_init(TUH_OPT_RHPORT);

    return self;
}
