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

def compute_derived(hse, pll):
    M, N, P, Q = pll
    vco_in = hse / M
    vco_out = hse * N / M
    pllck = hse / M * N / P
    pll48ck = hse / M * N / Q
    return (vco_in, vco_out, pllck, pll48ck)

def verify_pll(hse, pll):
    M, N, P, Q = pll
    vco_in, vco_out, pllck, pll48ck = compute_derived(hse, pll)

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

def generate_c_table(hse, valid_plls):
    valid_plls = valid_plls + [(16, (0, 0, 2, 0))]
    if hse < 16:
        valid_plls.append((hse, (1, 0, 2, 0)))
    valid_plls.sort()
    print("// (M, P/2-1, SYS) values for %u MHz HSE" % hse)
    print("static const uint16_t pll_freq_table[%u] = {" % len(valid_plls))
    for sys, (M, N, P, Q) in valid_plls:
        print("    (%u << 10) | (%u << 8) | %u," % (M, P // 2 - 1, sys))
    print("};")

def print_table(hse, valid_plls):
    print("HSE =", hse, "MHz")
    print("sys :  M      N     P     Q : VCO_IN VCO_OUT   PLLCK PLL48CK")
    out_format = "%3u : %2u  %.1f  %.2f  %.2f :  %5.2f  %6.2f  %6.2f  %6.2f"
    for sys, pll in valid_plls:
        print(out_format % ((sys,) + pll + compute_derived(hse, pll)))
    print("found %u valid configurations" % len(valid_plls))

def main():
    global out_format

    # parse input args
    import sys
    argv = sys.argv[1:]

    c_table = False
    if argv[0] == '-c':
        c_table = True
        argv.pop(0)

    if len(argv) != 1:
        print("usage: pllvalues.py [-c] <hse in MHz>")
        sys.exit(1)

    if argv[0].startswith("file:"):
        # extract HSE_VALUE from header file
        with open(argv[0][5:]) as f:
            for line in f:
                line = line.strip()
                if line.startswith("#define") and line.find("HSE_VALUE") != -1:
                    idx_start = line.find("((uint32_t)") + 11
                    idx_end = line.find(")", idx_start)
                    hse = int(line[idx_start:idx_end]) // 1000000
                    break
            else:
                raise ValueError("%s does not contain a definition of HSE_VALUE" % argv[0])
    else:
        # HSE given directly as an integer
        hse = int(argv[0])

    valid_plls = []
    for sysclk in range(1, 217):
        pll = compute_pll2(hse, sysclk)
        if pll is not None:
            verify_pll(hse, pll)
            valid_plls.append((sysclk, pll))

    if c_table:
        generate_c_table(hse, valid_plls)
    else:
        print_table(hse, valid_plls)

if __name__ == "__main__":
    main()
