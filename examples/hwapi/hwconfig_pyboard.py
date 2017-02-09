from machine import Pin, Signal

# The blue LED on pin LED_BLUE also known as B4
LED = Signal(Pin('LED_RED', Pin.OUT))

# Red LED on pin LED_ERD also kown as A13
LED2 = Signal(Pin('LED_GREEN', Pin.OUT))

# Green LED on pin LED_GREEN also known as A14
LED3 = Signal(Pin('LED_YELLOW', Pin.OUT))

# Yellow LED on pin LED_YELLOW also known as A15
LED4 = Signal(Pin('LED_BLUE', Pin.OUT))
