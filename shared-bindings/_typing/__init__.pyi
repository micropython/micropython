"""Types for the C-level protocols"""

from typing import Union

import array
import audiocore
import audiomixer
import audiomp3
import rgbmatrix
import ulab

ReadableBuffer = Union[
    bytes, bytearray, memoryview, array.array, ulab.array, rgbmatrix.RGBMatrix
]
"""Classes that implement the readable buffer protocol

  - `bytes`
  - `bytearray`
  - `memoryview`
  - `array.array`
  - `ulab.array`
  - `rgbmatrix.RGBMatrix`
"""

WriteableBuffer = Union[
    bytearray, memoryview, array.array, ulab.array, rgbmatrix.RGBMatrix
]
"""Classes that implement the writeable buffer protocol

  - `bytearray`
  - `memoryview`
  - `array.array`
  - `ulab.array`
  - `rgbmatrix.RGBMatrix`
"""

AudioSample = Union[
    audiocore.WaveFile, audiocore.RawSample, audiomixer.Mixer, audiomp3.MP3Decoder
]
"""Classes that implement the audiosample protocol

  - `audiocore.WaveFile`
  - `audiocore.RawSample`
  - `audiomixer.Mixer`
  - `audiomp3.MP3Decoder`

  You can play these back with `audioio.AudioOut`, `audiobusio.I2SOut` or `audiopwmio.PWMAudioOut`.
"""

FrameBuffer = Union[rgbmatrix.RGBMatrix]
"""Classes that implement the framebuffer protocol

  - `rgbmatrix.RGBMatrix`
"""

Alarm = Union[
    alarm_time.Time, alarm_pin.PinLevel, alarm_touch.PinTouch
]
"""Classes that implement the audiosample protocol

  - `alarm_time.Time`
  - `alarm_pin.PinLevel`
  - `alarm_touch.PinTouch`

  You can play use these alarms to wake from light or deep sleep.
"""
