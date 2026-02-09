# partition_writer module for MicroPython on ESP32
# MIT license; Copyright (c) 2023 Glenn Moloney @glenn20

# Based on OTA class by Thorsten von Eicken (@tve):
#   https://github.com/tve/mqboard/blob/master/mqrepl/mqrepl.py

import hashlib
import io

from micropython import const

IOCTL_BLOCK_COUNT: int = const(4)  # type: ignore
IOCTL_BLOCK_SIZE: int = const(5)  # type: ignore
IOCTL_BLOCK_ERASE: int = const(6)  # type: ignore


# An IOBase compatible class to wrap access to an os.AbstractBlockdev() device
# such as a partition on the device flash. Writes must be aligned to block
# boundaries.
# https://docs.micropython.org/en/latest/library/os.html#block-device-interface
# Extend IOBase so we can wrap this with io.BufferedWriter in BlockdevWriter
class Blockdev(io.IOBase):
    def __init__(self, device):
        self.device = device
        self.blocksize = int(device.ioctl(IOCTL_BLOCK_SIZE, None))
        self.blockcount = int(device.ioctl(IOCTL_BLOCK_COUNT, None))
        self.pos = 0  # Current position (bytes from beginning) of device
        self.end = 0  # Current end of the data written to the device

    # Data must be a multiple of blocksize unless it is the last write to the
    # device. The next write after a partial block will raise ValueError.
    def write(self, data: bytes | bytearray | memoryview) -> int:
        block, remainder = divmod(self.pos, self.blocksize)
        if remainder:
            raise ValueError(f"Block {block} write not aligned at block boundary.")
        data_len = len(data)
        nblocks, remainder = divmod(data_len, self.blocksize)
        mv = memoryview(data)
        if nblocks:  # Write whole blocks
            self.device.writeblocks(block, mv[: nblocks * self.blocksize])
            block += nblocks
        if remainder:  # Write left over data as a partial block
            self.device.ioctl(IOCTL_BLOCK_ERASE, block)  # Erase block first
            self.device.writeblocks(block, mv[-remainder:], 0)
        self.pos += data_len
        self.end = self.pos  # The "end" of the data written to the device
        return data_len

    # Read data from the block device.
    def readinto(self, data: bytearray | memoryview):
        size = min(len(data), self.end - self.pos)
        block, remainder = divmod(self.pos, self.blocksize)
        self.device.readblocks(block, memoryview(data)[:size], remainder)
        self.pos += size
        return size

    # Set the current file position for reading or writing
    def seek(self, offset: int, whence: int = 0):
        start = [0, self.pos, self.end]
        self.pos = start[whence] + offset


# Calculate the SHA256 sum of a file (has a readinto() method)
def sha_file(f, buffersize=4096) -> str:
    mv = memoryview(bytearray(buffersize))
    read_sha = hashlib.sha256()
    while (n := f.readinto(mv)) > 0:
        read_sha.update(mv[:n])
    return read_sha.digest().hex()


# BlockdevWriter provides a convenient interface to writing images to any block
# device which implements the micropython os.AbstractBlockDev interface (eg.
# Partition on flash storage on ESP32).
# https://docs.micropython.org/en/latest/library/os.html#block-device-interface
# https://docs.micropython.org/en/latest/library/esp32.html#flash-partitions
class BlockDevWriter:
    def __init__(
        self,
        device,  # Block device to recieve the data (eg. esp32.Partition)
        verify: bool = True,  # Should we read back and verify data after writing
        verbose: bool = True,
    ):
        self.device = Blockdev(device)
        self.writer = io.BufferedWriter(
            self.device, self.device.blocksize  # type: ignore
        )
        self._sha = hashlib.sha256()
        self.verify = verify
        self.verbose = verbose
        self.sha: str = ""
        self.length: int = 0
        blocksize, blockcount = self.device.blocksize, self.device.blockcount
        if self.verbose:
            print(f"Device capacity: {blockcount} x {blocksize} byte blocks.")

    def set_sha_length(self, sha: str, length: int):
        self.sha = sha
        self.length = length
        blocksize, blockcount = self.device.blocksize, self.device.blockcount
        if length > blocksize * blockcount:
            raise ValueError(f"length ({length} bytes) is > size of partition.")
        if self.verbose and length:
            blocks, remainder = divmod(length, blocksize)
            print(f"Writing {blocks} blocks + {remainder} bytes.")

    def print_progress(self):
        if self.verbose:
            block, remainder = divmod(self.device.pos, self.device.blocksize)
            print(f"\rBLOCK {block}", end="")
            if remainder:
                print(f" + {remainder} bytes")

    # Append data to the block device
    def write(self, data: bytearray | bytes | memoryview) -> int:
        self._sha.update(data)
        n = self.writer.write(data)
        self.print_progress()
        return n

    # Append data from f (a stream object) to the block device
    def write_from_stream(self, f: io.BufferedReader) -> int:
        mv = memoryview(bytearray(self.device.blocksize))
        tot = 0
        while (n := f.readinto(mv)) != 0:
            tot += self.write(mv[:n])
        return tot

    # Flush remaining data to the block device and confirm all checksums
    # Raises:
    #   ValueError("SHA mismatch...") if SHA of received data != expected sha
    #   ValueError("SHA verify fail...") if verified SHA != written sha
    def close(self) -> None:
        self.writer.flush()
        self.print_progress()
        # Check the checksums (SHA256)
        nbytes: int = self.device.end
        if self.length and self.length != nbytes:
            raise ValueError(f"Received {nbytes} bytes (expect {self.length}).")
        write_sha = self._sha.digest().hex()
        if not self.sha:
            self.sha = write_sha
        if self.sha != write_sha:
            raise ValueError(f"SHA mismatch recv={write_sha} expect={self.sha}.")
        if self.verify:
            if self.verbose:
                print("Verifying SHA of the written data...", end="")
            self.device.seek(0)  # Reset to start of partition
            read_sha = sha_file(self.device, self.device.blocksize)
            if read_sha != write_sha:
                raise ValueError(f"SHA verify failed write={write_sha} read={read_sha}")
            if self.verbose:
                print("Passed.")
        if self.verbose or not self.sha:
            print(f"SHA256={self.sha}")
        self.device.seek(0)  # Reset to start of partition

    def __enter__(self):
        return self

    def __exit__(self, e_t, e_v, e_tr):
        if e_t is None:
            self.close()
