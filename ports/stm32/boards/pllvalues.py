"""
This is an auxiliary script that is used to compute valid PLL values to set
the CPU frequency to a given value.  The algorithm here appears as C code
for the machine.freq() function.
"""

import re


class MCU:
    def __init__(
        self, range_sysclk, range_m, range_n, range_p, range_q, range_vco_in, range_vco_out
    ):
        self.range_sysclk = range_sysclk
        self.range_m = range_m
        self.range_n = range_n
        self.range_p = range_p
        self.range_q = range_q
        self.range_vco_in = range_vco_in
        self.range_vco_out = range_vco_out


mcu_default = MCU(
    range_sysclk=range(2, 216 + 1, 2),
    range_m=range(2, 63 + 1),
    range_n=range(192, 432 + 1),
    range_p=range(2, 8 + 1, 2),
    range_q=range(2, 15 + 1),
    range_vco_in=range(1, 2 + 1),
    range_vco_out=range(192, 432 + 1),
)

mcu_table = {
    "stm32f413": MCU(
        range_sysclk=range(2, 100 + 1, 2),
        range_m=range(2, 63 + 1),
        range_n=range(50, 432 + 1),
        range_p=range(2, 8 + 1, 2),
        range_q=range(2, 15 + 1),
        range_vco_in=range(1, 2 + 1),
        range_vco_out=range(100, 432 + 1),
    ),
    "stm32h5": MCU(
        range_sysclk=range(2, 250 + 1, 2),
        range_m=range(1, 63 + 1),
        range_n=range(4, 512 + 1),
        range_p=range(2, 128 + 1, 2),
        range_q=range(1, 128 + 1),
        range_vco_in=range(1, 16 + 1),
        range_vco_out=range(150, 836 + 1),  # 150-420=medium, 192-836=wide
    ),
    "stm32h7": MCU(
        range_sysclk=range(2, 400 + 1, 2),  # above 400MHz currently unsupported
        range_m=range(1, 63 + 1),
        range_n=range(4, 512 + 1),
        range_p=range(2, 128 + 1, 2),
        range_q=range(1, 128 + 1),
        range_vco_in=range(1, 16 + 1),
        range_vco_out=range(150, 960 + 1),  # 150-420=medium, 192-960=wide
    ),
}


def close_int(x):
    return abs(x - round(x)) < 0.01


# original version that requires N/M to be an integer (for simplicity)
def compute_pll(hse, sys):
    for P in (2, 4, 6, 8):  # allowed values of P
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
def compute_pll2(hse, sys, relax_pll48):
    # Loop over the allowed values of P, looking for a valid PLL configuration
    # that gives the desired "sys" frequency.
    fallback = None
    for P in mcu.range_p:
        # VCO_OUT must be between 192MHz and 432MHz
        if sys * P not in mcu.range_vco_out:
            continue
        NbyM = sys * P / hse
        # scan M
        M_min = mcu.range_n[0] // int(round(NbyM))  # starting value
        while mcu.range_vco_in[-1] * M_min < hse:
            M_min += 1
        # VCO_IN must be >=1MHz, but higher is better for stability so start high (low M)
        for M in range(M_min, hse + 1):
            # compute N
            N = NbyM * M
            # N must be an integer
            if not close_int(N):
                continue
            N = round(N)
            # N is restricted
            if N not in mcu.range_n:
                continue
            Q = sys * P / 48
            # Q must be an integer in a set range
            if close_int(Q) and round(Q) in mcu.range_q:
                # found valid values
                return (M, N, P, Q)
            # Re-try Q to get at most 48MHz
            Q = (sys * P + 47) // 48
            if Q not in mcu.range_q:
                continue
            if fallback is None:
                # the values don't give 48MHz on PLL48 but are otherwise OK
                fallback = M, N, P, Q
    if relax_pll48:
        # might have found values which don't give 48MHz on PLL48
        return fallback
    else:
        # no valid values found which give 48MHz on PLL48
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
    assert M in mcu.range_m
    assert N in mcu.range_n
    assert P in mcu.range_p
    assert Q in mcu.range_q
    assert mcu.range_vco_in[0] <= vco_in <= mcu.range_vco_in[-1]
    assert mcu.range_vco_out[0] <= vco_out <= mcu.range_vco_out[-1]


def compute_pll_table(source_clk, relax_pll48):
    valid_plls = []
    for sysclk in mcu.range_sysclk:
        pll = compute_pll2(source_clk, sysclk, relax_pll48)
        if pll is not None:
            verify_pll(source_clk, pll)
            valid_plls.append((sysclk, pll))
    return valid_plls


def generate_c_table(hse, valid_plls):
    valid_plls.sort()
    if (
        mcu.range_sysclk[-1] <= 0xFF
        and mcu.range_m[-1] <= 0x3F
        and mcu.range_p[-1] // 2 - 1 <= 0x3
    ):
        typedef = "uint16_t"
        sys_mask = 0xFF
        m_shift = 10
        m_mask = 0x3F
        p_shift = 8
        p_mask = 0x3
    else:
        typedef = "uint32_t"
        sys_mask = 0xFFFF
        m_shift = 24
        m_mask = 0xFF
        p_shift = 16
        p_mask = 0xFF
    print("#define PLL_FREQ_TABLE_SYS(pll) ((pll) & %d)" % (sys_mask,))
    print("#define PLL_FREQ_TABLE_M(pll) (((pll) >> %d) & %d)" % (m_shift, m_mask))
    print("#define PLL_FREQ_TABLE_P(pll) (((((pll) >> %d) & %d) + 1) * 2)" % (p_shift, p_mask))
    print("typedef %s pll_freq_table_t;" % (typedef,))
    print("// (M, P/2-1, SYS) values for %u MHz source" % hse)
    print("static const pll_freq_table_t pll_freq_table[%u] = {" % (len(valid_plls),))
    for sys, (M, N, P, Q) in valid_plls:
        print("    (%u << %u) | (%u << %u) | %u," % (M, m_shift, P // 2 - 1, p_shift, sys), end="")
        if M >= 2:
            vco_in, vco_out, pllck, pll48ck = compute_derived(hse, (M, N, P, Q))
            print(
                " // M=%u N=%u P=%u Q=%u vco_in=%.2f vco_out=%.2f pll48=%.2f"
                % (M, N, P, Q, vco_in, vco_out, pll48ck),
                end="",
            )
        print()
    print("};")


def print_table(hse, valid_plls):
    print("HSE =", hse, "MHz")
    print("sys :  M      N     P     Q : VCO_IN VCO_OUT   PLLCK PLL48CK")
    out_format = "%3u : %2u  %.1f  %.2f  %.2f :  %5.2f  %6.2f  %6.2f  %6.2f"
    for sys, pll in valid_plls:
        print(out_format % ((sys,) + pll + compute_derived(hse, pll)))
    print("found %u valid configurations" % len(valid_plls))


def search_header_for_hsx_values(filename):
    hse = hsi = None
    regex_def = re.compile(
        r"static.* +(micropy_hw_hs[ei]_value) = +([0-9 +-/\*()]+);",
    )
    with open(filename) as f:
        for line in f:
            line = line.strip()
            m = regex_def.match(line)
            if m:
                # Found HSE_VALUE or HSI_VALUE
                found = m.group(2)
                val = eval(found) // 1000000
                if m.group(1) == "micropy_hw_hse_value":
                    hse = val
                else:
                    hsi = val
    return hse, hsi


def main():
    global mcu
    global out_format

    # parse input args
    import sys

    argv = sys.argv[1:]

    c_table = False
    mcu_series = "stm32f4"
    hse = None
    hsi = None

    while True:
        if argv[0] == "-c":
            c_table = True
            argv.pop(0)
        elif argv[0] == "-m":
            argv.pop(0)
            mcu_series = argv.pop(0).lower()
        else:
            break

    if len(argv) != 1:
        print("usage: pllvalues.py [-c] [-m <mcu_series>] <hse in MHz>")
        sys.exit(1)

    if argv[0].startswith("file:"):
        # extract HSE_VALUE, and optionally HSI_VALUE, from header file
        hse, hsi = search_header_for_hsx_values(argv[0][5:])
        if hse is None:
            raise ValueError("%s does not contain a definition of micropy_hw_hse_value" % argv[0])
    else:
        # HSE given directly as an integer
        hse = int(argv[0])

    # Select MCU parameters
    mcu = mcu_default
    for m in mcu_table:
        if mcu_series.startswith(m):
            mcu = mcu_table[m]
            break

    # Relax constraint on PLLQ being 48MHz on MCUs which have separate PLLs for 48MHz
    relax_pll48 = mcu_series.startswith(("stm32f413", "stm32f7", "stm32h5", "stm32h7", "stm32n6"))

    hse_valid_plls = compute_pll_table(hse, relax_pll48)
    if hsi is not None:
        hsi_valid_plls = compute_pll_table(hsi, relax_pll48)

    if c_table:
        print("#if MICROPY_HW_CLK_USE_HSI")
        if hsi is not None:
            hsi_valid_plls.append((hsi, (0, 0, 2, 0)))
            generate_c_table(hsi, hsi_valid_plls)
        print("#else")
        if hsi is not None:
            hse_valid_plls.append((hsi, (0, 0, 2, 0)))
        hse_valid_plls.append((hse, (1, 0, 2, 0)))
        generate_c_table(hse, hse_valid_plls)
        print("#endif")
    else:
        print_table(hse, hse_valid_plls)


if __name__ == "__main__":
    main()
