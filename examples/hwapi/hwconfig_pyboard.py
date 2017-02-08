from machine import Pin, Signal

# Red LED on pin LED_RED also kown as A13
LED = Signal(Pin('LED_RED', Pin.OUT))

# Green LED on pin LED_GREEN also known as A14
LED2 = Signal(Pin('LED_GREEN', Pin.OUT))

# Yellow LED on pin LED_YELLOW also known as A15
LED3 = Signal(Pin('LED_YELLOW', Pin.OUT))

# Blue LED on pin LED_BLUE also known as B4
LED4 = Signal(Pin('LED_BLUE', Pin.OUT))
