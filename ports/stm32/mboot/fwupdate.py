# Update Mboot or MicroPython from a .dfu.gz file on the board's filesystem
# MIT license; Copyright (c) 2019-2022 Damien P. George

from micropython import const
import struct, time
import deflate, machine, stm

# Constants to be used with update_mpy
VFS_FAT = 1
VFS_LFS1 = 2
VFS_LFS2 = 3
VFS_RAW = 4

# Constants for creating mboot elements.
_ELEM_TYPE_END = const(1)
_ELEM_TYPE_MOUNT = const(2)
_ELEM_TYPE_FSLOAD = const(3)
_ELEM_TYPE_STATUS = const(4)


def check_mem_contains(addr, buf):
    mem8 = stm.mem8
    r = range(len(buf))
    for off in r:
        if mem8[addr + off] != buf[off]:
            return False
    return True


def dfu_read(filename):
    from binascii import crc32

    f = open(filename, "rb")

    hdr = f.read(3)
    f.seek(0)
    if hdr == b"Dfu":
        pass
    elif hdr == b"\x1f\x8b\x08":
        f = deflate.DeflateIO(f, deflate.GZIP)
    else:
        print("Invalid firmware", filename)
        return None

    crc = 0
    elems = []

    hdr = f.read(11)
    crc = crc32(hdr, crc)
    sig, ver, size, num_targ = struct.unpack("<5sBIB", hdr)

    file_offset = 11

    for i in range(num_targ):
        hdr = f.read(274)
        crc = crc32(hdr, crc)
        sig, alt, has_name, name, t_size, num_elem = struct.unpack("<6sBi255sII", hdr)

        file_offset += 274
        file_offset_t = file_offset
        for j in range(num_elem):
            hdr = f.read(8)
            crc = crc32(hdr, crc)
            addr, e_size = struct.unpack("<II", hdr)
            data = f.read(e_size)
            crc = crc32(data, crc)
            elems.append((addr, data))
            file_offset += 8 + e_size

        if t_size != file_offset - file_offset_t:
            print("corrupt DFU", t_size, file_offset - file_offset_t)
            return None

    if size != file_offset:
        print("corrupt DFU", size, file_offset)
        return None

    hdr = f.read(16)
    crc = crc32(hdr, crc)
    crc = ~crc & 0xFFFFFFFF
    if crc != 0:
        print("CRC failed", crc)
        return None

    return elems


class Flash:
    _FLASH_KEY1 = 0x45670123
    _FLASH_KEY2 = 0xCDEF89AB

    def __init__(self):
        import os, uctypes

        self.addressof = uctypes.addressof

        # Detect MCU.
        machine = os.uname().machine
        if "STM32F4" in machine or "STM32F7" in machine:
            dev_id = stm.mem32[0xE004_2000] & 0xFFF
        elif "STM32H7" in machine:
            dev_id = stm.mem32[0x5C00_1000] & 0xFFF
        else:
            dev_id = 0

        # Configure flash parameters based on MCU.
        if dev_id in (0x413, 0x419, 0x431, 0x434, 0x451, 0x452):
            # 0x413: STM32F405/407, STM32F415/417
            # 0x419: STM32F42x/43x
            # 0x431: STM32F411
            # 0x434: STM32F469/479
            # 0x451: STM32F76x/77x
            # 0x452: STM32F72x/73x
            self._keyr = stm.FLASH + stm.FLASH_KEYR
            self._sr = stm.FLASH + stm.FLASH_SR
            self._sr_busy = 1 << 16
            self._cr = stm.FLASH + stm.FLASH_CR
            self._cr_lock = 1 << 31
            self._cr_init_erase = lambda s: 2 << 8 | s << 3 | 1 << 1  # PSIZE=32-bits, SNB, SER
            self._cr_start_erase = 1 << 16  # STRT
            self._cr_init_write = 2 << 8 | 1 << 0  # PSIZE=32-bits, PG
            self._cr_flush = None
            self._write_multiple = 4
            if dev_id == 0x451 and stm.mem32[0x1FFF_0008] & 1 << 13:  # check nDBANK
                # STM32F76x/77x in single-bank mode
                self.sector0_size = 32 * 1024
            else:
                self.sector0_size = 16 * 1024
        elif dev_id == 0x450:
            # 0x450: STM32H742, STM32H743/753, STM32H750
            self._keyr = stm.FLASH + stm.FLASH_KEYR1
            self._sr = stm.FLASH + stm.FLASH_SR1
            self._sr_busy = 1 << 2  # QW1
            self._cr = stm.FLASH + stm.FLASH_CR1
            self._cr_lock = 1 << 0  # LOCK1
            self._cr_init_erase = lambda s: s << 8 | 3 << 4 | 1 << 2  # SNB1, PSIZE1=64-bits, SER1
            self._cr_start_erase = 1 << 7  # START1
            self._cr_init_write = 3 << 4 | 1 << 1  # PSIZE1=64-bits, PG1=1
            self._cr_flush = 1 << 6  # FW1=1
            self._write_multiple = 16
            self.sector0_size = 128 * 1024
        else:
            raise Exception(f"unknown MCU {machine} DEV_ID=0x{dev_id:x}")

    def wait_not_busy(self):
        while stm.mem32[self._sr] & self._sr_busy:
            machine.idle()

    def unlock(self):
        if stm.mem32[self._cr] & self._cr_lock:
            stm.mem32[self._keyr] = self._FLASH_KEY1
            stm.mem32[self._keyr] = self._FLASH_KEY2

    def lock(self):
        stm.mem32[self._cr] = self._cr_lock

    def erase_sector(self, sector):
        self.wait_not_busy()
        stm.mem32[self._cr] = self._cr_init_erase(sector)
        stm.mem32[self._cr] |= self._cr_start_erase
        self.wait_not_busy()
        stm.mem32[self._cr] = 0

    # This method is optimised for speed, to reduce the time data is being written.
    def write(self, addr, buf):
        assert len(buf) % 4 == 0
        mem32 = stm.mem32
        buf_addr = self.addressof(buf)
        r = range(0, len(buf), 4)
        self.wait_not_busy()
        mem32[self._cr] = self._cr_init_write
        for off in r:
            mem32[addr + off] = mem32[buf_addr + off]
            if off % self._write_multiple == 0:
                while mem32[self._sr] & self._sr_busy:
                    pass
        if self._cr_flush is not None:
            mem32[self._cr] |= self._cr_flush
            self.wait_not_busy()
        mem32[self._cr] = 0


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

    print("Found Mboot data with size %u." % len(mboot_fw))

    chk = check_mem_contains(mboot_addr, mboot_fw)
    if chk:
        print("Supplied version of Mboot is already on device.")
        return

    print("Programming Mboot, do not turn off!")
    time.sleep_ms(50)

    flash = Flash()
    irq = machine.disable_irq()
    flash.unlock()
    flash.erase_sector(0)
    if len(mboot_fw) > flash.sector0_size:
        flash.erase_sector(1)
    flash.write(mboot_addr, mboot_fw)
    flash.lock()
    machine.enable_irq(irq)

    print("New Mboot programmed.")

    if check_mem_contains(mboot_addr, mboot_fw):
        print("Verification of new Mboot succeeded.")
    else:
        print("Verification of new Mboot FAILED!  Try rerunning.")

    print("Programming finished, can now reset or turn off.")


def _create_element(kind, body):
    return bytes([kind, len(body)]) + body


def update_app_elements(
    filename,
    fs_base,
    fs_len,
    fs_type=VFS_FAT,
    fs_blocksize=0,
    status_addr=None,
    addr_64bit=False,
    *,
    fs_base2=0,
    fs_len2=0,
):
    if fs_type != VFS_RAW:
        # Check firmware is of .dfu or .dfu.gz type
        try:
            with open(filename, "rb") as f:
                hdr = deflate.DeflateIO(f, deflate.GZIP).read(6)
        except Exception:
            with open(filename, "rb") as f:
                hdr = f.read(6)
        if hdr != b"DfuSe\x01":
            print("Firmware must be a .dfu(.gz) file.")
            return ()

    if fs_type in (VFS_LFS1, VFS_LFS2) and not fs_blocksize:
        raise Exception("littlefs requires fs_blocksize parameter")

    mount_point = 1
    if fs_type == VFS_RAW:
        mount_encoding = "<BBQQQQ" if addr_64bit else "<BBLLLL"
        elems = _create_element(
            _ELEM_TYPE_MOUNT,
            struct.pack(mount_encoding, mount_point, fs_type, fs_base, fs_len, fs_base2, fs_len2),
        )
    else:
        mount_encoding = "<BBQQL" if addr_64bit else "<BBLLL"
        elems = _create_element(
            _ELEM_TYPE_MOUNT,
            struct.pack(mount_encoding, mount_point, fs_type, fs_base, fs_len, fs_blocksize),
        )
    elems += _create_element(
        _ELEM_TYPE_FSLOAD, struct.pack("<B", mount_point) + bytes(filename, "ascii")
    )
    if status_addr is not None:
        # mboot will write 0 to status_addr on success, or a negative number on failure
        machine.mem32[status_addr] = 1
        elems += _create_element(_ELEM_TYPE_STATUS, struct.pack("<L", status_addr))
    elems += _create_element(_ELEM_TYPE_END, b"")
    return elems


def update_mpy(*args, **kwargs):
    elems = update_app_elements(*args, **kwargs)
    if elems:
        machine.bootloader(elems)
