# Low down, the numerator coefficients of a low-pass are tiny next to the
# feedback ones. The filter should still be about 3 dB down at its own cutoff,
# and a high-pass at 30 Hz should pass 1 kHz at unity.
import array
import math
from audiocore import get_buffer, RawSample
from audiofilters import Filter
from synthio import Biquad, FilterMode

RATE = 48000
FRAMES = 4800  # 0.1 s, so a tone of any multiple of 10 Hz loops without a seam
LEVEL = 8000


def gain_db(mode, cutoff, tone_hz):
    values = array.array(
        "h",
        [int(LEVEL * math.sin(2 * math.pi * tone_hz * n / RATE)) for n in range(FRAMES)],
    )
    source = RawSample(values, sample_rate=RATE, channel_count=1)
    effect = Filter(
        filter=Biquad(mode, cutoff, Q=0.5**0.5),
        sample_rate=RATE,
        channel_count=1,
        bits_per_sample=16,
        samples_signed=True,
        buffer_size=FRAMES * 2,
    )
    effect.play(source, loop=True)
    out = []
    for block in range(12):
        data = bytes(get_buffer(effect)[1])
        if block < 10:  # let the filter settle
            continue
        for index in range(0, len(data) - 1, 2):
            sample = data[index] | (data[index + 1] << 8)
            out.append(sample - 65536 if sample >= 32768 else sample)
    rms = math.sqrt(sum(value * value for value in out) / len(out))
    return 20 * math.log10(max(rms, 1e-3) / (LEVEL / math.sqrt(2)))


def report(label, gain, want):
    # Print the gain only if it is not within half a dB of what it should be.
    print(label, "ok" if abs(gain - want) < 0.5 else "%+.1f dB" % gain)


for cutoff in (30, 50, 100):
    report("low-pass at %d Hz:" % cutoff, gain_db(FilterMode.LOW_PASS, cutoff, cutoff), -3)
report("high-pass at 30 Hz, 1 kHz tone:", gain_db(FilterMode.HIGH_PASS, 30, 1000), 0)
