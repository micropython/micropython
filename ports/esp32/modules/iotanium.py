import network
import esp32
import json
from time import sleep, time
from machine import Pin, TouchPad

config_file = "./iotanium_cfg.json"
ap_prefix = "IoTanium"
ap_passwd = b"iotanium"
capacative_touch = TouchPad(Pin(12))
onboard_led = Pin(15, Pin.OUT)
button = Pin(2, Pin.IN)

def test_path(fname):
    try:
        with open(fname):
            pass
        return True
    except OSError:
        return False

def get_config():
    with open(config_file) as f:
        config = json.load(f)
        f.close()
    return config

def connect_sta_if():
    try:
        from time import time
        if test_path(config_file):
            print('config file %s found' % config_file)
            config = get_config()
            sta_if = network.WLAN(network.STA_IF)
            for wlan in config['wlans']:
                sta_if.active(False)
                print('connecting to wlan %s/%s...' % (wlan['friendly_name'],wlan['essid']))
                sta_if.active(True)
                sta_if.connect(wlan['essid'],wlan['passwd'])
                now = time()
                while time() < now + config['timeout_seconds']:
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
    ap_if.active(False)
    essid = b"%s-%s" % (ap_prefix, ubinascii.hexlify(ap_if.config("mac")[-3:]))
    ap_if.config(essid=essid, authmode=network.AUTH_WPA_WPA2_PSK, password=ap_passwd)
    ap_if.active(True)

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
        blink(100,5) #blink  onboard LED to notify user that AP mode has been enabled by button press
    elif not connect_sta_if():
        print('wifi connection failed')
        enable_ap_if()