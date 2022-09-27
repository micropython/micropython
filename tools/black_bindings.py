#!/usr/bin/python3
from concurrent.futures import ThreadPoolExecutor
import os
import re
import subprocess
import sys


def transform(fn, s):
    lines = s.rstrip().split("\n")
    lines = [line.removeprefix("//| ").removeprefix("//|") for line in lines]
    while lines and not lines[0]:
        del lines[0]

    s = "\n".join(lines) + "\n"
    if s[0] == " ":
        prefix = "if 0:\n"
    else:
        prefix = ""
    s = prefix + s
    try:
        # Line length is 95 so that with "//| " the max is 99
        result = subprocess.run(
            ["black", "--pyi", "-l95", "-q", "-"],
            input=s,
            check=True,
            stdout=subprocess.PIPE,
            encoding="utf-8",
        )
    except subprocess.CalledProcessError as e:
        print(f"{fn}:0: Failed to process block:\n{s}")
        raise

    result = result.stdout[len(prefix) :].strip("\n")
    result = (result.rstrip()).split("\n")
    return "\n".join("//| " + line if line else "//|" for line in result) + "\n"


def process_one_file(fn):
    with open(fn, "r", encoding="utf-8") as f:
        content = f.read()

    old_end = 0

    parts = []
    for m in re.finditer("(?m)((?:^//\|.*\n)(?:^//\|.*\n)*)", content):
        parts.append(content[old_end : m.start()])
        parts.append(transform(fn, m.group()))
        old_end = m.end()
    parts.append(content[old_end:])
    newcontent = "".join(parts)

    if newcontent != content:
        with open(fn, "w", encoding="utf-8") as f:
            f.write(newcontent)


if __name__ == "__main__":
    # Use a thread pool because most processing is inside black!
    executor = ThreadPoolExecutor(max_workers=os.cpu_count())
    futures = [executor.submit(process_one_file, fn) for fn in sys.argv[1:]]
    status = 0
    for f in futures:
        try:
            f.result()
        except Exception as e:
            status = 1
    executor.shutdown()
    raise SystemExit(status)
