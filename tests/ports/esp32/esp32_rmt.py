import time, sys
from machine import Pin
from esp32 import RMT, RMTRX

from target_wiring import rmt_loopback_kwargs

pin_send = Pin(rmt_loopback_kwargs["send"], Pin.OUT)
pin_recv = Pin(rmt_loopback_kwargs["recv"])

# Check the loopback channel is there

pin_send.value(1)
time.sleep_ms(1)
if not pin_recv.value():
    print("Loopback not ok")
pin_send.value(0)
time.sleep_ms(1)
if pin_recv.value():
    print("Loopback not ok")
print("Loopback ok")

pin_send.value(1)
time.sleep_ms(1)

# Bitbang a sequence to RMTRX
# Start and finish with long silence. Last 2 samples are noise.
sequence = [
    -10000,
    300,
    -1000,
    300,
    -300,
    300,
    -1000,
    300,
    -600,
    600,
    -300,
    1000,
    -300,
    300,
    -300,
    600,
    -600,
    1000,
    -300,
    300,
    -600,
    600,
    -600,
    2000,
    -10000,
    500,
    -500,
]
rmtrx = RMTRX(
    pin=pin_recv, min_ns=3100, max_ns=5000 * 1000, resolution_hz=1000 * 1000, soft_min_len=10
)
rmtrx.active(1)

for sample in sequence:
    pin_send.value(sample > 0 and 1 or 0)
    time.sleep_us(abs(sample))

time.sleep_ms(100)
data = rmtrx.get_data()
if not data:
    print("RMTRX did not receive anything")
elif len(data) != (len(sequence) - 4):
    print("RMTRX got sequence of wrong length exp=%d recv=%d" % (len(sequence) - 2, len(data)))
    print(data)
else:
    for i in range(1, len(data) - 3):
        exp_sample = sequence[i + 1]
        recv_sample = data[i]
        if recv_sample * exp_sample < 0:
            print("RMTRX: sample %d of wrong level exp=%d recv=%d" % (i, exp_sample, recv_sample))
            print(data)
            break
        if abs(recv_sample - exp_sample) > 50:
            print("RMTRX: sample %d of wrong length exp=%d recv=%d" % (i, exp_sample, recv_sample))
            print(data)
            break
    else:
        print("RMTRX got bitbang sequence")

# Send the same sequence using RMT TX
rmttx = RMT(pin=pin_send, resolution_hz=1000 * 1000, idle_level=0)
rmttx.write_pulses([abs(sample) for sample in sequence], 0)
rmttx.wait_done(timeout=1000)  # ms

time.sleep_ms(100)
data = rmtrx.get_data()
if not data:
    print("RMTRX did not receive anything from RMTTX")
elif len(data) != (len(sequence) - 4):
    print(
        "RMTRX got sequence from RMTTX of wrong length exp=%d recv=%d"
        % (len(sequence) - 2, len(data))
    )
    print(data)
else:
    for i in range(1, len(data) - 3):
        exp_sample = sequence[i + 1]
        recv_sample = data[i]
        if recv_sample * exp_sample < 0:
            print(
                "RMTRX: sample RMTTX %d of wrong level exp=%d recv=%d"
                % (i, exp_sample, recv_sample)
            )
            print(data)
            break
        if abs(recv_sample - exp_sample) > 1:
            print(
                "RMTRX: sample RMTTX %d of wrong length exp=%d recv=%d"
                % (i, exp_sample, recv_sample)
            )
            print(data)
            break
    else:
        print("RMTRX got RMTTX sequence")

rmttx.deinit()
rmtrx.deinit()
print("RMT stopped")
