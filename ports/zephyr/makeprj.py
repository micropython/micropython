#!/usr/bin/env python3
import sys
import os
import hashlib


def hash_file(fname):
    if not os.path.exists(fname):
        return b""
    hasher = hashlib.md5()
    with open(fname, "rb") as f:
        hasher.update(f.read())
    return hasher.digest()


old_digest = hash_file(sys.argv[3])

with open(sys.argv[3] + ".tmp", "wb") as f:
    f.write(open(sys.argv[1], "rb").read())
    if os.path.exists(sys.argv[2]):
        f.write(open(sys.argv[2], "rb").read())

new_digest = hash_file(sys.argv[3] + ".tmp")

if new_digest != old_digest:
    print("Replacing")
    os.rename(sys.argv[3] + ".tmp", sys.argv[3])
else:
    os.remove(sys.argv[3] + ".tmp")
