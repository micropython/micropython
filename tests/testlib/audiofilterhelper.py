import array
import random
from ulab import numpy as np
from math import sin, pi, ceil
from audiocore import get_buffer, RawSample
from synthio import Note, LFO, MathOperation, Synthesizer

random.seed(41)

whitedata = array.array("h", [random.randint(-32000, 32000) for i in range(600)])
white8k = RawSample(whitedata, sample_rate=8000)

sinedata = array.array("h", [int(32767 * sin(i * 2 * pi / 600)) for i in range(600)])
sine8k = RawSample(sinedata, sample_rate=8000)


def synth_test(_gen=None, dtype=np.int16, divisor=32768.0, channel_count=1):
    def func(gen):
        g = gen()
        synth, blocks = next(g)
        t = 0
        for nframes in g:
            for i in range(nframes):
                samples = np.frombuffer(get_buffer(synth)[1], dtype=dtype) / divisor
                block_values = [b.value for b in blocks]
                for k in range(0, len(samples), channel_count):
                    print(t, *(list(samples[k : k + channel_count]) + block_values))
                    t += 1

    if _gen is None:
        return func
    else:
        func(_gen)
