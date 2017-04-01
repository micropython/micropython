from ubluepy import Scanner, constants

def bytes_to_str(bytes):
    string = ""
    for b in bytes:
        string += chr(b)
    return string

def get_device_names(scan_entries):
    dev_names = []
    for e in scan_entries:
        scan = e.getScanData()
        if scan:
            for s in scan:
               if s[0] == constants.ad_types.AD_TYPE_COMPLETE_LOCAL_NAME:
                   dev_names.append((e, bytes_to_str(s[2])))
    return dev_names

def find_device_by_name(name):
    s = Scanner()
    scan_res = s.scan(100)
    
    device_names = get_device_names(scan_res)
    for dev in device_names:
        if name == dev[1]:
            return dev[0]

# >>> res = find_device_by_name("micr")
# >>> if res:
# ...     print("address:", res.addr())
# ...     print("address type:", res.addr_type())
# ...     print("rssi:", res.rssi())
# ...     
# ...     
# ... 
# address: c2:73:61:89:24:45
# address type: 1
# rssi: -26
