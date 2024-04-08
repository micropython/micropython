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
    # Periodic timer
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
        tim_oneshot.deinit()  # Deinitialize the periodic timer


def test_periodic():
    # Periodic timer
    global periodic_triggered
    tim_periodic = Timer(0, period=1, mode=Timer.PERIODIC, callback=call_periodic)

    try:
        # Wait for 15 seconds
        for i in range(15):
            time.sleep(1)
            if periodic_triggered:
                print("Periodic timer triggered")
                periodic_triggered = False
    finally:
        tim_periodic.deinit()  # Deinitialize the periodic timer


if __name__ == "__main__":
    print("*****Oneshot Timer Execution*****")
    test_oneshot()
    print("*****Periodic Timer Execution*****")
    test_periodic()
