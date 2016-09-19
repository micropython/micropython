"""
This is an auxiliary script that is used to compute valid PLL values to set
the CPU frequency to a given value.  The algorithm here appears as C code
for the machine.freq() function.
"""

def close_int(x):
    return abs(x - round(x)) < 0.01

# original version that requires N/M to be an integer (for simplicity)
def compute_pll(hse, sys):
    for P in (2, 4, 6, 8): # allowed values of P
        Q = sys * P / 48
        NbyM = sys * P / hse
        # N/M and Q must be integers
        if not (close_int(NbyM) and close_int(Q)):
            continue
        # VCO_OUT must be between 192MHz and 432MHz
        if not (192 <= hse * NbyM <= 432):
            continue
        # compute M
        M = int(192 // NbyM)
        while hse > 2 * M or NbyM * M < 192:
            M += 1
        # VCO_IN must be between 1MHz and 2MHz (2MHz recommended)
        if not (M <= hse):
            continue
        # compute N
        N = NbyM * M
        # N and Q are restricted
        if not (192 <= N <= 432 and 2 <= Q <= 15):
            continue
        # found valid values
        assert NbyM == N // M
        return (M, N, P, Q)
    # no valid values found
    return None

# improved version that doesn't require N/M to be an integer
def compute_pll2(hse, sys):
    for P in (2, 4, 6, 8): # allowed values of P
        Q = sys * P / 48
        # Q must be an integer in a set range
        if not (close_int(Q) and 2 <= Q <= 15):
            continue
        NbyM = sys * P / hse
        # VCO_OUT must be between 192MHz and 432MHz
        if not (192 <= hse * NbyM <= 432):
            continue
        # compute M
        M = 192 // NbyM # starting value
        while hse > 2 * M or NbyM * M < 192 or not close_int(NbyM * M):
            M += 1
        # VCO_IN must be between 1MHz and 2MHz (2MHz recommended)
        if not (M <= hse):
            continue
        # compute N
        N = NbyM * M
        # N must be an integer
        if not close_int(N):
            continue
        # N is restricted
        if not (192 <= N <= 432):
            continue
        # found valid values
        return (M, N, P, Q)
    # no valid values found
    return None

def verify_and_print_pll(hse, sys, pll):
    M, N, P, Q = pll

    # compute derived quantities
    vco_in = hse / M
    vco_out = hse * N / M
    pllck = hse / M * N / P
    pll48ck = hse / M * N / Q

    # verify ints
    assert close_int(M)
    assert close_int(N)
    assert close_int(P)
    assert close_int(Q)

    # verify range
    assert 2 <= M <= 63
    assert 192 <= N <= 432
    assert P in (2, 4, 6, 8)
    assert 2 <= Q <= 15
    assert 1 <= vco_in <= 2
    assert 192 <= vco_out <= 432

    # print out values
    print(out_format % (sys, M, N, P, Q, vco_in, vco_out, pllck, pll48ck))

def main():
    global out_format
    import sys
    if len(sys.argv) != 2:
        print("usage: pllvalues.py <hse in MHz>")
        sys.exit(1)
    hse_value = int(sys.argv[1])
    print("HSE =", hse_value, "MHz")
    print("sys :  M      N     P     Q : VCO_IN VCO_OUT   PLLCK PLL48CK")
    out_format = "%3u : %2u  %.1f  %.2f  %.2f :  %5.2f  %6.2f  %6.2f  %6.2f"
    n_valid = 0
    for sysclk in range(1, 217):
        pll = compute_pll2(hse_value, sysclk)
        if pll is not None:
            n_valid += 1
            verify_and_print_pll(hse_value, sysclk, pll)
    print("found %u valid configurations" % n_valid)

if __name__ == "__main__":
    main()
