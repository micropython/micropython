def bm_run(N, M):
    try:
        from time import ticks_us, ticks_diff
    except ImportError:
        from time import perf_counter

        ticks_us = lambda: int(perf_counter() * 1000000)
        ticks_diff = lambda a, b: a - b

    # Pick sensible parameters given N, M
    cur_nm = (0, 0)
    param = None
    for nm, p in bm_params.items():
        if 10 * nm[0] <= 12 * N and nm[1] <= M and nm > cur_nm:
            cur_nm = nm
            param = p
    if param is None:
        print(-1, -1, "SKIP: no matching params")
        return

    # Run and time benchmark
    run, result = bm_setup(param)
    t0 = ticks_us()
    run()
    t1 = ticks_us()
    norm, out = result()
    print(ticks_diff(t1, t0), norm, out)
