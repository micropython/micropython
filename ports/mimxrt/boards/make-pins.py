#!/usr/bin/env python

from collections import defaultdict
import os
import re
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../../../tools"))
import boardgen


IOMUX_REGEXS = [
    r"IOMUXC_(?P<pin>GPIO_SD_B\d_\d\d)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_(?P<pin>GPIO_AD_B\d_\d\d)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_(?P<pin>GPIO_EMC_\d\d)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_(?P<pin>GPIO_B\d_\d\d)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_(?P<pin>GPIO_\d\d)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_(?P<pin>GPIO_AD_\d\d)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_(?P<pin>GPIO_SD_\d\d)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_(?P<pin>GPIO_EMC_B\d_\d\d)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_(?P<pin>GPIO_DISP_B\d_\d\d)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_(?P<pin>GPIO_LPSR_\d\d)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_[SNVS_]*(?P<pin>WAKEUP[_DIG]*)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_[SNVS_]*(?P<pin>PMIC_ON_REQ[_DIG]*)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_[SNVS_]*(?P<pin>PMIC_STBY_REQ[_DIG]*)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    r"IOMUXC_(?P<pin>GPIO_SNVS_\d\d_DIG)_(?P<function>\w+) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
]

SUPPORTED_AF_FNS = {"GPIO", "USDHC", "FLEXPWM", "TMR"}


class MimxrtPin(boardgen.Pin):
    def __init__(self, cpu_pin_name):
        super().__init__(cpu_pin_name)

        self._afs = []
        self._adc_fns = []

    # Called for each AF defined in the csv file for this pin.
    def add_af(self, af_idx, af_name, af):
        # mimxrt-specific: Any pin in the af.csv is implicitly part of the board
        # pins.csv and will therefore be in the Pin.cpu dict. This is
        # equivalent to adding `,CPUNAME` to the board.csv for every pin in the
        # af.csv.
        self._available = True

        if af_name == "ALT5":
            m = re.match("GPIO([0-9]+)_IO([0-9]+)", af)
            self._port = int(m.group(1))
            self._pin = int(m.group(2))

        if af_name.startswith("ALT"):
            instance = af.split("_")[0]
            fn = re.match("^([A-Z][A-Z0-9]+[A-Z])[0-9]*$", instance).group(1)
            if fn not in SUPPORTED_AF_FNS:
                return
            iomux_config = self._generator._iomux_pin_config[self.name()]
            input_reg = iomux_config[af_idx]["inputRegister"].strip("U")
            input_daisy = int(iomux_config[af_idx]["inputDaisy"].strip("U"), 16)
            self._afs.append((af_idx, input_reg, input_daisy, instance, fn, af))

        elif af_name == "ADC":
            adc_regex = r"ADC(?P<instance>\d*)_IN(?P<channel>\d*)"
            lpadc_regex = r"ADC(?P<instance>\d*)_CH(?P<channel>\d*)"  # LPADC for MIMXRT11xx chips

            matches = re.finditer(adc_regex, af, re.MULTILINE)
            for match in matches:
                self._adc_fns.append(
                    (int(match.group("instance")), int(match.group("channel")), "ADC")
                )

            matches = re.finditer(lpadc_regex, af, re.MULTILINE)
            for match in matches:
                self._adc_fns.append(
                    (int(match.group("instance")), int(match.group("channel")), "LPADC")
                )

    # Use the PIN() macro defined in samd_prefix.c for defining the pin
    # objects.
    def definition(self):
        if "_LPSR_" in self.name():
            macro = "PIN_LPSR"
        elif (
            "_SNVS_" in self.name()
            or self.name().startswith("PMIC_")
            or self.name().startswith("WAKEUP")
        ):
            macro = "PIN_SNVS"
        else:
            macro = "PIN"

        # PIN(_name, _gpio, _pin, _af_list, _adc_list_len, _adc_list)
        return "{:s}({:s}, GPIO{:d}, {:d}, pin_{:s}_af, {:d}, {:s})".format(
            macro,
            self.name(),
            self._port,
            self._pin,
            self.name(),
            len(self._adc_fns),
            "pin_{:s}_adc".format(self.name()) if self._adc_fns else "NULL",
        )

    # This will be called at the start of the output (after the prefix) for
    # each pin. Use it to emit the af and adc objects.
    def print_source(self, out_source):
        print(
            "static const machine_pin_af_obj_t pin_{:s}_af[] = {{".format(self.name()),
            file=out_source,
        )
        for af_idx, input_reg, input_daisy, instance, _fn, af in self._afs:
            print(
                "    PIN_AF({:s}, PIN_AF_MODE_ALT{:d}, {:d}, {:s}, {:s}, {:s}),".format(
                    af, af_idx, input_daisy, instance, input_reg, "0x10B0U"
                ),
                file=out_source,
            )
        print("};", file=out_source)
        print(file=out_source)

        if self._adc_fns:
            print(
                "static const machine_pin_adc_obj_t pin_{:s}_adc[] = {{".format(self.name()),
                file=out_source,
            )
            for instance, channel, peripheral in self._adc_fns:
                print(
                    "    PIN_ADC({:s}{:d}, {:d}),".format(peripheral, instance, channel),
                    file=out_source,
                )
            print("};", file=out_source)

    # mimxrt cpu names must be "GPIO_<funcs>_<num>", with zero-prefixed two-digit <num>.
    @staticmethod
    def validate_cpu_pin_name(cpu_pin_name):
        boardgen.Pin.validate_cpu_pin_name(cpu_pin_name)

        if not re.match(
            "^((GPIO_((SNVS|EMC|LPSR|DISP)_)?([AS]D_)?(B[012]_)?[0-9][0-9])|WAKEUP|PMIC_(ON|STBY)_REQ)(_DIG)?$",
            cpu_pin_name,
        ):
            raise boardgen.PinGeneratorError("Invalid cpu pin name '{}'".format(cpu_pin_name))


class MimxrtPinGenerator(boardgen.PinGenerator):
    def __init__(self):
        # Use custom pin type above, and also enable the --af-csv argument so
        # that add_af gets called on each pin.
        super().__init__(
            pin_type=MimxrtPin,
            enable_af=True,
        )
        self._iomux_pin_config = {}

    # Load the iomux configuration from fsl_iomuxc.h.
    def load_iomux_header(self, iomux_filename):
        with open(iomux_filename, "r") as ipt:
            input_str = ipt.read()
            for regex in IOMUX_REGEXS:
                matches = re.finditer(regex, input_str, re.MULTILINE)

                for match in matches:
                    if match.group("pin") not in self._iomux_pin_config:
                        self._iomux_pin_config[match.group("pin")] = {
                            int((match.groupdict()["muxMode"].strip("U")), 16): match.groupdict()
                        }
                    else:
                        self._iomux_pin_config[match.group("pin")][
                            int((match.groupdict()["muxMode"].strip("U")), 16)
                        ] = match.groupdict()

    # Also load the iomux header.
    def load_inputs(self, out_source):
        if self.args.iomux_header:
            print("// --iomux-header {:s}".format(self.args.iomux_header), file=out_source)
            self.load_iomux_header(self.args.iomux_header)

        super().load_inputs(out_source)

    # Provide a macro for each supported (pin,af) that can be used to
    # initialise a struct containing a machine_pin_obj_t* and the
    # corresponding af for that pin. e.g. A mimxrt_sdcard_pin_t instance can
    # be initialised with GPIO_SD_B0_00_USDHC1_CMD which tells it how to get
    # the CMD pin of the USDHC1 function on the GPIO_SD_B0_00 pin.
    def print_module_instances(self, out_header):
        print(file=out_header)
        for match_fn in ("USDHC", "FLEXPWM", "TMR"):
            module_instances = defaultdict(list)
            for pin in self.available_pins():
                for i, (_af_idx, _input_reg, _input_daisy, instance, fn, af) in enumerate(
                    pin._afs
                ):
                    if fn == match_fn:
                        module_instances[instance].append(
                            "#define {:s}_{:s} pin_{:s}, {:d}".format(
                                pin.name(), af, pin.name(), i
                            )
                        )
            for k, v in module_instances.items():
                print("// {:s}".format(k), file=out_header)
                print("#define {:s}_AVAIL (1)".format(k), file=out_header)
                if match_fn == "FLEXPWM":
                    print("#define {:s} {:s}".format(k, k[-4:]), file=out_header)
                for i in v:
                    print(i, file=out_header)

    # Override to also print the module instances.
    def print_header(self, out_header):
        super().print_header(out_header)
        self.print_module_instances(out_header)

    # Override the default implementation just to change the default arguments
    # (extra header row, skip first column).
    def parse_af_csv(self, filename):
        return super().parse_af_csv(filename, header_rows=1, pin_col=0, af_col=1)

    # We need to know the mcu to emit the correct AF list.
    def extra_args(self, parser):
        parser.add_argument("--iomux-header")


if __name__ == "__main__":
    MimxrtPinGenerator().main()
