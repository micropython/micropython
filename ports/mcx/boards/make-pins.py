#!/usr/bin/env python
"""Creates the pin file for MCX series"""

import os
import re
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "../../../tools"))
import boardgen

# Dictionary contains the supported AF and its regexp to select one
SUPPORTED_AF = {
    "GPIO": "P([0-9]+)_([0-9]+)",
    "LPFC": "FC([0-9]+)_P([0-9]+)",
    "CT32": "CT([0-9]*)_(MAT|INP)([0-9]+)",
}

ADC_RE = "ADC([0-9]+)_([A-B])([0-9]+)"


class MCXPin(boardgen.Pin):
    def __init__(self, cpu_pin_name):
        super().__init__(cpu_pin_name)
        self.__af_table = []
        self.__adc_table = []

    def add_af(self, af_idx, af_name, af):
        if af_name == "ADC":
            m = re.match(ADC_RE, af)
            if not m:
                print("Unable to detect ADC parameter from AF {:s}".format(af))
            else:
                self.__af_add_adc(int(m.group(1)), m.group(2), int(m.group(3)))
        else:
            for k, v in SUPPORTED_AF.items():
                m = re.match(v, af)
                if m:
                    match k:
                        case "GPIO":
                            self.__af_add_gpio(af_idx, int(m.group(1)), int(m.group(2)))
                        case "LPFC":
                            self.__af_add_flexcomm(af_idx, int(m.group(1)), int(m.group(2)))
                        case "CT32":
                            # Instance may be None.
                            instance = m.group(1)
                            if instance == "":
                                instance = "0"

                            attribute = "{:s}{:s}".format(m.group(2), m.group(3))

                            self.__af_add_ctimer(af_idx, int(instance), attribute)
                        case _:
                            # Simply does not exist.
                            print("??")

    def definition(self):
        port, pin = self.__get_pin_id()

        af_name = "pin_{:d}_{:d}_af".format(port, pin)

        adc_name = "NULL"
        if len(self.__adc_table):
            adc_name = "&pin_{:d}_{:d}_adc".format(port, pin)

        pin_def = "PIN({:s}, {:d}, {:d}, {:s}, {:s})".format(
            self.name(), port, pin, af_name, adc_name
        )

        return pin_def

    def print_source(self, out_source):
        port, pin = self.__get_pin_id()

        print("const machine_pin_af_t pin_{:d}_{:d}_af[] = {{".format(port, pin), file=out_source)
        for af_item in self.__af_table:
            print(
                "    AF({:d}, {:s}, {:d}, {:s}),".format(
                    af_item["index"], af_item["type"], af_item["instance"], af_item["attribute"]
                ),
                file=out_source,
            )

        print("};", file=out_source)

        for adc_item in self.__adc_table:
            # Add ADC item entries.
            print(
                "const machine_pin_adc_t pin_{:d}_{:d}_adc = ADC({:d}, '{:s}', {:d});".format(
                    port, pin, adc_item["instance"], adc_item["side"], adc_item["channel"]
                ),
                file=out_source,
            )

        print("", file=out_source)

    def __af_add_adc(self, instance, side, channel):
        self.__adc_table.append({"instance": instance, "side": side, "channel": channel})

    def __af_add_gpio(self, af_idx, instance, attribute):
        self.__af_table.append(
            {
                "type": "GPIO",
                "index": af_idx,
                "instance": instance,
                "attribute": "GPIO_P{:d}".format(attribute),
            }
        )

    def __af_add_flexcomm(self, af_idx, instance, attribute):
        self.__af_table.append(
            {
                "type": "LPFC",
                "index": af_idx,
                "instance": instance,
                "attribute": "LPFC_P{:d}".format(attribute),
            }
        )

    def __af_add_ctimer(self, af_idx, instance, attribute):
        self.__af_table.append(
            {
                "type": "CT32",
                "index": af_idx,
                "instance": instance,
                "attribute": "CT32_{:s}".format(attribute),
            }
        )

    def __get_pin_id(self):
        m = re.match("P([0-9]+)_([0-9]+)", self.name())
        if not m:
            raise boardgen.PinGeneratorError(
                "Port ID not found for given pin {:s}".format(self.name())
            )

        return int(m.group(1)), int(m.group(2))


class MCXPinGenerator(boardgen.PinGenerator):
    def __init__(self):
        # Use custom pin type above, and also enable the --af-csv argument.
        super().__init__(
            pin_type=MCXPin,
            enable_af=True,
        )

    def board_name_define_prefix(self):
        return "pyb_"

    def parse_af_csv(self, filename):
        return super().parse_af_csv(filename, header_rows=1, pin_col=1, af_col=2)

    def print_source(self, out_source):
        super().print_source(out_source)

    def print_header(self, out_header):
        super().print_header(out_header)


if __name__ == "__main__":
    MCXPinGenerator().main()
