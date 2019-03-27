import network
import ubinascii
import machine
from umqtt import MQTTClient
import micropython
from machine import Pin
import upygatt

# ESP8266 ESP-12 modules have blue, active-low LED on GPIO2, replace
# with something else if needed.
led = Pin(2, Pin.OUT, value=0)
adapter = upygatt.GATTToolBackend()

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
        adapter = upygatt.GATTToolBackend()
        adapter.start()
        adapter.connect("E5:FB:01:09:F7:B4")
        # adapter.char_write_handle(handle=0x000e, value=bytearray(b'\x55\x00\xFF\xDF\x24\x0E\xC6\x94\xD1\x97\x43\xaa'), wait_for_response=True)
        # adapter.char_write_handle(handle=0x000c, value=bytearray(b'\x01\x00'), wait_for_response=True)
        # adapter.char_write_handle(handle=0x000e, value=bytearray(b'\x55\x01\x01\xaa'), wait_for_response=True)
        # adapter.char_write_handle(handle=0x000e, value=bytearray(b'\x55\x02\x05\x00\x00\x64\x00\xaa'), wait_for_response=True)
        # adapter.char_write_handle(handle=0x000e, value=bytearray(b'\x55\x03\x03\xaa'), wait_for_response=True)
        # adapter.disconnect("E5:FB:01:09:F7:B4")
    elif msg == b"off":
        led.value(0)
        state = 1
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
