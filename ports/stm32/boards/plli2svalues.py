"""
This program computes I2S PLL parameters for STM32
processors supporting an I2S PLL in the clock tree.
Those processors are listed below in the mcu_support_plli2s[] list.
"""

import re
from collections import namedtuple


class MCU:
    def __init__(self, range_plli2sn, range_plli2sr):
        self.range_plli2sn = range_plli2sn
        self.range_plli2sr = range_plli2sr


mcu_default = MCU(range_plli2sn=range(50, 432 + 1), range_plli2sr=range(2, 7 + 1))

mcu_table = {"stm32f401xe": MCU(range_plli2sn=range(192, 432 + 1), range_plli2sr=range(2, 7 + 1))}

# list of stm32 processors that support an I2S PLL in the clock tree
mcu_support_plli2s = [
    "stm32f405xx",
    "stm32f401xe",
    "stm32f407xx",
    "stm32f411xe",
    "stm32f412zx",
    "stm32f413xx",
    "stm32f427xx",
    "stm32f429xx",
    "stm32f439xx",
    "stm32f446xx",
    "stm32f722xx",
    "stm32f733xx",
    "stm32f746xx",
    "stm32f756xx",
    "stm32f767xx",
    "stm32f769xx",
]


# The following function calculates the multiplier (plli2sn) and divider (plli2sr) parameters
# for the I2S PLL that leads to the best possible accuracy for the I2S sampling clock.
# This is done by creating a set of candidate parameters (plli2sn, plli2sr)
# and then determining which candidate parameters lead to a sampling clock frequency (Fs)
# that most closely matches the desired sampling clock frequency (I2S rate).
#
# A description of the clock tree helps to understand this function:
# The clock tree on a STM32 device is complex.  A subset of the clock tree is used for I2S, as follows:
# 1. HSE clock is divided by the PLLM divider and feeds the I2S PLL (called PLLI2S in the STM32 clock tree).
# 2. The output frequency of the I2S PLL is controlled by two parameters, plli2sn and plli2sr.
# 3. The clock output of the I2S PLL is called PLLI2SCLK
# 4. PLLI2SCLK is gated into the I2S peripheral, where the name changes to I2SxCLK
# 5. I2SxCLK is an input to the I2S clock generator.
# 6. The output frequency of the I2S clock generator is controlled by
#    two configuration parameters, I2SDIV and ODD.
# 7. The output of the I2S clock generator is the audio sampling frequency (Fs),
#    which is used to create the signal at the I2S WS output pin.
#
# Example references:
#    RM0090 Reference manual STM32F405/415, STM32F407/417, STM32F427/437 and STM32F429/439
#    - section 6.3.23 RCC PLLI2S configuration register (RCC_PLLI2SCFGR)
#    - section 28.4.4 Clock generator
#    RM0385 Reference manual STM32F75xxx and STM32F74xxx
#    - section 5.3.23 RCC PLLI2S configuration register (RCC_PLLI2SCFGR)
#    - section 32.7.5 Clock generator
#
# The calculations below mimic the fixed-point integer calculations in the STM32 I2S driver,
# in the function HAL_I2S_Init().
def compute_plli2s_table(hse, pllm):
    plli2s = namedtuple("plli2s", "bits rate plli2sn plli2sr i2sdiv odd error")
    plli2s_table = []
    for bits in (16, 32):
        for rate in (8_000, 11_025, 12_000, 16_000, 22_050, 24_000, 32_000, 44_100, 48_000):
            plli2s_candidates = []
            for plli2sn in mcu.range_plli2sn:
                for plli2sr in mcu.range_plli2sr:
                    I2SxCLK = hse // pllm * plli2sn // plli2sr
                    if I2SxCLK < 192_000_000:
                        # compute I2S clock generator parameters: i2sdiv, odd
                        tmp = (((I2SxCLK // (bits * 2)) * 10) // rate) + 5
                        tmp = tmp // 10
                        odd = tmp & 1
                        i2sdiv = (tmp - odd) // 2
                        Fs = I2SxCLK / ((bits * 2) * ((2 * i2sdiv) + odd))
                        error = (abs(Fs - rate) / rate) * 100
                        plli2s_candidates.append(
                            plli2s(
                                bits=bits,
                                rate=rate,
                                plli2sn=plli2sn,
                                plli2sr=plli2sr,
                                i2sdiv=i2sdiv,
                                odd=odd,
                                error=error,
                            )
                        )
            # sort based on error
            plli2s_candidates_sorted = sorted(plli2s_candidates, key=lambda x: x.error)
            # select the best candidate
            plli2s_table.append(plli2s_candidates_sorted[0])
    return plli2s_table


def generate_c_table(plli2s_table, hse, pllm):
    print("// MAKE generated file, created by plli2svalues.py: DO NOT EDIT")
    print("// This table is used in machine_i2s.c")
    print(f"// HSE_VALUE = {hse}")
    print(f"// MICROPY_HW_CLK_PLLM = {pllm} \n")
    print("#define PLLI2S_TABLE \\")
    print("{ \\")

    for plli2s in plli2s_table:
        print(
            f"    {{{plli2s.rate}, "
            f"{plli2s.bits}, "
            f"{plli2s.plli2sr}, "
            f"{plli2s.plli2sn} }}, "
            f"/* i2sdiv: {int(plli2s.i2sdiv)}, "
            f"odd: {plli2s.odd}, "
            f"rate error % (desired vs actual)%: {plli2s.error:.4f} */ \\"
        )
    print("}")


def search_header(filename, re_define, lookup):
    regex_define = re.compile(re_define)
    val = None
    with open(filename) as f:
        for line in f:
            line = line.strip()
            m = regex_define.match(line)
            if m:
                # found lookup value
                found = m.group(2)
                if m.group(1) == lookup:
                    val = eval(found)
    return val


def main():
    global mcu

    # parse input args
    import sys

    argv = sys.argv[1:]

    c_table = False
    mcu_series = "stm32f4"
    hse = None
    pllm = None

    while True:
        if argv[0] == "-c":
            c_table = True
            argv.pop(0)
        elif argv[0] == "-m":
            argv.pop(0)
            mcu_series = argv.pop(0).lower()
        else:
            break

    if mcu_series in mcu_support_plli2s:
        if len(argv) not in (1, 2):
            print("usage: pllvalues.py [-c] [-m <mcu_series>] <hse in MHz> <pllm in MHz>")
            sys.exit(1)

        if argv[0].startswith("hse:"):
            hse = int(argv[0][len("hse:") :])

        if argv[0].startswith("pllm:"):
            pllm = int(argv[0][len("pllm:") :])

        if argv[0].startswith("file:"):
            # extract hse value from processed header files
            hse = search_header(
                argv[0][len("file:") :],
                r"static.* (micropy_hw_hse_value) = +([0-9 +-/\*()]+);",
                "micropy_hw_hse_value",
            )
            if hse is None:
                raise ValueError(
                    "%s does not contain a definition of micropy_hw_hse_value" % argv[0]
                )

            # extract pllm value from processed header files
            pllm = search_header(
                argv[0][len("file:") :],
                r"static.* (micropy_hw_clk_pllm) = +([0-9 +-/\*()]+);",
                "micropy_hw_clk_pllm",
            )
            if pllm is None:
                raise ValueError(
                    "%s does not contain a definition of micropy_hw_clk_pllm" % argv[0]
                )
            argv.pop(0)

        # Select MCU parameters
        mcu = mcu_default
        for m in mcu_table:
            if mcu_series.startswith(m):
                mcu = mcu_table[m]
                break
        plli2s_table = compute_plli2s_table(hse, pllm)
        if c_table:
            generate_c_table(plli2s_table, hse, pllm)


if __name__ == "__main__":
    main()
