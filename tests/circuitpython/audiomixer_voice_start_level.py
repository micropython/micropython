# A voice must play at its level from the first sample after play(), not wait
# for a zero crossing in the sample before it is heard.
import array
from audiocore import get_buffer, reset_buffer, RawSample
from audiomixer import Mixer

RATE = 8000

# Starts at 0, so the first voice can change level right away.
ramp = RawSample(array.array("h", [0, 1000, 2000, 3000] * 64), sample_rate=RATE)
# Starts away from 0 and has no zero crossing in its first 100 samples.
square = RawSample(array.array("h", [8000] * 100 + [-8000] * 100), sample_rate=RATE)


def mixed(mixer):
    return list(get_buffer(mixer)[1])


def show(name, got, want):
    worst = max(abs(g - w) for g, w in zip(got, want))
    print(name, got[:4], "worst", worst)


# Two voices: the second one must be in the very first buffer.
mixer = Mixer(voice_count=2, channel_count=1, sample_rate=RATE, buffer_size=512)
mixer.voice[0].level = 0.5
mixer.voice[1].level = 0.5
mixer.voice[0].play(ramp, loop=True)
mixer.voice[1].play(square, loop=True)
got = mixed(mixer)
want = [
    ([0, 1000, 2000, 3000][i % 4] + (8000 if i % 200 < 100 else -8000)) // 2
    for i in range(len(got))
]
show("two voices", got, want)
mixer.deinit()

# A level set while the voice is stopped applies from the first sample of the next play().
mixer = Mixer(voice_count=1, channel_count=1, sample_rate=RATE, buffer_size=512)
mixer.voice[0].play(square, loop=True)
mixed(mixer)
mixer.voice[0].stop()
mixer.voice[0].level = 0.25
mixed(mixer)
mixer.voice[0].play(square, loop=True)
got = mixed(mixer)
want = [2000 if i % 200 < 100 else -2000 for i in range(len(got))]
show("new level", got, want)
mixer.deinit()

# Restarting the mixer output also restarts each voice at its current level.
mixer = Mixer(voice_count=1, channel_count=1, sample_rate=RATE, buffer_size=512)
mixer.voice[0].level = 0.5
mixer.voice[0].play(square, loop=True)
mixed(mixer)
mixer.voice[0].level = 0.25
reset_buffer(mixer)
got = mixed(mixer)
want = [2000 if i % 200 < 100 else -2000 for i in range(len(got))]
show("restart", got, want)
mixer.deinit()

# Stereo with panning: each channel starts at its own panned level.
mixer = Mixer(voice_count=1, channel_count=2, sample_rate=RATE, buffer_size=512)
mixer.voice[0].level = 0.5
mixer.voice[0].panning = -0.5
mixer.voice[0].play(square, loop=True)
got = mixed(mixer)
want = [
    (4000 if i % 2 == 0 else 2000) * (1 if (i // 2) % 200 < 100 else -1) for i in range(len(got))
]
show("stereo pan", got, want)
mixer.deinit()
