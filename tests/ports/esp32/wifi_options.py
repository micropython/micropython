import machine
import sys

if sys.implementation.name == "micropython" and sys.platform != "esp32":
    print("SKIP")
    raise SystemExit

import network
import random

wlan = network.WLAN(network.STA_IF)

wlan.active(True)

RUN_TESTS = True

# This goes with test_non_blocking_soft_reboot() to check for safely handled soft resets
if wlan.scan_in_progress() and machine.reset_cause() == machine.SOFT_RESET:
    import time

    MAX_TIMEOUT = 20000  # Maximum wait time in milliseconds

    # This is a test case where the soft reset is called while a scan is in progress.
    start_time = time.ticks_ms()
    elapsed_time = 0
    while wlan.scan_in_progress() and elapsed_time < MAX_TIMEOUT:
        elapsed_time = time.ticks_diff(time.ticks_ms(), start_time)

    if elapsed_time >= MAX_TIMEOUT:
        print("ERROR: Scan did not complete within the maximum wait time after soft reset.")
    else:
        print("SUCCESS: Scan in progress was cleared after soft reset.")

    # Set a flag to skip the tests, since a scan is in progress after a soft reset
    # is caused by the final test case.
    RUN_TESTS = False


# TODO: This test case does work, but only if copied to main.py, and not using mpremote run
# Unsure if there is any way to let mpremote run know to expected a soft reset condition and keep listening
def test_non_blocking_soft_reboot():
    # Create scan options that cause a semi-long scan, so that a reset can complete and the scan
    # will have not yet completed.
    scan_options = {
        "blocking": False,
        "scan_time_active_min": 500,
        "scan_time_active_max": 750,
        "scan_time_passive": 500,
    }

    def scan_callback(interface, callback_event):
        # This callback should not be called after the soft reboot
        print("ERROR: Scan callback was called after soft reboot, which should not happen.")

    wlan.scan(callback=scan_callback, **scan_options)

    # Perform a soft reset. This will cause micropython to reset, but the scan will
    # continue running in the background since reset only resets the micropython thread.
    # This will cause the C callback to still be called, but the user's callback function
    # will no longer exist in the python state, and thus should not be called.
    machine.soft_reset()


def test_channel_single_value():
    try:
        wlan.scan(channel=1)
        print("SUCCESS: Scan with channel=1 succeeded.")
    except Exception as e:
        print(f"ERROR: Scan with channel=1 failed: {e}")


def test_channel_single_value_too_low():
    try:
        wlan.scan(channel=0)
        print("ERROR: Scan with channel=0 succeeded instead of failing.")
    except ValueError as e:
        print("SUCCESS: Scan with channel=0 failed as expected.")


def test_channel_single_value_too_high():
    try:
        wlan.scan(channel=15)
        print("ERROR: Scan with channel=15 succeeded instead of failing.")
    except ValueError as e:
        print("SUCCESS: Scan with channel=15 failed as expected.")


def test_channel_single_value_not_int():
    try:
        wlan.scan(channel="string")
        print("Scan with channel='1' succeeded instead of failing.")
    except TypeError as e:
        print("SUCCESS: Scan with channel='string' failed with TypeError as expected.")


def test_int_channel_scan():
    test_channel_single_value()
    test_channel_single_value_too_low()
    test_channel_single_value_too_high()
    test_channel_single_value_not_int()


def test_channel_with_empty_list():
    test_list = []
    try:
        wlan.scan(channel=test_list)
        print("ERROR: Scan with [] channel list succeeded instead of failing.")
    except ValueError as e:
        print("SUCCESS: Scan with [] channel list failed with ValueError as expected.")


def test_channel_with_single_value_list():
    test_list = [1]
    try:
        wlan.scan(channel=test_list)
        print("SUCCESS: Scan with [1] channel succeeded.")
    except Exception as e:
        print(f"ERROR: Scan with [1] channel failed: {e}")


def test_channel_with_single_value_too_low():
    test_list = [0, 1]
    try:
        wlan.scan(channel=test_list)
        print("ERROR: Scan with [0, 1] channel succeeded instead of failing.")
    except ValueError as e:
        print("SUCCESS: Scan with [0, 1] channel failed with ValueError as expected.")


def test_channel_with_single_value_too_high():
    test_list = [14, 15]
    try:
        wlan.scan(channel=test_list)
        print("ERROR: Scan with [14, 15] channel succeeded instead of failing.")
    except ValueError as e:
        print("SUCCESS: Scan with [14, 15] channel failed with ValueError as expected.")


def test_channel_full_list():
    # This is all the channels, which are 1 to 14
    test_list = [item for item in range(1, 15)]
    try:
        wlan.scan(channel=test_list)
        print("SUCCESS: Scan with full channel list succeeded.")
    except Exception as e:
        print(f"ERROR: Scan with full channel list failed: {e}")


def test_channel_with_invalid_list():
    test_list = [0, 15]
    try:
        wlan.scan(channel=test_list)
        print("ERROR: Scan with [0, 15] channel list succeeded instead of failing.")
    except ValueError as e:
        print("SUCCESS: Scan with [0, 15] channel failed with ValueError as expected.")


def test_channel_with_random_list():
    test_list = [random.randint(1, 14) for _ in range(3)]
    try:
        wlan.scan(channel=test_list)
        print(f"SUCCESS: Scan with random channel list random_list succeeded")
    except Exception as e:
        print(f"ERROR: Scan with random channel list random_list: {test_list} failed: {e}")


def test_channel_with_duplicates():
    test_list = [1, 1, 2, 3, 3, 4]
    try:
        wlan.scan(channel=test_list)
        print("SUCCESS: Scan with duplicates in channel list succeeded.")
    except Exception as e:
        print(f"ERROR: Scan with duplicates in channel list failed: {e}")


def test_channel_with_tuple():
    test_tuple = (1, 2, 3)
    try:
        wlan.scan(channel=test_tuple)
        print("SUCCESS: Scan with tuple channel list succeeded.")
    except Exception as e:
        print(f"ERROR: Scan with tuple channel list failed: {e}")


def test_channel_with_invalid_item():
    test_list = [1, 2, "3", 4]
    try:
        wlan.scan(channel=test_list)
        print("ERROR: Scan with invalidly typed item in channel list succeeded instead of failing")
    except TypeError as e:
        print(
            "SUCCESS: Scan with invalidly typed item in channel list failed with TypeError as expected"
        )


def test_list_channel_scan():
    if not wlan.active():
        wlan.active(True)
    test_channel_with_empty_list()
    test_channel_with_single_value_list()
    test_channel_with_single_value_too_low()
    test_channel_with_single_value_too_high()
    test_channel_full_list()
    test_channel_with_invalid_list()
    test_channel_with_random_list()
    test_channel_with_duplicates()
    test_channel_with_tuple()
    test_channel_with_invalid_item()


def check_scan_time(scan_time_active_max):
    import time

    start_time = time.ticks_ms()
    results = wlan.scan(scan_time_active_max=scan_time_active_max)
    end_time = time.ticks_ms()
    elapsed_time = time.ticks_diff(end_time, start_time)
    print(f"Scan completed in {elapsed_time} ms with scan_time_active_max={scan_time_active_max}")
    return elapsed_time


def test_scan_time_passive_high():
    try:
        wlan.scan(scan_time_passive=2000)
        print("ERROR: Scan with scan_time_passive=2000 succeeded instead of failing.")
    except ValueError as e:
        print("SUCCESS: Scan with scan_time_passive=2000 failed with ValueError as expected.")


def test_scan_time_passive_low():
    try:
        wlan.scan(scan_time_passive=-1)
        print("ERROR: Scan with scan_time_passive=-1 succeeded instead of failing.")
    except ValueError as e:
        print("SUCCESS: Scan with scan_time_passive=-1 failed with ValueError as expected.")


def test_scan_time_passive_invalid():
    try:
        wlan.scan(scan_time_passive="string")
        print("ERROR: Scan with scan_time_passive='string' succeeded instead of failing.")
    except TypeError as e:
        print("SUCCESS: Scan with scan_time_passive='string' failed with TypeError as expected.")


def test_scan_time_passive_valid():
    try:
        wlan.scan(scan_time_passive=100)
        print("SUCCESS: Scan with scan_time_passive=100 succeeded.")
    except Exception as e:
        print(f"ERROR: Scan with scan_time_passive=100 failed: {e}")


def test_scan_time_passive():
    test_scan_time_passive_high()
    test_scan_time_passive_low()
    test_scan_time_passive_invalid()
    test_scan_time_passive_valid()


def test_home_chan_dwell_time_high():
    # home_chan_dwell_time is a uint8_t (0-255), so 256 is too high
    try:
        wlan.scan(home_chan_dwell_time=256)
        print("ERROR: Scan with home_chan_dwell_time=256 succeeded instead of failing.")
    except ValueError as e:
        print("SUCCESS: Scan with home_chan_dwell_time=256 failed with ValueError as expected.")


def test_home_chan_dwell_time_low():
    # home_chan_dwell_time is a uint8_t (0-255), so -1 is too low
    try:
        wlan.scan(home_chan_dwell_time=-1)
        print("ERROR: Scan with home_chan_dwell_time=-1 succeeded instead of failing.")
    except ValueError as e:
        print("SUCCESS: Scan with home_chan_dwell_time=-1 failed with ValueError as expected.")


def test_home_chan_dwell_time_invalid():
    try:
        wlan.scan(home_chan_dwell_time="string")
        print("ERROR: Scan with home_chan_dwell_time='string' succeeded instead of failing.")
    except TypeError as e:
        print(
            "SUCCESS: Scan with home_chan_dwell_time='string' failed with TypeError as expected."
        )


def test_home_chan_dwell_time_valid():
    try:
        wlan.scan(home_chan_dwell_time=50)
        print("SUCCESS: Scan with home_chan_dwell_time=100 succeeded.")
    except Exception as e:
        print(f"ERROR: Scan with home_chan_dwell_time=100 failed: {e}")


def test_home_chan_dwell_time():
    test_home_chan_dwell_time_high()
    test_home_chan_dwell_time_low()
    test_home_chan_dwell_time_invalid()
    test_home_chan_dwell_time_valid()


def test_scan_time_active_min_high():
    try:
        wlan.scan(scan_time_active_min=2000)
        print("ERROR: Scan with scan_time_active_min=2000 succeeded instead of failing.")
    except ValueError as e:
        print("SUCCESS: Scan with scan_time_active_min=2000 failed with ValueError as expected.")


def test_scan_time_active_min_low():
    try:
        wlan.scan(scan_time_active_min=-1)
        print("ERROR: Scan with scan_time_active_min=-1 succeeded instead of failing.")
    except ValueError as e:
        print("SUCCESS: Scan with scan_time_active_min=-1 failed with ValueError as expected.")


def test_scan_time_active_min_invalid():
    try:
        wlan.scan(scan_time_active_min="string")
        print("ERROR: Scan with scan_time_active_min='string' succeeded instead of failing.")
    except TypeError as e:
        print(
            "SUCCESS: Scan with scan_time_active_min='string' failed with TypeError as expected."
        )


def test_scan_time_active_min_above_default_high():
    # Since scan_time_active_max has a default of 120, this test checks if settings min above the default
    # max will raise an error
    try:
        wlan.scan(scan_time_active_min=1000)
        print("ERROR: Scan with scan_time_active_min=1000 succeeded instead of failing.")
    except ValueError as e:
        print("SUCCESS: Scan with scan_time_active_min=1000 failed with ValueError as expected.")


def test_scan_time_active_min_valid():
    try:
        wlan.scan(scan_time_active_min=100)
        print("SUCCESS: Scan with scan_time_active_min=100 succeeded.")
    except Exception as e:
        print(f"ERROR: Scan with scan_time_active_min=100 failed: {e}")


def test_scan_time_active_min():
    test_scan_time_active_min_high()
    test_scan_time_active_min_low()
    test_scan_time_active_min_invalid()
    test_scan_time_active_min_above_default_high()
    test_scan_time_active_min_valid()


def test_scan_time_active_max_high():
    try:
        wlan.scan(scan_time_active_max=2000)
        print("ERROR: Scan with scan_time_active_max=2000 succeeded instead of failing.")
    except ValueError as e:
        print("SUCCESS: Scan with scan_time_active_max=2000 failed with ValueError as expected.")


def test_scan_time_active_max_low():
    try:
        wlan.scan(scan_time_active_max=-1)
        print("ERROR: Scan with scan_time_active_max=-1 succeeded instead of failing.")
    except ValueError as e:
        print("SUCCESS: Scan with scan_time_active_max=-1 failed with ValueError as expected.")


def test_scan_time_active_max_invalid():
    try:
        wlan.scan(scan_time_active_max="string")
        print("ERROR: Scan with scan_time_active_max='string' succeeded instead of failing.")
    except TypeError as e:
        print(
            "SUCCESS: Scan with scan_time_active_max='string' failed with TypeError as expected."
        )


def test_scan_time_active_max_valid():
    import time

    try:
        start_time = time.ticks_ms()
        wlan.scan(scan_time_active_max=1500, scan_time_active_min=1400)
        end_time = time.ticks_ms()

        elapsed_time = time.ticks_diff(end_time, start_time)
        # 15 channels with a maximum time of 1500ms and min of 140ms should take around 20 seconds
        # There are some timing variations, so allow for
        if elapsed_time < 15000:
            print(
                f"ERROR: Scan with scan_time_active_max=1000 was not long enough: {elapsed_time} ms"
            )
        else:
            print(f"SUCCESS: Scan with scan_time_active_max=1000 completed in reasonable time")
    except Exception as e:
        print(f"ERROR: Scan with scan_time_active_max=1000 failed: {e}")


def test_scan_time_active_max():
    test_scan_time_active_max_high()
    test_scan_time_active_max_low()
    test_scan_time_active_max_invalid()
    test_scan_time_active_max_valid()


def test_scan_time_active_match():
    try:
        wlan.scan(scan_time_active_min=100, scan_time_active_max=100)
        print(
            "SUCCESS: Scan with scan_time_active_min=100 and scan_time_active_max=100 succeeded."
        )
    except Exception as e:
        print(
            f"ERROR: Scan with scan_time_active_min=100 and scan_time_active_max=100 failed: {e}"
        )


def test_scan_time_active_inverted():
    try:
        wlan.scan(scan_time_active_min=150, scan_time_active_max=100)
        print(
            "ERROR: Scan with scan_time_active_min=150 and scan_time_active_max=100 succeeded instead of failing."
        )
    except ValueError as e:
        print(
            "SUCCESS: Scan with scan_time_active_min=150 and scan_time_active_max=100 failed with ValueError as expected."
        )


def test_scan_times():
    test_home_chan_dwell_time()
    test_scan_time_passive()
    test_scan_time_active_min()
    test_scan_time_active_max()
    test_scan_time_active_match()
    test_scan_time_active_inverted()


def test_ssid_too_long():
    ssid = "a" * 33  # SSID longer than 32 characters
    try:
        wlan.scan(ssid=ssid)
        print("ERROR: Scan with long SSID succeeded instead of failing")
    except ValueError as e:
        print("SUCCESS: Scan with long SSID failed with ValueError as expected.")


def test_ssid_empty():
    ssid = ""
    try:
        results = wlan.scan(ssid=ssid)
        for network in results:
            # Check if the network is a hidden network, since that should be the only ones returned
            if network[5] != True:
                print("ERROR: Scan with empty SSID found non-hidden networks")
            # TODO: Should this also check that the SSID is empty?
        print("SUCCESS: Scan with empty SSID succeeded")
    except Exception as e:
        print(f"ERROR: Scan with empty SSID failed: {e}")


# TODO: Is there a way to validate this test with a real SSID easily?
def test_ssid_correct():
    ssid = "TestSSID"
    try:
        # It would be nice if there was a guaranteed WiFi network to test against
        results = wlan.scan(ssid=ssid)
        print("SUCCESS: Scan with correct SSID completed")
    except Exception as e:
        print(f"ERROR: Scan with correct SSID failed: {e}")


def test_ssid_real_world():
    # Hidden networks have no SSID, so only get networks with a non-empty SSID
    # This does pick a random network from the scan results, so it is not fully reliable
    base_results = wlan.scan(show_hidden=False)

    import random

    if not base_results:
        print("ERROR: No networks found to test SSID scan")
        return

    random_network = random.choice(base_results)

    results = wlan.scan(ssid=random_network[0])

    if not results:
        print(f"ERROR: Scan with SSID {random_network[0]} found no networks")
        return

    found_network = False
    for network in results:
        if network[0] != random_network[0]:
            print(
                f"ERROR: Scan with SSID {random_network[0]} found network with different SSID: {network[0]}"
            )
        else:
            found_network = True

    if found_network:
        print(f"SUCCESS: Real world SSID scan completed and found results")
    else:
        print(
            f"ERROR: Real world SSID scan did not find the expected network with SSID {random_network[0]}"
        )


def test_ssid():
    test_ssid_too_long()
    test_ssid_empty()
    test_ssid_correct()
    test_ssid_real_world()


def test_bssid_too_long():
    bssid = b"\x01\x02\x03\x04\x05\x06\x07"
    try:
        wlan.scan(bssid=bssid)
        print("ERROR: Scan with too long BSSID succeeded instead of failing")
    except ValueError as e:
        print("SUCCESS: Scan with too long BSSID failed with ValueError as expected.")


def test_bssid_too_short():
    bssid = b"\x01\x02\x03\x04\x05"
    try:
        wlan.scan(bssid=bssid)
        print("ERROR: Scan with too short BSSID succeeded instead of failing")
    except ValueError as e:
        print("SUCCESS: Scan with too short BSSID failed with ValueError as expected.")


def test_bssid_correct():
    bssid = b"\x01\x02\x03\x04\x05\x06"  # Valid BSSID
    try:
        results = wlan.scan(bssid=bssid)
        print("SUCCESS: Scan with correct BSSID completed")
    except Exception as e:
        print(f"ERROR: Scan with correct BSSID failed: {e}")


# This picks a random network from a base set of scan results, making this test not fully
# reliable, since it could pick a network that is moving and out of range by
# the time the additional scans is performed.
def test_ssid_bssid_combined_scan():
    # Do a default scan to get a list of networks
    base_results = wlan.scan()

    import random

    if not base_results:
        print("ERROR: No networks found to test SSID and BSSID scan")
        return

    random_network = random.choice(base_results)
    # The SSID being empty means a hidden network, which is not ideal for scanning by SSID
    while random_network[0] == b"":
        random_network = random.choice(base_results)

    # Get the SSID from the random network
    ssid = random_network[0]

    # Scan for networks with the same SSID
    results = wlan.scan(ssid=ssid)

    bssid_match = False
    for network in results:
        if network[0] != ssid:
            print(f"ERROR: Scan with SSID {ssid} found network with different SSID: {network[0]}")
            break
        # Check that at least one network matches the BSSID of the random network
        if network[1] == random_network[1]:
            bssid_match = True

    if not bssid_match:
        print(f"ERROR: Scan with SSID {ssid} did not find a matching BSSID")
    else:
        print("SUCCESS: Scan with SSID found original network with matching BSSID")

    # Pick a new random network to test the BSSID scan
    random_network = random.choice(base_results)

    # Get the BSSID from the random network
    bssid = random_network[1]

    # Scan for networks with the same BSSID
    results = wlan.scan(bssid=bssid)

    ssid_match = False
    for network in results:
        if network[1] != bssid:
            print(
                f"ERROR: Scan with BSSID {bssid.hex()} found network with different BSSID: {network[1].hex()}"
            )
            break
        # Check that at least one network matches the SSID of the random network
        if network[0] == random_network[0]:
            ssid_match = True

    if not ssid_match:
        print(f"ERROR: Scan with BSSID {bssid.hex()} did not find a matching SSID")
    else:
        print("SUCCESS: Scan with BSSID found original network with matching SSID")


def test_bssid_real_world():
    # This picks a random network from a base set of scan results, making this test not fully
    # reliable, since it could pick a network that is moving and out of range by
    # the time the additional scans is performed.
    base_results = wlan.scan()

    if not base_results:
        print("ERROR: No networks found to test BSSID scan")
        return

    import random

    random_network = random.choice(base_results)

    results = wlan.scan(bssid=random_network[1])

    if not results:
        print(f"ERROR: Scan with BSSID {random_network[1].hex()} found no networks")
        return

    found_network = False
    for network in results:
        if network[1] != random_network[1]:
            print(
                f"ERROR: Scan with BSSID {random_network[1].hex()} found network with different BSSID: {network[1].hex()}"
            )
        else:
            found_network = True

    if found_network:
        print(f"SUCCESS: Real world BSSID scan completed and found results")
    else:
        print(
            f"ERROR: Real world BSSID scan did not find the expected network with BSSID {random_network[1].hex()}"
        )


def test_bssid():
    test_bssid_too_long()
    test_bssid_too_short()
    test_bssid_correct()
    test_bssid_real_world()


def test_show_hidden():
    if not wlan.active():
        wlan.active(True)
    try:
        wlan.scan(show_hidden=True)
        print("SUCCESS: Scan with show_hidden=True succeeded")
    except Exception as e:
        print(f"Scan with show_hidden=True failed: {e}")
    try:
        results = wlan.scan(show_hidden=False)
        # TODO: There is no guarantee that there are no hidden networks, so this is a bit of a weak test
        for network in results:
            # Check if the network is a hidden network, since that should be the only ones returned
            if network[5] == True:
                print("ERROR: Scan with show_hidden=False found hidden networks")
        print("SUCCESS: Scan with show_hidden=False succeeded")
    except Exception as e:
        print(f"Scan with show_hidden=False failed: {e}")


def test_active_scan_off():
    import time

    # These settings will result in the ~10ms * 15 channels for a passive scan, well under 1 second
    # And an active scan that should take around 22 seconds via ~1.5s * 15 channels.
    # Allowing for a clear distinction between the an active and passive scan
    test_data = {
        "active_scan": False,
        "scan_time_active_min": 1400,
        "scan_time_active_max": 1500,
        "scan_time_passive": 10,
    }

    if not wlan.active():
        wlan.active(True)

    try:
        start_time = time.ticks_ms()
        wlan.scan(**test_data)
        end_time = time.ticks_ms()
        elapsed_time = time.ticks_diff(end_time, start_time)
        # The elapsed time if the passive scan should be well under 1 second, compared to a 15+ active scan
        if elapsed_time < 1000:
            print(f"SUCCESS: Scan with active_scan=False completed in correct time")
        elif elapsed_time > 5000:
            print(
                f"ERROR: Scan with active_scan=False appears to have used the active scan times: {elapsed_time} ms"
            )
        else:
            print(f"ERROR: Scan with active_scan=False took too long: {elapsed_time} ms")
    except Exception as e:
        print(f"ERROR: Scan with active_scan=False failed: {e}")


def test_active_scan_on():
    import time

    # These settings should result in an active scan that takes is significantly shorter than the passive scan
    test_data = {
        "active_scan": True,
        "scan_time_active_min": 0,
        "scan_time_active_max": 20,
        "scan_time_passive": 1500,
    }

    if not wlan.active():
        wlan.active(True)

    try:
        start_time = time.ticks_ms()
        wlan.scan(**test_data)
        end_time = time.ticks_ms()
        elapsed_time = time.ticks_diff(end_time, start_time)
        # The elapsed time on the active scan is not as fast as the passive, but still seems reliable under 5 seconds
        # I believe that the in my noisy environment, meaning I get a lot of results and the processing of those responses
        # might be what causes this to take longer than expected.
        if elapsed_time < 5000:
            print(f"SUCCESS: Scan with active_scan=True completed in correct time")
        elif elapsed_time > 10000:
            print(
                f"ERROR: Scan with active_scan=True appears to have used the passive scan times: {elapsed_time} ms"
            )
        else:
            print(f"ERROR: Scan with active_scan=True took too long: {elapsed_time} ms")
    except Exception as e:
        print(f"ERROR: Scan with active_scan=True failed: {e}")


def test_active_scan():
    test_active_scan_off()
    test_active_scan_on()


def test_non_blocking_scan_valid():
    import time

    MAX_WAIT_TIME = 15000  # Maximum wait time in milliseconds
    loop_counter = 0

    # The callback function to be called when the scan completes
    def callback_function(interface, callback_event):
        if callback_event[0] != True:
            print(f"ERROR: Callback function received a failed status: {callback_event[0]}")
            return
        # TODO: Should the scan results be checked here somehow?
        if len(callback_event[1]) == 0:
            print("ERROR: Callback function received no networks found")
        else:
            print(f"SUCCESS: Callback function had networks found")

    try:
        start_time = time.ticks_ms()
        wlan.scan(callback=callback_function, blocking=False)
        elapsed_time = time.ticks_diff(time.ticks_ms(), start_time)

        # This first elapsed_time should be tiny since the scan is running in the background
        if elapsed_time > 1000:
            print(
                f"ERROR: Scan with valid callback and non-blocking mode took more than 1 second to start: {elapsed_time} ms"
            )

        # This loop will run at the same time as the scan is running in the background
        # A timeout is set to ensure that the test does not run indefinitely
        while wlan.scan_in_progress() and elapsed_time < MAX_WAIT_TIME:
            # Increment the loop counter to show work is being done
            loop_counter += 1
            elapsed_time = time.ticks_diff(time.ticks_ms(), start_time)

        if elapsed_time >= MAX_WAIT_TIME:
            print("ERROR: Scan did not complete within the maximum wait time")
        elif loop_counter < 1:
            print(
                "ERROR: Scan did not perform any iterations in the loop, indicating background processing did not occur"
            )
        else:
            print(f"SUCCESS: Scan completed with valid callback in background mode")
    except Exception as e:
        print(f"ERROR: Scan with valid callback and non-blocking mode failed: {e}")


def test_non_blocking_no_callback():
    try:
        wlan.scan(blocking=False)
        print("ERROR: Scan with no callback and non-blocking mode succeeded instead of failing")
    except TypeError as e:
        print(
            "SUCCESS: Scan with no callback and non-blocking mode failed with TypeError as expected."
        )


def test_non_blocking_invalid_callback():
    non_callable = "This is not a callable"
    try:
        wlan.scan(callback=non_callable, blocking=False)
        print("ERROR: Scan with non-callable callback succeeded instead of failing")
    except TypeError as e:
        print("SUCCESS: Scan with non-callable callback failed with TypeError as expected.")


def test_blocking_scan_with_callback():
    def callback_function(interface, callback_event):
        # This callback should not be called in blocking mode
        print("ERROR: Callback function should not be called in blocking mode")

    try:
        wlan.scan(callback=callback_function, blocking=True)
    except TypeError as e:
        print(
            "SUCCESS: Scan with callback and blocking mode failed with RuntimeError as expected."
        )


scan_running = None


def test_non_blocking_background_scan():
    # While test_non_blocking_scan_valid() does have a basic iterator in a background loop, using a timer is a second good check
    MAX_TIMEOUT = 10000
    # Channel count * max active time * 1.15 for some margin of error
    ALLOWED_TIME = 3300 * 1.15

    import time

    global scan_running
    scan_running = False

    def scan_callback(interface, event):
        global scan_running
        # Stop the scan
        scan_running = None

    scan_options = {
        # Set three channels
        "channel": [1, 2, 3],
        "scan_time_active_min": 1000,
        "scan_time_active_max": 1100,
        "callback": scan_callback,
        "blocking": False,
    }

    scan_running = True
    wlan.scan(**scan_options)

    start_time = time.ticks_ms()
    elapsed_time = None
    while scan_running:
        elapsed_time = time.ticks_ms()

        if time.ticks_diff(elapsed_time, start_time) > MAX_TIMEOUT:
            print("ERROR: test_non_blocking_background_scan failed with a timeout")
            break
    else:
        elapsed_time = time.ticks_ms()
        overall_time = time.ticks_diff(elapsed_time, start_time)
        if overall_time < ALLOWED_TIME:
            print("SUCCESS: Complete test_non_blocking_background_scan")
        else:
            print("ERROR: test_non_blocking_background_scan took longer than expected")


def test_non_blocking_scan():
    if not wlan.active():
        wlan.active(True)

    test_non_blocking_no_callback()
    test_non_blocking_invalid_callback()
    test_blocking_scan_with_callback()
    test_non_blocking_scan_valid()
    test_non_blocking_background_scan()


def test_scan_already_running():
    import time

    MAX_WAIT_TIME = 15000  # Maximum wait time in milliseconds

    def scan_callback(interface, callback_event):
        # This callback is just to ensure the scan completes
        pass

    wlan.scan(
        blocking=False, callback=scan_callback, scan_time_active_min=750, scan_time_active_max=1000
    )
    try:
        wlan.scan()
        print("ERROR: Scan while another scan is running succeeded instead of failing.")
    except RuntimeError as e:
        print("SUCCESS: Scan while another scan is running failed with RuntimeError as expected.")

    # Wait for the scan to complete with a maximum wait time
    start_time = time.ticks_ms()
    elapsed_time = 0
    while wlan.scan_in_progress() and elapsed_time < MAX_WAIT_TIME:
        elapsed_time = time.ticks_diff(time.ticks_ms(), start_time)

    if elapsed_time >= MAX_WAIT_TIME:
        print(
            "ERROR: Scan did not complete within the maximum wait time after scan_already_running test."
        )


def run_average(num_runs: int, should_print: bool = True):
    import time

    total_time = 0
    print_step = max(1, num_runs // 10)  # Print every 10% of the runs
    for count in range(num_runs):
        if should_print and count % print_step == 0:
            print(f"Running scan {count + 1}/{num_runs}...")
        start_time = time.ticks_ms()
        wlan.scan()
        end_time = time.ticks_ms()
        elapsed_time = time.ticks_diff(end_time, start_time)
        total_time += elapsed_time
    average_time = total_time / num_runs
    if should_print:
        print(f"Average scan time over {num_runs} runs: {average_time} ms")
    return average_time


def non_blocking_average(num_runs: int, should_print: bool = True):
    import time

    def callback_function(interface, callback_event):
        # This callback is just to ensure the scan completes
        pass

    total_time = 0
    print_step = max(1, num_runs // 10)  # Print every 10% of the runs
    for count in range(num_runs):
        if should_print and count % print_step == 0:
            print(f"Running non-blocking scan {count + 1}/{num_runs}...")
        start_time = time.ticks_ms()
        wlan.scan(blocking=False, callback=callback_function)
        while not wlan.scan_in_progress():
            # Wait for the scan to complete
            pass
        end_time = time.ticks_ms()
        elapsed_time = time.ticks_diff(end_time, start_time)
        total_time += elapsed_time
    average_time = total_time / num_runs
    if should_print:
        print(f"Average non-blocking scan time over {num_runs} runs: {average_time} ms")
    return average_time


def test_bad_user_callback_no_inputs():
    def bad_user_callback_no_inputs():
        print("ERROR: bad_user_callback_no_inputs should not execute")

    wlan.scan(blocking=False, callback=bad_user_callback_no_inputs)

    while wlan.scan_in_progress():
        pass


def test_bad_user_callback_one_input():
    def bad_user_callback_one_input(interface):
        print("ERROR: bad_user_callback_one_input should not execute")

    wlan.scan(blocking=False, callback=bad_user_callback_one_input)
    while wlan.scan_in_progress():
        pass


def test_bad_user_callback_three_input():
    def bad_user_callback_three_input(interface, event, other):
        print("ERROR: bad_user_callback_three_input should not execute")

    wlan.scan(blocking=False, callback=bad_user_callback_three_input)
    while wlan.scan_in_progress():
        pass


def test_good_user_callback():
    callback_called = False

    def good_user_callback(interface, event):
        nonlocal callback_called
        callback_called = True
        if not event[0]:
            print("ERROR: good_user_callback received a failed status")
        elif len(event[1]) == 0:
            print("ERROR: good_user_callback received no networks found")
        else:
            print("SUCCESS: good_user_callback received networks found")

    wlan.scan(blocking=False, callback=good_user_callback)
    while wlan.scan_in_progress():
        pass

    if not callback_called:
        print("ERROR: good_user_callback was not called")
    else:
        print("SUCCESS: good_user_callback was called")


def test_bad_user_callbacks():
    test_bad_user_callback_no_inputs()
    test_bad_user_callback_one_input()
    test_bad_user_callback_three_input()


if __name__ == "__main__":
    # If this is the second time this is running after a soft reset, skip the tests
    if RUN_TESTS:
        test_int_channel_scan()
        test_list_channel_scan()
        test_show_hidden()
        test_ssid()
        test_bssid()
        test_ssid_bssid_combined_scan()
        test_scan_times()
        test_active_scan()
        test_non_blocking_scan()
        test_scan_already_running()
        test_bad_user_callbacks()
        test_good_user_callback()
        test_non_blocking_soft_reboot()
