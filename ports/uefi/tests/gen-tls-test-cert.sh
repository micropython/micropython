#!/usr/bin/env bash
# Generate the throwaway self-signed cert/key used by the network TLS self-test:
# the host TLS echo peer (harness) presents it, and the guest verifies against it
# (T2). Regenerated on demand and git-ignored — never committed. SAN covers the
# test hostname and the SLIRP gateway address the guest dials.
set -euo pipefail
dir="$(cd "$(dirname "$0")" && pwd)"

openssl req -x509 -newkey rsa:2048 -nodes \
    -keyout "$dir/tls_test_key.pem" -out "$dir/tls_test_cert.pem" \
    -days 3650 -subj "/CN=mpy-uefi.test" \
    -addext "subjectAltName=DNS:mpy-uefi.test,IP:10.0.2.2" 2>/dev/null
# DER form too: both TLS backends parse it identically (mbedTLS is picky about PEM).
openssl x509 -in "$dir/tls_test_cert.pem" -outform DER -out "$dir/tls_test_cert.der"
echo "generated tests/tls_test_cert.{pem,der} + tls_test_key.pem"
