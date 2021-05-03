#!/usr/bin/env python3

import sys
import re
import os
import itertools
from concurrent.futures import ProcessPoolExecutor
import multiprocessing
import threading
import subprocess

from makeqstrdefs import qstr_unescape, QSTRING_BLOCK_LIST

re_line = re.compile(r"#[line]*\s(\d+)\s\"([^\"]+)\"", re.DOTALL)
re_qstr = re.compile(r"MP_QSTR_[_a-zA-Z0-9]+", re.DOTALL)
re_translate = re.compile(r"translate\(\"((?:(?=(\\?))\2.)*?)\"\)", re.DOTALL)


def write_out(fname, output_dir, output):
    if output:
        for m, r in [("/", "__"), ("\\", "__"), (":", "@"), ("..", "@@")]:
            fname = fname.replace(m, r)
        with open(output_dir + "/" + fname + ".qstr", "w") as f:
            f.write("\n".join(output) + "\n")


def process_file(fname, output_dir, content):
    content = content.decode("utf-8", errors="ignore")
    output = []
    for match in re_qstr.findall(content):
        name = match.replace("MP_QSTR_", "")
        if name not in QSTRING_BLOCK_LIST:
            output.append("Q(" + qstr_unescape(name) + ")")
    for match in re_translate.findall(content):
        output.append('TRANSLATE("' + match[0] + '")')

    write_out(fname, output_dir, output)


def checkoutput1(args):
    info = subprocess.run(args, check=True, stdout=subprocess.PIPE, input="")
    return info.stdout


def preprocess(command, output_dir, fn):
    try:
        output = checkoutput1(command + [fn])
        process_file(fn, output_dir, output)
    except Exception as e:
        print(e, file=sys.stderr)


def maybe_preprocess(command, output_dir, fn):
    # Preprocess the source file if it contains "MP_QSTR", "translate",
    # or if it uses enum.h (which generates "MP_QSTR" strings.
    if subprocess.call(["grep", "-lqE", r"(MP_QSTR|translate|enum\.h)", fn]) == 0:
        preprocess(command, output_dir, fn)


if __name__ == "__main__":

    idx1 = sys.argv.index("--")
    idx2 = sys.argv.index("--", idx1 + 1)
    output_dir = sys.argv[1]
    check = sys.argv[2:idx1]
    always = sys.argv[idx1 + 1 : idx2]
    command = sys.argv[idx2 + 1 :]

    if not os.path.isdir(output_dir):
        os.makedirs(output_dir)

    # Mac and Windows use 'spawn'.  Uncomment this during testing to catch spawn-specific problems on Linux.
    # multiprocessing.set_start_method("spawn")
    executor = ProcessPoolExecutor(max_workers=multiprocessing.cpu_count() + 1)
    executor.map(maybe_preprocess, itertools.repeat(command), itertools.repeat(output_dir), check)
    executor.map(preprocess, itertools.repeat(command), itertools.repeat(output_dir), always)
    executor.shutdown()
