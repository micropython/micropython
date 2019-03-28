import time
from umqtt import MQTTClient
import ubinascii
import machine
import micropython
import network
import esp
import g201s
import upygatt
import gc
gc.collect()
from machine import Pin

SERVER = "m24.cloudmqtt.com"
PORT = 18489
USER = b"nyfmvmka"
PASSWORD = b"3ibNbFURtUOR"
CLIENT_ID = ubinascii.hexlify(machine.unique_id())
TOPIC = b"esp32/kettle/#"

# ESP8266 ESP-12 modules have blue, active-low LED on GPIO2, replace with something else if needed.
time.sleep(2)
led = Pin(2, Pin.OUT, value=0)
wlan = network.WLAN(network.STA_IF)
device = g201s.G201S("E5:FB:01:09:F7:B4")
c = MQTTClient(CLIENT_ID, SERVER, PORT, USER, PASSWORD)

# Received messages from subscriptions will be delivered to this callback
state = 0
mqtt_state = 0

def sub_cb(topic, msg):
    global state
    print((topic, msg))
    if msg == b"on":
        led.value(1)
        state = 0
        wlan.active(False)
        time.sleep(1)
        device.turn_on()
        time.sleep(1)
        wifi()
    elif msg == b"off":
        led.value(0)
        state = 1
        wlan.active(False)
        time.sleep(1)
        device.turn_off()
        time.sleep(1)
        wifi()
    elif msg == b"toggle":
        # LED is inversed, so setting it to current state value will make it toggle
        led.value(state)
        state = 1 - state

def mqtt():
    # Subscribed messages will be delivered to this callback
    global mqtt_state
    if mqtt_state == 1:
        c.disconnect()
    c.set_callback(sub_cb)
    c.connect()
    c.subscribe(TOPIC)
    mqtt_state = 1
    print("Connected to %s, subscribed to %s topic" % (SERVER, TOPIC))
    try:
        while 1:
            micropython.mem_info()
            c.wait_msg()
    finally:
        c.disconnect()
        mqtt_state = 0

def wifi():
    if not wlan.active() or not wlan.isconnected():
        wlan.active(True)
        wlan.connect("Smart_box-142", "natalia31081980")
    while not wlan.isconnected():
        pass
    print('network config:', wlan.ifconfig())
    mqtt()

wifi()
