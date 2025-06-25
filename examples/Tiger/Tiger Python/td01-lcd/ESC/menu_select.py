from Tiger import LCD1
import time

# Menu items to display. A colon separates each entry and the
# string ends with a double colon.
MENU_ITEMS = ["AUTO", "MANUAL", "ALARM", "STOP"]
MENU_STR = ":" + ":".join(MENU_ITEMS) + "::"

lcd = LCD1(kbd_on=True)


def show_menu(idx):
    lcd.clear()
    lcd.writeString("select:")
    lcd.cursorPosition(0, 1)
    lcd.menuSelect(idx, MENU_STR)


index = 0
while True:
    show_menu(index)
    while lcd.getKeyboardBufferSize() == 0:
        time.sleep_ms(10)
    key = lcd.readKeyboard(1)
    if key == b"\r":
        break
    elif key == b"\x0b":
        index = (index - 1) % len(MENU_ITEMS)
    elif key == b"\x0a":
        index = (index + 1) % len(MENU_ITEMS)

lcd.clear()
lcd.writeString("menu index:")
lcd.cursorPosition(0, 1)
lcd.writeString(str(index))
