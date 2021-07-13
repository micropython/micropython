import audiocore
import audiopwmio
import board
import digitalio
import time
import math
import os

trigger = digitalio.DigitalInOut(board.D4)
trigger.switch_to_output(True)

sample_prefix = "jeplayer-splash"

samples = []
for fn in os.listdir("/"):
    if fn.startswith(sample_prefix):
        samples.append(fn)

dac = audiopwmio.PWMAudioOut(left_channel=board.D12, right_channel=board.D13)
for filename in samples:
    print("playing", filename)
    with open(filename, "rb") as sample_file:
        try:
            sample = audiocore.WaveFile(sample_file)
        except OSError as e:
            print(e)
            continue
        trigger.value = False
        dac.play(sample)
        while dac.playing:
            time.sleep(1)
        trigger.value = True
    time.sleep(0.1)
    print()

print("done")
