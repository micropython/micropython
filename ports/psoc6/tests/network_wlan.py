import network

print("Testing PSoC6 Network module")

sta_if = network.WLAN(network.STA_IF)

ssid = "OnePlus 9 5G"
password = "psoc-mpy"
ifconfig_params = ("192.168.255.75", "255.255.255.0", "192.168.255.73", "192.168.255.73")

if not sta_if.isconnected():
    print("Status: ", isconnected())
    print("Connecting to hotspot...")
    sta_if.ifconfig(ifconfig_params)
    sta_if.connect(ssid, password)
    if sta_if.ifconfig() == ifconfig_params:
        print("ifconfig params match")
    if sta_if.active() == 1:
        print("Successfully connected")

sta_if.disconnect()
