#! /bin/bash -e
# Generate mp_mbedtls_errors.c for inclusion in ports that use $MPY/lib/mbedtls
./generate_errors.pl ../../lib/mbedtls/include/mbedtls . mp_mbedtls_errors.c
