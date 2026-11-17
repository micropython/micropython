# MIT license; Copyright (c) 2024 OpenMV LLC.
#
# Note: this test completely erases the filesystem!
# It is intended to be run manually.

import alif
import hashlib
import os
import sys
import time
import vfs

hash_algo = "sha256"


def flash_make_filesystem():
    try:
        vfs.umount("/flash")
    except:
        pass
    bdev = alif.Flash()
    vfs.VfsFat.mkfs(bdev)
    vfs.mount(vfs.VfsFat(bdev), "/flash")
    sys.path.append("/flash")
    sys.path.append("/flash/lib")
    os.chdir("/flash")


def flash_block_test():
    try:
        vfs.umount("/flash")
    except:
        pass
    dev = alif.Flash()

    data512 = os.urandom(512)
    buf512 = bytearray(512)
    block_numbers = tuple(range(32)) + tuple(range(250, 266))

    print("Block read/write integrity: ", end="")
    ok = True
    for block_n in block_numbers:
        dev.writeblocks(block_n, data512)
        dev.readblocks(block_n, buf512)
        if buf512 != data512:
            ok = False
    print(ok)

    print("Block read back integrity: ", end="")
    ok = True
    for block_n in block_numbers:
        dev.readblocks(block_n, buf512)
        if buf512 != data512:
            ok = False
    print(ok)

    N = 16 * 1024
    data_big = os.urandom(N)
    t0 = time.ticks_us()
    dev.writeblocks(0, data_big)
    dt = time.ticks_diff(time.ticks_us(), t0)
    print(f"Block write speed: {len(data_big) / 1024 / dt * 1_000_000} KiB/sec")

    buf_big = bytearray(N)
    t0 = time.ticks_us()
    dev.readblocks(0, buf_big)
    dt = time.ticks_diff(time.ticks_us(), t0)
    print(f"Block read speed:  {len(buf_big) / 1024 / dt * 1_000_000} KiB/sec")

    if buf_big != data_big:
        raise RuntimeError("big block read-back failed")

    try:
        import uctypes

        xip = memoryview(dev)
    except:
        xip = None
    if xip is not None:
        t0 = time.ticks_us()
        buf_big[:] = xip[: len(buf_big)]
        dt = time.ticks_diff(time.ticks_us(), t0)
        print(f"XIP read speed:    {len(buf_big) / 1024 / dt * 1_000_000} KiB/sec")

        if buf_big != data_big:
            raise RuntimeError("XIP read-back failed")
        for i in range(len(buf_big)):
            if xip[i] != data_big[i]:
                raise RuntimeError("XIP byte-wise read-back failed")


def flash_write_verify(path, size=1024 * 1024, block_size=1024):
    hash_sum = getattr(hashlib, hash_algo)()
    block_count = size // block_size

    print("-" * 16)
    print(f"Writing file {size=} {block_size=}")
    t0 = time.ticks_ms()
    total_size = 0
    with open(path, "wb") as file:
        for i in range(block_count):
            buf = os.urandom(block_size)
            # Update digest
            hash_sum.update(buf)
            total_size += file.write(buf)
            if i % (block_count // 16) == 0:
                print(f"{i}/{block_count}", end="\r")
    dt = time.ticks_diff(time.ticks_ms(), t0)
    print(f"Flash write finished:       {total_size / 1024 / dt * 1000} KiB/sec")
    digest = hash_sum.digest()

    print("Reading file... ", end="")
    hash_sum = getattr(hashlib, hash_algo)()
    buf = bytearray(block_size)
    t0 = time.ticks_ms()
    total_size = 0
    with open(path, "rb") as file:
        for i in range(block_count):
            total_size += file.readinto(buf)
    dt = time.ticks_diff(time.ticks_ms(), t0)
    print(f"finished:   {total_size / 1024 / dt * 1000} KiB/sec")

    print("Verifying file... ", end="")
    hash_sum = getattr(hashlib, hash_algo)()
    t0 = time.ticks_ms()
    total_size = 0
    with open(path, "rb") as file:
        for i in range(block_count):
            buf = file.read(block_size)
            total_size += len(buf)
            # Update digest
            hash_sum.update(buf)
    dt = time.ticks_diff(time.ticks_ms(), t0)
    print(f"finished: {total_size / 1024 / dt * 1000} KiB/sec; ", end="")

    if digest != hash_sum.digest():
        raise RuntimeError(f"{hash_algo} checksum verify failed")

    print(f"{hash_algo} checksum verified")


if __name__ == "__main__":
    flash_block_test()
    flash_make_filesystem()
    flash_write_verify("test0.bin", size=64 * 1024, block_size=1024)
    flash_write_verify("test1.bin", size=128 * 1024, block_size=1024)
    flash_write_verify("test2.bin", size=64 * 1024, block_size=2048)
    flash_write_verify("test4.bin", size=256 * 1024, block_size=4096)
    flash_write_verify("test4.bin", size=512 * 1024, block_size=16384)
