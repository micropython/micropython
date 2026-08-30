# test time resolutions

try:
    import time
except ImportError:
    print("SKIP")
    raise SystemExit


def test_monotonic_advance(func_name, min_advance_s, sleep_ms):
    """Test that a time function advances within expected bounds."""
    try:
        if func_name.endswith("_time"):
            # Helper functions defined below
            time_func = globals()[func_name]
        else:
            time_func = getattr(time, func_name)
    except AttributeError:
        return None  # Function not available

    try:
        t1 = time_func()
        time.sleep_ms(sleep_ms)
        t2 = time_func()
    except AttributeError:
        # Function exists but calls unavailable time functions (e.g., gmtime_time)
        return None

    # For tuple return values (gmtime, localtime), compare as tuples
    if isinstance(t1, tuple) and isinstance(t2, tuple):
        # Should have changed (checking resolution)
        return t2 != t1
    # For numeric return values (time, ticks_*)
    else:
        # Use appropriate diff function for ticks
        if func_name.startswith("ticks_"):
            diff = time.ticks_diff(t2, t1)
            if func_name == "ticks_ms":
                # Expect 80%-200% of sleep time (tolerance for overhead and loaded CI)
                expected = sleep_ms
                return (expected * 0.8) <= diff <= (expected * 2.0)
            elif func_name == "ticks_us":
                # Expect 80%-200% of sleep time in microseconds
                expected = sleep_ms * 1000
                return (expected * 0.8) <= diff <= (expected * 2.0)
            elif func_name == "ticks_ns":
                # Expect 80%-200% of sleep time in nanoseconds
                expected = sleep_ms * 1000000
                return (expected * 0.8) <= diff <= (expected * 2.0)
            elif func_name == "ticks_cpu":
                # ticks_cpu may return 0 on some ports, just check it advanced
                return diff > 0 or t2 == 0
        else:
            # For time() and other float/int returns, check both bounds
            # 2x tolerance for overhead and loaded CI systems
            min_expected = min_advance_s
            max_expected = (sleep_ms / 1000.0) * 2.0
            actual_diff = t2 - t1
            return min_expected <= actual_diff <= max_expected


def gmtime_time():
    # May raise AttributeError if gmtime not available
    return time.gmtime(time.time())


def localtime_time():
    # May raise AttributeError if localtime not available
    return time.localtime(time.time())


def test():
    # Test configuration: (function name, minimum advance in seconds, sleep time in ms)
    TEST_CONFIG = (
        ("time", 1, 1200),
        ("gmtime", 0, 1200),  # gmtime returns tuple, just check it changes
        ("localtime", 0, 1200),
        ("gmtime_time", 0, 1200),
        ("localtime_time", 0, 1200),
        ("ticks_ms", 0, 150),  # Test millisecond resolution
        ("ticks_us", 0, 150),  # Test microsecond resolution
        ("ticks_ns", 0, 150),  # Test nanosecond resolution
        ("ticks_cpu", 0, 150),
    )

    for func_name, min_advance, sleep_ms in TEST_CONFIG:
        print("Testing %s" % func_name)
        result = test_monotonic_advance(func_name, min_advance, sleep_ms)
        if result is None:
            # Function not available, skip silently
            pass
        elif not result:
            print("%s() did not advance as expected" % func_name)


test()
