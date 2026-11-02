# A filter set near Nyquist should still be the filter that was asked for:
# a low-pass is 3 dB down at its own cutoff, and a high-pass at 22 kHz has
# to reject a 12 kHz tone.
import array
import math
from audiocore import get_buffer, RawSample
from audiofilters import Filter
from synthio import Biquad, FilterMode

RATE = 48000
FRAMES = 4096
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
    for block in range(3):
        data = bytes(get_buffer(effect)[1])
        if block < 2:  # let the filter settle
            continue
        for index in range(0, len(data) - 1, 2):
            sample = data[index] | (data[index + 1] << 8)
            out.append(sample - 65536 if sample >= 32768 else sample)
    rms = math.sqrt(sum(v * v for v in out) / len(out))
    return 20 * math.log10(max(rms, 1e-3) / (LEVEL / math.sqrt(2)))


for cutoff in (4000, 8000, 12000, 16000, 20000, 22000):
    print("low-pass at", cutoff, "Hz: %+.1f dB" % gain_db(FilterMode.LOW_PASS, cutoff, cutoff))

print("22 kHz high-pass, 12 kHz tone: %+.0f dB" % gain_db(FilterMode.HIGH_PASS, 22000, 12000))
