"""This script diffs two dumps of symbol sizes by matching up the symbol names

To generate the input files do something like:

arm-none-eabi-nm --size-sort build-bluemicro833/firmware.elf > new_sizes.txt

The command will vary by board and along with git state.

To print the diff do:

python diff_nm_sizes.py old_sizes.txt new_sizes.txt
"""

import sys
import pathlib

old = pathlib.Path(sys.argv[-2])
new = pathlib.Path(sys.argv[-1])
old_symbols = {}
old_total_size = 0
longest_symbol = 0
for line in old.read_text().split("\n"):
    if not line:
        continue
    size, t, name = line.split()
    old_size = int(size, 16)
    old_total_size += old_size
    old_symbols[name] = old_size
    longest_symbol = max(longest_symbol, len(name))

new_total_size = 0
for line in new.read_text().split("\n"):
    if not line:
        continue
    size, t, name = line.split()
    size = int(size, 16)
    new_total_size += size
    if name not in old_symbols:
        print(f"{name:<{longest_symbol}}{size:>+6}")
    else:
        old_size = old_symbols[name]
        del old_symbols[name]
        if size == old_size:
            continue
        print(f"{name:<{longest_symbol}}{size - old_size:>+6}")

for name in old_symbols:
    old_size = old_symbols[name]
    print(f"{name:<{longest_symbol}}{-old_size:>+6}")

print()
total_label = f"Total {new_total_size} - {old_total_size}"
print(f"{total_label:<{longest_symbol}}{new_total_size - old_total_size:>+6}")
