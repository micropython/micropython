# MIT license; Copyright (c) 2022 Damien P. George

import struct, sys, os


class VfsRomWriter:
    MAGIC = 0x294d

    def __init__(self):
        self.data = bytearray()
        self.mkdir("")

    def _encode_uint(self, value):
        encoded = [value & 0x7F]
        value >>= 7
        while value != 0:
            encoded.insert(0, 0x80 | (value & 0x7F))
            value >>= 7
        return bytes(encoded)

    def _pack(self, kind, payload):
        return self._encode_uint(kind) + self._encode_uint(len(payload)) + payload

    def finalise(self):
        encoded_kind = self._encode_uint(VfsRomWriter.MAGIC)
        encoded_len = self._encode_uint(len(self.data))
        if (len(encoded_len) + len(self.data)) % 2 == 1:
            encoded_len = b"\x80" + encoded_len
        self.data = encoded_kind + encoded_len + self.data
        return self.data

    def mkdir(self, dirname):
        payload = bytes(dirname, "ascii")
        self.data += self._pack(1, payload)

    def mkfile(self, filename, filedata):
        filename = bytes(filename, "ascii")
        payload = self._encode_uint(len(filename))
        payload += self._encode_uint(len(filedata))
        payload += filename
        payload += filedata
        self.data += self._pack(2, payload)


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
            # todo mpy-cross .py files
            print(" -", dest_name)
            with open(src_name, "rb") as src:
                vfs.mkfile(dest_name, src.read())


def make_romfs(src_dir, dest_dir):
    if not src_dir.endswith("/"):
        src_dir += "/"

    vfs = VfsRomWriter()

    # Build the filesystem recursively.
    print("Building romfs filesystem, source directory: {}".format(src_dir))
    try:
        copy_recursively(vfs, src_dir, dest_dir)
    except OSError as er:
        if er.args[0] == 28:  # ENOSPC
            print("Error: not enough space on filesystem", file=sys.stderr)
            sys.exit(1)
        else:
            print("Error: OSError {}".format(er.args[0]), file=sys.stderr)
            sys.exit(1)

    return vfs.finalise()


def main():
    if len(sys.argv) != 2:
        print("usage: {} <dir>".format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)

    # Parse arguments.
    dir = sys.argv[1]

    romfs = make_romfs(dir, "/")

    # Save the block device data.
    output = dir.rstrip("/") + ".romfs"
    print("Writing filesystem image to {}".format(output))
    with open(output, "wb") as f:
        f.write(romfs)


if __name__ == "__main__":
    main()
