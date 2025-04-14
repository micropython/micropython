"""
    Minimal Logger
    Author: Wolf Paulus wolf@paulus.com
    MIT license; Copyright (c) 2022 wolfpaulus.com
"""
from micropython import const
from utime import localtime
from uos import remove

CRITICAL = const(50)
ERROR = const(40)
WARNING = const(30)
INFO = const(20)
DEBUG = const(10)

LOG_FILE = "/log.txt"

log_level = WARNING


def reset() -> None:
    try:
        remove(LOG_FILE)
    except:
        pass


def log(level: int, msg: str):
    try:
        if level >= log_level:
            with open(LOG_FILE, "a") as file:
                file.write(f"{level} : {localtime()} : {msg}\n")
    except:
        pass
