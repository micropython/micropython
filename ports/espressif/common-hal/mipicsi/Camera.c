// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/gc.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/nlr.h"
#include "py/runtime.h"

#include "common-hal/mipicsi/Camera.h"
#include "shared-bindings/mipicsi/Camera.h"
#include "shared-bindings/mipicsi/__init__.h"
#include "bindings/espidf/__init__.h"

#include "common-hal/mipidsi/Bus.h"

#include <string.h>
#include "esp_attr.h"
#include "esp_cache.h"
#include "esp_cam_ctlr.h"
#include "esp_private/esp_cache_private.h"
#include "esp_cam_ctlr_csi.h"
#include "hal/color_hal.h"

// MIPI CSI receives data from a fixed, dedicated set of pins, so no pin
// configuration or claiming is done here.

#define ISP_CLK_HZ (80 * 1000 * 1000)

static cam_ctlr_color_t mipicsi_color_for(mipicsi_pixel_format_t format) {
    switch (format) {
        case MIPICSI_FORMAT_RAW8:
            return CAM_CTLR_COLOR_RAW8;
        case MIPICSI_FORMAT_RAW10:
            return CAM_CTLR_COLOR_RAW10;
        case MIPICSI_FORMAT_RAW12:
            return CAM_CTLR_COLOR_RAW12;
        case MIPICSI_FORMAT_RGB888:
            return CAM_CTLR_COLOR_RGB888;
        case MIPICSI_FORMAT_YUV422:
            return CAM_CTLR_COLOR_YUV422_YUYV;
        case MIPICSI_FORMAT_GRAYSCALE:
            return CAM_CTLR_COLOR_GRAY8;
        case MIPICSI_FORMAT_RGB565:
        default:
            return CAM_CTLR_COLOR_RGB565;
    }
}

static mp_int_t mipicsi_bits_per_pixel_for(mipicsi_pixel_format_t format) {
    switch (format) {
        case MIPICSI_FORMAT_RAW10:
            return 10;
        case MIPICSI_FORMAT_RAW12:
            return 12;
        case MIPICSI_FORMAT_RGB888:
            return 24;
        case MIPICSI_FORMAT_RGB565:
        case MIPICSI_FORMAT_YUV422:
            return 16;
        case MIPICSI_FORMAT_RAW8:
        case MIPICSI_FORMAT_GRAYSCALE:
        default:
            return 8;
    }
}

#if SOC_ISP_SUPPORTED
// The ISP color types share their FOURCC values with the CAM_CTLR color
// types, except for GRAYSCALE (no ISP grey mode) and YUV422 (the ISP input is
// UYVY, the CAM_CTLR YUYV). Only sensor RAW formats convert here.
static isp_color_t mipicsi_isp_color_for(mipicsi_pixel_format_t format) {
    switch (format) {
        case MIPICSI_FORMAT_RAW8:
            return ISP_COLOR_RAW8;
        case MIPICSI_FORMAT_RAW10:
            return ISP_COLOR_RAW10;
        case MIPICSI_FORMAT_RAW12:
            return ISP_COLOR_RAW12;
        case MIPICSI_FORMAT_RGB565:
            return ISP_COLOR_RGB565;
        case MIPICSI_FORMAT_RGB888:
            return ISP_COLOR_RGB888;
        case MIPICSI_FORMAT_YUV422:
        case MIPICSI_FORMAT_GRAYSCALE:
        default:
            return 0;
    }
}
#endif

// Called by the driver (in ISR context) to get the next buffer to DMA into.
static IRAM_ATTR bool mipicsi_on_get_new_trans(
    esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data) {
    mipicsi_camera_obj_t *self = user_data;

    // DMA the whole frame into the current destination; a change is picked
    // up on the next frame. The controller only runs while a destination is
    // set, so dest_base is never NULL here.
    trans->buffer = (void *)self->dest_base;
    trans->buflen = self->buffer_len;
    return false;
}

// Called by the driver (in ISR context) when a frame has been written.
static IRAM_ATTR bool mipicsi_on_trans_finished(
    esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data) {
    mipicsi_camera_obj_t *self = user_data;

    // Report where the frame actually landed: after a destination change
    // (or a pause) the frame that was in flight lands at the previous one.
    if (self->dest_base != NULL && self->dest_on_frame != NULL) {
        self->dest_on_frame(self->dest_ctx, trans->buffer);
    }
    return false;
}

void common_hal_mipicsi_camera_construct(
    mipicsi_camera_obj_t *self,
    mipicsi_pixel_format_t output_format) {

    self->width = 1;
    self->height = 1;
    self->pixel_format = output_format;
    self->output_format = output_format;
    self->dest_base = NULL;
    self->dest_on_frame = NULL;
    self->dest_ctx = NULL;
    self->ctlr_started = false;
    self->paused = false;
    self->sensor = mp_const_none;
    self->phy_ldo = NULL;
    self->isp_proc = NULL;
    self->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    // No receiver hardware yet: it is created in the videosource play step,
    // where the sensor's current mode is known.
}

void common_hal_mipicsi_camera_videosource_play(
    mipicsi_camera_obj_t *self,
    const mipicsi_sensor_params_t *params,
    bool paused) {

    if (self->handle != NULL) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("already playing"));
    }

    // pixel_format is reread from the sensor at every play, so validate the
    // conversion here.
    #if SOC_ISP_SUPPORTED
    if (self->output_format != params->pixel_format) {
        // The ISP only converts from RAW. Other formats must be passed
        // through unconverted.
        const isp_color_t isp_in = mipicsi_isp_color_for(params->pixel_format);
        if (isp_in != ISP_COLOR_RAW8 && isp_in != ISP_COLOR_RAW10 && isp_in != ISP_COLOR_RAW12) {
            mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("unsupported %q type"), MP_QSTR_pixel_format);
        }
        const isp_color_t isp_out = mipicsi_isp_color_for(self->output_format);
        if (isp_out != ISP_COLOR_RGB565 && isp_out != ISP_COLOR_RGB888) {
            mp_raise_ValueError(MP_ERROR_TEXT("output_format must be RGB565 or RGB888"));
        }
    }
    #else
    if (self->output_format != params->pixel_format) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("ISP conversion not supported on this chip"));
    }
    #endif

    self->paused = paused;

    // Adopt the sensor's current mode.
    self->pixel_format = params->pixel_format;
    self->width = params->width;
    self->height = params->height;
    self->bayer_order = params->bayer_order;
    self->data_lanes = params->data_lanes;
    self->lane_bit_rate_mbps = params->lane_bit_rate_mbps;

    const cam_ctlr_color_t color = mipicsi_color_for(self->pixel_format);

    #if SOC_ISP_SUPPORTED
    if (self->output_format != self->pixel_format) {
        // The CSI bridge on gen-1 P4 silicon can't convert, so the ISP does
        // (as in the IDF esp_driver_cam csi test).
        const isp_color_t isp_in = mipicsi_isp_color_for(self->pixel_format);
        const isp_color_t isp_out = mipicsi_isp_color_for(self->output_format);
        esp_isp_processor_cfg_t isp_config = {
            .clk_hz = ISP_CLK_HZ,
            .input_data_source = ISP_INPUT_DATA_SOURCE_CSI,
            .input_data_color_type = isp_in,
            .output_data_color_type = isp_out,
            .has_line_start_packet = false,
            .has_line_end_packet = false,
            .h_res = self->width,
            .v_res = self->height,
            .bayer_order = (color_raw_element_order_t)self->bayer_order,
        };
        CHECK_ESP_RESULT(esp_isp_new_processor(&isp_config, &self->isp_proc));
    }
    #endif

    // Power the MIPI D-PHY. The LDO is shared with mipidsi (see
    // mipidsi_phy_ldo_acquire).
    self->phy_ldo = mipidsi_phy_ldo_acquire();

    // The CSI bridge doesn't convert (input and output color match); the ISP
    // does. bits_per_pixel is the output depth so the driver sizes the DMA
    // for the converted frame, not the smaller RAW input.
    esp_cam_ctlr_csi_config_t csi_config = {
        .ctlr_id = 0,
        .h_res = self->width,
        .v_res = self->height,
        .data_lane_num = self->data_lanes,
        .lane_bit_rate_mbps = self->lane_bit_rate_mbps,
        .input_data_color_type = color,
        .output_data_color_type = color,
        .data_type = {
            .bits_per_pixel = mipicsi_bits_per_pixel_for(self->output_format),
            .data_type_min = 0x12,
            .data_type_max = 0x2f,
        },
        // Frames go straight to the destination and the controller only runs
        // while one is set, so skip the driver's frame-sized backup buffer.
        .queue_items = 1,
        .bk_buffer_dis = true,
    };

    esp_err_t result = esp_cam_new_csi_ctlr(&csi_config, &self->handle);
    if (result != ESP_OK) {
        mipidsi_phy_ldo_release();
        self->phy_ldo = NULL;
        raise_esp_error(result);
    }

    // Unpadded frame size. set_destination() replaces it with the padded
    // length before the controller starts.
    self->buffer_len = ((size_t)self->width * self->height * mipicsi_bits_per_pixel_for(self->output_format) + 7) / 8;

    esp_cam_ctlr_evt_cbs_t cbs = {
        .on_get_new_trans = mipicsi_on_get_new_trans,
        .on_trans_finished = mipicsi_on_trans_finished,
    };
    result = esp_cam_ctlr_register_event_callbacks(self->handle, &cbs, self);
    if (result == ESP_OK) {
        result = esp_cam_ctlr_enable(self->handle);
    }
    #if SOC_ISP_SUPPORTED
    if (result == ESP_OK && self->isp_proc) {
        result = esp_isp_enable(self->isp_proc);
    }
    #endif
    if (result != ESP_OK) {
        common_hal_mipicsi_camera_deinit(self);
        raise_esp_error(result);
    }

    // Start the sensor so frames are ready once a destination is set.
    // Sensors' _start() is expected to be idempotent.
    if (self->sensor != mp_const_none) {
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            mp_obj_t start_method[2];
            mp_load_method(self->sensor, MP_QSTR__start, start_method);
            mp_call_method_n_kw(0, 0, start_method);
            nlr_pop();
        } else {
            common_hal_mipicsi_camera_deinit(self);
            nlr_raise(nlr.ret_val);
        }
    }

    // The controller starts in set_destination(), once frames have somewhere
    // to go.
}

// Turn off the CSI receiver and ISP. Must not raise: this also runs from
// background contexts such as reload. The sensor is left alone.
static void mipicsi_camera_teardown(mipicsi_camera_obj_t *self) {
    // Always stop, even when paused: a pause's stop finishes in the
    // background, and it must be complete before the controller is disabled.
    // esp_cam_ctlr_stop waits for it (and is a no-op once stopped).
    if (self->handle != NULL) {
        esp_cam_ctlr_stop(self->handle);
    }
    self->ctlr_started = false;
    portENTER_CRITICAL(&self->spinlock);
    self->dest_base = NULL;
    self->dest_on_frame = NULL;
    self->dest_ctx = NULL;
    self->paused = false;
    portEXIT_CRITICAL(&self->spinlock);

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        if (self->handle != NULL) {
            esp_cam_ctlr_disable(self->handle);
            esp_cam_ctlr_del(self->handle);
            self->handle = NULL;
        }
        #if SOC_ISP_SUPPORTED
        if (self->isp_proc) {
            esp_isp_disable(self->isp_proc);
            esp_isp_del_processor(self->isp_proc);
            self->isp_proc = NULL;
        }
        #endif
        mipidsi_phy_ldo_release();
        self->phy_ldo = NULL;
        nlr_pop();
    } else {
        // Nothing to do about a failed teardown, not even a partially
        // released one; swallow the exception and continue.
    }
}

void common_hal_mipicsi_camera_set_sensor(mipicsi_camera_obj_t *self, mp_obj_t sensor) {
    self->sensor = sensor;
}

mp_obj_t common_hal_mipicsi_camera_get_sensor(mipicsi_camera_obj_t *self) {
    return self->sensor;
}

// Sensors' _stop() is expected to be idempotent.
static void mipicsi_camera_stop_sensor(mipicsi_camera_obj_t *self) {
    mp_obj_t stop_method[2];
    mp_load_method(self->sensor, MP_QSTR__stop, stop_method);
    mp_call_method_n_kw(0, 0, stop_method);
}

void common_hal_mipicsi_camera_deinit(mipicsi_camera_obj_t *self) {
    if (self->handle != NULL) {
        mipicsi_camera_teardown(self);
        // Stop the sensor too, but only if it is still allocated: from the GC
        // sweep, it may already have been freed. Ignore errors since this
        // must not raise.
        if (mp_obj_is_obj(self->sensor) && gc_nbytes(MP_OBJ_TO_PTR(self->sensor)) > 0) {
            nlr_buf_t nlr;
            if (nlr_push(&nlr) == 0) {
                mipicsi_camera_stop_sensor(self);
                nlr_pop();
            }
        }
    }
    // Clearing the sensor marks the camera deinitialised.
    self->sensor = mp_const_none;
}

void common_hal_mipicsi_camera_videosource_stop(mipicsi_camera_obj_t *self) {
    if (self->handle == NULL) {
        return; // Not playing.
    }

    // The receiver first: after the CSI controller stops, no more frames are
    // captured, whatever the sensor does.
    mipicsi_camera_teardown(self);

    // Stop the sensor (e.g. the sc2336 stops streaming and unlocks its
    // mode).
    if (self->sensor != mp_const_none) {
        mipicsi_camera_stop_sensor(self);
    }
}

void common_hal_mipicsi_camera_videosource_pause(mipicsi_camera_obj_t *self) {
    if (self->handle == NULL) {
        return; // Not playing.
    }

    // Keep the ISP running so its AE/AWB state survives; stopping the CSI
    // controller is enough to keep frames out of memory. Stop at the frame
    // end without waiting: that last frame still lands and is reported
    // through on_frame so the target can repaint over it.
    if (self->ctlr_started) {
        // Only fails when not started, which ctlr_started rules out.
        (void)esp_cam_ctlr_csi_stop_at_frame_end(self->handle);
        self->ctlr_started = false;
    }
    self->paused = true;
}

// May raise; paused stays set if the controller can't start.
void common_hal_mipicsi_camera_videosource_resume(mipicsi_camera_obj_t *self) {
    if (self->handle == NULL) {
        return; // Not playing.
    }

    if (!self->ctlr_started && self->dest_base != NULL) {
        // Starting while the pause's stop is still pending cancels it; the
        // frame in flight still lands at the destination from pause time and
        // on_frame reports it there.
        CHECK_ESP_RESULT(esp_cam_ctlr_start(self->handle));
        self->ctlr_started = true;
    }
    self->paused = false;
}

bool common_hal_mipicsi_camera_deinited(mipicsi_camera_obj_t *self) {
    return self->sensor == mp_const_none;
}

bool common_hal_mipicsi_camera_set_destination(
    mipicsi_camera_obj_t *self,
    uint8_t *base,
    size_t stride,
    void (*on_frame)(void *ctx, const void *frame),
    void *ctx) {
    const size_t row_bytes = ((size_t)self->width * mipicsi_bits_per_pixel_for(self->output_format) + 7) / 8;
    const size_t frame_bytes = (size_t)self->height * row_bytes;

    // The videosource protocol never passes NULL.
    assert(base != NULL);

    // The driver's per-frame esp_cache_msync needs a cache-line aligned
    // buffer. PSRAM GC allocations already are (64-byte GC blocks), so this
    // is a safety net.
    size_t cache_line = 0;
    esp_cache_get_alignment(MALLOC_CAP_SPIRAM, &cache_line);
    if (cache_line == 0) {
        cache_line = 64;
    }
    if ((uintptr_t)base % cache_line != 0) {
        return false;
    }

    // The CSI DMA only does contiguous transfers, so rows can't be padded.
    // The caller has already checked that the frame fits.
    if (stride != row_bytes) {
        return false;
    }

    // Size handed to the driver: the frame, cache-line padded at the end so
    // the driver's per-frame sync is happy.
    const size_t buffer_len = (frame_bytes + cache_line - 1) / cache_line * cache_line;

    portENTER_CRITICAL(&self->spinlock);
    self->dest_base = base;
    self->dest_on_frame = on_frame;
    self->dest_ctx = ctx;
    self->buffer_len = buffer_len;
    portEXIT_CRITICAL(&self->spinlock);

    // The sensor streaming and the receiver hardware exist from play();
    // here only the controller runs or doesn't. While paused, just remember
    // the new destination: resume() starts the controller there.
    if (!self->paused && !self->ctlr_started) {
        esp_err_t result = esp_cam_ctlr_start(self->handle);
        if (result != ESP_OK) {
            return false;
        }
        self->ctlr_started = true;
    }
    return true;
}

mipicsi_pixel_format_t common_hal_mipicsi_camera_get_pixel_format(mipicsi_camera_obj_t *self) {
    return self->pixel_format;
}

mipicsi_pixel_format_t common_hal_mipicsi_camera_get_output_format(mipicsi_camera_obj_t *self) {
    return self->output_format;
}

mp_int_t common_hal_mipicsi_camera_get_width(mipicsi_camera_obj_t *self) {
    return self->width;
}

mp_int_t common_hal_mipicsi_camera_get_height(mipicsi_camera_obj_t *self) {
    return self->height;
}

mp_int_t common_hal_mipicsi_camera_get_bits_per_pixel(mipicsi_camera_obj_t *self) {
    return mipicsi_bits_per_pixel_for(self->output_format);
}
