import sys
name = sys.argv[1].split('/')[-1].split('.')[0]
with open(sys.argv[1]) as f:
    lines_correct = [l.strip('\n') for l in f.readlines()]
lines_me = [l.strip('\n') for l in sys.stdin.readlines()]
if len(lines_me) != len(lines_correct):
    if len(lines_me) == 0:
        print('{:<20}: no output'.format(name))
    elif lines_me[0].find('syntax error') >= 0:
        print('{:<20}: syntax error'.format(name))
    elif lines_me[0].find(' cannot be compiled') >= 0:
        print('{:<20}: compile error: {}'.format(name, lines_me[0]))
    else:
        print('{:<20}: mismatch in number of lines'.format(name))
else:
    total = len(lines_me)
    same = 0
    bad_num_fields = 0
    bad_2 = 0
    bad_3 = 0
    jump_op = ['JUMP_FORWARD', 'JUMP_ABSOLUTE', 'POP_JUMP_IF_FALSE', 'POP_JUMP_IF_TRUE', 'SETUP_LOOP']
    jump_abs_op = ['JUMP_FORWARD', 'JUMP_ABSOLUTE']
    for i in range(total):
        if lines_me[i] == lines_correct[i]:
            same += 1
        else:
            # line is different
            line_me = lines_me[i].strip().split(' ', 2)
            line_correct = lines_correct[i].strip().split(' ', 2)
            allow = False
            if len(line_me) != len(line_correct):
                bad_num_fields += 1
            elif len(line_me) == 2:
                if line_me[0] == line_correct[0] == 'stacksize':
                    allow = True
                else:
                    bad_2 += 1
            else:
                assert(len(line_me) == 3)
                if line_me[0] == line_correct[0] and line_me[1] in jump_abs_op and line_correct[1] in jump_abs_op:
                    allow = True
                elif line_me[0] == line_correct[0] and line_me[1] == line_correct[1] in jump_op:
                    allow = True
                else:
                    bad_3 += 1
            #if not allow:
            #    print(line_me, 'vs', line_correct)

    bad_str = ''
    if bad_num_fields > 0:
        bad_str += ', {} bad num fields'.format(bad_num_fields)
    if bad_2 > 0:
        bad_str += ', {} bad 2-field'.format(bad_2)
    if bad_3 > 0:
        bad_str += ', {} bad 3-field'.format(bad_3)
    print('{:<20}: {:>6} lines, {:>5.1f}% correct{}'.format(name, total, 100 * same / total, bad_str))
