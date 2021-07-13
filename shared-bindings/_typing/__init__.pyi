"""Types for the C-level protocols"""

from typing import Union

import alarm
import alarm.pin
import alarm.time
import array
import audiocore
import audiomixer
import audiomp3
import rgbmatrix
import ulab

ReadableBuffer = Union[
    bytes, bytearray, memoryview, array.array, ulab.ndarray, rgbmatrix.RGBMatrix
]
"""Classes that implement the readable buffer protocol

  - `bytes`
  - `bytearray`
  - `memoryview`
  - `array.array`
  - `ulab.ndarray`
  - `rgbmatrix.RGBMatrix`
"""

WriteableBuffer = Union[
    bytearray, memoryview, array.array, ulab.ndarray, rgbmatrix.RGBMatrix
]
"""Classes that implement the writeable buffer protocol

  - `bytearray`
  - `memoryview`
  - `array.array`
  - `ulab.ndarray`
  - `rgbmatrix.RGBMatrix`
"""

AudioSample = Union[
    audiocore.WaveFile, audiocore.RawSample, audiomixer.Mixer, audiomp3.MP3Decoder, synthio.MidiTrack
]
"""Classes that implement the audiosample protocol

  - `audiocore.WaveFile`
  - `audiocore.RawSample`
  - `audiomixer.Mixer`
  - `audiomp3.MP3Decoder`
  - `synthio.MidiTrack`

  You can play these back with `audioio.AudioOut`, `audiobusio.I2SOut` or `audiopwmio.PWMAudioOut`.
"""

FrameBuffer = Union[rgbmatrix.RGBMatrix]
"""Classes that implement the framebuffer protocol

  - `rgbmatrix.RGBMatrix`
"""

Alarm = Union[
    alarm.pin.PinAlarm, alarm.time.TimeAlarm
]
"""Classes that implement alarms for sleeping and asynchronous notification.

  - `alarm.pin.PinAlarm`
  - `alarm.time.TimeAlarm`

  You can use these alarms to wake up from light or deep sleep.
"""
