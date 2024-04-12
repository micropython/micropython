import machine


def hard_reset():
    machine.reset()


def soft_reset():
    machine.soft_reset()


def hard_reset_validate():
    pass


def soft_reset_validate():
    pass


def freq_get_validate():
    freq = machine.freq()
    if freq == 10000000:
        print("Default Frequency")


def reset_cause():
    machine.reset()
    reset = machine.reset_cause()
    # reset check comments..call each reset and then check the reset cause
