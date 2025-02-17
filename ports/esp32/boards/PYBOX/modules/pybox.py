"""
    NeoPixel, BlueLED, SDDrive, WiFi for MicroPython on SparkFun Thing Plus
    Author: wolf@paulus.com
    MIT license; Copyright (c) 2022 wolfpaulus.com
"""
from micropython import const
from machine import freq, Pin, SDCard, Signal, RTC
from utime import sleep
from network import WLAN, STA_IF
from neopixel import NeoPixel
from uos import mount
from pybox_btn import Button
from collections import OrderedDict

_BTN_UP_PIN = const(27)
_BTN_DN_PIN = const(33)
_TURBO_SPEED = const(240000000)
_NORMAL_SPEED = const(160000000)
_BLUE_LED_PIN = const(13)
_NEO_PIN = const(2)


class Neo:
    """
    NeoPixel
    """

    def __init__(self, pin: int = 2):
        self.neo = NeoPixel(Pin(pin, Pin.OUT, value=False), 1)

    def color(self, r: int = 255, g: int = 255, b: int = 255):
        self.neo[0] = (r, g, b)
        self.neo.write()

    def get_color(self) -> (int, int, int):
        return self.neo[0]

    def off(self) -> None:
        self.neo[0] = (0, 0, 0)
        self.neo.write()


def turbo(on: bool = True) -> None:
    """
    Switch ESP's clock speed
    :param on: 240MHz is True else, defaulting to 160MHz
    :return: None
    """
    freq(_TURBO_SPEED if on else _NORMAL_SPEED)


def mount_sd(directory: str = "/sd") -> None:
    """
    Define removable storage media - secure digital memory card
    """
    sd = SDCard(slot=2, width=1, cd=None, wp=None, sck=18, miso=19, mosi=23, cs=5, freq=40000000)
    mount(sd, directory)


def connect(wifi_networks: list) -> bool:
    """
    Connect to Wifi router or become an Access Point
    :param wifi_networks: list of tuples [(ssid, password),..]
    :return: True if connected
    """
    global hostname, ip_addr
    conn = False
    while not conn:
        for ssid, pw in wifi_networks:
            print(ssid)
            sta_if = WLAN(STA_IF)
            if not sta_if.isconnected():
                sta_if.active(True)
                sta_if.connect(ssid, pw)
                # sta_if.config(dhcp_hostname=profile.HostName)
            for i in range(2, 8):
                sta_if = WLAN(STA_IF)
                if sta_if.isconnected():
                    conn = True
                    ip_addr = sta_if.ifconfig()[0]
                    hostname = sta_if.config("dhcp_hostname")
                    break
                else:
                    sleep(i)
            if conn:
                break
    return conn


def is_connected() -> bool:
    """
    :return: True is connected to WiFi
    """
    return WLAN(STA_IF).isconnected()


def set_system_time(d: dict) -> None:
    """
    Set the Systemtime
    :param d: dictionary
    :return:
    """
    RTC().datetime((d["year"], d["month"], d["day"], 0, d["hour"], d["minute"], d["seconds"], 0))


def rgb_color() -> (int, int, int):
    """
    RGB color generator  (r,g,b in the range 0..255)
    :return: (r,g,b all in the range 0..255)
    """
    for i in range(1, 86):
        yield 255 - i * 3, 0, i * 3
    for i in range(1, 86):
        yield 0, i * 3, 255 - i * 3
    for i in range(1, 86):
        yield i * 3, 255 - i * 3, 0


palette = OrderedDict(
    [
        # red
        ("Auriga", (255, 0, 63)),
        ("Antares", (255, 0, 0)),
        # orange
        ("Jupiter", (255, 32, 0)),
        ("Proxima", (191, 128, 0)),
        # gray
        ("Sirius", (255, 255, 255)),
        ("Vega", (127, 127, 191)),
        # green
        ("Nebula", (0, 255, 0)),
        ("Kepler", (0, 127, 63)),
        ("Uranus", (0, 191, 255)),
        # blue
        ("Rigel", (31, 31, 255)),
        ("Medusa", (127, 0, 255)),
        ("Orion", (255, 31, 255)),
    ]
)

blue_led = Signal(Pin(_BLUE_LED_PIN, mode=Pin.OUT, value=False), invert=False)
blue_led.off()
neo = Neo(_NEO_PIN)
neo.color(0, 0, 0)
btn_up = Button(_BTN_UP_PIN)
btn_dn = Button(_BTN_DN_PIN)
hostname = ""
ip_addr = "0.0.0.0"
