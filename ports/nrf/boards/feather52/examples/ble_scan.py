from ubluepy import Scanner, constants

def display_scan_results(scan_entries):
    for e in scan_entries:
        print("ADDR:  ", e.addr())
        print("TYPE:  ", e.addr_type())
        print("RSSI:  ", e.rssi())

        # Parse the contents of the advertising packet
        scan = e.getScanData()
        if scan:
            for s in scan:
                # Convert byte array to hex format string
                hex = ' '.join('0x%02X' % b for b in s[2])
                # Display enum value and hex string together
                print('\t{}: {}'.format(s[1], hex))

        # Line break between record sets
        print("")

# Scan 1s for advertising devices in range
s = Scanner()
scan_res = s.scan(1000)

# Display the scan results
display_scan_results(scan_res)
