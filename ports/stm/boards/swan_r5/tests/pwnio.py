import time
import pwmio


def fade(pin):
    led = pwmio.PWMOut(pin, frequency=5000, duty_cycle=0)
    # LED setup for QT Py M0:
    # led = pwmio.PWMOut(board.SCK, frequency=5000, duty_cycle=0)

    while True:
        for i in range(100):
            # PWM LED up and down
            if i < 50:
                led.duty_cycle = int(i * 2 * 65535 / 100)  # Up
            else:
                led.duty_cycle = 65535 - int((i - 50) * 2 * 65535 / 100)  # Down
            time.sleep(0.01)
