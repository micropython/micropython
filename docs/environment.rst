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

CIRCUITPY_HEAP_START_SIZE
~~~~~~~~~~~~~~~~~~~~~~~~~
Sets the initial size of the python heap, allocated from the outer heap. Must be a multiple of 4.
The default is currently 8192.
The python heap will grow by doubling and redoubling this initial size until it cannot fit in the outer heap.
Larger values will reserve more RAM for python use and prevent the supervisor and SDK
from large allocations of their own.
Smaller values will likely grow sooner than large start sizes.

CIRCUITPY_PYSTACK_SIZE
~~~~~~~~~~~~~~~~~~~~~~
Sets the size of the python stack. Must be a multiple of 4. The default value is currently 1536.
Increasing the stack reduces the size of the heap available to python code.
Used to avoid "Pystack exhausted" errors when the code can't be reworked to avoid it.

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
