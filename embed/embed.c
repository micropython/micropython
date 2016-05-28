// demo embedding MicroPython into a C program

#include "upy.h"

static uint8_t heap0[64*1024];
static uint8_t heap1[32*1024];

int main(int argc, char **argv) {
    mp_state_ctx_t *mp0 = micropy_create(heap0, sizeof(heap0));
    mp_state_ctx_t *mp1 = micropy_create(heap1, sizeof(heap1));
    micropy_exec_str(mp0, "print([x + 1 for x in range(10)])\nx=1");
    micropy_exec_str(mp0, "print(globals())\n");
    micropy_exec_str(mp1, "try: print(x)\nexcept: print('x not defined')");
    micropy_destroy(mp0);
    micropy_destroy(mp1);
    return 0;
}
