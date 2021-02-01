import pyb


def led_angle(seconds_to_run_for):
    # make LED objects
    l1 = pyb.LED(1)
    l2 = pyb.LED(2)
    accel = pyb.Accel()

    for i in range(20 * seconds_to_run_for):
        # get x-axis
        x = accel.x()

        # turn on LEDs depending on angle
        if x < -10:
            l1.on()
            l2.off()
        elif x > 10:
            l1.off()
            l2.on()
        else:
            l1.off()
            l2.off()

        # delay so that loop runs at at 1/50ms = 20Hz
        pyb.delay(50)
