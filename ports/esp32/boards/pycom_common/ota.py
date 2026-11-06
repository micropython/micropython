"""
OTA (Over-The-Air) firmware update helper for Pycom boards.

Requires the OTA board variant (BOARD_VARIANT=OTA) which provides
dual app partitions (ota_0 / ota_1).

Usage:
    from ota import OTA
    updater = OTA()
    updater.apply_update("http://server/firmware.bin")
    # Board reboots into new firmware
"""

import machine
from esp32 import Partition


class OTA:
    """OTA firmware update using ESP-IDF partition API."""

    def __init__(self):
        self._current = Partition(Partition.RUNNING)
        self._target = self._current.get_next_update()

    @property
    def current_partition(self):
        """Name and info of the currently running partition."""
        return self._current.info()

    @property
    def target_partition(self):
        """Name and info of the target partition for next update."""
        return self._target.info()

    def apply_update(self, url):
        """
        Download firmware from HTTP URL and write to the next OTA partition.

        Args:
            url: HTTP(S) URL to the firmware .bin file.
                 The file must be a valid MicroPython application binary.

        After successful write, the board reboots into the new firmware.
        """
        import urequests

        print("Downloading firmware from:", url)
        response = urequests.get(url)

        if response.status_code != 200:
            response.close()
            raise OSError("HTTP error: {}".format(response.status_code))

        firmware = response.content
        response.close()

        self._write_firmware(firmware)

    def apply_update_from_file(self, path):
        """
        Write firmware from a local file to the next OTA partition.

        Args:
            path: Path to the firmware .bin file on the filesystem.
        """
        with open(path, "rb") as f:
            firmware = f.read()

        self._write_firmware(firmware)

    def _write_firmware(self, firmware):
        """Write firmware bytes to the target partition and reboot."""
        size = len(firmware)
        part_info = self._target.info()
        part_size = part_info[3]

        if size > part_size:
            raise ValueError("Firmware too large: {} bytes > {} partition".format(size, part_size))

        print("Writing {} bytes to partition '{}'...".format(size, part_info[4]))

        # Erase and write in block-aligned chunks
        BLOCK_SIZE = 4096
        offset = 0
        while offset < size:
            chunk = firmware[offset : offset + BLOCK_SIZE]
            if len(chunk) < BLOCK_SIZE:
                chunk = chunk + b"\xff" * (BLOCK_SIZE - len(chunk))
            self._target.writeblocks(offset // BLOCK_SIZE, chunk)
            offset += BLOCK_SIZE

        print("Setting boot partition...")
        self._target.set_boot()

        print("Rebooting...")
        machine.reset()

    def rollback(self):
        """
        Roll back to the previous firmware.

        Sets the other OTA partition as boot target and reboots.
        """
        self._target.set_boot()
        print("Rolling back, rebooting...")
        machine.reset()

    @staticmethod
    def validate():
        """
        Mark the current firmware as valid after a successful OTA update.

        Call this early in boot after verifying the new firmware works.
        If not called, the bootloader will roll back on next reboot.
        """
        Partition.mark_app_valid_cancel_rollback()
        print("Firmware validated.")
