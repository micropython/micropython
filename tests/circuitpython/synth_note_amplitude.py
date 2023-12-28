from synthnotehelper import *


@synth_test
def gen(synth):
    l = LFO(sine, offset=0.2, scale=0.8, rate=2)
    yield [l]
    n = Note(8, amplitude=l)
    synth.press(n)
    yield 2
