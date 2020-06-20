MBEDTLS Error Strings for MicroPython
=====================================

This directory contains source code and tools to rework the Mbedtls error strings for
micropython to use less space. In short, instead of storing and printing something like
"SSL - Our own certificate(s) is/are too large to send in an SSL message" it prints
the name of the error #define, which would be "MBEDTLS_ERR_SSL_CERTIFICATE_TOO_LARGE" in
this case, and only stores `SSL_CERTIFICATE_TOO_LARGE` in flash. The exact Mbedtls error
defines are used because they're easy to search for to find more detailed information.

Mbedtls defines a specific format for error value #defines and
includes a Perl script to gather all `MBEDTLS_ERR` defines from includes files together with
english error text. From that the Perl script generates `mbedtls_strerror()`. The files in this
directory modify this process to produce a more space efficient error lookup table with
shorter error strings. Since the esp32 port uses Mbedtls from the ESP-IDF source while other ports
use the Mbedtls version in the MicroPython source there are two different versions of files.
The files are as follows:
- `generate_errors.diff` - diff for original mbedtls perl script
- `esp32_generate_errors.pl` - perl script with above diff applied, for esp32
- `mp_generate_errors.pl` - perl script with above diff applied, for other ports
- `error.fmt` - modified code template for MicroPython
- `esp32_mbedtls_errors.c` - source file with `mbedtls_strerror` for Esp32, this is built
  using the include files in ESP-IDF's version of mbedtls
- `do-esp32.sh` - shell script to produce `esp32_mbedtls_errors.c` -- note that as of v4.0
  and v3.3.2 ESP-IDF uses the same version of mbedtls
- `mp_mbedtls_errors.c` - source file with `mbedtls_strerror` for ports using MicroPython's
  version of mbedtls, this is built using the include files in `../mbedtls`
- `do-mp.sh` - shell script to produce `mp_mbedtls_errors.c`
- `tester.c` - simple C main to test `mp_mbedtls_errors.c` locally on a dev box
- `do-test.sh` - shell script to produce `mp_mbedtls_errors.c` and compile the `tester` app

### How-to

- To build MicroPython all that is needed is to include the `xx_mbedtls_errors.c` into the build
  (the Makefiles do this automatically). Note that Perl is not needed for routine MicroPython
  builds.
- When a new version of Mbedtls is pulled-in the appropriate `do_` script(s) should be run to
  re-generate `esp32_mbedtls_errors.c` and/or `mp_mbedtls_errors.c`.
- The `tester` app should be run if changes to the string handling in `error.fmt` are made:
  it tests that there is not an off-by-one error in the string copying/appending, etc.
- To include `mbedtls_strerror` error strings in a specific MicroPython build use
  `MICROPY_SSL_MBEDTLS_ERRSTR=1`. By default `mbedtls_strerror` produces a string with the raw
  error code and no text message.
  On the esp32, define `MBEDTLS_ERROR_C` in the ESP-IDF
  `components/mbedtls/port/include/mbedtls/esp_config.h` (this is the ESP-IDF default).
