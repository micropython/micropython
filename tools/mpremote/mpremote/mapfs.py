# MIT license; Copyright (c) 2022 Damien P. George

import struct, sys, os


class VfsMapWriter:
    MAGIC = b"MF"

    def __init__(self):
        self.filename = None
        self.data = bytearray()
        self.data += VfsMapWriter.MAGIC

    def finalise(self):
        self.data += b"\x00\x00"

    def open(self, filename, attr):
        assert self.filename is None
        assert attr == "wb"
        self.filename = filename
        self.filedata = b""
        return self

    def mkdir(self, dir):
        assert self.filename is None
        self.data += struct.pack("<H", 0x8000 | len(dir))
        self.data += bytes(dir, "ascii")

    def close(self):
        assert self.filename
        self.data += struct.pack("<HI", len(self.filename), len(self.filedata))
        self.data += bytes(self.filename, "ascii")
        self.data += self.filedata
        self.filename = None

    def write(self, data):
        assert self.filename
        self.filedata += data

    def __enter__(self):
        return self

    def __exit__(self, a, b, c):
        self.close()


def copy_recursively(vfs, src_dir, dest_dir):
    if dest_dir == "/":
        dest_dir = ""
    assert src_dir.endswith("/")
    assert len(dest_dir) == 0 or dest_dir.endswith("/")
    DIR = 1 << 14
    for name in os.listdir(src_dir):
        if name in (".", ".."):
            continue
        src_name = src_dir + name
        dest_name = dest_dir + name
        st = os.stat(src_name)
        if st[0] & DIR:
            print(" -", dest_name + "/")
            vfs.mkdir(dest_name)
            copy_recursively(vfs, src_name + "/", dest_name + "/")
        else:
            print(" -", dest_name)
            with open(src_name, "rb") as src, vfs.open(dest_name, "wb") as dest:
                dest.write(src.read())


def make_mapfs(src_dir, dest_dir):
    if not src_dir.endswith("/"):
        src_dir += "/"

    vfs = VfsMapWriter()

    # Build the filesystem recursively.
    print("Building mapfs filesystem, source directory: {}".format(src_dir))
    try:
        copy_recursively(vfs, src_dir, dest_dir)
    except OSError as er:
        if er.args[0] == 28:  # ENOSPC
            print("Error: not enough space on filesystem", file=sys.stderr)
            sys.exit(1)
        else:
            print("Error: OSError {}".format(er.args[0]), file=sys.stderr)
            sys.exit(1)

    vfs.finalise()

    return vfs.data


def main():
    if len(sys.argv) != 2:
        print("usage: {} <dir>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    # Parse arguments.
    dir = sys.argv[1]

    mapfs = make_mapfs(dir, "/")

    # Save the block device data.
    output = dir.rstrip("/") + ".mapfs"
    print("Writing filesystem image to {}".format(output))
    with open(output, "wb") as f:
        f.write(mapfs)


if __name__ == "__main__":
    main()
