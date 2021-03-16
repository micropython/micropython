import board
import digitalio
import gamepad
import time

pad = gamepad.GamePad(
    digitalio.DigitalInOut(board.P0_11),
    digitalio.DigitalInOut(board.P0_12),
    digitalio.DigitalInOut(board.P0_24),
    digitalio.DigitalInOut(board.P0_25),
)

prev_buttons = 0

while True:
    buttons = pad.get_pressed()

    if buttons != prev_buttons:
        for i in range(0, 4):
            bit = 1 << i
            if (buttons & bit) != (prev_buttons & bit):
                print("Button %d %s" % (i + 1, "pressed" if buttons & bit else "released"))

        prev_buttons = buttons

    time.sleep(0.1)
