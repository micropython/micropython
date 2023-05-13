import sys

sys.path.insert(
    0, f"{__file__.rpartition('/')[0] or '.'}/../../../../frozen/Adafruit_CircuitPython_Wave"
)

import audiocore
import synthio
from ulab import numpy as np
import adafruit_wave as wave

SAMPLE_SIZE = 1024
VOLUME = 32700
sine = np.array(
    np.sin(np.linspace(0, 2 * np.pi, SAMPLE_SIZE, endpoint=False)) * VOLUME,
    dtype=np.int16,
)

envelope = synthio.Envelope(
    attack_time=0.1, decay_time=0.05, release_time=0.2, attack_level=1, sustain_level=0.8
)
melody = synthio.MidiTrack(
    b"\0\x90H\0*\x80H\0\6\x90J\0*\x80J\0\6\x90L\0*\x80L\0\6\x90J\0"
    + b"*\x80J\0\6\x90H\0*\x80H\0\6\x90J\0*\x80J\0\6\x90L\0T\x80L\0"
    + b"\x0c\x90H\0T\x80H\0\x0c\x90H\0T\x80H\0",
    tempo=240,
    sample_rate=48000,
    waveform=sine,
    envelope=envelope,
)

# sox -r 48000 -e signed -b 16 -c 1 tune.raw tune.wav
with wave.open("tune.wav", "w") as f:
    f.setnchannels(1)
    f.setsampwidth(2)
    f.setframerate(48000)
    while True:
        result, data = audiocore.get_buffer(melody)
        if data is None:
            break
        f.writeframes(data)
        if result != 1:
            break

melody = synthio.MidiTrack(
    b"\0\x90H\0*\x80H\0\6\x90J\0*\x80J\0\6\x90L\0*\x80L\0\6\x90J\0"
    + b"*\x80J\0\6\x90H\0*\x80H\0\6\x90J\0*\x80J\0\6\x90L\0T\x80L\0"
    + b"\x0c\x90H\0T\x80H\0\x0c\x90H\0T\x80H\0",
    tempo=240,
    sample_rate=48000,
    waveform=sine,
)

# sox -r 48000 -e signed -b 16 -c 1 tune.raw tune.wav
with wave.open("tune-noenv.wav", "w") as f:
    f.setnchannels(1)
    f.setsampwidth(2)
    f.setframerate(48000)
    while True:
        result, data = audiocore.get_buffer(melody)
        if data is None:
            break
        f.writeframes(data)
        if result != 1:
            break
