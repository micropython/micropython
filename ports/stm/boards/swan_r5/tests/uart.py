import board
import busio
import digitalio
import usb_cdc
import time

while not usb_cdc.console.in_waiting:
    time.sleep(0.1)

print("USART test")

# For most CircuitPython boards:
led = digitalio.DigitalInOut(board.LED)
# For QT Py M0:
# led = digitalio.DigitalInOut(board.SCK)
led.direction = digitalio.Direction.OUTPUT

uart = busio.UART(board.TX, board.RX, baudrate=9600)

while True:
    data = uart.read(32)  # read up to 32 bytes
    # print(data)  # this is a bytearray type

    if data is not None:
        led.value = True

        # convert bytearray to string
        data_string = "*".join([chr(b) for b in data])
        print(data_string, end="")

        led.value = False

    if usb_cdc.console.in_waiting:
        data = usb_cdc.console.read()
        data_string = "*".join([chr(b) for b in data])
        print("writing " + data_string)
        uart.write(data)
