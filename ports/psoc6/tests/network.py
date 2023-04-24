import machine, network

print("Testing Network")

import network

sta_if = network.WLAN(network.STA_IF)
sta_if.config(trace=1)

print("deactivating network")
sta_if.active(False)  # disable wifi if currently active - in case of issues from a previous run
# sta_if.deinit()
# pyb.delay(5000)

print("setting active")
sta_if.active(True)
# pyb.delay(5000)


print("setting antenna")
sta_if.config(antenna=0)  # select antenna, 0=chip, 1=external
# pyb.delay(5000)

print("connecting")
sta_if.connect("ssid", "password")
# pyb.delay(5000)

print("checking if is connected")
while not sta_if.isconnected():
    pyb.delay(5000)

    # Check the status
    print("getting status")
    status = sta_if.status()

    if status <= 0:
        # Error States?
        return False

    print("checking if is connected")


import machine, network

sta_if = network.WLAN(network.STA_IF)
count = 0

if not sta_if.isconnected():
    print("connecting to hotspot...")
    #    sta_if.active(True)
    sta_if.ifconfig(("192.168.10.99", "255.255.255.0", "192.168.10.1", "8.8.8.8"))
    sta_if.connect("HUAWEI-E8231-c4fd", "my_password")
