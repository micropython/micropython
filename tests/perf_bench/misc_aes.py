# Pure Python AES encryption routines.
#
# AES is integer based and inplace so doesn't use the heap.  It is therefore
# a good test of raw performance and correctness of the VM/runtime.
#
# The AES code comes first (code originates from a C version authored by D.P.George)
# and then the test harness at the bottom.
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

##################################################################
# discrete arithmetic routines, mostly from a precomputed table

# non-linear, invertible, substitution box
aes_s_box_table = bytes((
    0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
    0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
    0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
    0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
    0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
    0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
    0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
    0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
    0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
    0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
    0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
    0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
    0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
    0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
    0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
    0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16,
))

# multiplication of polynomials modulo x^8 + x^4 + x^3 + x + 1 = 0x11b
def aes_gf8_mul_2(x):
    if x & 0x80:
        return (x << 1) ^ 0x11b
    else:
        return x << 1

def aes_gf8_mul_3(x):
    return x ^ aes_gf8_mul_2(x)

# non-linear, invertible, substitution box
def aes_s_box(a):
    return aes_s_box_table[a & 0xff]

# return 0x02^(a-1) in GF(2^8)
def aes_r_con(a):
    ans = 1
    while a > 1:
        ans <<= 1;
        if ans & 0x100:
            ans ^= 0x11b
        a -= 1
    return ans

##################################################################
# basic AES algorithm; see FIPS-197

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
        temp[temp_idx]     = aes_gf8_mul_2(x0) ^ aes_gf8_mul_3(x1) ^ x2 ^ x3 ^ w[w_idx]
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
        temp[temp_idx]     = x0 ^ w[w_idx]
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
        self.state_pos = 16;

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

###########################################################################
# Benchmark interface

bm_params = {
    (50, 25): (1, 16),
    (100, 100): (1, 32),
    (1000, 1000): (4, 256),
    (5000, 1000): (20, 256),
}

def bm_setup(params):
    nloop, datalen = params

    aes = AES(256)
    key = bytearray(256 // 8)
    iv = bytearray(16)
    data = bytearray(datalen)
    # from now on we don't use the heap

    def run():
        for loop in range(nloop):
            # encrypt
            aes.set_key(key)
            aes.set_iv(iv)
            for i in range(2):
                aes.apply_to(data)

            # decrypt
            aes.set_key(key)
            aes.set_iv(iv)
            for i in range(2):
                aes.apply_to(data)

            # verify
            for i in range(len(data)):
                assert data[i] == 0

    def result():
        return params[0] * params[1], True

    return run, result
