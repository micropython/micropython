import Tiger
from Tiger import LCD1

lcd = LCD1()

def starxy(x,y):
    lcd.cursorPosition(x,y)
    lcd.writeString("*")
    Tiger.delay(200)
    lcd.cursorPosition(x,y)
    lcd.writeString(" ")

cset =bytes([
        0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,
        0x01,0x02,0x02,0x04,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x04,0x02,0x02,0x01,0x00,
        0x04,0x04,0x04,0x04,0x00,0x00,0x04,0x00,
        0x00,0x00,0x00,0x04,0x08,0x08,0x10,0x00,
        0x00,0x00,0x00,0x1C,0x00,0x00,0x00,0x00,
        0x10,0x08,0x08,0x04,0x00,0x00,0x00,0x00
])



print("Start")


# ------------------------- SELECT_FONT.TIG ------------------------------------------------

Tiger.delay(1000)
lcd.cursorOff()
lcd.clear()

lcd.writeString("Select Font:") #clear
lcd.cursorPosition(0,1)
Tiger.delay(2000)

for x in range(8):
    data = (0x80 + x).to_bytes(1,'big')
    lcd.writeString(data + b'-')

for x in range(16):
   lcd.activateSpecialCharset(x)
   Tiger.delay(2000)

lcd.clear()

# ------------------------- LOAD_FONT.TIG ------------------------------------------------

lcd.cursorOff()
lcd.clear()
lcd.writeString("Load Font:")
Tiger.delay(2000)
lcd.cursorPosition(0,1)
lcd.cursorOff()

for x in range(8):
    data = (0x80 + x).to_bytes(1,'big')
    lcd.writeString(data + b' ')

lcd.activateSpecialCharset(7)

Tiger.delay(2000)
lcd.loadSpecialCharset(7,cset)
Tiger.delay(2000)
lcd.resetSpecialCharset(7)


# ------------------------- CURSOR.TIG ------------------------------------------------

Tiger.delay(2000)
lcd.cursorOff()
lcd.clear()
lcd.writeString("Cursor TIG:")
Tiger.delay(2000)

for x in range(3):
    if x == 0:
        lcd.cursorOff()
        lcd.clear()
        lcd.writeString("Cursor off")
    elif x == 1:
        lcd.cursorOn()
        lcd.clear()
        lcd.writeString("Cursor on")
    elif x == 2:
        lcd.cursorBlink()
        lcd.clear()
        lcd.writeString("Cursor blinks")
    Tiger.delay(3000)


# ------------------------- GOTOXY.TIG ------------------------------------------------

lcd.cursorOff()
lcd.clear()

lcd.writeString("Go to XY:")
Tiger.delay(2000)

lcd.cursorOff()
lcd.clear()

for a in range(2):
    for x in range(20):
        starxy(x,1)
    for x in range(18, 0, -1):
        starxy(x, 1)

Tiger.delay(2000)

#-----------------------------------------------------------------------------------------