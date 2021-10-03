# Update Mboot or MicroPython from a .dfu.gz file on the board's filesystem
# MIT license; Copyright (c) 2019-2020 Damien P. George

from micropython import const
import struct, time
import uzlib, machine, stm

# Constants to be used with update_mpy
VFS_FAT = 1
VFS_LFS1 = 2
VFS_LFS2 = 3

# Constants for creating mboot elements.
_ELEM_TYPE_END = const(1)
_ELEM_TYPE_MOUNT = const(2)
_ELEM_TYPE_FSLOAD = const(3)
_ELEM_TYPE_STATUS = const(4)

FLASH_KEY1 = 0x45670123
FLASH_KEY2 = 0xCDEF89AB


def check_mem_contains(addr, buf):
    mem8 = stm.mem8
    r = range(len(buf))
    for off in r:
        if mem8[addr + off] != buf[off]:
            return False
    return True


def check_mem_erased(addr, size):
    mem16 = stm.mem16
    r = range(0, size, 2)
    for off in r:
        if mem16[addr + off] != 0xFFFF:
            return False
    return True


def dfu_read(filename):
    f = open(filename, "rb")

    hdr = f.read(3)
    f.seek(0)
    if hdr == b"Dfu":
        pass
    elif hdr == b"\x1f\x8b\x08":
        f = uzlib.DecompIO(f, 16 + 15)
    else:
        print("Invalid firmware", filename)
        return None

    elems = []

    hdr = f.read(11)
    sig, ver, size, num_targ = struct.unpack("<5sBIB", hdr)

    file_offset = 11

    for i in range(num_targ):
        hdr = f.read(274)
        sig, alt, has_name, name, t_size, num_elem = struct.unpack("<6sBi255sII", hdr)

        file_offset += 274
        file_offset_t = file_offset
        for j in range(num_elem):
            hdr = f.read(8)
            addr, e_size = struct.unpack("<II", hdr)
            data = f.read(e_size)
            elems.append((addr, data))
            file_offset += 8 + e_size

        if t_size != file_offset - file_offset_t:
            print("corrupt DFU", t_size, file_offset - file_offset_t)
            return None

    if size != file_offset:
        print("corrupt DFU", size, file_offset)
        return None

    hdr = f.read(16)
    hdr = struct.unpack("<HHHH3sBI", hdr)

    return elems


def flash_wait_not_busy():
    while stm.mem32[stm.FLASH + stm.FLASH_SR] & 1 << 16:
        machine.idle()


def flash_unlock():
    stm.mem32[stm.FLASH + stm.FLASH_KEYR] = FLASH_KEY1
    stm.mem32[stm.FLASH + stm.FLASH_KEYR] = FLASH_KEY2


def flash_lock():
    stm.mem32[stm.FLASH + stm.FLASH_CR] = 1 << 31  # LOCK


def flash_erase_sector(sector):
    assert 0 <= sector <= 7  # for F722
    flash_wait_not_busy()
    cr = 2 << 8 | sector << 3 | 1 << 1  # PSIZE = 32 bits  # SNB  # SER
    stm.mem32[stm.FLASH + stm.FLASH_CR] = cr
    stm.mem32[stm.FLASH + stm.FLASH_CR] = cr | 1 << 16  # STRT
    flash_wait_not_busy()
    stm.mem32[stm.FLASH + stm.FLASH_CR] = 0


def flash_write(addr, buf):
    assert len(buf) % 4 == 0
    flash_wait_not_busy()
    cr = 2 << 8 | 1 << 0  # PSIZE = 32 bits  # PG
    stm.mem32[stm.FLASH + stm.FLASH_CR] = cr
    for off in range(0, len(buf), 4):
        stm.mem32[addr + off] = struct.unpack_from("I", buf, off)[0]
        flash_wait_not_busy()
    stm.mem32[stm.FLASH + stm.FLASH_CR] = 0


def update_mboot(filename):
    print("Loading file", filename)

    mboot_fw = dfu_read(filename)
    if mboot_fw is None:
        return
    if len(mboot_fw) != 1:
        assert 0
    mboot_addr, mboot_fw = mboot_fw[0]
    if mboot_addr != 0x08000000:
        assert 0

    # TODO: Validate firmware in a simple way

    print("Found Mboot data with size %u." % len(mboot_fw))

    chk = check_mem_contains(mboot_addr, mboot_fw)
    if chk:
        print("Supplied version of Mboot is already on device.")
        return

    print("Programming Mboot, do not turn off!")
    time.sleep_ms(50)

    irq = machine.disable_irq()
    flash_unlock()
    flash_erase_sector(0)
    if len(mboot_fw) > 16 * 1024 and not check_mem_erased(mboot_addr + 16 * 1024, 16 * 1024):
        flash_erase_sector(1)
    flash_write(mboot_addr, mboot_fw)
    flash_lock()
    machine.enable_irq(irq)

    print("New Mboot programmed.")

    if check_mem_contains(mboot_addr, mboot_fw):
        print("Verification of new Mboot succeeded.")
    else:
        print("Verification of new Mboot FAILED!  Try rerunning.")

    print("Programming finished, can now reset or turn off.")


def _create_element(kind, body):
    return bytes([kind, len(body)]) + body


def update_mpy(filename, fs_base, fs_len, fs_type=VFS_FAT, fs_blocksize=0, status_addr=None):
    # Check firmware is of .dfu or .dfu.gz type
    try:
        with open(filename, "rb") as f:
            hdr = uzlib.DecompIO(f, 16 + 15).read(6)
    except Exception:
        with open(filename, "rb") as f:
            hdr = f.read(6)
    if hdr != b"DfuSe\x01":
        print("Firmware must be a .dfu(.gz) file.")
        return

    if fs_type in (VFS_LFS1, VFS_LFS2) and not fs_blocksize:
        raise Exception("littlefs requires fs_blocksize parameter")

    mount_point = 1
    elems = _create_element(
        _ELEM_TYPE_MOUNT,
        struct.pack("<BBLLL", mount_point, fs_type, fs_base, fs_len, fs_blocksize),
    )
    elems += _create_element(
        _ELEM_TYPE_FSLOAD, struct.pack("<B", mount_point) + bytes(filename, "ascii")
    )
    if status_addr is not None:
        # mboot will write 0 to status_addr on succes, or a negative number on failure
        machine.mem32[status_addr] = 1
        elems += _create_element(_ELEM_TYPE_STATUS, struct.pack("<L", status_addr))
    elems += _create_element(_ELEM_TYPE_END, b"")
    machine.bootloader(elems)
