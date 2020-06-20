#! /bin/bash -e
# Generate mp_mbedtls_errors.c for inclusion in ports that use $MPY/lib/mbedtls
patch -o mp_generate_errors.pl ../mbedtls/scripts/generate_errors.pl <generate_errors.diff
perl ./mp_generate_errors.pl ../mbedtls/include/mbedtls . mp_mbedtls_errors.c
