Environment Variables
=====================

CircuitPython 8.0.0 introduces support for environment variables. Environment
variables are commonly used to store "secrets" such as Wi-Fi passwords and API
keys. This method *does not* make them secure. It only separates them from the
code.

CircuitPython supports these by parsing a subset of the `toml <https://toml.io/>`_ file format internally.

Here is a simple example:

.. code-block:: bash

  KEY1="value1"
  # Comment
  KEY2="value2\ncontains a newline"

  [SECTION] # Only values in the "root table" are parsed
  SECTION_VALUE = ... # so this value cannot be seen by getenv

CircuitPython uses the ``settings.toml`` at the drive root (no folder) as the environment.
User code can access the values from the file using `os.getenv()`. It is
recommended to save any values used repeatedly in a variable because `os.getenv()`
will parse the ``settings.toml`` file contents on every access.

Details of the toml language subset
-----------------------------------

* The content is required to be in UTF-8 encoding
* The supported data types are string and integer
* Only basic strings are supported, not triple-quoted strings
* Only integers supported by strtol. (no 0o, no 0b, no underscores 1_000, 011
  is 9, not 11)
* Only bare keys are supported
* Duplicate keys are not diagnosed.
* Comments are supported
* Only values from the "root table" can be retrieved

CircuitPython behavior
----------------------

CircuitPython will also read the environment to configure its behavior. Other
keys are ignored by CircuitPython. Here are the keys it uses:

CIRCUITPY_BLE_NAME
~~~~~~~~~~~~~~~~~~
Default BLE name the board advertises as, including for the BLE workflow.

CIRCUITPY_RESERVED_PSRAM
~~~~~~~~~~~~~~~~~~~~~~~~
On boards with Espressif microcontrollers with PSRAM (also called SPIRAM), permanently reserve a portion of PSRAM for use by esp-idf.
This storage is removed from the CircuitPython "heap" and is available for allocation by esp-idf routines in the core instead.
Generally, only set this to a non-zero value when it is required by a specific core module.

CIRCUITPY_WEB_API_PASSWORD
~~~~~~~~~~~~~~~~~~~~~~~~~~
Password required to make modifications to the board from the Web Workflow.

CIRCUITPY_WEB_API_PORT
~~~~~~~~~~~~~~~~~~~~~~
TCP port number used for the web HTTP API. Defaults to 80 when omitted.

CIRCUITPY_WIFI_PASSWORD
~~~~~~~~~~~~~~~~~~~~~~~
Wi-Fi password used to auto connect to CIRCUITPY_WIFI_SSID.

CIRCUITPY_WIFI_SSID
~~~~~~~~~~~~~~~~~~~
Wi-Fi SSID to auto-connect to even if user code is not running.
