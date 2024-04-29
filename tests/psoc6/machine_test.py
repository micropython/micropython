import machine


def soft_reset():
    machine.soft_reset()


def freq_get_validate():
    freq = machine.freq()
    if freq == 100000000:
        print("Default Frequency")


print("Standard Frequency Validation")
freq_get_validate()
print("Calling Soft Reset")
soft_reset()
