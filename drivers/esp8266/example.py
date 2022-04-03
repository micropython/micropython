"""
File name: example.py
Michal Pindiak, hobbyastro}at[gmail(com
Inspired by https://github.com/Circuit-Digest/Raspberry_Pi_Pico_Tutorial/tree/main/T7_Interfacing_ESP8266-01_WiFi
"""

from esp8266 import ESP8266
import time
import sys
from settings import *

def start():
    print("OS version: ", sys.implementation.name, sys.version)
    esp8622board = ESP8266(uart_port, uart_baud_rate, uart_tx_pin, uart_rx_pin)
    print("\nESP8266 initialisation: ", esp8622board.initialise_ESP8266())
    # print("\nESP8266 Reset: ",esp8622board.reset_ESP8266())
    # print("\nESP8266 Restore: ",esp8622board.restore_ESP8266())
    print("\nESP8266 AT commands echoing: ", esp8622board.echo_commands())
    # Set the current WiFi mode
    # 1: Station mode
    # 2: SoftAP mode
    # 3: SoftAP+Station mode
    status = esp8622board.get_status()
    print("\nESP8266 current status: ", status)
    return(esp8622board)

def view_ESP8266_version():
    # Show ESP8266 AT command version and SDK details
    version = esp8622board.view_version_info()
    if(version is not None):
        print("\nESP8266 version info:")
        print(version)

def scan_for_networks():
    # List available WiFi networks
    print("\nScanning for WiFi networks")
    access_points = esp8622board.list_available_access_points()
    print("Found WiFi networks:")
    for items in access_points:
        print(items)

def connect_wifi():
    # Connects to WiFi
    print("\nTrying to connect to WiFi with SSID: {ssid}".format(ssid=ssid))
    message = esp8622board.connect_wifi(ssid, password)
    status = esp8622board.get_status()
    ip = esp8622board.get_ip()
    print(message)
    print("\nESP8266 current status: ", status)
    print(ip)

def disconnect_wifi():
    # Disconnects from WiFi
    print("\nTrying to disconnect from WiFi: ", esp8622board.disconnect_wifi())
    status = esp8622board.get_status()
    print("ESP8266 current status: ", status)

def get_default_mode():
    # Get the default ESP8266 mode
    message = esp8622board.get_default_mode()
    print("\nDefault ESP8266 mode: ", message)

def get_current_mode():
    # Get the current ESP8266 mode
    message = esp8622board.get_current_mode()
    print("\nCurrent ESP8266 mode: ", message)

def http_get(gethost, getpath, getuseragent, getcontenttype, getpayload, getport):
    # HTTP Get
    print("\nHTTP Get")
    response = esp8622board.doHttpGet(gethost, getpath, getuseragent, getcontenttype, getpayload, getport)
    print("Response code:", response["code"], "\n")
    print("Response headers:\n")
    for _ in response["headers"]:
        print(_)
    print("\n")
    print("Response body:")
    print(response["body"], "\n")

def http_put(puthost, putpath, putuseragent, putcontenttype, putpayload, putport):
    # HTTP Put
    print("\nHTTP Put")
    response = esp8622board.doHttpPut(puthost, putpath, putuseragent, putcontenttype, putpayload, putport)
    print("Response code:", response["code"], "\n")
    print("Response headers:\n")
    for _ in response["headers"]:
        print(_)
    print("\n")
    print("Response body:")
    print(response["body"], "\n")

def http_post(posthost, postpath, postuseragent, postcontenttype, postpayload, postport):
    # HTTP post
    print("\nHTTP Post")
    response = esp8622board.doHttppost(posthost, postpath, postuseragent, postcontenttype, postpayload, postport)
    print("Response code:", response["code"], "\n")
    print("Response headers:\n")
    for _ in response["headers"]:
        print(_)
    print("\n")
    print("Response body:")
    print(response["body"], "\n")


esp8622board = start()
view_ESP8266_version()
scan_for_networks()
connect_wifi()
get_default_mode()
get_current_mode()
http_get(gethost, getpath, getuseragent, getcontenttype, getpayload, getport)
http_put(puthost, putpath, putuseragent, putcontenttype, putpayload, putport)
http_post(posthost, postpath, postuseragent, postcontenttype, postpayload, postport)
disconnect_wifi()
