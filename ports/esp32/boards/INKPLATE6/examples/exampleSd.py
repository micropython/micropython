import os, time
from inkplate6 import Inkplate

display = Inkplate(Inkplate.INKPLATE_2BIT)
display.begin()

# This prints all the files on card
print(os.listdir("/sd"))

f = open("sd/text.txt", "r")

# Print file contents
print(f.read())
f.close()

time.sleep(5)

# Utterly slow, can take minutes :(
display.drawImageFile(0, 0, "sd/1.bmp")

display.display()
