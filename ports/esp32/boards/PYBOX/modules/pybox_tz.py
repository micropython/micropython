"""
    Fetch Time Zone info from timeapi.io
    Author: wolf@paulus.com
    MIT license; Copyright (c) 2022 wolfpaulus.com
"""
from urequests import request
from utime import sleep
import pybox_log as log


def get_time_stamp(tz: str) -> dict:
    """
    Fetches the current time stamp at a given time zone:
    :param tz: str
    :return:   {
    "year": 2022,
    "month": 12,
    "day": 18,
    "hour": 11,
    "minute": 4,
    "seconds": 2,
    "milliSeconds": 692,
    "dateTime": "2022-12-18T11:04:02.6927298",
    "date": "12/18/2022",
    "time": "11:04",
    "timeZone": "America/Phoenix",
    "dayOfWeek": "Sunday",
    "dstActive": false
    }
    """
    url = f"https://timeapi.io/api/Time/current/zone?timeZone={tz}"
    headers = {"accept": "application/json"}
    for i in range(1, 4):
        try:
            return request("GET", url, headers=headers).json()
        except Exception as e:
            log.log(log.ERROR, str(e))
            sleep(i)
    return {}
