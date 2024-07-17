import openamp
import time
from machine import Pin


def ept_recv_callback(src_addr, data):
    print("Received message on endpoint", data)


# Create a new RPMsg endpoint to communicate with main core.
ept = openamp.Endpoint("vuart-channel", callback=ept_recv_callback)

pin = Pin("LED_BLUE", Pin.OUT)

count = 0
while True:
    if ept.is_ready():
        ept.send("Hello from HE %d" % count, timeout=1000)
        count += 1
    time.sleep_ms(100)
    pin(not pin())
