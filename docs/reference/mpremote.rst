.. _mpremote_files:

MicroPython .mpremote files
===========================

This CLI tool provides an integrated set of utilities to remotely interact with and automate a MicroPython device over a serial connection.

The simplest way to use this tool is::

mpremote

This will automatically connect to the device and provide an interactive REPL.

The full list of supported commands are:

*  connect to the given device

  ..code-block:: bash
  
    $ mpremote <device-shortcut>       

* connect to given device (device may be: list, auto, id:x, port:x
  or any valid device name/path)
  
  ..code-block:: pyhton3
     
     $ mpremote connect <device> 

* disconnect current device

  ..code-block:: python3
  
    $ mpremote disconnect

* mount local directory on device

  ..code-block:: python3
  
    $ mpremote mount <local-dir> 

* evaluate and print the string

  ..code-block:: python3
  
    $ mpremote eval <string> 

* execute the string

  ..code-block:: python3

    $ mpremote exec <string> 

* run the given local script

  ..code-block:: python3
  
    $ mpremote run <file>
    
* execute filesystem commands on the device command may be: 
  cat, ls, cp, rm, mkdir, rmdir; use ":" as a prefix to 
  specify a file on the device

  ..code-block:: python3
  
    mpremote fs <command> <args...>  

* enter REPL
  
  options:
      --capture <file>
      --inject-code <string>
      --inject-file <file>
      
   .. code-block:: pyhthon3
   
     $ mpremote repl 
   
   
                                  
                                    
Multiple commands can be specified and they will be run sequentially.
Connection and disconnection will be done automatically at the start and end
of the execution of the tool, if such commands are not explicitly given.
Automatic connection will search for the first available serial device. If no
action is specified then the REPL will be entered.

Shortcuts can be defined using the macro system. Built-in shortcuts are::

* a0, a1, a2, a3 : connect to /dev/ttyACM?

* u0, u1, u2, u3 : connect to /dev/ttyUSB?

* c0, c1, c2, c3 : connect to COM?

* cat, ls, cp, rm, mkdir, rmdir, df : filesystem commands

* reset : reset the device

* bootloader : make the device enter its bootloader

* setrtc : sets the date and time of the module

Any user configuration, including user-defined shortcuts, can be placed in
.config/mpremote/config.py. For example:

# Custom macro commands

..code-block:: python3
  commands = {
      "c33": "connect id:334D335C3138",
      "bl": "bootloader",
      "double x=4": "eval x*2",
  }

Examples::

..code-block:: python3

  mpremote
  mpremote a1
  mpremote connect /dev/ttyUSB0 repl
  mpremote ls
  mpremote a1 ls
  mpremote exec "import micropython; micropython.mem_info()"
  mpremote eval 1/2 eval 3/4
  mpremote mount .
  mpremote mount . exec "import local_script"
  mpremote ls
  mpremote cat boot.py
  mpremote cp :main.py .
  mpremote cp main.py :
  mpremote cp -r dir/ :
