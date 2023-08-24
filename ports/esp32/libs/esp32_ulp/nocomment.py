def remove_comments(s):
    """
    Remove comments of these styles:

        CHASH:       # comment python style, up to: EOL
        CSLASHSLASH: // comment C style, up to: EOL
        CSLASHSTAR:  /* comment C style (single/multi line), up to: */

    Strings can be like 'strings' or "strings".
    Any comment-starting chars within strings are not considered.
    Escaping of (string-end) chars via backslash in strings is considered.

    Also, leading and trailing whitespace is removed (after comment removal).
    Indented lines are re-indented afterwards with a single tab char.

    Line numbers stay as in input file because empty lines are kept.

    s: string with comments (can include newlines)
    returns: list of text lines
    """
    # note: micropython's ure module was not capable enough to process this:
    # missing methods, re modes, recursion limit exceeded, ...
    # simpler hacks also didn't seem powerful enough to address all the
    # corner cases of CSLASHSTAR vs. *STR, so this state machine came to life:
    SRC, CHASH, CSLASHSLASH, CSLASHSTAR, DSTR, SSTR = range(6)  # states

    line = []  # collect chars of one line
    lines = []  # collect result lines

    def finish_line():
        # assemble a line from characters, try to get rid of trailing and
        # most of leading whitespace (keep/put one tab for indented lines).
        nonlocal line
        line = ''.join(line)
        is_indented = line.startswith(' ') or line.startswith('\t')
        line = line.strip()
        if line and is_indented:
            line = '\t' + line
        lines.append(line)
        line = []

    state = SRC
    i = 0
    length = len(s)
    while i < length:
        c = s[i]
        cn = s[i + 1] if i + 1 < length else '\0'
        if state == SRC:
            if c == '#':  # starting to-EOL comment
                state = CHASH
                i += 1
            elif c == '/':
                if cn == '/':  # starting to-EOL comment
                    state = CSLASHSLASH
                    i += 2
                elif cn == '*':  # starting a /* comment
                    state = CSLASHSTAR
                    i += 2
                else:
                    i += 1
                    line.append(c)
            elif c == '"':
                state = DSTR
                i += 1
                line.append(c)
            elif c == "'":
                state = SSTR
                i += 1
                line.append(c)
            elif c == '\n':
                i += 1
                finish_line()
            else:
                i += 1
                line.append(c)
        elif state == CHASH or state == CSLASHSLASH:
            if c != '\n':  # comment runs until EOL
                i += 1
            else:
                state = SRC
                i += 1
                finish_line()
        elif state == CSLASHSTAR:
            if c == '*' and cn == '/':  # ending a comment */
                state = SRC
                i += 2
            elif c == '\n':
                i += 1
                finish_line()
            else:
                i += 1
        elif state == DSTR and c == '"' or state == SSTR and c == "'":  # string end
            state = SRC
            i += 1
            line.append(c)
        elif state == DSTR or state == SSTR:
            i += 1
            line.append(c)
            if c == '\\':  # escaping backslash
                i += 1  # do not look at char after the backslash
                line.append(cn)
        else:
            raise Exception("state: %d c: %s cn: %s" % (state, c, cn))
    if line:
        # no final \n triggered processing these chars yet, do it now
        finish_line()
    return lines


if __name__ == '__main__':
    import sys
    filename = sys.argv[1]
    with open(filename, "r") as f:
        text = f.read()
    lines = remove_comments(text)
    with open(filename + ".nocomments", "w") as f:
        for line in lines:
            f.write(line + '\n')

