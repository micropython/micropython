.. _manifest_file:

When building a sorce distribution for a package, we are only allowed to include 
some files initially. To include any extra file in the source distribution, we 
use a :manifest.py: file to add and remove files from source distribution.


MicroPython manifest file
==========================

..code-block:: python3

  include(manifest, **kwargs)
  
The manifest argument can be a string (filename) or an iterable of strings.
Relative paths are resolved with respect to the current manifest file.
Optional kwargs can be provided which will be available to the included script
via the `options` variable. e.g. include("path.py", extra_features=True).

In path.py:

..code-block:: python3

        options.defaults(standard_features=True)
        # freeze minimal modules.
        if options.standard_features:
            # freeze standard modules.
        if options.extra_features:
            # freeze extra modules.
            

Freeze the input, automatically determining its type.  A .py script
will be compiled to a .mpy first then frozen, and a .mpy file will be
frozen directly.
`path` must be a directory, which is the base directory to search for
files from.  When importing the resulting frozen modules, the name of
the module will start after `path`, ie `path` is excluded from the
module name.
If `path` is relative, it is resolved to the current manifest.py.
Use $(MPY_DIR), $(MPY_LIB_DIR), $(PORT_DIR), $(BOARD_DIR) if you need
to access specific paths.
If `script` is None all files in `path` will be frozen.
If `script` is an iterable then freeze() is called on all items of the
iterable (with the same `path` and `opt` passed through).
If `script` is a string then it specifies the file or directory to
freeze, and can include extra directories before the file or last
directory.  The file or directory will be searched for in `path`.  If
`script` is a directory then all files in that directory will be frozen.
`opt` is the optimisation level to pass to mpy-cross when compiling .py
 to .mpy.  
  
         
The MicroPython manifest files supports various methods which are::


* Freeze the given `path` and all .py scripts within it as a string, 
  which will be compiled upon import.
  
  ..code-block:: python3
  
    freeze_as_str(path) 

* Freeze the input (see below) by first compiling the .py scripts 
  to .mpy files, then freezing the resulting .mpy files.

  ..code-block:: python3
  
    freeze_as_mpy(path, script=None, opt=0)  
    
* Freeze the input (see below), which must be .mpy files that are 
frozen directly.

  ..code-block:: python3
  
    freeze_mpy(path, script=None, opt=0)
