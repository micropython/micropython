.. _writingtests:

Testing MicroPython
===================

MicroPython includes a comprehensive test suite to ensure correctness and stability across all ports and platforms.

Quick Start
-----------

If you want to run existing tests or check if your MicroPython build is working correctly, see:

- :ref:`test_quickstart` - Essential information for running tests

Developer Documentation
-----------------------

If you're contributing to MicroPython and need to write or modify tests, see:

- :ref:`test_developers` - Comprehensive guide for test development

Test Categories
---------------

The test suite includes:

- **Standard tests** - Core functionality tests that run on a single instance
- **Multi-instance tests** - Tests for networking, Bluetooth, and other communication
- **Performance benchmarks** - Measure execution speed and efficiency
- **Native module tests** - Verify dynamic native modules
- **Internal benchmarks** - Low-level VM and C code performance

All test scripts are located in the ``tests/`` directory with various subdirectories for organization.

.. toctree::
   :hidden:
   :maxdepth: 1

   test_quickstart
   test_developers