import struct
import synthio
import audiocore


def dump_samples():
    print(struct.unpack("12h", audiocore.get_buffer(s)[1][:24]))


s = synthio.Synthesizer(sample_rate=8000)
print(s.pressed)
dump_samples()

s.press((80,))
print(s.pressed)
dump_samples()

s.press((91,))
print(s.pressed)
dump_samples()

s.release_then_press((80,))
print(s.pressed)
dump_samples()
