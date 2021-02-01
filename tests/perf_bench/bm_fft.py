# Copyright (c) 2019 Project Nayuki. (MIT License)
# https://www.nayuki.io/page/free-small-fft-in-multiple-languages

import math, cmath


def transform_radix2(vector, inverse):
    # Returns the integer whose value is the reverse of the lowest 'bits' bits of the integer 'x'.
    def reverse(x, bits):
        y = 0
        for i in range(bits):
            y = (y << 1) | (x & 1)
            x >>= 1
        return y

    # Initialization
    n = len(vector)
    levels = int(math.log2(n))
    coef = (2 if inverse else -2) * cmath.pi / n
    exptable = [cmath.rect(1, i * coef) for i in range(n // 2)]
    vector = [vector[reverse(i, levels)] for i in range(n)]  # Copy with bit-reversed permutation

    # Radix-2 decimation-in-time FFT
    size = 2
    while size <= n:
        halfsize = size // 2
        tablestep = n // size
        for i in range(0, n, size):
            k = 0
            for j in range(i, i + halfsize):
                temp = vector[j + halfsize] * exptable[k]
                vector[j + halfsize] = vector[j] - temp
                vector[j] += temp
                k += tablestep
        size *= 2
    return vector


###########################################################################
# Benchmark interface

bm_params = {
    (50, 25): (2, 128),
    (100, 100): (3, 256),
    (1000, 1000): (20, 512),
    (5000, 1000): (100, 512),
}


def bm_setup(params):
    state = None
    signal = [math.cos(2 * math.pi * i / params[1]) + 0j for i in range(params[1])]
    fft = None
    fft_inv = None

    def run():
        nonlocal fft, fft_inv
        for _ in range(params[0]):
            fft = transform_radix2(signal, False)
            fft_inv = transform_radix2(fft, True)

    def result():
        nonlocal fft, fft_inv
        fft[1] -= 0.5 * params[1]
        fft[-1] -= 0.5 * params[1]
        fft_ok = all(abs(f) < 1e-3 for f in fft)
        for i in range(len(fft_inv)):
            fft_inv[i] -= params[1] * signal[i]
        fft_inv_ok = all(abs(f) < 1e-3 for f in fft_inv)
        return params[0] * params[1], (fft_ok, fft_inv_ok)

    return run, result
