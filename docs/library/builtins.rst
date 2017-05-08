Builtin Functions
=================

All builtin functions are described here. They are also available via
``builtins`` module.

.. function:: abs()

.. function:: all()

.. function:: any()

.. function:: bin()

.. class:: bool()

.. class:: bytearray()

.. class:: bytes()

.. function:: callable()

.. function:: chr()

.. function:: classmethod()

.. function:: compile(source, filename, mode[, flags[, dont_inherit]])

   Enable it in ```mpconfigport.h``` by adding the following. This will built the compile-function into the micropython interpreter:
   
   .. code-block:: c
      #define MICROPY_PY_BUILTINS_COMPILE (1)
      
   ```source``` is the string of python-code you want to compile, ```filename``` a string of an arbitrary filename (to be used for error-output), ```mode``` can be "eval", "exec" or "single" and determines the compilation-mode (used this way in CPython, please confirm for micropython).

.. class:: complex()

.. function:: delattr(obj, name)

   The argument *name* should be a string, and this function deletes the named
   attribute from the object given by *obj*.

.. class:: dict()

.. function:: dir()

.. function:: divmod()

.. function:: enumerate()

.. function:: eval()

.. function:: exec()

.. function:: filter()

.. class:: float()

.. class:: frozenset()

.. function:: getattr()

.. function:: globals()

.. function:: hasattr()

.. function:: hash()

.. function:: hex()

.. function:: id()

.. function:: input()

.. class:: int()

.. function:: isinstance()

.. function:: issubclass()

.. function:: iter()

.. function:: len()

.. class:: list()

.. function:: locals()

.. function:: map()

.. function:: max()

.. class:: memoryview()

.. function:: min()

.. function:: next()

.. class:: object()

.. function:: oct()

.. function:: open()

.. function:: ord()

.. function:: pow()

.. function:: print()

.. function:: property()

.. function:: range()

.. function:: repr()

.. function:: reversed()

.. function:: round()

.. class:: set()

.. function:: setattr()

.. class:: slice()

   The *slice* builtin is the type that slice objects have.

.. function:: sorted()

.. function:: staticmethod()

.. class:: str()

.. function:: sum()

.. function:: super()

.. class:: tuple()

.. function:: type()

.. function:: zip()
