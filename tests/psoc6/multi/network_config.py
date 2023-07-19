import binascii, network, time

channel_new = 5
ssid_new = "mpy-test-conf-wlan"
pass_new = "alicessecret"
sec_new = network.WPA3


# Access Point
def instance0():
    ap_if = network.WLAN(network.AP_IF)
    print("ap instance created")

    # ifconfig()

    # get config()
    print("ap config get channel: ", ap_if.config("channel") == channel_new)
    ap_if.config(channel=channel_new)
    print("ap config get ssid: ", ap_if.config("ssid") == ssid_new)
    ap_if.config(ssid=ssid_new)
    print("ap config get pass: ", ap_if.config("key") == pass_new)
    print("ap config get security: ", ap_if.config("security") == sec_new)
    ap_if.config(sec=ssid_new, key=pass_new)

    # active()
    ap_if.active(True)
    while ap_if.active() == False:
        pass
    print("ap is activated")

    print(" > yield station")
    multitest.next()

    while ap_if.isconnected() == False:
        pass
    print("ap has clients: ", ap_if.isconnected() == True)

    # status()
    stations = ap_if.status("stations")
    print("ap status has stations: ", stations != [])


# Station
def instance1():
    sta_if = network.WLAN(network.STA_IF)
    print("sta instance created")

    # connect()
    sta_if.connect(ssid_new, pass_new)
    print("sta attempt connection to ap")

    # active()
    print("sta is (now) active: ", sta_if.active() == True)

    # isConnected()
    print("sta is (now) connected: ", sta_if.isconnected() == True)

    # config()
    print("sta assoc ap channel config: ", sta_if.config("channel") == channel_new)
    print("sta assoc ap ssid config: ", sta_if.config("ssid") == ssid_new)
    print("sta assoc ap security config: ", sta_if.config("security") == sec_new)

    try:
        sta_if.config("key")  # not for STA
    except ValueError as err:
        print("network config key not for sta: ", err)

    # print(" > yield access point")
    # multitest.next()

    # ifconfig()
