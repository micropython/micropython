import audiobusio
import board
import digitalio
import array
import time
import math

trigger = digitalio.DigitalInOut(board.D4)
trigger.switch_to_output(True)


def mean(values):
    return sum(values) / len(values)


def normalized_rms(values):
    minbuf = int(mean(values))
    samples_sum = sum(float(sample - minbuf) * (sample - minbuf) for sample in values)

    return math.sqrt(samples_sum / len(values))


# signed 16 bit
s16 = array.array("H", [0] * 10000)

pdm = audiobusio.PDMIn(clock_pin=board.D11, data_pin=board.D12, sample_rate=24000, bit_depth=16)

print("starting read")
trigger.value = False
count = pdm.record(s16, len(s16))
trigger.value = True
print("read done")
print("recorded {} samples".format(count))
for v in s16[:count]:
    print(v)


magnitude = normalized_rms(s16)
print("magnitude", magnitude)

print("count", count)

print("done")
