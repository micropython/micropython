import time
from hwconfig import LED, BUTTON

# Light LED when (and while) a BUTTON is pressed

while 1:
    LED.value(BUTTON.value())
    # Don't burn CPU
    time.sleep_ms(10)
