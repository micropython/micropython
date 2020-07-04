# Print a nice list of pins, their current settings, and available afs.
# Requires pins_af.py from ports/stm32/build-PYBV10/ directory.

import pyb
import pins_af


def af():
    max_name_width = 0
    max_af_width = 0
    for pin_entry in pins_af.PINS_AF:
        max_name_width = max(max_name_width, len(pin_entry[0]))
        for af_entry in pin_entry[1:]:
            max_af_width = max(max_af_width, len(af_entry[1]))
    for pin_entry in pins_af.PINS_AF:
        pin_name = pin_entry[0]
        print("%-*s " % (max_name_width, pin_name), end="")
        for af_entry in pin_entry[1:]:
            print("%2d: %-*s " % (af_entry[0], max_af_width, af_entry[1]), end="")
        print("")


def pins():
    mode_str = {
        pyb.Pin.IN: "IN",
        pyb.Pin.OUT_PP: "OUT_PP",
        pyb.Pin.OUT_OD: "OUT_OD",
        pyb.Pin.AF_PP: "AF_PP",
        pyb.Pin.AF_OD: "AF_OD",
        pyb.Pin.ANALOG: "ANALOG",
    }
    pull_str = {pyb.Pin.PULL_NONE: "", pyb.Pin.PULL_UP: "PULL_UP", pyb.Pin.PULL_DOWN: "PULL_DOWN"}
    width = [0, 0, 0, 0]
    rows = []
    for pin_entry in pins_af.PINS_AF:
        row = []
        pin_name = pin_entry[0]
        pin = pyb.Pin(pin_name)
        pin_mode = pin.mode()
        row.append(pin_name)
        row.append(mode_str[pin_mode])
        row.append(pull_str[pin.pull()])
        if pin_mode == pyb.Pin.AF_PP or pin_mode == pyb.Pin.AF_OD:
            pin_af = pin.af()
            for af_entry in pin_entry[1:]:
                if pin_af == af_entry[0]:
                    af_str = "%d: %s" % (pin_af, af_entry[1])
                    break
            else:
                af_str = "%d" % pin_af
        else:
            af_str = ""
        row.append(af_str)
        for col in range(len(width)):
            width[col] = max(width[col], len(row[col]))
        rows.append(row)
    for row in rows:
        for col in range(len(width)):
            print("%-*s " % (width[col], row[col]), end="")
        print("")
