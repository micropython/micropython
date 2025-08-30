import os
import vfs


class Filesystem:
    def mount(self, readonly, mkfs):
        pass

    def open(self, file, mode):
        return None  # violates vfs contract


fs = Filesystem()
vfs.mount(fs, "/test_mnt")
try:
    execfile("/test_mnt/test.py")
    print("ExecFile succeeded")
except TypeError:
    print("TypeError")
