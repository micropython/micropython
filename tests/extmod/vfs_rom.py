# Test VfsRom filesystem.

try:
    import sys, struct, os, uctypes, vfs

    vfs.VfsRom
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

import unittest


class VfsRomWriter:
    MAGIC = b"MF"

    def __init__(self):
        self.data = bytearray()
        self.data += VfsRomWriter.MAGIC

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


class Test(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.orig_sys_path = list(sys.path)
        fs_inner = make_romfs((("test_inner.txt", b"contents_inner"), ("c.py", b"")))
        cls.romfs = make_romfs(
            (
                ("fs.romfs", fs_inner),
                ("test.txt", b"contents"),
                ("dir/", b""),
                ("dir/a.py", b""),
                ("dir/b.py", b""),
            )
        )
        cls.romfs_addr = uctypes.addressof(cls.romfs)
        cls.romfs_addr_range = range(cls.romfs_addr, cls.romfs_addr + len(cls.romfs))

    @classmethod
    def tearDownClass(cls):
        sys.path = cls.orig_sys_path

    def setUp(self):
        vfs.mount(vfs.VfsRom(self.romfs), "/test_rom")

    def tearDown(self):
        vfs.umount("/test_rom")

    def test_file_operations(self):
        self.assertEqual(os.listdir("/test_rom"), ["fs.romfs", "test.txt", "dir"])

        with open("/test_rom/test.txt") as f:
            self.assertEqual(f.read(), "contents")

    def test_import(self):
        sys.path.append("/test_rom/dir")
        self.assertEqual(__import__("a").__file__, "/test_rom/dir/a.py")
        self.assertEqual(__import__("b").__file__, "/test_rom/dir/b.py")

    def test_memory_mapping(self):
        with open("/test_rom/test.txt", "rb") as f:
            addr = uctypes.addressof(f)
            data = memoryview(f)
            self.assertIn(addr, self.romfs_addr_range)
            self.assertIn(addr + len(data), self.romfs_addr_range)
            self.assertEqual(bytes(data), f.read())

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
