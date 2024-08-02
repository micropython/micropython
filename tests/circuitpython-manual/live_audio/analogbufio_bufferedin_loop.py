import analogbufio
import array
import audiocore
import audiopwmio
import board

samples = 100
buffer = array.array("H", [0x0000] * samples)
adc = analogbufio.BufferedIn(board.A0, sample_rate=100000)

adc.readinto(buffer)

print("Sample,Print 1, Print 2,Print 3, Print 4")
for i in range(4):
    for j in range(samples):
        print(j, "," * (i + 1), buffer[j])

adc.readinto(buffer, loop=True)

print("Sample,Print 1, Print 2,Print 3, Print 4")
for i in range(4):
    for j in range(samples):
        print(j, "," * (i + 1), buffer[j])
