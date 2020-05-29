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
shorter error strings.

The files are as follows:
- `generate_errors.diff` - diff for original mbedtls perl script
- `error.fmt` - modified code template for MicroPython
- `mp_mbedtls_errors.c` - source file with `mbedtls_strerror` this is built using the include
  files in `../mbedtls`
- `do-mp.sh` - shell script to produce `mp_mbedtls_errors.c`
- `tester.c` - simple C main to test `mp_mbedtls_errors.c` locally on a dev box
- `do-test.sh` - shell script to produce `mp_mbedtls_errors.c` and compile the `tester` app
- `do-esp32.sh` - shell script to produce `esp32_mbedtls_errors.c` -- see below

In order not to store multiple copies of `mbedtls_errors.c`
([https://github.com/micropython/micropython/pull/5819#discussion_r445528006](see))
it is assumed that all ports use the same version of mbedtls with the same error #defines.
This is true as of MP v1.13, and ESP-IDF versions 3.3.2 and 4.0.1. If anything changes in the
future the `do-esp32.sh` script can be used to generate an esp32-specific version.

### How-to

- To build MicroPython all that is needed is to include the `mp_mbedtls_errors.c` into the build
  (the Makefiles do this automatically). Note that Perl is not needed for routine MicroPython
  builds.
- When a new version of Mbedtls is pulled-in the `do-mp.sh` script should be run to
  re-generate `mp_mbedtls_errors.c`.
- The `tester` app should be run if changes to the string handling in `error.fmt` are made:
  it tests that there is not an off-by-one error in the string copying/appending, etc.
- To include `mbedtls_strerror` error strings define `MBEDTLS_ERROR_C` in the build.
