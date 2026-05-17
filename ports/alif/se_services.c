/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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

#include <stdio.h>
#include <string.h>

#include "irq.h"
#include "se_services.h"

#include "mhu.h"
#include "services_lib_bare_metal.h"
#include "services_lib_protocol.h"

#include "py/mphal.h"

// MHU indices.
#define MHU_SESS_MHU0   0
#define MHU_RTSS_MHU0   1
#define MAX_MHU         2

// The following timeout is implemented in se_services_handle.c as a
// simple loop busy polling on a variable set from an IRQ.
#define TIMEOUT         10000000

typedef struct {
    volatile unsigned int RST_CTRL; // 0x1A010318
    volatile unsigned int RST_STAT; // 0x1A01031C
} CPU_Type;

// HE CPU register flags
#define RST_CTRL_CPUWAIT_MASK   (1 << 0)
#define RST_CTRL_RST_REQ_MASK   (1 << 1)
#define RST_STAT_RST_ACK_MASK   (3 << 1)
#define HE_CPU                  ((CPU_Type *)0x1A010318)

static const uint32_t mhu_sender_base_address_list[MAX_MHU] = {
    MHU_SESS_S_TX_BASE,
    MHU_RTSS_S_TX_BASE
};

static const uint32_t mhu_receiver_base_address_list[MAX_MHU] = {
    MHU_SESS_S_RX_BASE,
    MHU_RTSS_S_RX_BASE
};

// Must be aligned as a uint32_t.
static uint32_t packet_buffer[SERVICES_MAX_PACKET_BUFFER_SIZE / sizeof(uint32_t)];

static uint32_t se_sess_handle;
static uint32_t se_rtss_handle;

static mhu_driver_out_t mhu_driver_out;

void MHU_SESS_S_TX_IRQHandler(void) {
    mhu_driver_out.sender_irq_handler(MHU_SESS_MHU0);
}

void MHU_SESS_S_RX_IRQHandler(void) {
    mhu_driver_out.receiver_irq_handler(MHU_SESS_MHU0);
}

void MHU_RTSS_S_TX_IRQHandler(void) {
    mhu_driver_out.sender_irq_handler(MHU_RTSS_MHU0);
}

void MHU_RTSS_S_RX_IRQHandler(void) {
    mhu_driver_out.receiver_irq_handler(MHU_RTSS_MHU0);
}

int dummy_printf(const char *fmt, ...) {
    (void)fmt;
    return 0;
}

static void se_services_irq_config(IRQn_Type irqn, bool enable) {
    if (enable) {
        NVIC_ClearPendingIRQ(irqn);
        NVIC_SetPriority(irqn, IRQ_PRI_MHU);
        NVIC_EnableIRQ(irqn);
    } else {
        NVIC_DisableIRQ(irqn);
        NVIC_ClearPendingIRQ(irqn);
    }
}

void se_services_rx_callback(uint32_t id, uint32_t channel, uint32_t data) {
    switch (id) {
        case MHU_SESS_MHU0:
            SERVICES_rx_msg_callback(id, channel, data);
            break;
        case MHU_RTSS_MHU0:
            #if MICROPY_PY_OPENAMP
            extern void metal_rproc_notified(void);
            metal_rproc_notified();
            #endif
            break;
        default:
            break;
    }
}

void se_services_init(void) {
    // Initialize MHU.
    mhu_driver_in_t mhu_driver_in;
    mhu_driver_in.sender_base_address_list = (uint32_t *)mhu_sender_base_address_list;
    mhu_driver_in.receiver_base_address_list = (uint32_t *)mhu_receiver_base_address_list;
    mhu_driver_in.mhu_count = MAX_MHU;
    mhu_driver_in.send_msg_acked_callback = SERVICES_send_msg_acked_callback;
    mhu_driver_in.rx_msg_callback = se_services_rx_callback;
    mhu_driver_in.debug_print = NULL; // not currently used by MHU_driver_initialize
    MHU_driver_initialize(&mhu_driver_in, &mhu_driver_out);

    // Initialize SE services.
    services_lib_t services_init_params = {
        .packet_buffer_address = (uint32_t)packet_buffer,
        .fn_send_mhu_message = mhu_driver_out.send_message,
        .fn_wait_ms = NULL, // not currently used by services_host_handler.c
        .wait_timeout = TIMEOUT,
        .fn_print_msg = dummy_printf,
    };
    SERVICES_initialize(&services_init_params);

    // Register SESS MHU channel.
    se_sess_handle = SERVICES_register_channel(MHU_SESS_MHU0, 0);
    se_services_irq_config(MHU_SESS_S_RX_IRQ_IRQn, true);
    se_services_irq_config(MHU_SESS_S_TX_IRQ_IRQn, true);

    // Register RTSS MHU channel.
    se_rtss_handle = SERVICES_register_channel(MHU_RTSS_MHU0, 0);
    se_services_irq_config(MHU_RTSS_S_RX_IRQ_IRQn, true);
    se_services_irq_config(MHU_RTSS_S_TX_IRQ_IRQn, true);

    // Send heartbeat services requests until one succeeds.
    SERVICES_synchronize_with_se(se_sess_handle);
}

void se_services_deinit(void) {
    // Disable SESS MHU channel IRQs.
    se_services_irq_config(MHU_SESS_S_RX_IRQ_IRQn, false);
    se_services_irq_config(MHU_SESS_S_TX_IRQ_IRQn, false);

    // Disable RTSS MHU channel IRQs.
    se_services_irq_config(MHU_RTSS_S_RX_IRQ_IRQn, false);
    se_services_irq_config(MHU_RTSS_S_TX_IRQ_IRQn, false);
}

void se_services_dump_device_data(void) {
    uint32_t error_code;

    uint8_t revision[80];
    SERVICES_get_se_revision(se_sess_handle, revision, &error_code);

    SERVICES_version_data_t data;
    SERVICES_system_get_device_data(se_sess_handle, &data, &error_code);

    printf("SE revision: %s\n", revision);
    printf("ALIF_PN:     %s\n", data.ALIF_PN);
    printf("Raw device data:\n");
    for (int i = 0; i < sizeof(data); ++i) {
        printf(" %02x", ((uint8_t *)&data)[i]);
        if (i % 16 == 15) {
            printf("\n");
        }
    }
    printf("\n");
}

void se_services_get_unique_id(uint8_t id[8]) {
    uint32_t error_code;
    SERVICES_system_get_eui_extension(se_sess_handle, false, id, &error_code);
}

__attribute__((noreturn)) void se_services_reset_soc(void) {
    SERVICES_boot_reset_soc(se_sess_handle);
    NVIC_SystemReset();
}

uint64_t se_services_rand64(void) {
    // If the SE core is not ready then the return value can be
    // SERVICES_REQ_NOT_ACKNOWLEDGE.  So retry a few times.
    for (int retry = 0; retry < 100; ++retry) {
        uint64_t value;
        int32_t error_code;
        uint32_t ret = SERVICES_cryptocell_get_rnd(se_sess_handle, sizeof(uint64_t), &value, &error_code);
        if (ret == SERVICES_REQ_SUCCESS) {
            return value;
        }
    }

    // No random number available.
    return 0;
}

uint32_t se_services_notify(void) {
    uint32_t ret = SERVICES_send_msg(se_rtss_handle, LocalToGlobal(0));
    if (ret != SERVICES_REQ_SUCCESS) {
        return -1;
    }
    return 0;
}

uint32_t se_services_enable_clock(clock_enable_t clock, bool enable) {
    uint32_t error_code;
    SERVICES_clocks_enable_clock(se_sess_handle, clock, enable, &error_code);
    return error_code;
}

uint32_t se_services_select_pll_source(pll_source_t source, pll_target_t target) {
    uint32_t error_code;
    SERVICES_clocks_select_pll_source(se_sess_handle, source, target, &error_code);
    return error_code;
}

uint32_t se_services_get_run_profile(run_profile_t *profile) {
    uint32_t error_code;
    SERVICES_get_run_cfg(se_sess_handle, profile, &error_code);
    return error_code;
}

uint32_t se_services_set_run_profile(run_profile_t *profile) {
    uint32_t error_code;
    SERVICES_set_run_cfg(se_sess_handle, profile, &error_code);
    return error_code;
}

uint32_t se_services_get_off_profile(off_profile_t *profile) {
    uint32_t error_code;
    SERVICES_get_off_cfg(se_sess_handle, profile, &error_code);
    return error_code;
}

uint32_t se_services_set_off_profile(off_profile_t *profile) {
    uint32_t error_code;
    SERVICES_set_off_cfg(se_sess_handle, profile, &error_code);
    return error_code;
}

uint32_t se_services_boot_process_toc_entry(const uint8_t *image_id) {
    uint32_t error_code;
    SERVICES_boot_process_toc_entry(se_sess_handle, image_id, &error_code);
    return error_code;
}

uint32_t se_services_boot_cpu(uint32_t cpu_id, uint32_t address) {
    uint32_t error_code;
    SERVICES_boot_cpu(se_sess_handle, cpu_id, address, &error_code);
    return error_code;
}

uint32_t se_services_boot_reset_cpu(uint32_t cpu_id) {
    uint32_t error_code;
    if (HE_CPU->RST_CTRL & RST_CTRL_CPUWAIT_MASK) {
        // CPU held in reset
        return SERVICES_REQ_SUCCESS;
    }

    for (mp_uint_t start = mp_hal_ticks_ms(); ; mp_hal_delay_ms(1)) {
        uint32_t ret = SERVICES_boot_reset_cpu(se_sess_handle, cpu_id, &error_code);
        if (ret != SERVICES_REQ_SUCCESS) {
            return error_code;
        }

        if ((HE_CPU->RST_STAT & RST_STAT_RST_ACK_MASK) == 0x4) {
            return SERVICES_REQ_SUCCESS;
        }

        if ((mp_hal_ticks_ms() - start) >= 100) {
            return SERVICES_REQ_TIMEOUT;
        }
    }

    return error_code;
}

uint32_t se_services_boot_release_cpu(uint32_t cpu_id) {
    uint32_t error_code;
    SERVICES_boot_release_cpu(se_sess_handle, cpu_id, &error_code);
    return error_code;
}
