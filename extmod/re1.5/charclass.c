#include "re1.5.h"

int _re1_5_classmatch(const char *pc, const char *sp)
{
    // pc points to "cnt" byte after opcode
    int is_positive = (pc[-1] == Class);
    int cnt = *pc++;
    while (cnt--) {
        if (*sp >= *pc && *sp <= pc[1]) return is_positive;
        pc += 2;
    }
    return !is_positive;
}
