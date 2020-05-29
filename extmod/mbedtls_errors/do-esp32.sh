#! /bin/bash -e
# Generate esp32_mbedtls_errors.c for use in the Esp32 port, with the ESP-IDF version of mbedtls
# The ESPIDF env var must be set to the top-level dir of ESPIDF
echo "ESPIDF=$ESPIDF"
./generate_errors.pl $ESPIDF/components/mbedtls/mbedtls/include/mbedtls . esp32_mbedtls_errors.c
