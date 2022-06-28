#! /bin/bash -e
# Generate mp_mbedtls_errors.c and build the tester app
./do-mp.sh
cc -o tester -I../mbedtls/include/ mp_mbedtls_errors.c tester.c
