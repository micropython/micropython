#!/usr/bin/env python


from collections import defaultdict, namedtuple
import os
import re
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../../../tools"))
import boardgen


# Must have matching entries in AF_FN_* enum in ../pin_defs_stm32.h
SUPPORTED_AF = {
    "TIM": ["CH1", "CH2", "CH3", "CH4", "CH1N", "CH2N", "CH3N", "CH1_ETR", "ETR", "BKIN"],
    "I2C": ["SDA", "SCL"],
    "I2S": ["CK", "MCK", "SD", "WS", "EXTSD"],
    "USART": ["RX", "TX", "CTS", "RTS", "CK"],
    "UART": ["RX", "TX", "CTS", "RTS"],
    "LPUART": ["RX", "TX", "CTS", "RTS"],
    "SPI": ["NSS", "SCK", "MISO", "MOSI"],
    "SDMMC": ["CK", "CMD", "D0", "D1", "D2", "D3"],
    "CAN": ["TX", "RX"],
}

# Only make these AFs available if we actually enable those peripherals (by
# assigning default pins in mpconfigboard.h).
CONDITIONAL_VAR = {
    "I2C": "MICROPY_HW_I2C{num}_SCL",
    "I2S": "MICROPY_HW_I2S{num}",
    "SPI": "MICROPY_HW_SPI{num}_SCK",
    "UART": "MICROPY_HW_UART{num}_TX",
    "LPUART": "MICROPY_HW_LPUART{num}_TX",
    "USART": "MICROPY_HW_UART{num}_TX",
    "SDMMC": "MICROPY_HW_SDMMC{num}_CK",
    "CAN": "MICROPY_HW_CAN{num}_TX",
}

# Contains the result of parsing an cell from af.csv.
PinAf = namedtuple(
    "PinAf",
    [
        "af_idx",  # int, 0-15
        "af_fn",  # e.g. "I2C"
        "af_unit",  # int, e.g. 1 (for I2C1) or None (for OTG_HS_ULPI_CK)
        "af_pin",  # e.g. "SDA"
        "af_supported",  # bool, see table above
        "af_name",  # e.g. "I2C1_SDA"
    ],
)

# Only support ADC1, ADC2, ADC3 for now (e.g. cannot support ADC4 & ADC5 on
# STM32G4).
MIN_ADC_UNIT = 1
MAX_ADC_UNIT = 3


class Stm32Pin(boardgen.Pin):
    def __init__(self, cpu_pin_name):
        super().__init__(cpu_pin_name)

        # Pins ending in "_C" correspond to the analog-only pad of a pair
        # of dual (analog) pads found on H7 MCUs (eg PA0 and PA0_C pair).
        self._analog_only = cpu_pin_name.endswith("_C")
        if self._analog_only:
            cpu_pin_name = cpu_pin_name[:-2]

        # P<port><num> (already verified by validate_cpu_pin_name).
        self._port = cpu_pin_name[1]
        self._pin = int(cpu_pin_name[2:])

        # List of PinAf instances.
        self._afs = []

        # The channel that this pin uses on each of the listed units.
        self._adc_channel = 0
        # e.g. ADC123 -> [1,2,3]
        self._adc_units = []

    # Called for each AF defined in the csv file for this pin. e.g. "SPI2_NSS/I2S2_WS"
    def add_af(self, af_idx, af_name, af):
        if af_idx > 16:
            # The AF csv files should not have more than 16 AFs + 1 ADC list.
            return

        # AF 16 is the ADC, which is special cased below.
        if af_idx == 16:
            if af_name != "ADC":
                raise boardgen.PinGeneratorError(
                    "Invalid AF column name '{:s}' for ADC column with index {:d}.".format(
                        af_name, af_idx
                    )
                )
            return self.add_adc(af)

        if af_name != "AF{:d}".format(af_idx):
            raise boardgen.PinGeneratorError(
                "Invalid AF column name '{:s}' for AF index {:d}.".format(af_name, af_idx)
            )

        # If there is a slash, then the slash separates multiple aliases for
        # the same alternate function.
        for af_name in af.split("/"):
            if not af_name.strip():
                continue
            # This matches <fn><unit>_<pin>, with consideration for:
            #  - fn may contain a digit (e.g. I2C)
            #  - there may be an "ext" after the unit
            m = re.match("([A-Z0-9]+[A-Z])(([0-9]+)(ext)?)?(_(.*))?", af_name)
            if not m:
                raise boardgen.PinGeneratorError(
                    "Invalid af '{:s}' for pin '{:s}'".format(af_name, self.name())
                )
            else:
                af_fn = m.group(1)
                af_unit = int(m.group(3)) if m.group(3) is not None else None
                af_ext = m.group(4) == "ext"
                af_pin = m.group(6)

                # Special case. We change I2S2ext_SD into I2S2_EXTSD so that it parses
                # the same way the other peripherals do.
                if af_ext:
                    af_pin = "EXT" + af_pin

                # Special case: FDCAN peripheral is named CAN in MicroPython, same as bxCAN
                af_fn = af_fn.replace("FDCAN", "CAN")

                af_supported = af_fn in SUPPORTED_AF and af_pin in SUPPORTED_AF[af_fn]

                self._afs.append(PinAf(af_idx, af_fn, af_unit, af_pin, af_supported, af_name))

    # ADCs are slash separated "ADC<list of units>_<mode><channel>", where unit=1,2,3,4,5 and mode=IN,INN,INP
    def add_adc(self, adc):
        if not adc.strip():
            return

        # TODO: This needs to be improved to support the case where a pin can
        # be the P for one channel, and the N for a different channel.
        # e.g. "ADC123_INP12/ADC123_INN11".
        for adc_name in adc.split("/"):
            m = re.match("ADC([1-5]+)_(IN[NP]?)([0-9]+)$", adc_name)
            if not m:
                raise boardgen.PinGeneratorError(
                    "Invalid adc '{:s}' for pin '{:s}'".format(adc_name, self.name())
                )
            adc_units = [int(x) for x in m.group(1) if MIN_ADC_UNIT <= int(x) <= MAX_ADC_UNIT]
            adc_mode = m.group(2)
            if adc_mode == "INN":
                # On H7 we have INN/INP, all other parts use IN only. Only use
                # IN or INP channels.
                continue
            adc_channel = int(m.group(3))

            # Pick the entry with the most ADC units, e.g. "ADC1_INP16/ADC12_INN1/ADC12_INP0" --> "ADC12_INN1".
            if len(adc_units) > len(self._adc_units):
                self._adc_units = adc_units
                self._adc_channel = adc_channel

    # STM32-specific behavior, strip the "P" from the start of the name when emitting this pin.
    # e.g. the #define is pin_A11 not pin_PA11. Fortunately we don't have to special case this
    # as there are no places where emit the full "PA11" name.
    def name(self):
        return self._cpu_pin_name[1:]

    # Use the PIN() macro defined in stm32f4xx_prefix.c for defining the pin
    # objects.
    def definition(self):
        # Generate bitfield of supported ADC units where lsb is unit 1 (e.g. [1,3] --> 0b101).
        adc_units_bitfield = (
            " | ".join("PIN_ADC{}".format(unit) for unit in self._adc_units) or "0"
        )

        # PIN(p_port, p_pin, p_af, p_adc_num, p_adc_channel)
        pin_macro = "PIN_ANALOG" if self._analog_only else "PIN"
        return "{:s}({:s}, {:d}, pin_{:s}_af, {:s}, {:d})".format(
            pin_macro, self._port, self._pin, self.name(), adc_units_bitfield, self._adc_channel
        )

    # This will be called at the start of the output (after the prefix). Use
    # it to emit the af objects (via the AF() macro).
    def print_source(self, out_source):
        print(file=out_source)
        print("const pin_af_obj_t pin_{:s}_af[] = {{".format(self.name()), file=out_source)
        for af in self._afs:
            if af.af_fn in CONDITIONAL_VAR:
                print(
                    "    #if defined({:s})".format(
                        CONDITIONAL_VAR[af.af_fn].format(num=af.af_unit)
                    ),
                    file=out_source,
                )
            if af.af_supported:
                print("    ", end="", file=out_source)
            else:
                print("    // ", end="", file=out_source)
            # AF(af_idx, af_fn, af_unit, af_type, af_ptr)
            print(
                "AF({:d}, {:s}, {:d}, {:s}, {:s}{:s}),  // {:s}".format(
                    af.af_idx,
                    af.af_fn,
                    af.af_unit or 0,
                    af.af_pin or "NONE",
                    af.af_fn,
                    "" if af.af_unit is None else str(af.af_unit),
                    af.af_name,
                ),
                file=out_source,
            )
            if af.af_fn in CONDITIONAL_VAR:
                print("    #endif", file=out_source)
        print("};", file=out_source)

    # STM32 cpu names must be "P<port><num>".
    @staticmethod
    def validate_cpu_pin_name(cpu_pin_name):
        boardgen.Pin.validate_cpu_pin_name(cpu_pin_name)

        if not re.match("P[A-O][0-9]+(_C)?$", cpu_pin_name):
            raise boardgen.PinGeneratorError("Invalid cpu pin name '{}'".format(cpu_pin_name))


class Stm32PinGenerator(boardgen.PinGenerator):
    def __init__(self):
        # Use custom pin type above, and also enable the --af-csv argument so
        # that add_af gets called on each pin.
        super().__init__(
            pin_type=Stm32Pin,
            enable_af=True,
        )

    # STM32-specific behavior, we use pin_A0 for the cpu names, but
    # pyb_pin_X11 for the board names.
    def board_name_define_prefix(self):
        return "pyb_"

    # Override the default implementation just to change the default arguments
    # (extra header row, skip first column).
    def parse_af_csv(self, filename):
        return super().parse_af_csv(filename, header_rows=2, pin_col=1, af_col=2)

    # Find which ADCs are used on this chip and on how many pins and the
    # maximum channel number for each.
    def count_adc_pins(self):
        adc_units = defaultdict(lambda: (0, 0))
        for pin in self._pins:  # All pins
            for unit in pin._adc_units:
                num, max_channel = adc_units[unit]
                if pin._available:
                    adc_units[unit] = num + 1, max(max_channel, pin._adc_channel)
        return adc_units.items()

    # Print table of pins for each ADC (indexed by channel).
    def print_adcs(self, out_source):
        for adc_unit, (num_pins, max_channel) in self.count_adc_pins():
            print(file=out_source)
            print(
                "const machine_pin_obj_t * const pin_adc{:d}[{:d}] = {{".format(
                    adc_unit, max_channel + 1
                ),
                file=out_source,
            )
            # Don't include pins that weren't in pins.csv.
            for pin in self.available_pins():
                if pin._hidden:
                    continue
                if adc_unit in pin._adc_units:
                    print(
                        "    [{:d}] = {:s},".format(pin._adc_channel, self._cpu_pin_pointer(pin)),
                        file=out_source,
                    )
            print("};", file=out_source)

    # Print externs for the adc pin tables.
    def print_adc_externs(self, out_source):
        print(file=out_source)
        for adc_unit, (num_pins, max_channel) in self.count_adc_pins():
            print(
                "extern const machine_pin_obj_t * const pin_adc{:d}[{:d}];".format(
                    adc_unit, max_channel + 1
                ),
                file=out_source,
            )

    # Append ADC definitions to the end of the source output.
    def print_source(self, out_source):
        super().print_source(out_source)
        self.print_adcs(out_source)

    # Append ADC externs to the end of the header output, and don't include
    # externs in mboot mode.
    def print_header(self, out_header):
        if self.args.mboot_mode:
            self.print_defines(out_header, cpu=False)
        else:
            super().print_header(out_header)
            self.print_adc_externs(out_header)

    # This is a set of map entries `MP_QSTR_AF<num>_<fn>` -> `GPIO_AF<num>_<fn>`
    # that become part of the locals dict of machine.Pin.
    def print_af_const(self, out_af_const):
        # Extract all unique "AF<num>_<fn>" values.
        names = set()
        for pin in self.available_pins():
            for af in pin._afs:
                if not af.af_supported:
                    continue
                key = (
                    "AF{:d}_{:s}{:d}".format(af.af_idx, af.af_fn, af.af_unit),
                    af.af_fn,
                    af.af_unit,
                )
                names.add(key)

        # Generate the table.
        for key in sorted(names):
            name, af_fn, af_unit = key
            if af_fn in CONDITIONAL_VAR:
                print(
                    "    #if defined({:s})".format(CONDITIONAL_VAR[af_fn].format(num=af_unit)),
                    file=out_af_const,
                )
            print(
                "    {{ MP_ROM_QSTR(MP_QSTR_{:s}), MP_ROM_INT(GPIO_{:s}) }},".format(name, name),
                file=out_af_const,
            )
            if af_fn in CONDITIONAL_VAR:
                print("    #endif", file=out_af_const)

    # Output macros to compile-time match a pin object to its AF. See
    # mp_hal_pin_config_alt_static and mp_hal_pin_config_alt_static_speed in
    # pin_static_af.h.
    def print_af_defs(self, out_af_defs):
        # Get the set of unique "<fn><unit>_<pinfn>" (e.g. I2C1_SDA) and which
        # pins can be used for each.
        af_defs = defaultdict(list)
        for pin in self._pins:
            for af in pin._afs:
                key = af.af_fn, af.af_unit, af.af_pin
                af_defs[key].append((pin, af.af_idx))

        # Emit a macro for each that will match a pin object to the
        # corresponding af index for that pin doing that function.
        for key, pins in af_defs.items():
            af_fn, af_unit, af_pin = key
            print(file=out_af_defs)
            print(
                "#define STATIC_AF_{:s}{:s}_{:s}(pin_obj) ( \\".format(
                    af_fn, "" if af_unit is None else str(af_unit), af_pin or "NULL"
                ),
                file=out_af_defs,
            )
            for pin, af_idx in pins:
                if self.args.mboot_mode:
                    print(
                        "    ((pin_obj) == (pin_{:s})) ? ({:d}) : \\".format(pin.name(), af_idx),
                        file=out_af_defs,
                    )
                else:
                    # Match either "(pin_A11_obj)" (if using pin_A11 or
                    # pyb_pin_X11) or "((pin_A11_obj))" (if going via another
                    # macro e.g. MICROPY_HW_QSPIFLASH_CS).
                    # TODO: Why do we need do do string matching? (i.e. why can't the mboot behavior be used always?).
                    print(
                        '    ((strcmp( #pin_obj , "(&pin_{:s}_obj)") & strcmp( #pin_obj , "((&pin_{:s}_obj))")) == 0) ? ({:d}) : \\'.format(
                            pin.name(), pin.name(), af_idx
                        ),
                        file=out_af_defs,
                    )
            print("    (0xffffffffffffffffULL))", file=out_af_defs)

    # Additional stm32-specific outputs that will be written in
    # generate_extra_files().
    def extra_args(self, parser):
        parser.add_argument("--output-af-const")
        parser.add_argument("--output-af-defs")

        # In mboot mode the af-defs use object rather than string comparison,
        # and we don't include externs in the header file.
        parser.add_argument("--mboot-mode", action="store_true")

    # Called in main() after everything else is done to write additional files.
    def generate_extra_files(self):
        if self.args.output_af_const:
            with open(self.args.output_af_const, "w") as out_af_const:
                self.print_af_const(out_af_const)

        if self.args.output_af_defs:
            with open(self.args.output_af_defs, "w") as out_af_defs:
                self.print_af_defs(out_af_defs)


if __name__ == "__main__":
    Stm32PinGenerator().main()
