import gc
import uos
import time
from umqtt import MQTTClient
import ubinascii
import machine
import micropython
import network
import esp
from flashbdev import bdev

try:
    if bdev:
        uos.mount(bdev, '/')
except OSError:
    import inisetup
    vfs = inisetup.setup()

gc.collect()

wlan = network.WLAN(network.STA_IF)
if not wlan.active() or not wlan.isconnected():
wlan.active(True)
print('connecting to:', ssid)
wlan.connect("Smart_box-142", "natalia31081980")
while not wlan.isconnected():
    pass
print('network config:', wlan.ifconfig())
