# Test VfsRom filesystem.

try:
    import sys, struct, os, uctypes, vfs

    vfs.VfsRom
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

import unittest

IFDIR = 0x4000
IFREG = 0x8000

SEEK_SET = 0
SEEK_CUR = 1
SEEK_END = 2

# An mpy file with two constant objects: a str and bytes.
test_mpy = (
    b"M\x06\x00\x1f"  # mpy file header
    b"\x03"  # n_qstr
    b"\x02"  # n_obj
    b"\x0etest.py\x00"  # qstr0 = "test.py"
    b"\x0estr_obj\x00"  # qstr1 = "str_obj"
    b"\x12bytes_obj\x00"  # qstr2 = "bytes_obj"
    b"\x05\x14this is a str object\x00"
    b"\x06\x16this is a bytes object\x00"
    b"\x68"  # 13 bytes, no children, bytecode
    b"\x00\x02"  # prelude
    b"\x00"  # simple name (test.py)
    b"\x23\x00"  # LOAD_CONST_OBJ(0)
    b"\x17\x01"  # STORE_GLOBAL(str_obj)
    b"\x23\x01"  # LOAD_CONST_OBJ(1)
    b"\x17\x02"  # STORE_GLOBAL(bytes_obj)
    b"\x51"  # LOAD_CONST_NONE
    b"\x63"  # RETURN_VALUE
)


class VfsRomWriter:
    MAGIC = b"RM\x01\x00"

    def __init__(self):
        self.data = bytearray()
        self.data += VfsRomWriter.MAGIC
        self.mkdir("")

    def finalise(self):
        self.data += b"\x00\x00"
        return self.data

    def mkdir(self, dirname):
        self.data += struct.pack("<H", 0x8000 | len(dirname))
        self.data += bytes(dirname, "ascii")

    def mkfile(self, filename, filedata):
        self.data += struct.pack("<HI", len(filename), len(filedata))
        self.data += bytes(filename, "ascii")
        self.data += filedata


def make_romfs(files):
    fs = VfsRomWriter()
    for filename, contents in files:
        if filename.endswith("/"):
            fs.mkdir(filename[:-1])
        else:
            fs.mkfile(filename, contents)
    return fs.finalise()


class TestBase(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fs_inner = make_romfs((("test_inner.txt", b"contents_inner"), ("c.py", b"")))
        cls.romfs = make_romfs(
            (
                ("fs.romfs", fs_inner),
                ("test.txt", b"contents"),
                ("dir/", b""),
                ("dir/a.py", b""),
                ("dir/b.py", b""),
                ("dir/test.mpy", test_mpy),
            )
        )
        cls.romfs_ilistdir = [
            ("fs.romfs", IFREG, 0, 52),
            ("test.txt", IFREG, 0, 8),
            ("dir", IFDIR, 0, 0),
        ]
        cls.romfs_listdir = [x[0] for x in cls.romfs_ilistdir]
        cls.romfs_listdir_dir = ["a.py", "b.py", "test.mpy"]
        cls.romfs_listdir_bytes = [bytes(x, "ascii") for x in cls.romfs_listdir]
        cls.romfs_addr = uctypes.addressof(cls.romfs)
        cls.romfs_addr_range = range(cls.romfs_addr, cls.romfs_addr + len(cls.romfs))


class TestStandalone(TestBase):
    def test_constructor(self):
        self.assertIsInstance(vfs.VfsRom(self.romfs), vfs.VfsRom)
        self.assertIsInstance(vfs.VfsRom(self.romfs_addr), vfs.VfsRom)
        with self.assertRaises(OSError):
            vfs.VfsRom(b"not a romfs")

    def test_mount(self):
        vfs.VfsRom(self.romfs).mount(True, False)
        with self.assertRaises(OSError):
            vfs.VfsRom(self.romfs).mount(True, True)

    def test_ilistdir(self):
        fs = vfs.VfsRom(self.romfs)
        self.assertEqual(list(fs.ilistdir("")), self.romfs_ilistdir)
        self.assertEqual(list(fs.ilistdir("/")), self.romfs_ilistdir)

    def test_stat(self):
        fs = vfs.VfsRom(self.romfs)
        self.assertEqual(fs.stat(""), (IFDIR, 0, 0, 0, 0, 0, 0, 0, 0, 0))
        self.assertEqual(fs.stat("/"), (IFDIR, 0, 0, 0, 0, 0, 0, 0, 0, 0))
        self.assertEqual(fs.stat("test.txt"), (IFREG, 0, 0, 0, 0, 0, 8, 0, 0, 0))
        self.assertEqual(fs.stat("dir"), (IFDIR, 0, 0, 0, 0, 0, 0, 0, 0, 0))

    def test_statvfs(self):
        fs = vfs.VfsRom(self.romfs)
        self.assertEqual(fs.statvfs(""), (0, 0, 0, 0, 0, 0, 0, 0, 0, 32767))

    def test_open(self):
        fs = vfs.VfsRom(self.romfs)
        with fs.open("test.txt", "") as f:
            self.assertEqual(f.read(), "contents")
        with fs.open("test.txt", "rt") as f:
            self.assertEqual(f.read(), "contents")
        with fs.open("test.txt", "rb") as f:
            self.assertEqual(f.read(), b"contents")
        with self.assertRaises(OSError):
            fs.open("file does not exist", "")
        with self.assertRaises(OSError):
            fs.open("test.txt", "w")
        with self.assertRaises(OSError):
            fs.open("test.txt", "a")
        with self.assertRaises(OSError):
            fs.open("test.txt", "+")

    def test_file_seek(self):
        fs = vfs.VfsRom(self.romfs)
        with fs.open("test.txt", "") as f:
            self.assertEqual(f.seek(0, SEEK_SET), 0)
            self.assertEqual(f.seek(3, SEEK_SET), 3)
            self.assertEqual(f.read(), "tents")
            self.assertEqual(f.seek(0, SEEK_SET), 0)
            self.assertEqual(f.seek(100, SEEK_CUR), 8)
            self.assertEqual(f.seek(-1, SEEK_END), 7)
            self.assertEqual(f.read(), "s")
            self.assertEqual(f.seek(1, SEEK_END), 8)

    def test_memory_mapping(self):
        fs = vfs.VfsRom(self.romfs)
        with fs.open("test.txt", "rb") as f:
            addr = uctypes.addressof(f)
            data = memoryview(f)
            self.assertIn(addr, self.romfs_addr_range)
            self.assertIn(addr + len(data), self.romfs_addr_range)
            self.assertEqual(bytes(data), b"contents")


class TestMounted(TestBase):
    def setUp(self):
        self.orig_sys_path = list(sys.path)
        self.orig_cwd = os.getcwd()
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
        self.assertEqual(os.listdir(), self.romfs_listdir)

        os.chdir("/test_rom/")
        self.assertEqual(os.listdir(), self.romfs_listdir)

        # chdir within the romfs is not implemented.
        with self.assertRaises(OSError):
            os.chdir("/test_rom/dir")

    def test_import_py(self):
        sys.path.append("/test_rom/dir")
        self.assertEqual(__import__("a").__file__, "/test_rom/dir/a.py")
        self.assertEqual(__import__("b").__file__, "/test_rom/dir/b.py")

    def test_import_mpy(self):
        sys.path.append("/test_rom/dir")
        test = __import__("test")
        self.assertEqual(test.__file__, "/test_rom/dir/test.mpy")
        self.assertEqual(test.str_obj, "this is a str object")
        self.assertEqual(test.bytes_obj, b"this is a bytes object")
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
