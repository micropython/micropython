// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/videocore/VideoBuffer.h"
#include "shared-bindings/videocore/__init__.h"

//| """Support for playing video sources
//|
//| This module provides the video pipeline shared between video sources and
//| video outputs. It is the video equivalent of the `audiocore` module.
//|
//| Video sources, such as `mipicsi.Camera`, implement the internal videosource
//| protocol. A `VideoBuffer` tells a video source where to put its frames in a
//| framebuffer, such as `mipidsi.Display`, and can move it as needed.
//|
//| Stream a MIPI CSI camera live into the board's display (a MIPI DSI panel
//| on ESP32-P4-Function-EV, for example):
//|
//| .. code-block:: python
//|
//|     import board
//|     import displayio
//|     import mipicsi
//|     import sc2336
//|     import videocore
//|
//|     display = board.DISPLAY()
//|     framebuffer = display.framebuffer
//|     sensor = sc2336.SC2336(board.I2C())
//|     sensor.mode = sc2336.Mode.HD_1280x720_RAW10_30FPS
//|     # Trim the sensor output to the display size.
//|     sensor.width = display.width
//|     sensor.height = display.height
//|     camera = mipicsi.Camera(sensor, output_format=mipicsi.PixelFormat.RGB565)
//|
//|     video = videocore.VideoBuffer(framebuffer)
//|     video.play(camera)
//|     root = displayio.Group()
//|     root.append(video)
//|     display.root_group = root
//|
//| Capture a smaller part of the sensor's image into a `bytearray` and let
//| displayio place it in a Group, together with other layers. displayio bulk
//| copies new frames into the group's area of the display (no color
//| conversion):
//|
//| .. code-block:: python
//|
//|     import board
//|     import displayio
//|     import mipicsi
//|     import sc2336
//|     import videocore
//|
//|     sensor = sc2336.SC2336(board.I2C())
//|     sensor.mode = sc2336.Mode.VGA_640x480_RAW10_50FPS
//|     # Trim the sensor output to a 320x240 window.
//|     sensor.width = 320
//|     sensor.height = 240
//|     camera = mipicsi.Camera(sensor, output_format=mipicsi.PixelFormat.RGB565)
//|
//|     capture = videocore.VideoBuffer(bytearray(320 * 240 * 2))
//|     capture.play(camera)
//|
//|     root = displayio.Group()
//|     root.append(capture)
//|     capture.x = 350  # position of the live video within the group
//|     capture.y = 180
//|     board.DISPLAY().root_group = root  # live video joins the displayio scene
//| """
//|

static const mp_rom_map_elem_t videocore_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_videocore) },
    { MP_ROM_QSTR(MP_QSTR_VideoBuffer), MP_ROM_PTR(&videocore_videobuffer_type) },
};

static MP_DEFINE_CONST_DICT(videocore_module_globals, videocore_module_globals_table);

const mp_obj_module_t videocore_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&videocore_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_videocore, videocore_module);
