#! /usr/bin/env python3
import argparse

import os
import os.path
import sys
import re
from string import Template

parser = argparse.ArgumentParser(description='Apply #define values to .template.ld file.')
parser.add_argument('template_files', metavar='TEMPLATE_FILE', type=argparse.FileType('r'),
                    nargs='+', help="template filename: <something>.template.ld")
parser.add_argument('--defines', type=argparse.FileType('r'), required=True)
parser.add_argument('--out_dir', required=True)

args = parser.parse_args()

defines = {}

# We're looking for lines like this:
# <expression> ///DEFINE_VALUE NAME_OF_VALUE <optional Python lambda expression to transform value>
VALUE_LINE_RE = re.compile(r'^([^/].*); ///DEFINE_VALUE (\w+)(.*)$')

for line in args.defines:
    match = VALUE_LINE_RE.match(line.strip())
    if match:
        value = match.group(1).strip()
        name = match.group(2)
        lambda_exp = match.group(3).strip()
        # Apply the given lambda to the value if it is present, else just store the value.
        defines[match.group(2)] = eval(lambda_exp)(value) if lambda_exp else value

#print(defines)
fail = False

for template_file in args.template_files:
    ld_template_basename = os.path.basename(template_file.name)
    ld_pathname = os.path.join(args.out_dir, ld_template_basename.replace('.template.ld', '.ld'))
    with open(ld_pathname, 'w') as output:
        try:
            output.write(Template(template_file.read()).substitute(defines))
        except KeyError as e:
            print("ERROR: {}: No #define for '{}'".format(ld_pathname, e.args[0]), file=sys.stderr)
            fail = True

if fail:
    sys.exit(1)
