"""
WLAN test for the CC3200 based boards.
"""

from network import WLAN
import os
import time
import testconfig

mch = os.uname().machine
if not "LaunchPad" in mch and not "WiPy" in mch:
    raise Exception("Board not supported!")


def wait_for_connection(wifi, timeout=10):
    while not wifi.isconnected() and timeout > 0:
        time.sleep(1)
        timeout -= 1
    if wifi.isconnected():
        print("Connected")
    else:
        print("Connection failed!")


wifi = WLAN(0, WLAN.STA)
print(wifi.mode() == WLAN.STA)
print(wifi.antenna() == WLAN.INT_ANT)

wifi = WLAN(mode=WLAN.AP)
print(wifi.mode() == WLAN.AP)
print(wifi.channel() == 1)
print(wifi.auth() == None)
print(wifi.antenna() == WLAN.INT_ANT)
wifi = WLAN(0, mode=WLAN.AP, ssid="test-wlan", auth=(WLAN.WPA, "123456abc"), channel=7)
print(wifi.mode() == WLAN.AP)
print(wifi.channel() == 7)
print(wifi.ssid() == "test-wlan")
print(wifi.auth() == (WLAN.WPA, "123456abc"))
print(wifi.antenna() == WLAN.INT_ANT)

wifi = WLAN(mode=WLAN.STA)
print(wifi.mode() == WLAN.STA)
time.sleep(5)  # this ensures a full network scan
scan_r = wifi.scan()
print(len(scan_r) > 3)
for net in scan_r:
    if net.ssid == testconfig.wlan_ssid:
        # test that the scan results contains the desired params
        print(len(net.bssid) == 6)
        print(net.channel == None)
        print(net.sec == testconfig.wlan_auth[0])
        print(net.rssi < 0)
        print("Network found")
        break

wifi.mode(WLAN.STA)
print(wifi.mode() == WLAN.STA)
wifi.channel(7)
print(wifi.channel() == 7)
wifi.ssid("t-wlan")
print(wifi.ssid() == "t-wlan")
wifi.auth(None)
print(wifi.auth() == None)
wifi.auth((WLAN.WEP, "11223344556677889900"))
print(wifi.auth() == (WLAN.WEP, "11223344556677889900"))
wifi.antenna(WLAN.INT_ANT)
print(wifi.antenna() == WLAN.INT_ANT)

wifi.antenna(WLAN.EXT_ANT)
print(wifi.antenna() == WLAN.EXT_ANT)
time.sleep(2)  # this ensures a full network scan
scan_r = wifi.scan()
print(len(scan_r) > 3)
for net in scan_r:
    if net.ssid == testconfig.wlan_ssid:
        print("Network found")
        break

wifi.antenna(WLAN.INT_ANT)
wifi.mode(WLAN.STA)
print(wifi.mode() == WLAN.STA)
wifi.connect(testconfig.wlan_ssid, auth=testconfig.wlan_auth, timeout=10000)
wait_for_connection(wifi)

wifi.ifconfig(config="dhcp")
wait_for_connection(wifi)
print("0.0.0.0" not in wifi.ifconfig())
wifi.ifconfig(0, ("192.168.178.109", "255.255.255.0", "192.168.178.1", "8.8.8.8"))
wait_for_connection(wifi)
print(wifi.ifconfig(0) == ("192.168.178.109", "255.255.255.0", "192.168.178.1", "8.8.8.8"))
wait_for_connection(wifi)

print(wifi.isconnected() == True)
wifi.disconnect()
print(wifi.isconnected() == False)

t0 = time.ticks_ms()
wifi.connect(testconfig.wlan_ssid, auth=testconfig.wlan_auth, timeout=0)
print(time.ticks_ms() - t0 < 500)

wifi.disconnect()
print(wifi.isconnected() == False)

# test init again
wifi.init(WLAN.AP, ssid="www.wipy.io", auth=None, channel=5, antenna=WLAN.INT_ANT)
print(wifi.mode() == WLAN.AP)

# get the current instance without re-init
wifi = WLAN()
print(wifi.mode() == WLAN.AP)
wifi = WLAN(0)
print(wifi.mode() == WLAN.AP)

# test the MAC address length
print(len(wifi.mac()) == 6)

# next ones MUST raise
try:
    wifi.init(mode=12345)
except:
    print("Exception")

try:
    wifi.init(1, mode=WLAN.AP)
except:
    print("Exception")

try:
    wifi.init(mode=WLAN.AP, ssid=None)
except:
    print("Exception")

try:
    wifi = WLAN(mode=WLAN.AP, channel=12)
except:
    print("Exception")

try:
    wifi.antenna(2)
except:
    print("Exception")

try:
    wifi.mode(10)
except:
    print("Exception")

try:
    wifi.ssid(
        "11111sdfasdfasdfasdf564sdf654asdfasdf123451245ssdgfsdf1111111111111111111111111234123412341234asdfasdf"
    )
except:
    print("Exception")

try:
    wifi.auth((0))
except:
    print("Exception")

try:
    wifi.auth((0, None))
except:
    print("Exception")

try:
    wifi.auth((10, 10))
except:
    print("Exception")

try:
    wifi.channel(0)
except:
    print("Exception")

try:
    wifi.ifconfig(1, "dhcp")
except:
    print("Exception")

try:
    wifi.ifconfig(config=())
except:
    print("Exception")
