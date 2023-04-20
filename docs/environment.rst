Environment Variables
=====================

CircuitPython 8.0.0 introduces support for environment variables. Environment
variables are commonly used to store "secrets" such as Wi-Fi passwords and API
keys. This method *does not* make them secure. It only separates them from the
code.

CircuitPython uses a file called ``settings.toml`` at the drive root (no
folder) as the environment.  User code can access the values from the file
using `os.getenv()`. It is recommended to save any values used repeatedly in a
variable because `os.getenv()` will parse the ``settings.toml`` file contents
on every access.

CircuitPython only supports a subset of the full toml specification, see below
for more details. The subset is very "Python-like", which is a key reason we
selected the format.

Due to technical limitations it probably also accepts some files that are
not valid TOML files; bugs of this nature are subject to change (i.e., be
fixed) without the usual deprecation period for incompatible changes.

File format example:

.. code-block::

   str_key="Hello world" # with trailing comment
   int_key = 7
   unicode_key="œuvre"
   unicode_key2="\\u0153uvre" # same as above
   unicode_key3="\\U00000153uvre" # same as above
   escape_codes="supported, including \\r\\n\\"\\\\"
   # comment
   [subtable]
   subvalue="cannot retrieve this using getenv"


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
* due to technical limitations, the content of multi-line
  strings can erroneously be parsed as a value.

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

CIRCUITPY_WEB_INSTANCE_NAME
~~~~~~~~~~~~~~~~~~~~~~~~~~~
Name the board advertises as for the WEB workflow. Defaults to human readable board name if omitted.

CIRCUITPY_WIFI_PASSWORD
~~~~~~~~~~~~~~~~~~~~~~~
Wi-Fi password used to auto connect to CIRCUITPY_WIFI_SSID.

CIRCUITPY_WIFI_SSID
~~~~~~~~~~~~~~~~~~~
Wi-Fi SSID to auto-connect to even if user code is not running.
