from ussl import *
import ussl as _ussl

# constants
PROTOCOL_TLS_CLIENT = const(0)
PROTOCOL_TLS_SERVER = const(1)


# backwards compatibility even after C code is deprecated
def wrap_socket(
    sock,
    server_side=False,
    key=None,
    cert=None,
    cert_reqs=CERT_NONE,
    cadata=None,
    server_hostname=None,
    do_handshake=True,
    keyfile=None,
    certfile=None,
    ciphers="",
):
    ctx = _ussl.ctx_init()
    if keyfile:
        key = keyfile
    if certfile:
        cert = certfile
    if key is not None:  # and (cert is not None):
        ctx.load_certchain(key=key, cert=cert)
    if cadata:
        ctx.load_cadata(cadata)
    if ciphers:
        ctx.set_ciphers(ciphers.split(":"))
    return ctx.wrap_socket(
        sock,
        server_side=server_side,
        cert_reqs=cert_reqs,
        server_hostname=server_hostname,
        do_handshake=do_handshake,
    )


class SSLContext:
    def __init__(self, protocol):
        # protocol is PROTOCOL_TLS_CLIENT or PROTOCOL_TLS_SERVER
        self._protocol = protocol
        self._verify_mode = CERT_NONE
        self._check_hostname = False
        if protocol == PROTOCOL_TLS_CLIENT:
            self.verify_mode = CERT_REQUIRED
            self.check_hostname = True
        self.key = None
        self.cert = None
        self.cadata = None
        self.ciphersuite = None
        self.ctx = _ussl.ctx_init()
        self._reload_ctx = False

        # PRE-ALLOCATE CONTEXT(key,cert) and SSL socket BUFFERS e.g. from (ussl) ?
        # self.ssl_buffer_sock = _ussl.context_allocate_buffer()
        # initiate context so we can get/set ciphers? :
        # self._ctx = _ussl.context_init()
        #

    @property
    def protocol(self):
        return self._protocol

    @property
    def check_hostname(self):
        return self._check_hostname

    @check_hostname.setter
    def check_hostname(self, value):
        assert type(value) == bool

        if value is True and not self.verify_mode:
            self.verify_mode = CERT_REQUIRED

        self._check_hostname = value

    @property
    def verify_mode(self):
        return self._verify_mode

    @verify_mode.setter
    def verify_mode(self, value):
        assert value in (CERT_NONE, CERT_OPTIONAL, CERT_REQUIRED)
        if not self.check_hostname:
            self._verify_mode = value

    def _reset(self, server=True):
        ctx = _ussl.ctx_init()
        if server:
            ctx.load_certchain(key=self.key, cert=self.cert)
            if self.cadata:
                ctx.load_cadata(self.cadata)
        else:
            if self.cadata:
                ctx.load_cadata(self.cadata)
            if self.key:
                ctx.load_certchain(key=self.key, cert=self.cert)

        if self.ciphersuite:
            ctx.set_ciphers(self.ciphersuite)
        return ctx

    def load_cert_chain(self, certfile, keyfile=None):
        # load certificate/key from a file, is possible to use memoryview to
        # save RAM?
        if isinstance(certfile, bytes):
            self.cert = certfile
        else:
            with open(certfile, "rb") as cert:
                self.cert = cert.read()
        if keyfile:
            if isinstance(keyfile, bytes):
                self.key = keyfile
            else:
                with open(keyfile, "rb") as key:
                    self.key = key.read()

        self.ctx.load_certchain(key=self.key, cert=self.cert)

    def load_default_certs(self):
        pass  # not sure how/if implement this

    def load_verify_locations(self, cafile=None, capath=None, cadata=None):
        if cafile:
            with open(cafile, "rb") as ca_cert:
                self.cadata = ca_cert.read()
        if capath:
            pass  ## not implemented, it needs to concatenate multiple files

        if cadata:
            self.cadata = cadata

        self.ctx.load_cadata(self.cadata)

    def get_ciphers(self):
        return self.ctx.get_ciphers()

    def set_ciphers(self, ciphersuite):
        self.ciphersuite = ciphersuite.split(":")
        return self.ctx.set_ciphers(self.ciphersuite)

    def wrap_socket(
        self,
        sock,
        server_side=False,
        do_handshake_on_connect=True,
        server_hostname=None,
    ):
        if self.check_hostname and server_hostname is None:
            raise ValueError("check_hostname requires server_hostname")
        # to be substituted by e.g. _ussl._context_wrap_socket in modussl_mbedtls.c ?:
        # _ussl._context_wrap_socket(*args, **kargs)
        if self._reload_ctx:
            self.ctx = self._reset(server=server_side)
        try:
            ssl_sock = self.ctx.wrap_socket(
                sock,
                server_side=server_side,
                cert_reqs=self.verify_mode,
                server_hostname=server_hostname,
                do_handshake=do_handshake_on_connect,
            )
            self._reload_ctx = True
        except Exception as e:
            self._reload_ctx = True
            raise e
        return ssl_sock
