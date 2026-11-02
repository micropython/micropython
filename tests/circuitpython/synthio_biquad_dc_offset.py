# A shelf with a gain of 1.0 does nothing, so it must not leave anything behind
# once its input falls silent. Rounding the wrong way on every sample used to
# leave an offset of hundreds of counts at 200 Hz.
import array
from audiocore import get_buffer, RawSample
from audiofilters import Filter
from synthio import Biquad, FilterMode

RATE = 36000
NOISE = RATE // 2  # half a second of quiet noise, then silence

seed = 1
values = array.array("h", [0] * (2 * NOISE))
for n in range(NOISE):
    total = 0
    for _ in range(12):  # twelve uniform draws make a bell with a sigma of 100
        seed = (seed * 1664525 + 1013904223) & 0xFFFFFFFF
        total += (seed >> 8) / 16777216
    values[n] = int((total - 6) * 100)

source = RawSample(values, sample_rate=RATE, channel_count=1)
effect = Filter(
    filter=Biquad(FilterMode.LOW_SHELF, 200, Q=0.5**0.5, A=1.0),
    sample_rate=RATE,
    channel_count=1,
    bits_per_sample=16,
    samples_signed=True,
    buffer_size=2048,
)
effect.play(source)

out = array.array("h")
while len(out) < len(values):
    out.extend(array.array("h", bytes(get_buffer(effect)[1])))

tail = out[len(values) - 2000 : len(values)]
offset = sum(tail) / len(tail)
print("offset in silence:", "none" if abs(offset) < 1 else "%+.0f" % offset)
