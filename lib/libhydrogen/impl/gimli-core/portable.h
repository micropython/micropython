static void
gimli_core(uint32_t state[gimli_BLOCKBYTES / 4])
{
    unsigned int round;
    unsigned int column;
    uint32_t     x;
    uint32_t     y;
    uint32_t     z;

    for (round = 24; round > 0; round--) {
        for (column = 0; column < 4; column++) {
            x = ROTL32(state[column], 24);
            y = ROTL32(state[4 + column], 9);
            z = state[8 + column];

            state[8 + column] = x ^ (z << 1) ^ ((y & z) << 2);
            state[4 + column] = y ^ x ^ ((x | z) << 1);
            state[column]     = z ^ y ^ ((x & y) << 3);
        }
        switch (round & 3) {
        case 0:
            x        = state[0];
            state[0] = state[1];
            state[1] = x;
            x        = state[2];
            state[2] = state[3];
            state[3] = x;
            state[0] ^= ((uint32_t) 0x9e377900 | round);
            break;
        case 2:
            x        = state[0];
            state[0] = state[2];
            state[2] = x;
            x        = state[1];
            state[1] = state[3];
            state[3] = x;
        }
    }
}
