#!/usr/bin/env python

from __future__ import print_function

import sys
import os
import subprocess
import make_frozen


def main():
    ino_cpp = sys.argv[-3]
    main_py = os.path.join(os.path.dirname(sys.argv[-3]), "main.py")

    # Copy the .ino.cpp to main.py.
    with open(main_py, "w") as fout:
        with open(ino_cpp) as fin:
            for line in fin:
                fout.write(line)

    # Generate the frozen module and write it to .ino.cpp.
    frozen = make_frozen.generate_frozen([main_py])
    with open(ino_cpp, "w") as fout:
        fout.write(frozen)
    subprocess.check_call(sys.argv[1:])

    
if __name__ == '__main__':
    main()
