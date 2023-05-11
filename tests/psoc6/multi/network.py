import network

print("### Testing Network ###")


# Access Point
def instance0():
    network.hostname("mpy-psoc6-test")
    print(network.hostname() == "mpy-psoc6-test")

    ap_if = network.WLAN(network.AP_IF)

    # active()
    print(ap_if.active() == False)
    ap_if.active(True)
    print(ap_if.active() == True)
    ap_if.active(False)
    print(ap_if.active() == False)
    ap_if.active(True)
    while ap_if.active() == False:
        pass

    multitest.next()

    # isConnected()

    # status()

    # ifconfig()

    # config()


# Station
def instance1():
    multitest.next()

    sta_if = network.WLAN(network.STA_IF)

    # active()
    print(sta_if.active() == False)

    # scan()
    wlan_nets = sta_if.scan()

    test_ap_net = [net for net in wlan_nets if net[0] == b"mpy-psoc6-wlan"]

    print(test_ap_net is not None)

    # connect()
    sta_if.connect("mpy-psoc6-wlan", "mpy_PSOC6_w3lc0me!")

    # active()
    print(sta_if.active() == True)

    # isConnected()

    # disconnect()

    # status()

    # ifconfig()

    # config()
