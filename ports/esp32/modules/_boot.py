import gc
import uos
from flashbdev import bdev
from ota_updater import OTAUpdater

def download_and_install_update_if_available():
    ota_updater = OTAUpdater('https://github.com/ilyamordasov/micropython')
    ota_updater.download_and_install_update_if_available('Smart_box-142','natalia31081980')
    
def boot():
    try:
        if bdev:
            uos.mount(bdev, '/')
    except OSError:
        import inisetup
        inisetup.setup()

    gc.collect()
    download_and_install_update_if_available()

boot()