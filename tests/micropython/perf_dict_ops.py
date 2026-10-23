# Benchmark dict operations: creation, lookup, insert, delete, iteration.

# Dict sizes to test
SMALL = 10
MEDIUM = 100
LARGE = 500


def bm_setup(params):
    import time

    N = params[0]
    ITERS = params[1]
    return N, ITERS, time.ticks_us, time.ticks_diff


def bm_run(N, ITERS, ticks_us, ticks_diff):
    # 1. Dict creation (comprehension)
    t0 = ticks_us()
    for _ in range(ITERS):
        d = {i: i for i in range(MEDIUM)}
    t1 = ticks_us()
    print("create_100:", ticks_diff(t1, t0))

    # 2. Insertion into empty dict
    t0 = ticks_us()
    for _ in range(ITERS):
        d = {}
        for i in range(MEDIUM):
            d[i] = i
    t1 = ticks_us()
    print("insert_100:", ticks_diff(t1, t0))

    # 3. Lookup hit (qstr keys -- simulates attribute access)
    d = {}
    keys = []
    for i in range(MEDIUM):
        k = "key_{}".format(i)
        d[k] = i
        keys.append(k)
    t0 = ticks_us()
    for _ in range(ITERS):
        for k in keys:
            d[k]
    t1 = ticks_us()
    print("lookup_qstr_100:", ticks_diff(t1, t0))

    # 4. Lookup hit (int keys)
    d = {i: i for i in range(MEDIUM)}
    t0 = ticks_us()
    for _ in range(ITERS):
        for i in range(MEDIUM):
            d[i]
    t1 = ticks_us()
    print("lookup_int_100:", ticks_diff(t1, t0))

    # 5. Lookup miss
    d = {i: i for i in range(MEDIUM)}
    t0 = ticks_us()
    for _ in range(ITERS):
        for i in range(MEDIUM, MEDIUM * 2):
            i in d
    t1 = ticks_us()
    print("lookup_miss_100:", ticks_diff(t1, t0))

    # 6. Iteration
    d = {i: i for i in range(MEDIUM)}
    t0 = ticks_us()
    for _ in range(ITERS * 10):
        for k in d:
            pass
    t1 = ticks_us()
    print("iterate_100:", ticks_diff(t1, t0))

    # 7. Delete + add cycles (tombstone/compact stress)
    d = {i: i for i in range(MEDIUM)}
    t0 = ticks_us()
    for _ in range(ITERS):
        for i in range(MEDIUM):
            del d[i]
            d[i + MEDIUM] = i
        for i in range(MEDIUM):
            del d[i + MEDIUM]
            d[i] = i
    t1 = ticks_us()
    print("del_add_cycle_100:", ticks_diff(t1, t0))

    # 8. popitem drain
    t0 = ticks_us()
    for _ in range(ITERS):
        d = {i: i for i in range(MEDIUM)}
        while d:
            d.popitem()
    t1 = ticks_us()
    print("popitem_drain_100:", ticks_diff(t1, t0))

    # 9. Global variable access (most critical path)
    # This is measured at module level below
    pass


bm_params = {
    (1000, 100): (MEDIUM, 100),
    (5000, 100): (MEDIUM, 500),
}
