# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2020 Damien P. George
# Copyright (c) 2020 Jim Mussared
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# This script provides helpers for working with the FUS/WS firmware on the WB55.
# It can be frozen into the MicroPython firmware (via manifest.py)
#
# The current FUS and WS firmware version and state can be queried via the
# `stm` module, e.g.
# stm.rfcore_status()  (returns the first word of the device info table)
# stm.rfcore_fw_version(id)  (returns a 5-tuple indicating fw version; id is: 0=FUS, 1=WS)
# stm.rfcore_sys_hci(ogf, ocf, cmd_buf)  (synchronously execute HCI command on SYS channel)
#
# To perform a firmware update:
#
# 1. Generate "obfuscated" binary images using rfcore_makefirmware.py, eg.
#  $ python3 ./boards/NUCLEO_WB55/rfcore_makefirmware.py ~/src/github.com/STMicroelectronics/STM32CubeWB/Projects/STM32WB_Copro_Wireless_Binaries/STM32WB5x/ /tmp
#  This will generate /tmp/{fus_102,fus_110,ws_ble_hci}.bin
#  It may warn that stm32wb5x_FUS_fw_1_0_2.bin cannot be found, newer packs don't include this
#  which can be ignored unless your currently flashed FUS is older than 1.0.2
#
# 2. Copy required files to the device filesystem.
#  $ mpremote cp /tmp/fus_102.bin :
#  $ mpremote cp /tmp/fus_110.bin :
#  $ mpremote cp /tmp/ws_ble_hci.bin :
#  $ mpremote cp ./boards/NUCLEO_WB55/rfcore_firmware.py :
#  In general, it's always safe to copy all three files and the updater will
#  figure out what needs to be done. This is the recommended option.
#  However, if you already have the latest FUS (1.1.0) installed, then just the
#  WS firmware is required.
#  If a FUS binary is present, then the existing WS will be removed so it's a good
#  idea to always include the WS binary if updating FUS.
#  Note that a WS binary will not be installed unless FUS 1.1.0 is installed.
#
# 3. Ensure boot.py calls `rfcore_firmware.resume()`.
#  $ mpremote exec "import rfcore_firmware; rfcore_firmware.install_boot()"
#  The WB55 will reset several times during the firmware update process, so this
#  script manages the update state using RTC backup registers.
#  `rfcore_firmware.resume()` will continue the update operation on startup to
#  resume any in-progress update operation, and either trigger another reset, or
#  return 0 to indicate that the operation completed successfully, or a reason
#  code (see REASON_* below) to indicate failure.
#
# 4. Call rfcore_firmware.check_for_updates() to start the update process.
#  $ mpremote exec "import rfcore_firmware; rfcore_firmware.check_for_updates()"
#  The device will then immediately reboot and when the firmware update completes,
#  the status will be returned from rfcore_firmware.resume(). See the REASON_ codes below.
#  You can use the built-in stm.rfcore_fw_version() to query the installed version
#  from your application code.

import struct, os

try:
    import machine, stm
    from ubinascii import crc32
    from micropython import const
except ImportError:
    # cpython
    from binascii import crc32

    machine = stm = None
    const = lambda x: x


_OGF_VENDOR = const(0x3F)

_OCF_FUS_GET_STATE = const(0x52)
_OCF_FUS_FW_UPGRADE = const(0x54)
_OCF_FUS_FW_DELETE = const(0x55)
_OCF_FUS_START_WS = const(0x5A)
_OCF_BLE_INIT = const(0x66)

_HCI_KIND_VENDOR_RESPONSE = const(0x11)


# The firmware updater will search all of flash for the image to install, so
# it's important that the file doesn't exist anywhere on the filesystem and
# that the updater only finds the version that we copy into the reserved area.
# Otherwise it will find matching headers/footers in the flash filesystem and
# get confused leading to either "FUS_STATE_IMG_NOT_AUTHENTIC" or (worse)
# corrupting the FUS.
# See footnote [1] referenced by Table 9 in AN5185 - Rev 4 -- the address
# passed to FUS_FW_UPGRADE is ignored (implying that it must be searching the
# flash). This requires that the firmware files have been pre-processed by
# rfcore_makefirmware.py and this key must match the one there.
_OBFUSCATION_KEY = const(0x0573B55AA)

# On boards using the internal flash filesystem, this must match the
# `_micropy_hw_internal_flash_storage_end` symbol defined by the linker script
# (see eg boards/stm32wb55xg.ld).
# We erase everything from here until the start of the secure area (defined by
# SFSA) just to ensure that no other fragments of firmware files are left
# behind. On boards with external flash, this just needs to ensure that it
# includes any regions that may contain partial firmware data.
# This is non-const so it can be override.
STAGING_AREA_START = 0x80C0000

# First word of device info table indicating FUS state (returned by `stm.rfcore_status()`).
_MAGIC_FUS_ACTIVE = const(0xA94656B9)  # AN5185
_MAGIC_IPCC_MEM_INCORRECT = const(0x3DE96F61)  # # AN5185

# Argument to `stm.rfcore_fw_version()`.
_FW_VERSION_FUS = const(0)
_FW_VERSION_WS = const(1)

# No firmware update in progress. Boot normally.
_STATE_IDLE = const(0)

# A previous firmware update failed. Will return reason code from resume().
_STATE_FAILED = const(1)

# Trying to get into the FUS. Keep issuing GET_STATE until the FUS is active.
_STATE_WAITING_FOR_FUS = const(2)

# Trying to get into the WS. Keep issuing START_WS until the WS is active (or fails).
_STATE_WAITING_FOR_WS = const(3)

# FW_DELETE has been issued. Waiting for the WS version to report zero.
_STATE_DELETING_WS = const(4)

# Flash copy has started for FUS/WS. If a reboot occurs, then fail.
_STATE_COPYING_FUS = const(5)
_STATE_COPYING_WS = const(6)

# Flash write fully completed, ready for install.
_STATE_COPIED_FUS = const(7)
_STATE_COPIED_WS = const(8)

# Check for next update to perform.
# Either we've just gotten into the FUS, or the first update in a sequence
# has completed. (e.g. FUS done, now do WS).
_STATE_CHECK_UPDATES = const(9)

# Installation has started, keep polling GET_STATE.
_STATE_INSTALLING_WS = const(10)
_STATE_INSTALLING_FUS = const(11)

# Update completed successfully.
REASON_OK = const(0)
# The device reset during flash copy. Possibly WS still installed.
REASON_FLASH_COPY_FAILED = const(1)
# Unable to start the WS after firmware update.
REASON_NO_WS = const(2)
# Copying FUS image to staging area caused FUS to fail.
REASON_FLASH_FUS_BAD_STATE = const(3)
# Copying WS image to staging area caused FUS to fail.
REASON_FLASH_WS_BAD_STATE = const(4)
# Cannot get into the FUS. Perhaps rfcore misconfigured.
REASON_FUS_NOT_RESPONDING = const(5)
# After a FUS install, unable to get back to the FUS.
REASON_FUS_NOT_RESPONDING_AFTER_FUS = const(6)
# After a WS install, unable to get back to the FUS.
REASON_FUS_NOT_RESPONDING_AFTER_WS = const(7)
# Unable to query rfcore version/active.
REASON_RFCORE_NOT_CONFIGURED = const(8)
# The WS deletion didn't have any effect.
REASON_WS_STILL_PRESENT = const(9)
# FUS refused to delete the WS.
REASON_WS_DELETION_FAILED = const(10)
# FUS returned a specific code for a FUS update.
# See AN5185 Rev 4, Table 12. Reason between 0x00-0x11 will be added.
REASON_FUS_VENDOR = const(0x10)
# FUS returned a specific code for a WS update. Values as for the FUS update.
REASON_WS_VENDOR = const(0x30)

# FUS 1.0.2 must be installed before FUS 1.1.0 can be installed.
# A factory Nucleo board has FUS (0, 5, 3, 0, 0) and WS (0, 5, 1, 0, 0).
_FUS_VERSION_102 = (1, 0, 2, 0, 0)
_FUS_VERSION_110 = (1, 1, 0, 0, 0)
_PATH_FUS_102 = "fus_102.bin"
_PATH_FUS_110 = "fus_110.bin"
_PATH_WS_BLE_HCI = "ws_ble_hci.bin"

# When installing the FUS/WS it can take a long time to return to the first
# GET_STATE HCI command.
# e.g. Installing stm32wb5x_BLE_Stack_full_fw.bin takes 3600ms to respond.
_INSTALLING_FUS_GET_STATE_TIMEOUT = const(1000)
_INSTALLING_WS_GET_STATE_TIMEOUT = const(6000)


def log(msg, *args, **kwargs):
    print("[rfcore update]", msg.format(*args, **kwargs))


class _Flash:
    _FLASH_KEY1 = 0x45670123
    _FLASH_KEY2 = 0xCDEF89AB

    _FLASH_CR_STRT_MASK = 1 << 16
    _FLASH_CR_LOCK_MASK = 1 << 31
    _FLASH_SR_BSY_MASK = 1 << 16

    def wait_not_busy(self):
        while machine.mem32[stm.FLASH + stm.FLASH_SR] & _Flash._FLASH_SR_BSY_MASK:
            machine.idle()

    def unlock(self):
        if machine.mem32[stm.FLASH + stm.FLASH_CR] & _Flash._FLASH_CR_LOCK_MASK:
            # Only unlock if already locked (i.e. FLASH_CR_LOCK is set).
            machine.mem32[stm.FLASH + stm.FLASH_KEYR] = _Flash._FLASH_KEY1
            machine.mem32[stm.FLASH + stm.FLASH_KEYR] = _Flash._FLASH_KEY2
        else:
            log("Flash was already unlocked.")

    def lock(self):
        machine.mem32[stm.FLASH + stm.FLASH_CR] = _Flash._FLASH_CR_LOCK_MASK

    def erase_page(self, page):
        assert 0 <= page <= 255  # 1MiB range (4k page)
        self.wait_not_busy()
        cr = page << 3 | 1 << 1  # PNB  # PER
        machine.mem32[stm.FLASH + stm.FLASH_CR] = cr
        machine.mem32[stm.FLASH + stm.FLASH_CR] = cr | _Flash._FLASH_CR_STRT_MASK
        self.wait_not_busy()
        machine.mem32[stm.FLASH + stm.FLASH_CR] = 0

    def write(self, addr, buf, sz, key=0):
        assert sz % 4 == 0
        self.wait_not_busy()
        cr = 1 << 0  # PG
        machine.mem32[stm.FLASH + stm.FLASH_CR] = cr
        off = 0
        while off < sz:
            v = (buf[off]) | (buf[off + 1] << 8) | (buf[off + 2] << 16) | (buf[off + 3] << 24)
            machine.mem32[addr + off] = v ^ key
            off += 4
            if off % 8 == 0:
                self.wait_not_busy()
        if off % 8:
            machine.mem32[addr + off] = 0
            self.wait_not_busy()
        machine.mem32[stm.FLASH + stm.FLASH_CR] = 0


def validate_crc(f):
    """Should match copy of function in rfcore_makefirmware.py to confirm operation"""
    f.seek(0)
    file_crc = 0
    chunk = 16 * 1024
    buff = bytearray(chunk)
    while True:
        read = f.readinto(buff)
        if read < chunk:
            file_crc = crc32(buff[0:read], file_crc)
            break
        file_crc = crc32(buff, file_crc)

    file_crc = 0xFFFFFFFF & -file_crc - 1
    f.seek(0)
    return file_crc == 0


def check_file_details(filename):
    with open(filename, "rb") as f:
        if not validate_crc(f):
            raise ValueError("file validation failed: incorrect crc")

        # Check the footer on the file
        f.seek(-64, 2)
        footer = f.read()
        details = struct.unpack("<37sIIIIbbbII", footer)
        (
            src_filename,
            addr_1m,
            addr_640k,
            addr_512k,
            addr_256k,
            vers_major,
            vers_minor,
            vers_patch,
            KEY,
            crc,
        ) = details
        src_filename = src_filename.strip(b"\x00").decode()
        if KEY != _OBFUSCATION_KEY:
            raise ValueError("file validation failed: incorrect key")

    return (
        src_filename,
        addr_1m,
        addr_640k,
        addr_512k,
        addr_256k,
        (vers_major, vers_minor, vers_patch),
    )


def _copy_file_to_flash(filename):
    flash = _Flash()
    flash.unlock()
    # Reset any previously stored address
    _write_target_addr(0)
    try:
        (
            src_filename,
            addr_1m,
            addr_640k,
            addr_512k,
            addr_256k,
            vers,
        ) = check_file_details(filename)

        # TODO add support for querying the correct flash size on chip
        addr = load_addr = addr_1m

        log(f"Writing {src_filename} v{vers[0]}.{vers[1]}.{vers[2]} to addr: 0x{addr:x}")

        # Erase the entire staging area in flash.
        erase_addr = STAGING_AREA_START
        sfr_sfsa = machine.mem32[stm.FLASH + stm.FLASH_SFR] & 0xFF
        erase_limit = 0x08000000 + sfr_sfsa * 4096
        while erase_addr < erase_limit:
            flash.erase_page((erase_addr - 0x08000000) // 4096)
            erase_addr += 4096

        # Write the contents of the firmware (note flash.write will apply the
        # XOR de-obfuscation).
        with open(filename, "rb") as f:
            buf = bytearray(4096)

            while 1:
                sz = f.readinto(buf)
                if sz == 0:
                    break
                flash.write(addr, buf, sz, _OBFUSCATION_KEY)
                addr += 4096

        # Cache the intended target load address
        _write_target_addr(load_addr)

    finally:
        flash.lock()


def _parse_vendor_response(data):
    assert len(data) >= 7
    assert data[0] == _HCI_KIND_VENDOR_RESPONSE
    assert data[1] == 0x0E
    # assert data[3] == 0xff # "Num HCI" -- docs say 0xff, but we see 0x01
    op = (data[5] << 8) | data[4]
    return (op >> 10, op & 0x3FF, data[6], data[7] if len(data) > 7 else 0)


def _run_sys_hci_cmd(ogf, ocf, buf=b"", timeout=0):
    try:
        ogf_out, ocf_out, status, result = _parse_vendor_response(
            stm.rfcore_sys_hci(ogf, ocf, buf, timeout)
        )
    except OSError:
        # Timeout or FUS not active.
        return (0xFF, 0xFF)
    assert ogf_out == ogf
    assert ocf_out == ocf
    return (status, result)


def fus_get_state(timeout=0):
    return _run_sys_hci_cmd(_OGF_VENDOR, _OCF_FUS_GET_STATE, timeout=timeout)


def fus_is_idle():
    return fus_get_state() == (0, 0)


def fus_start_ws():
    return _run_sys_hci_cmd(_OGF_VENDOR, _OCF_FUS_START_WS)


def _fus_fwdelete():
    return _run_sys_hci_cmd(_OGF_VENDOR, _OCF_FUS_FW_DELETE)


def _fus_run_fwupgrade():
    # Note: Address is ignored by the FUS (see comments above).
    addr = _read_target_addr()
    if not addr:
        log(f"Update failed: Invalid load address: 0x{addr:x}")
        return False

    log(f"Loading to: 0x{addr:x}")
    return _run_sys_hci_cmd(_OGF_VENDOR, _OCF_FUS_FW_UPGRADE, struct.pack("<I", addr))


if stm:
    # Get/set current state/reason/addr to RTC Backup Domain.
    # Using the second, third, and fourth-last registers (16, 17, 18) as
    # the final one (19) is reserved by powerctrl.c for restoring the frequency.
    # Can be overridden if necessary.
    REG_RTC_STATE = stm.RTC + stm.RTC_BKP18R
    REG_RTC_REASON = stm.RTC + stm.RTC_BKP17R
    REG_RTC_ADDR = stm.RTC + stm.RTC_BKP16R


def _read_state():
    return machine.mem32[REG_RTC_STATE]


def _write_state(state):
    machine.mem32[REG_RTC_STATE] = state


def _read_failure_reason():
    return machine.mem32[REG_RTC_REASON]


def _write_failure_state(reason):
    machine.mem32[REG_RTC_REASON] = reason
    _write_state(_STATE_FAILED)
    return reason


def _read_target_addr():
    return machine.mem32[REG_RTC_ADDR]


def _write_target_addr(addr):
    machine.mem32[REG_RTC_ADDR] = addr


# Check for the presence of a given file and attempt to start installing it.
def _stat_and_start_copy(path, copying_state, copied_state):
    try:
        os.stat(path)
    except OSError:
        log("{} not found", path)
        return False

    log("{} update is available", path)
    if sum(stm.rfcore_fw_version(_FW_VERSION_WS)):
        # There was some WS firmware already installed. Need to remove that
        # before copying to flash (both FUS or WS copy require this).
        log("Removing existing WS firmware")
        _write_state(_STATE_DELETING_WS)
        _fus_fwdelete()
    else:
        log("Copying {} to flash", path)
        # Mark that the flash write has started. Any failure should result in an overall failure.
        _write_state(copying_state)  # Either _STATE_COPYING_FUS or _STATE_COPYING_WS
        _copy_file_to_flash(path)
        log("Copying complete")
        # The entire write has completed successfully, start the install.
        _write_state(copied_state)  # Either _STATE_COPIED_FUS or _STATE_COPIED_WS

    return True


# This should be called in boot.py to resume any in-progress update.
# If there's nothing to do, it will return 0 and the app can continue as normal.
# If a previous update has failed, then it will return the failure reason.
# Otherwise it will attempt to continue the update from where it left off.
def resume():
    log("Checking firmware update progress...")

    if stm.rfcore_status() == _MAGIC_IPCC_MEM_INCORRECT:
        return _write_failure_state(REASON_RFCORE_NOT_CONFIGURED)

    while True:
        state = _read_state()

        if state == _STATE_IDLE:
            log("Firmware update complete")
            return 0

        elif state == _STATE_FAILED:
            log("Firmware update failed")
            return _read_failure_reason()

        # Keep calling GET_STATE until error or FUS.
        elif state == _STATE_WAITING_FOR_FUS:
            log("Querying FUS state")
            status, result = fus_get_state()
            log("FUS state: {} {}", status, result)

            if status == 0xFF and result == 0xFF:
                _write_failure_state(REASON_FUS_NOT_RESPONDING)
            elif status != 0:
                log("Operation in progress. Re-querying FUS state")
            elif stm.rfcore_status() == _MAGIC_FUS_ACTIVE:
                log("FUS active")
                _write_state(_STATE_CHECK_UPDATES)

        # Keep trying to start the WS until !fus_active() (or error).
        elif state == _STATE_WAITING_FOR_WS:
            if stm.rfcore_status() != _MAGIC_FUS_ACTIVE:
                log("WS active")
                _write_state(_STATE_IDLE)
                # Need to force a reset otherwise BLE will fail if FUS has changed.
                machine.reset()
            else:
                log("Starting WS")
                status, result = fus_start_ws()
                if status != 0:
                    log("Can't start WS")
                    log("WS version: {}", stm.rfcore_fw_version(_FW_VERSION_WS))
                    _write_failure_state(REASON_NO_WS)

        # Sequence the FUS 1.0.2 -> FUS 1.1.0 -> WS (depending on what's available).
        elif state == _STATE_CHECK_UPDATES:
            log("Checking for updates")
            fus_version = stm.rfcore_fw_version(_FW_VERSION_FUS)
            log("FUS version {}", fus_version)
            if fus_version < _FUS_VERSION_102:
                log("Factory FUS detected")
                if _stat_and_start_copy(_PATH_FUS_102, _STATE_COPYING_FUS, _STATE_COPIED_FUS):
                    continue
            elif fus_version >= _FUS_VERSION_102 and fus_version < _FUS_VERSION_110:
                log("FUS 1.0.2 detected")
                if _stat_and_start_copy(_PATH_FUS_110, _STATE_COPYING_FUS, _STATE_COPIED_FUS):
                    continue
            else:
                log("FUS is up-to-date")

            if fus_version >= _FUS_VERSION_110:
                if _stat_and_start_copy(
                    _PATH_WS_BLE_HCI,
                    _STATE_COPYING_WS,
                    _STATE_COPIED_WS,
                ):
                    continue
                else:
                    log("No WS updates available")
            else:
                # Don't attempt to install WS if we're running an old FUS.
                log("Need latest FUS to install WS")

            # Attempt to go back to WS.
            # Either this will fail (because WS was removed due to FUS install), or
            # this whole thing was a no-op and we should be fine to restart WS.
            _write_state(_STATE_WAITING_FOR_WS)

        # This shouldn't happen - the flash write should always complete and
        # move straight onto the COPIED state. Failure here indicates that
        # the rfcore is misconfigured or the WS firmware was not deleted first.
        elif state == _STATE_COPYING_FUS or state == _STATE_COPYING_WS:
            log("Flash copy failed mid-write")
            _write_failure_state(REASON_FLASH_COPY_FAILED)

        # Flash write completed, we should immediately see GET_STATE return 0,0
        # so we can start the FUS install.
        elif state == _STATE_COPIED_FUS:
            if fus_is_idle():
                log("FUS copy complete, installing")
                _write_state(_STATE_INSTALLING_FUS)
                _fus_run_fwupgrade()
            else:
                log("FUS copy bad state")
                _write_failure_state(REASON_FLASH_FUS_BAD_STATE)

        # Keep polling the state until we see a 0,0 (success) or non-transient
        # error. In general we should expect to see (16,0) several times,
        # followed by a (255,0), followed by (0, 0).
        elif state == _STATE_INSTALLING_FUS:
            log("Installing FUS...")
            status, result = fus_get_state(_INSTALLING_FUS_GET_STATE_TIMEOUT)
            log("FUS state: {} {}", status, result)
            if 0x20 <= status <= 0x2F and result == 0:
                # FUS_STATE_FUS_UPGRD_ONGOING
                log("FUS still in progress...")
            elif 0x10 <= status <= 0x1F and result == 0x11:
                # FUS_STATE_FW_UPGRD_ONGOING and FUS_FW_ROLLBACK_ERROR
                # Confusingly this is a "FW_UPGRD" (0x10) not "FUS_UPRD" (0x20).
                log("Attempted to install same FUS version... re-querying FUS state to resume.")
            elif status == 0:
                log("FUS update successful")
                _write_state(_STATE_CHECK_UPDATES)
            elif result == 0:
                # See below (for equivalent path for WS install -- we
                # sometimes see (255,0) right at the end).
                log("Re-querying FUS state...")
            elif result == 0xFF:
                _write_failure_state(REASON_FUS_NOT_RESPONDING_AFTER_FUS)
            else:
                _write_failure_state(REASON_FUS_VENDOR + result)

        # Keep polling the state until we see 0,0 or failure (1,0). Any other
        # result means retry (but the docs say that 0 and 1 are the only
        # status values).
        elif state == _STATE_DELETING_WS:
            log("Deleting WS...")
            status, result = fus_get_state()
            log("FUS state: {} {}", status, result)
            if status == 0:
                if sum(stm.rfcore_fw_version(_FW_VERSION_WS)) == 0:
                    log("WS deletion complete")
                    _write_state(_STATE_CHECK_UPDATES)
                else:
                    log("WS deletion no effect")
                    _write_failure_state(REASON_WS_STILL_PRESENT)
            elif status == 1:
                log("WS deletion failed")
                _write_failure_state(REASON_WS_DELETION_FAILED)

        # As for _STATE_COPIED_FUS above. We should immediately see 0,0.
        elif state == _STATE_COPIED_WS:
            if fus_is_idle():
                log("WS copy complete, installing")
                _write_state(_STATE_INSTALLING_WS)
                _fus_run_fwupgrade()
            else:
                log("WS copy bad state")
                _write_failure_state(REASON_FLASH_WS_BAD_STATE)

        # As for _STATE_INSTALLING_FUS above.
        elif state == _STATE_INSTALLING_WS:
            log("Installing WS...")
            status, result = fus_get_state(_INSTALLING_WS_GET_STATE_TIMEOUT)
            log("FUS state: {} {}", status, result)
            if 0x10 <= status <= 0x1F and result == 0:
                # FUS_STATE_FW_UPGRD_ONGOING
                log("WS still in progress...")
            elif 0x10 <= status <= 0x1F and result == 0x11:
                # FUS_FW_ROLLBACK_ERROR
                log("Attempted to install same WS version... re-querying FUS state to resume.")
            elif status == 0:
                log("WS update successful")
                _write_state(_STATE_WAITING_FOR_WS)
            elif result in (0, 0xFE):
                # We get an error response with no payload sometimes at the end
                # of the update (this is not in AN5185). Additionally during
                # WS update, newer WS reports (status, result) of (255, 254)
                # before eventually reporting the correct state of
                # _STATE_INSTALLING_WS once again. In these cases, re-try the
                # GET_STATE.
                # The same thing happens transitioning from WS to FUS mode.
                # The actual HCI response has no payload, the result=0 comes from
                # _parse_vendor_response above when len=7.
                log("Re-querying FUS state...")
            elif result == 0xFF:
                # This is specifically a failure sending the HCI command.
                _write_failure_state(REASON_FUS_NOT_RESPONDING_AFTER_WS)
            else:
                _write_failure_state(REASON_WS_VENDOR + result)


def install_boot():
    boot_py = "/flash/boot.py"
    header = ""
    mode = "w"
    try:
        with open(boot_py, "r") as boot:
            header = "\n"
            mode = "a"
            for line in boot:
                if "rfcore_firmware.resume()" in line:
                    print("Already installed.")
                    return

        print("boot.py exists, adding upgrade handler.")
    except OSError:
        print("boot.py doesn't exists, adding with upgrade handler.")

    with open(boot_py, mode) as boot:
        boot.write(header)
        boot.write("# Handle rfcore updates.\n")
        boot.write("import rfcore_firmware\n")
        boot.write("rfcore_firmware.resume()\n")


# Start a firmware update.
# This will immediately trigger a reset and start the update process on boot.
def check_for_updates(force=False):
    (
        src_filename,
        addr_1m,
        addr_640k,
        addr_512k,
        addr_256k,
        vers_fus,
    ) = check_file_details(_PATH_FUS_110)
    (
        src_filename,
        addr_1m,
        addr_640k,
        addr_512k,
        addr_256k,
        vers_ws,
    ) = check_file_details(_PATH_WS_BLE_HCI)
    current_version_fus = stm.rfcore_fw_version(_FW_VERSION_FUS)
    fus_uptodate = current_version_fus[0:3] == vers_fus

    current_version_ws = stm.rfcore_fw_version(_FW_VERSION_WS)
    ws_uptodate = current_version_ws[0:3] == vers_ws
    if fus_uptodate and ws_uptodate and not force:
        log(f"Already up to date: fus: {current_version_fus}, ws: {current_version_ws}")
    else:
        log("Starting firmware update")
        log(f" - fus: {current_version_fus} -> {vers_fus}")
        log(f" - ws:  {current_version_ws} -> {vers_ws}")
        _write_state(_STATE_WAITING_FOR_FUS)
        machine.reset()
