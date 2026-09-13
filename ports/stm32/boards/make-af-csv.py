#!/usr/bin/env python
"""
Generates MCU alternate-function (AF) definition files for stm32 parts
using the official sources from:
https://github.com/STMicroelectronics/STM32_open_pin_data

Usage:
$ python3 make-af-csv.py <mcu_name>

Example:
$ python3 make-af-csv.py stm32wb55

Data flow
---------
The STM32_open_pin_data repo contains two types of XML files:

  MCU XMLs (mcu/STM32*.xml):
    One file per package variant (e.g. STM32F405RGTx, STM32F405VGTx).
    Each lists <Pin> elements for the pins available on that package, with
    <Signal> children naming every function the pin supports. These carry
    no AF number information but do carry ADC channel assignments and an
    IOModes attribute on the GPIO signal that indicates EVENTOUT capability.
    Each MCU XML also references a GPIO version string via
    <IP Name="GPIO" Version="...">.

  GPIO XMLs (mcu/IP/GPIO-*_Modes.xml):
    One file per GPIO IP version, shared across a chip family (e.g. F405,
    F407 and F417 all share one GPIO XML). Each <GPIO_Pin> has <PinSignal>
    children with <SpecificParameter Name="GPIO_AF"> holding the AF number
    as a string like "GPIO_AF1_TIM2". This is where the actual AF mapping
    lives, but it covers the full family, including pins and peripherals
    that don't exist on smaller chips.

The script unions pins across all MCU XMLs matching a target (to capture
every package variant), then filters the family-wide GPIO XML against that
pin/AF set to produce a per-chip CSV.

Output CSV format
-----------------
Consumed by make-pins.py (via boardgen.py parse_af_csv) with header_rows=2,
pin_col=1, af_col=2. Column layout:

  Port, Pin, AF0..AF15, ADC, SYS

make-pins.py validates AF0-AF15 headings and the ADC column, but silently
ignores any columns beyond ADC (af_idx > 16), so the SYS column is purely
for human reference. The second header row (peripheral category summary) is
consumed but discarded by make-pins.py. Signals within a cell are
/-delimited.

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


# GitHub Trees API endpoint for listing repository contents.
# Rate-limited to 60 requests/hour without authentication; we make exactly 2
# calls (repo tree + mcu subtree) per invocation via ensure_mcu_list().
repo_url = "https://api.github.com/repos/STMicroelectronics/STM32_open_pin_data/git/trees/"

# raw.githubusercontent.com has no rate limit and is used for all XML downloads.
xml_url = "https://raw.githubusercontent.com/STMicroelectronics/STM32_open_pin_data/master/mcu/"


# Cached GitHub tree listing. Populated once by ensure_mcu_list(), then
# shared across parallel generate_af_csv() calls.
mcu_list_all = None


def ensure_mcu_list():
    """Fetch the mcu/ file tree from GitHub (2 rate-limited API calls).

    Must be called before generate_af_csv(). The result is cached in
    mcu_list_all so parallel workers don't repeat the API calls.
    """
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
    # Strip trailing "x" wildcard so "stm32f405" matches STM32F405RGTx etc.
    cpu = target.rstrip("x").upper()

    # Each target matches multiple MCU XMLs (one per package variant).
    # e.g. "stm32f405" matches STM32F405RGTx, STM32F405VGTx, STM32F405ZGTx.
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

    # Look up the GPIO XML matching the version string from the MCU XML.
    # The MCU XML's <IP Name="GPIO" Version="STM32F417_gpio_v1_0"> maps to
    # the file "IP/GPIO-STM32F417_gpio_v1_0_Modes.xml". Try exact path first,
    # then substring match as fallback for naming variations.
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
    # The GPIO XML is family-wide: e.g. the F405/F407/F417 share one GPIO XML
    # that includes pins for the 176-pin F407 that don't exist on the 64-pin
    # F405. build_af_mapping already filters by available_pins_afs, but log
    # the filtering stats in verbose mode.
    if available_pins:
        gpio_all = set(mapping.keys())
        if verbose:
            filtered = sorted(gpio_all - available_pins, key=lambda p: (p[1], pin_int(p)))
            print("  Pins in MCU XML: {}".format(len(available_pins)))
            print("  Pins in GPIO XML: {}".format(len(gpio_all)))
            print("  Pins after filtering: {}".format(len(mapping)))
            if filtered:
                print("  Filtered out: {}".format(", ".join(filtered)))

    format_af_csv(target, mapping, adc_pins, verbose)


def format_af_csv(target, mapping, adc_pins, verbose=False):
    """Format AF mapping and ADC data into a padded CSV file.

    Applies signal normalization, builds the category summary row, and
    writes the output file. The mapping dict comes from build_af_mapping;
    adc_pins comes from collect_mcu_pins.
    """
    af_domains = ["AF%s" % i for i in range(16)] + ["ADC"] + ["SYS"]

    # CSV structure: row 1 = column headings (Port, Pin, AF0..AF15, ADC, SYS),
    # row 2 = peripheral category summary (e.g. "TIM1/2/SPI1/3"), row 3+ = pin data.
    heading = ["Port", "Pin"] + af_domains
    col_width = [4] * len(heading)
    # Accumulate peripheral names per column for the category summary row.
    category_parts = [defaultdict(set) for _ in range(len(heading))]
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
                # Expand opaque RTC_AFn mux names to the functions they gate.
                signal = signal.replace(
                    "RTC_AF1", "RTC_OUT_ALARM/RTC_OUT_CALIB/RTC_TAMP1/RTC_TS/RTC_WAKEUP/(RTC_AF1)"
                )
                signal = signal.replace("RTC_AF2", "RTC_TAMP1/RTC_TAMP2/RTC_TS/(RTC_AF2)")
            else:
                # Extract AF number from strings like "GPIO_AF1_TIM2".
                column = heading.index(re.search(r"(AF\d+)", af).group(1))
            # Build the category summary, e.g. "TIM" + {"1","2"} -> "TIM1/2".
            peripheral = re.search(r"([A-Z1-9]*[A-Z]+)(\d*)(_|$)", signal)
            category_parts[column][peripheral[1]].add(peripheral[2])
            # SYS_ prefix (from e.g. SYS_WKUP1) is internal; strip for output.
            signal = signal.replace("SYS_", "")
            signal = signal.replace("-", "_")
            row[column] = merge_af_signal(row[column], signal)
            # Signal ordering within cells matches existing hand-crafted CSVs.
            row[column] = reorder_signal(row[column], ("DAC",))
            row[column] = reorder_signal(row[column], ("RTS", "DE"))
            row[column] = reorder_signal(row[column], ("SPI", "I2S"))
            row[column] = reorder_signal(row[column], ("FSMC_NE", "FSMC_NC"))
            row[column] = reorder_signal(row[column], ("ETH_MII", "ETH_RMII"))
            row[column] = reorder_signal(row[column], ("TIM3", "TIM22"))  # L072

        if pin in adc_pins:
            # Format: ADC<units>_<channel>, e.g. "ADC12_IN5" or "ADC3_INP0".
            # make-pins.py parses multi-unit indices as individual digits.
            row[-2] = "/".join(("ADC{}_{}".format(i, ch) for ch, i in adc_pins[pin].items()))
            row[-2] = reorder_signal(row[-2], ("INP", "INN"))

        for i, val in enumerate(row):
            col_width[i] = max(col_width[i], len(val))

        rows.append(row)

        if (dual_pad := pin + "_C") in adc_pins:
            # H7 dual-pad pins (PA0_C, PA1_C, PC2_C, PC3_C) have a dedicated
            # analog connection that bypasses the GPIO pad. They get their own
            # CSV row with only the ADC column populated.
            row_c = [""] * len(row)
            row_c[0] = row[0]
            row_c[1] = dual_pad
            row_c[-2] = "/".join(
                ("ADC{}_{}".format(i, ch) for ch, i in adc_pins[dual_pad].items())
            )
            row_c[-2] = reorder_signal(row_c[-2], ("INP", "INN"))
            rows.append(row_c)

    # Collapse e.g. {"TIM": {"1","2","8"}} into "TIM1/2/8" for the summary row.
    # A bare peripheral (no unit number) gets unit "1" when combined with others.
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
    """Check if EVENTOUT is globally mapped to AF15 in this GPIO XML.

    Some GPIO XMLs list EVENTOUT as per-pin <PinSignal> entries, others don't.
    The reliable source is the <RefMode Name="EVENTOUT"> element whose
    <Parameter Name="GPIO_AF"> child holds the AF assignment. We use this
    global declaration combined with the per-pin IOModes attribute from the
    MCU XML (collected in collect_mcu_pins) to determine which pins support
    EVENTOUT. Any per-pin EVENTOUT PinSignal entries in the GPIO XML are
    ignored by build_af_mapping's signal filter.
    """
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


# Substrings identifying system/non-AF signals in the MCU XML that should be
# placed in the SYS column rather than AF0. The ST GPIO XML puts these under
# AF0 (SYS_WKUP, RCC_OSC_IN, etc.) but they aren't alternate functions in the
# GPIO mux sense; they're fixed-function pin assignments. The SYS column
# separates them for clarity (make-pins.py ignores the SYS column).
SYS_FUNCTIONS = (
    "OSC",
    "RTC",
    "BOOT",
    "WKUP",
    "TAMP",
    "RCC",
    "NRST",
    "DAC",
    "USB",
    "LSCO",
    "SLEEP",
    "PWR",
    "OPAMP",
    "COMP",
)

# Exception: H7 power domain signals that appear in AF0 in the MCU XML
# (not the GPIO XML) and are genuine AF0 mux selections, not fixed-function
# system pins. These stay in AF0 instead of being moved to SYS.
AF0_SYS_SIGNALS = (
    "PWR_CSLEEP",
    "PWR_CDSLEEP",
    "PWR_CSTOP",
    "PWR_NDSTOP1",
    "PWR_NDSTOP2",
    "PWR_D1PWREN",
    "PWR_D2PWREN",
)

# Signal name replacements applied during CSV formatting to match make-pins.py
# and legacy hand-crafted CSVs. Each entry is (old, new) or (old, new, excludes)
# where excludes is a tuple of substrings that skip the replacement when present.
# Applied in order. The "/" -> "_" entry converts literal "/" in XML signal
# names (e.g. "SPI3/I2S3") before they become ambiguous with cell delimiters.
# eth_remap runs after SIGNAL_RENAMES, so its "/" output is preserved.
#
# make-pins.py parses signal names with the regex:
#     ([A-Z0-9]+[A-Z])(([0-9]+)(ext)?)?(_(.*))?
SIGNAL_RENAMES = (
    ("RCC_", ""),  # MCO signals: "RCC_MCO1" -> "MCO1"
    ("CAN_", "CAN1_"),  # Single-CAN chips: bare "CAN_TX" -> "CAN1_TX"
    ("FMPI2C", "I2CFMP"),  # F410/F413/F423 fast-mode plus I2C
    ("LTDC", "LCD"),  # LCD TFT display controller
    ("CM4-EVENTOUT", "EVENTOUT"),  # H7 dual-core CM4 variant
    ("UCPD1_FRSTX1", "UCPD1_FRSTX"),  # UCPD: collapse FRSTX1/FRSTX2
    ("UCPD2_FRSTX1", "UCPD2_FRSTX"),
    ("UCPD1_FRSTX2", "UCPD1_FRSTX"),
    ("UCPD2_FRSTX2", "UCPD2_FRSTX"),
    ("I2S_", "I2S1_", ("I2S_CKIN",)),  # Single-I2S: not I2S_CKIN (global clock)
    ("BK", "BKIN", ("BKIN", "SPI")),  # G0B1 TIM break: not QUADSPI/OCTOSPI bank
    ("/", "_"),  # Cell delimiter; literal "/" in names becomes "_"
)

# Regex-based signal renames applied after SIGNAL_RENAMES.
SIGNAL_REGEX_RENAMES = (
    # I2S ext: "I2S2_ext_SD" -> "I2S2ext_SD". make-pins.py regex captures
    # "ext" only immediately after unit digits; underscore-separated form
    # would parse as af_pin="ext_SD" which fails SUPPORTED_AF["I2S"] lookup.
    (r"(I2S\d+)_ext_", r"\1ext_"),
)

# AF corrections applied during GPIO XML processing.
# Each tuple: (mcu_list, pin, signal_substring, corrected_af).
# "*" matches all MCUs or all pins. Two categories:
# 1. System signals reclassified from AF0 to "SYS" column.
# 2. Genuine AF number errors verified against datasheets.
AF_CORRECTIONS = (
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
    # F413/F423: ST's XML assigns I2S3_ext_SD (PB4) and I2S3_CK (PB12)
    # to AF5, but DS11581 Table 12 shows both at AF7. The XML error
    # likely copies the AF5 value from PC11 (where it's correct) onto
    # PB4 (where AF5 is SPI1_MISO). The F412 XML and F405 XML both
    # have AF7 for I2S3_ext_SD on PB4, consistent with the datasheet.
    (("stm32f413", "stm32f423"), "PB4", "I2S3_ext_SD", "GPIO_AF7_SPI3"),
    (("stm32f413", "stm32f423"), "PB12", "I2S3_CK", "GPIO_AF7_SPI3"),
)


def build_af_mapping(root, cpu, target, available_pins_afs, event_out_af15, verbose):
    """Build per-pin AF mapping from GPIO XML, filtered by available_pins_afs.

    Returns dict of {pin_name: [(signal, af_string), ...]}.

    The GPIO XML's <GPIO_Pin> elements list every pin in the chip family with
    their <PinSignal> AF assignments. We filter each signal against
    available_pins_afs (built from MCU XMLs) to exclude peripherals not
    present on the specific MCU (e.g. ETH signals on F405 which has no
    ethernet, or LTDC signals on F427 which has no LCD controller).
    """
    mapping = defaultdict(list)
    for pin in root.findall("./GPIO_Pin"):
        pin_full_name = pin.get("Name")
        pname = pin_name(pin_full_name)
        if pname is None or pname not in available_pins_afs:
            continue
        _ = mapping[pname]  # Ensure GPIO-only pins appear in output.
        # BOOT1 on PB2 is not in any XML; it's a fixed function on L1/F4.
        if ("STM32L1" in cpu or "STM32F4" in cpu) and pname == "PB2":
            mapping[pname].append(("BOOT1", "SYS"))
        # Walk <PinSignal> -> <SpecificParameter Name="GPIO_AF"> -> <PossibleValue>
        # to extract AF assignments like "GPIO_AF1_TIM2".
        for pin_signal in pin.findall("./PinSignal"):
            for specific_param in pin_signal.findall("SpecificParameter"):
                if specific_param.get("Name") != "GPIO_AF":
                    continue
                signal_name = pin_signal.get("Name")
                # Filter against MCU-specific AF list (EVENTOUT is exempt
                # since it's declared globally, not per-pin in the GPIO XML).
                if signal_name != "EVENTOUT" and signal_name not in available_pins_afs[pname]:
                    continue
                af_fn = specific_param.find("./PossibleValue").text
                af_fn = fix_st_af_map_errors(target, pname, signal_name, af_fn, verbose)
                mapping[pname].append((signal_name, af_fn))
        # EVENTOUT is injected from RefMode + IOModes rather than from any
        # per-pin PinSignal entries (which some GPIO XMLs have, some don't).
        if "EVENTOUT" in available_pins_afs[pname] and event_out_af15:
            mapping[pname].append(("EVENTOUT", "GPIO_AF15_EVENTOUT"))

    # Populate SYS column from MCU XML signals that match SYS_FUNCTIONS.
    # This also ensures GPIO-only pins (e.g. PB2, oscillator pins) appear
    # in the output even if they have no AF entries from the GPIO XML.
    for pin, af_list in available_pins_afs.items():
        _ = mapping[pin]  # Ensure pin exists in mapping.
        for signal_name in af_list:
            if not any(sys_fn in signal_name for sys_fn in SYS_FUNCTIONS):
                continue
            # DAC_EXTI9, DAC1_EXTI9 etc are EXTI trigger inputs, not SYS.
            if "DAC" in signal_name and "EXT" in signal_name:
                continue
            # Skip if already added (e.g. from the GPIO XML AF0 processing).
            if any(signal_name == entry[0] for entry in mapping[pin]):
                continue
            # ST's F479 XML incorrectly lists RTC_AF1 on PI8.
            if pin == "PI8" and signal_name == "RTC_AF1":
                continue
            if signal_name in AF0_SYS_SIGNALS:
                mapping[pin].append((signal_name, "AF0_SYS"))
            else:
                mapping[pin].append((signal_name, "SYS"))

    return mapping


def collect_pin_names(root):
    """Extract pin names from a single MCU XML root.

    Returns a set of pin names (e.g. {"PA0", "PB12"}) including _C variants
    for H7 dual-pad pins. MCU XML <Pin> Names include suffixes like
    "PA0-WKUP" or "PH0-OSC_IN"; pin_name() extracts just "PA0"/"PH0".
    """
    pins = set()
    for pin_el in root.findall("./Pin"):
        pn = pin_name(pin_el.get("Name"))
        if pn is not None:
            pins.add(pn)
            # H7 dual-pad pins have names ending in "_C" in the XML.
            if pin_el.get("Name").endswith("_C"):
                pins.add(pn + "_C")
    return pins


def collect_pin_signals(root):
    """Extract per-pin signal lists from a single MCU XML root.

    Returns a defaultdict(list) of {pin_name: [signal_names]}, including
    EVENTOUT detection from the IOModes attribute on the GPIO signal.
    For _C pins, only ADC channel signals are captured; the AF signals
    are on the regular (non-_C) pad.
    """
    result = defaultdict(list)
    for pin in root.findall("./Pin"):
        pname = pin_name(pin.get("Name"))
        if pname is None:
            continue
        pin_full_name = pin.get("Name")
        # BOOT0 and NRST appear in pin names (e.g. "PA14-BOOT0",
        # "PF2-NRST") but not as <Signal> children. Inject them so they
        # reach the SYS column via SYS_FUNCTIONS.
        if "BOOT0" in pin_full_name and "BOOT0" not in result[pname]:
            result[pname].append("BOOT0")
        if "NRST" in pin_full_name and "NRST" not in result[pname]:
            result[pname].append("NRST")
        for sig in pin.findall("./Signal"):
            sig_name = sig.get("Name")
            io_modes = sig.get("IOModes")
            if pin.get("Name").endswith("_C"):
                if sig_name.startswith("ADC") and "_IN" in sig_name:
                    if sig_name not in result[pname + "_C"]:
                        result[pname + "_C"].append(sig_name)
                else:
                    _ = result[pname + "_C"]
            else:
                if sig_name not in result[pname]:
                    result[pname].append(sig_name)
                # EVENTOUT capability is indicated by the IOModes attribute
                # on the GPIO signal, e.g.
                # IOModes="Input,Output,Analog,EVENTOUT,EXTI".
                if (
                    io_modes is not None
                    and "EVENTOUT" in io_modes
                    and "EVENTOUT" not in result[pname]
                ):
                    result[pname].append("EVENTOUT")
    return result


def collect_adc_channels(root):
    """Extract ADC channel assignments from a single MCU XML root.

    Returns a dict of {pin_name: {channel: adc_units}}, e.g.
    {"PA0": {"IN0": "12"}, "PA0_C": {"INP0": "3"}}.
    Filters out non-channel signals (ADC1_EXTI15) and L1xx bank suffixes
    (IN0b). L0's "ADC_IN10" is normalized to "ADC1_IN10".
    """
    adc_pins = {}
    for pin in root.findall("./Pin"):
        for sig in pin.findall("./Signal"):
            pname = pin_name(pin.get("Name"))
            if pname is None:
                continue
            sig_name = sig.get("Name")
            if (
                not sig_name
                or not sig_name.startswith("ADC")
                or "_IN" not in sig_name
                or not sig_name[-1].isdigit()
            ):
                continue
            # L0 uses "ADC_IN10" (no instance number); normalize to "ADC1_".
            sig_name = sig_name.replace("ADC_", "ADC1_")
            index, channel = sig_name.split("_")
            if pin.get("Name").endswith("_C"):
                pname += "_C"
            # Merge ADC instances sharing a channel, e.g. ADC1_IN5 and
            # ADC2_IN5 on the same pin become {"IN5": "12"}.
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
    return adc_pins


def collect_mcu_pins(mcu_list, verbose):
    """Download all MCU XMLs for a target and union their pin/AF/ADC data.

    Different package variants of the same chip have different pin subsets
    (e.g. LQFP64 vs LQFP100 vs UFBGA176). We download every matching MCU
    XML and union the results so the CSV covers all package variants.

    Returns:
        available_pins: set of pin names (e.g. {"PA0", "PB12", ...})
        available_pins_afs: dict of {pin: [signal_names]} from MCU XMLs,
            used to filter the family-wide GPIO XML to this specific MCU.
        adc_pins: dict of {pin: {channel: adc_units}} for ADC column.
        gpio_version: GPIO IP version string from the largest MCU XML,
            used to select which GPIO XML to download.
    """
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

        available_pins |= collect_pin_names(root)

        for pin, signals in collect_pin_signals(root).items():
            for sig in signals:
                if sig not in available_pins_afs[pin]:
                    available_pins_afs[pin].append(sig)

        for pin, channels in collect_adc_channels(root).items():
            if pin not in adc_pins:
                adc_pins[pin] = dict(channels)
            else:
                for channel, units in channels.items():
                    if channel not in adc_pins[pin]:
                        adc_pins[pin][channel] = units
                    else:
                        adc_pins[pin][channel] = "".join(
                            sorted(set(adc_pins[pin][channel] + units))
                        )

        # Pick GPIO version from the largest MCU XML by file size. Larger
        # MCU XMLs correspond to higher flash/RAM variants which typically
        # have more peripherals and thus reference a more complete GPIO XML.
        mcu_size = mcu.get("size", 0)
        for detail in root.findall("./IP[@Name='GPIO']"):
            mcu_gpio_version = detail.get("Version")
            if gpio_version is None or mcu_size > best_mcu_size:
                gpio_version = mcu_gpio_version
                best_mcu_size = mcu_size
    return available_pins, available_pins_afs, adc_pins, gpio_version


def normalize_signal(signal):
    """Normalize signal names using SIGNAL_RENAMES and SIGNAL_REGEX_RENAMES."""
    for entry in SIGNAL_RENAMES:
        old, new = entry[0], entry[1]
        excludes = entry[2] if len(entry) > 2 else ()
        if not any(exc in signal for exc in excludes):
            signal = signal.replace(old, new)
    for pattern, replacement in SIGNAL_REGEX_RENAMES:
        signal = re.sub(pattern, replacement, signal)
    if signal.startswith("ETH"):
        signal = eth_remap(signal)
    return signal


def merge_af_signal(cell, signal):
    """Merge a signal into a /-delimited AF cell string, skipping duplicates.

    A pin can have several distinct functions on one AF number, e.g. AF1 on
    PA0 of the F405 carries both TIM2_CH1 and TIM2_ETR. Each stays a separate
    /-delimited entry so make-pins.py resolves them as individual pin
    functions.
    """
    if not cell:
        return signal
    if signal in cell.split("/"):
        return cell
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
    """Apply AF_CORRECTIONS to fix ST open pin data errors and reclassify
    system signals from AF0 to the SYS column."""
    for fix in AF_CORRECTIONS:
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
    # All STM XMLs declare xmlns="http://dummy.com", which causes ElementTree
    # to prefix every tag with "{http://dummy.com}". Strip it so findall()
    # can use bare tag names like "GPIO_Pin" instead of "{http://...}GPIO_Pin".
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
    return int(re.search(r"(?:P[A-Q]|AF)(\d+)", pname).group(1))


def pin_name(pin: str):
    # Extract pin name (e.g. "PB12") from XML strings like "PB12", "PA0-WKUP",
    # "PH0-OSC_IN". Returns None for power/reset pins (VBAT, NRST, BOOT0)
    # which don't match P[A-Q]. Port range A-Q matches make-pins.py's
    # validate_cpu_pin_name regex: P[A-Q][0-9]+(_C)?$
    return (re.search(r"P[A-Q]\d+", pin) or [None])[0]


def eth_remap(signal):
    # STM XML uses generic ETH signal names (ETH_TXD0, ETH_RX_DV) but
    # datasheets and legacy CSVs distinguish MII vs RMII variants. Signals
    # shared between MII and RMII get both forms joined with "/".
    # Mapping derived from STM32H573 datasheet Table 10.
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

    failed = []
    if args.jobs > 1 and len(targets) > 1:
        with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
            futures = {pool.submit(generate_af_csv, t, args.verbose): t for t in targets}
            for future in concurrent.futures.as_completed(futures):
                target = futures[future]
                try:
                    future.result()
                except Exception as e:
                    print("ERROR generating {}: {}".format(target, e))
                    failed.append(target)
    else:
        for target in targets:
            generate_af_csv(target, verbose=args.verbose)

    if failed:
        raise SystemExit("ERROR: failed to generate: {}".format(" ".join(sorted(failed))))
