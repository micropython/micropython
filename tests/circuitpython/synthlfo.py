import array
from math import sin, pi
from synthio import LFO, lfo_tick

bend_out = array.array("h", [0, 32767])
bend_in = array.array("h", [32767, 0])
triangle = array.array("h", [0, 32767, 0, -32767])
sine = array.array("h", [int(32767 * sin(i * 2 * pi / 600)) for i in range(600)])

lfos = [
    LFO(bend_out, once=True, rate=4),
    LFO(bend_in, once=True, rate=2.5, scale=2),
    LFO(triangle, rate=3),
    LFO(bend_in, interpolate=False, rate=5),
    LFO(bend_in, interpolate=True, rate=5),
]
lfos[2].rate = lfos[1]
lfos[2].offset = lfos[0]

print("Initial values", *(l.value for l in lfos))

for i in range(100):
    print(i * 256 / 48000, *list(lfo_tick(*lfos)) + [l.phase for l in lfos])
