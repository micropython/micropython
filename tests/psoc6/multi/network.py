import network, time


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

    # multitest.next()

    # print(ap_if.status('stations'))

    # status()

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
    print("sta scan finds ap wlan: ", test_ap_net is not None)

    # isconnect()
    print("sta is not (yet) connected: ", sta_if.isconnected() == False)

    # connect()
    sta_if.connect("mpy-psoc6-wlan", "mpy_PSOC6_w3lc0me!")
    print("sta attempt connection to ap")

    # active()
    print("sta is (now) active: ", sta_if.active() == True)

    # isConnected()
    print("sta is (now) connected: ", sta_if.isconnected() == True)

    print(" > yield access point")
    multitest.next()

    # disconnect()
    sta_if.disconnect()
    print("sta is disconnected: ", sta_if.active() == False)

    # status()

    # ifconfig()

    # config()
