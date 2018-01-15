from ubluepy import Scanner, constants

def bytes_to_str(bytes):
    string = ""
    for b in bytes:
        string += chr(b)
    return string

def list_scan_results(scan_entries):
    for e in scan_entries:
        print("ADDR: ", e.addr())
        print("TYPE: ", e.addr_type())
        print("RSSI: ", e.rssi())
        scan = e.getScanData()
        if scan:
            for s in scan:
                #if s[0] == constants.ad_types.AD_TYPE_COMPLETE_LOCAL_NAME:
                print('\t{}: {}'.format(s[1], s[2]))
        print("")

s = Scanner()
scan_res = s.scan(2000)
list_scan_results(scan_res)
