Environment Variables
=====================

CircuitPython 8.0.0 introduces support for environment variables. Environment
variables are commonly used to store "secrets" such as Wi-Fi passwords and API
keys. This method *does not* make them secure. It only separates them from the
code.

CircuitPython supports these by mimicking the `dotenv <https://github.com/theskumar/python-dotenv>`_
CPython library. Other languages such as Javascript, PHP and Ruby also have
dotenv libraries.

These libraries store environment variables in a ``.env`` file. Here is a simple
example:

.. code-block:: bash

  KEY1='value1'
  # Comment
  KEY2='value2
  is multiple lines'

CircuitPython uses the ``.env`` at the drive root (no folder) as the environment.
User code can access the values from the file using `os.getenv()`. It is
recommended to save any values used repeatedly in a variable because `os.getenv()`
will parse the ``/.env`` on every access.

CircuitPython behavior
----------------------

CircuitPython will also read the environment to configure its behavior. Other
keys are ignored by CircuitPython. Here are the keys it uses:

CIRCUITPY_WIFI_PASSWORD
~~~~~~~~~~~~~~~~~~~~~~~
Wi-Fi password used to auto connect to CIRCUITPY_WIFI_SSID

CIRCUITPY_WIFI_SSID
~~~~~~~~~~~~~~~~~~~
Wi-Fi SSID to auto-connect to even if user code is not running.
