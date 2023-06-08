from synthio import Synthesizer

s = Synthesizer(sample_rate=48000)


def print_filter(x):
    print(" ".join(f"{v:.4g}" for v in x))


print_filter(s.low_pass_filter(330))
print_filter(s.high_pass_filter(330))
print_filter(s.band_pass_filter(330))
