access_point = None

def enable_ap(ssid : str):
    global access_point
    import network

    if access_point is None:
        print('enabling ap')
        access_point = network.WLAN(network.AP_IF)
        access_point.active(True)
        access_point.config(essid=ssid)

def disable_wifi():
    ...
