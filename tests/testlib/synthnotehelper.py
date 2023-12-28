import array
from ulab import numpy as np
from math import sin, pi, ceil
from audiocore import get_buffer
from synthio import Note, LFO, MathOperation, Synthesizer

bend_out = array.array("h", [0, 32767])
bend_in = array.array("h", [32767, 0])
sweep = array.array("h", [-32767, 32767])
triangle = array.array("h", [0, 32767, 0, -32767])
sine = array.array("h", [int(32767 * sin(i * 2 * pi / 600)) for i in range(600)])


def print_result(*blocks):
    for i in range(48000 / 256):
        print(*lfo_tick(*blocks))


def mathop_test(kind):
    v = LFO(sweep, rate=1, scale=2, once=True)
    varying_a = kind(v, 2, -3)
    varying_b = kind(-3, v, 2)
    varying_c = kind(2, -3, v)
    print_result(v, varying_a, varying_b, varying_c)


def synth_test(_gen=None, **kw):
    kw.setdefault("waveform", sine)
    kw.setdefault("channel_count", 1)
    kw.setdefault("sample_rate", 8000)

    def func(gen):
        synth = Synthesizer(**kw)
        t = 0
        dt = 1 / kw["sample_rate"]
        channel_count = kw["channel_count"]
        g = gen(synth)
        blocks = list(next(g))
        for st in g:
            nframes = ceil(st / dt / 256)
            for i in range(nframes):
                samples = np.frombuffer(get_buffer(synth)[1], dtype=np.int16) / 32768.0
                block_values = [b.value for b in blocks]
                for k in range(0, len(samples), channel_count):
                    print(t * dt, *(list(samples[k : k + channel_count]) + block_values))
                    t += 1

    if _gen is None:
        return func
    else:
        func(_gen)


def rms(seq):
    return np.sqrt(np.mean(seq**2))


def synth_test_rms(_gen=None, **kw):
    kw.setdefault("waveform", sine)
    kw.setdefault("channel_count", 1)
    kw.setdefault("sample_rate", 8192)

    def func(gen):
        synth = Synthesizer(**kw)
        t = 0
        dt = 256 / kw["sample_rate"]
        channel_count = kw["channel_count"]
        g = gen(synth)
        blocks = list(next(g))
        for st in g:
            nframes = ceil(st / dt)
            for i in range(nframes):
                samples = np.frombuffer(get_buffer(synth)[1], dtype=np.int16) / 32768.0
                rms_values = [rms(samples[i::2]) for i in range(channel_count)]
                block_values = [b.value for b in blocks]
                print(t * dt, *(rms_values + block_values))
                t += 1

    if _gen is None:
        return func
    else:
        func(_gen)
