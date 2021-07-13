"""Simple script that translates "Backtrace:" lines from the ESP output to files
   and line numbers.

   Run with: python3 tools/decode_backtrace.py <board>

   Enter the backtrace line at the "? " prompt. CTRL-C to exit the script.
   """

import subprocess
import sys

board = sys.argv[1]
print(board)

while True:
    addresses = input("? ")
    if addresses.startswith("Backtrace:"):
        addresses = addresses[len("Backtrace:") :]
    addresses = addresses.strip().split()
    addresses = [address.split(":")[0] for address in addresses]
    print("got", addresses)
    subprocess.run(
        ["xtensa-esp32s2-elf-addr2line", "-e", "build-{}/firmware.elf".format(board)] + addresses
    )
