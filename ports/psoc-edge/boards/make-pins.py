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
        self._unhidden_ports = []
        self._port_max_index = 0

    # Collect all unhidden ports from the available
    # pins.
    # This function can be only used after parse_board_csv()
    # has run, as the available_pin generator is constructed
    # there.
    def add_ports(self):
        for pin in self.available_pins():
            if pin._port > self._port_max_index:
                self._port_max_index = pin._port
            if pin._port not in self._unhidden_ports and not pin._hidden:
                self._unhidden_ports.append(pin._port)

    # Overwrite the parse_board_csv to add
    # the unhidden ports after parsing the board CSV.
    def parse_board_csv(self, filename):
        super().parse_board_csv(filename)
        self.add_ports()

    def print_port_defines(self, out_header):
        print(file=out_header)
        print(
            f"#define MICROPY_PY_MACHINE_PIN_PORT_NUM_ENTRIES ({self._port_max_index + 1})",
            file=out_header,
        )

        print(file=out_header)
        print("// The MICROPY_PY_MACHINE_PIN_FOR_ALL_PORTS(DO) macro will", file=out_header)
        print("// apply the DO macro to all user available ports.", file=out_header)
        print("// The DO macro takes the port as argument: DO(port).", file=out_header)
        print("#define MICROPY_PY_MACHINE_PIN_FOR_ALL_PORTS(DO) \\", file=out_header)

        lines = [f"DO({port})" for port in self._unhidden_ports]
        macro_body = " \\\n".join(lines)
        print(macro_body, file=out_header)

        print(file=out_header)

    # Overwrite print_header to extend the print header base class
    # function with the pin port defines
    def print_header(self, out_header):
        super().print_header(out_header)
        self.print_port_defines(out_header)


if __name__ == "__main__":
    PSE84PinGenerator().main()
