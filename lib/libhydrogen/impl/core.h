int
hydro_init(void)
{
    if (hydro_random_init() != 0) {
        abort();
    }
    return 0;
}

void
hydro_memzero(void *pnt, size_t len)
{
#ifdef HAVE_EXPLICIT_BZERO
    explicit_bzero(pnt, len);
#else
    volatile unsigned char *volatile pnt_ = (volatile unsigned char *volatile) pnt;
    size_t i                              = (size_t) 0U;

    while (i < len) {
        pnt_[i++] = 0U;
    }
#endif
}

void
hydro_increment(uint8_t *n, size_t len)
{
    size_t        i;
    uint_fast16_t c = 1U;

    for (i = 0; i < len; i++) {
        c += (uint_fast16_t) n[i];
        n[i] = (uint8_t) c;
        c >>= 8;
    }
}

char *
hydro_bin2hex(char *hex, size_t hex_maxlen, const uint8_t *bin, size_t bin_len)
{
    size_t       i = (size_t) 0U;
    unsigned int x;
    int          b;
    int          c;

    if (bin_len >= SIZE_MAX / 2 || hex_maxlen <= bin_len * 2U) {
        abort();
    }
    while (i < bin_len) {
        c = bin[i] & 0xf;
        b = bin[i] >> 4;
        x = (unsigned char) (87U + c + (((c - 10U) >> 8) & ~38U)) << 8 |
            (unsigned char) (87U + b + (((b - 10U) >> 8) & ~38U));
        hex[i * 2U] = (char) x;
        x >>= 8;
        hex[i * 2U + 1U] = (char) x;
        i++;
    }
    hex[i * 2U] = 0U;

    return hex;
}

int
hydro_hex2bin(uint8_t *bin, size_t bin_maxlen, const char *hex, size_t hex_len, const char *ignore,
              const char **hex_end_p)
{
    size_t        bin_pos = (size_t) 0U;
    size_t        hex_pos = (size_t) 0U;
    int           ret     = 0;
    unsigned char c;
    unsigned char c_alpha0, c_alpha;
    unsigned char c_num0, c_num;
    uint8_t       c_acc = 0U;
    uint8_t       c_val;
    unsigned char state = 0U;

    while (hex_pos < hex_len) {
        c        = (unsigned char) hex[hex_pos];
        c_num    = c ^ 48U;
        c_num0   = (c_num - 10U) >> 8;
        c_alpha  = (c & ~32U) - 55U;
        c_alpha0 = ((c_alpha - 10U) ^ (c_alpha - 16U)) >> 8;
        if ((c_num0 | c_alpha0) == 0U) {
            if (ignore != NULL && state == 0U && strchr(ignore, c) != NULL) {
                hex_pos++;
                continue;
            }
            break;
        }
        c_val = (uint8_t)((c_num0 & c_num) | (c_alpha0 & c_alpha));
        if (bin_pos >= bin_maxlen) {
            ret   = -1;
            errno = ERANGE;
            break;
        }
        if (state == 0U) {
            c_acc = c_val * 16U;
        } else {
            bin[bin_pos++] = c_acc | c_val;
        }
        state = ~state;
        hex_pos++;
    }
    if (state != 0U) {
        hex_pos--;
        errno = EINVAL;
        ret   = -1;
    }
    if (ret != 0) {
        bin_pos = (size_t) 0U;
    }
    if (hex_end_p != NULL) {
        *hex_end_p = &hex[hex_pos];
    } else if (hex_pos != hex_len) {
        errno = EINVAL;
        ret   = -1;
    }
    if (ret != 0) {
        return ret;
    }
    return (int) bin_pos;
}

bool
hydro_equal(const void *b1_, const void *b2_, size_t len)
{
    const volatile uint8_t *volatile b1 = (const volatile uint8_t *volatile) b1_;
    const uint8_t *b2                   = (const uint8_t *) b2_;
    size_t         i;
    uint8_t        d = (uint8_t) 0U;

    if (b1 == b2) {
        d = ~d;
    }
    for (i = 0U; i < len; i++) {
        d |= b1[i] ^ b2[i];
    }
    return (bool) (1 & ((d - 1) >> 8));
}

int
hydro_compare(const uint8_t *b1_, const uint8_t *b2_, size_t len)
{
    const volatile uint8_t *volatile b1 = (const volatile uint8_t *volatile) b1_;
    const uint8_t *b2                   = (const uint8_t *) b2_;
    uint8_t        gt                   = 0U;
    uint8_t        eq                   = 1U;
    size_t         i;

    i = len;
    while (i != 0U) {
        i--;
        gt |= ((b2[i] - b1[i]) >> 8) & eq;
        eq &= ((b2[i] ^ b1[i]) - 1) >> 8;
    }
    return (int) (gt + gt + eq) - 1;
}

int
hydro_pad(unsigned char *buf, size_t unpadded_buflen, size_t blocksize, size_t max_buflen)
{
    unsigned char *        tail;
    size_t                 i;
    size_t                 xpadlen;
    size_t                 xpadded_len;
    volatile unsigned char mask;
    unsigned char          barrier_mask;

    if (blocksize <= 0U || max_buflen > INT_MAX) {
        return -1;
    }
    xpadlen = blocksize - 1U;
    if ((blocksize & (blocksize - 1U)) == 0U) {
        xpadlen -= unpadded_buflen & (blocksize - 1U);
    } else {
        xpadlen -= unpadded_buflen % blocksize;
    }
    if (SIZE_MAX - unpadded_buflen <= xpadlen) {
        return -1;
    }
    xpadded_len = unpadded_buflen + xpadlen;
    if (xpadded_len >= max_buflen) {
        return -1;
    }
    tail = &buf[xpadded_len];
    mask = 0U;
    for (i = 0; i < blocksize; i++) {
        barrier_mask = (unsigned char) (((i ^ xpadlen) - 1U) >> ((sizeof(size_t) - 1U) * CHAR_BIT));
        tail[-i]     = (tail[-i] & mask) | (0x80 & barrier_mask);
        mask |= barrier_mask;
    }
    return (int) (xpadded_len + 1);
}

int
hydro_unpad(const unsigned char *buf, size_t padded_buflen, size_t blocksize)
{
    const unsigned char *tail;
    unsigned char        acc = 0U;
    unsigned char        c;
    unsigned char        valid   = 0U;
    volatile size_t      pad_len = 0U;
    size_t               i;
    size_t               is_barrier;

    if (padded_buflen < blocksize || blocksize <= 0U) {
        return -1;
    }
    tail = &buf[padded_buflen - 1U];

    for (i = 0U; i < blocksize; i++) {
        c          = tail[-i];
        is_barrier = (((acc - 1U) & (pad_len - 1U) & ((c ^ 0x80) - 1U)) >> 8) & 1U;
        acc |= c;
        pad_len |= (i & -is_barrier);
        valid |= (unsigned char) is_barrier;
    }
    if (valid == 0) {
        return -1;
    }
    return (int) (padded_buflen - 1 - pad_len);
}
