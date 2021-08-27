import alarm
import board
import time
import microcontroller
import storage

temperature = microcontroller.cpu.temperature
print("Temperature:", temperature)

try:
    with open("/log.txt", "a") as sdc:
        sdc.write("{}\n".format(temperature))
except OSError as e:
    print("Cannot write to fs, is GP4 grounded?")

## USB enumeration may take 4-5s per restart
time_alarm = alarm.time.TimeAlarm(monotonic_time=time.monotonic() + 10)

alarm.exit_and_deep_sleep_until_alarms(time_alarm)
