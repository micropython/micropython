import urequests
import valuestore
from machine import RTC

def timesync():
    timezone = valuestore.load("system", "timezone")
    if not timezone:
        timezone = "Europe/Amsterdam"
    
    res = urequests.get("http://worldtimeapi.org/api/timezone/"+timezone)

    if res.status_code == 200:
        data = res.json()
        print(data)
        tm = data["unixtime"] - 946684800 + data["raw_offset"] + data["dst_offset"]

        rtc = RTC()
        rtc.init(tm)