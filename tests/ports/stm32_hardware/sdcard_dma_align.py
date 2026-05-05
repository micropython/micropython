# Test DMA read operations when the buffer alignment in RAM varies.
#
# Test requirements:
# A mostly empty FAT formatted SDCard installed in SD socket
import errno
import os
import pyb
import machine
import micropython
import vfs
import unittest

from micropython import const

_BLOCK_SZ = const(512)
_OFFS_WIDTH = const(64)  # Should be at least the cache line size plus the GC block size
_TEST_BUF_SZ = const(_BLOCK_SZ + _OFFS_WIDTH)

# More repeats = longer test run, more chance of triggering a cache coherence issue
_REPEATS = const(8)

MOUNT_POINT = "/sd"
FILE_PATH = "/sd/stm32_align.blk"

# Skip the whole test if there isn't a mountable SDCard
try:
    sd = pyb.SDCard()
    vfs.mount(sd, MOUNT_POINT)
    vfs.umount(MOUNT_POINT)
    del sd
except (OSError, AttributeError):
    print("SKIP")
    raise SystemExit


def verify_contents(buf, silent=False):
    # Verify that each byte in 'buf' has the value of its index in the buffer
    bad_pos = []
    for i in range(_BLOCK_SZ):
        bi = buf[i]
        if bi != i & 0xFF:
            if silent:
                return False
            bad_pos.append((i, bi))
    if not bad_pos:
        return True

    assert not silent

    print("{} bad readback values in sector:".format(len(bad_pos)), end="")
    for i, v in bad_pos:
        print(" {:#x}={:#x}".format(i, v), end="")
    print()
    return False


class TestSDAlign(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.sd = pyb.SDCard()
        vfs.mount(cls.sd, MOUNT_POINT)
        buf = bytearray(_BLOCK_SZ)
        try:
            with open(FILE_PATH, "rb") as f:
                rlen = f.readinto(buf)
                if rlen != _BLOCK_SZ:
                    raise RuntimeError("Unexpected length of temporary file ", FILE_PATH, rlen)
                if not verify_contents(buf):
                    raise RuntimeError("Corrupt test block in temporary file ", FILE_PATH)
        except OSError as e:
            if e.errno != errno.ENOENT:
                raise

            print("Creating new temp file...")
            with open(FILE_PATH, "wb") as f:
                f.write(bytes(i & 0xFF for i in range(_BLOCK_SZ)))

        # Now look for the sector which holds the temporary file
        # (assume is in the first 20MB of the SD Card)
        for b in range(1, 40960):
            if not cls.sd.readblocks(b, buf):
                raise RuntimeError("Failed to call readblocks on SDCard. block:", b)
            if verify_contents(buf, True):
                print("Temporary file contents found in block {}".format(b))
                cls.block = b
                return

        raise RuntimeError(
            "Contents of temporary file not found near start of SDCard. Too many files?"
        )

    @classmethod
    def tearDownClass(cls):
        try:
            os.unlink(FILE_PATH)
            print("Deleted temp file")
        except OSError:
            pass
        vfs.umount(MOUNT_POINT)
        del cls.sd

    def setUp(self):
        self.offs = 0

    @micropython.native
    def _test_reads_inner(self, buf, repeats=_REPEATS):
        for offs in range(_OFFS_WIDTH):
            with self.subTest(offs=offs):
                self.offs = offs
                slice = memoryview(buf)[offs : offs + _BLOCK_SZ]
                assert len(slice) == _BLOCK_SZ
                for r in range(repeats):
                    self.assertTrue(
                        self.sd.readblocks(self.block, slice),
                        "Read failed for block {} offs {} repeat {}/{}".format(
                            self.block, offs, r, repeats
                        ),
                    )
                    self.assertTrue(
                        verify_contents(slice),
                        "Verify failed for block {} offs {} repeat {}/{}".format(
                            self.block, offs, r, repeats
                        ),
                    )

    def test_reads(self):
        # Test reading at all available offsets in a buffer
        buf = bytearray(_TEST_BUF_SZ)
        # This test is the most random as any failure depends on speculative reads while
        # the DMA operation is in progress. We run the test more times to increase the chance
        # of hitting one of these cases, but even if the issue is present the test only fails
        # once per ~250 iterations. The other tests inject explicit reads and writes so they fail
        # more or less immediately.
        self._test_reads_inner(buf, repeats=_REPEATS * 4)

    def test_interrupted_reads(self):
        # Test reading at all available offsets in a buffer, while an interrupt is
        # scanning through the whole buffer
        buf = bytearray(_TEST_BUF_SZ)
        t = None
        self.scan = 0
        self.val = None
        try:

            @micropython.native
            def timer_cb(_):
                # Arbitrary read from somewhere in the buffer
                self.val = buf[self.scan % _TEST_BUF_SZ]
                self.scan += 1

            t = pyb.Timer(1, freq=30_000, callback=timer_cb, hard=True)
            self._test_reads_inner(buf)
        finally:
            if t:
                t.deinit()
        # print("scan count", self.scan, self.val)

    def test_interrupted_read_write(self):
        # Test reading at all available offsets in a buffer, while an interrupt is
        # writing before & after the DMA buffer
        buf = bytearray(_TEST_BUF_SZ)
        t = None
        try:

            @micropython.native
            def timer_cb(t):
                # Arbitrary CPU write just before and after the buffer, trying to dirty a DMA cache line
                #
                # Note: we never write into a word overlapping the DMA buffer, because if the buffer is not 4-byte aligned
                # sdcard_read_blocks() will do a trick to align it temporarily, and this will race with that trick and
                # corrupt the memory.
                offs = self.offs
                if offs > 3:
                    buf[offs - 4] = 0x55
                offs += _BLOCK_SZ
                if offs < _TEST_BUF_SZ - 3:
                    buf[offs] = 0x56

            # pybd SF2 at default CPU frequency can manage >30kHz <40kHz
            t = pyb.Timer(1, freq=35_000, callback=timer_cb, hard=True)
            self._test_reads_inner(buf)
        finally:
            if t:
                t.deinit()


if __name__ == "__main__":
    unittest.main()
