import time


def test_ticks_ms():
    print("\n***** Test 1: ticks_ms() *****\n")
    t0 = time.ticks_ms()
    time.sleep_ms(1)
    t1 = time.ticks_ms()
    diff = time.ticks_diff(t1, t0)
    tick_val = [
        t0,
        t1,
        diff,
        0 <= diff <= 2,
    ]  # allow up to 2ms: FreeRTOS 1ms tick can add one extra tick
    if tick_val[3]:
        print("Status: PASS")
    else:
        print("Status: FAIL")


def test_ticks_us():
    print("\n***** Test 2: ticks_us() *****\n")
    t0 = time.ticks_us()
    time.sleep_us(1)
    t1 = time.ticks_us()
    diff = time.ticks_diff(t1, t0)
    tick_val = [t0, t1, diff, 0 <= diff <= 500]
    if tick_val[3]:
        print("Status: PASS")
    else:
        print("Status: FAIL")


def test_ticks_cpu():
    # ticks_cpu may not be implemented, at least make sure it doesn't decrease'''
    print("\n***** Test 3: ticks_cpu() *****\n")
    t0 = time.ticks_cpu()
    time.sleep_us(1)
    t1 = time.ticks_cpu()
    diff = time.ticks_diff(t1, t0)
    tick_val = [t0, t1, diff, diff >= 0]
    if tick_val[3]:
        print("Status: PASS")
    else:
        print("Status: FAIL")


def test_boundary_us_cond():
    print("\n***** Test 4: Checking boundary conditions *****\n")
    max_tick = 15000000
    tick_var = []
    for i in range(1, 11):
        t0 = time.ticks_us()
        time.sleep_us(max_tick - t0)
        t1 = time.ticks_us()
        diff = time.ticks_diff(t1, t0)
        tick_val = [t0, t1, diff, diff >= 0]
        tick_var.append(tick_val)

    # At least 7 out of 10 should pass
    pass_count = 0
    for i in range(0, 10):
        if tick_var[i][3]:
            pass_count += 1

    if pass_count >= 7:
        print("PASS")
    else:
        print("FAIL")


def test_us_deviation():
    print("\n***** Test 5: Checking wrap around condition *****\n")
    for i in range(150):
        t0 = time.ticks_us()
        time.sleep_us(i)
        t1 = time.ticks_us()
        print("[instant, t0, t1, diff] : [", i, t0, t1, time.ticks_diff(t1, t0), "]")


def test_time_time_vs_time_ns():
    print("\n***** Test 6: time() vs time_ns() *****\n")
    # Retry a few times to avoid second-boundary races between the two calls.
    match = True
    for _ in range(5):
        # t_ns = time.time_ns()
        # t_s = time.time()
        if int(time.time_ns() // 1_000_000_000) != time.time():
            match = False
            print(
                "time() and time_ns() mismatch: time()=",
                time.time(),
                " time_ns()=",
                time.time_ns(),
            )
        time.sleep_ms(1)

    if match:
        print("Status: PASS")
    else:
        print("Status: FAIL")


def test_time_ns_strictly_monotonic():
    print("\n***** Test 7: time_ns() strict monotonicity *****\n")
    prev = time.time_ns()
    monotonic = True
    for _ in range(50):
        cur = time.time_ns()
        if cur <= prev:
            monotonic = False
            break
        prev = cur

    if monotonic:
        print("Status: PASS")
    else:
        print("Status: FAIL")


def test_time_elapsed_matches_delay():
    print("\n***** Test 8: Delay vs elapsed time *****\n")
    delays_ms = [20, 50, 100, 250]

    # Use second-scale delays to validate elapsed time with time().
    delays_s = [1, 2]

    # Tolerance is intentionally relaxed for scheduler/clock granularity variance.
    tolerance_ns = 1_000_000  # 1 ms tolerance for time_ns()
    tolerance_s = 1

    all_ok = True
    for delay_ms in delays_ms:
        t0_ns = time.time_ns()
        time.sleep_ms(delay_ms)
        t1_ns = time.time_ns()

        elapsed_ns = t1_ns - t0_ns
        target_ns = delay_ms * 1_000_000

        ns_ok = (elapsed_ns >= target_ns - tolerance_ns) and (
            elapsed_ns <= target_ns + tolerance_ns
        )

        if not ns_ok:
            all_ok = False
            print("Delay(ms)=", delay_ms, " elapsed_ns=", elapsed_ns, " -> FAIL")

    for delay_s in delays_s:
        t0_s = time.time()
        time.sleep(delay_s)
        t1_s = time.time()

        elapsed_s = t1_s - t0_s
        s_ok = (elapsed_s >= delay_s) and (elapsed_s <= delay_s + tolerance_s)

        if not s_ok:
            all_ok = False
            print("Delay(s)=", delay_s, " elapsed_s=", elapsed_s, " -> FAIL")

    if all_ok:
        print("Status: PASS")
    else:
        print("Status: FAIL")


test_ticks_ms()
test_ticks_us()
test_ticks_cpu()
test_boundary_us_cond()
test_time_time_vs_time_ns()
test_time_ns_strictly_monotonic()
test_time_elapsed_matches_delay()
# Enable this test only if needed for advanced checking
# test_us_deviation()
