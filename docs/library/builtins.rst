:mod:`builtins` -- builtin functions and exceptions
===================================================

All builtin functions and exceptions are described here. They are also
available via ``builtins`` module.

Functions and types
-------------------

.. function:: abs(x, /)

    Return the absolute value of a number. |see_cpython| `python:abs`.

.. function:: all(iterable, /)

    Return ``True`` if all elements of the iterable are true. |see_cpython| `python:all`.

.. function:: any(iterable, /)

    Return ``True`` if any element of the iterable is true. |see_cpython| `python:any`.

.. function:: bin(number, /)

    Convert an integer to a binary string prefixed with ``0b``. |see_cpython| `python:bin`.

.. class:: bool(x=False, /)

    |see_cpython| `python:bool`.

.. class:: bytearray(source=b'', encoding=None, errors=None, /)

    |see_cpython| `python:bytearray`.

    Slice deletion or assignment with a step other than 1 is not implemented,
    and slice-assignment only accepts a value supporting the same operations
    as a list (not an arbitrary iterable)
    (:ref:`Details <cpydiff_types_bytearray_sliceassign>`).

.. class:: bytes(source=b'', encoding=None, errors=None, /)

    |see_cpython| `python:bytes`.

    ``bytes(string, encoding)`` does not accept ``encoding`` as a keyword
    argument -- pass it positionally. ``bytes`` objects also support
    ``.format()`` (in addition to ``%``-formatting), and subscripting with a
    step other than 1 is not implemented
    (:ref:`Details <cpydiff_types_bytes_keywords>`,
    :ref:`Details <cpydiff_types_bytes_format>`,
    :ref:`Details <cpydiff_types_bytes_subscrstep>`).

.. function:: callable(object, /)

    Return whether the object appears to be callable. |see_cpython| `python:callable`.

.. function:: chr(i, /)

    Return the string of one character whose code point is the given integer.
    On builds without full Unicode support, only code points 0-255 are
    accepted. |see_cpython| `python:chr`.

.. function:: classmethod(function, /)

    Transform a method into a class method. |see_cpython| `python:classmethod`.

.. function:: compile(source, filename, mode, flags=0, dont_inherit=False, optimize=-1, /)

    Compile source into a code object that can later be executed by `exec` or
    `eval`. The ``flags``, ``dont_inherit`` and ``optimize`` arguments are
    accepted for CPython compatibility but are ignored. Only available on
    builds that enable this optional feature. |see_cpython| `python:compile`.

.. class:: complex(real=0, imag=0, /)

    |see_cpython| `python:complex`.

    Unlike CPython, the string parser also accepts a space (instead of a
    ``+``) between the real and imaginary parts, e.g. ``complex("1 1j")``
    (:ref:`Details <cpydiff_types_complex_parser>`).

.. function:: delattr(obj, name, /)

   The argument *name* should be a string, and this function deletes the named
   attribute from the object given by *obj*.

   Only available on builds with the CPython-compatibility extras enabled.

.. class:: dict(mapping_or_iterable=(), /, **kwarg)

    |see_cpython| `python:dict`.

    The object returned by ``dict.keys()`` does not behave as a set view --
    set operations such as ``&`` or ``|`` against it are not supported
    (:ref:`Details <cpydiff_types_dict_keys_set>`).

.. function:: dir(object=None, /)

    Without an argument, return the names in the current local scope; with an
    argument, return the names available on that object. Unlike CPython, the
    returned list is not sorted. |see_cpython| `python:dir`.

.. function:: divmod(x, y, /)

    Return the pair ``(a // b, a % b)``. |see_cpython| `python:divmod`.

.. function:: enumerate(iterable, start=0, /)

    Return an iterator yielding pairs of index and value. |see_cpython| `python:enumerate`.

.. function:: eval(expression, globals=None, locals=None, /)

    Evaluate the given expression and return its value. At module scope this
    behaves as in CPython, but because MicroPython does not keep a symbolic
    namespace for a function's local variables, code run by ``eval()`` inside
    a function cannot see that function's locals: it effectively only has
    access to the module's globals
    (:ref:`Details <cpydiff_core_locals_eval>`). |see_cpython| `python:eval`.

.. function:: exec(source, globals=None, locals=None, /)

    Execute the given source. |see_cpython| `python:exec`. Subject to the
    same local-scope limitation as :func:`eval`.

.. function:: filter(function, iterable, /)

    Construct an iterator of the elements for which a function returns true.
    |see_cpython| `python:filter`.

.. class:: float(x=0.0, /)

    |see_cpython| `python:float`.

    Unlike CPython, MicroPython implicitly converts an object that implements
    ``__float__()`` when it appears alongside a ``float`` in a math operation,
    instead of requiring an explicit ``float(obj)`` conversion
    (:ref:`Details <cpydiff_types_float_implicit_conversion>`).

.. class:: frozenset(iterable=(), /)

    |see_cpython| `python:frozenset`. Only available on builds that enable
    this optional feature.

.. function:: getattr(object, name, default=None, /)

    Return the value of a named attribute of an object, with an optional
    default. |see_cpython| `python:getattr`.

.. function:: globals()

    Return the dictionary representing the current module's global namespace.
    |see_cpython| `python:globals`.

.. function:: hasattr(object, name, /)

    Return whether the object has the given attribute. |see_cpython| `python:hasattr`.

.. function:: hash(object, /)

    Return the hash value of an object. Unlike CPython, the result is always
    truncated to a small, machine-word-sized integer, so hash values will
    generally not match CPython's for the same object.
    |see_cpython| `python:hash`.

.. function:: hex(number, /)

    Convert an integer to a hexadecimal string prefixed with ``0x``.
    |see_cpython| `python:hex`.

.. function:: id(object, /)

    Return an integer that is guaranteed to be constant and unique for the
    object during its lifetime. |see_cpython| `python:id`.

.. function:: input(prompt='', /)

    Read a line from input, optionally after printing a prompt.
    |see_cpython| `python:input`.

.. class:: int(x=0, base=10, /)

   .. classmethod:: from_bytes(bytes, byteorder, /)

      In MicroPython, `byteorder` parameter must be positional (this is
      compatible with CPython).

   .. method:: to_bytes(size, byteorder, /)

      In MicroPython, `byteorder` parameter must be positional (this is
      compatible with CPython).

      .. note:: The optional ``signed`` kwarg from CPython is not supported.
                MicroPython currently converts negative integers as signed,
                and positive as unsigned. (:ref:`Details <cpydiff_types_int_to_bytes>`.)

   Unlike CPython, ``int`` does not implement the ``bit_length()`` method
   (:ref:`Details <cpydiff_types_int_bit_length>`), and arithmetic on ``int``
   subclasses does not convert the result back to that subclass
   (:ref:`Details <cpydiff_types_int_subclassconv>`).

.. function:: isinstance(object, classinfo, /)

    Return whether an object is an instance of a class (or one of a tuple of
    classes). |see_cpython| `python:isinstance`.

.. function:: issubclass(class_, classinfo, /)

    Return whether a class is a subclass of another class (or one of a tuple
    of classes). |see_cpython| `python:issubclass`.

.. function:: iter(object, /)

    Return an iterator for an object. Unlike CPython, the two-argument form
    ``iter(callable, sentinel)`` is not implemented. |see_cpython| `python:iter`.

.. function:: len(s, /)

    Return the length of a container. |see_cpython| `python:len`.

.. class:: list(iterable=(), /)

    |see_cpython| `python:list`.

    Loading, storing or deleting a slice with a step other than 1 is not
    implemented, and slice-assignment only accepts a ``tuple`` or ``list`` on
    the right-hand side rather than an arbitrary iterable
    (:ref:`Details <cpydiff_types_list_store_subscrstep>`,
    :ref:`Details <cpydiff_types_list_delete_subscrstep>`,
    :ref:`Details <cpydiff_types_list_store_noniter>`).

.. function:: locals()

    At module scope, behaves as in CPython. Inside a function, MicroPython
    compiles local variables to array slots rather than a symbolic namespace,
    so the dictionary returned by ``locals()`` does not include that
    function's local variables (:ref:`Details <cpydiff_core_locals>`).
    |see_cpython| `python:locals`.

.. function:: map(function, *iterables)

    Apply a function to every item of an iterable, returning an iterator of
    the results. |see_cpython| `python:map`.

.. function:: max(*args, key=None, default=None)

    Return the largest item in an iterable, or the largest of two or more
    arguments. |see_cpython| `python:max`.

.. class:: memoryview(obj, /)

    |see_cpython| `python:memoryview`.

    Unlike CPython, MicroPython does not prevent a ``bytearray`` or
    ``io.BytesIO`` object from being resized while a ``memoryview`` still
    references it; doing so can leave the ``memoryview`` referencing invalid,
    freed memory (:ref:`Details <cpydiff_types_memoryview_invalid>`).

.. function:: min(*args, key=None, default=None)

    Return the smallest item in an iterable, or the smallest of two or more
    arguments. |see_cpython| `python:min`.

.. function:: next(iterator, default=None, /)

    Retrieve the next item from an iterator. The two-argument form
    ``next(iterator, default)`` is only available on builds that enable this
    optional feature. |see_cpython| `python:next`.

.. class:: object()

    |see_cpython| `python:object`.

    User-defined classes do not support the ``__del__`` special method
    (:ref:`Details <cpydiff_core_class_delnotimpl>`). When subclassing a
    native type, methods inherited from that native base should not be used
    before ``super().__init__()`` has been called
    (:ref:`Details <cpydiff_core_class_super_init>`).

.. function:: oct(number, /)

    Convert an integer to an octal string prefixed with ``0o``.
    |see_cpython| `python:oct`.

.. function:: open(name, mode='r', /)

    |see_cpython| `python:open`. See also :mod:`io`.

.. function:: ord(c, /)

    Return the code point of a one-character string. |see_cpython| `python:ord`.

.. function:: pow(base, exp, mod=None, /)

    Return ``base`` raised to the power ``exp``, optionally modulo ``mod``.
    The three-argument form is only available on builds that enable this
    optional feature; where it is not available it raises
    `NotImplementedError`. |see_cpython| `python:pow`.

.. function:: print(*objects, sep=' ', end='\n', file=sys.stdout)

    Print objects to a text stream, separated by ``sep`` and followed by
    ``end``. Unlike CPython, the ``flush`` keyword argument is not supported.
    |see_cpython| `python:print`.

.. function:: property(fget=None, fset=None, fdel=None, doc=None, /)

    |see_cpython| `python:property`.

    In a subclass that overrides a property, accessing the property through
    ``super()`` returns the ``property`` object itself rather than invoking
    the getter and returning its value
    (:ref:`Details <cpydiff_core_class_superproperty>`).

.. function:: range(stop, /)
              range(start, stop, step=1, /)

    |see_cpython| `python:range`.

    ``start``/``stop``/``step`` values large enough to overflow the
    interpreter's native integer type may raise `OverflowError`, or produce an
    incorrect ``len()`` (:ref:`Details <cpydiff_types_range_limits>`).

.. function:: repr(object, /)

    Return a printable representation of an object. |see_cpython| `python:repr`.

.. function:: reversed(sequence, /)

    Return a reverse iterator. |see_cpython| `python:reversed`.

.. function:: round(number, ndigits=None, /)

    |see_cpython| `python:round`. Ties are rounded to the nearest even
    number, as in CPython. Rounding a ``float`` to a given number of digits,
    and rounding an ``int`` (the two-argument forms), are only available on
    builds that enable the relevant optional features.

.. class:: set(iterable=(), /)

    |see_cpython| `python:set`.

.. function:: setattr(object, name, value, /)

    Set a named attribute on an object to a given value. |see_cpython| `python:setattr`.

.. class:: slice(stop, /)
           slice(start, stop, step=None, /)

   The *slice* builtin is the type that slice objects have.

.. function:: sorted(iterable, /, *, key=None)

    Return a new sorted list from the items in an iterable. |see_cpython| `python:sorted`.

.. function:: staticmethod(function, /)

    Transform a method into a static method. |see_cpython| `python:staticmethod`.

.. class:: str(object='', encoding=None, errors=None, /)

    |see_cpython| `python:str`.

    A number of `str` methods and behaviours available in CPython are not
    implemented: ``str(bytes, encoding)`` does not accept ``encoding`` as a
    keyword argument, ``ljust()``/``rjust()`` do not exist, ``rsplit(None,
    n)`` is not supported, and subscripting with a step other than 1 is not
    implemented. ``str.format()`` does not support attribute/index access in
    replacement fields (e.g. ``"{a[0]}"``), and does not reject the ``,``
    grouping option for non-decimal presentation types like CPython does
    (:ref:`Details <cpydiff_types_str_keywords>`,
    :ref:`Details <cpydiff_types_str_ljust_rjust>`,
    :ref:`Details <cpydiff_types_str_rsplitnone>`,
    :ref:`Details <cpydiff_types_str_subscrstep>`,
    :ref:`Details <cpydiff_types_str_formatsubscr>`,
    :ref:`Details <cpydiff_types_str_formatsep>`).

.. function:: sum(iterable, start=0, /)

    Sum the items of an iterable, with an optional start value.
    |see_cpython| `python:sum`.

.. function:: super()

    |see_cpython| `python:super`.

    Only the zero-argument form (used inside a method body) is implemented --
    the single-argument "unbound" form ``super(type)`` is not supported. The
    method resolution order used to look up attributes through ``super()`` is
    a simplified, depth-first search rather than CPython's C3 linearization,
    so results can differ in non-trivial multiple-inheritance hierarchies, and
    calling ``super()`` from a class with multiple base classes only reaches
    one of them rather than cooperatively chaining through all of them
    (:ref:`Details <cpydiff_core_class_mro>`,
    :ref:`Details <cpydiff_core_class_supermultiple>`).

.. class:: tuple(iterable=(), /)

    |see_cpython| `python:tuple`.

    Subscripting with a step other than 1 is not implemented
    (:ref:`Details <cpydiff_types_tuple_subscrstep>`).

.. function:: type(object, /)
              type(name, bases, dict, /)

    |see_cpython| `python:type`. Class creation follows the same simplified
    method resolution order described under :func:`super`.

.. function:: zip(*iterables)

    Iterate over several iterables in parallel. |see_cpython| `python:zip`.


Exceptions
----------

.. exception:: AssertionError

    |see_cpython| `python:AssertionError`.

.. exception:: AttributeError

    |see_cpython| `python:AttributeError`.

.. exception:: Exception

    |see_cpython| `python:Exception`.

    All exceptions expose readable ``value`` and ``errno`` attributes, not
    just ``StopIteration.value`` and ``OSError.errno`` as in CPython -- only
    rely on those two documented attributes for CPython compatibility
    (:ref:`Details <cpydiff_types_exception_attrs>`). Setting a new,
    user-defined attribute on a builtin exception *instance* is not supported;
    use an exception subclass instead
    (:ref:`Details <cpydiff_types_exception_instancevar>`). Exception chaining
    (``__context__``/``__cause__``) is not implemented
    (:ref:`Details <cpydiff_types_exception_chaining>`). There is no unbound
    ``Exception.__init__`` method to call directly on the class; use
    ``super().__init__()`` instead
    (:ref:`Details <cpydiff_types_exception_subclassinit>`). A subclass must
    also finish calling that ``__init__`` before doing anything with ``self``,
    such as raising it
    (:ref:`Details <cpydiff_core_exception_construction>`).

.. exception:: ImportError

    |see_cpython| `python:ImportError`.

.. exception:: IndexError

    |see_cpython| `python:IndexError`.

.. exception:: KeyboardInterrupt

   |see_cpython| `python:KeyboardInterrupt`.

   See also in the context of :ref:`soft_bricking`.

.. exception:: KeyError

    |see_cpython| `python:KeyError`.

.. exception:: MemoryError

    |see_cpython| `python:MemoryError`.

.. exception:: NameError

    |see_cpython| `python:NameError`.

.. exception:: NotImplementedError

    |see_cpython| `python:NotImplementedError`.

.. exception:: OSError

    |see_cpython| `python:OSError`.

    MicroPython does not implement the CPython-standard ``OSError``
    subclasses (such as ``FileNotFoundError`` or ``PermissionError``): every
    OS-level error raises a plain ``OSError``, and callers should inspect its
    ``errno`` attribute rather than the exception type
    (:ref:`Details <cpydiff_types_oserror_errnomap>`).

.. exception:: RuntimeError

    |see_cpython| `python:RuntimeError`.

.. exception:: StopIteration

    |see_cpython| `python:StopIteration`.

.. exception:: SyntaxError

    |see_cpython| `python:SyntaxError`.

.. exception:: SystemExit

    |see_cpython| `python:SystemExit`.

    On non-embedded ports (i.e. Windows and Unix), an unhandled ``SystemExit``
    exits the MicroPython process in a similar way to CPython.

    On embedded ports, an unhandled ``SystemExit`` currently causes a
    :ref:`soft_reset` of MicroPython.

.. exception:: TypeError

    |see_cpython| `python:TypeError`.

.. exception:: ValueError

    |see_cpython| `python:ValueError`.

.. exception:: ZeroDivisionError

    |see_cpython| `python:ZeroDivisionError`.
