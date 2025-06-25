from Tiger import LCD1
import time

MENU_ITEMS = ["AUTO", "MANUAL", "ALARM", "STOP"]

lcd = LCD1(kbd_on=True)


def show_menu(index):
    lcd.clear()
    lcd.writeString("your choice:")
    lcd.cursorPosition(0, 1)
    lcd.writeString(MENU_ITEMS[index])


index = 0
running = True

while running:
    show_menu(index)
    while lcd.getKeyboardBufferSize() == 0:
        time.sleep_ms(10)
    key = lcd.readKeyboard(1)
    if key == b"\r":
        running = False
    elif key == b"\x0b":  # Up key
        index = (index - 1) % len(MENU_ITEMS)
    elif key == b"\x0a":  # Down key
        index = (index + 1) % len(MENU_ITEMS)

lcd.clear()
lcd.writeString("Menu index is:")
lcd.cursorPosition(0, 1)
lcd.writeString(str(index))