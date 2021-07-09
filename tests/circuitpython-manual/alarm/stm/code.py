import alarm
import board
import time
import microcontroller
import sdioio
import storage
import neopixel

np = neopixel.NeoPixel(board.NEOPIXEL, 1)
np[0] = (50, 0, 0)

# SD Card
sd = sdioio.SDCard(
    clock=board.SDIO_CLOCK, command=board.SDIO_COMMAND, data=board.SDIO_DATA, frequency=25000000
)
vfs = storage.VfsFat(sd)
storage.mount(vfs, "/sd")

with open("/sd/log.txt", "a") as sdc:
    temperature = microcontroller.cpu.temperature
    print("Temperature:", temperature)
    sdc.write("{}\n".format(temperature))

## USB enumeration may take 4-5s per restart
time_alarm = alarm.time.TimeAlarm(monotonic_time=time.monotonic() + 10)

np[0] = (0, 0, 0)

alarm.exit_and_deep_sleep_until_alarms(time_alarm)
