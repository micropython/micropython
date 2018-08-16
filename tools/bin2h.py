# Reads in a binary file, and converts it to a byte array so that it
# can be #included as a static array like:
#    static const uint8_t bytes_from_binary[] =
#    #include "build/binfile.h"
#    ;
# This script simply prints the escaped string straight to stdout

from __future__ import print_function

import sys

if __name__ == "__main__":
    filename = sys.argv[1]
    with open(filename, "rb") as fh:
        count = 0
        byte = fh.read(1)
        while byte:
            if (count % 16) == 0:
                sys.stdout.write("\n\t")
            sys.stdout.write("0x%02x," % ord(byte))
            count += 1
            byte = fh.read(1)
        print("")
        fh.close()

