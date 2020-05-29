MBEDTLS Error Strings for MicroPython
=====================================

This directory contains source code and tools to rework the Mbedtls error strings for
micropython to use less space. In short, instead of storing and printing something like
"SSL - Our own certificate(s) is/are too large to send in an SSL message" it prints
the name of the error #define, which would be `MBEDTLS_ERR_SSL_CERTIFICATE_TOO_LARGE` in
this case, and only stores `SSL_CERTIFICATE_TOO_LARGE` in flash. The exact Mbedtls error
defines are used because they're easy to search for to find more detailed information.

Mbedtls defines a specific format for error values and
includes a Perl script to gather all `MBEDTLS_ERR` defines from includes files together with
english error text. From that the Perl script generates `mbedtls_strerror()`. The files in this
directory modify this process to produce a more space efficient error lookup table with
shorter error strings. The files are as follows:
- `generate_errors_orig.pl` - original mbedtls perl script, copied here for reference
- `error_orig.fmt` - original mbedtls code template input to perl script
- `generate_errors.pl` - modified perl script for MicroPython
- `error.fmt` - modified code template for MicroPython
- `esp32_mbedtls_errors.c` - source file with `mbedtls_strerror` for Esp32, this is built
  using the include files in ESP-IDF's version of mbedtls
- `do-esp32.sh` - shell script to produce `esp32_mbedtls_errors.c` -- note that as of v4.0
  and v3.3.2 ESP-IDF uses the same version of mbedtls
- `mp_mbedtls_errors.c` - source file with `mbedtls_strerror` for ports using MicroPython's
  version of mbedtls, this is built using the include files in `$MPY/lib/mbedtls`
- `do-mp.sh` - shell script to produce `mp_mbedtls_errors.c`
- `tester.c` - simple C main to test `mp_mbedtls_errors.c` locally on a dev box
- `do-test.sh` - shell script to produce `mp_mbedtls_errors.c` and compile the `tester` app

### How-to

- To build MicroPython all that is needed is to include the `xx_mbedtls_errors.c` into the build
  (the Makefiles do this automatically). Note that Perl is not needed for routine MicroPython
  builds.
- When a new version of Mbedtls is pulled-in the appropriate `do_` script(s) should be run and
  a diff using the `_orig` files can show whether Mbedtls changed something in the error
  formatting.
- The `tester` app doesn't need to be run unless changes to the string handling in `error.fmt`
  are made: it really just tests that there is not an off-by-one error in the string
  copying/appending, etc.
- To include/exclude `mbedtls_strerror` error strings in a specific MicroPython build use the
  `??` macro. Excluding causes `mbedtls_strerror` to produce a string with the raw error code.
  On the esp32, define/undefine `MBEDTLS_ERROR_C` in the ESP-IDF
  `components/mbedtls/port/include/mbedtls/esp_config.h`.
