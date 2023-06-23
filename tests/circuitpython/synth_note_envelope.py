from synthnotehelper import *


@synth_test
def gen(synth):
    l = LFO(bend_out, offset=0.2, scale=0.8, rate=4, once=True)
    yield [l]
    n = Note(128, amplitude=l)
    synth.press(n)
    yield 1 / 4
