# MIT license; Copyright (c) 2022 Damien P. George

import struct, sys, os

try:
    from mpy_cross import run as mpy_cross_run
except ImportError:
    mpy_cross_run = None


class VfsRomWriter:
    ROMFS_HEADER = b"\xd2\xcd\x31"

    ROMFS_RECORD_KIND_UNUSED = 0
    ROMFS_RECORD_KIND_PADDING = 1
    ROMFS_RECORD_KIND_DATA_VERBATIM = 2
    ROMFS_RECORD_KIND_DATA_POINTER = 3
    ROMFS_RECORD_KIND_DIRECTORY = 4
    ROMFS_RECORD_KIND_FILE = 5

    def __init__(self):
        self._dir_stack = [(None, bytearray())]

    def _encode_uint(self, value):
        encoded = [value & 0x7F]
        value >>= 7
        while value != 0:
            encoded.insert(0, 0x80 | (value & 0x7F))
            value >>= 7
        return bytes(encoded)

    def _pack(self, kind, payload):
        return self._encode_uint(kind) + self._encode_uint(len(payload)) + payload

    def _extend(self, data):
        buf = self._dir_stack[-1][1]
        buf.extend(data)
        return len(buf)

    def finalise(self):
        _, data = self._dir_stack.pop()
        encoded_kind = VfsRomWriter.ROMFS_HEADER
        encoded_len = self._encode_uint(len(data))
        if (len(encoded_kind) + len(encoded_len) + len(data)) % 2 == 1:
            encoded_len = b"\x80" + encoded_len
        data = encoded_kind + encoded_len + data
        return data

    def opendir(self, dirname):
        self._dir_stack.append((dirname, bytearray()))

    def closedir(self):
        dirname, dirdata = self._dir_stack.pop()
        dirdata = self._encode_uint(len(dirname)) + bytes(dirname, "ascii") + dirdata
        self._extend(self._pack(VfsRomWriter.ROMFS_RECORD_KIND_DIRECTORY, dirdata))

    def mkdata(self, data):
        assert len(self._dir_stack) == 1
        return self._extend(self._pack(VfsRomWriter.ROMFS_RECORD_KIND_DATA_VERBATIM, data)) - len(
            data
        )

    def mkfile(self, filename, filedata):
        filename = bytes(filename, "ascii")
        payload = self._encode_uint(len(filename))
        payload += filename
        if isinstance(filedata, tuple):
            sub_payload = self._encode_uint(filedata[0])
            sub_payload += self._encode_uint(filedata[1])
            payload += self._pack(VfsRomWriter.ROMFS_RECORD_KIND_DATA_POINTER, sub_payload)
        else:
            payload += self._pack(VfsRomWriter.ROMFS_RECORD_KIND_DATA_VERBATIM, filedata)
        self._dir_stack[-1][1].extend(self._pack(VfsRomWriter.ROMFS_RECORD_KIND_FILE, payload))


def copy_recursively(vfs, src_dir, print_prefix, mpy_cross):
    assert src_dir.endswith("/")
    DIR = 1 << 14
    mpy_cross_missed = 0
    dir_contents = sorted(os.listdir(src_dir))
    for name in dir_contents:
        src_name = src_dir + name
        st = os.stat(src_name)

        if name == dir_contents[-1]:
            # Last entry in the directory listing.
            print_entry = "\\--"
            print_recurse = "    "
        else:
            # Not the last entry in the directory listing.
            print_entry = "|--"
            print_recurse = "|   "

        if st[0] & DIR:
            # A directory, enter it and copy its contents recursively.
            print(print_prefix + print_entry, name + "/")
            vfs.opendir(name)
            mpy_cross_missed += copy_recursively(
                vfs, src_name + "/", print_prefix + print_recurse, mpy_cross
            )
            vfs.closedir()
        else:
            # A file.
            did_mpy = False
            name_extra = ""
            if mpy_cross and name.endswith(".py"):
                name_mpy = name[:-3] + ".mpy"
                src_name_mpy = src_dir + name_mpy
                if not os.path.isfile(src_name_mpy):
                    if mpy_cross_run is not None:
                        did_mpy = True
                        proc = mpy_cross_run(src_name)
                        proc.wait()
                    else:
                        mpy_cross_missed += 1
            if did_mpy:
                name_extra = " -> .mpy"
            print(print_prefix + print_entry, name + name_extra)
            if did_mpy:
                name = name_mpy
                src_name = src_name_mpy
            with open(src_name, "rb") as src:
                vfs.mkfile(name, src.read())
            if did_mpy:
                os.remove(src_name_mpy)
    return mpy_cross_missed


def make_romfs(src_dir, *, mpy_cross):
    if not src_dir.endswith("/"):
        src_dir += "/"

    vfs = VfsRomWriter()

    # Build the filesystem recursively.
    print("Building romfs filesystem, source directory: {}".format(src_dir))
    print("/")
    try:
        mpy_cross_missed = copy_recursively(vfs, src_dir, "", mpy_cross)
    except OSError as er:
        print("Error: OSError {}".format(er), file=sys.stderr)
        sys.exit(1)

    if mpy_cross_missed:
        print("Warning: `mpy_cross` module not found, .py files were not precompiled")
        mpy_cross = False

    return vfs.finalise()
