Git commit conventions
======================

Each commit message should start with a directory or full file path
prefix, so it was clear which part of codebase a commit affects. If
a change affects one file, it's better to use path to a file. If it
affects few files in a subdirectory, using subdirectory as a prefix
is ok. For longish paths, it's acceptable to drop intermediate
components, which still should provide good context of a change.
It's also ok to drop file extensions.

Besides prefix, first line of a commit message should describe a
change clearly and to the point, and be a grammatical sentence with
final full stop. First line should fit within 78 characters. Examples
of good first line of commit messages:

    py/objstr: Add splitlines() method.
    py: Rename FOO to BAR.
    docs/machine: Fix typo in reset() description.
    ports: Switch to use lib/foo instead of duplicated code.

After the first line, add an empty line and in following lines describe
a change in a detail, if needed. Any change beyond 5 lines would likely
require such detailed description.

To get good practical examples of good commits and their messages, browse
thry the `git log` of the project.

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
- Header files should be protected from multiple inclusion with #if
  directives. See an existing header for naming convention.

Names:
- Use underscore_case, not camelCase for all names.
- Use CAPS_WITH_UNDERSCORE for enums and macros.
- When defining a type use underscore_case and put '_t' after it.

Integer types: MicroPython runs on 16, 32, and 64 bit machines, so it's
important to use the correctly-sized (and signed) integer types.  The
general guidelines are:
- For most cases use mp_int_t for signed and mp_uint_t for unsigned
  integer values.  These are guaranteed to be machine-word sized and
  therefore big enough to hold the value from a MicroPython small-int
  object.
- Use size_t for things that count bytes / sizes of objects.
- You can use int/uint, but remember that they may be 16-bits wide.
- If in doubt, use mp_int_t/mp_uint_t.

Comments:
- Be concise and only write comments for things that are not obvious.
- Use `// ` prefix, NOT `/* ... */`. No extra fluff.

Memory allocation:
- Use m_new, m_renew, m_del (and friends) to allocate and free heap memory.
  These macros are defined in py/misc.h.

Examples
--------

Braces, spaces, names and comments:

    #define TO_ADD (123)

    // This function will always recurse indefinitely and is only used to show
    // coding style
    int foo_function(int x, int some_value) {
        if (x < some_value) {
            foo(some_value, x);
        } else {
            foo(x + TO_ADD, some_value - 1);
        }

        for (int my_counter = 0; my_counter < x; my_counter++) {
        }
    }

Type declarations:

    typedef struct _my_struct_t {
        int member;
        void *data;
    } my_struct_t;
