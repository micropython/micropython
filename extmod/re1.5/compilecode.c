// Copyright 2014 Paul Sokolovsky.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "re1.5.h"

static void insert_code(char *code, int at, int num, int *pc)
{
    memmove(code + at + num, code + at, *pc - at);
    *pc += num;
}

#define REL(at, to) (to - at - 2)

int re1_5_sizecode(const char *re)
{
    int pc = 5 + NON_ANCHORED_PREFIX; // Save 0, Save 1, Match; more bytes for "search" (vs "match") prefix code

    for (; *re; re++) {
        switch (*re) {
        case '\\':
            re++;
        default:
            pc += 2;
            break;
        case '+':
            // Skip entire "+?"
            if (re[1] == '?')
                re++;
        case '?':
            pc += 2;
            break;
        case '.':
        case '^':
        case '$':
            pc++;
            break;
        case '*':
            // Skip entire "*?"
            if (re[1] == '?')
                re++;
        case '|':
        case '(':
            pc += 4;
            break;
        case ')':
            break;
        case '[': {
            pc += 2;
            re++;
            if (*re == '^') re++;
            while (*re != ']') {
                if (!*re) return -1;
                if (re[1] == '-') {
                    re += 2;
                }
                pc += 2;
                re++;
            }
        }
        }
    }

    return pc;
}

#define EMIT(at, byte) code[at] = byte

const char *_compilecode(const char *re, ByteProg *prog)
{
    char *code = prog->insts;
    int pc = prog->bytelen;
    int start = pc;
    int term = pc;
    int alt_label = 0;

    for (; *re && *re != ')'; re++) {
        switch (*re) {
        case '\\':
            re++;
            if ((*re | 0x20) == 'd' || (*re | 0x20) == 's' || (*re | 0x20) == 'w') {
                term = pc;
                EMIT(pc++, NamedClass);
                EMIT(pc++, *re);
                prog->len++;
                break;
            }
        default:
            term = pc;
            EMIT(pc++, Char);
            EMIT(pc++, *re);
            prog->len++;
            break;
        case '.':
            term = pc;
            EMIT(pc++, Any);
            prog->len++;
            break;
        case '[': {
            int cnt;
            term = pc;
            re++;
            if (*re == '^') {
                EMIT(pc++, ClassNot);
                re++;
            } else {
                EMIT(pc++, Class);
            }
            pc++; // Skip # of pair byte
            prog->len++;
            for (cnt = 0; *re != ']'; re++, cnt++) {
                if (!*re) return NULL;
                EMIT(pc++, *re);
                if (re[1] == '-') {
                    re += 2;
                }
                EMIT(pc++, *re);
            }
            EMIT(term + 1, cnt);
            break;
        }
        case '(': {
            term = pc;
            int sub = ++prog->sub;

            EMIT(pc++, Save);
            EMIT(pc++, 2 * sub);
            prog->len++;

            prog->bytelen = pc;
            re = _compilecode(re + 1, prog);
            pc = prog->bytelen;

            EMIT(pc++, Save);
            EMIT(pc++, 2 * sub + 1);
            prog->len++;

            break;
        }
        case '?':
            insert_code(code, term, 2, &pc);
            EMIT(term, Split);
            EMIT(term + 1, REL(term, pc));
            prog->len++;
            break;
        case '*':
            insert_code(code, term, 2, &pc);
            EMIT(pc, Jmp);
            EMIT(pc + 1, REL(pc, term));
            pc += 2;
            if (re[1] == '?') {
                EMIT(term, RSplit);
                re++;
            } else {
                EMIT(term, Split);
            }
            EMIT(term + 1, REL(term, pc));
            prog->len += 2;
            break;
        case '+':
            if (re[1] == '?') {
                EMIT(pc, Split);
                re++;
            } else {
                EMIT(pc, RSplit);
            }
            EMIT(pc + 1, REL(pc, term));
            pc += 2;
            prog->len++;
            break;
        case '|':
            if (alt_label) {
                EMIT(alt_label, REL(alt_label, pc) + 1);
            }
            insert_code(code, start, 2, &pc);
            EMIT(pc++, Jmp);
            alt_label = pc++;
            EMIT(start, Split);
            EMIT(start + 1, REL(start, pc));
            prog->len += 2;
            break;
        case '^':
            EMIT(pc++, Bol);
            prog->len++;
            break;
        case '$':
            EMIT(pc++, Eol);
            prog->len++;
            break;
        }
    }

    if (alt_label) {
        EMIT(alt_label, REL(alt_label, pc) + 1);
    }
    prog->bytelen = pc;
    return re;
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

    _compilecode(re, prog);

    prog->insts[prog->bytelen++] = Save;
    prog->insts[prog->bytelen++] = 1;
    prog->len++;

    prog->insts[prog->bytelen++] = Match;
    prog->len++;

    return 0;
}

void
cleanmarks(ByteProg *prog)
{
       char *pc = prog->insts;
       char *end = pc + prog->bytelen;
       while (pc < end) {
               *pc &= 0x7f;
               switch (*pc) {
               case Jmp:
               case Split:
               case RSplit:
               case Save:
               case Char:
                       pc++;
               }
               pc++;
       }
}

#if 0
int main(int argc, char *argv[])
{
    int pc = 0;
    ByteProg *code = re1_5_compilecode(argv[1]);
    re1_5_dumpcode(code);
}
#endif
