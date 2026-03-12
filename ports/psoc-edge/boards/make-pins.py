import os
import sys
from collections import defaultdict, namedtuple

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../../../tools"))
import boardgen

SUPPORTED_AF = {
    "I2C": ["SDA", "SCL"],
    "UART": ["TX", "RX", "CTS", "RTS"],
    "SPI": ["CLK", "MOSI", "MISO", "SELECT0", "SELECT1"],
    "PDM": ["CLK", "DATA"],
    # TODO: Other active functionalities that we need to figure out:
    # - TDM
    # - TCPWM
    # - SMIF
    # - CANFD
    # - PERIx_TR_IO
    # - SDHC
    # - ETH
    # - GFXSS
    # - DEBUG
    # - I3C
    # - M33SYSCPUSS
    # - SRSS_EXT_CLK
}

# Contains the result of parsing an cell from af.csv.
PinAf = namedtuple(
    "PinAf",
    [
        "af_idx",  # int, 0-15
        "af_fn",  # e.g. "I2C"
        "af_unit",  # int, e.g. 1 (for I2C1) or None (for OTG_HS_ULPI_CK)
        "af_signal",  # e.g. "SDA"
        "af_supported",  # bool, see table above
        "af_name",  # e.g. "ACT_x"
        "af_ptr",  # SCBx or <peripheral_instance>
    ],
)


class PSE84Pin(boardgen.Pin):
    def __init__(self, cpu_pin_name):
        super().__init__(cpu_pin_name)

        # The pins are name like PX_Y, where X is the port number and Y is the pin number.
        # Each of the numbers can be multiple digits.
        # The port are the characters after 'P' and before '_'
        self._port = int(cpu_pin_name[1 : cpu_pin_name.index("_")])
        # The pin number is the characters after '_'
        self._pin = int(cpu_pin_name[cpu_pin_name.index("_") + 1 :])

        # List of PinAF instances
        self._afs = []

    def definition(self):
        return f"PIN({self._port}, {self._pin}, pin_{self.name()}_af)"

    def add_af_scb(self, af_idx, af_name, af):
        # The SCB alternate function follow the
        # convention:
        # SCB<unit>_<function>_<signal_type>

        # The peripheral pointer is the first prefix including
        # number, e.g. SCB0
        af_ptr = af.split("_")[0]
        # The unit is the number after "SCB" and before "_".
        # For example for SCB0 -> 0.
        af_unit = af.split("_")[0][3:]  # After 'SCB'
        # The second token is the function.
        # For example I2C, SPI, UART
        af_fn = af.split("_")[1]
        # The last token is the signal signal type for the
        # given function, e.g. SDA, SCL, MOSI
        af_signal = af.split("_")[2]

        af_supported = af_fn in SUPPORTED_AF and af_signal in SUPPORTED_AF[af_fn]

        pin_af = PinAf(af_idx, af_fn, af_unit, af_signal, af_supported, af_name, af_ptr)
        self._afs.append(pin_af)

    def add_af_pdm(self, af_idx, af_name, af):
        # The PDM alternate function follow
        # the convention:
        # PDM_PDM_<signal><channel>

        af_ptr = (
            af.split("_")[0] + "0"
        )  # The peripheral is PDM0 (this index is missing in .csv definition)
        af_fn = af.split("_")[1]

        # Split the signal+channel part into alphabetic (signal) and numeric (unit) parts
        signal_channel = af.split("_")[2]

        # Find where the numeric part starts
        import re

        match = re.match(r"^([A-Za-z]+)(\d+)$", signal_channel)

        if match:
            af_signal = match.group(1)  # Alphabetic part (e.g., "CLK", "DATA")
            af_unit = match.group(2)  # Numeric part (e.g., "3", "15", "123")
        else:
            # Fallback for unexpected format
            af_signal = signal_channel
            af_unit = None

        af_supported = af_fn in SUPPORTED_AF and af_signal in SUPPORTED_AF[af_fn]

        pin_af = PinAf(af_idx, af_fn, af_unit, af_signal, af_supported, af_name, af_ptr)
        self._afs.append(pin_af)

    def add_af(self, af_idx, af_name, af):
        if af_idx > 19:
            return

        if af_idx <= 15:
            if af_name != "ACT_{:d}".format(af_idx):
                raise boardgen.PinGeneratorError(
                    "Invalid AF column name '{:s}' for AF index {:d}.".format(af_name, af_idx)
                )

        if af_idx > 15:
            format_idx = af_idx - 15 + 1  # DS2 to DS5  are valid
            if af_name != "DS_{:d}".format(format_idx):
                raise boardgen.PinGeneratorError(
                    "Invalid AF column name '{:s}' for AF index {:d}.".format(af_name, af_idx)
                )
        # TODO: Review implementation.
        # The names have the following patterns:
        #
        # <peripheral/block#>_<function/protocol>_<signal/line/channel#>
        #
        # As this is not very consistent we will handle each case
        # separately and as we improve our understanding we will update
        # this code accordingly.
        # Currently we are using the definition in the GPIO_PSE84_BGA_220
        # file which has manually added to the boards/pse84x_af.csv.
        # But ideally this could be also automatically generated if more
        # PSE devices need to be supported in the future.

        # If the prefix match the format SBCx
        if af.startswith("SCB"):
            self.add_af_scb(af_idx, af_name, af)
        elif af.startswith("PDM"):
            self.add_af_pdm(af_idx, af_name, af)
        else:
            # TODO: Extend the parsing to other peripherals.
            pass

    # This will be called at the start of the output (after the prefix). Use
    # it to emit the af objects (via the AF() macro).
    def print_source(self, out_source):
        print(file=out_source)
        print("const machine_pin_af_obj_t pin_{:s}_af[] = {{".format(self.name()), file=out_source)
        for af in self._afs:
            if af.af_supported:
                print("    ", end="", file=out_source)
            else:
                print("    // ", end="", file=out_source)
            # AF(af_name_idx, af_fn, af_unit, af_signal, af_ptr)
            print(
                "AF({:s}, {:s}, {:s}, {:s}, {:s}),  // {:s}_{:s}{:s}_{:s}".format(
                    af.af_name,
                    af.af_fn,
                    af.af_unit or 0,
                    af.af_signal or "NONE",
                    af.af_ptr,
                    af.af_ptr,
                    af.af_fn,
                    af.af_unit,
                    af.af_signal,
                ),
                file=out_source,
            )
        print("};", file=out_source)


class PSE84PinGenerator(boardgen.PinGenerator):
    def __init__(self):
        super().__init__(pin_type=PSE84Pin, enable_af=True)
        self._unhidden_ports = []
        self._port_max_index = 0

        self._unhidden_scb = []
        self._scb_max_index = 0

        self._unhidden_i2c = []
        self._unhidden_spi = []
        self._unhidden_uart = []

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

    # Collect all unhidden SCB units from the available pins.
    # And the each of the potential available I2C, SPI and UART units
    # It will not verify that all signals are available for a given unit.
    # Therefore, the actual amount of available units for each protocol
    # may be lower than the one collected here.
    def add_scbs(self):
        for pin in self.available_pins():
            if not pin._hidden:
                for af in pin._afs:
                    if af.af_ptr.startswith("SCB"):
                        if af.af_unit not in self._unhidden_scb:
                            self._unhidden_scb.append(af.af_unit)
                            if int(af.af_unit) > self._scb_max_index:
                                self._scb_max_index = int(af.af_unit)

                        if af.af_fn == "I2C":
                            if af.af_unit not in self._unhidden_i2c:
                                self._unhidden_i2c.append(af.af_unit)
                        elif af.af_fn == "SPI":
                            if af.af_unit not in self._unhidden_spi:
                                self._unhidden_spi.append(af.af_unit)
                        elif af.af_fn == "UART":
                            if af.af_unit not in self._unhidden_uart:
                                self._unhidden_uart.append(af.af_unit)

        self._unhidden_scb.sort(key=int)
        self._unhidden_i2c.sort(key=int)
        self._unhidden_spi.sort(key=int)
        self._unhidden_uart.sort(key=int)

    # Override the parse_board_csv to add
    # the unhidden ports after parsing the board CSV.
    def parse_board_csv(self, filename):
        super().parse_board_csv(filename)
        self.add_ports()
        self.add_scbs()

    # Override the default implementation just to change the default arguments
    # (extra header row, skip first column).
    def parse_af_csv(self, filename):
        return super().parse_af_csv(filename, header_rows=1, pin_col=1, af_col=2)

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

    def print_scb_defines(self, out_header):
        print(file=out_header)
        print(
            f"#define MICROPY_PY_MACHINE_SCB_NUM_ENTRIES ({self._scb_max_index + 1})",
            file=out_header,
        )

        print(
            f"#define MICROPY_PY_MACHINE_I2C_NUM_ENTRIES ({len(self._unhidden_i2c)})",
            file=out_header,
        )

        print(
            f"#define MICROPY_PY_MACHINE_SPI_NUM_ENTRIES ({len(self._unhidden_spi)})",
            file=out_header,
        )

        print(
            f"#define MICROPY_PY_MACHINE_UART_NUM_ENTRIES ({len(self._unhidden_uart)})",
            file=out_header,
        )

        print(file=out_header)
        print("// The MICROPY_PY_MACHINE_FOR_ALL_SCB(DO) macro will", file=out_header)
        print("// apply the DO macro to all user available SCB units.", file=out_header)
        print("// The DO macro takes the SCB unit as argument: DO(unit).", file=out_header)
        print("#define MICROPY_PY_MACHINE_FOR_ALL_SCB(DO) \\", file=out_header)

        lines = [f"DO({scb})" for scb in self._unhidden_scb]
        macro_body = " \\\n".join(lines)
        print(macro_body, file=out_header)

        print(file=out_header)

    # Overwrite print_header to extend the print header base class
    # function with the pin port defines
    def print_header(self, out_header):
        super().print_header(out_header)
        self.print_port_defines(out_header)

    def print_af_header(self, out_af_header):
        self.print_scb_defines(out_af_header)

    # Add additional header file for AF defines and constants
    def extra_args(self, parser):
        parser.add_argument("--output-af-header")

    # Called in main() after everything else is done to write additional files.
    def generate_extra_files(self):
        if self.args.output_af_header:
            with open(self.args.output_af_header, "w") as out_af_header:
                self.print_af_header(out_af_header)


if __name__ == "__main__":
    PSE84PinGenerator().main()
