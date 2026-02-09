"""Transport-agnostic OTA helpers for MicroPython ESP32 targets.

This module consolidates the block-device writer and partition management logic
required to stream a firmware image into the inactive OTA slot, verify the
SHA-256 digest, and mark the new image for boot. It is designed to be reused by
any transport layer (BLE, Wi-Fi, USB, etc.) that can provide ordered chunks of
firmware data.
"""

import hashlib
import io
import machine
import time
from esp32 import Partition
from micropython import const

IOCTL_BLOCK_COUNT = const(4)
IOCTL_BLOCK_SIZE = const(5)
IOCTL_BLOCK_ERASE = const(6)


class OTAError(Exception):
    """Base exception for OTA writer errors."""


class _PartitionBlock(io.IOBase):
    """Minimal block-device wrapper around an ``esp32.Partition`` instance."""

    def __init__(self, device: Partition):
        self.device = device
        self.blocksize = int(device.ioctl(IOCTL_BLOCK_SIZE, None))
        self.blockcount = int(device.ioctl(IOCTL_BLOCK_COUNT, None))
        self.pos = 0
        self.end = 0

    def write(self, data: bytes | bytearray | memoryview) -> int:
        block, remainder = divmod(self.pos, self.blocksize)
        if remainder:
            raise OTAError("Unaligned block write detected.")
        data_len = len(data)
        nblocks, remainder = divmod(data_len, self.blocksize)
        mv = memoryview(data)
        if nblocks:
            self.device.writeblocks(block, mv[: nblocks * self.blocksize])
            block += nblocks
        if remainder:
            self.device.ioctl(IOCTL_BLOCK_ERASE, block)
            self.device.writeblocks(block, mv[-remainder:], 0)
        self.pos += data_len
        self.end = self.pos
        return data_len

    def readinto(self, data: bytearray | memoryview) -> int:
        size = min(len(data), self.end - self.pos)
        block, remainder = divmod(self.pos, self.blocksize)
        self.device.readblocks(block, memoryview(data)[:size], remainder)
        self.pos += size
        return size

    def seek(self, offset: int, whence: int = 0) -> None:
        start = [0, self.pos, self.end]
        self.pos = start[whence] + offset


def _sha_stream(stream, buffersize=4096) -> str:
    mv = memoryview(bytearray(buffersize))
    digest = hashlib.sha256()
    while (n := stream.readinto(mv)) > 0:
        digest.update(mv[:n])
    return digest.digest().hex()


class _PartitionWriter:
    """Buffered writer that enforces SHA and length constraints."""

    def __init__(self, partition: Partition, verify: bool, verbose: bool):
        self.device = _PartitionBlock(partition)
        self.writer = io.BufferedWriter(self.device, self.device.blocksize)
        self.verify = verify
        self.verbose = verbose
        self._sha = hashlib.sha256()
        self.expected_sha = ""
        self.expected_length = 0
        if verbose:
            blocks = self.device.blockcount
            blocksize = self.device.blocksize
            print(f"Device capacity: {blocks} x {blocksize} bytes")

    def set_expected(self, sha_hex: str, length: int) -> None:
        if length and length > self.device.blocksize * self.device.blockcount:
            raise OTAError("Firmware larger than OTA partition")
        self.expected_sha = sha_hex
        self.expected_length = length
        if self.verbose and length:
            blocks, rem = divmod(length, self.device.blocksize)
            print(f"Writing {blocks} blocks + {rem} bytes")

    def write(self, data: bytes | bytearray | memoryview) -> int:
        self._sha.update(data)
        return self.writer.write(data)

    def close(self) -> None:
        self.writer.flush()
        written = self.device.end
        if self.expected_length and written != self.expected_length:
            raise OTAError(
                f"Received {written} bytes but expected {self.expected_length}"
            )
        write_sha = self._sha.digest().hex()
        expected = self.expected_sha or write_sha
        if write_sha != expected:
            raise OTAError(f"SHA mismatch recv={write_sha} expect={expected}")
        if self.verify:
            if self.verbose:
                print("Verifying written image...", end="")
            self.device.seek(0)
            read_sha = _sha_stream(self.device, self.device.blocksize)
            if read_sha != write_sha:
                raise OTAError(
                    f"SHA verify failed write={write_sha} read={read_sha}"
                )
            if self.verbose:
                print("Passed.")
        self.device.seek(0)


class OTABinaryWriter:
    """High-level helper that writes firmware into the spare OTA partition."""

    def __init__(self, *, verify: bool = True, verbose: bool = True, auto_reboot=False):
        self.verify = verify
        self.verbose = verbose
        self.auto_reboot = auto_reboot
        self._writer: _PartitionWriter | None = None
        self._partition: Partition | None = None
        self._active = False
        self._received = 0
        self._expected_length = 0
        self._expected_sha = ""

    @property
    def active(self) -> bool:
        return self._active

    @property
    def received_bytes(self) -> int:
        return self._received

    @property
    def expected_bytes(self) -> int:
        return self._expected_length

    def begin(self, *, length: int = 0, sha_hex: str = "") -> None:
        if self._active:
            raise OTAError("OTA session already active")
        partition = Partition(Partition.RUNNING).get_next_update()
        self._partition = partition
        self._writer = _PartitionWriter(partition, self.verify, self.verbose)
        if length or sha_hex:
            self._writer.set_expected(sha_hex, length)
        self._expected_length = length
        self._expected_sha = sha_hex
        self._received = 0
        self._active = True
        if self.verbose:
            print(f"Writing new image to partition '{partition.info()[4]}'")

    def append(self, chunk: bytes | bytearray | memoryview) -> int:
        if not self._active or self._writer is None:
            raise OTAError("Call begin() before append()")
        written = self._writer.write(chunk)
        self._received += written
        return written

    def abort(self) -> None:
        if self.verbose and self._active:
            print("Aborting OTA session and discarding data")
        self._writer = None
        self._partition = None
        self._active = False
        self._received = 0
        self._expected_length = 0
        self._expected_sha = ""

    def finalize(self, *, reboot: bool = False, delay: int = 5) -> str:
        if not self._active or self._writer is None or self._partition is None:
            raise OTAError("No active OTA session to finalize")
        partition = self._partition
        try:
            self._writer.close()
            partition.set_boot()
            boot_name = Partition(Partition.BOOT).info()[4]
            if self.verbose:
                print(
                    f"Partition '{partition.info()[4]}' ready. Next boot: '{boot_name}'."
                )
        finally:
            self._active = False
            self._writer = None
        if reboot or self.auto_reboot:
            self.reboot(delay)
        return partition.info()[4]

    @staticmethod
    def reboot(delay: int = 5) -> None:
        for i in range(delay, 0, -1):
            print(f"\rRebooting in {i:2} seconds (ctrl-C to cancel)", end="")
            time.sleep(1)
        print()
        machine.reset()
