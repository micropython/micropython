try:
    from esp32 import Partition as p
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    # try to make nvs partition bootable, which ain't gonna work
    p.set_boot(p.find(type=p.TYPE_DATA)[0])
    print("FAILED TO RAISE")
except OSError as e:
    # 55=EOTHER
    print(e.args[0] == 55 and "_ERR_OTA_" in e.args[1])
