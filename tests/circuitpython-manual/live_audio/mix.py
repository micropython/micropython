import audiocore
import audiopwmio
import audiomixer
import board
import array
import time
import math

CHANNELS = 2
RATE = 8000
SAMPLE_TYPE = "H"
OFFSET = 2**15 - 1
BUFFER_SIZE = 640
SINGLE_BUFFER = True
LOOP = True

# (frequency, amp_left, amp_right)
VOICES = ((200, 1, 0), (400, 0, 1), (100, 1, 1))


def play(
    voices=VOICES,
    channels=CHANNELS,
    rate=RATE,
    sample_type=SAMPLE_TYPE,
    offset=OFFSET,
    buffer_size=BUFFER_SIZE,
    single_buffer=SINGLE_BUFFER,
    loop=LOOP,
):
    waves = []
    samples = []
    for v in voices:
        print(v)
        sample_length = int(rate // v[0])
        wave = array.array(sample_type, [offset] * sample_length * channels)
        for i in range(0, sample_length):
            if channels == 1:
                wave[i] = int(
                    math.sin(math.pi * 2 * i / sample_length) * v[1] * (2**15 - 1) + offset
                )
            else:
                wave[2 * i] = int(
                    math.sin(math.pi * 2 * i / sample_length) * v[1] * (2**15 - 1) + offset
                )
                wave[2 * i + 1] = int(
                    math.sin(math.pi * 2 * i / sample_length) * v[2] * (2**15 - 1) + offset
                )
        waves.append(wave)
        samples.append(
            audiocore.RawSample(
                wave, sample_rate=rate, channel_count=channels, single_buffer=single_buffer
            )
        )
    mixer = audiomixer.Mixer(
        voice_count=len(voices),
        sample_rate=rate,
        channel_count=channels,
        bits_per_sample=16,
        samples_signed=False,
        buffer_size=buffer_size,
    )
    pwm = audiopwmio.PWMAudioOut(left_channel=board.D12, right_channel=board.D13)
    pwm.play(mixer)
    for i in range(len(samples)):
        mixer.voice[i].play(samples[i], loop=loop)
        mixer.voice[i].level = 0.5
