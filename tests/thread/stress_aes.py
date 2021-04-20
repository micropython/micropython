# Stress test for threads using AES encryption routines.
#
# AES was chosen because it is integer based and inplace so doesn't use the
# heap.  It is therefore a good test of raw performance and correctness of the
# VM/runtime.  It can be used to measure threading performance (concurrency is
# in principle possible) and correctness (it's non trivial for the encryption/
# decryption to give the correct answer).
#
# The AES code comes first (code originates from a C version authored by D.P.George)
# and then the test harness at the bottom.  It can be tuned to be more/less
# aggressive by changing the amount of data to encrypt, the number of loops and
# the number of threads.
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

##################################################################
# discrete arithmetic routines, mostly from a precomputed table

# non-linear, invertible, substitution box
aes_s_box_table = bytes(
    (
        0x63,
        0x7C,
        0x77,
        0x7B,
        0xF2,
        0x6B,
        0x6F,
        0xC5,
        0x30,
        0x01,
        0x67,
        0x2B,
        0xFE,
        0xD7,
        0xAB,
        0x76,
        0xCA,
        0x82,
        0xC9,
        0x7D,
        0xFA,
        0x59,
        0x47,
        0xF0,
        0xAD,
        0xD4,
        0xA2,
        0xAF,
        0x9C,
        0xA4,
        0x72,
        0xC0,
        0xB7,
        0xFD,
        0x93,
        0x26,
        0x36,
        0x3F,
        0xF7,
        0xCC,
        0x34,
        0xA5,
        0xE5,
        0xF1,
        0x71,
        0xD8,
        0x31,
        0x15,
        0x04,
        0xC7,
        0x23,
        0xC3,
        0x18,
        0x96,
        0x05,
        0x9A,
        0x07,
        0x12,
        0x80,
        0xE2,
        0xEB,
        0x27,
        0xB2,
        0x75,
        0x09,
        0x83,
        0x2C,
        0x1A,
        0x1B,
        0x6E,
        0x5A,
        0xA0,
        0x52,
        0x3B,
        0xD6,
        0xB3,
        0x29,
        0xE3,
        0x2F,
        0x84,
        0x53,
        0xD1,
        0x00,
        0xED,
        0x20,
        0xFC,
        0xB1,
        0x5B,
        0x6A,
        0xCB,
        0xBE,
        0x39,
        0x4A,
        0x4C,
        0x58,
        0xCF,
        0xD0,
        0xEF,
        0xAA,
        0xFB,
        0x43,
        0x4D,
        0x33,
        0x85,
        0x45,
        0xF9,
        0x02,
        0x7F,
        0x50,
        0x3C,
        0x9F,
        0xA8,
        0x51,
        0xA3,
        0x40,
        0x8F,
        0x92,
        0x9D,
        0x38,
        0xF5,
        0xBC,
        0xB6,
        0xDA,
        0x21,
        0x10,
        0xFF,
        0xF3,
        0xD2,
        0xCD,
        0x0C,
        0x13,
        0xEC,
        0x5F,
        0x97,
        0x44,
        0x17,
        0xC4,
        0xA7,
        0x7E,
        0x3D,
        0x64,
        0x5D,
        0x19,
        0x73,
        0x60,
        0x81,
        0x4F,
        0xDC,
        0x22,
        0x2A,
        0x90,
        0x88,
        0x46,
        0xEE,
        0xB8,
        0x14,
        0xDE,
        0x5E,
        0x0B,
        0xDB,
        0xE0,
        0x32,
        0x3A,
        0x0A,
        0x49,
        0x06,
        0x24,
        0x5C,
        0xC2,
        0xD3,
        0xAC,
        0x62,
        0x91,
        0x95,
        0xE4,
        0x79,
        0xE7,
        0xC8,
        0x37,
        0x6D,
        0x8D,
        0xD5,
        0x4E,
        0xA9,
        0x6C,
        0x56,
        0xF4,
        0xEA,
        0x65,
        0x7A,
        0xAE,
        0x08,
        0xBA,
        0x78,
        0x25,
        0x2E,
        0x1C,
        0xA6,
        0xB4,
        0xC6,
        0xE8,
        0xDD,
        0x74,
        0x1F,
        0x4B,
        0xBD,
        0x8B,
        0x8A,
        0x70,
        0x3E,
        0xB5,
        0x66,
        0x48,
        0x03,
        0xF6,
        0x0E,
        0x61,
        0x35,
        0x57,
        0xB9,
        0x86,
        0xC1,
        0x1D,
        0x9E,
        0xE1,
        0xF8,
        0x98,
        0x11,
        0x69,
        0xD9,
        0x8E,
        0x94,
        0x9B,
        0x1E,
        0x87,
        0xE9,
        0xCE,
        0x55,
        0x28,
        0xDF,
        0x8C,
        0xA1,
        0x89,
        0x0D,
        0xBF,
        0xE6,
        0x42,
        0x68,
        0x41,
        0x99,
        0x2D,
        0x0F,
        0xB0,
        0x54,
        0xBB,
        0x16,
    )
)

# multiplication of polynomials modulo x^8 + x^4 + x^3 + x + 1 = 0x11b
def aes_gf8_mul_2(x):
    if x & 0x80:
        return (x << 1) ^ 0x11B
    else:
        return x << 1


def aes_gf8_mul_3(x):
    return x ^ aes_gf8_mul_2(x)


# non-linear, invertible, substitution box
def aes_s_box(a):
    return aes_s_box_table[a & 0xFF]


# return 0x02^(a-1) in GF(2^8)
def aes_r_con(a):
    ans = 1
    while a > 1:
        ans <<= 1
        if ans & 0x100:
            ans ^= 0x11B
        a -= 1
    return ans


##################################################################
# basic AES algorithm; see FIPS-197
#
# Think of it as a pseudo random number generator, with each
# symbol in the sequence being a 16 byte block (the state).  The
# key is a parameter of the algorithm and tells which particular
# sequence of random symbols you want.  The initial vector, IV,
# sets the start of the sequence.  The idea of a strong cipher
# is that it's very difficult to guess the key even if you know
# a large part of the sequence.  The basic AES algorithm simply
# provides such a sequence.  En/de-cryption is implemented here
# using OCB, where the sequence is xored against the plaintext.
# Care must be taken to (almost) always choose a different IV.

# all inputs must be size 16
def aes_add_round_key(state, w):
    for i in range(16):
        state[i] ^= w[i]


# combined sub_bytes, shift_rows, mix_columns, add_round_key
# all inputs must be size 16
def aes_sb_sr_mc_ark(state, w, w_idx, temp):
    temp_idx = 0
    for i in range(4):
        x0 = aes_s_box_table[state[i * 4]]
        x1 = aes_s_box_table[state[1 + ((i + 1) & 3) * 4]]
        x2 = aes_s_box_table[state[2 + ((i + 2) & 3) * 4]]
        x3 = aes_s_box_table[state[3 + ((i + 3) & 3) * 4]]
        temp[temp_idx] = aes_gf8_mul_2(x0) ^ aes_gf8_mul_3(x1) ^ x2 ^ x3 ^ w[w_idx]
        temp[temp_idx + 1] = x0 ^ aes_gf8_mul_2(x1) ^ aes_gf8_mul_3(x2) ^ x3 ^ w[w_idx + 1]
        temp[temp_idx + 2] = x0 ^ x1 ^ aes_gf8_mul_2(x2) ^ aes_gf8_mul_3(x3) ^ w[w_idx + 2]
        temp[temp_idx + 3] = aes_gf8_mul_3(x0) ^ x1 ^ x2 ^ aes_gf8_mul_2(x3) ^ w[w_idx + 3]
        w_idx += 4
        temp_idx += 4
    for i in range(16):
        state[i] = temp[i]


# combined sub_bytes, shift_rows, add_round_key
# all inputs must be size 16
def aes_sb_sr_ark(state, w, w_idx, temp):
    temp_idx = 0
    for i in range(4):
        x0 = aes_s_box_table[state[i * 4]]
        x1 = aes_s_box_table[state[1 + ((i + 1) & 3) * 4]]
        x2 = aes_s_box_table[state[2 + ((i + 2) & 3) * 4]]
        x3 = aes_s_box_table[state[3 + ((i + 3) & 3) * 4]]
        temp[temp_idx] = x0 ^ w[w_idx]
        temp[temp_idx + 1] = x1 ^ w[w_idx + 1]
        temp[temp_idx + 2] = x2 ^ w[w_idx + 2]
        temp[temp_idx + 3] = x3 ^ w[w_idx + 3]
        w_idx += 4
        temp_idx += 4
    for i in range(16):
        state[i] = temp[i]


# take state as input and change it to the next state in the sequence
# state and temp have size 16, w has size 16 * (Nr + 1), Nr >= 1
def aes_state(state, w, temp, nr):
    aes_add_round_key(state, w)
    w_idx = 16
    for i in range(nr - 1):
        aes_sb_sr_mc_ark(state, w, w_idx, temp)
        w_idx += 16
    aes_sb_sr_ark(state, w, w_idx, temp)


# expand 'key' to 'w' for use with aes_state
# key has size 4 * Nk, w has size 16 * (Nr + 1), temp has size 16
def aes_key_expansion(key, w, temp, nk, nr):
    for i in range(4 * nk):
        w[i] = key[i]
    w_idx = 4 * nk - 4
    for i in range(nk, 4 * (nr + 1)):
        t = temp
        t_idx = 0
        if i % nk == 0:
            t[0] = aes_s_box(w[w_idx + 1]) ^ aes_r_con(i // nk)
            for j in range(1, 4):
                t[j] = aes_s_box(w[w_idx + (j + 1) % 4])
        elif nk > 6 and i % nk == 4:
            for j in range(0, 4):
                t[j] = aes_s_box(w[w_idx + j])
        else:
            t = w
            t_idx = w_idx
        w_idx += 4
        for j in range(4):
            w[w_idx + j] = w[w_idx + j - 4 * nk] ^ t[t_idx + j]


##################################################################
# simple use of AES algorithm, using output feedback (OFB) mode


class AES:
    def __init__(self, keysize):
        if keysize == 128:
            self.nk = 4
            self.nr = 10
        elif keysize == 192:
            self.nk = 6
            self.nr = 12
        else:
            assert keysize == 256
            self.nk = 8
            self.nr = 14

        self.state = bytearray(16)
        self.w = bytearray(16 * (self.nr + 1))
        self.temp = bytearray(16)
        self.state_pos = 16

    def set_key(self, key):
        aes_key_expansion(key, self.w, self.temp, self.nk, self.nr)
        self.state_pos = 16

    def set_iv(self, iv):
        for i in range(16):
            self.state[i] = iv[i]
        self.state_pos = 16

    def get_some_state(self, n_needed):
        if self.state_pos >= 16:
            aes_state(self.state, self.w, self.temp, self.nr)
            self.state_pos = 0
        n = 16 - self.state_pos
        if n > n_needed:
            n = n_needed
        return n

    def apply_to(self, data):
        idx = 0
        n = len(data)
        while n > 0:
            ln = self.get_some_state(n)
            n -= ln
            for i in range(ln):
                data[idx + i] ^= self.state[self.state_pos + i]
            idx += ln
            self.state_pos += n


##################################################################
# test code

try:
    import utime as time
except ImportError:
    import time
import _thread


class LockedCounter:
    def __init__(self):
        self.lock = _thread.allocate_lock()
        self.value = 0

    def add(self, val):
        self.lock.acquire()
        self.value += val
        self.lock.release()


count = LockedCounter()


def thread_entry():
    global count

    aes = AES(256)
    key = bytearray(256 // 8)
    iv = bytearray(16)
    data = bytearray(128)
    # from now on we don't use the heap

    for loop in range(5):
        # encrypt
        aes.set_key(key)
        aes.set_iv(iv)
        for i in range(8):
            aes.apply_to(data)

        # decrypt
        aes.set_key(key)
        aes.set_iv(iv)
        for i in range(8):
            aes.apply_to(data)

        # verify
        for i in range(len(data)):
            assert data[i] == 0

    count.add(1)


if __name__ == "__main__":
    n_thread = 20
    for i in range(n_thread):
        _thread.start_new_thread(thread_entry, ())
    while count.value < n_thread:
        time.sleep(1)
