from time import time
from microdot import redirect
from microdot.microdot import urlencode, invoke_handler


class Login:
    """User login support for Microdot.

    :param login_url: the URL to redirect to when a login is required. The
                      default is '/login'.
    """
    def __init__(self, login_url='/login'):
        self.login_url = login_url
        self.user_loader_callback = None

    def user_loader(self, f):
        """Decorator to configure the user callback.

        The decorated function receives the user ID as an argument and must
        return the corresponding user object, or ``None`` if the user ID is
        invalid.
        """
        self.user_loader_callback = f

    def _get_session(self, request):
        return request.app._session.get(request)

    def _update_remember_cookie(self, request, days, user_id=None):
        remember_payload = request.app._session.encode({
            'user_id': user_id,
            'days': days,
            'exp': time() + days * 24 * 60 * 60
        })

        @request.after_request
        async def _set_remember_cookie(request, response):
            response.set_cookie('_remember', remember_payload,
                                max_age=days * 24 * 60 * 60)
            return response

    def _get_user_id_from_session(self, request):
        session = self._get_session(request)
        if session and '_user_id' in session:
            return session['_user_id']
        if '_remember' in request.cookies:
            remember_payload = request.app._session.decode(
                request.cookies['_remember'])
            user_id = remember_payload.get('user_id')
            if user_id:  # pragma: no branch
                self._update_remember_cookie(
                    request, remember_payload.get('_days', 30), user_id)
                session['_user_id'] = user_id
                session['_fresh'] = False
                session.save()
                return user_id

    async def _redirect_to_login(self, request):
        return '', 302, {'Location': self.login_url + '?next=' + urlencode(
            request.url)}

    async def login_user(self, request, user, remember=False,
                         redirect_url='/'):
        """Log a user in.

        :param request: the request object
        :param user: the user object
        :param remember: if the user's logged in state should be remembered
                         with a cookie after the session ends. Set to the
                         number of days the remember cookie should last, or to
                         ``True`` to use a default duration of 30 days.
        :param redirect_url: the URL to redirect to after login

        This call marks the user as logged in by storing their user ID in the
        user session. The application must call this method to log a user in
        after their credentials have been validated.

        The method returns a redirect response, either to the URL the user
        originally intended to visit, or if there is no original URL to the URL
        specified by the `redirect_url`.
        """
        session = self._get_session(request)
        session['_user_id'] = user.id
        session['_fresh'] = True
        session.save()

        if remember:
            days = 30 if remember is True else int(remember)
            self._update_remember_cookie(request, days, session['_user_id'])

        next_url = request.args.get('next', redirect_url)
        if not next_url.startswith('/'):
            next_url = redirect_url
        return redirect(next_url)

    async def logout_user(self, request):
        """Log a user out.

        :param request: the request object

        This call removes information about the user's log in from the user
        session. If a remember cookie exists, it is removed as well.
        """
        session = self._get_session(request)
        session.pop('_user_id', None)
        session.pop('_fresh', None)
        session.save()
        if '_remember' in request.cookies:
            self._update_remember_cookie(request, 0)

    def __call__(self, f):
        """Decorator to protect a route with authentication.

        If the user is not logged in, Microdot will redirect to the login page
        first. The decorated route will only run after successful login by the
        user. If the user is already logged in, the route will run immediately.
        Example::

            login = Login()

            @app.route('/secret')
            @login
            async def secret(request):
                # only accessible to authenticated users

        """
        async def wrapper(request, *args, **kwargs):
            user_id = self._get_user_id_from_session(request)
            if not user_id:
                return await self._redirect_to_login(request)
            request.g.current_user = await invoke_handler(
                self.user_loader_callback, user_id)
            if not request.g.current_user:
                return await self._redirect_to_login(request)
            return await invoke_handler(f, request, *args, **kwargs)

        return wrapper

    def fresh(self, f):
        """Decorator to protect a route with "fresh" authentication.

        This decorator prevents the route from running when the login session
        is not fresh. A fresh session is a session that has been created from
        direct user interaction with the login page, while a non-fresh session
        occurs when a login is restored from a "remember me" cookie. Example::

            login = Login()

            @app.route('/secret')
            @auth.fresh
            async def secret(request):
                # only accessible to authenticated users
                # users logged in via remember me cookie will need to
                # re-authenticate
        """
        base_wrapper = self.__call__(f)

        async def wrapper(request, *args, **kwargs):
            session = self._get_session(request)
            if session.get('_fresh'):
                return await base_wrapper(request, *args, **kwargs)
            return await self._redirect_to_login(request)

        return wrapper
