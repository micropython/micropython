// Copyright 2014 Paul Sokolovsky.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "re1.5.h"

#define INSERT_CODE(at, num, pc) \
    ((code ? memmove(code + at + num, code + at, pc - at) : 0), pc += num)
#define REL(at, to) (to - at - 2)
#define EMIT(at, byte) (code ? (code[at] = byte) : (at))
#define EMIT_CHECKED(at, byte) (_emit_checked(at, code, byte, &err))
#define PC (prog->bytelen)

static void _emit_checked(int at, char *code, int val, bool *err) {
    *err |= val != (int8_t)val;
    if (code) {
        code[at] = val;
    }
}

static const char *_compilecode(const char *re, ByteProg *prog, int sizecode)
{
    char *code = sizecode ? NULL : prog->insts;
    bool err = false;
    int start = PC;
    int term = PC;
    int alt_label = 0;

    for (; *re && *re != ')'; re++) {
        switch (*re) {
        case '\\':
            re++;
            if (!*re) return NULL; // Trailing backslash
            if ((*re | 0x20) == 'd' || (*re | 0x20) == 's' || (*re | 0x20) == 'w') {
                term = PC;
                EMIT(PC++, NamedClass);
                EMIT(PC++, *re);
                prog->len++;
                break;
            }
            MP_FALLTHROUGH
        default:
            term = PC;
            EMIT(PC++, Char);
            EMIT(PC++, *re);
            prog->len++;
            break;
        case '.':
            term = PC;
            EMIT(PC++, Any);
            prog->len++;
            break;
        case '[': {
            int cnt;
            term = PC;
            re++;
            if (*re == '^') {
                EMIT(PC++, ClassNot);
                re++;
            } else {
                EMIT(PC++, Class);
            }
            PC++; // Skip # of pair byte
            prog->len++;
            for (cnt = 0; *re != ']'; re++, cnt++) {
                if (*re == '\\') {
                    ++re;
                }
                if (!*re) return NULL;
                EMIT(PC++, *re);
                if (re[1] == '-' && re[2] != ']') {
                    re += 2;
                }
                EMIT(PC++, *re);
            }
            EMIT_CHECKED(term + 1, cnt);
            break;
        }
        case '(': {
            term = PC;
            int sub = 0;
            int capture = re[1] != '?' || re[2] != ':';

            if (capture) {
                sub = ++prog->sub;
                EMIT(PC++, Save);
                EMIT_CHECKED(PC++, 2 * sub);
                prog->len++;
            } else {
                    re += 2;
            }

            re = _compilecode(re + 1, prog, sizecode);
            if (re == NULL || *re != ')') return NULL; // error, or no matching paren

            if (capture) {
                EMIT(PC++, Save);
                EMIT_CHECKED(PC++, 2 * sub + 1);
                prog->len++;
            }

            break;
        }
        case '?':
            if (PC == term) return NULL; // nothing to repeat
            INSERT_CODE(term, 2, PC);
            if (re[1] == '?') {
                EMIT(term, RSplit);
                re++;
            } else {
                EMIT(term, Split);
            }
            EMIT_CHECKED(term + 1, REL(term, PC));
            prog->len++;
            term = PC;
            break;
        case '*':
            if (PC == term) return NULL; // nothing to repeat
            INSERT_CODE(term, 2, PC);
            EMIT(PC, Jmp);
            EMIT_CHECKED(PC + 1, REL(PC, term));
            PC += 2;
            if (re[1] == '?') {
                EMIT(term, RSplit);
                re++;
            } else {
                EMIT(term, Split);
            }
            EMIT_CHECKED(term + 1, REL(term, PC));
            prog->len += 2;
            term = PC;
            break;
        case '+':
            if (PC == term) return NULL; // nothing to repeat
            if (re[1] == '?') {
                EMIT(PC, Split);
                re++;
            } else {
                EMIT(PC, RSplit);
            }
            EMIT_CHECKED(PC + 1, REL(PC, term));
            PC += 2;
            prog->len++;
            term = PC;
            break;
        case '|':
            if (alt_label) {
                EMIT_CHECKED(alt_label, REL(alt_label, PC) + 1);
            }
            INSERT_CODE(start, 2, PC);
            EMIT(PC++, Jmp);
            alt_label = PC++;
            EMIT(start, Split);
            EMIT_CHECKED(start + 1, REL(start, PC));
            prog->len += 2;
            term = PC;
            break;
        case '^':
            EMIT(PC++, Bol);
            prog->len++;
            term = PC;
            break;
        case '$':
            EMIT(PC++, Eol);
            prog->len++;
            term = PC;
            break;
        }
    }

    if (alt_label) {
        EMIT_CHECKED(alt_label, REL(alt_label, PC) + 1);
    }
    return err ? NULL : re;
}

int re1_5_sizecode(const char *re)
{
    ByteProg dummyprog = {
         // Save 0, Save 1, Match; more bytes for "search" (vs "match") prefix code
        .bytelen = 5 + NON_ANCHORED_PREFIX
    };

    if (_compilecode(re, &dummyprog, /*sizecode*/1) == NULL) return -1;

    return dummyprog.bytelen;
}

int re1_5_compilecode(ByteProg *prog, const char *re)
{
    prog->len = 0;
    prog->bytelen = 0;
    prog->sub = 0;

    // Add code to implement non-anchored operation ("search"),
    // for anchored operation ("match"), this code will be just skipped.
    // TODO: Implement search in much more efficient manner
    prog->insts[prog->bytelen++] = RSplit;
    prog->insts[prog->bytelen++] = 3;
    prog->insts[prog->bytelen++] = Any;
    prog->insts[prog->bytelen++] = Jmp;
    prog->insts[prog->bytelen++] = -5;
    prog->len += 3;

    prog->insts[prog->bytelen++] = Save;
    prog->insts[prog->bytelen++] = 0;
    prog->len++;

    re = _compilecode(re, prog, /*sizecode*/0);
    if (re == NULL || *re) return 1;

    prog->insts[prog->bytelen++] = Save;
    prog->insts[prog->bytelen++] = 1;
    prog->len++;

    prog->insts[prog->bytelen++] = Match;
    prog->len++;

    return 0;
}

#if 0
int main(int argc, char *argv[])
{
    int pc = 0;
    ByteProg *code = re1_5_compilecode(argv[1]);
    re1_5_dumpcode(code);
}
#endif
