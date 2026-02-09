from esp32 import Partition
print(f"Active partition: {Partition.RUNNING}")
print(f"Partition at next boot (from main): {Partition.BOOT}")
from _todefrost import package_md5sum

md5 = None
try:
    with open('/package.md5') as f:
        md5 = f.read().strip()
except Exception as e:
    pass

if md5 != package_md5sum.md5sum:
    from time import sleep
    from app.system import NVSManager
    nvs = NVSManager()
    print("package md5 changed....defrosting...")
    print(f"old md5: {md5}")
    print(f"new md5: {package_md5sum.md5sum}")
    from _todefrost import microwave
    microwave.defrost()
    # from esp32 import Partition
    # print(f"package md5 changed, defrosted, setting boot partition to {Partition.RUNNING}")
    # Set the current partition as the boot partition for next boot (do not mark valid)
    Partition(Partition.RUNNING).set_boot()
    print(f"Active partition post defrost: {Partition.RUNNING}")
    sleep(1)
    print(f"Partition at next boot (pre reboot): {Partition.BOOT}")
    nvs.log.info("Device defrosted due to package md5 change. Rebooting...")
    import machine
    machine.reset()


def startApp():
    import app.run


try:
    startApp()
except KeyboardInterrupt:
    print("recognized keyboard interupt")
except Exception as e:
    import time
    print("Unable to start app, reverting to previous version: ", e)
    time.sleep(2)

