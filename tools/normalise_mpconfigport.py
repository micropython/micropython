"""
Normalise an mpconfigport.h file by ordering config options, and removing any
that are the defaults.  The standard order and defaults come from py/mpconfig.h.

Run from the root of the repository.

Example usage:

    python3 tools/normalise_mpconfigport.py ports/stm32/mpconfigport.h

"""

import collections, re, sys


regexs = (
    ("define", re.compile(r"#define (MICROPY_[A-Z0-9_]+) +(.+)")),
    ("comment", re.compile(r"// (Extended modules)")),
    ("comment_strip", re.compile(r"/\* (.+) \*/")),
)


def match_line(line):
    for kind, regex in regexs:
        m = regex.match(line)
        if m:
            return kind, m
    return None, None


def parse_config(filename, parse_section_names=False):
    config = collections.OrderedDict()
    section_num = 0
    section_comment = None
    with open(filename) as f:
        for line in f:
            kind, match = match_line(line)
            if kind == "define":
                if section_comment:
                    if parse_section_names:
                        config[f"_section{section_num}"] = section_comment
                    section_num += 1
                    section_comment = None
                config[match.group(1)] = match.group(2)
            elif kind == "comment":
                section_comment = match.group(1)
            elif kind == "comment_strip":
                section_comment = match.group(1).strip()

    return config


def main():
    defaults = parse_config("py/mpconfig.h", True)
    port = parse_config(sys.argv[1])

    # Print out the standard config options in order.
    for k, v in defaults.items():
        if k.startswith("_section"):
            print()
            print(f"// {v}")
            continue
        if k in port:
            if port[k] != v:
                print("#define {:40}{}".format(k, port[k]))
            del port[k]

    # Print out any remaining config options that are not standard ones.
    if port:
        print()
        for k, v in port.items():
            print("#define {:40}{}".format(k, v))


main()
