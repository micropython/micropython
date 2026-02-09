# esp32_ota module for MicroPython on ESP32
# MIT license; Copyright (c) 2023 Glenn Moloney @glenn20

# Inspired by OTA class by Thorsten von Eicken (@tve):
#   https://github.com/tve/mqboard/blob/master/mqrepl/mqrepl.py

import gc
import io

from esp32 import Partition

from .blockdev_writer import BlockDevWriter, sha_file 
from .status import ota_reboot


# Micropython sockets don't have context manager methods. This wrapper provides
# those.
class SocketWrapper:
    def __init__(self, f: io.BufferedReader):
        self.f = f

    def __enter__(self) -> io.BufferedReader:
        return self.f

    def __exit__(self, e_t, e_v, e_tr):
        self.f.close()


# Open a file or a URL and return a File-like object for reading
def open_url(url_or_filename: str, **kw) -> io.BufferedReader:
    if url_or_filename.split(":", 1)[0] in ("http", "https"):
        import requests

        r = requests.get(url_or_filename, **kw)
        code: int = r.status_code
        if code != 200:
            r.close()
            raise ValueError(f"HTTP Error: {code}")
        return SocketWrapper(r.raw)  # type: ignore
    else:
        return open(url_or_filename, "rb")


# OTA manages a MicroPython firmware update over-the-air. It checks that there
# are at least two "ota" "app" partitions in the partition table and writes new
# firmware into the partition that is not currently running. When the update is
# complete, it sets the new partition as the next one to boot. Set reboot=True
# to force a reset/restart, or call machine.reset() explicitly. Remember to call
# ota.rollback.cancel() after a successful reboot to the new image.
class OTA:
    def __init__(self, verify=True, verbose=True, reboot=False, sha="", length=0):
        self.reboot = reboot
        self.verbose = verbose
        
        # Get the next free OTA partition
        # Raise OSError(ENOENT) if no OTA partition available
        self.part = Partition(Partition.RUNNING).get_next_update()
        if verbose:
            name: str = self.part.info()[4]
            print(f"Writing new micropython image to OTA partition '{name}'...")
        self.writer = BlockDevWriter(self.part, verify, verbose)
        if sha or length:
            self.writer.set_sha_length(sha, length)

    # Append the data to the OTA partition
    def write(self, data: bytearray | bytes | memoryview) -> int:
        return self.writer.write(data)
    
    
    # Flush any buffered data to the ota partition and set it as the boot
    # partition. If verify is True, will read back the written firmware data to
    # check the sha256 of the written data. If reboot is True, will reboot the
    # device after 10 seconds.
    def close(self) -> None:
        if self.writer is None:
            return
        self.writer.close()
        # Set as boot partition for next reboot
        name: str = self.part.info()[4]
        print(f"OTA Partition '{name}' updated successfully.")
        self.part.set_boot()  # Raise OSError(-5379) if image on part is not valid
        bootname = Partition(Partition.BOOT).info()[4]
        if name != bootname:
            print(f"Warning: failed to set {name} as the next boot partition.")
        print(f"Micropython will boot from '{bootname}' partition on next boot.")
        print("Remember to call ota.rollback.cancel() after successful reboot.")
        if self.reboot:
            ota_reboot(1)

    def __enter__(self):
        return self

    def __exit__(self, e_t, e_v, e_tr):
        if e_t is None:  # If exception is thrown, don't flush data or set bootable
            self.close()

    # Load a firmware file from the provided io stream
    # - f: an io stream (supporting the f.readinto() method)
    # - sha: (optional) the sha256sum of the firmware file
    # - length: (optional) the length (in bytes) of the firmware file
    def from_stream(self, f: io.BufferedReader, sha: str = "", length: int = 0) -> int:
        if sha or length:
            self.writer.set_sha_length(sha, length)
        gc.collect()
        return self.writer.write_from_stream(f)

    # Write new firmware to the OTA partition from the given url
    # - url: a filename or a http[s] url for the micropython.bin firmware.
    # - sha: the sha256sum of the firmware file
    # - length: the length (in bytes) of the firmware file
    def from_firmware_file(self, url: str, sha: str = "", length: int = 0, **kw) -> int:
        if self.verbose:
            print(f"Opening firmware file {url}...")
        with open_url(url, **kw) as f:
            return self.from_stream(f, sha, length)

    # Load a firmware file, the location of which is read from a json file
    # containing the url for the firmware file, the sha and length of the file.
    # - url: the name of a file or url containing the json.
    # - kw: extra keywords arguments that will be passed to `requests.get()`
    def from_json(self, url: str, **kw) -> int:
        if not url.endswith(".json"):
            raise ValueError("Url does not end with '.json'")
        if self.verbose:
            print(f"Opening json file {url}...")
        with open_url(url, **kw) as f:
            from json import load

            data: dict = load(f)
        try:
            firmware: str = data["firmware"]
            sha: str = data["sha"]
            length: int = data["length"]
            if not any(firmware.startswith(s) for s in ("https:", "http:", "/")):
                # If firmware filename is relative, append to base of url of json file
                baseurl, *_ = url.rsplit("/", 1)
                firmware = f"{baseurl}/{firmware}"
            return self.from_firmware_file(firmware, sha, length, **kw)
        except KeyError as err:
            print('OTA json must include "firmware", "sha" and "length" keys.')
            raise err


# Convenience functions which use the OTA class to perform OTA updates.
def from_file(
    url: str, sha="", length=0, verify=True, verbose=True, reboot=True, **kw
) -> None:
    with OTA(verify, verbose, reboot) as ota_update:
        ota_update.from_firmware_file(url, sha, length, **kw)


def from_json(url: str, verify=True, verbose=True, reboot=True, **kw) -> None:
    with OTA(verify, verbose, reboot) as ota_update:
        ota_update.from_json(url, **kw)
