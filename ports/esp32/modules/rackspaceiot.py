import network
import esp32
import json
import os
from time import sleep, time
from machine import Pin, TouchPad

config_file = "./rackspace_iot_cfg.json"
ap_prefix = "RackspaceIoT"
ap_passwd = b"rackspaceiot"
timeout = 15 # seconds, used for wlan connection and scan timeouts
capacative_touch = TouchPad(Pin(12))
onboard_led = Pin(15, Pin.OUT)
button = Pin(2, Pin.IN)
sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)

def test_path(fname):
    try:
        os.stat(fname)
        return True
    except OSError:
        return False

def get_config():
    with open(config_file) as f:
        config = json.load(f)
        f.close()
    return config

def get_wlans(config):
    matches = []
    now = time()
    while time() < now + timeout and not matches:
        print('wifi scanning')
        sta_if.active(True)
        foundNetworks = {n[0].decode("utf-8"):n for n in sta_if.scan()}
        matches = [x for x in config['wlans'] if x['essid'] in foundNetworks]
    if matches:
        for match in matches:
            print("found wlan %s" % match['essid'])
    return matches

def connect_sta_if():
    from time import time
    try:
        if test_path(config_file):
            print('config file %s found' % config_file)
            config = get_config()
            wlans = get_wlans(config)
            sta_if.disconnect()
            for wlan in wlans:
                print('connecting to wlan %s/%s...' % (wlan['friendly_name'],wlan['essid']))
                sta_if.connect(wlan['essid'],wlan['passwd'])
                now = time()
                while time() < now + timeout:
                    if sta_if.isconnected():
                        print('connection to wlan %s/%s successful! To continue: ' % (wlan['friendly_name'],wlan['essid']))
                        print('1. reconnect your workstation to wifi network: %s' % wlan['essid'])
                        print('2. once reconnected, connect to webrepl on: ')
                        print('ws://%s:8266/' % sta_if.ifconfig()[0])
                        return True
            return False
        else:
            print('config file not found')
            return False
    except:
        return False

def enable_ap_if():
    print('enabling access point')
    import ubinascii
    ap_if = network.WLAN(network.AP_IF)
    sta_if.active(False)
    ap_if.active(True)
    essid = b"%s-%s" % (ap_prefix, ubinascii.hexlify(ap_if.config("mac")[-3:]))
    ap_if.config(essid=essid, authmode=network.AUTH_WPA_WPA2_PSK, password=ap_passwd)
    blink(100,5) #blink  onboard LED to indicate the device has entered AP mode

def temp():
    return esp32.raw_temperature()

def hall():
    return esp32.hall_sensor()

def touch():
    return capacative_touch.read()

def read_all():
    sensors ={"temp": temp(), "touch": touch(), "hall": hall()}
    print(json.dumps(sensors))

def led(state):
    if state.lower()=="on":
        onboard_led.value(0)
    elif state.lower()=="off":
        onboard_led.value(1)
    else:
        return "invalid state passed for led.  valid values are `on` and `off`"

def blink(delay_ms, duration_sec):
    try:
        int_test = delay_ms/1
        int_test = duration_sec/1
    except:
        return "invalid value(s) passed for blink.  Accepts only numeric values for delay_ms and duration_sec"
    ms = delay_ms/1000
    now = time()
    while time() < now + duration_sec:
        led("on")
        sleep(ms)
        led("off")
        sleep(ms)


def setup():
    if button.value():
        print('button press detected during boot')
        enable_ap_if()
    elif not connect_sta_if():
        print('wifi connection failed')
        enable_ap_if()