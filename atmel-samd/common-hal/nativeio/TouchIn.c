/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#include <string.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/binary.h"
#include "py/mphal.h"
#include "shared-bindings/nativeio/TouchIn.h"

#include "tick.h"

#include "QTouch/touch_api_ptc.h"

#define   DEF_SELFCAP_CAL_SEQ1_COUNT                8
#define   DEF_SELFCAP_CAL_SEQ2_COUNT                4
#define DEF_SELFCAP_CC_CAL_CLK_PRESCALE_PER_NODE DEF_SELFCAP_CLK_PRESCALE_PER_NODE
#define DEF_SELFCAP_CC_CAL_SENSE_RESISTOR_PER_NODE DEF_SELFCAP_SENSE_RESISTOR_PER_NODE
#define   DEF_SELFCAP_QUICK_REBURST_ENABLE          1u

/**
 * Mutual Cap sensors measured data pointer.
 * Note: This pointer is initialized by the QTouch library once the
 * touch_mutlcap_sensors_init API is called. */
touch_measure_data_t *p_mutlcap_measure_data = NULL;

/**
 * Self Cap sensors measured data pointer.
 * Note: This pointer is initialized by the QTouch library once the
 * touch_selfcap_sensors_init API is called. */
touch_measure_data_t *p_selfcap_measure_data = NULL;


/**
 * Self Cap sensors data block provided as input to Touch library.
 */
static uint8_t selfcap_data_blk[PRIV_SELFCAP_DATA_BLK_SIZE];

/**
 * Self Cap sensors Pins Info.
 */

uint32_t selfcap_y_nodes[DEF_SELFCAP_NUM_CHANNELS] = {DEF_SELFCAP_LINES};

filter_level_t selfcap_filter_level_per_node[DEF_SELFCAP_NUM_CHANNELS] = {DEF_SELFCAP_FILTER_LEVEL_PER_NODE};

uint8_t selfcap_auto_os_per_node[DEF_SELFCAP_NUM_CHANNELS]= {DEF_SELFCAP_AUTO_OS_PER_NODE};

gain_t selfcap_gain_per_node[DEF_SELFCAP_NUM_CHANNELS] = {DEF_SELFCAP_GAIN_PER_NODE};

rsel_val_t selfcap_resistor_per_node[DEF_SELFCAP_NUM_CHANNELS] = {DEF_SELFCAP_SENSE_RESISTOR_PER_NODE};

prsc_div_sel_t selfcap_prsc_per_node[DEF_SELFCAP_NUM_CHANNELS] = {DEF_SELFCAP_CLK_PRESCALE_PER_NODE};

prsc_div_sel_t selfcap_boot_prsc_per_node[DEF_SELFCAP_NUM_CHANNELS] = {DEF_SELFCAP_CC_CAL_CLK_PRESCALE_PER_NODE};

rsel_val_t selfcap_boot_resistor_per_node[DEF_SELFCAP_NUM_CHANNELS] = { DEF_SELFCAP_CC_CAL_SENSE_RESISTOR_PER_NODE};

freq_hop_sel_t selfcap_freq_hops[3u] = {DEF_SELFCAP_HOP_FREQS};

/**
 * Self Cap Configuration structure provided as input to Touch Library.
 *
 * Note: Use the touch.h configuration header file to fill in
 * the elements of this structure.  DO NOT modify any of the input values
 * directly in this structure.
 */
touch_selfcap_config_t selfcap_config = {
    0,           /* Self Cap number of channels. */
    0,                                  /* Self Cap number of sensors. */
    DEF_SELFCAP_NUM_ROTORS_SLIDERS,     /* Self Cap number of rotors and
                                         * sliders. */

    /* Self Cap GLOBAL SENSOR CONFIGURATION INFO. */
    {
        DEF_SELFCAP_DI,                 /* uint8_t  di; Sensor detect
                                         * integration (DI) limit. */

        /* Interchanging Negative and Positive Drift rate, since Signal
         * increases on Touch. */
        DEF_SELFCAP_ATCH_DRIFT_RATE,    /* uint8_t  neg_drift_rate;
                                         * Sensor
                                         * negative drift rate. */
        DEF_SELFCAP_TCH_DRIFT_RATE,     /* uint8_t  pos_drift_rate;
                                         * Sensor
                                         * positive drift rate. */
        DEF_SELFCAP_MAX_ON_DURATION,    /* uint8_t  max_on_duration;
                                         * Sensor
                                         * maximum on duration. */
        DEF_SELFCAP_DRIFT_HOLD_TIME,    /* uint8_t  drift_hold_time;
                                         * Sensor
                                         * drift hold time. */
        DEF_SELFCAP_ATCH_RECAL_DELAY,   /* uint8_t  pos_recal_delay;
                                         * Sensor positive recalibration
                                         * delay. */

        DEF_SELFCAP_CAL_SEQ1_COUNT,
        DEF_SELFCAP_CAL_SEQ2_COUNT,
        DEF_SELFCAP_ATCH_RECAL_THRESHOLD, /* recal_threshold_t
                                           * recal_threshold; Sensor
                                           * recalibration threshold. */
        DEF_SELFCAP_TOUCH_POSTPROCESS_MODE,
        DEF_SELFCAP_AUTO_OS_SIGNAL_STABILITY_LIMIT,
        DEF_SELFCAP_FREQ_AUTO_TUNE_SIGNAL_STABILITY_LIMIT,
        DEF_SELFCAP_FREQ_AUTO_TUNE_IN_CNT,
        DEF_SELFCAP_NOISE_MEAS_SIGNAL_STABILITY_LIMIT, /* signal
                                                        *stability */
        DEF_SELFCAP_NOISE_LIMIT,
        DEF_SELFCAP_LOCKOUT_SEL,
        DEF_SELFCAP_LOCKOUT_CNTDOWN,
        #if ((SAMC20) || (SAMC21) || (SAML22))
        DEF_SELF_CAP_CSD_VALUE,
        #endif
    },
    {
        selfcap_gain_per_node,      /* Self Cap channel gain setting. */
        DEF_SELFCAP_FREQ_MODE,      /* Self Cap noise counter measure
                                     * enable/disable. */
        selfcap_prsc_per_node,
        selfcap_resistor_per_node,
        selfcap_boot_prsc_per_node,
        selfcap_boot_resistor_per_node,
        selfcap_freq_hops,
        selfcap_filter_level_per_node,   /* Self Cap filter level setting. */
        selfcap_auto_os_per_node,        /* Self Cap auto oversamples
                                     *setting. */
    },
    selfcap_data_blk,                   /* Self Cap data block index. */
    PRIV_SELFCAP_DATA_BLK_SIZE,         /* Self Cap data block size. */
    selfcap_y_nodes,                    /* Self Cap channel nodes. */
    DEF_SELFCAP_QUICK_REBURST_ENABLE,
    DEF_SELFCAP_FILTER_CALLBACK,         /* Self Cap filter callback
                                          * function
                                          * pointer. */
    DEF_SELFCAP_FREQ_AUTO_TUNE_ENABLE,
    DEF_SELFCAP_NOISE_MEAS_ENABLE,
    DEF_SELFCAP_NOISE_MEAS_BUFFER_CNT,
    DEF_SELFCAP_MOIS_TOLERANCE_ENABLE,
    DEF_SELFCAP_NUM_MOIS_GROUPS,
    DEF_SELFCAP_MOIS_QUICK_REBURST_ENABLE,
    DEF_SELFCAP_PTC_GPIO_STATE,
      .tlib_feature_list ={
          .lk_chk = DEF_LOCKOUT_FUNC,
          .auto_os_init = DEF_AUTO_OS_FUNC,//auto_os_resolve,
          .auto_tune_init = DEF_SELF_AUTO_TUNE_FUNC,// self_auto_tuning
          .enable_aks = DEF_AKS_FUNC, /*AKS grouping function */
      }
};

/**
 * Touch Library input configuration structure.
 */
touch_config_t touch_config = {
    NULL,
    &selfcap_config,                /* Pointer to Self Cap configuration
                                     * structure. */
    DEF_TOUCH_PTC_ISR_LVL,          /* PTC interrupt level. */
};

nativeio_touchin_obj_t *active_touchin_obj[DEF_SELFCAP_NUM_CHANNELS];

void common_hal_nativeio_touchin_construct(nativeio_touchin_obj_t* self,
        const mcu_pin_obj_t *pin) {
    if (!pin->has_touch) {
        // No ADC function on that pin
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "pin %q does not have touch capabilities", pin->name));
    }

    if (selfcap_config.num_channels > 0) {
        // Deinit the touch sensor, we're going to reinitialize it.
        touch_selfcap_sensors_deinit();

        // Remove holes in the active list from sensors that may have been
        // disabled.
        int first_hole = -1;
        for (int i = 0; i < selfcap_config.num_channels; i++) {
            if (active_touchin_obj[i] == NULL && first_hole == -1) {
                first_hole = i;
            } else if (active_touchin_obj[i] != NULL && first_hole != -1) {
                active_touchin_obj[first_hole] = active_touchin_obj[i];
                first_hole = i;
            }
        }
        if (first_hole > -1) {
            selfcap_config.num_channels = first_hole;
            selfcap_config.num_sensors = first_hole;
        }
    }

    // Add our sensor to the end of the list.
    self->pin = pin;
    selfcap_y_nodes[selfcap_config.num_channels++] = Y(pin->touch_y_line);
    active_touchin_obj[selfcap_config.num_sensors++] = self;

    /* Setup and enable generic clock source for PTC module. */
    struct system_gclk_chan_config gclk_chan_conf;
    system_gclk_chan_get_config_defaults(&gclk_chan_conf);
    gclk_chan_conf.source_generator = GCLK_GENERATOR_3;
    system_gclk_chan_set_config(PTC_GCLK_ID, &gclk_chan_conf);
    system_gclk_chan_enable(PTC_GCLK_ID);
    system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBC, PM_APBCMASK_PTC);

    touch_ret_t status;
    /* Initialize touch library for Self Cap operation. */
    status = touch_selfcap_sensors_init_with_rs_table(&touch_config,
            PRIV_SELFCAP_RS_TABLE_INIT, PRIV_NM_TABLE_INIT,
            PRIV_FREQ_AUTO_TUNE_CHK, PRIV_MOIS_TOLERANCE_CHK);
    if (status != TOUCH_SUCCESS) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Touch init failed (%d)", status));
    }
    for (int i = 0; i < selfcap_config.num_channels; i++) {
        status = touch_selfcap_sensor_config(SENSOR_TYPE_KEY, i, i,
            NO_AKS_GROUP, 10u, HYST_25, RES_8_BIT, &active_touchin_obj[i]->sensor_id);
        if (status != TOUCH_SUCCESS) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Touch pad config failed (%d)", status));
        }
    }
    status = touch_selfcap_sensors_calibrate(AUTO_TUNE_RSEL);
    if (status != TOUCH_SUCCESS) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Touch pad calibration failed (%d)", status));
    }

    // Run a measurement to get calibrated.
    common_hal_nativeio_touchin_get_value(self);
}

// This may get called twice, once on exit or deinit and once from the finaliser
// (__del__). We need the finaliser to reduce the chance of us writing to memory
// we no longer own because we keep track of Python pointers in active_touchin_obj.
void common_hal_nativeio_touchin_deinit(nativeio_touchin_obj_t* self) {
    touch_selfcap_sensor_disable(self->sensor_id);
    // Remove ourselves from the list of active objects. We don't change the
    // selfcap settings at all because we don't want to reinit now. Instead,
    // we'll collect blank spots later on reinit.
    for (int i = 0; i < DEF_SELFCAP_NUM_CHANNELS; i++) {
        if (active_touchin_obj[i] == self) {
            active_touchin_obj[i] = NULL;
            break;
        }
    }
}

volatile bool touch_read_ready = false;
volatile touch_acq_status_t  touch_acq_status;

void touch_selfcap_measure_complete_callback(void)
{
    touch_read_ready = true;
    touch_acq_status = p_selfcap_measure_data->acq_status;
}

bool common_hal_nativeio_touchin_get_value(nativeio_touchin_obj_t *self) {
    if (p_selfcap_measure_data->acq_status & TOUCH_CC_CALIB_ERROR) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Touch calibration error"));
    }
    touch_acq_status = TOUCH_BURST_AGAIN;
    uint64_t start_ticks = ticks_ms;
    while((touch_acq_status & TOUCH_BURST_AGAIN) && ticks_ms - start_ticks < 1000) {
        touch_read_ready = false;
        touch_ret_t touch_ret = touch_selfcap_sensors_measure(
                ticks_ms,
                NORMAL_ACQ_MODE,
                touch_selfcap_measure_complete_callback);

        if (touch_ret != TOUCH_SUCCESS) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Touch measure failed"));
        }

        while(!touch_read_ready && ticks_ms - start_ticks < 1000) {
            // wait
            #ifdef MICROPY_VM_HOOK_LOOP
                MICROPY_VM_HOOK_LOOP
            #endif
        }
    }

    if (touch_acq_status & TOUCH_BURST_AGAIN) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Touch read failed"));
    }

    return (p_selfcap_measure_data->p_sensor_states[self->sensor_id / 8] & (1 << (self->sensor_id % 8))) != 0;
}
