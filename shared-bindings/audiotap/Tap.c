// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "shared-bindings/audiotap/Tap.h"
#include "shared-bindings/audiocore/__init__.h"

#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/util.h"

//| class Tap:
//|     """Passes audio through unchanged and keeps a copy that the program can read"""
//|
//|     def __init__(self, capacity: float = 0.1) -> None:
//|         """Create a Tap. It plays a sample like the effects in `audiofilters`
//|         and `audiodelays` do, so it can go anywhere in an audio chain. Whatever
//|         plays the tap gets the sample's audio unchanged.
//|
//|         As the audio passes through, the tap keeps up to ``capacity`` seconds
//|         of it in a buffer, which `readinto` copies out of without waiting, and
//|         measures its `peak` and `rms` levels. The program can look at
//|         the audio this way, at the point in the chain where the tap is,
//|         while it plays.
//|
//|         The tap takes its sample rate, channel count, bits per sample and
//|         signedness from the first sample it plays, and allocates the buffer
//|         then. So `play` a sample with the tap before anything plays the tap.
//|
//|         The tap only sees audio that is played through it: something, such
//|         as an `audiomixer.Mixer` voice playing to an audio output, must play
//|         it or a chain that it is part of. To look at audio without hearing
//|         it, play it at a ``level`` of 0. When its sample is stopped or ends,
//|         the tap plays silence and keeps nothing.
//|
//|         When the program reads too slowly, the buffer fills up, the tap keeps
//|         the audio it has and drops the newer audio until there is room
//|         again, and `overflow` turns True. The audio that passes through is
//|         never affected.
//|
//|         :param float capacity: How many seconds of audio are kept for
//|           `readinto`. 0 keeps none, for a tap that only measures `peak` and
//|           `rms`.
//|
//|         Showing the level of a USB microphone while it is heard through the
//|         DVI output, and keeping the audio for a spectrum display. The
//|         ``adafruit_usb_host_descriptors`` library finds the microphone and
//|         its format, and `usb.core` starts its stream; see
//|         `usb_host_audio.USBIn`. DVI audio plays at 32, 44.1 or 48 kHz, so an
//|         `audiospeed.Resampler` converts other sample rates::
//|
//|           import array
//|           import time
//|
//|           import adafruit_usb_host_descriptors
//|           import audiomixer
//|           import audiospeed
//|           import audiotap
//|           import displayio
//|           import supervisor
//|           import usb_host_audio
//|           import vectorio
//|
//|           device = adafruit_usb_host_descriptors.find_microphone()
//|           if device is None:
//|               raise RuntimeError("Plug a USB microphone or webcam into the USB host port")
//|           formats = adafruit_usb_host_descriptors.find_microphone_formats(device)
//|           mic_format = adafruit_usb_host_descriptors.choose_microphone_format(formats, sample_rate=48000)
//|           sample_rate = mic_format.nearest_rate(48000)
//|
//|           device.set_configuration()
//|           # SET_INTERFACE to the format's alternate setting starts the stream, then
//|           # SET_CUR of the endpoint's sampling frequency picks the rate.
//|           device.ctrl_transfer(0x01, 11, mic_format.alternate, mic_format.interface, None)
//|           if mic_format.rate_control:
//|               device.ctrl_transfer(
//|                   0x22, 0x01, 0x0100, mic_format.endpoint, sample_rate.to_bytes(3, "little")
//|               )
//|
//|           mic = usb_host_audio.USBIn(
//|               device,
//|               mic_format.endpoint,
//|               sample_rate=sample_rate,
//|               channel_count=mic_format.channel_count,
//|               bit_depth=8 * mic_format.bytes_per_sample,
//|               samples_signed=mic_format.samples_signed,
//|               max_packet_size=mic_format.max_packet_size,
//|           )
//|           # Keep a quarter second, more than arrives between reads of the loop below.
//|           tap = audiotap.Tap(capacity=0.25)
//|           mixer = audiomixer.Mixer(voice_count=1, sample_rate=48000, channel_count=mic.channel_count)
//|           supervisor.runtime.audio.play(mixer)
//|           mixer.voice[0].play(audiospeed.Resampler(tap.play(mic)))
//|
//|           # Bars for the peak (top) and RMS (bottom) levels. Showing them hides the
//|           # console, whose scrolling stalls the microphone's stream. print() still
//|           # goes to the serial console.
//|           display = supervisor.runtime.display
//|           palette = displayio.Palette(1)
//|           palette[0] = 0x00FF00
//|           peak_bar = vectorio.Rectangle(pixel_shader=palette, width=1, height=40, x=0, y=60)
//|           rms_bar = vectorio.Rectangle(pixel_shader=palette, width=1, height=40, x=0, y=140)
//|           meter = displayio.Group()
//|           meter.append(peak_bar)
//|           meter.append(rms_bar)
//|           display.root_group = meter
//|
//|           samples = array.array("h", [0] * 1024)
//|           while True:
//|               # Read the audio that is waiting. A spectrum display would use these samples.
//|               while tap.available >= len(samples):
//|                   tap.readinto(samples)
//|               peak = tap.peak
//|               rms = tap.rms
//|               peak_bar.width = max(1, int(peak * display.width))
//|               rms_bar.width = max(1, int(rms * display.width))
//|               print(f"peak {peak:.3f} rms {rms:.3f}")
//|               time.sleep(0.1)"""
//|         ...
//|
static mp_obj_t audiotap_tap_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_capacity, };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_capacity, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_float_t capacity = (args[ARG_capacity].u_obj == MP_OBJ_NULL)
        ? MICROPY_FLOAT_CONST(0.1)
        : mp_obj_get_float(args[ARG_capacity].u_obj);
    mp_arg_validate_float_range(capacity, 0, 60, MP_QSTR_capacity);

    audiotap_tap_obj_t *self = mp_obj_malloc_with_finaliser(audiotap_tap_obj_t, &audiotap_tap_type);
    common_hal_audiotap_tap_construct(self, capacity);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialises the Tap."""
//|         ...
//|
static mp_obj_t audiotap_tap_deinit(mp_obj_t self_in) {
    audiotap_tap_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiotap_tap_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(audiotap_tap_deinit_obj, audiotap_tap_deinit);

static void check_for_deinit(audiotap_tap_obj_t *self) {
    audiosample_check_for_deinit(&self->base);
}

//|     def __enter__(self) -> Tap:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
//  Provided by context manager helper.

//|     playing: bool
//|     """True when the tap is playing a sample. (read-only)"""
//|
static mp_obj_t audiotap_tap_obj_get_playing(mp_obj_t self_in) {
    audiotap_tap_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_audiotap_tap_get_playing(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiotap_tap_get_playing_obj, audiotap_tap_obj_get_playing);

MP_PROPERTY_GETTER(audiotap_tap_playing_obj,
    (mp_obj_t)&audiotap_tap_get_playing_obj);

//|     available: int
//|     """The number of samples waiting to be read with `readinto`. (read-only)"""
//|
static mp_obj_t audiotap_tap_obj_get_available(mp_obj_t self_in) {
    audiotap_tap_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_int_from_uint(common_hal_audiotap_tap_get_available(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiotap_tap_get_available_obj, audiotap_tap_obj_get_available);

MP_PROPERTY_GETTER(audiotap_tap_available_obj,
    (mp_obj_t)&audiotap_tap_get_available_obj);

//|     overflow: bool
//|     """True if audio was dropped since the last time this was checked,
//|     because `readinto` did not keep up. Reading this clears it. (read-only)"""
//|
static mp_obj_t audiotap_tap_obj_get_overflow(mp_obj_t self_in) {
    audiotap_tap_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_audiotap_tap_get_overflow(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiotap_tap_get_overflow_obj, audiotap_tap_obj_get_overflow);

MP_PROPERTY_GETTER(audiotap_tap_overflow_obj,
    (mp_obj_t)&audiotap_tap_get_overflow_obj);

//|     def readinto(self, buffer: WriteableBuffer) -> int:
//|         """Copies the oldest audio waiting into ``buffer`` and returns the number
//|         of samples copied, which is 0 when none is waiting. Does not wait for
//|         audio. Copies whole frames only: a stereo frame is two samples, left
//|         then right.
//|
//|         The samples are copied as they are played, so ``buffer`` is
//|         usually an ``array.array`` of typecode ``'h'`` for 16 bit signed
//|         samples. A ``bytearray`` works for any format and holds half as
//|         many 16 bit samples as its length. To read into part of a buffer,
//|         pass a slice of a ``memoryview`` of it."""
//|         ...
//|
static mp_obj_t audiotap_tap_obj_readinto(mp_obj_t self_in, mp_obj_t buffer_in) {
    audiotap_tap_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buffer_in, &bufinfo, MP_BUFFER_WRITE);
    uint32_t length = bufinfo.len / (self->base.bits_per_sample / 8);
    return mp_obj_new_int_from_uint(common_hal_audiotap_tap_readinto(self, bufinfo.buf, length));
}
MP_DEFINE_CONST_FUN_OBJ_2(audiotap_tap_readinto_obj, audiotap_tap_obj_readinto);

//|     def clear(self) -> None:
//|         """Discards the audio waiting, so that `readinto` returns only audio
//|         that passes through after this. Use it to skip ahead to the live
//|         audio, for example after an `overflow`."""
//|         ...
//|
static mp_obj_t audiotap_tap_obj_clear(mp_obj_t self_in) {
    audiotap_tap_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_audiotap_tap_clear(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(audiotap_tap_clear_obj, audiotap_tap_obj_clear);

//|     peak: float
//|     """The largest magnitude of the audio that passed through since this was
//|     last read, from 0.0 to 1.0 of full scale. Reading this starts a new
//|     measurement. 0.0 when no audio passed through. The channels of stereo
//|     audio are measured together. (read-only)"""
//|
static mp_obj_t audiotap_tap_obj_get_peak(mp_obj_t self_in) {
    audiotap_tap_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_float(common_hal_audiotap_tap_get_peak(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiotap_tap_get_peak_obj, audiotap_tap_obj_get_peak);

MP_PROPERTY_GETTER(audiotap_tap_peak_obj,
    (mp_obj_t)&audiotap_tap_get_peak_obj);

//|     rms: float
//|     """The RMS level of the audio that passed through since this was last
//|     read, from 0.0 to 1.0 of full scale. Reading this starts a new
//|     measurement, separate from `peak`'s. 0.0 when no audio passed through.
//|     The channels of stereo audio are measured together. (read-only)"""
//|
static mp_obj_t audiotap_tap_obj_get_rms(mp_obj_t self_in) {
    audiotap_tap_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_float(common_hal_audiotap_tap_get_rms(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiotap_tap_get_rms_obj, audiotap_tap_obj_get_rms);

MP_PROPERTY_GETTER(audiotap_tap_rms_obj,
    (mp_obj_t)&audiotap_tap_get_rms_obj);

//|     def play(self, sample: circuitpython_typing.AudioSample, *, loop: bool = False) -> Tap:
//|         """Plays the sample once when loop=False and continuously when loop=True.
//|         Does not block. Use `playing` to block.
//|
//|         The first sample sets the tap's format. Later samples must match it.
//|
//|         :return: The tap itself. Can be used for chaining, ie:
//|           ``audio.play(tap.play(sample))``.
//|         :rtype: Tap"""
//|         ...
//|
static mp_obj_t audiotap_tap_obj_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_sample, ARG_loop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sample,    MP_ARG_OBJ | MP_ARG_REQUIRED, {} },
        { MP_QSTR_loop,      MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    audiotap_tap_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    common_hal_audiotap_tap_play(self, args[ARG_sample].u_obj, args[ARG_loop].u_bool);

    return MP_OBJ_FROM_PTR(self);
}
MP_DEFINE_CONST_FUN_OBJ_KW(audiotap_tap_play_obj, 1, audiotap_tap_obj_play);

//|     def stop(self) -> None:
//|         """Stops playback of the sample. The audio already kept can still be read."""
//|         ...
//|
//|
static mp_obj_t audiotap_tap_obj_stop(mp_obj_t self_in) {
    audiotap_tap_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_audiotap_tap_stop(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(audiotap_tap_stop_obj, audiotap_tap_obj_stop);

static const mp_rom_map_elem_t audiotap_tap_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audiotap_tap_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&audiotap_tap_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&audiotap_tap_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&audiotap_tap_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&audiotap_tap_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&audiotap_tap_clear_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_playing), MP_ROM_PTR(&audiotap_tap_playing_obj) },
    { MP_ROM_QSTR(MP_QSTR_available), MP_ROM_PTR(&audiotap_tap_available_obj) },
    { MP_ROM_QSTR(MP_QSTR_overflow), MP_ROM_PTR(&audiotap_tap_overflow_obj) },
    { MP_ROM_QSTR(MP_QSTR_peak), MP_ROM_PTR(&audiotap_tap_peak_obj) },
    { MP_ROM_QSTR(MP_QSTR_rms), MP_ROM_PTR(&audiotap_tap_rms_obj) },
    AUDIOSAMPLE_FIELDS,
};
static MP_DEFINE_CONST_DICT(audiotap_tap_locals_dict, audiotap_tap_locals_dict_table);

static const audiosample_p_t audiotap_tap_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_audiosample)
    .reset_buffer = (audiosample_reset_buffer_fun)audiotap_tap_reset_buffer,
    .get_buffer = (audiosample_get_buffer_fun)audiotap_tap_get_buffer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    audiotap_tap_type,
    MP_QSTR_Tap,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, audiotap_tap_make_new,
    locals_dict, &audiotap_tap_locals_dict,
    protocol, &audiotap_tap_proto
    );
