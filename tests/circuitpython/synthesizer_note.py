import struct
import synthio
import audiocore


def dump_samples():
    print([i for i in audiocore.get_buffer(s)[1][:24]])


n80 = synthio.Note(synthio.midi_to_hz(80))
n91 = synthio.Note(synthio.midi_to_hz(80))

s = synthio.Synthesizer(sample_rate=8000)
print(s.pressed)
dump_samples()

s.press((n80,))
print(s.pressed)
dump_samples()

s.press((n91,))
print(s.pressed)
dump_samples()

s.release_then_press((n80,))
print(s.pressed)
dump_samples()

envelope = synthio.Envelope(
    attack_time=0.1, decay_time=0.05, release_time=0.2, attack_level=1, sustain_level=0.8
)
n60 = synthio.Note(synthio.midi_to_hz(60), envelope=envelope)
s = synthio.Synthesizer(sample_rate=8000, envelope=envelope)
s.press((n60,))
for _ in range(12):
    buf = audiocore.get_buffer(s)[1]
    print((min(buf), max(buf)))
s.release_all()
for _ in range(12):
    buf = audiocore.get_buffer(s)[1]
    print((min(buf), max(buf)))
