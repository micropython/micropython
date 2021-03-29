import audiocore
import audiopwmio
import board
import digitalio
import array
import time
import math

trigger = digitalio.DigitalInOut(board.D4)
trigger.switch_to_output(True)

# Generate one period of sine wav.
length = 8000 // 440

samples = []
sample_names = ["unsigned 8 bit", "signed 8 bit", "unsigned 16 bit", "signed 16 bit"]


# unsigned 8 bit
u8 = array.array("B", [0] * length)
for i in range(length):
    u8[i] = int(math.sin(math.pi * 2 * i / length) * (2 ** 7) + 2 ** 7)

samples.append(audiocore.RawSample(u8, sample_rate=4000))

# signed 8 bit
s8 = array.array("b", [0] * length)
for i in range(length):
    s8[i] = int(math.sin(math.pi * 2 * i / length) * (2 ** 7))

samples.append(audiocore.RawSample(s8, sample_rate=16000))

# unsigned 16 bit
u16 = array.array("H", [0] * length)
for i in range(length):
    u16[i] = int(math.sin(math.pi * 2 * i / length) * (2 ** 15) + 2 ** 15)

samples.append(audiocore.RawSample(u16, sample_rate=8000))


# signed 16 bit
s16 = array.array("h", [0] * length)
for i in range(length):
    s16[i] = int(math.sin(math.pi * 2 * i / length) * (2 ** 15))

samples.append(audiocore.RawSample(s16, sample_rate=8000))


dac = audiopwmio.PWMAudioOut(board.D13)
for sample, name in zip(samples, sample_names):
    print(name)
    trigger.value = False
    dac.play(sample, loop=True)
    time.sleep(1)
    dac.stop()
    time.sleep(0.1)
    trigger.value = True
    print()

print("done")
