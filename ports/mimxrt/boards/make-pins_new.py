#!/usr/bin/env python
"""Creates the pin file for the MIMXRT10xx."""

import argparse
import csv
import re
import warnings

"""
typedef struct gpio_mux_info_t_ {
    uint16_t:1 available;  # `1` if Pad supports peripheral function
    uint16_t:5 mux_mode;  # Alternate function index - e.g. 4 for AF4
    uint16_t:4 instance_idx;  # Instance number - e.g. `1` for LPSPI1
    uint16_t:6 io;
    uint16_t misc;
    uint32_t mux_reg;
    uint32_t config_reg;    
} gpio_mux_info_t;

typedef struct adc_mux_info_t_ {
    uint32_t placeholder;
} adc_mux_info_t;

typedef struct acmp_mux_info_t_ {
    uint32_t placeholder;
} acmp_mux_info_t;

// Todo: Make mux array dynamic dependent on actually available alternate functions of chip

typedef struct pin_config_t_ {
    gpio_mux_info_t gpio_mux[9];
    adc_mux_info_t adc_mux;
    acmp_mux_info_t acmp_mux;
}
"""

"""
ENET
FLEXIO
FLEXPWM
FLEXSPI
GPIO
GPT
KPP
LPSPI
LPUART
MQS
PIT
SAI
SPDIF
TMR
USB
---
ADC
ACMP

"""

"""
- Erzeugen von separaten mux konfigurationen für ADC und ACMP
    - vor allem bei ACMP muss die Kombination von ACMP_IN und ACMP_OUT auf einem GPIO berücksichtigt werden
- Zusammenführung von geparsten informaiton aus der iomux und dem csv. IM CSV müsste igentlich nur infos für ADC und ACMP stehen!
- Eventuell könnte mux und config reg auf weniger Speicherplatz getrimmt werden wenn:
    - lookup bei wenigen basisadressen mit index
    - offset in 16/8bit mit Speichern der Basis   
- Speichern von inputRegister inputDaisy   in der mux config // Ausgabe
- besser gekapselter pretty print
- zusammenfügen des parsens von af.csv und iomux um direkt "volle" Pin Objekte zu erzeugen
- mux array abhängig von gewünschten/unterstützten mux modes//AFs -> einbauen für code generierung
- prüfung einbauen ob gleiches peripheral bei einem gpio mehrer mux mides belegt (ACMP bekannte ausnahme die gesondert behandelt werden muss)
- herausfindne wie groß die pin datentypen derzeit sind
"""


class PinAf(object):
    def __init__(self, **kwargs):
        self.peripheral = kwargs.get("peripheral", "")
        # mux_mode
        mux_mode_temp = kwargs.get("muxMode", "")
        mux_mode_temp = mux_mode_temp.strip("U")  # Remove unsigned specifier
        self.mux_mode = int(mux_mode_temp, base=16)  # Create int from hex string
        #
        self.instance = kwargs.get("instance", "")
        self.instance_number = kwargs.get("instance_number", "")
        #
        self.pin_function = kwargs.get("pin_function", "")
        self.mux_register = kwargs.get("muxRegister", "")
        self.input_register = kwargs.get("inputRegister", "")
        # input_daisy
        input_daisy_temp = kwargs.get("inputDaisy", "")
        input_daisy_temp = input_daisy_temp.strip("U")  # Remove unsigned specifier
        self.input_daisy = int(input_daisy_temp, base=16)  # Create int from hex string
        #
        self.config_register = kwargs.get("configRegister", "")

    def __repr__(self):
        return f"[{self.mux_mode}] {self.peripheral:10} {self.pin_function}"


class PinNew(object):
    def __init__(self, pad):
        self.pad = pad
        self.afs = dict()

    def add_af(self, af):
        if af.peripheral in self.afs:
            warnings.warn(f"{self.pad} already contains AF for {af.peripheral}!")
        self.afs[af.peripheral] = af

    def __hash__(self):
        return hash(self.pad)

    def __eq__(self, other):
        if hash(other) == hash(self):
            return True
        else:
            return False

    def __lt__(self, other):
        return self.pad < other.pad

    def __repr__(self):
        return self.pad


class Pin(object):
    af_pattern = (
        r"(?P<instance>(?P<peripheral>[a-zA-Z]+)(?P<instance_number>\d?))\_?(?P<pin_function>\w*)"
    )

    def __init__(self, pad, afs, alt_name=""):
        self.pad = pad
        self.alt_name = alt_name

        # Split alternate function into:
        #   FLEXPWM2_PWM1_A
        #  |________|______|
        #       |       |----> <pin_function> PWM1_A
        #       |------------> <instance>     FLEXPWM2
        #                                    |______|_|
        #       <peripheral>      FLEXPWM <------|   |
        #       <instance_number> 2 <----------------|
        self.afs = dict()
        pattern = re.compile(self.af_pattern)
        #
        for i, af in enumerate(afs):
            match = pattern.match(af)
            if match:
                af_dict = match.groupdict()
                af_dict["af_idx"] = i
                self.afs[match.group("peripheral")] = af_dict

    def __hash__(self):
        return hash(self.pad)

    def __eq__(self, other):
        if hash(other) == hash(self):
            return True
        else:
            return False

    def __lt__(self, other):
        return self.pad < other.pad

    def __str__(self):
        # Get GPIO info
        gpio_af = self.afs["GPIO"]
        return f"{self.pad} ({self.alt_name}) - {gpio_af['instance']} {gpio_af['pin_function']}"

    def __repr__(self):
        return self.pad


def parse_mux_files(mux_file, user_mux_file):
    pins_dict = dict()  # {"pad": Pin}

    # Parse mux file and generate Pin objects from it
    regexes = [
        r"IOMUXC_(?P<pad>GPIO_SD_B\d_\d\d)_(?P<instance>(?P<peripheral>[a-zA-Z]+)(?P<instance_number>\d?))\_?(?P<pin_function>\w*) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
        r"IOMUXC_(?P<pad>GPIO_AD_B\d_\d\d)_(?P<instance>(?P<peripheral>[a-zA-Z]+)(?P<instance_number>\d?))\_?(?P<pin_function>\w*) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
        r"IOMUXC_(?P<pad>GPIO_EMC_\d\d)_(?P<instance>(?P<peripheral>[a-zA-Z]+)(?P<instance_number>\d?))\_?(?P<pin_function>\w*) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
        r"IOMUXC_(?P<pad>GPIO_B\d_\d\d)_(?P<instance>(?P<peripheral>[a-zA-Z]+)(?P<instance_number>\d?))\_?(?P<pin_function>\w*) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
        r"IOMUXC_(?P<pad>GPIO_\d\d)_(?P<instance>(?P<peripheral>[a-zA-Z]+)(?P<instance_number>\d?))\_?(?P<pin_function>\w*) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
        r"IOMUXC_(?P<pad>GPIO_AD_\d\d)_(?P<instance>(?P<peripheral>[a-zA-Z]+)(?P<instance_number>\d?))\_?(?P<pin_function>\w*) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
        r"IOMUXC_(?P<pad>GPIO_SD_\d\d)_(?P<instance>(?P<peripheral>[a-zA-Z]+)(?P<instance_number>\d?))\_?(?P<pin_function>\w*) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    ]

    with open(mux_file, "r") as mux_file_:
        input_str = mux_file_.read()

        # Create iomux configuration tree
        #
        # From:
        # #define IOMUXC_GPIO_AD_B0_00_GPT1_COMPARE1 0x401F80BCU, 0x7U, 0,           0,    0x401F8230U
        # #define IOMUXC_GPIO_EMC_41_SPDIF_IN        0x401F80B8U, 0x2U, 0x401F8488U, 0x1U, 0x401F822CU
        #
        # Generate:
        #  {
        #    "GPIO_AD_B0_00": {
        #       'pad': 'GPIO_AD_B0_00',
        #       'instance': 'GPT1',
        #       'peripheral': 'GPT',
        #       'instance_number': '1',
        #       'pin_function': 'COMPARE1',
        #       'muxRegister': '0x401F80BCU',
        #       'muxMode': '0x7U',
        #       'inputRegister': '0',
        #       'inputDaisy': '0',
        #       'configRegister': '0x401F8230U'
        #    }
        # }

        for regex in regexes:
            matches = re.finditer(regex, input_str, re.MULTILINE)

            for match in matches:
                pin_af = PinAf(**match.groupdict())

                if match.group("pad") not in pins_dict:
                    pins_dict[match.group("pad")] = PinNew(match.group("pad"))

                pins_dict[match.group("pad")].add_af(pin_af)

    print(len(pins_dict))
    print(pins_dict["GPIO_AD_B1_01"])
    for af in pins_dict["GPIO_AD_B1_01"].afs:
        print("\t" + str(af))

    # Parse user mux file and enricht pin objets with more information
    # Return list of pins
    pass


def parse_af_file(af_file, pad_col, af_start_col):
    af_end_col = af_start_col + 12
    available_pins = list()
    available_afs = set()

    with open(af_file, "r") as csvfile:
        _available_pins = set()
        rows = csv.reader(csvfile)
        header = next(rows)
        for row in rows:
            _available_pins.add(Pin(row[pad_col], row[af_start_col:af_end_col]))

            # Parse alternate functions
            for af_idx, af in enumerate(row[af_start_col:af_end_col]):
                match = re.match(Pin.af_pattern, af)
                if match:
                    available_afs.add(match.group("peripheral"))
        available_pins = list(_available_pins)
    available_pins.sort()

    return {available_pin.pad: available_pin for available_pin in available_pins}, list(
        available_afs
    )


def parse_iomux_file(iomux_file):
    iomux_pin_configs = dict()
    regexes = [
        r"IOMUXC_(?P<pad>GPIO_SD_B\d_\d\d)_(?P<instance>(?P<peripheral>[a-zA-Z]+)(?P<instance_number>\d?))\_?(?P<pin_function>\w*) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
        r"IOMUXC_(?P<pad>GPIO_AD_B\d_\d\d)_(?P<instance>(?P<peripheral>[a-zA-Z]+)(?P<instance_number>\d?))\_?(?P<pin_function>\w*) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
        r"IOMUXC_(?P<pad>GPIO_EMC_\d\d)_(?P<instance>(?P<peripheral>[a-zA-Z]+)(?P<instance_number>\d?))\_?(?P<pin_function>\w*) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
        r"IOMUXC_(?P<pad>GPIO_B\d_\d\d)_(?P<instance>(?P<peripheral>[a-zA-Z]+)(?P<instance_number>\d?))\_?(?P<pin_function>\w*) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
        r"IOMUXC_(?P<pad>GPIO_\d\d)_(?P<instance>(?P<peripheral>[a-zA-Z]+)(?P<instance_number>\d?))\_?(?P<pin_function>\w*) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
        r"IOMUXC_(?P<pad>GPIO_AD_\d\d)_(?P<instance>(?P<peripheral>[a-zA-Z]+)(?P<instance_number>\d?))\_?(?P<pin_function>\w*) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
        r"IOMUXC_(?P<pad>GPIO_SD_\d\d)_(?P<instance>(?P<peripheral>[a-zA-Z]+)(?P<instance_number>\d?))\_?(?P<pin_function>\w*) (?P<muxRegister>\w+), (?P<muxMode>\w+), (?P<inputRegister>\w+), (?P<inputDaisy>\w+), (?P<configRegister>\w+)",
    ]

    with open(iomux_file, "r") as iomuxfile:
        input_str = iomuxfile.read()

        # Create iomux configuration tree
        #
        # From:
        # #define IOMUXC_GPIO_AD_B0_00_GPT1_COMPARE1 0x401F80BCU, 0x7U, 0, 0, 0x401F8230U
        #
        # Generate:
        #  {
        #    "GPIO_AD_B0_00": {
        #       'pad': 'GPIO_AD_B0_00',
        #       'instance': 'GPT1',
        #       'peripheral': 'GPT',
        #       'instance_number': '1',
        #       'pin_function': 'COMPARE1',
        #       'muxRegister': '0x401F80BCU',
        #       'muxMode': '0x7U',
        #       'inputRegister': '0',
        #       'inputDaisy': '0',
        #       'configRegister': '0x401F8230U'
        #    }
        # }

        for regex in regexes:
            matches = re.finditer(regex, input_str, re.MULTILINE)

            for match in matches:
                if match:
                    iomux_pin_configs[match.group("pad")] = match.groupdict()
    return iomux_pin_configs


def parse_board_pins_file(board_file):
    board_pins = dict()
    with open(board_file, "r") as csvfile:
        rows = csv.reader(csvfile)
        for row in rows:
            if len(row) == 0 or row[0].startswith("#"):
                # Skip empty lines, and lines starting with "#"
                continue
            if len(row) != 2:
                raise ValueError("Expecting two entries in a row")

            # Todo: Add error when pin is found multiple times
            board_pins[row[1]] = row[0]

    # Return dict with {pad: alt_name}
    return board_pins


def main():
    parser = argparse.ArgumentParser(
        prog="make-pins.py",
        usage="%(prog)s [options] [command]",
        description="Generate board specific pin file",
    )
    parser.add_argument(
        "-a",
        "--af",
        dest="af_filename",
        help="Specifies the alternate function file for the chip",
        default="mimxrt1021_af.csv",
    )
    parser.add_argument(
        "-i",
        "--iomux",
        dest="iomux_filename",
        help="Specifies the fsl_iomuxc.h file for the chip",
        default="fsl_iomuxc.h",
    )
    parser.add_argument(
        "-b",
        "--board",
        dest="board_filename",
        help="Specifies the board file",
        default="MIMXRT1020_EVK/pins.csv",
    )
    parser.add_argument(
        "-p",
        "--prefix",
        dest="prefix_filename",
        help="Specifies beginning portion of generated pins file",
        default="mimxrt_prefix.c",
    )
    parser.add_argument(
        "-r",
        "--hdr",
        dest="hdr_filename",
        help="Specifies name of generated pin header file",
        default="build/pins.h",
    )

    # test code
    args = parser.parse_args(
        [
            "-a=/home/philipp/Projects/micropython/micropython/ports/mimxrt/boards/MIMXRT1064_af.csv",
            "-i=/home/philipp/Projects/micropython/micropython/lib/nxp_driver/sdk/devices/MIMXRT1021/drivers/fsl_iomuxc.h",
        ]
    )
    #

    parse_mux_files(args.iomux_filename, args.af_filename)

    """
    available_pins, available_afs = parse_af_file(args.af_filename, 0, 1)
    iomux_configs = parse_iomux_file(args.iomux_filename)
    board_pins = parse_board_pins_file(args.board_filename)
    #
    available_afs = [
        "ENET",
        "FLEXIO",
        "FLEXPWM",
        "FLEXSPI",
        "GPIO",
        "GPT",
        "KPP",
        "LPSPI",
        "LPUART",
        "MQS",
        "PIT",
        "SAI",
        "SPDIF",
        "TMR",
        "USB",
        "ADC",
    ]
    #
    pins_to_generate = list()
    for pad, alt_name in board_pins.items():
        pin = available_pins[pad]
        pin.alt_name = alt_name
        pins_to_generate.append(pin)
    #
    def print_pin_config(pin, indent="", end=","):
        print(indent + "[" + pin.pad + "] " + "=" + " {")
        for alt_fn in available_afs[:-1]:
            if alt_fn in pin.afs:
                print((indent * 2) + "[" + alt_fn + "]" + " = " + f"GPIO_MUX(1U, {pin.afs[alt_fn]['af_idx']}, {pin.afs[alt_fn]['instance_number']}, {pin.afs['GPIO']['pin_function'].lstrip('IO')}, 0U, {iomux_configs[pin.pad]['muxRegister']}, {iomux_configs[pin.pad]['configRegister']}), ")
            else:
                print((indent * 2) + "[" + alt_fn + "]" + " = " + f"GPIO_MUX(0U, 0U, 0U, 0U, 0U, 0U, 0U), ")
        
        if available_afs[-1] in pin.afs:
            print((indent * 2) + "[" + available_afs[-1] + "]" + " = " + f"GPIO_MUX(1U, {pin.afs[available_afs[-1]]['af_idx']}, {pin.afs[available_afs[-1]]['instance_number']}, {pin.afs['GPIO']['pin_function'].lstrip('IO')}, 0U, {iomux_configs[pin.pad]['muxRegister']}, {iomux_configs[pin.pad]['configRegister']}), ")
        else:
            print((indent * 2) + "[" + available_afs[-1] + "]" + " = " + f"GPIO_MUX(0U, 0U, 0U, 0U, 0U, 0U, 0U), ")
        
        print(indent + "}" + end)

    print("pinconfigs = ")
    print("{")
    for pin in pins_to_generate[:-1]:
        print_pin_config(pin, indent="    ")
    print_pin_config(pins_to_generate[-1],  indent="    ", end="")
    print("};")
    """


# .available=1U, .mux_mode=10, .instance_idx=, .io=, .misc=, .mux_reg=, .config_reg=


if __name__ == "__main__":
    main()
