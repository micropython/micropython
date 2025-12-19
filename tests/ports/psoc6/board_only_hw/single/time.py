import time


def test_ticks_ms():
    print("\n***** Test 1: ticks_ms() *****\n")
    t0 = time.ticks_ms()
    time.sleep_ms(1)
    t1 = time.ticks_ms()
    diff = time.ticks_diff(t1, t0)
    tick_val = [t0, t1, diff, 0 <= diff <= 1]
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
    tick_val = [t0, t1, diff, 0 <= diff <= 500]
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


test_ticks_ms()
test_ticks_us()
test_ticks_cpu()
test_boundary_us_cond()
# Enable this test only if needed for advanced checking
# test_us_deviation()
