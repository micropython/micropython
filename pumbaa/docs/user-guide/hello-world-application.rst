Hello World application
=======================

Let's start with the `Pumbaa` "Hello World" application. It
examplifies what an application is and how to build and run it.

It consists of two files; ``main.py`` and ``Makefile``.

main.py
-------

:github-blob:`main.py<examples/hello_world/main.py>` is the main
script of the application.

.. code-block:: python

   print("Hello world!")

Makefile
--------

:github-blob:`Makefile<examples/hello_world/Makefile>` contains build
configuration of the application.

.. code-block:: makefile

   NAME = hello_world
   BOARD ?= linux

   PUMBAA_ROOT ?= ../..
   include $(PUMBAA_ROOT)/make/app.mk

Build and run
-------------

Compile, link and run it by typing the commands below in a shell:

.. code-block:: text

   $ cd examples/hello_world
   $ make -s run
   <build system output>
   Hello world!
   $

Cross-compile, link and then run on an Arduino Due:

.. code-block:: text

   $ cd examples/hello_world
   $ make -s BOARD=arduino_due run
   <build system output>
   Hello world!
   $
