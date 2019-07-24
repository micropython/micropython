# OTA PAL Testing

Please provision your device with the correct certificates in tests/common/ota/test_files before
running any OTA PAL tests (these tests are enabled by setting testrunnerFULL_OTA_PAL_ENABLED to 1, see aws_test_runner_config.h).  

The names of the files correspond to the signing algorithm associated with that certificate.  

If your OTA PAL layer port uses aws_ota_codesigner_certificate.h, then please copy in the certificate, 
with the applicable signing algorithm for your device, located under tests/common/ota/test_files. 
Currently tests/common/include/aws_ota_codesigner_certificate.h is pre-filled in with the ecdsa-sha256-signer.crt.pem
certificate located under tests/common/ota/test_files.

## OTA PAL Device Certificates File information

File: **rsa-sha256-signer.crt.pem**  - Certificate  
File: **rsa-sha256-signer.key.pem**  - Private key  
Self signed SHA256 with RSA certificates. 
  
File: **rsa-sha1-root-ca-cert-key.pem** - Root CA private key  
File: **rsa-sha1-root-ca-cert.pem** - Root CA certificate  
File: **rsa-sha1-signer.crt.pem**  - Certificate  
File: **rsa-sha1-signer.key.pem**  - Private key  
This RSA with SHA1 certificate, was signed with the accompanying root CA certificates.

File: **ecdsa-sha256-signer.crt.pem**  - Certificate  
File: **ecdsa-sha256-signer.key.pem**  - Private key  
Self signed ECDSA with SHA256 signatures.  

