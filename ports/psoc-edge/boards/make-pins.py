from operator import add
import os
import re
import sys
from collections import defaultdict, namedtuple

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../../../tools"))
import boardgen

SUPPORTED_AF = {
    "I2C": ["SDA", "SCL"],
    "UART": ["TX", "RX", "CTS", "RTS"],
    "SPI": ["CLK", "MOSI", "MISO", "SELECT0", "SELECT1"],
    "PDM": ["CLK", "DATA"],
    "TCPWM": ["LINE"],
    # TODO: Other active functionalities that we need to figure out:
    # - TDM
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

    def add_af_tcpwm(self, af_idx, af_name, af):
        # The TCPWM alternate functions can appear directly as TCPWM0_LINE<n>
        # or as a suffix after another function, e.g. SMIFx_..._TCPWM0_LINE<n>.
        match = re.search(r"(TCPWM\d+)_(LINE(?:_COMPL)?)(\d+)$", af)
        if not match:
            return

        af_ptr = match.group(1)
        af_fn = "TCPWM"
        af_signal = match.group(2)
        af_unit = match.group(3)

        af_supported = af_fn in SUPPORTED_AF and af_signal in SUPPORTED_AF[af_fn]

        pin_af = PinAf(af_idx, af_fn, af_unit, af_signal, af_supported, af_name, af_ptr)
        self._afs.append(pin_af)

    def add_af(self, af_idx, af_name, af):
        # The AF index matches the column index for the ACTx functions 0-15
        # while for DSx functions the columns 16-19 are mapped to DS2-DS5 respectively.
        af_act_max_idx = 15
        af_ds_num = 4

        if af_idx > af_act_max_idx + af_ds_num:
            return

        if af_idx <= af_act_max_idx:
            if af_name != "ACT_{:d}".format(af_idx):
                raise boardgen.PinGeneratorError(
                    "Invalid AF column name '{:s}' for AF index {:d}.".format(af_name, af_idx)
                )

        if af_idx > af_act_max_idx:
            format_idx = (
                af_idx - af_act_max_idx + 1
            )  # The starting index is 2, so we need to subtract
            # the max ACTx index and add 1 to get the correct DS index.
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
        elif "TCPWM" in af:
            self.add_af_tcpwm(af_idx, af_name, af)
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

        # TCPWM LINE tracking
        self._pwm_pin_count = 0  # unique exposed pins with a TCPWM LINE AF
        self._tcpwm_counter_max = 0  # highest counter number seen across all LINE AFs
        self._tcpwm_counters = []  # sorted unique TCPWM counter IDs seen in LINE AFs

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

    # Collect TCPWM LINE statistics across all available pins:
    # - count of unique TCPWM counter IDs visible through at least one LINE AF
    #   (this equals the maximum number of concurrent PWM instances)
    # - highest counter number seen (used to compute compact array size)
    def add_tcpwm(self):
        seen_counters = set()
        for pin in self.available_pins():
            for af in pin._afs:
                if af.af_fn == "TCPWM" and af.af_signal == "LINE" and af.af_supported:
                    counter_num = int(af.af_unit)
                    seen_counters.add(counter_num)
                    if counter_num > self._tcpwm_counter_max:
                        self._tcpwm_counter_max = counter_num
        self._tcpwm_counters = sorted(seen_counters)
        self._pwm_pin_count = len(seen_counters)

    # Override the parse_board_csv to add
    # the unhidden ports after parsing the board CSV.
    def parse_board_csv(self, filename):
        super().parse_board_csv(filename)
        self.add_ports()
        self.add_scbs()
        self.add_tcpwm()

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

    def print_tcpwm_defines(self, out_header):
        # TCPWM counter ownership array must be large enough to index the highest
        # counter number seen. Counters 0-7 map to indices 0-7, counters 256+ map
        # to indices 8+, so the compact array size is (max_counter - 256 + 8 + 1)
        # for high counters, or simply (max_counter + 1) for low ones.
        max_c = self._tcpwm_counter_max
        if max_c >= 256:
            num_counters = (max_c - 256 + 8) + 1
        else:
            num_counters = max_c + 1

        print(file=out_header)
        print(
            "// Number of entries in the compact TCPWM0 counter ownership array.",
            file=out_header,
        )
        print(
            f"#define MICROPY_PY_MACHINE_TCPWM0_NUM_COUNTERS ({num_counters}U)",
            file=out_header,
        )
        print(
            "// Number of exposed GPIO pins that support at least one TCPWM LINE AF.",
            file=out_header,
        )
        print(
            f"#define MICROPY_PY_MACHINE_PWM_MAX_OBJS ({self._pwm_pin_count})",
            file=out_header,
        )
        print(file=out_header)

    def print_timer_hw_map(self, out_header):
        # PSE8x fixed timer-to-counter-to-IRQ mapping.
        # This is hardware-defined per MCU and does not depend on board configuration.
        print(file=out_header)
        print("// Per-ID hardware mapping for timers to TCPWM counters and IRQs.", file=out_header)
        print("// Each row: X(timer_id, tcpwm_counter_num, irq)", file=out_header)
        print("#define MICROPY_PY_MACHINE_TIMER_HW_MAP(X) \\", file=out_header)

        timer_entries = [
            # Group 0: Timers 0-3 use counters 2, 3, 5, 6
            "    X(0,   2U,   tcpwm_0_interrupts_2_IRQn) \\",
            "    X(1,   3U,   tcpwm_0_interrupts_3_IRQn) \\",
            "    X(2,   5U,   tcpwm_0_interrupts_5_IRQn) \\",
            "    X(3,   6U,   tcpwm_0_interrupts_6_IRQn) \\",
            # Group 1: Timers 4-27 use counters 256-279
            "    X(4,   256U, tcpwm_0_interrupts_256_IRQn) \\",
            "    X(5,   257U, tcpwm_0_interrupts_257_IRQn) \\",
            "    X(6,   258U, tcpwm_0_interrupts_258_IRQn) \\",
            "    X(7,   259U, tcpwm_0_interrupts_259_IRQn) \\",
            "    X(8,   260U, tcpwm_0_interrupts_260_IRQn) \\",
            "    X(9,   261U, tcpwm_0_interrupts_261_IRQn) \\",
            "    X(10,  262U, tcpwm_0_interrupts_262_IRQn) \\",
            "    X(11,  263U, tcpwm_0_interrupts_263_IRQn) \\",
            "    X(12,  264U, tcpwm_0_interrupts_264_IRQn) \\",
            "    X(13,  265U, tcpwm_0_interrupts_265_IRQn) \\",
            "    X(14,  266U, tcpwm_0_interrupts_266_IRQn) \\",
            "    X(15,  267U, tcpwm_0_interrupts_267_IRQn) \\",
            "    X(16,  268U, tcpwm_0_interrupts_268_IRQn) \\",
            "    X(17,  269U, tcpwm_0_interrupts_269_IRQn) \\",
            "    X(18,  270U, tcpwm_0_interrupts_270_IRQn) \\",
            "    X(19,  271U, tcpwm_0_interrupts_271_IRQn) \\",
            "    X(20,  272U, tcpwm_0_interrupts_272_IRQn) \\",
            "    X(21,  273U, tcpwm_0_interrupts_273_IRQn) \\",
            "    X(22,  274U, tcpwm_0_interrupts_274_IRQn) \\",
            "    X(23,  275U, tcpwm_0_interrupts_275_IRQn) \\",
            "    X(24,  276U, tcpwm_0_interrupts_276_IRQn) \\",
            "    X(25,  277U, tcpwm_0_interrupts_277_IRQn) \\",
            "    X(26,  278U, tcpwm_0_interrupts_278_IRQn) \\",
            "    X(27,  279U, tcpwm_0_interrupts_279_IRQn)",  # Last entry: no backslash
        ]

        for entry in timer_entries:
            print(entry, file=out_header)
        print(file=out_header)

    def print_tcpwm_counter_map(self, out_header):
        print(file=out_header)
        print("// All exposed TCPWM LINE counters and their PCLK destinations.", file=out_header)
        print("// Each row: X(tcpwm_counter_num, pclk_dst)", file=out_header)
        print("#define MICROPY_PY_MACHINE_TCPWM0_COUNTER_MAP(X) \\", file=out_header)

        lines = []
        for counter in self._tcpwm_counters:
            pclk = f"PCLK_TCPWM0_CLOCK_COUNTER_EN{counter}"
            lines.append(f"    X({counter}U, {pclk})")

        if lines:
            for i, line in enumerate(lines):
                suffix = " \\" if i < len(lines) - 1 else ""
                print(f"{line}{suffix}", file=out_header)
        else:
            print("    /* no TCPWM LINE counters exposed */", file=out_header)

        print(file=out_header)

    def print_af_header(self, out_af_header):
        self.print_scb_defines(out_af_header)
        self.print_tcpwm_defines(out_af_header)
        self.print_timer_hw_map(out_af_header)
        self.print_tcpwm_counter_map(out_af_header)

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
