"""
This script processes the output from the C preprocessor and extracts all
qstr. Each qstr is transformed into a qstr definition of the form 'Q(...)'.

This script works with Python 2.6, 2.7, 3.3 and 3.4.
"""

import sys
import re
import argparse
import os

# Blacklist of qstrings that are specially handled in further
# processing and should be ignored
QSTRING_BLACK_LIST = {'NULL', 'number_of', }


def debug(message):
    #sys.stderr.write("%s\n" % message)
    pass


def process_file(f):
    output = []
    for line in f:
        for match in re.findall(r'MP_QSTR_[_a-zA-Z0-9]+', line):
            name = match.replace('MP_QSTR_', '')
            if name not in QSTRING_BLACK_LIST:
                output.append('Q(' + name + ')')

    # make sure there is a newline at the end of the output
    output.append('')

    return '\n'.join(output)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Generates qstr definitions from a specified source')

    parser.add_argument('-o', '--output-file', dest='output_filename',
        help='Output filename (defaults to stdout)')
    parser.add_argument('input_filename', nargs='?',
        help='Name of the input file (when not specified, the script reads standard input')
    parser.add_argument('-s', '--skip-write-when-same', dest='skip_write_when_same',
        action='store_true', default=False,
        help="Don't write the output file if it already exists and the contents have not changed (disabled by default)")

    args = parser.parse_args()

    # Check if the file contents changed from last time
    write_file = True

    # By default write into STDOUT
    outfile = sys.stdout
    real_output_filename = 'STDOUT'

    if args.input_filename:
        infile = open(args.input_filename, 'r')
    else:
        infile = sys.stdin

    file_data = process_file(infile)
    infile.close()

    # Detect custom output file name
    if args.output_filename:
        real_output_filename = args.output_filename
        if os.path.isfile(args.output_filename) and args.skip_write_when_same:
            with open(args.output_filename, 'r') as f:
                existing_data = f.read()
            if existing_data == file_data:
                debug("Skip regeneration of: %s\n" % real_output_filename)
                write_file = False
            else:
                debug("File HAS changed, overwriting\n")
                outfile = open(args.output_filename, 'w')
        else:
            outfile = open(args.output_filename, 'w')

    # Only write the file if we the data has changed
    if write_file:
        sys.stderr.write("QSTR %s\n" % real_output_filename)
        outfile.write(file_data)
        outfile.close()
