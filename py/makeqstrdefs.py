"""
This script processes the output from the C preprocessor and extracts all
qstr. Each qstr is transformed into a qstr definition of the form 'Q(...)'.

This script works with Python 2.6, 2.7, 3.3 and 3.4.
"""

import re
import argparse
import os

# Blacklist of qstrings that are specially handled in further
# processing and should be ignored
QSTRING_BLACK_LIST = set(['NULL', 'number_of'])


def write_out(fname, output):
    if output:
        for m, r in [("/", "__"), ("\\", "__"), (":", "@"), ("..", "@@")]:
            fname = fname.replace(m, r)
        with open(args.output_dir + "/" + fname + ".qstr", "w") as f:
            f.write("\n".join(output) + "\n")

def process_file(f):
    output = []
    last_fname = None
    for line in f:
        # match gcc-like output (# n "file") and msvc-like output (#line n "file")
        if line and (line[0:2] == "# " or line[0:5] == "#line"):
            m = re.match(r"#[line]*\s\d+\s\"([^\"]+)\"", line)
            assert m is not None
            fname = m.group(1)
            if not fname.endswith(".c"):
                continue
            if fname != last_fname:
                write_out(last_fname, output)
                output = []
                last_fname = fname
            continue
        for match in re.findall(r'MP_QSTR_[_a-zA-Z0-9]+', line):
            name = match.replace('MP_QSTR_', '')
            if name not in QSTRING_BLACK_LIST:
                output.append('Q(' + name + ')')

    write_out(last_fname, output)
    return ""


def cat_together():
    import glob
    import hashlib
    hasher = hashlib.md5()
    all_lines = []
    outf = open(args.output_dir + "/out", "wb")
    for fname in glob.glob(args.output_dir + "/*.qstr"):
        with open(fname, "rb") as f:
            lines = f.readlines()
            all_lines += lines
    all_lines.sort()
    all_lines = b"\n".join(all_lines)
    outf.write(all_lines)
    outf.close()
    hasher.update(all_lines)
    new_hash = hasher.hexdigest()
    #print(new_hash)
    old_hash = None
    try:
        with open(args.output_file + ".hash") as f:
            old_hash = f.read()
    except IOError:
        pass
    if old_hash != new_hash:
        print("QSTR updated")
        try:
            # rename below might fail if file exists
            os.remove(args.output_file)
        except:
            pass
        os.rename(args.output_dir + "/out", args.output_file)
        with open(args.output_file + ".hash", "w") as f:
            f.write(new_hash)
    else:
        print("QSTR not updated")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Generates qstr definitions from a specified source')

    parser.add_argument('command',
        help='Command (split/cat)')
    parser.add_argument('input_filename',
        help='Name of the input file (when not specified, the script reads standard input)')
    parser.add_argument('output_dir',
        help='Output directory to store individual qstr files')
    parser.add_argument('output_file',
        help='Name of the output file with collected qstrs')

    args = parser.parse_args()
    try:
        os.makedirs(args.output_dir)
    except OSError:
        pass

    if args.command == "split":
        with open(args.input_filename) as infile:
            process_file(infile)

    if args.command == "cat":
        cat_together()
