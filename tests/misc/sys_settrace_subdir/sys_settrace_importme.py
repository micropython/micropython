print("Yep, I got imported.")

try:
    x = const(1)
except NameError:
    # Either running on CPython or MICROPY_COMP_CONST disabled.
    const = lambda x: x


# No const optimisation.
_CNT01 = "CONST01"

# Const assigned to an underscore name. Invisible to MicroPython with
# MICROPY_COMP_CONST enabled.
_CNT02 = const(123)

# Consts assigned to regular name, executed normally.
A123 = const(123)
a123 = const(123)


def dummy():
    return False


def saysomething():
    print("There, I said it.")


def neverexecuted():
    print("Never got here!")


print("Yep, got here")
