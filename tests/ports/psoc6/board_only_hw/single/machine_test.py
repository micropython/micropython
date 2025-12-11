import machine


def soft_reset():
    machine.soft_reset()


def freq_get_validate():
    freq = machine.freq()
    if freq == 100000000:
        print("PASS")
    else:
        print(freq)
        print("FAIL")


print("*****Test 1: Default Frequency Validation*****")
freq_get_validate()
print("*****Test 2: Calling Soft Reset*****")
soft_reset()
