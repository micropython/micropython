from __future__ import print_function
import subprocess
import argparse
import os
import re
from itertools import chain

SIZE = 'arm-none-eabi-size'

def find_best_fit(it, capacity):
    items = sorted(it, key=lambda x: x[1])
    total = 0
    while True:
        will_fit = [o for o in items if total + o[1] <= capacity]
        if will_fit == []:
            break
        o = will_fit[-1]  # biggest fitting object
        total += o[1]
        items.remove(o)
        yield o


def get_sections_size(filename, regex):
    """return combined size of all sections matching <regex> from <filename>"""
    return sum(x[1] for x in get_sections(filename, regex))


def get_sections(filename, regex):
    """return all sections from <filename> with name matching <regex>"""
    output = subprocess.check_output([SIZE, '-A', filename])
    def parse_line(line):
        name, size_str = line.split()[:2]
        size = int(size_str)
        # align to a word size
        if size % 4:
            size += 4 - (size % 4)
        return name, size
    return (parse_line(line) for line in output.splitlines() if regex.match(line))


def main():
    cmd_parser = argparse.ArgumentParser(description="Find best matching sections that can fit into first FLASH sector.")
    cmd_parser.add_argument('-s', '--sector-size', type=int, action='store', help='size of FLASH sector')
    cmd_parser.add_argument('-d', '--discard', action='store', help='list of discarded sections')
    cmd_parser.add_argument('files', nargs="+", action='store', help='input files')
    args = cmd_parser.parse_args()

    try:
        startup = [f for f in args.files if f.endswith('startup_stm32.o')][0]
        capacity = args.sector_size - get_sections_size(startup, re.compile(r'\.isr_vector'))
    except IndexError:
        raise ValueError('section ".isr_vector" not found')

    # find all .text.* sections
    re_text = re.compile(r'\.text\.')
    objects = chain.from_iterable(get_sections(f, re_text) for f in args.files)

    # filter out sections discarded by linker
    if args.discard:
        with open(args.discard) as h:
            discarded = set(h.read().splitlines())
            objects = filter(lambda o: o[0] not in discarded, objects)

    # find which sections will best fit into 'capacity'
    # print linker script entries to stdout
    for obj in find_best_fit(objects, capacity):
        print('*(%s)  /* %d bytes */' % obj)


if __name__ == '__main__':
    main()
