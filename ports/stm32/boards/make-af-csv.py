#!/usr/bin/env python
"""
Generates MCU alternate-function (AF) definition files for stm32 parts
using the official sources from:
https://github.com/STMicroelectronics/STM32_open_pin_data

Usage:
$ python3 make-af-csv.py <mcu_name>

Example:
$ python3 make-af-csv.py stm32wb55

"""
import json
import re
import sys
import urllib.request
import xml.etree.ElementTree as ET
from collections import defaultdict
from io import BytesIO
from pathlib import Path


# https://docs.github.com/en/rest/git/trees?apiVersion=2022-11-28#get-a-tree
# Note this API has a limit of 60 (unauthenticated) requests per hour.
repo_url = "https://api.github.com/repos/STMicroelectronics/STM32_open_pin_data/git/trees/"

# Raw xml files can be downloaded here
xml_url = "https://raw.githubusercontent.com/STMicroelectronics/STM32_open_pin_data/master/mcu/"


mcu_list_all = None


def generate_af_csv(target: str) -> None:
    """
    Generates an AF CSV file based on the given target.

    Parameters:
    - target (str): The mcu to generate the AF CSV for.

    """
    global mcu_list_all

    print("Generating AF file for:", target)
    cpu = target.rstrip("x").upper()

    if mcu_list_all is None:
        with urllib.request.urlopen(repo_url + "master") as response:
            repo_list_all = json.load(response)["tree"]
        mcu_list_url = [entry for entry in repo_list_all if entry["path"] == "mcu"][0]["url"]
        with urllib.request.urlopen(mcu_list_url + "?recursive=1") as response:
            mcu_list_all = json.load(response)["tree"]

    # Get list of all mcu files matching the target
    mcu_list = [mcu for mcu in mcu_list_all if mcu.get("path").upper().startswith(cpu)]
    if not mcu_list:
        raise SystemExit('ERROR: Could not find mcu "{}" on: \n{}'.format(cpu, mcu_list_url))

    # Check the mcu definition files for one of the matching mcus,
    # pick largest to include all possible pins.
    gpio_file_url = None
    mcu_xml_len = 0
    mcu_url = None
    for mcu in mcu_list:
        mcu_xml_url = xml_url + mcu["path"]
        with urllib.request.urlopen(mcu_xml_url) as response:
            length = int(response.headers["content-length"])
            if length > mcu_xml_len:
                mcu_xml_len = length
                mcu_url = mcu_xml_url

    with urllib.request.urlopen(mcu_url) as response:
        mcuxmlstr = response.read()
    print("Downloaded:", mcu_url)

    # The STM xml files have a dummy namespace declared which confuses ElementTree.
    # This wrapper iter removes them.
    it = ET.iterparse(BytesIO(mcuxmlstr))
    for _, el in it:
        _, _, el.tag = el.tag.rpartition("}")  # strip ns
    root = it.root

    # This mcu definition file declares which pins have ADC pins. Gather them now.
    adc_pins = dict()
    for pin in root.findall("./Pin"):
        for sig in pin.findall("./Signal"):
            sig_name = sig.get("Name")
            if sig_name.startswith("ADC") and "_IN" in sig_name:
                # On some parts they omit the 1 on the first ADC peripheral
                sig_name = sig_name.replace("ADC_", "ADC1_")
                index, channel = sig_name.split("_")
                pname = pin_name(pin.get("Name"))
                dual_pad_pin = pin.get("Name").endswith("_C")
                if dual_pad_pin:
                    # https://github.com/micropython/micropython/pull/13764
                    pname += "_C"
                # Keep just the ADC index and IN channel.
                n_index = re.search("\d+", index)[0]
                if pname in adc_pins:
                    if channel not in adc_pins[pname]:
                        adc_pins[pname][channel] = n_index
                    else:
                        # Merge ADC unit together, eg ADC1 and ADC3 becomes ADC13
                        adc_pins[pname][channel] = "".join(
                            sorted(adc_pins[pname][channel] + n_index)
                        )
                else:
                    adc_pins[pname] = {channel: n_index}

    # The mcu definition xml file includes a reference to the GPIO definition file
    # matching the chip. This is the file with AF data.
    for detail in root.findall("./IP[@Name='GPIO']"):
        gpio_file = [
            gpio
            for gpio in mcu_list_all
            if gpio["path"].startswith("IP/GPIO") and detail.get("Version") in gpio["path"]
        ][0]
        gpio_file_url = xml_url + gpio_file["path"]
        break

    if gpio_file_url is None:
        raise SystemError("ERROR: Could not find GPIO details in {}".format(mcu_xml_url))

    with urllib.request.urlopen(gpio_file_url) as response:
        xml_data = response.read()

    print("Downloaded:", gpio_file_url)

    # Parse all the alternate function mappings from the xml file.
    it = ET.iterparse(BytesIO(xml_data))
    for _, el in it:
        _, _, el.tag = el.tag.rpartition("}")
    root = it.root

    af_domains = set()

    mapping = defaultdict(list)
    for pin in root.findall("./GPIO_Pin"):
        pname = pin_name(pin.get("Name"))
        if pname is None:
            continue
        _ = mapping[pname]  # Ensure pin gets captured / listed
        for pin_signal in pin.findall("./PinSignal"):
            for specific_param in pin_signal.findall("SpecificParameter"):
                if specific_param.get("Name") != "GPIO_AF":
                    continue
                signal_name = pin_signal.get("Name")
                af_fn = specific_param.find("./PossibleValue").text
                mapping[pname].append((signal_name, af_fn))
                af_domains.add(af_fn.split("_")[1])

    # Note: "AF0" though "AF15" are somewhat standard across STM range,
    # however not all chips use all domains. List them all for consistency however.
    af_domains = ["AF%s" % i for i in range(16)] + ["ADC"]

    # Format the AF data into appropriate CSV file.
    # First row in CSV is the heading, going through all the AF domains
    heading = ["Port", "Pin"] + af_domains
    col_width = [4] * len(heading)
    # Second csv row lists the peripherals handled by that column
    category_parts = [defaultdict(set) for _ in range(len(heading))]
    # Third row onwards are the individual pin function mappings
    rows = []
    for pin in sorted(mapping, key=pin_int):
        functions = mapping[pin]
        row = [""] * len(heading)
        row[0] = "Port%s" % pin[1]
        row[1] = pin
        for signal, af in functions:
            # Some signals are loose about whether 1 is declared or not.
            signal = signal.replace("CAN_", "CAN1_")
            signal = signal.replace("I2S_", "I2S1_")
            # / is used later to delineate multiple functions
            signal = signal.replace("/", "_")

            if signal.startswith("ETH"):
                signal = eth_remap(signal)

            column = heading.index(re.search("(AF\d+)", af).group(1))
            peripheral = re.search("([A-Z1-9]*[A-Z]+)(\d*)(_|$)", signal)
            category_parts[column][peripheral[1]].add(peripheral[2])
            signal = signal.replace("SYS_", "")
            signal = signal.replace("-", "/")
            if row[column]:
                if signal not in row[column].split("/"):
                    # multiple af signals per pin
                    row[column] = "/".join((row[column], signal))
            else:
                row[column] = signal

        if pin in adc_pins:
            row[-1] = "/".join(("ADC{}_{}".format(i, ch) for ch, i in adc_pins[pin].items()))

        for i, val in enumerate(row):
            col_width[i] = max(col_width[i], len(val))

        rows.append(row)

        if (dual_pad := pin + "_C") in adc_pins:
            # Add extra row for dual-pad ADC entry.
            row_c = [""] * len(row)
            row_c[0] = row[0]
            row_c[1] = dual_pad
            row_c[-1] = "/".join(
                ("ADC{}_{}".format(i, ch) for ch, i in adc_pins[dual_pad].items())
            )
            rows.append(row_c)

    # Simplify the category sets, eg TIM1/TIM2 -> TIM1/2
    category_head = ["" for _ in range(len(heading))]
    for i, periph in enumerate(category_parts):
        periphs = []
        for key, vals in periph.items():
            if len(vals) > 1:
                if "" in vals:
                    vals.remove("")
                    vals.add("1")
                periphs.append(key + "/".join(sorted(vals, key=int)))
            else:
                periphs.append(key + vals.pop())

        category_head[i] = "/".join(sorted(periphs))
        col_width[i] = max(col_width[i], len(category_head[i]))
    category_head[-1] = "ADC"

    # Sort the rows by the first two columns; port and pin number
    rows.sort(key=lambda row: (row[0], pin_int(row[1])))

    output_file = Path(__file__).parent / ("%s_af.csv" % target.lower())
    with output_file.open("w") as out:
        print("# This was auto-generated by make-af.csv.py", file=out)
        padded_heading = [val.ljust(col_width[i]) for i, val in enumerate(heading)]
        print(",".join(padded_heading).strip(), file=out)
        categories = [val.ljust(col_width[i]) for i, val in enumerate(category_head)]
        print(",".join(categories).rstrip(), file=out)
        for row in rows:
            padded_row = [val.ljust(col_width[i]) for i, val in enumerate(row)]
            print(",".join(padded_row).strip(), file=out)

    print("Written:", output_file.resolve())


def pin_int(pname: str):
    # Takes a Pin (or AF) name like PA5, PD15 or AF4 and returns the integer component
    return int(re.search("[PA][A-Z](\d+)", pname).group(1))


def pin_name(pin: str):
    # Filters out just the pin name, eg PB12 from provided pin string
    return (re.search("P[A-Z]\d+", pin) or [None])[0]


def eth_remap(signal):
    # The AF signal names in stm xml's generally match the names used in datasheets,
    # except for ethernet RMII / MII ones...
    # This mapping was generated from comparing xml data to datasheet on STM32H573
    eth_signals = {
        "ETH_MDC": "ETH_MDC",
        "ETH_MDIO": "ETH_MDIO",
        "ETH_PPS_OUT": "ETH_PPS_OUT",
        "ETH_CRS": "ETH_MII_CRS",
        "ETH_COL": "ETH_MII_COL",
        "ETH_TX_ER": "ETH_MII_TX_ER",
        "ETH_RXD2": "ETH_MII_RXD2",
        "ETH_RXD3": "ETH_MII_RXD3",
        "ETH_TXD3": "ETH_MII_TXD3",
        "ETH_RX_ER": "ETH_MII_RX_ER",
        "ETH_TXD2": "ETH_MII_TXD2",
        "ETH_TX_CLK": "ETH_MII_TX_CLK",
        "ETH_RX_CLK": "ETH_MII_RX_CLK",
        "ETH_RX_DV": "ETH_MII_RX_DV",
        "ETH_REF_CLK": "ETH_RMII_REF_CLK",
        "ETH_CRS_DV": "ETH_RMII_CRS_DV",
        "ETH_TX_EN": "ETH_MII_TX_EN/ETH_RMII_TX_EN",
        "ETH_TXD0": "ETH_MII_TXD0/ETH_RMII_TXD0",
        "ETH_TXD1": "ETH_MII_TXD1/ETH_RMII_TXD1",
        "ETH_RXD0": "ETH_MII_RXD0/ETH_RMII_RXD0",
        "ETH_RXD1": "ETH_MII_RXD1/ETH_RMII_RXD1",
    }
    return eth_signals.get(signal, signal)


if __name__ == "__main__":
    for target in sys.argv[1:]:
        generate_af_csv(target)
