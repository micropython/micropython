WiFi scanning with options
==========================

There are a number of options available for scanning of WiFi networks while using
the ESP32. They can all be used to customize the scan that takes places, allowing
for the scan to be tailored to the code's needs.

.. code-block:: python3

    import network

    # Create the WiFi station interface
    sta_if = network.WLAN(network.STA_IF)
    sta_if.active(True)

    # Scan for available networks with the default options
    networks = sta_if.scan(
        # This takes to indicate if the scan should block until complete
        blocking=True,              # Have the scan block until it is complete
        # A callback function to use if the scan in non-blocking
        # This takes in two parameters:
        #  - interface: the network interface that performed the scan
        #  - callback_event: a tuple with the first element being the status of the scan
        #    (True if successful, False if not), and the second element being a list of networks found
        callback=None,              # No callback function on a blocking scan
        # This takes an integer or a list of integers to specify the channel(s) to scan
        channel=None,               # Defaults to None, which means all channels will be scanned
        # This takes a byte string to filter the scan results by BSSID
        bssid=None,                 # Filter by BSSID (None means no filter)
        # This takes a string to filter the scan results by SSID
        ssid=None,                  # Filter by SSID (None means no filter)
        # This takes a boolean to indicate if hidden networks should be shown
        show_hidden=True,           # Show hidden networks found in the scan
        # This takes a boolean to indicate if the scan should be an active scan.
        # An active scan has the ESP32 sending probe requests and waiting for responses,
        # while a passive scan sits on each channel only listening for beacons.
        active_scan=True,           # An active scan will be performed
        # This takes an integer to specify the time spent on each channel in ms during a passive scan
        # It is not recommended to set this value above 1500 ms, as it can cause the station interface
        # to disconnect from the AP it is connected to.
        scan_time_passive=360,      # The time spent on each channel in a passive scan
        # This takes an integer to specify the minimum time spent on each channel in ms during an active scan
        scan_time_active_min=0,     # The minimum time spent on each channel in an active scan
        # This takes an integer to specify the maximum time spent on each channel in ms during an active scan
        # It is not recommended to set this value above 1500 ms, as it can cause the station interface
        # to disconnect from the AP it is connected to.
        scan_time_active_max=120,   # The maximum time spent on each channel in an active scan
        # This takes an integer to specify the time in ms to return to the channel of this connected network
        # Between each channel in a scan, the ESP32 will return to the channel of the connected network 
        # to avoid disconnection.
        home_chan_dwell_time=30     # The amount of time to return to the channel of the connected network
    )

Using these options can help simplify code. For example, if you want to keep scanning
until you find a specific SSID that you want to connect to. You can set the `ssid`
option, which will cause the scan to return an empty list until the SSID is found.
Allowing you to retry the scan until the SSID is found, without having to manually
check the results of the scan each time.

.. code-block:: python3

    import network

    # Create the WiFi station interface
    sta_if = network.WLAN(network.STA_IF)
    sta_if.active(True)

    SSID = 'YourNetworkName'
    PASSWORD = 'YourNetworkPassword'

    networks = []
    # networks will remain an empty list until the SSID is found
    while not networks:
        # Scan for networks with only the specified SSID
        networks = sta_if.scan(ssid=SSID)
        if not networks:
            print("SSID was not found, retrying scan...")
    
    # If the network was found, connect to it
    sta_if.connect(SSID, PASSWORD)
    print("Connected to network:", SSID)


Scanning on a specific channel/channels is something that is also supported if you are certain
that the network you are looking for is on a specific channel or channels. This will speed up
the scanning time even when used in a blocking manner, since only the specified channels are scanned.

.. code-block:: python3

    import network
    import time

    # Create the WiFi station interface
    sta_if = network.WLAN(network.STA_IF)
    sta_if.active(True)

    start_time = time.ticks_ms()
    sta_if.scan()
    end_time = time.ticks_ms()

    print(f"Full scan took {time.ticks_diff(end_time, start_time)} ms")

    # Networks can be scanned by a specific channel
    start_time = time.ticks_ms()
    networks = sta_if.scan(channel=3)
    end_time = time.ticks_ms()

    for network in networks:
        print(f"Network found: {network[0]}, channel: {network[2]}")

    print(f"Scan on channel 3 took {time.ticks_diff(end_time, start_time)} ms")

    # A list of channels may also be provided
    channels = [1, 6, 11]
    start_time = time.ticks_ms()
    networks = sta_if.scan(channel=channels)
    end_time = time.ticks_ms()

    for network in networks:
        print(f"Network found: {network[0]}, channel: {network[2]}")
    
    print(f"Scan on channels {channels} took {time.ticks_diff(end_time, start_time)} ms")

Another method that can be used to speed up the scanning process is to change
the scan timing parameters. By default, the ESP32 will spend a certain amount of time
on each channel during a scan, and this can be reduced to speed up the scan.
Do note that this may result in some networks not being found, in the event they
do not respond within the reduced time frame.

.. code-block:: python3

    import network
    import time

    # Create the WiFi station interface
    sta_if = network.WLAN(network.STA_IF)
    sta_if.active(True)

    # Scan with default timing parameters
    start_time = time.ticks_ms()
    networks = sta_if.scan()
    end_time = time.ticks_ms()
    print(f"Default scan took {time.ticks_diff(end_time, start_time)} ms")
    print(f"and found {len(networks)} networks")

    # Scan with reduced active scan time
    scan_options = {
        'scan_time_active_min': 20,
        # Only allow 50 ms per channel
        'scan_time_active_max': 50
    }

    start_time = time.ticks_ms()
    # Since the options are keyword arguments, the dictionary can be passed
    networks = sta_if.scan(**scan_options)
    end_time = time.ticks_ms()
    print(f"Reduced active scan time took {time.ticks_diff(end_time, start_time)} ms")
    print(f"and found {len(networks)} networks")

While a blocking scan is the simplest way to scan for networks, it is possible
to perform a non-blocking scan that allow your code to continue running while
the scan is in progress. This is useful if you want to perform other tasks on the
core while waiting for the scan results to complete, or have a task occur when a
scan is completed. For example this code below provides an example of both of these.
The scan is performed in a non-blocking manner, and the callback will attempt search
through the results checking for networks that match a list of networks we want to
connect to. If a matching network is found, it will attempt to connect to it using
the password provided in the list. At the same time the scan is occurring, the
next loop will continue to run incrementing a counter.

.. code-block:: python3

    import network

    # Create the WiFi station interface
    sta_if = network.WLAN(network.STA_IF)
    sta_if.active(True)

    # Define the networks we want to connect to
    NETWORK_NAME = ['YourNetworkName', 'SecondNetworkName']
    NETWORK_PASSWORD = ['YourNetworkPassword', 'SecondNetworkPassword']

    if len(NETWORK_NAME) != len(NETWORK_PASSWORD):
        raise ValueError("NETWORK_NAME and NETWORK_PASSWORD must have the same length")

    loop_counter = 0
    NETWORK_FOUND = False

    def scan_callback(interface, callback_event):
        # interface is the network interface that performed the scan
        # callback_event is a two element tuple:
        #  - the first element is the status of the scan, True if successful, False if not
        #  - the second element is a list of networks found during the scan
        print("Scan completed: ", callback_event[0])
        for network in callback_event[1]:
            # Check each scanned network if it's in our list of networks
            if network[0].decode('utf-8') in NETWORK_NAME:
                print("Found network: ", network[0].decode('utf-8'))
                # Get the index for the password
                index = NETWORK_NAME.index(network[0].decode('utf-8'))
                sta_if.connect(network[0].decode('utf-8'), NETWORK_PASSWORD[index])

                sta_if.ipconfig('addr4')

                global NETWORK_FOUND
                NETWORK_FOUND = True

    return_val = sta_if.scan(
        blocking=False,  # Run a non-blocking scan
        callback=scan_callback,  # Use a callback function to handle the scan results
    )

    # While the scan is in progress, we can do other things
    while sta_if.scan_in_progress():
        loop_counter += 1
        if loop_counter % 1000 == 0:
            print(f"Looped {loop_counter} times, while waiting for scan results...")

    print(f"Scan completed, and our loop ran {loop_counter} times.")

    if NETWORK_FOUND:
        print("Connected to a network!")
        # Print the ip address of the station interface after connecting
        print("Station interface IP address: ", sta_if.ifconfig()[0])
    else:
        print("No matching network found, or connection failed.")

