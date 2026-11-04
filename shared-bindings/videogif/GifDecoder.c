// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/builtin.h"
#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared-bindings/util.h"
#include "shared-bindings/videogif/GifDecoder.h"
#include "shared/runtime/context_manager_helpers.h"

//| class GifDecoder:
//|     """Decode an animated GIF file for video playback.
//|
//|     Pass a GifDecoder to ``videocore.VideoBuffer.play()`` to show the GIF
//|     on a display or to write its frames into a buffer.
//|
//|     Each frame is shown for the time the GIF file gives it. A few cases
//|     work the same way as in web browsers:
//|
//|     - A frame time of 0 or 0.01 seconds is shown for 0.1 seconds.
//|     - A GIF with no loop count plays once.
//|     - A loop count of 0 repeats the GIF forever. Any other loop count is
//|       the number of extra times to play it, so a loop count of 2 plays the
//|       GIF three times.
//|
//|     When the GIF finishes its last loop, its last frame stays on the screen
//|     and `videocore.VideoBuffer.playing` becomes False. The same happens if
//|     a frame or the file can't be read.
//|
//|     Frames are decoded in the background, in short breaks between your
//|     Python code. If a frame takes longer to decode than its frame time,
//|     the animation slows down so your Python code still gets time to run.
//|
//|     The GIF starts again from its first frame, filled with its background
//|     color, at each loop and whenever the VideoBuffer showing it directly
//|     on a display is moved or shown again.
//|
//|     When a frame asks for its area to be cleared after it is shown (GIF
//|     disposal method 2), that area is filled with the background color.
//|     GIFs that ask to restore the previous image (disposal method 3) may
//|     show leftover pixels, because that is not supported.
//|
//|     Play a GIF in a Group on a display:
//|
//|     .. code-block:: python
//|
//|         import board
//|         import displayio
//|         import videocore
//|         import videogif
//|
//|         # Match your display's byte order. A BusDisplay usually swaps the
//|         # bytes of each pixel (True). Framebuffer displays such as
//|         # mipidsi.Display don't (False).
//|         gif = videogif.GifDecoder("/animation.gif", reverse_bytes_in_word=False)
//|         video = videocore.VideoBuffer(bytearray(gif.width * gif.height * 2))
//|         video.play(gif)
//|
//|         root = displayio.Group()
//|         root.append(video)
//|         board.DISPLAY.root_group = root
//|         while True:
//|             pass
//|     """
//|
//|     def __init__(
//|         self,
//|         filename: str,
//|         *,
//|         reverse_bytes_in_word: bool = False,
//|     ) -> None:
//|         """Open a GIF file so it can be played.
//|
//|         :param str filename: The name of a GIF file, such as ``"/animation.gif"``.
//|             The GifDecoder closes the file when it is deinitialized.
//|         :param bool reverse_bytes_in_word: Swap the two bytes of each 16-bit
//|             pixel. Use the same value as your display, such as
//|             `busdisplay.BusDisplay`'s ``reverse_bytes_in_word``.
//|
//|         Frames are RGB565. Raises `ValueError` if the file is not a GIF or if
//|         the GIF is wider than 320 pixels.
//|         """
//|         ...
//|
static mp_obj_t videogif_gifdecoder_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_filename, ARG_reverse_bytes_in_word, NUM_ARGS };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_filename, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_reverse_bytes_in_word, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(allowed_args) == NUM_ARGS);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t filename = mp_arg_validate_type_string(args[ARG_filename].u_obj, MP_QSTR_filename);
    mp_obj_t file = mp_call_function_2(MP_OBJ_FROM_PTR(&mp_builtin_open_obj), filename, MP_ROM_QSTR(MP_QSTR_rb));

    videogif_gifdecoder_obj_t *self = mp_obj_malloc_with_finaliser(videogif_gifdecoder_obj_t, &videogif_gifdecoder_type);
    common_hal_videogif_gifdecoder_construct(self, file, args[ARG_reverse_bytes_in_word].u_bool);
    return MP_OBJ_FROM_PTR(self);
}

static videogif_gifdecoder_obj_t *native_gifdecoder(mp_obj_t self_in) {
    return MP_OBJ_TO_PTR(mp_obj_cast_to_native_base(self_in, &videogif_gifdecoder_type));
}

static void check_for_deinit(videogif_gifdecoder_obj_t *self) {
    if (common_hal_videogif_gifdecoder_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def deinit(self) -> None:
//|         """Stop playing and free the memory used to decode the GIF. The
//|         GifDecoder can't be used after this."""
//|         ...
//|
static mp_obj_t videogif_gifdecoder_deinit(mp_obj_t self_in) {
    common_hal_videogif_gifdecoder_deinit(native_gifdecoder(self_in));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(videogif_gifdecoder_deinit_obj, videogif_gifdecoder_deinit);

//|     def __enter__(self) -> GifDecoder:
//|         """No-op used by Context Managers."""
//|         ...
//|
//|     def __exit__(self) -> None:
//|         """Automatically deinitializes when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|

//|     width: int
//|     """Width of the GIF in pixels. (read-only)"""
//|
static mp_obj_t videogif_gifdecoder_get_width(mp_obj_t self_in) {
    videogif_gifdecoder_obj_t *self = native_gifdecoder(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_videogif_gifdecoder_get_width(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(videogif_gifdecoder_get_width_obj, videogif_gifdecoder_get_width);

MP_PROPERTY_GETTER(videogif_gifdecoder_width_obj,
    (mp_obj_t)&videogif_gifdecoder_get_width_obj);

//|     height: int
//|     """Height of the GIF in pixels. (read-only)"""
//|
//|
static mp_obj_t videogif_gifdecoder_get_height(mp_obj_t self_in) {
    videogif_gifdecoder_obj_t *self = native_gifdecoder(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_videogif_gifdecoder_get_height(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(videogif_gifdecoder_get_height_obj, videogif_gifdecoder_get_height);

MP_PROPERTY_GETTER(videogif_gifdecoder_height_obj,
    (mp_obj_t)&videogif_gifdecoder_get_height_obj);

static const mp_rom_map_elem_t videogif_gifdecoder_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&videogif_gifdecoder_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&videogif_gifdecoder_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&videogif_gifdecoder_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&videogif_gifdecoder_width_obj) },
};
static MP_DEFINE_CONST_DICT(videogif_gifdecoder_locals_dict, videogif_gifdecoder_locals_dict_table);

// Implementation of the videocore videosource protocol so a VideoBuffer can
// tell the GifDecoder where to put frames.
static void videogif_gifdecoder_play(mp_obj_t self_in, bool paused) {
    videogif_gifdecoder_obj_t *self = native_gifdecoder(self_in);
    check_for_deinit(self);
    common_hal_videogif_gifdecoder_play(self, paused);
}

static void videogif_gifdecoder_stop(mp_obj_t self_in) {
    common_hal_videogif_gifdecoder_stop(native_gifdecoder(self_in));
}

static void videogif_gifdecoder_pause(mp_obj_t self_in) {
    common_hal_videogif_gifdecoder_pause(native_gifdecoder(self_in));
}

static void videogif_gifdecoder_resume(mp_obj_t self_in) {
    common_hal_videogif_gifdecoder_resume(native_gifdecoder(self_in));
}

static bool videogif_gifdecoder_set_destination(mp_obj_t self_in, const videocore_destination_t *destination,
    videocore_on_frame_fun on_frame, void *context) {
    return common_hal_videogif_gifdecoder_set_destination(native_gifdecoder(self_in), destination, on_frame, context);
}

static mp_int_t videogif_gifdecoder_videosource_get_width(mp_obj_t self_in) {
    return common_hal_videogif_gifdecoder_get_width(native_gifdecoder(self_in));
}

static mp_int_t videogif_gifdecoder_videosource_get_height(mp_obj_t self_in) {
    return common_hal_videogif_gifdecoder_get_height(native_gifdecoder(self_in));
}

static mp_int_t videogif_gifdecoder_get_bits_per_pixel(mp_obj_t self_in) {
    return 16;
}

static const videosource_p_t videogif_gifdecoder_videosource_protocol = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_videosource)
    .play = videogif_gifdecoder_play,
    .pause = videogif_gifdecoder_pause,
    .resume = videogif_gifdecoder_resume,
    .stop = videogif_gifdecoder_stop,
    .set_destination = videogif_gifdecoder_set_destination,
    .get_width = videogif_gifdecoder_videosource_get_width,
    .get_height = videogif_gifdecoder_videosource_get_height,
    .get_bits_per_pixel = videogif_gifdecoder_get_bits_per_pixel,
};

MP_DEFINE_CONST_OBJ_TYPE(
    videogif_gifdecoder_type,
    MP_QSTR_GifDecoder,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, videogif_gifdecoder_make_new,
    locals_dict, &videogif_gifdecoder_locals_dict,
    protocol, &videogif_gifdecoder_videosource_protocol
    );
