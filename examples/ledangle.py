def led_angle(seconds_to_run_for):
    # make LED objects
    l1 = pyb.Led(1)
    l2 = pyb.Led(2)

    for i in range(20 * seconds_to_run_for):
        # get x-axis
        accel = pyb.accel()[0]

        # turn on LEDs depending on angle
        if accel < -10:
            l1.on()
            l2.off()
        elif accel > 10:
            l1.off()
            l2.on()
        else:
            l1.off()
            l2.off()

        # delay so that loop runs at at 1/50ms = 20Hz
        pyb.delay(50)
