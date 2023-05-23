try:
    from ulab import numpy as np
except ImportError:
    import numpy as np


def lpf(fS, f, N, win=lambda N: 1):
    if not (N & 1):
        raise ValueError("filter length must be odd")
    h = np.sinc(2 * f / fS * (np.arange(N) - (N - 1) / 2))
    h = h * win(N)
    return h * (1 / np.sum(h))


def hpf(fS, f, N, win=lambda N: 1):
    if not (N & 1):
        raise ValueError("filter length must be odd")
    h = -lpf(fS, f, N)
    h = h * win(N)
    h[(N - 1) // 2] += 1
    return h


def brf(fS, fL, NL, fH, NH, win=lambda N: 1):
    hlpf = lpf(fS, fL, NL, win)
    hhpf = hpf(fS, fH, NH, win)

    if NH > NL:
        h = hhpf
        h[(NH - NL) // 2 : (NH - NL) // 2 + NL] += hlpf
    else:
        h = hlpf
        h[(NL - NH) // 2 : (NL - NH) // 2 + NH] += hhpf

    return h


def bpf(fS, fL, NL, fH, NH, win=lambda N: 1):
    hlpf = lpf(fS, fL, NL, win)
    hhpf = hpf(fS, fH, NH, win)
    return np.convolve(hlpf, hhpf)


def blackman(M):
    n = np.arange(1 - M, M, 2)
    return 0.42 + 0.5 * np.cos(np.pi * n / (M - 1)) + 0.08 * np.cos(2.0 * np.pi * n / (M - 1))


def tosynthio(coeffs):
    result = np.array(coeffs * 32767, dtype=np.int16)
    return trim_zeros(result)


def trim_zeros(arr):
    i = 0
    j = len(arr) - 1
    while i < len(arr) and arr[i] == 0:
        i += 1
    while j > i and arr[j] == 0:
        j -= 1
    return arr[i : j + 1]


# fiiir.com uses factor 4.6 for blackman window, 0.91 for rectangular
def ntaps(fS, fB, factor=4.6):
    b = fB / fS
    return round(factor / b) | 1


def LPF(*args, **kw):
    return tosynthio(lpf(*args, **kw))


def HPF(*args, **kw):
    return tosynthio(hpf(*args, **kw))


def BRF(*args, **kw):
    return tosynthio(brf(*args, **kw))


def BPF(*args, **kw):
    return tosynthio(bpf(*args, **kw))


if __name__ == "__main__":
    print("lpf(24000, 2040, 13) # 1920Hz transition window")
    print(list(lpf(24000, 2040, 13)))

    print("hpf(24000, 9600, 13) # 960Hz transition window")
    print(list(hpf(24000, 9600, 23)))

    print("bpf(24000, 1200, 11, 3960, 15) #  2400Hz, 1600Hz transition windows")
    print(list(bpf(24000, 1200, 11, 3960, 15)))

    print("brf(24000, 960, 19, 2400, 13) # 1200, 1800Hz transition windows")
    brf_tst = brf(24000, 960, 19, 2400, 13)
    print(brf_tst)

    print("brf(24000, 960, 13, 2400, 19) # 1200, 1800Hz transition windows")
    brf_tst = brf(24000, 960, 13, 2400, 19)
    print(brf_tst)

    print("lpf(1, 0.1, 59, blackman) # 1920Hz transition window, blackman")
    print(lpf(1, 0.1, 59, blackman))
