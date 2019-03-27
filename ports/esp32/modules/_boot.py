import gc
import uos
from flashbdev import bdev

try:
    if bdev:
        uos.mount(bdev, '/')
except OSError:
    import inisetup
    vfs = inisetup.setup()

gc.collect()

station = network.WLAN(network.STA_IF)
station.active(True)
station.connect(SSID, PW)
while station.isconnected() == False:
  pass
print('Connection successful')
print(station.ifconfig())
