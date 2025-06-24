import time
from Tiger import LCD1

# Simple demo for the LCD1 driver showing display, keyboard input and beep.

lcd = LCD1(kbd_on=True, beep_on=True)

lcd.clear()
lcd.writeString("LCD1 Test")
lcd.cursorPosition(0, 1)
lcd.writeString("Press keys:")

while True:
    if lcd.getKeyboardBufferSize() > 0:
        key = lcd.readKeyboard(1)
        if key is not None:
            lcd.writeString(b"\x07")
            lcd.cursorPosition(0, 2)
            lcd.writeString(key)
    time.sleep_ms(50)
