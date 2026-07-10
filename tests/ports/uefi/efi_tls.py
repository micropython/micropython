# TLS/ssl hermetic surface (no network): the ssl shim maps onto the C `tls` module for
# either backend (mbedtls or efi). SKIP when built TLS=none. Handshake/cert behaviour is
# covered by the upstream ssl_* tests; here we check the context surface only.
try:
    import tls
    import ssl
except ImportError:
    print("SKIP")
    raise SystemExit

print(tls.SSLContext is ssl.SSLContext)
print(ssl.SSLError is OSError)
print(
    all(
        getattr(tls, c) == getattr(ssl, c) for c in ("CERT_NONE", "CERT_OPTIONAL", "CERT_REQUIRED")
    )
)
# no-arg ctor -> TypeError
try:
    tls.SSLContext()
    print("accepted")
except TypeError:
    print("TypeError")
ctx = tls.SSLContext(tls.PROTOCOL_TLS_CLIENT)
# unknown attr load/store -> AttributeError
try:
    ctx.does_not_exist
    print("loaded")
except AttributeError:
    print("AttributeError")
try:
    ctx.does_not_exist = 1
    print("stored")
except AttributeError:
    print("AttributeError")
# server default verify_mode is CERT_NONE; get/set round-trips
sctx = tls.SSLContext(tls.PROTOCOL_TLS_SERVER)
print(sctx.verify_mode == tls.CERT_NONE)
for vm in (tls.CERT_NONE, tls.CERT_OPTIONAL, tls.CERT_REQUIRED):
    ctx.verify_mode = vm
    assert ctx.verify_mode == vm
print("SSLContext" in str(sctx))
print("tls/ssl ok")
