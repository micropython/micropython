import audiocore
import audiobusio
import board
import digitalio
import array
import time
import math
import rp2pio
import adafruit_pioasm

time.sleep(10)

trigger = digitalio.DigitalInOut(board.D4)
trigger.switch_to_output(True)

# Generate one period of sine wav.
length = 8000 // 440

# signed 16 bit
s16 = array.array("h", [0] * length)
for i in range(length):
    s16[i] = int(math.sin(math.pi * 2 * i / length) * (2 ** 15))
    print(s16[i])

sample = audiocore.RawSample(s16, sample_rate=8000)

dac = audiobusio.I2SOut(bit_clock=board.D10, word_select=board.D11, data=board.D12)

trigger.value = False
dac.play(sample, loop=True)
time.sleep(1)
dac.stop()
trigger.value = True

print("done")
