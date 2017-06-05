from machine import Pin

#
# Olimex E407 board
#

# Green LED on port C, pin 13
LED = Pin("C13", Pin.OUT)

# Push button switch (WKUP) on port A, pin 0
BUTTON = Pin("A0", Pin.IN)

