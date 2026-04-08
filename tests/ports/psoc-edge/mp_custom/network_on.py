import network
import secrets as s

from utime import sleep

wlan = network.WLAN(network.STA_IF)
if wlan.isconnected():
    print("[network-module] : Already connected")

# enable and connect wlan
wlan.connect(s.ssid, s.key)
# wait for connection to establish
sleep(5)

for i in range(0, 100):
    if not wlan.isconnected():
        print("[Network] Waiting to connect..")
        sleep(2)

if not wlan.active():
    print("[network-module] : Connection failed.Try again!")
