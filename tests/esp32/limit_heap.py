# Test MicroPython heap limits on the esp32.
# This test requires resetting the device and thus does not simply run in run-tests.
# You can run this test manually using pyboard and hitting ctrl-c after each reset and rerunning
# the script. It will cycle through the various settings.
import gc
import machine
from esp32 import NVS, idf_heap_info, HEAP_DATA

nvs = NVS("micropython")
try:
    min_idf = nvs.get_i32("min_idf_heap")
except OSError:
    min_idf = 0
try:
    max_mp = nvs.get_i32("max_mp_heap")
except OSError:
    max_mp = None

mp_total = gc.mem_alloc() + gc.mem_free()
print("MP heap:", mp_total)
idf_free = sum([h[2] for h in idf_heap_info(HEAP_DATA)])
print("IDF heap free:", idf_free)

if min_idf == 0:
    nvs.set_i32("min_idf_heap", 100000)
    nvs.commit()
    print("IDF MIN heap changed to 100000")
    machine.reset()
elif max_mp is None:
    nvs.set_i32("max_mp_heap", 50000)
    nvs.commit()
    print("MAX heap changed to 50000")
    machine.reset()
else:
    try:
        nvs.erase_key("min_idf_heap")
    except OSError:
        pass
    try:
        nvs.erase_key("max_mp_heap")
    except OSError:
        pass
    print("Everything reset to default")
    machine.reset()
