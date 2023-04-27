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
final full stop. First line should fit within 72 characters. Examples
of good first line of commit messages:

    py/objstr: Add splitlines() method.
    py: Rename FOO to BAR.
    docs/machine: Fix typo in reset() description.
    ports: Switch to use lib/foo instead of duplicated code.

After the first line add an empty line and in the following lines describe
the change in a detail, if needed, with lines fitting within 75 characters
(with an exception for long items like URLs which cannot be broken). Any
change beyond 5 lines would likely require such detailed description.

To get good practical examples of good commits and their messages, browse
the `git log` of the project.

When committing you are encouraged to sign-off your commit by adding
"Signed-off-by" lines and similar, eg using "git commit -s".  If you don't
explicitly sign-off in this way then the commit message, which includes your
name and email address in the "Author" line, implies your sign-off.  In either
case, of explicit or implicit sign-off, you are certifying and signing off
against the following:

* That you wrote the change yourself, or took it from a project with
  a compatible license (in the latter case the commit message, and possibly
  source code should provide reference where the implementation was taken
  from and give credit to the original author, as required by the license).
* That you are allowed to release these changes to an open-source project
  (for example, changes done during paid work for a third party may require
  explicit approval from that third party).
* That you (or your employer) agree to release the changes under
  MicroPython's license, which is the MIT license. Note that you retain
  copyright for your changes (for smaller changes, the commit message
  conveys your copyright; if you make significant changes to a particular
  source module, you're welcome to add your name to the file header).
* Your contribution including commit message will be publicly and
  indefinitely available for anyone to access, including redistribution
  under the terms of the project's license.
* Your signature for all of the above, which is the "Signed-off-by" line
  or the "Author" line in the commit message, includes your full real name and
  a valid and active email address by which you can be contacted in the
  foreseeable future.

Code auto-formatting
====================

Both C and Python code are auto-formatted using the `tools/codeformat.py`
script.  This uses [uncrustify](https://github.com/uncrustify/uncrustify) to
format C code and [black](https://github.com/psf/black) to format Python code.
After making changes, and before committing, run this tool to reformat your
changes to the correct style.  Without arguments this tool will reformat all
source code (and may take some time to run).  Otherwise pass as arguments to
the tool the files that changed and it will only reformat those.

uncrustify
==========

Only [uncrustify](https://github.com/uncrustify/uncrustify) v0.71 or v0.72 can
be used for MicroPython. Different uncrustify versions produce slightly
different formatting, and the configuration file formats are often
incompatible. v0.73 or newer *will not work*.

Depending on your operating system version, it may be possible to install a pre-compiled
uncrustify version:

Ubuntu, Debian
--------------

Ubuntu versions 21.10 or 22.04LTS, and Debian versions bullseye or bookworm all
include v0.72 so can be installed directly:

```
$ apt install uncrustify
```

Arch Linux
----------

The current Arch uncrustify version is too new. There is an [old Arch package
for v0.72](https://archive.archlinux.org/packages/u/uncrustify/) that can be
installed from the Arch Linux archive ([more
information](https://wiki.archlinux.org/title/Downgrading_packages#Arch_Linux_Archive)). Use
the [IgnorePkg feature](https://wiki.archlinux.org/title/Pacman#Skip_package_from_being_upgraded)
to prevent it re-updating.

Brew
----

This command may work, please raise a new Issue if it doesn't:

```
curl -L https://github.com/Homebrew/homebrew-core/raw/2b07d8192623365078a8b855a164ebcdf81494a6/Formula/uncrustify.rb > uncrustify.rb && brew install uncrustify.rb && rm uncrustify.rb
```

Automatic Pre-Commit Hooks
==========================

To have code formatting and commit message conventions automatically checked,
a configuration file is provided for the [pre-commit](https://pre-commit.com/)
tool.

First install `pre-commit`, either from your system package manager or via
`pip`. When installing `pre-commit` via pip, it is recommended to use a
virtual environment. Other sources, such as Brew are also available, see
[the docs](https://pre-commit.com/index.html#install) for details.

```
$ apt install pre-commit       # Ubuntu, Debian
$ pacman -Sy python-precommit  # Arch Linux
$ brew install pre-commit      # Brew
$ pip install pre-commit       # PyPI
```

Next, install [uncrustify (see above)](#uncrustify). Other dependencies are managed by
pre-commit automatically, but uncrustify needs to be installed and available on
the PATH.

Then, inside the MicroPython repository, register the git hooks for pre-commit
by running:

```
$ pre-commit install --hook-type pre-commit --hook-type commit-msg
```

pre-commit will now automatically run during `git commit` for both code and
commit message formatting.

The same formatting checks will be run by CI for any Pull Request submitted to
MicroPython. Pre-commit allows you to see any failure more quickly, and in many
cases will automatically correct it in your local working copy.

To unregister `pre-commit` from your MicroPython repository, run:

```
$ pre-commit uninstall --hook-type pre-commit --hook-type commit-msg
```

Tips:

* To skip pre-commit checks on a single commit, use `git commit -n` (for
  `--no-verify`).
* To ignore the pre-commit message format check temporarily, start the commit
  message subject line with "WIP" (for "Work In Progress").

Python code conventions
=======================

Python code follows [PEP 8](https://legacy.python.org/dev/peps/pep-0008/) and
is auto-formatted using [black](https://github.com/psf/black) with a line-length
of 99 characters.

Naming conventions:
- Module names are short and all lowercase; eg pyb, stm.
- Class names are CamelCase, with abbreviations all uppercase; eg I2C, not
  I2c.
- Function and method names are all lowercase with words separated by
  a single underscore as necessary to improve readability; eg mem_read.
- Constants are all uppercase with words separated by a single underscore;
  eg GPIO_IDR.

C code conventions
==================

C code is auto-formatted using [uncrustify](https://github.com/uncrustify/uncrustify)
and the corresponding configuration file `tools/uncrustify.cfg`, with a few
minor fix-ups applied by `tools/codeformat.py`.  When writing new C code please
adhere to the existing style and use `tools/codeformat.py` to check any changes.
The main conventions, and things not enforceable via the auto-formatter, are
described below.

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

        for (int my_counter = 0; my_counter < x; ++my_counter) {
        }
    }

Type declarations:

    typedef struct _my_struct_t {
        int member;
        void *data;
    } my_struct_t;

Documentation conventions
=========================

MicroPython generally follows CPython in documentation process and
conventions. reStructuredText syntax is used for the documentation.

Specific conventions/suggestions:

* Use `*` markup to refer to arguments of a function, e.g.:

```
.. method:: poll.unregister(obj)

   Unregister *obj* from polling.
```

* Use following syntax for cross-references/cross-links:

```
:func:`foo` - function foo in current module
:func:`module1.foo` - function foo in module "module1"
    (similarly for other referent types)
:class:`Foo` - class Foo
:meth:`Class.method1` - method1 in Class
:meth:`~Class.method1` - method1 in Class, but rendered just as "method1()",
    not "Class.method1()"
:meth:`title <method1>` - reference method1, but render as "title" (use only
    if really needed)
:mod:`module1` - module module1

`symbol` - generic xref syntax which can replace any of the above in case
    the xref is unambiguous. If there's ambiguity, there will be a warning
    during docs generation, which need to be fixed using one of the syntaxes
    above
```

* Cross-referencing arbitrary locations
~~~
.. _xref_target:

Normal non-indented text.

This is :ref:`reference <xref_target>`.

(If xref target is followed by section title, can be just
:ref:`xref_target`).
~~~

* Linking to external URL:
```
`link text <http://foo.com/...>`_
```

* Referencing builtin singleton objects:
```
``None``, ``True``, ``False``
```

* Use following syntax to create common description for more than one element:
~~~
.. function:: foo(x)
              bar(y)

   Description common to foo() and bar().
~~~


More detailed guides and quickrefs:

* http://www.sphinx-doc.org/en/stable/rest.html
* http://www.sphinx-doc.org/en/stable/markup/inline.html
* http://docutils.sourceforge.net/docs/user/rst/quickref.html
