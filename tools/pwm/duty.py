def duty(filename):
    rising_edge = None
    falling_edge = None
    with open(filename, "r") as f:
        line = f.readline()
        t, val = line.split(",")
        t = float(t)
        val = int(val)
        if val == 0:
            if falling_edge and rising_edge is not None:
                period = t - falling_edge
                duty_cycle = (t - rising_edge) / period
                print(t, val, period, duty_cycle)
            falling_edge = t
        else:
            rising_edge = t
