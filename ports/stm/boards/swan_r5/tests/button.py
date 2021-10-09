import board
import digitalio
import time


def monitor_button(pin, callback):
    with digitalio.DigitalInOut(pin) as button:
        newstate = not button.value  # state is inverted
        state = not newstate  # ensure change reported to start with
        while callback(newstate, newstate != state):
            state = newstate
            newstate = button.value
            time.sleep(0.01)


def print_changes(state, changed):
    if changed:
        print(f"button pressed {state}")
    return True


monitor_button(board.BUTTON_USR, print_changes)
