class CORS:
    """Add CORS headers to HTTP responses.

    :param app: The application to add CORS headers to.
    :param allowed_origins: A list of origins that are allowed to make
                            cross-site requests. If set to '*', all origins are
                            allowed.
    :param allow_credentials: If set to True, the
                              ``Access-Control-Allow-Credentials`` header will
                              be set to ``true`` to indicate to the browser
                              that it can expose cookies and authentication
                              headers.
    :param allowed_methods: A list of methods that are allowed to be used when
                            making cross-site requests. If not set, all methods
                            are allowed.
    :param expose_headers: A list of headers that the browser is allowed to
                           exposed.
    :param allowed_headers: A list of headers that are allowed to be used when
                            making cross-site requests. If not set, all headers
                            are allowed.
    :param max_age: The maximum amount of time in seconds that the browser
                    should cache the results of a preflight request.
    :param handle_cors: If set to False, CORS headers will not be added to
                        responses. This can be useful if you want to add CORS
                        headers manually.
    """
    def __init__(self, app=None, allowed_origins=None, allow_credentials=False,
                 allowed_methods=None, expose_headers=None,
                 allowed_headers=None, max_age=None, handle_cors=True):
        self.allowed_origins = allowed_origins
        self.allow_credentials = allow_credentials
        self.allowed_methods = allowed_methods
        self.expose_headers = expose_headers
        self.allowed_headers = None if allowed_headers is None \
            else [h.lower() for h in allowed_headers]
        self.max_age = max_age
        if app is not None:
            self.initialize(app, handle_cors=handle_cors)

    def initialize(self, app, handle_cors=True):
        """Initialize the CORS object for the given application.

        :param app: The application to add CORS headers to.
        :param handle_cors: If set to False, CORS headers will not be added to
                            responses. This can be useful if you want to add
                            CORS headers manually.
        """
        self.default_options_handler = app.options_handler
        if handle_cors:
            app.options_handler = self.options_handler
            app.after_request(self.after_request)
            app.after_error_request(self.after_request)

    def options_handler(self, request):
        headers = self.default_options_handler(request)
        headers.update(self.get_cors_headers(request))
        return headers

    def get_cors_headers(self, request):
        """Return a dictionary of CORS headers to add to a given request.

        :param request: The request to add CORS headers to.
        """
        cors_headers = {}
        origin = request.headers.get('Origin')
        if self.allowed_origins == '*':
            cors_headers['Access-Control-Allow-Origin'] = origin or '*'
            if origin:
                cors_headers['Vary'] = 'Origin'
        elif origin in (self.allowed_origins or []):
            cors_headers['Access-Control-Allow-Origin'] = origin
            cors_headers['Vary'] = 'Origin'
        if self.allow_credentials and \
                'Access-Control-Allow-Origin' in cors_headers:
            cors_headers['Access-Control-Allow-Credentials'] = 'true'
        if self.expose_headers:
            cors_headers['Access-Control-Expose-Headers'] = \
                ', '.join(self.expose_headers)

        if request.method == 'OPTIONS':
            # handle preflight request
            if self.max_age:
                cors_headers['Access-Control-Max-Age'] = str(self.max_age)

            method = request.headers.get('Access-Control-Request-Method')
            if method:
                method = method.upper()
                if self.allowed_methods is None or \
                        method in self.allowed_methods:
                    cors_headers['Access-Control-Allow-Methods'] = method

            headers = request.headers.get('Access-Control-Request-Headers')
            if headers:
                if self.allowed_headers is None:
                    cors_headers['Access-Control-Allow-Headers'] = headers
                else:
                    headers = [h.strip() for h in headers.split(',')]
                    headers = [h for h in headers
                               if h.lower() in self.allowed_headers]
                    cors_headers['Access-Control-Allow-Headers'] = \
                        ', '.join(headers)

        return cors_headers

    def after_request(self, request, response):
        saved_vary = response.headers.get('Vary')
        response.headers.update(self.get_cors_headers(request))
        if saved_vary and saved_vary != response.headers.get('Vary'):
            response.headers['Vary'] = (
                saved_vary + ', ' + response.headers['Vary'])
