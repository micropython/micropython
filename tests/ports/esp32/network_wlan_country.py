# Test WLAN country code get/set - this must be run before any WLAN
# instantiation, ideally right after a board reboot.

import network

network.country("XX")
print(network.country())

network.country("QQ")
try:
    network.WLAN(network.STA_IF)
    print("Failed to raise")
except OSError as e:
    print(str(e))
print(network.country())

network.country("AU")
print(network.country())

wlan = network.WLAN(network.STA_IF)
print(network.country())

try:
    network.country("!!")
    print("Failed to raise")
except OSError as e:
    print(str(e))
print(network.country())

wlan = network.WLAN(network.AP_IF)
print(network.country())
network.country("XX")
print(network.country())

try:
    network.country("FP")
    print("Failed to raise")
except OSError as e:
    print(str(e))
print(network.country())
