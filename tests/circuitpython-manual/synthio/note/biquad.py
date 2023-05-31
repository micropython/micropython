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
    attack_time=0.15, decay_time=0, release_time=0.08, attack_level=1.0, sustain_level=1.0
)

SAMPLE_SIZE = 1024
VOLUME = 14700
sine = np.array(
    np.sin(np.linspace(0, 2 * np.pi, SAMPLE_SIZE, endpoint=False)) * VOLUME,
    dtype=np.int16,
)
noise = np.array([random.randint(-VOLUME, VOLUME) for i in range(SAMPLE_SIZE)], dtype=np.int16)
bend_out = np.linspace(0, 32767, num=SAMPLE_SIZE, endpoint=True, dtype=np.int16)


def synthesize(synth):
    for waveform in (sine, None, noise):
        for biquad in (
            None,
            synth.low_pass_filter(120),
        ):
            for midi_note in range(24, 90, 3):
                n = synthio.Note(
                    frequency=synthio.midi_to_hz(midi_note),
                    envelope=envelope,
                    filter=biquad,
                    waveform=waveform,
                    bend=synthio.LFO(bend_out, once=True, rate=1 / 2, scale=5),
                )

                synth.press(n)
                print(n.frequency)
                yield 24
                synth.release_all()
                yield 16
            yield 24
        yield 48


with wave.open("biquad.wav", "w") as f:
    f.setnchannels(1)
    f.setsampwidth(2)
    f.setframerate(48000)
    synth = synthio.Synthesizer(sample_rate=48000)
    for n in synthesize(synth):
        for i in range(n):
            result, data = audiocore.get_buffer(synth)
            f.writeframes(data)
