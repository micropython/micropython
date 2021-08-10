.. _manifest_file:

MicroPython manifest file
==========================

Syntax :  include(manifest, **kwargs)
  
The manifest argument can be a string (filename) or an iterable of strings.
Relative paths are resolved with respect to the current manifest file.
Optional kwargs can be provided which will be available to the included script
via the `options` variable. e.g. include("path.py", extra_features=True)

in path.py:
        options.defaults(standard_features=True)
        # freeze minimal modules.
        if options.standard_features:
            # freeze standard modules.
        if options.extra_features:
            # freeze extra modules.
         
The MicroPython manifest files supports variousmethods which are:


* freeze_as_str(path) : Freeze the given `path` and all .py scripts within it
                        as a string, which will be compiled upon import.

* freeze_as_mpy(path, script=None, opt=0) : Freeze the input (see above) by
                                            first compiling the .py scripts 
                                            to .mpy files, then freezing the 
                                            resulting.mpy files.

def freeze_mpy(path, script=None, opt=0): Freeze the input (see above), which  
                                          must be .mpy files that are frozen 
                                          directly.
