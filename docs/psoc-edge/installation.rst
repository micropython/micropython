.. _pse_mpy_install:

Installing MicroPython
======================

To facilitate the installation of the MicroPython PSOC™ Edge port, the ``mp-ifx-flash.py`` Python script is
provided. It is compatible with Windows, Linux, and macOS.

Prerequisites
-------------

Before downloading and running the script, it is recommended to create a new folder to keep all the
related files together. For example:

.. code-block:: bash

    $ mkdir mp-install
    $ cd mp-install

You can easily download the script from the terminal using the following command:

.. code-block:: bash

    $ curl -s -L https://raw.githubusercontent.com/micropython/micropython/master/ports/psoc-edge/tools/mp-ifx-flash.py > mp-ifx-flash.py

Ensure you have a recent version of `Python 3.x <https://www.python.org/downloads/>`_ and the
`pip <https://pip.pypa.io/en/stable/installation/>`_ package installer installed. Then install the following packages:

.. code-block:: bash

    $ pip install requests

Getting the firmware
^^^^^^^^^^^^^^^^^^^^

Download the desired MicroPython firmware version for the PSOC™ Edge board from the `MicroPython download page <https://micropython.org/download/>`_.
The downloaded file is a ``.zip`` package containing the firmware binary and the necessary files for flashing.

Flashing
--------

To flash the firmware, use the ``from-package`` command.
Specify the board using the ``--board`` flag and the path to the
downloaded ``.zip`` package using the ``--zip-package`` flag, as shown below:

.. code-block:: bash

    $ python mp-ifx-flash.py from-package --board KIT_PSE84_AI --zip-package pathtodir/psoc-edge-package.zip

Multiple connected devices
^^^^^^^^^^^^^^^^^^^^^^^^^^

If you have multiple PSOC™ Edge boards connected to your computer, you can identify them by their
serial number and flash each one individually. To do so, use the ``-n`` flag to provide the serial number of
the target board as shown below:

.. code-block:: bash

    $ python mp-ifx-flash.py from-package --board KIT_PSE84_AI --zip-package pathtodir/psoc-edge-package.zip -n 181F0D5A01212300



