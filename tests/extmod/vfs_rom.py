# Test VfsRom filesystem.

try:
    import errno, sys, struct, os, uctypes, vfs

    vfs.VfsRom
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

try:
    import select
except ImportError:
    select = None

import unittest

IFDIR = 0x4000
IFREG = 0x8000

SEEK_SET = 0
SEEK_CUR = 1
SEEK_END = 2

# An mpy file with four constant objects: str, bytes, long-int, float.
test_mpy = (
    # header
    b"M\x06\x00\x1f"  # mpy file header
    b"\x06"  # n_qstr
    b"\x05"  # n_obj
    # qstrs
    b"\x0etest.py\x00"  # qstr0 = "test.py"
    b"\x0f"  # qstr1 = "<module>"
    b"\x0estr_obj\x00"  # qstr2 = "str_obj"
    b"\x12bytes_obj\x00"  # qstr3 = "bytes_obj"
    b"\x0eint_obj\x00"  # qstr4 = "int_obj"
    b"\x12float_obj\x00"  # qstr5 = "float_obj"
    # objects
    b"\x05\x14this is a str object\x00"
    b"\x06\x16this is a bytes object\x00"
    b"\x07\x0a1234567890"  # long-int object
    b"\x08\x041.23"  # float object
    b"\x05\x07str_obj\x00"  # str object of existing qstr
    # bytecode
    b"\x81\x28"  # 21 bytes, no children, bytecode
    b"\x00\x02"  # prelude
    b"\x01"  # simple name (<module>)
    b"\x23\x00"  # LOAD_CONST_OBJ(0)
    b"\x16\x02"  # STORE_NAME(str_obj)
    b"\x23\x01"  # LOAD_CONST_OBJ(1)
    b"\x16\x03"  # STORE_NAME(bytes_obj)
    b"\x23\x02"  # LOAD_CONST_OBJ(2)
    b"\x16\x04"  # STORE_NAME(int_obj)
    b"\x23\x03"  # LOAD_CONST_OBJ(3)
    b"\x16\x05"  # STORE_NAME(float_obj)
    b"\x51"  # LOAD_CONST_NONE
    b"\x63"  # RETURN_VALUE
)


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

    def mkfile(self, filename, filedata, extra_payload=b""):
        filename = bytes(filename, "ascii")
        payload = self._encode_uint(len(filename))
        payload += filename
        payload += extra_payload
        if isinstance(filedata, tuple):
            sub_payload = self._encode_uint(filedata[0])
            sub_payload += self._encode_uint(filedata[1])
            payload += self._pack(VfsRomWriter.ROMFS_RECORD_KIND_DATA_POINTER, sub_payload)
        else:
            payload += self._pack(VfsRomWriter.ROMFS_RECORD_KIND_DATA_VERBATIM, filedata)
        self._dir_stack[-1][1].extend(self._pack(VfsRomWriter.ROMFS_RECORD_KIND_FILE, payload))


def _make_romfs(fs, files, data_map):
    for filename, contents in files:
        if isinstance(contents, tuple):
            fs.opendir(filename)
            _make_romfs(fs, contents, data_map)
            fs.closedir()
        elif isinstance(contents, int):
            fs.mkfile(filename, data_map[contents])
        else:
            fs.mkfile(filename, contents)


def make_romfs(files, data=None):
    fs = VfsRomWriter()
    data_map = {}
    if data:
        for k, v in data.items():
            data_map[k] = len(v), fs.mkdata(v)
    _make_romfs(fs, files, data_map)
    return fs.finalise()


# A class to test if a value is within a range, needed because MicroPython's range
# doesn't support arbitrary objects.
class Range:
    def __init__(self, lower, upper):
        self._lower = lower
        self._upper = upper

    def __repr__(self):
        return "Range({}, {})".format(self._lower, self._upper)

    def __contains__(self, value):
        return self._lower <= value < self._upper


class TestBase(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fs_inner = make_romfs((("test_inner.txt", b"contents_inner"), ("c.py", b"")))
        cls.romfs = make_romfs(
            (
                ("fs.romfs", 0),
                ("test.txt", b"contents"),
                (
                    "dir",
                    (
                        ("a.py", b"x = 1"),
                        ("b.py", b"x = 2"),
                        ("test.mpy", test_mpy),
                    ),
                ),
            ),
            {0: fs_inner},
        )
        cls.romfs_ilistdir = [
            ("fs.romfs", IFREG, 0, 46),
            ("test.txt", IFREG, 0, 8),
            ("dir", IFDIR, 0, 198),
        ]
        cls.romfs_listdir = [x[0] for x in cls.romfs_ilistdir]
        cls.romfs_listdir_dir = ["a.py", "b.py", "test.mpy"]
        cls.romfs_listdir_bytes = [bytes(x, "ascii") for x in cls.romfs_listdir]
        cls.romfs_addr = uctypes.addressof(cls.romfs)
        cls.romfs_addr_range = Range(cls.romfs_addr, cls.romfs_addr + len(cls.romfs))


class TestEdgeCases(unittest.TestCase):
    def test_empty_romfs(self):
        empty_romfs = make_romfs(())
        self.assertEqual(empty_romfs, bytes([0x80 | ord("R"), 0x80 | ord("M"), ord("1"), 0]))
        fs = vfs.VfsRom(empty_romfs)
        self.assertIsInstance(fs, vfs.VfsRom)
        fs.mount(True, False)
        self.assertEqual(list(fs.ilistdir("")), [])
        self.assertEqual(fs.stat(""), (IFDIR, 0, 0, 0, 0, 0, 0, 0, 0, 0))
        self.assertEqual(fs.statvfs(""), (1, 0, 0, 0, 0, 0, 0, 0, 0, 32767))

    def test_error(self):
        for bad_romfs in (b"", b"xxx", b"not a romfs"):
            with self.assertRaises(OSError) as ctx:
                vfs.VfsRom(bad_romfs)
            self.assertEqual(ctx.exception.errno, errno.ENODEV)

    def test_unknown_record(self):
        fs = VfsRomWriter()
        fs._extend(fs._pack(VfsRomWriter.ROMFS_RECORD_KIND_PADDING, b"payload"))
        fs.mkfile(
            "test",
            b"contents",
            extra_payload=fs._pack(VfsRomWriter.ROMFS_RECORD_KIND_PADDING, b"pad"),
        )
        romfs = fs.finalise()
        fs = vfs.VfsRom(romfs)
        self.assertEqual(list(fs.ilistdir("")), [("test", IFREG, 0, 8)])
        with fs.open("test", "rb") as f:
            self.assertEqual(f.read(), b"contents")


class TestCorrupt(unittest.TestCase):
    def test_corrupt_filesystem(self):
        # Make the filesystem length bigger than the buffer.
        romfs = bytearray(make_romfs(()))
        romfs[3] = 0x01
        with self.assertRaises(OSError):
            vfs.VfsRom(romfs)

        # Corrupt the filesystem length.
        romfs = bytearray(make_romfs(()))
        romfs[3] = 0xFF
        with self.assertRaises(OSError):
            vfs.VfsRom(romfs)

        # Corrupt the contents of the filesystem.
        romfs = bytearray(make_romfs(()))
        romfs[3] = 0x01
        romfs.extend(b"\xff\xff")
        fs = vfs.VfsRom(romfs)
        with self.assertRaises(OSError):
            fs.stat("a")
        self.assertEqual(list(fs.ilistdir("")), [])

    def test_corrupt_file_entry(self):
        romfs = make_romfs((("file", b"data"),))

        # Corrupt the length of filename.
        romfs_corrupt = bytearray(romfs)
        romfs_corrupt[7:] = b"\xff" * (len(romfs) - 7)
        fs = vfs.VfsRom(romfs_corrupt)
        with self.assertRaises(OSError):
            fs.stat("file")
        self.assertEqual(list(fs.ilistdir("")), [])

        # Erase the data record (change it to a padding record).
        romfs_corrupt = bytearray(romfs)
        romfs_corrupt[12] = VfsRomWriter.ROMFS_RECORD_KIND_PADDING
        fs = vfs.VfsRom(romfs_corrupt)
        with self.assertRaises(OSError):
            fs.stat("file")
        self.assertEqual(list(fs.ilistdir("")), [])

        # Corrupt the header of the data record.
        romfs_corrupt = bytearray(romfs)
        romfs_corrupt[12:] = b"\xff" * (len(romfs) - 12)
        fs = vfs.VfsRom(romfs_corrupt)
        with self.assertRaises(OSError):
            fs.stat("file")

        # Corrupt the interior of the data record.
        romfs_corrupt = bytearray(romfs)
        romfs_corrupt[13:] = b"\xff" * (len(romfs) - 13)
        fs = vfs.VfsRom(romfs_corrupt)
        with self.assertRaises(OSError):
            fs.stat("file")

        # Change the data record to an indirect pointer and corrupt the length.
        romfs_corrupt = bytearray(romfs)
        romfs_corrupt[12] = VfsRomWriter.ROMFS_RECORD_KIND_DATA_POINTER
        romfs_corrupt[14:18] = b"\xff\xff\xff\xff"
        fs = vfs.VfsRom(romfs_corrupt)
        with self.assertRaises(OSError):
            fs.stat("file")

        # Change the data record to an indirect pointer and corrupt the offset.
        romfs_corrupt = bytearray(romfs)
        romfs_corrupt[12] = VfsRomWriter.ROMFS_RECORD_KIND_DATA_POINTER
        romfs_corrupt[14:18] = b"\x00\xff\xff\xff"
        fs = vfs.VfsRom(romfs_corrupt)
        with self.assertRaises(OSError):
            fs.stat("file")


class TestStandalone(TestBase):
    def test_constructor(self):
        self.assertIsInstance(vfs.VfsRom(self.romfs), vfs.VfsRom)
        with self.assertRaises(TypeError):
            vfs.VfsRom(self.romfs_addr)

    def test_mount(self):
        vfs.VfsRom(self.romfs).mount(True, False)
        with self.assertRaises(OSError):
            vfs.VfsRom(self.romfs).mount(True, True)

    def test_ilistdir(self):
        fs = vfs.VfsRom(self.romfs)
        self.assertEqual(list(fs.ilistdir("")), self.romfs_ilistdir)
        self.assertEqual(list(fs.ilistdir("/")), self.romfs_ilistdir)
        with self.assertRaises(OSError):
            fs.ilistdir("does not exist")

    def test_stat(self):
        fs = vfs.VfsRom(self.romfs)
        self.assertEqual(fs.stat(""), (IFDIR, 0, 0, 0, 0, 0, 289, 0, 0, 0))
        self.assertEqual(fs.stat("/"), (IFDIR, 0, 0, 0, 0, 0, 289, 0, 0, 0))
        self.assertEqual(fs.stat("/test.txt"), (IFREG, 0, 0, 0, 0, 0, 8, 0, 0, 0))
        self.assertEqual(fs.stat("/dir"), (IFDIR, 0, 0, 0, 0, 0, 198, 0, 0, 0))
        with self.assertRaises(OSError):
            fs.stat("/does-not-exist")

    def test_statvfs(self):
        fs = vfs.VfsRom(self.romfs)
        self.assertEqual(fs.statvfs(""), (1, 0, 289, 0, 0, 0, 0, 0, 0, 32767))

    def test_open(self):
        fs = vfs.VfsRom(self.romfs)

        with fs.open("/test.txt", "") as f:
            self.assertEqual(f.read(), "contents")
        with fs.open("/test.txt", "rt") as f:
            self.assertEqual(f.read(), "contents")
        with fs.open("/test.txt", "rb") as f:
            self.assertEqual(f.read(), b"contents")

        with self.assertRaises(OSError) as ctx:
            fs.open("/file-does-not-exist", "")
        self.assertEqual(ctx.exception.errno, errno.ENOENT)

        with self.assertRaises(OSError) as ctx:
            fs.open("/dir", "rb")
        self.assertEqual(ctx.exception.errno, errno.EISDIR)

        with self.assertRaises(OSError):
            fs.open("/test.txt", "w")
        with self.assertRaises(OSError):
            fs.open("/test.txt", "a")
        with self.assertRaises(OSError):
            fs.open("/test.txt", "+")

    def test_file_seek(self):
        fs = vfs.VfsRom(self.romfs)
        with fs.open("/test.txt", "") as f:
            self.assertEqual(f.seek(0, SEEK_SET), 0)
            self.assertEqual(f.seek(3, SEEK_SET), 3)
            self.assertEqual(f.read(), "tents")
            self.assertEqual(f.seek(0, SEEK_SET), 0)
            self.assertEqual(f.seek(100, SEEK_CUR), 8)
            self.assertEqual(f.seek(-1, SEEK_END), 7)
            self.assertEqual(f.read(), "s")
            self.assertEqual(f.seek(1, SEEK_END), 8)
            with self.assertRaises(OSError):
                f.seek(-1, SEEK_SET)
            f.seek(0, SEEK_SET)
            with self.assertRaises(OSError):
                f.seek(-1, SEEK_CUR)
            with self.assertRaises(OSError):
                f.seek(-100, SEEK_END)

    @unittest.skipIf(select is None, "no select module")
    def test_file_ioctl_invalid(self):
        fs = vfs.VfsRom(self.romfs)
        with fs.open("/test.txt", "") as f:
            p = select.poll()
            p.register(f)
            with self.assertRaises(OSError):
                p.poll(0)

    def test_memory_mapping(self):
        fs = vfs.VfsRom(self.romfs)
        with fs.open("/test.txt", "rb") as f:
            addr = uctypes.addressof(f)
            data = memoryview(f)
            self.assertIn(addr, self.romfs_addr_range)
            self.assertIn(addr + len(data), self.romfs_addr_range)
            self.assertEqual(bytes(data), b"contents")


class TestMounted(TestBase):
    def setUp(self):
        self.orig_sys_path = list(sys.path)
        self.orig_cwd = os.getcwd()
        sys.path = []
        vfs.mount(vfs.VfsRom(self.romfs), "/test_rom")

    def tearDown(self):
        vfs.umount("/test_rom")
        os.chdir(self.orig_cwd)
        sys.path = self.orig_sys_path

    def test_listdir(self):
        self.assertEqual(os.listdir("/test_rom"), self.romfs_listdir)
        self.assertEqual(os.listdir("/test_rom/dir"), self.romfs_listdir_dir)
        self.assertEqual(os.listdir(b"/test_rom"), self.romfs_listdir_bytes)

    def test_chdir(self):
        os.chdir("/test_rom")
        self.assertEqual(os.getcwd(), "/test_rom")
        self.assertEqual(os.listdir(), self.romfs_listdir)

        os.chdir("/test_rom/")
        self.assertEqual(os.getcwd(), "/test_rom")
        self.assertEqual(os.listdir(), self.romfs_listdir)

        # chdir within the romfs is not implemented.
        with self.assertRaises(OSError):
            os.chdir("/test_rom/dir")

    def test_stat(self):
        self.assertEqual(os.stat("/test_rom"), (IFDIR, 0, 0, 0, 0, 0, 289, 0, 0, 0))
        self.assertEqual(os.stat("/test_rom/"), (IFDIR, 0, 0, 0, 0, 0, 289, 0, 0, 0))
        self.assertEqual(os.stat("/test_rom/test.txt"), (IFREG, 0, 0, 0, 0, 0, 8, 0, 0, 0))
        self.assertEqual(os.stat("/test_rom/dir"), (IFDIR, 0, 0, 0, 0, 0, 198, 0, 0, 0))
        with self.assertRaises(OSError):
            os.stat("/test_rom/does-not-exist")

    def test_open(self):
        with open("/test_rom/test.txt") as f:
            self.assertEqual(f.read(), "contents")
        with open("/test_rom/test.txt", "b") as f:
            self.assertEqual(f.read(), b"contents")

        with self.assertRaises(OSError) as ctx:
            open("/test_rom/file-does-not-exist")
        self.assertEqual(ctx.exception.errno, errno.ENOENT)

        with self.assertRaises(OSError) as ctx:
            open("/test_rom/dir")
        self.assertEqual(ctx.exception.errno, errno.EISDIR)

    def test_import_py(self):
        sys.path.append("/test_rom/dir")
        a = __import__("a")
        b = __import__("b")
        self.assertEqual(a.__file__, "/test_rom/dir/a.py")
        self.assertEqual(a.x, 1)
        self.assertEqual(b.__file__, "/test_rom/dir/b.py")
        self.assertEqual(b.x, 2)

    def test_import_mpy(self):
        sys.path.append("/test_rom/dir")
        test = __import__("test")
        self.assertEqual(test.__file__, "/test_rom/dir/test.mpy")
        self.assertEqual(test.str_obj, "this is a str object")
        self.assertEqual(test.bytes_obj, b"this is a bytes object")
        self.assertEqual(test.int_obj, 1234567890)
        self.assertEqual(test.float_obj, 1.23)
        self.assertIn(uctypes.addressof(test.str_obj), self.romfs_addr_range)
        self.assertIn(uctypes.addressof(test.bytes_obj), self.romfs_addr_range)

    def test_romfs_inner(self):
        with open("/test_rom/fs.romfs", "rb") as f:
            romfs_inner = vfs.VfsRom(memoryview(f))

        vfs.mount(romfs_inner, "/test_rom2")

        self.assertEqual(os.listdir("/test_rom2"), ["test_inner.txt", "c.py"])

        sys.path.append("/test_rom2")
        self.assertEqual(__import__("c").__file__, "/test_rom2/c.py")

        with open("/test_rom2/test_inner.txt") as f:
            self.assertEqual(f.read(), "contents_inner")

        with open("/test_rom2/test_inner.txt", "rb") as f:
            addr = uctypes.addressof(f)
            data = memoryview(f)
            self.assertIn(addr, self.romfs_addr_range)
            self.assertIn(addr + len(data), self.romfs_addr_range)

        vfs.umount("/test_rom2")


if __name__ == "__main__":
    unittest.main()
