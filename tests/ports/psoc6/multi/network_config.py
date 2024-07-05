import binascii, time

try:
    import network
except ImportError:
    print("SKIP")
    raise SystemExit

channel_new = 5
ssid_new = "mpy-test-conf-wlan"
pass_new = "alicessecret"
sec_new = network.WLAN.WPA2

ap_ip = "10.20.0.1"
gateway_ip = ap_ip
netmask_ip = "255.255.255.128"
dns_ip = "0.0.0.0"
ap_net_conf = (ap_ip, netmask_ip, gateway_ip, dns_ip)


# Access Point
def instance0():
    ap_if = network.WLAN(network.AP_IF)
    print("ap instance created")

    # get config()
    ap_if.config(channel=channel_new)
    print("ap config get channel: ", ap_if.config("channel") == channel_new)
    ap_if.config(ssid=ssid_new)
    print("ap config get ssid: ", ap_if.config("ssid") == ssid_new)
    ap_if.config(security=sec_new, key=pass_new)
    print("ap config get security: ", ap_if.config("security") == sec_new)
    try:
        ap_if.config("password")  # only if default
    except ValueError as err:
        print(err)

    # active()
    ap_if.active(True)
    while ap_if.active() == False:
        pass
    print("ap is activated")

    # set ifconfig()
    ap_if.ifconfig(ap_net_conf)

    # get ifconfig()
    print("ap ip settings: ", ap_if.ifconfig() == ap_net_conf)

    print(" > yield station")
    multitest.next()

    while ap_if.isconnected() == False:
        pass
    print("ap has clients: ", ap_if.isconnected() == True)

    # status()
    stations = ap_if.status("stations")
    print("ap status has stations: ", stations != [])

    print(ap_if)


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
        print(err)

    # ifconfig()
    sta_net_conf = ("10.20.0.2", netmask_ip, ap_ip, ap_ip)
    print("sta ip settings: ", sta_if.ifconfig() == sta_net_conf)

    try:
        sta_if.ifconfig(sta_net_conf)  # not for STA
    except ValueError as err:
        print(err)

    print(sta_if)
