import sys

sys.path.insert(
    0, f"{__file__.rpartition('/')[0] or '.'}/../../../../frozen/Adafruit_CircuitPython_Wave"
)

import random
import audiocore
import synthio
from ulab import numpy as np
import adafruit_wave as wave

SAMPLE_SIZE = 1024
VOLUME = 14700
sine = np.array(
    np.sin(np.linspace(0, 2 * np.pi, SAMPLE_SIZE, endpoint=False)) * VOLUME,
    dtype=np.int16,
)

envelope = synthio.Envelope(
    attack_time=0.1, decay_time=0.05, release_time=0.2, attack_level=0.8, sustain_level=0.8
)

synth = synthio.Synthesizer(sample_rate=48000)


def synthesize(synth):
    random.seed(3)
    target_notes = [synthio.midi_to_hz(n + o) for n in (60, 64, 67, 70) for o in (-12, 12, 0)]
    print(target_notes)
    notes = [
        synthio.Note(frequency=random.randint(60, 20000), waveform=sine, envelope=envelope)
        for note in target_notes
    ]
    synth.press(notes)
    target = synthio.midi_to_hz(72)
    factor = 0.98
    for i in range(600):
        yield 1
        for ni, ti in zip(notes, target_notes):
            print(ni.frequency, ti)
            break
        for ni, ti in zip(notes, target_notes):
            ni.frequency = (ni.frequency * factor) + (ti * (1 - factor))
    synth.release_all()
    yield 36


def synthesize2(synth):
    n = synthio.Note(
        frequency=synthio.midi_to_hz(60),
        tremolo_depth=0.2,
        tremolo_rate=2,
        waveform=sine,
        envelope=envelope,
    )
    synth.press((n,))
    yield 360
    synth.release_all()
    yield 36


def synthesize3(synth):
    n = synthio.Note(
        frequency=synthio.midi_to_hz(60),
        bend_depth=0.1,
        bend_rate=8,
        waveform=sine,
        envelope=envelope,
    )
    synth.press((n,))
    yield 360
    synth.release_all()
    yield 36


def synthesize4(synth):
    n = synthio.Note(
        frequency=synthio.midi_to_hz(60),
        tremolo_depth=0.1,
        tremolo_rate=1.5,
        bend_depth=0.1,
        bend_rate=3,
        waveform=sine,
        envelope=envelope,
    )
    synth.press((n,))
    yield 720
    synth.release_all()
    yield 36


def synthesize5(synth):
    notes = [
        synthio.Note(
            frequency=synthio.midi_to_hz(60 + i + o),
            waveform=sine,
            envelope=envelope,
        )
        for i in [0, 4, 7]
        for o in [0, -12, 12]
    ]

    for n in notes:
        print(n)
        synth.press((n,))
        yield 120
    synth.release_all()
    yield 36


def synthesize6(synth):
    n = synthio.Note(
        frequency=synthio.midi_to_hz(60),
        tremolo_depth=0.1,
        tremolo_rate=1.5,
        bend_depth=-5 / 12,
        bend_rate=1 / 2,
        bend_mode=synthio.BendMode.SWEEP,
        waveform=sine,
        envelope=envelope,
    )
    synth.press((n,))
    yield 720
    synth.release_all()
    yield 36


def chain(*args):
    for a in args:
        yield from a


# sox -r 48000 -e signed -b 16 -c 1 tune.raw tune.wav
with wave.open("tune-noenv.wav", "w") as f:
    f.setnchannels(1)
    f.setsampwidth(2)
    f.setframerate(48000)
    for n in chain(synthesize6(synth)):
        for i in range(n):
            result, data = audiocore.get_buffer(synth)
            f.writeframes(data)
