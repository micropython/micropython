.. _psoc6_mpy_usage:

Working with MicroPython
=========================

With MicroPython already installed in your board there are several flavors and
tools to work with micropython (:ref:`Installing MicroPython <psoc6_mpy_install>`). 
In this section we introduce some of the ways you can work with MicroPython.

Serial prompt (REPL Mode)
-------------------------

With MicroPython deployed on your PSoC6™ board, you can access the REPL mode using
the USB-UART interface from the on-board debugger. 

REPL stands for Read Evaluate Print Loop, and is the name given to the interactive MicroPython
prompt that you can access on the PSoC6™ board. Using the REPL is by far the easiest way to test out your
code and run commands. This is equivalent to running the *python* commmand (without passing a script) in the command line terminal of your machine.

Use your preferred serial terminal software to connect to the board. Examples of serial
terminal tools are `Putty <https://www.putty.org/>`_, which works for Windows and
Unix machines; or other platform specific such as `Tera Term <https://ttssh2.osdn.jp/index.html.en>`_, or `minicom <https://en.wikipedia.org/wiki/Minicom>`_. 

Configure the serial connection with **115200 bauds** and **8-N-1** (8 bits frame, no parity and 1 stop
bit), and connect to the board port, the MicroPython REPL prompt will appear, and you can start
typing some python code :) 

.. image:: img/mpy-psoc6-repl.jpg
    :alt: MicroPython REPL prompt
    :width: 520px

Running a script
----------------

In order to implement more elaborated programs, and use the embedded device stand-alone you can write
python scripts.

There are several IDEs that you can install that integrate a text editor with the tools to run your
python script in your MicroPython device, as well as handling the files system of your MicroPython
device. The most popular are:

* `Thonny <https://thonny.org/>`_
* `Mu Editor <https://codewith.mu/>`_
* `Arduino Lab for MicroPython <https://labs.arduino.cc/en/labs/micropython>`_

Alternatively, MicroPython offers :ref:`mpremote` as a command line tool that can be as well used for executing
scripts. Find more the information it the provided link.

In MicroPython there are primarily two ways to execute a script:  

Host REPL mode 
~~~~~~~~~~~~~~

In this mode, the MicroPython PSoC6™ board is connected through the serial interface to the
host development machine. 
Each of the line will be executed in the controller. Any output like print messages in your application or
exceptions will be send through the serial connection to the host machine, which will display them
in the serial terminal console. 

The REPL mode is used, but the IDE or command line tool will take care of sending
each line of the script and process its output to show it in the terminal.
 
On-target file system mode
~~~~~~~~~~~~~~~~~~~~~~~~~~

When the board provides a file system and data storage, you will have the possibility to store your
scripts in the device. 

You can split your program in different files, and use import to make use of the provided features
in other scripts. 
To run a script in the device at boot, there are two scripts that need to be present in the file
system: ``boot.py`` and ``main.py``. The scripts are executed in the sequence, first ``boot.py`` followed by ``main.py``. 

User-defined python modules or code can also be converted into bytecode (:ref:`mpy files <mpy_files>`) by using the ``mpy-cross`` tool and then copied onto the
filesystem for getting speed benefit in execution. 

A more advanced and efficient mode of script execution is also available wherein, the scripts to be executed are :ref:`frozen <optimizations>` as a part of the application. However, this is only possible if the user has access to the build flow of the specific port.      

The filesystem is described in the section below with some examples.

The MicroPython filesystem
---------------------------

The PSoC6™ port offers both the ``FAT`` and ``LFS2`` filesystems, implemented in :ref:`MicroPython <filesystem>`. However, given its stabilty and reliability, the ``LFS2`` filesystem is selected as default. In addition, the filesystem is located, by default on the External Flash which has a capacity of 512 Mb (64 MB). 

The filesystem is mounted with the help of frozen scripts, located in the ``ports/psoc6/freeze`` directory. The default mount point of the filesystem is the ``/flash`` directory, which serves as its root. 

Given below are a few examples on various operations on the filesystem:

Creating and reading files
~~~~~~~~~~~~~~~~~~~~~~~~~~

MicroPython on the PSoC6™ supports the standard way of accessing files in
CPython, using the built-in ``open()`` function. The ``open()`` function returns a file pointer. The file is created if not already present and  its contents overwritten, otherwise.

To create a file::

    >>> f = open('data.txt', 'w')
    >>> f.write('some data')
    9
    >>> f.close()

The number "9" returned from the function, is the number of bytes that were written with the ``write()`` method.
Then the user can read back the contents of this new file using::

    >>> f = open('data.txt')
    >>> f.read()
    'some data'
    >>> f.close()

Note that the default mode when opening a file is to open it in read-only mode,
and as a text file.  Specify ``'wb'`` as the second argument to ``open()`` to
open for writing in binary mode, and ``'rb'`` to open for reading in binary
mode.

Listing file and more
~~~~~~~~~~~~~~~~~~~~~

The ``os`` module can be used for further control over the filesystem. First,
the ``os`` module need to be imported::

    >>> import os

Then the contents of the filesystem can be listed::

    >>> os.listdir()
    ['boot.py', 'port_config.py', 'data.txt']

New directories can be created::

    >>> os.mkdir('dir')

And entries can be removed::

    >>> os.remove('data.txt')

Also, entries can be renamed::

    >>> os.rename('data.txt','data_new.txt')  # os.rename('old_filepath','new_filepath')
    
Start up scripts
~~~~~~~~~~~~~~~~

As mentioned above, there are two files that are treated specially by the port when it starts up:
``boot.py`` and ``main.py``. The user can create these files and populate them with the code that can run at startup.

Using MicroPython remote control (mpremote) for filesystem operations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The :ref:`mpremote <mpremote>` tool can be used to transfer files located on the user's host filesystem into the MicroPython filesystem using the command shown below:

.. code-block:: bash

    $ mpremote.py cp <path_to_script_on_host>/main.py :/flash/main.py


Similarly, to transfer files from the MicroPython filesystem to the host filesystem, the arguments have to be flipped, as shown:

.. code-block:: bash

    $ mpremote.py cp :/flash/main.py <path_to_script_on_host>/main.py


The tool can also be used to execute scripts on the target board, without utilizing the target's filesystem. In this case, the script is sent to the target over the UART interface and is executed on the REPL, where it is interpreted. Due to the bottleneck of the transmission, execution times may be longer. An example is given below:

.. code-block:: bash

    $ mpremote.py run <path_to_script_on_host>/main.py



Using third-party IDEs for filesystem operations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Thonny
^^^^^^

The MicroPython port for PSoC6™ can be detected by the `Thonny IDE <https://thonny.org/>`_ when the ``MicroPython (generic)`` option is selected at the bottom right corner, as shown. Additionally, the filesystem is detected by the IDE, as shown in the lower left column. Using the GUI, file operations can be carried out, such as creating a new file, adding contents to it and then saving it to the filesystem on the MicroPython device, with a given name.

.. image:: img/mpy-thonny-filesystem.jpg
    :alt: Filesystem operation using Thonny IDE 
    :width: 800px
