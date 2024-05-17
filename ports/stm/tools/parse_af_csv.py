# This file is part of the CircuitPython project: https://circuitpython.org
#
# SPDX-FileCopyrightText: Copyright (c) 2020 Lucian Copeland for Adafruit Industries
#
# SPDX-License-Identifier: MIT

import csv
import sys

# Use: parse_af_csv.py Filename.csv -pins-only
# Designed for use with .csv files from Micropython, or in identical format
# created via Datasheet peripheral tables with a Sheets program.
#
# See examples/stm32f405.csv for example formatting.


# Most peripherals (SPI, I2C) output 3 values:
# peripheral index, alt function, pin string
def evaluate_periph(inper, inlist, periph, subtype, altfn, pin):
    # ex) SPI1_SCK,SPI3_SCK/I2S3_CK
    # Clean anything after a '\' due to SPI/I2S mixing
    if not inper.find("/") == -1:
        inper = inper[: inper.find("/")]

    if inper[: len(periph)] == periph and inper[-len(subtype) :] == subtype:
        inlist.append([inper[len(periph) : len(periph) + 1], altfn, pin])


# Timers (TIM) are a special case with 4 values
# timer index, alt function, channel, pin string
def evaluate_tim(inper, inlist, altfn, pin):
    # ex) TIM2_CH1/TIM2_ETR,  TIM5_CH1
    # Clean anything after a '\' to filter ETR
    if not inper.find("/") == -1:
        inper = inper[: inper.find("/")]

    if inper[:3] == "TIM" and inper[5:7] == "CH" and inper[-1:] != "N":
        inlist.append([inper[3:4], altfn, inper[-1:], pin])
    elif inper[:3] == "TIM" and inper[6:8] == "CH" and inper[-1:] != "N":
        inlist.append([inper[3:5], altfn, inper[-1:], pin])


# Open target file
with open(sys.argv[1]) as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=",")
    line_count = 0

    if sys.argv[2] != "-pins-only":
        # List of peripheral pin types to read
        todo = [
            ["I2C", "SDA"],
            ["I2C", "SCL"],
            ["SPI", "SCK"],
            ["SPI", "MOSI"],
            ["SPI", "MISO"],
            ["SPI", "NSS"],
            ["UART", "TX"],
            ["UART", "RX"],
        ]

        # Make a list of empty lists to populate
        outlist = []
        for items in todo:
            empty = []
            outlist.append(empty)
        # TIM
        empty = []
        outlist.append(empty)

        # Each line is a list of strings
        for row in csv_reader:
            altfn = 0
            pin = row[0]
            if len(pin) < 4:  # add additional leading 0 to pin number after port
                pin = pin[:2] + "0" + pin[2:]
            for col in row:
                array_index = 0
                # Evaluate the string for every possible todo entry
                for item in todo:
                    evaluate_periph(col, outlist[array_index], item[0], item[1], altfn - 1, pin)
                    # UART special case, run again for USART variant
                    if item[0] == "UART":
                        evaluate_periph(
                            col, outlist[array_index], "USART", item[1], altfn - 1, pin
                        )
                    array_index += 1
                # TIM special case
                evaluate_tim(col, outlist[-1], altfn - 1, pin)
                altfn += 1
            line_count += 1

        # Print formatted output
        for i in range(len(todo)):
            ins = (todo[i][0]).lower() + "_" + (todo[i][1]).lower() + "_"
            #      const mcu_i2c_sda_obj_t mcu_i2c_sda_list[4] = {
            print("const mcu_periph_obj_t mcu_" + ins + "list[" + str(len(outlist[i])) + "] = {")
            for row in outlist[i]:
                print("    PERIPH(" + row[0] + ", " + str(row[1]) + ", &pin_" + row[2] + "),")
            print("};")

        # Timer special case:
        print("const mcu_tim_pin_obj_t mcu_tim_pin_list[" + str(len(outlist[-1])) + "] = {")
        for row in outlist[-1]:
            print(
                "    TIM("
                + row[0]
                + ", "
                + str(row[1])
                + ", "
                + str(row[2])
                + ", &pin_"
                + row[3]
                + "),"
            )
        print("};")

    else:
        # Format all viable pins listed
        # ex) const mcu_pin_obj_t pin_PE02 = PIN(4, 2, NO_ADC);
        # ex 2) extern const mcu_pin_obj_t pin_PE02;
        # TODO: add ADC detection
        outlist = []

        for row in csv_reader:
            altfn = 0
            pin = row[0]
            if len(pin) < 4:
                pin = pin[:2] + "0" + pin[2:]
            outlist.append([pin, str(ord(pin[1:2]) - 65), pin[2:4]])
            line_count += 1

        for line in outlist:
            print(
                "const mcu_pin_obj_t pin_"
                + line[0]
                + " = PIN("
                + line[1]
                + ", "
                + line[2]
                + ", NO_ADC);"
            )

        for line in outlist:
            print("extern const mcu_pin_obj_t pin_" + line[0] + ";")

    print("Processed %d lines." % line_count)
