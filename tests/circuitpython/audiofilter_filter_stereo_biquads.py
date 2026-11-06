from audiofilters import Filter
from audiofilterhelper import synth_test, white8k
from audiomixer import Mixer
from synthio import Biquad, FilterMode, LFO


@synth_test
def stereo_filter():
    args = {
        "bits_per_sample": 16,
        "samples_signed": True,
        "channel_count": 2,
    }
    effect = Filter(
        filter=(
            Biquad(FilterMode.LOW_PASS, 400),
            Biquad(FilterMode.HIGH_PASS, 300, Q=8),
        ),
        **args,
    )
    mixer = Mixer(**args)
    mixer.voice[0].panning = LFO()
    effect.play(mixer)
    yield effect, []

    mixer.play(white8k, loop=True)
    yield 400
