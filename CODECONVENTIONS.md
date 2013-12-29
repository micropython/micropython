Code conventions
================

When writing new code, please adhere to the following conventions.

White space:
- Expand tabs to 4 spaces.
- Don't leave trailing whitespace at the end of a line.
- For control blocks (if, for, while), put 1 space between the
  keyword and the opening parenthesis.
- Put 1 space after a comma, and 1 space around operators.

Braces: 
- Use braces for all blocks, even no-line and single-line pieces of
  code.
- Put opening braces on the end of the line it belongs to, not on
  a new line.
- For else-statements, put the else on the same line as the previous
  closing brace.

Include directives:
- Don't include within a header file.

Type names and declarations:
- When defining a type, put '_t' after it.

Examples
--------

Braces and spaces:

    int foo(int x, int y) {
        if (x < y) {
            foo(y, x);
        } else {
            foo(x + 1, y - 1);
        }

        for (int i = 0; i < x; i++) {
        }
    }

Type declarations:

    typedef struct _my_struct_t {
        int member;
        void *data;
    } my_struct_t;
