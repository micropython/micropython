from microdot import abort
from microdot.microdot import invoke_handler


class BaseAuth:
    def __init__(self):
        self.auth_callback = None
        self.error_callback = None

    def __call__(self, f):
        """Decorator to protect a route with authentication.

        An instance of this class must be used as a decorator on the routes
        that need to be protected. Example::

           auth = BasicAuth()  # or TokenAuth()

           @app.route('/protected')
           @auth
           def protected(request):
               # ...

        Routes that are decorated in this way will only be invoked if the
        authentication callback returned a valid user object, otherwise the
        error callback will be executed.
        """
        async def wrapper(request, *args, **kwargs):
            auth = self._get_auth(request)
            if not auth:
                return await invoke_handler(self.error_callback, request)
            request.g.current_user = await invoke_handler(
                self.auth_callback, request, *auth)
            if not request.g.current_user:
                return await invoke_handler(self.error_callback, request)
            return await invoke_handler(f, request, *args, **kwargs)

        return wrapper


class BasicAuth(BaseAuth):
    """Basic Authentication.

    :param realm: The realm that is displayed when the user is prompted to
                  authenticate in the browser.
    :param charset: The charset that is used to encode the realm.
    :param scheme: The authentication scheme. Defaults to 'Basic'.
    :param error_status: The error status code to return when authentication
                         fails. Defaults to 401.
    """
    def __init__(self, realm='Please login', charset='UTF-8', scheme='Basic',
                 error_status=401):
        super().__init__()
        self.realm = realm
        self.charset = charset
        self.scheme = scheme
        self.error_status = error_status
        self.error_callback = self.authentication_error

    def _get_auth(self, request):
        auth = request.headers.get('Authorization')
        if auth and auth.startswith('Basic '):
            import binascii
            try:
                username, password = binascii.a2b_base64(
                    auth[6:]).decode().split(':', 1)
            except Exception:  # pragma: no cover
                return None
            return username, password

    def authentication_error(self, request):
        return '', self.error_status, {
            'WWW-Authenticate': '{} realm="{}", charset="{}"'.format(
                self.scheme, self.realm, self.charset)}

    def authenticate(self, f):
        """Decorator to configure the authentication callback.

        This decorator must be used with a function that accepts the request
        object, a username and a password and returns a user object if the
        credentials are valid, or ``None`` if they are not. Example::

           @auth.authenticate
           async def check_credentials(request, username, password):
               user = get_user(username)
               if user and user.check_password(password):
                   return get_user(username)
        """
        self.auth_callback = f


class TokenAuth(BaseAuth):
    """Token based authentication.

    :param header: The name of the header that will contain the token. Defaults
                   to 'Authorization'.
    :param scheme: The authentication scheme. Defaults to 'Bearer'.
    :param error_status: The error status code to return when authentication
                         fails. Defaults to 401.
    """
    def __init__(self, header='Authorization', scheme='Bearer',
                 error_status=401):
        super().__init__()
        self.header = header
        self.scheme = scheme.lower()
        self.error_status = error_status
        self.error_callback = self.authentication_error

    def _get_auth(self, request):
        auth = request.headers.get(self.header)
        if auth:
            if self.header == 'Authorization':
                try:
                    scheme, token = auth.split(' ', 1)
                except Exception:
                    return None
                if scheme.lower() == self.scheme:
                    return (token.strip(),)
            else:
                return (auth,)

    def authenticate(self, f):
        """Decorator to configure the authentication callback.

        This decorator must be used with a function that accepts the request
        object, a username and a password and returns a user object if the
        credentials are valid, or ``None`` if they are not. Example::

           @auth.authenticate
           async def check_credentials(request, token):
               return get_user(token)
        """
        self.auth_callback = f

    def errorhandler(self, f):
        """Decorator to configure the error callback.

        Microdot calls the error callback to allow the application to generate
        a custom error response. The default error response is to call
        ``abort(401)``.
        """
        self.error_callback = f

    def authentication_error(self, request):
        abort(self.error_status)
