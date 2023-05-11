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

envelope = synthio.Envelope(
    attack_time=0.1, decay_time=0.05, release_time=0.2, attack_level=0.8, sustain_level=0.8
)

h = np.array(
    [
        -0.001229734800309099,
        -0.008235561806605458,
        -0.015082497016061390,
        -0.020940136918319988,
        -0.024981800822463429,
        -0.026464233332370746,
        -0.024803890156806906,
        -0.019642276775473012,
        -0.010893620860173042,
        0.001230341899766145,
        0.016221637398855598,
        0.033304135659230648,
        0.051486665261155681,
        0.069636961761409016,
        0.086570197432542767,
        0.101144354207918147,
        0.112353938422488253,
        0.119413577288191297,
        0.121823886314051028,
        0.119413577288191297,
        0.112353938422488253,
        0.101144354207918147,
        0.086570197432542767,
        0.069636961761409016,
        0.051486665261155681,
        0.033304135659230648,
        0.016221637398855598,
        0.001230341899766145,
        -0.010893620860173042,
        -0.019642276775473012,
        -0.024803890156806906,
        -0.026464233332370746,
        -0.024981800822463429,
        -0.020940136918319988,
        -0.015082497016061390,
        -0.008235561806605458,
        -0.001229734800309099,
    ]
)

filter_coeffs = np.array(h[::-1] * 32768, dtype=np.int16)

synth = synthio.Synthesizer(sample_rate=48000, filter=filter_coeffs)


def synthesize(synth):
    n = synthio.Note(
        frequency=120,
        envelope=envelope,
        filter=False,
    )

    print(synth, n)
    synth.press((n,))
    for _ in range(20):
        n.frequency *= 1.0595
        yield 36
    synth.release_all()
    yield 36

    n.filter = True
    n.frequency = 120
    synth.press((n,))
    for _ in range(20):
        n.frequency *= 1.0595
        yield 36
    synth.release_all()
    yield 36


def chain(*args):
    for a in args:
        yield from a


# sox -r 48000 -e signed -b 16 -c 1 tune.raw tune.wav
with wave.open("fir.wav", "w") as f:
    f.setnchannels(1)
    f.setsampwidth(2)
    f.setframerate(48000)
    for n in chain(synthesize(synth)):
        for i in range(n):
            result, data = audiocore.get_buffer(synth)
            f.writeframes(data)
