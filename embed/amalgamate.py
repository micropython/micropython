import sys

grammar = None
re15 = None
log = False
build_dir = 'build'

verbatim_files = {
    '#include "genhdr/qstrdefs.generated.h"\n': ['genhdr/qstrdefs.generated.h', None],
    '#include "genhdr/mpversion.h"\n': ['genhdr/mpversion.h', None],
    '#include "py/grammar.h"\n': ['py/grammar.h', None],
    '    #include "py/vmentrytable.h"\n': ['py/vmentrytable.h', None],
    '#include "re1.5/re1.5.h"\n': ['../extmod/re1.5/re1.5.h', None],
    '#include "re1.5/compilecode.c"\n': ['../extmod/re1.5/compilecode.c', None],
    '#include "re1.5/dumpcode.c"\n': ['../extmod/re1.5/dumpcode.c', None],
    '#include "re1.5/recursiveloop.c"\n': ['../extmod/re1.5/recursiveloop.c', None],
    '#include "re1.5/charclass.c"\n': ['../extmod/re1.5/charclass.c', None],
    '#include "crypto-algorithms/sha256.h"\n': ['../extmod/crypto-algorithms/sha256.h', None],
    '#include "crypto-algorithms/sha256.c"\n': ['../extmod/crypto-algorithms/sha256.c', None],
}

for line in sys.stdin:
    if line in verbatim_files:
        vf = verbatim_files[line]
        if vf[1] is None:
            vf_filename = vf[0]
            if not vf_filename.startswith('..'):
                vf_filename = build_dir + '/' + vf_filename
            with open(vf_filename) as f:
                ls = []
                for l in f:
                    if l.startswith('#define EMIT('):
                        ls.append('#undef EMIT\n')
                        ls.append(l)
                    elif not l.startswith('#include'):
                        ls.append(l)
                vf[1] = ''.join(ls)
        sys.stdout.write(vf[1])
    elif line.startswith('#include "py/'):
        sys.stdout.write('//' + line)
    elif line.startswith('#include "extmod/'):
        sys.stdout.write('//' + line)
    elif line == '#include <mphalport.h>\n':
        sys.stdout.write('//' + line)
    elif line == '#include <mpconfigport.h>\n':
        sys.stdout.write('#include "mpconfigport.h"\n')
    elif line.startswith('#define DIG_MASK '):
        sys.stdout.write('#undef DIG_MASK\n')
        sys.stdout.write(line)
    elif line == 'STATIC const uint8_t log_base2_floor[] = {\n':
        if log:
            for l in sys.stdin:
                if l == '};\n':
                    break
        else:
            sys.stdout.write(line)
            log = True
    else:
        sys.stdout.write(line)
