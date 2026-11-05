from audiofilters import Filter
from audiofilterhelper import synth_test, white8k
from synthio import Biquad, FilterMode


@synth_test
def basic_filter():
    effect = Filter(
        filter=(
            Biquad(FilterMode.LOW_PASS, 400),
            Biquad(FilterMode.HIGH_PASS, 300, Q=8),
        ),
        bits_per_sample=16,
        samples_signed=True,
    )
    yield effect, []

    effect.play(white8k, loop=True)
    yield 400
