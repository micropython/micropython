import os
import sys


sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../../../tools"))
import boardgen


class PSE84Pin(boardgen.Pin):
    def __init__(self, cpu_pin_name):
        super().__init__(cpu_pin_name)

        # The pins are name like PX_Y, where X is the port number and Y is the pin number.
        # Each of the numbers can be multiple digits.
        # The port are the characters after 'P' and before '_'
        self._port = int(cpu_pin_name[1 : cpu_pin_name.index("_")])
        # The pin number is the characters after '_'
        self._pin = int(cpu_pin_name[cpu_pin_name.index("_") + 1 :])

    def definition(self):
        return f"PIN({self._port}, {self._pin})"


class PSE84PinGenerator(boardgen.PinGenerator):
    def __init__(self):
        super().__init__(pin_type=PSE84Pin, enable_af=False)


if __name__ == "__main__":
    PSE84PinGenerator().main()
