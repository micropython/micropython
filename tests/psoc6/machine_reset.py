import machine


def reset_cause():
    reset_cause_code = machine.reset_cause()
    if reset_cause_code == machine.SOFT_RESET:
        print("Soft Reset")
    elif reset_cause_code == machine.HARD_RESET:
        print("Hard Reset")
    # elif reset_cause_code == machine.DEEPSLEEP_RESET:
    #     print("Deep Sleep Reset")
    else:
        print("Unknown Reset Cause")


print("Reset Validation after a soft Reset")
reset_cause()
