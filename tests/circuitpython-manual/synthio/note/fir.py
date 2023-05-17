import sys

sys.path.insert(
    0, f"{__file__.rpartition('/')[0] or '.'}/../../../../frozen/Adafruit_CircuitPython_Wave"
)

import random
import audiocore
import synthio
from ulab import numpy as np
import adafruit_wave as wave
import mkfilter

random.seed(9)

envelope = synthio.Envelope(
    attack_time=0.1, decay_time=0.05, release_time=0.2, attack_level=0.8, sustain_level=0.8
)

SAMPLE_SIZE = 1024
bend_out = np.linspace(0, 32767, num=SAMPLE_SIZE, endpoint=True, dtype=np.int16)

filter_rectangular = mkfilter.LPF(48000, 800, 13)
filter_rectangular_big = mkfilter.LPF(48000, 800, 59)
filter_blackman = mkfilter.LPF(48000, 800, 59, win=mkfilter.blackman)
print(filter_blackman)


def synthesize(synth):
    n = synthio.Note(
        frequency=120,
        envelope=envelope,
        filter=True,
        bend=synthio.LFO(bend_out, once=True, rate=1 / 2, scale=5),
    )

    synth.press(n)
    print(synth, n)
    yield 2 * 48000 // 256
    synth.release_all()
    yield 36


with wave.open("fir.wav", "w") as f:
    f.setnchannels(1)
    f.setsampwidth(2)
    f.setframerate(48000)
    for filter_coeffs in [None, filter_rectangular, filter_rectangular_big, filter_blackman]:
        synth = synthio.Synthesizer(sample_rate=48000, filter=filter_coeffs)
        for n in synthesize(synth):
            for i in range(n):
                result, data = audiocore.get_buffer(synth)
                f.writeframes(data)
