:mod:`os` --- Miscellaneous operating system interfaces
=======================================================

.. module:: os
   :synopsis: Miscellaneous operating system interfaces.

This module provides a portable way of using operating system
dependent functionality.

----------------------------------------------


.. function:: os.uname()

   Returns information identifying the current operating system. The
   return value is an object with five attributes:

   - sysname - operating system name
   - nodename - name of machine on network (implementation-defined)
   - release - operating system release
   - version - operating system version
   - machine - hardware identifier


.. function:: os.getcwd()

   Return a string representing the current working directory.


.. function:: os.listdir(path='.')

   Return a list containing the names of the entries in the directory
   given by path. The list is in arbitrary order, and does not include
   the special entries ``'.'`` and ``'..'`` even if they are present
   in the directory.


.. function:: os.stat()

   Return a `stat_result` object for this entry.


.. function:: os.system(command)

   Returns the output of given file system command. Raises ``OSError``
   if the command is missing or fails to execute.


.. function:: os.format(path)

   Format file system at given path. All data in the file system will
   be lost.
