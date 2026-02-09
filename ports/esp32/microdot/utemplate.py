from utemplate import recompile

_loader = None


class Template:
    """A template object.

    :param template: The filename of the template to render, relative to the
                     configured template directory.
    """
    @classmethod
    def initialize(cls, template_dir='templates',
                   loader_class=recompile.Loader):
        """Initialize the templating subsystem.

        :param template_dir: the directory where templates are stored. This
                             argument is optional. The default is to load
                             templates from a *templates* subdirectory.
        :param loader_class: the ``utemplate.Loader`` class to use when loading
                             templates. This argument is optional. The default
                             is the ``recompile.Loader`` class, which
                             automatically recompiles templates when they
                             change.
        """
        global _loader
        _loader = loader_class(None, template_dir)

    def __init__(self, template):
        if _loader is None:  # pragma: no cover
            self.initialize()
        #: The name of the template
        self.name = template
        self.template = _loader.load(template)

    def generate(self, *args, **kwargs):
        """Return a generator that renders the template in chunks, with the
        given arguments."""
        return self.template(*args, **kwargs)

    def render(self, *args, **kwargs):
        """Render the template with the given arguments and return it as a
        string."""
        return ''.join(self.generate(*args, **kwargs))

    def generate_async(self, *args, **kwargs):
        """Return an asynchronous generator that renders the template in
        chunks, using the given arguments."""
        class sync_to_async_iter():
            def __init__(self, iter):
                self.iter = iter

            def __aiter__(self):
                return self

            async def __anext__(self):
                try:
                    return next(self.iter)
                except StopIteration:
                    raise StopAsyncIteration

        return sync_to_async_iter(self.generate(*args, **kwargs))

    async def render_async(self, *args, **kwargs):
        """Render the template with the given arguments asynchronously and
        return it as a string."""
        response = ''
        async for chunk in self.generate_async(*args, **kwargs):
            response += chunk
        return response
