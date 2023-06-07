import binascii, network, time


# Access Point
def instance0():
    network.hostname("mpy-psoc6-test")
    print("set hostname: ", network.hostname() == "mpy-psoc6-test")

    ap_if = network.WLAN(network.AP_IF)
    print("ap instance created")

    # active()
    print("ap initially not active: ", ap_if.active() == False)
    ap_if.active(True)
    print("ap is activated: ", ap_if.active() == True)
    ap_if.active(False)
    print("ap is deactivated: ", ap_if.active() == False)
    ap_if.active(True)
    while ap_if.active() == False:
        pass

    # isConnected()
    print("ap has no client: ", ap_if.isconnected() == False)

    multitest.globals(ap_mac=ap_if.config("mac"))

    print(" > yield station")
    multitest.next()

    while ap_if.isconnected() == False:
        pass
    print("ap has clients: ", ap_if.isconnected() == True)

    # try disconnect()
    try:
        ap_if.disconnect()  # not for AP
    except ValueError as err:
        print("ap cannot disconnect: ", err)

    # status()
    stations = ap_if.status("stations")
    print("ap status has stations: ", stations != [])

    # ifconfig()

    # config()


# Station
def instance1():
    sta_if = network.WLAN(network.STA_IF)
    print("sta instance created")

    # active()
    print("sta is not active: ", sta_if.active() == False)

    # scan()
    wlan_nets = sta_if.scan()
    test_ap_net = [net for net in wlan_nets if net[0] == b"mpy-psoc6-wlan"]
    print("sta scan finds ap wlan: ", test_ap_net != [])

    wlan_ssid_filter = sta_if.scan(ssid="mpy-psoc6-wlan")
    test_ap_net = [net for net in wlan_ssid_filter if net[0] == b"mpy-psoc6-wlan"]
    print("sta scan finds ap wlan (ssid filter): ", test_ap_net != [])

    # print('ap_mac: ', binascii.hexlify(ap_mac, ':'))

    wlan_bssid_filter = sta_if.scan(bssid=ap_mac)
    test_ap_net = [net for net in wlan_bssid_filter if net[1] == ap_mac]
    print("sta scan finds ap wlan (mac filter): ", test_ap_net != [])

    # isconnect()
    print("sta is not (yet) connected: ", sta_if.isconnected() == False)

    # connect()
    sta_if.connect("mpy-psoc6-wlan", "mpy_PSOC6_w3lc0me!")
    print("sta attempt connection to ap")

    # active()
    print("sta is (now) active: ", sta_if.active() == True)

    # isConnected()
    print("sta is (now) connected: ", sta_if.isconnected() == True)

    # status()
    print("sta status rssi in range: ", -70 < sta_if.status("rssi") < 0)

    print(" > yield access point")
    multitest.next()

    # disconnect()
    sta_if.disconnect()
    print("sta is disconnected: ", sta_if.active() == False)

    # ifconfig()

    # config()
