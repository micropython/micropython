import random
import audiocore
import synthio
from ulab import numpy as np
import adafruit_wave as wave

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


def randf(lo, hi):
    return random.random() * (hi - lo) + lo


SAMPLE_SIZE = 1024
VOLUME = 14700
sine = np.array(
    np.sin(np.linspace(0, 2 * np.pi, SAMPLE_SIZE, endpoint=False)) * VOLUME, dtype=np.int16
)
square = np.array([24000] * (SAMPLE_SIZE // 2) + [-24000] * (SAMPLE_SIZE // 2), dtype=np.int16)

noise = np.array(
    [random.randint(-32768, 32767) for i in range(SAMPLE_SIZE)],
    dtype=np.int16,
)

bend_in = np.linspace(32767, 0, num=SAMPLE_SIZE, endpoint=True, dtype=np.int16)

envelope = synthio.Envelope(
    attack_time=0.1, decay_time=0.05, release_time=0.2, attack_level=1, sustain_level=0.8
)

instant = synthio.Envelope(
    attack_time=0, decay_time=0, release_time=0, attack_level=1, sustain_level=1
)
synth = synthio.Synthesizer(
    sample_rate=48000, envelope=None, filter=filter_coeffs, channel_count=2
)


def synthesize(synth):
    print(
        """you can use arbitrary waveforms, including ones calculated on the fly or read from wave files (up to 1024 points)"""
    )

    waveform = np.zeros(SAMPLE_SIZE, dtype=np.int16)
    chord = [
        synthio.Note(synthio.midi_to_hz(n), waveform=waveform, envelope=envelope)
        for n in (60, 64, 67, 70)
    ]
    synth.press(chord)

    for i in range(256):
        ctrl = i / 255
        waveform[:] = np.array(square * (1 - ctrl) + sine * ctrl, dtype=np.int16)
        yield 4


def synthesize2(synth):
    print("""Envelope controls how notes fade in or out""")

    chord = [
        synthio.Note(synthio.midi_to_hz(n), waveform=sine, envelope=instant)
        for n in (60, 64, 67, 70)
    ]

    for i in range(4):
        for c in chord:
            synth.release_all_then_press((c,))
            yield 24
    synth.release_all()

    chord = [
        synthio.Note(synthio.midi_to_hz(n), waveform=sine, envelope=envelope)
        for n in (60, 64, 67, 70)
    ]

    for i in range(4):
        for c in chord:
            old = (c,)
            synth.release_all_then_press((c,))
            yield 24


def synthesize3(synth):
    print("""A noise waveform creates percussive sounds""")

    env = synthio.Envelope(
        attack_time=0,
        decay_time=0.2,
        sustain_level=0,
    )

    notes = [
        synthio.Note(
            frequency=synthio.midi_to_hz(1 + i),
            waveform=noise,
            envelope=env,
        )
        for i in range(12)
    ]

    random.seed(9)
    for _ in range(16):
        n = random.choice(notes)
        d = random.randint(30, 60)
        synth.press((n,))
        yield d


def synthesize4(synth):
    print("""Tremolo varies the note volume within a range at a low frequency""")

    chord = [
        synthio.Note(synthio.midi_to_hz(n), waveform=sine, envelope=None) for n in (60, 64, 67, 70)
    ]

    synth.press(chord)
    for i in range(16):
        for c in chord:
            d = i / 50
            c.amplitude = synthio.LFO(scale=d / 2, offset=1 - d, rate=(i + 1) / 4, waveform=sine)
        yield 48
    yield 36


def synthesize5(synth):
    print("""You can add vibrato or frequency sweep to notes""")

    chord = [synthio.Note(synthio.midi_to_hz(n), waveform=sine) for n in (60, 64, 67, 70)]

    synth.press(chord)
    for i in range(16):
        for c in chord:
            c.bend = synthio.LFO(scale=1 / 24, rate=(i + 1) / 2, waveform=sine)
        yield 24
    synth.release_all()
    yield 100

    for c in chord:
        synth.release_all()
        c.bend = synthio.LFO(scale=randf(-1, 1), rate=1 / 2, waveform=bend_in)
    synth.press(chord)
    yield 320


def synthesize6(synth):
    print("""Ring modulation multiplies two waveforms together to create rich sounds""")

    chord = [
        synthio.Note(synthio.midi_to_hz(n), waveform=square, ring_waveform=sine, envelope=envelope)
        for n in (60,)
    ]

    synth.press(chord)
    yield 200

    random.seed(75)

    for _ in range(3):
        synth.release_all()
        yield 36
        for note in chord:
            note.ring_frequency = note.frequency * (random.random() * 35 / 1200 + 8)
        synth.press(chord)
        yield 200


def synthesize7(synth):
    print("""FIR filtering can reproduce low, high, notch and band filters""")
    chord = [
        synthio.Note(synthio.midi_to_hz(n), waveform=square, filter=False)
        for n in (60, 64, 67, 70)
    ]

    for i in range(4):
        for c in chord:
            synth.release_all_then_press((c,))
            yield 24
    synth.release_all()

    for note in chord:
        note.filter = True

    for i in range(4):
        for c in chord:
            synth.release_all_then_press((c,))
            yield 24


def synthesize8(synth):
    print("""Notes can be panned between channels""")
    chord = [
        synthio.Note(synthio.midi_to_hz(n), waveform=square, envelope=envelope)
        for n in (60, 64, 67, 70)
    ]
    synth.press(chord)
    for p in range(-10, 11, 1):
        for note in chord:
            note.panning = p / 10
        yield 36


def delay(synth):
    synth.release_all()
    yield 200


def chain(*args):
    for a in args:
        yield from a


# sox -r 48000 -e signed -b 16 -c 1 tune.raw tune.wav
with wave.open("demo.wav", "w") as f:
    f.setnchannels(2)
    f.setsampwidth(2)
    f.setframerate(48000)
    import array

    for n in chain(
        synthesize(synth),
        delay(synth),
        synthesize2(synth),
        delay(synth),
        synthesize3(synth),
        delay(synth),
        synthesize4(synth),
        delay(synth),
        synthesize5(synth),
        delay(synth),
        synthesize6(synth),
        delay(synth),
        synthesize7(synth),
        delay(synth),
        synthesize8(synth),
    ):
        for i in range(n):
            result, data = audiocore.get_buffer(synth)
            f.writeframes(data)
