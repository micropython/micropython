import sys

sys.path.insert(
    0, f"{__file__.rpartition('/')[0] or '.'}/../../../../frozen/Adafruit_CircuitPython_Wave"
)

import random
import audiocore
import synthio
from ulab import numpy as np
import adafruit_wave as wave

random.seed(9)
SAMPLE_SIZE = 1024
VOLUME = 14700
noise = np.array(
    [random.randint(-32768, 32767) for i in range(SAMPLE_SIZE)],
    dtype=np.int16,
)

envelope = synthio.Envelope(
    attack_time=0,
    decay_time=0.2,
    sustain_level=0,
)

synth = synthio.Synthesizer(sample_rate=48000)


def randf(lo, hi):
    return random.random() * (hi - lo) + lo


def synthesize(synth):
    notes = [
        synthio.Note(
            frequency=synthio.midi_to_hz(1 + i),
            waveform=noise,
            envelope=envelope,
            bend_mode=synthio.BendMode.SWEEP,
            bend_depth=random.choice((-1, 1)),
            bend_rate=randf(4, 12),
        )
        for i in range(12)
    ]

    random.seed(9)
    for _ in range(16):
        n = random.choice(notes)
        d = random.randint(30, 60)
        print(n, d)
        synth.press((n,))
        yield d
    synth.release_all()
    yield 36


def chain(*args):
    for a in args:
        yield from a


# sox -r 48000 -e signed -b 16 -c 1 tune.raw tune.wav
with wave.open("noise.wav", "w") as f:
    f.setnchannels(1)
    f.setsampwidth(2)
    f.setframerate(48000)
    for n in chain(synthesize(synth)):
        for i in range(n):
            result, data = audiocore.get_buffer(synth)
            f.writeframes(data)
