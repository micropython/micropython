# Very basic test of DTLS.

try:
    from tls import PROTOCOL_DTLS_CLIENT, PROTOCOL_DTLS_SERVER, SSLContext
except (ImportError, NameError):
    print("SKIP")
    raise SystemExit


# Test constructing with arguments.
dtls_client = SSLContext(PROTOCOL_DTLS_CLIENT)
dtls_server = SSLContext(PROTOCOL_DTLS_SERVER)
print('OK')
