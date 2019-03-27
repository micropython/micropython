import network
import ubinascii
import machine
from umqtt import MQTTClient
import micropython
from machine import Pin
import upygatt, g201s

# ESP8266 ESP-12 modules have blue, active-low LED on GPIO2, replace
# with something else if needed.
led = Pin(2, Pin.OUT, value=0)
device = g201s.G201S("E5:FB:01:09:F7:B")

SERVER = "m24.cloudmqtt.com"
PORT = 18489
USER = b"nyfmvmka"
PASSWORD = b"3ibNbFURtUOR"
CLIENT_ID = ubinascii.hexlify(machine.unique_id())
TOPIC = b"ESP32/kettle/#"

def do_connect():
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    if not wlan.isconnected():
        print('connecting to network...')
        wlan.connect('Smart_box-142', 'natalia31081980')
        while not wlan.isconnected():
            pass
    print('network config:', wlan.ifconfig())
    mqtt()

# Received messages from subscriptions will be delivered to this callback
state = 0

def sub_cb(topic, msg):
    global state
    print((topic, msg))
    if msg == b"on":
        led.value(1)
        state = 0
        device.turn_on()
    elif msg == b"off":
        led.value(0)
        state = 1
        device.turn_off()
    elif msg == b"toggle":
        # LED is inversed, so setting it to current state
        # value will make it toggle
        led.value(state)
        state = 1 - state

def mqtt(server=SERVER, port=PORT, user=USER, password=PASSWORD):
    c = MQTTClient(CLIENT_ID, server, port, user, password)
    # Subscribed messages will be delivered to this callback
    c.set_callback(sub_cb)
    c.connect()
    c.subscribe(TOPIC)
    print("Connected to %s, subscribed to %s topic" % (server, TOPIC))
    try:
        # while 1:
            #micropython.mem_info()
        c.wait_msg()
    finally:
        c.disconnect()

do_connect()
