Python code conventions
=======================

Python code follows [PEP 8](http://legacy.python.org/dev/peps/pep-0008/).

Naming conventions:
- Module names are short and all lowercase; eg pyb, stm.
- Class names are CamelCase, with abreviations all uppercase; eg I2C, not
  I2c.
- Function and method names are all lowercase with words separated by
  a single underscore as necessary to improve readability; eg mem_read.
- Constants are all uppercase with words separated by a single underscore;
  eg GPIO_IDR.

C code conventions
==================

When writing new C code, please adhere to the following conventions.

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

Header files:
- Try to stick to the Plan 9 header style, where header files do not
  include other header files.
- Don't protect a header file from multiple inclusion with #if directives.

Type names and declarations:
- When defining a type, put '_t' after it.

Integer types: Micro Python runs on 32 and 64 bit machines (and one day
maybe 16 bit), so it's important to use the correctly-sized (and signed)
integer types.  The general guidelines are:
- For most cases use mp_int_t for signed and mp_uint_t for unsigned
  integer values.  These are guaranteed to be machine-word sized and
  therefore big enough to hold the value from a Micro Python small-int
  object.
- Use size_t for things that count bytes / sizes of objects.
- You can use int/uint, but remember that they may be 16-bits wide.
- If in doubt, use mp_int_t/mp_uint_t.

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
