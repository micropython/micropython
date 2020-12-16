# test utime resolutions

try:
    import utime
except ImportError:
    print("SKIP")
    raise SystemExit


def gmtime_time():
    return utime.gmtime(utime.time())


def localtime_time():
    return utime.localtime(utime.time())


def test():
    TEST_TIME = 2500
    EXPECTED_MAP = (
        # (function name, min. number of results in 2.5 sec)
        ("time", 3),
        ("gmtime", 3),
        ("localtime", 3),
        ("gmtime_time", 3),
        ("localtime_time", 3),
        ("ticks_ms", 15),
        ("ticks_us", 15),
        ("ticks_ns", 15),
        ("ticks_cpu", 15),
    )

    # call time functions
    results_map = {}
    end_time = utime.ticks_ms() + TEST_TIME
    while utime.ticks_diff(end_time, utime.ticks_ms()) > 0:
        utime.sleep_ms(100)
        for func_name, _ in EXPECTED_MAP:
            try:
                time_func = getattr(utime, func_name, None) or globals()[func_name]
                now = time_func()  # may raise AttributeError
            except (KeyError, AttributeError):
                continue
            try:
                results_map[func_name].add(now)
            except KeyError:
                results_map[func_name] = {now}

    # check results
    for func_name, min_len in EXPECTED_MAP:
        print("Testing %s" % func_name)
        results = results_map.get(func_name)
        if results is None:
            pass
        elif func_name == "ticks_cpu" and results == {0}:
            # ticks_cpu() returns 0 on some ports (e.g. unix)
            pass
        elif len(results) < min_len:
            print(
                "%s() returns %s result%s in %s ms, expecting >= %s"
                % (func_name, len(results), "s"[: len(results) != 1], TEST_TIME, min_len)
            )


test()
