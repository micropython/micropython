import time
from umqtt import MQTTClient
import ubinascii
import machine
import micropython
from network import WLAN, STA_IF
import esp
import g201s
import upygatt
import gc
gc.enable()
gc.collect()
from machine import Pin
import _thread

SERVER = "m24.cloudmqtt.com"
PORT = 18489
USER = b"nyfmvmka"
PASSWORD = b"3ibNbFURtUOR"
CLIENT_ID = ubinascii.hexlify(machine.unique_id())
TOPIC = b"esp32/kettle/#"
P_TOPIC = b"esp32/callback/kettle/get/temperature"

WIFI_SSID_PASSWORD = 'Smart_box-142_g','natalia31081980'
PING_INTERVAL = 60
client = None #mqtt client
sta_if = WLAN(STA_IF)
mqtt_con_flag = False #mqtt connection flag
pingresp_rcv_flag = True #indicator that we received PINGRESP
lock = True #to lock callback function from recursion when many commands are received look mqqt.py line 138
next_ping_time = 0
mqtt_send_flag = False
temp = 0

device = g201s.G201S("E5:FB:01:09:F7:B4")
led = Pin(2, Pin.OUT, value=0)
state = 0

def ping_reset():
    global next_ping_time
    next_ping_time = time.time() + PING_INTERVAL #we use time.time() for interval measuring interval
    print("Next MQTT ping at", next_ping_time)

def ping():
    client.ping()
    ping_reset()


def check():
    global next_ping_time, mqtt_con_flag, pingresp_rcv_flag
    
    if (time.time() >= next_ping_time): #we use time.time() for interval measuring interval
        if not pingresp_rcv_flag :
            mqtt_con_flag = False #we have not received an PINGRESP so we are disconnected
            print("We have not received PINGRESP so broker disconnected")
        else :
            print("MQTT ping at", time.time())
            ping()
            pingresp_rcv_flag = False
    res = client.check_msg()
    if(res == b"PINGRESP") :
        pingresp_rcv_flag = True
        print("PINGRESP")

def sub_cb(topic, msg):
   global lock, state, client, sta_if, device, mqtt_send_flag, temp

   if not lock :
       lock = True
       print((topic, msg))
       if msg == b"on":
           led.value(1)
           state = 0
           sta_if.disconnect()
           sta_if.active(False)
           time.sleep_ms(500)
           device.turn_on()
           print("BT turn ON, size of free heap {0}".format(gc.mem_free()))
           wifi_connect()

       elif msg == b"off":
           led.value(0)
           state = 1
           sta_if.disconnect()
           sta_if.active(False)
           time.sleep_ms(500)
           device.turn_off()
           print("BT turn OFF, size of free heap {0}".format(gc.mem_free()))
           wifi_connect()

       elif msg == b"temperature":
           led.value(0)
           state = 1
           sta_if.disconnect()
           sta_if.active(False)
           time.sleep_ms(500)
           temp = device.get_temperature()
           mqtt_send_flag = True
           print("BT get temperature, size of free heap {0}".format(gc.mem_free()))
           led.value(1)
           wifi_connect()

       elif msg == b"toggle":
           led.value(state)
           state = 1 - state

client = MQTTClient(CLIENT_ID, SERVER,user=USER, password=PASSWORD, port=PORT)
client.set_callback(sub_cb)

def wifi_connect():
    while True:
        try:
            sta_if.active(True)
            sta_if.disconnect()
            sta_if.connect(*WIFI_SSID_PASSWORD)
            print("WIFI CONNECT, size of free heap {0}".format(gc.mem_free()))
            break
            time.sleep_ms(500)
        except Exception as e:
            print("Error in Wlan connect: [Exception] %s: %s" % (type(e).__name__, e))

def mqtt_connect():
    global next_ping_time, pingresp_rcv_flag, mqtt_con_flag, lock, mqtt_send_flag, temp
    
    while not mqtt_con_flag:
        try:
            client.connect()
            client.subscribe(topic=TOPIC,qos=0) #we subscribe with QoS 0 to avoid any retransmission of the command from the broker in case of network failure
            mqtt_con_flag=True
            pingresp_rcv_flag = True
            next_ping_time = time.time() + PING_INTERVAL
            lock = False # we allow callbacks only after everything is set
            print("MQTT CONNECT, size of free heap {0}".format(gc.mem_free()))
            if mqtt_send_flag == True:
                mqtt_send_flag = False
                client.publish(topic=P_TOPIC, qos=1, msg=str(temp))
        except Exception as e:
            print("Error in mqtt connect: [Exception] %s: %s" % (type(e).__name__, e))
        gc.collect()
        time.sleep_ms(500) # to brake the loop

wifi_connect()

def mainthread():
    while True:
        mqtt_connect() #ensure connection to broker
        try:
            check()
        except Exception as e:
            print("Error in Mqtt check message: [Exception] %s: %s" % (type(e).__name__, e))
            print("MQTT disconnected due to network problem")
            lock = True # reset the flags for restart of connection
            mqtt_con_flag = False
        time.sleep_ms(500)

npth = _thread.start_new_thread(mainthread, ())