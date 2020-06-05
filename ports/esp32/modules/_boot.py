import gc
import uos
from flashbdev import bdev
from ota_updater import OTAUpdater

WIFI_SSID_PASSWORD = 'Smart_box-142','natalia31081980'

def download_and_install_update_if_available():
    ota_updater = OTAUpdater('https://github.com/ilyamordasov/micropython/tree/esp32-bluetooth')
    ota_updater.download_and_install_update_if_available(_,WIFI_SSID_PASSWORD)
    
def boot():
    download_and_install_update_if_available()
    
    try:
        if bdev:
            uos.mount(bdev, '/')
    except OSError:
        import inisetup
        inisetup.setup()

    gc.collect()

boot()