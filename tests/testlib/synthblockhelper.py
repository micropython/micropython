import array
from math import sin, pi
from synthio import LFO, MathOperation, lfo_tick

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


def lfo_test(sweep_input_name, **inputs):
    inputs.setdefault("rate", 5)
    t = LFO(bend_out, rate=1, once=True)
    v = LFO(bend_out, rate=1.5, scale=2, once=True)
    test = LFO(sine, **inputs)
    setattr(test, sweep_input_name, v)
    print_result(t, v, test)
