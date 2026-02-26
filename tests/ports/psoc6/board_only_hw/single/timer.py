import os
from machine import Timer
import time

oneshot_triggered = False
periodic_triggered = False


def call_oneshot(timer):
    global oneshot_triggered
    oneshot_triggered = True


def call_periodic(timer):
    global periodic_triggered
    periodic_triggered = True


def test_oneshot():
    # Oneshot timer
    global oneshot_triggered
    tim_oneshot = Timer(0, period=1000, mode=Timer.ONE_SHOT, callback=call_oneshot)

    try:
        # Wait for 5 seconds
        for i in range(5):
            time.sleep(1)
            if oneshot_triggered:
                print("Oneshot timer triggered")
                oneshot_triggered = False
    finally:
        tim_oneshot.deinit()  # Deinitialize the Oneshot timer


def test_periodic():
    # Periodic timer
    global periodic_triggered
    tim_periodic = Timer(0, period=1000, mode=Timer.PERIODIC, callback=call_periodic)

    try:
        # Wait for 15 seconds
        for i in range(15):
            time.sleep(1)
            if periodic_triggered:
                print("Periodic timer triggered")
                periodic_triggered = False
    finally:
        tim_periodic.deinit()  # Deinitialize the periodic timer


def test_multiple_timers():
    global oneshot_triggered
    global periodic_triggered
    # Multiple timers
    tim_oneshot = Timer(0, period=1000, mode=Timer.ONE_SHOT, callback=call_oneshot)
    tim_periodic = Timer(1, period=3500, mode=Timer.PERIODIC, callback=call_periodic)

    try:
        # Wait for 15 seconds
        for i in range(15):
            time.sleep(1)
            if oneshot_triggered:
                print("Oneshot timer triggered")
                oneshot_triggered = False
            if periodic_triggered:
                print("Periodic timer triggered")
                periodic_triggered = False
    finally:
        tim_oneshot.deinit()  # Deinitialize the Oneshot timer
        tim_periodic.deinit()  # Deinitialize the periodic timer


if __name__ == "__main__":
    print("*****Oneshot Timer Execution*****")
    test_oneshot()
    print("*****Periodic Timer Execution*****")
    test_periodic()
    print("*****Multiple Timers Execution*****")
    test_multiple_timers()

    # TODO: Timer tests need to be refined.
    # The current implementation is not reliable.
    # After this tests CY8CPROTO-063-BLE board is not able to the subsequent test.
    # This workaround works for
    board = os.uname().machine
    if "CY8CPROTO-063-BLE" in board:
        time.sleep(3)
