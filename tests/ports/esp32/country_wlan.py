# Test WLAN country code get/set - this must be run before any WLAN instantiation.

import network


code1 = "XX"
code2 = "AU"

try:
    network.country(code1)
    print("FAILED TO RAISE")
except OSError as e:
    print(e)

wlan = network.WLAN(network.STA_IF)
current = network.country()
print(current in (code1, code2))
if current == code1:
    network.country(code2)
    print(network.country() == code2)
else:
    network.country(code1)
    print(network.country() == code1)
try:
    network.country("")
    print("FAILED TO RAISE")
except RuntimeError as e:
    print(e)
