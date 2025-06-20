import Tiger
from Tiger import LCD1

lcd = LCD1(kbd_on=True)

lcd.clear()
lcd.cursorOff()

for i in range(5, 0, -1):
    lcd.clear()
    lcd.writeString(str(i))
    Tiger.delay(1000)  # Warte 1 Sekunde
