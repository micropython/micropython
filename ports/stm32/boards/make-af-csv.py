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

import argparse
import concurrent.futures
import json
import re
import urllib.error
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


def ensure_mcu_list():
    # Fetch the MCU file tree from GitHub (rate-limited, 2 API calls).
    # Must be called before generate_af_csv.
    global mcu_list_all
    if mcu_list_all is None:
        repo_list_all = json.loads(fetch_url(repo_url + "master"))["tree"]
        mcu_list_url = [entry for entry in repo_list_all if entry["path"] == "mcu"][0]["url"]
        mcu_list_all = json.loads(fetch_url(mcu_list_url + "?recursive=1"))["tree"]


def generate_af_csv(target: str, verbose: bool = False) -> None:
    """
    Generates an AF CSV file based on the given target.

    Parameters:
    - target (str): The mcu to generate the AF CSV for.
    - verbose (bool): Print diagnostic output during generation.

    """
    ensure_mcu_list()

    print("Generating AF file for:", target)
    cpu = target.rstrip("x").upper()

    # Get list of all mcu files matching the target
    mcu_list = [mcu for mcu in mcu_list_all if mcu.get("path").upper().startswith(cpu)]
    if not mcu_list:
        raise SystemExit('ERROR: Could not find mcu "{}"'.format(cpu))

    if verbose:
        print("  MCU XML candidates:")
        for m in sorted(mcu_list, key=lambda m: m.get("size", 0), reverse=True):
            print("    {} ({} bytes)".format(m["path"], m.get("size", "?")))

    # Download all matching MCU XMLs and union their pin sets.
    # Different package variants may have different pin subsets (e.g. BGA vs QFP),
    # so we union them to get the complete set for this MCU family.
    available_pins, available_pins_afs, adc_pins, gpio_version = collect_mcu_pins(
        mcu_list, verbose
    )

    if verbose:
        print("  Total pins across all variants: {}".format(len(available_pins)))

    # Find the GPIO definition file matching the chip.
    gpio_file_url = None
    if gpio_version:
        expected_path = "IP/GPIO-{}_Modes.xml".format(gpio_version)
        gpio_matches = [g for g in mcu_list_all if g["path"] == expected_path]
        if not gpio_matches:
            gpio_matches = [
                g
                for g in mcu_list_all
                if g["path"].startswith("IP/GPIO") and gpio_version in g["path"]
            ]
        if not gpio_matches:
            raise SystemExit(
                "ERROR: Could not find GPIO XML for version '{}'".format(gpio_version)
            )
        gpio_file = gpio_matches[0]
        gpio_file_url = xml_url + gpio_file["path"]
        if verbose:
            print("  GPIO version: {}".format(gpio_version))
            print("  GPIO XML: {}".format(gpio_file["path"]))

    if gpio_file_url is None:
        raise SystemExit("ERROR: Could not find GPIO details for {}".format(cpu))

    xml_data = fetch_url(gpio_file_url)
    print("Downloaded:", gpio_file_url)

    root = parse_stm_xml(xml_data)

    event_out_af15 = detect_eventout_af15(root)
    if verbose and event_out_af15:
        print("  EVENTOUT on AF15")

    mapping = build_af_mapping(root, cpu, target, available_pins_afs, event_out_af15, verbose)
    # Filter GPIO pins to only those present on this MCU package.
    # The GPIO XML is family-wide and includes pins from larger packages.
    if available_pins:
        gpio_all = set(mapping.keys())
        if verbose:
            filtered = sorted(gpio_all - available_pins, key=lambda p: (p[1], pin_int(p)))
            print("  Pins in MCU XML: {}".format(len(available_pins)))
            print("  Pins in GPIO XML: {}".format(len(gpio_all)))
            print("  Pins after filtering: {}".format(len(mapping)))
            if filtered:
                print("  Filtered out: {}".format(", ".join(filtered)))

    # AF0 through AF15 are somewhat standard across STM range,
    # but not all chips use all domains. List them all for consistency.
    af_domains = ["AF%s" % i for i in range(16)] + ["ADC"] + ["SYS"]

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
            signal = normalize_signal(signal)
            if af == "SYS":
                column = len(heading) - 1
                signal = signal.replace(
                    "RTC_AF1", "RTC_OUT_ALARM/RTC_OUT_CALIB/RTC_TAMP1/RTC_TS/RTC_WAKEUP/(RTC_AF1)"
                )
                signal = signal.replace("RTC_AF2", "RTC_TAMP1/RTC_TAMP2/RTC_TS/(RTC_AF2)")
            else:
                column = heading.index(re.search(r"(AF\d+)", af).group(1))
            peripheral = re.search(r"([A-Z1-9]*[A-Z]+)(\d*)(_|$)", signal)
            category_parts[column][peripheral[1]].add(peripheral[2])
            signal = signal.replace("SYS_", "")
            signal = signal.replace("-", "_")
            row[column] = merge_af_signal(row[column], signal)
            # Reorder to match old af.csv files.
            row[column] = reorder_signal(row[column], ("DAC",))
            row[column] = reorder_signal(row[column], ("RTS", "DE"))
            row[column] = reorder_signal(row[column], ("SPI", "I2S"))
            row[column] = reorder_signal(row[column], ("FSMC_NE", "FSMC_NC"))
            row[column] = reorder_signal(row[column], ("ETH_MII", "ETH_RMII"))
            row[column] = reorder_signal(row[column], ("TIM3", "TIM22"))  # for L072

        if pin in adc_pins:
            row[-2] = "/".join(("ADC{}_{}".format(i, ch) for ch, i in adc_pins[pin].items()))
            row[-2] = reorder_signal(row[-2], ("INP", "INN"))  # Reorder INP before INN

        for i, val in enumerate(row):
            col_width[i] = max(col_width[i], len(val))

        rows.append(row)

        if (dual_pad := pin + "_C") in adc_pins:
            # Add extra row for dual-pad ADC entry.
            row_c = [""] * len(row)
            row_c[0] = row[0]
            row_c[1] = dual_pad
            row_c[-2] = "/".join(
                ("ADC{}_{}".format(i, ch) for ch, i in adc_pins[dual_pad].items())
            )
            row_c[-2] = reorder_signal(row_c[-2], ("INP", "INN"))  # Reorder INP before INN
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
    category_head[-2] = "ADC"

    # Sort the rows by the first two columns; port and pin number
    rows.sort(key=lambda row: (row[0], pin_int(row[1])))

    output_file = Path(__file__).parent / ("%s_af.csv" % target.lower())
    with output_file.open("w") as out:
        print("# ", target.upper(), " This was auto-generated by make-af-csv.py", file=out)
        print(",".join(pad_row(heading, col_width)), file=out)
        print(",".join(pad_row(category_head, col_width)), file=out)
        for row in rows:
            print(",".join(pad_row(row, col_width)), file=out)

    print("Written:", output_file.resolve())

    if verbose:
        total_afs = sum(1 for row in rows for cell in row[2:] if cell)
        print("  Output: {} pins, {} AF entries".format(len(rows), total_afs))


def detect_eventout_af15(root):
    # Check if EVENTOUT is globally mapped to AF15 in this GPIO XML.
    for ref_mode in root.findall("./RefMode"):
        if ref_mode.get("Name") != "EVENTOUT":
            continue
        for param in ref_mode.findall("Parameter"):
            if param.get("Name") != "GPIO_AF":
                continue
            pv = param.find("./PossibleValue")
            if pv is not None and pv.text == "GPIO_AF15_EVENTOUT":
                return True
    return False


# System functions that belong in the SYS column rather than AF0.
SYS_FUNCTIONS = (
    "OSC",
    "RTC",
    "BOOT",
    "WKUP",
    "TAMP",
    "RCC",
    "NRST",
    "DAC",
    "VBUS",
    "LSCO",
    "SLEEP",
    "PWR",
)

# H7 power mode signals that stay in AF0 rather than SYS.
AF0_SYS_SIGNALS = (
    "PWR_CSLEEP",
    "PWR_CDSLEEP",
    "PWR_CSTOP",
    "PWR_NDSTOP1",
    "PWR_NDSTOP2",
    "PWR_D1PWREN",
    "PWR_D2PWREN",
)


def build_af_mapping(root, cpu, target, available_pins_afs, event_out_af15, verbose):
    # Build the AF mapping from GPIO XML, filtered by available pins/AFs.
    mapping = defaultdict(list)
    for pin in root.findall("./GPIO_Pin"):
        pin_full_name = pin.get("Name")
        pname = pin_name(pin_full_name)
        if pname is None or pname not in available_pins_afs:
            continue
        _ = mapping[pname]  # Ensure pin gets captured / listed
        if "BOOT0" in pin_full_name:
            mapping[pname].append(("BOOT0", "SYS"))
        if ("STM32L1" in cpu or "STM32F4" in cpu) and pname == "PB2":
            mapping[pname].append(("BOOT1", "SYS"))
        for pin_signal in pin.findall("./PinSignal"):
            for specific_param in pin_signal.findall("SpecificParameter"):
                if specific_param.get("Name") != "GPIO_AF":
                    continue
                signal_name = pin_signal.get("Name")
                if signal_name != "EVENTOUT" and signal_name not in available_pins_afs[pname]:
                    continue
                af_fn = specific_param.find("./PossibleValue").text
                af_fn = fix_st_af_map_errors(target, pname, signal_name, af_fn, verbose)
                mapping[pname].append((signal_name, af_fn))
        if "EVENTOUT" in available_pins_afs[pname] and event_out_af15:
            mapping[pname].append(("EVENTOUT", "GPIO_AF15_EVENTOUT"))

    # Add pins without alt functions and populate SYS column entries.
    for pin, af_list in available_pins_afs.items():
        _ = mapping[pin]
        for signal_name in af_list:
            if not any(sys_fn in signal_name for sys_fn in SYS_FUNCTIONS):
                continue
            if "DAC" in signal_name and "EXT" in signal_name:
                continue
            if any(signal_name in entry for entry in mapping[pin]):
                continue
            if pin == "PI8" and signal_name == "RTC_AF1":
                continue  # ST open pin data error on F479
            if signal_name in AF0_SYS_SIGNALS:
                mapping[pin].append((signal_name, "AF0_SYS"))
            else:
                mapping[pin].append((signal_name, "SYS"))

    return mapping


def collect_mcu_pins(mcu_list, verbose):
    # Download all matching MCU XMLs and union their pin sets and ADC data.
    available_pins = set()
    adc_pins = dict()
    available_pins_afs = defaultdict(list)
    gpio_version = None
    best_mcu_size = 0
    for mcu in mcu_list:
        mcu_url = xml_url + mcu["path"]
        mcuxmlstr = fetch_url(mcu_url)
        if verbose:
            print("  Downloaded:", mcu["path"])
        else:
            print("Downloaded:", mcu_url)

        root = parse_stm_xml(mcuxmlstr)
        # process the MCU XML file and determine which pins and alt functions are available.
        for pin_el in root.findall("./Pin"):
            pn = pin_name(pin_el.get("Name"))
            if pn is not None:
                available_pins.add(pn)
                if pin_el.get("Name").endswith("_C"):
                    available_pins.add(pn + "_C")

        for pin in root.findall("./Pin"):
            for sig in pin.findall("./Signal"):
                pname = pin_name(pin.get("Name"))
                sig_name = sig.get("Name")
                io_modes = sig.get("IOModes")
                if pname is not None:
                    if pin.get("Name").endswith("_C"):
                        if sig_name.startswith("ADC") and "_IN" in sig_name:
                            if sig_name not in available_pins_afs[pname + "_C"]:
                                available_pins_afs[pname + "_C"].append(
                                    sig_name
                                )  # Add pin and its
                        else:
                            _ = available_pins_afs[
                                pname + "_C"
                            ]  # Add analog pin without alt functions.
                    else:
                        if sig_name not in available_pins_afs[pname]:
                            available_pins_afs[pname].append(
                                sig_name
                            )  # Add pin and its alt functions.
                        if (
                            io_modes is not None
                            and "EVENTOUT" in io_modes
                            and "EVENTOUT" not in available_pins_afs[pname]
                        ):
                            available_pins_afs[pname].append("EVENTOUT")  # Add EVENTOUT.
                # Check if sig is and ADC pin and does not end with bank suffix (eg IN0b in STM32L1xx)
                if (
                    not sig_name
                    or not sig_name.startswith("ADC")
                    or "_IN" not in sig_name
                    or not sig_name[-1].isdigit()
                ):
                    continue
                sig_name = sig_name.replace("ADC_", "ADC1_")
                index, channel = sig_name.split("_")
                if pin.get("Name").endswith("_C"):
                    pname += "_C"
                n_index = re.search(r"\d+", index)[0]
                if pname in adc_pins:
                    if channel not in adc_pins[pname]:
                        adc_pins[pname][channel] = n_index
                    else:
                        adc_pins[pname][channel] = "".join(
                            sorted(set(adc_pins[pname][channel] + n_index))
                        )
                else:
                    adc_pins[pname] = {channel: n_index}
        # Pick GPIO version from the largest MCU XML (most peripherals/flash).
        mcu_size = mcu.get("size", 0)
        for detail in root.findall("./IP[@Name='GPIO']"):
            mcu_gpio_version = detail.get("Version")
            if gpio_version is None or mcu_size > best_mcu_size:
                gpio_version = mcu_gpio_version
                best_mcu_size = mcu_size
    return available_pins, available_pins_afs, adc_pins, gpio_version


def normalize_signal(signal):
    # Normalize signal names from STM XML to match make-pins.py expectations.
    signal = signal.replace("RCC_", "")
    signal = signal.replace("CAN_", "CAN1_")
    signal = signal.replace("FMPI2C", "I2CFMP")
    signal = signal.replace("LTDC", "LCD")
    signal = signal.replace("CM4-EVENTOUT", "EVENTOUT")
    signal = signal.replace("UCPD1_FRSTX1", "UCPD1_FRSTX")
    signal = signal.replace("UCPD2_FRSTX1", "UCPD2_FRSTX")
    signal = signal.replace("UCPD1_FRSTX2", "UCPD1_FRSTX")  # Duplicates are removed.
    signal = signal.replace("UCPD2_FRSTX2", "UCPD2_FRSTX")
    if "I2S_CKIN" not in signal:
        signal = signal.replace("I2S_", "I2S1_")
    if (
        "BK" in signal and "BKIN" not in signal and "SPI" not in signal
    ):  # Exclude QUADSPIx_BKn, OCTOSPIx_BKn
        signal = signal.replace("BK", "BKIN")
    # STM XML uses I2S2_ext_SD but make-pins.py expects I2S2ext_SD.
    signal = re.sub(r"(I2S\d+)_ext_", r"\1ext_", signal)
    # / is used later to delineate multiple functions in a cell.
    signal = signal.replace("/", "_")
    if signal.startswith("ETH"):
        signal = eth_remap(signal)
    return signal


def merge_af_signal(cell, signal):
    # Merge a signal into an AF cell, combining TIMx_CHn + TIMx_ETR into
    # TIMx_CHn_ETR to match make-pins.py's SUPPORTED_AF. See PR #12272.
    if not cell:
        return signal
    existing_parts = cell.split("/")
    if signal in existing_parts:
        return cell
    # Check if signal is subsumed by an existing TIMx_CHn_ETR entry.
    m_new = re.match(r"(TIM\d+)_(CH\d+)$", signal)
    m_etr = re.match(r"(TIM\d+)_ETR$", signal)
    if m_new and any(
        p == "{}_{}_ETR".format(m_new.group(1), m_new.group(2)) for p in existing_parts
    ):
        return cell
    if m_etr and any(
        re.match(r"{}_CH\d+_ETR$".format(re.escape(m_etr.group(1))), p) for p in existing_parts
    ):
        return cell
    # Try to combine CHn + ETR into CHn_ETR.
    m_old_ch = re.match(r"(TIM\d+)_(CH\d+)$", cell)
    m_old_etr = re.match(r"(TIM\d+)_ETR$", cell)
    if m_new and m_old_etr and m_new.group(1) == m_old_etr.group(1):
        return "{}_{}_ETR".format(m_new.group(1), m_new.group(2))
    if m_etr and m_old_ch and m_etr.group(1) == m_old_ch.group(1):
        return "{}_{}_ETR".format(m_old_ch.group(1), m_old_ch.group(2))
    return "/".join((cell, signal))


# Reorder the signals containing specified substrings first, then other signals.
# Substrings not found are ignored.
def reorder_signal(signal, sig_list: tuple):
    new_sigs = []
    sigs = signal.split("/")
    for ordered_sig in sig_list:  # Add the listed sigs first.
        idx = next((i for i, s in enumerate(sigs) if ordered_sig in s), -1)
        if idx > -1:
            new_sigs.append(sigs[idx])
    for sig in sigs:  # Add the other sigs.
        if sig not in new_sigs:
            new_sigs.append(sig)
    return "/".join(new_sigs)


def fix_st_af_map_errors(target, pname, signal_name, af_fn, verbose):
    # Fix errors in open pin data where wrong AF number is assigned to signal and
    #   remove SYS functions (OSC, etc) from AF0 and assign to "SYS".
    # List format"
    # ((list of mcus | "*", for all mcus), pin | "*" for all pins", signal, new AF, or "SYS")
    fixes = (
        (("*",), "*", "WKUP", "SYS"),
        (("*",), "*", "RTC_AF", "SYS"),
        (("*",), "*", "RTC_OUT_ALARM", "SYS"),
        (("*",), "*", "RTC_OUT_CALIB", "SYS"),
        (("*",), "*", "RTC_TAMP", "SYS"),
        (("*",), "*", "RTC_TS", "SYS"),
        (("*",), "*", "RCC_OSC_IN", "SYS"),
        (("*",), "*", "RCC_OSC_OUT", "SYS"),
        (("*",), "*", "RCC_OSC32_IN", "SYS"),
        (("*",), "*", "RCC_OSC32_OUT", "SYS"),
        (("stm32f413", "stm32f423"), "PB4", "I2S3_ext_SD", "GPIO_AF7_SPI3"),
        (("stm32f413", "stm32f423"), "PB12", "I2S3_CK", "GPIO_AF7_SPI3"),
    )
    for fix in fixes:
        if (
            (target.lower() in fix[0] or fix[0][0] == "*")
            and (pname == fix[1] or fix[1] == "*")
            and fix[2] in signal_name
        ):
            af_fn = fix[3]
            if verbose:
                print("FIXED", fix)
    return af_fn


def fetch_url(url):
    # Download a URL, raising SystemExit with a clear message on failure.
    try:
        with urllib.request.urlopen(url) as response:
            return response.read()
    except urllib.error.HTTPError as e:
        if e.code == 403:
            raise SystemExit(
                "ERROR: GitHub API rate limit exceeded. Wait and retry.\n  URL: {}".format(url)
            ) from None
        raise SystemExit("ERROR: HTTP {} fetching {}".format(e.code, url)) from None
    except urllib.error.URLError as e:
        raise SystemExit("ERROR: Network error fetching {}: {}".format(url, e)) from None


def parse_stm_xml(xml_bytes):
    # The STM xml files have a dummy namespace declared which confuses ElementTree.
    # Strip namespace prefixes from element tags during parsing.
    it = ET.iterparse(BytesIO(xml_bytes))
    for _, el in it:
        _, _, el.tag = el.tag.rpartition("}")
    return it.root


def pad_row(row, col_width):
    # Pad all columns except the last to avoid trailing whitespace.
    last = len(row) - 1
    return [val if i == last else val.ljust(col_width[i]) for i, val in enumerate(row)]


def pin_int(pname: str):
    # Takes a Pin name like PA5, PD15 or AF name like AF4 and returns the integer component.
    # Matches "P" + port letter + digits, or "AF" + digits.
    return int(re.search(r"(?:P[A-O]|AF)(\d+)", pname).group(1))


def pin_name(pin: str):
    # Filters out just the pin name, eg PB12 from provided pin string.
    # Only accepts ports A-O, matching make-pins.py's P[A-O] validation.
    return (re.search(r"P[A-O]\d+", pin) or [None])[0]


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


def find_all_targets():
    # Discover targets from existing *_af.csv files in the same directory.
    boards_dir = Path(__file__).parent
    return sorted(p.stem.removesuffix("_af") for p in boards_dir.glob("*_af.csv"))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Generate STM32 alternate-function CSV files from STM32_open_pin_data."
    )
    parser.add_argument("targets", nargs="*", help="MCU target names, e.g. stm32wb55")
    parser.add_argument(
        "--all", action="store_true", help="Regenerate all existing *_af.csv files"
    )
    parser.add_argument("-v", "--verbose", action="store_true", help="Show diagnostic output")
    parser.add_argument(
        "-j",
        "--jobs",
        type=int,
        default=4,
        help="Number of parallel downloads (default: 4)",
    )
    args = parser.parse_args()

    if args.all:
        targets = find_all_targets()
        if not targets:
            raise SystemExit("ERROR: No existing *_af.csv files found")
        print("Regenerating {} targets: {}".format(len(targets), " ".join(targets)))
    elif args.targets:
        targets = args.targets
    else:
        parser.error("specify target names or use --all")

    # Fetch the tree index once before parallelising (rate-limited API calls).
    ensure_mcu_list()

    if args.jobs > 1 and len(targets) > 1:
        with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
            futures = {pool.submit(generate_af_csv, t, args.verbose): t for t in targets}
            for future in concurrent.futures.as_completed(futures):
                target = futures[future]
                try:
                    future.result()
                except Exception as e:
                    print("ERROR generating {}: {}".format(target, e))
    else:
        for target in targets:
            generate_af_csv(target, verbose=args.verbose)
