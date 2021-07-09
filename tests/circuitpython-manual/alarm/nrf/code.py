import time
import adafruit_ble
from adafruit_ble_eddystone import uid, url
import alarm

np = neopixel.NeoPixel(board.NEOPIXEL, 1)

radio = adafruit_ble.BLERadio()
# Reuse the BLE address as our Eddystone instance id.
eddystone_uid = uid.EddystoneUID(radio.address_bytes)
eddystone_url = url.EddystoneURL("https://adafru.it/discord")

while True:
    np[0] = (50, 0, 0)
    # Alternate between advertising our ID and our URL.
    radio.start_advertising(eddystone_uid)
    time.sleep(0.5)
    radio.stop_advertising()

    radio.start_advertising(eddystone_url)
    time.sleep(0.5)
    radio.stop_advertising()

    ## USB enumeration may take 4-5s per restart
    time_alarm = alarm.time.TimeAlarm(monotonic_time=time.monotonic() + 10)

    np[0] = (0, 0, 0)

    alarm.exit_and_deep_sleep_until_alarms(time_alarm)
