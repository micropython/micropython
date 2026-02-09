import os
from random import choice
from microdot import abort, iscoroutine, AsyncBytesIO
from microdot.helpers import wraps


class FormDataIter:
    """Asynchronous iterator that parses a ``multipart/form-data`` body and
    returns form fields and files as they are parsed.

    :param request: the request object to parse.

    Example usage::

        from microdot.multipart import FormDataIter

        @app.post('/upload')
        async def upload(request):
            async for name, value in FormDataIter(request):
                print(name, value)

    The iterator returns no values when the request has a content type other
    than ``multipart/form-data``. For a file field, the returned value is of
    type :class:`FileUpload`, which supports the
    :meth:`read() <FileUpload.read>` and :meth:`save() <FileUpload.save>`
    methods. Values for regular fields are provided as strings.

    The request body is read efficiently in chunks of size
    :attr:`buffer_size <FormDataIter.buffer_size>`. On iterations in which a
    file field is encountered, the file must be consumed before moving on to
    the next iteration, as the internal stream stored in ``FileUpload``
    instances is invalidated at the end of the iteration.
    """
    #: The size of the buffer used to read chunks of the request body.
    buffer_size = 256

    def __init__(self, request):
        self.request = request
        self.buffer = None
        try:
            mimetype, boundary = request.content_type.rsplit('; boundary=', 1)
        except ValueError:
            return  # not a multipart request
        if mimetype.split(';', 1)[0] == \
                'multipart/form-data':  # pragma: no branch
            self.boundary = b'--' + boundary.encode()
            self.extra_size = len(boundary) + 4
            self.buffer = b''

    def __aiter__(self):
        return self

    async def __anext__(self):
        if self.buffer is None:
            raise StopAsyncIteration

        # make sure we have consumed the previous entry
        while await self._read_buffer(self.buffer_size) != b'':
            pass

        # make sure we are at a boundary
        s = self.buffer.split(self.boundary, 1)
        if len(s) != 2 or s[0] != b'':
            abort(400)  # pragma: no cover
        self.buffer = s[1]
        if self.buffer[:2] == b'--':
            # we have reached the end
            raise StopAsyncIteration
        elif self.buffer[:2] != b'\r\n':
            abort(400)  # pragma: no cover
        self.buffer = self.buffer[2:]

        # parse the headers of this part
        name = ''
        filename = None
        content_type = None
        while True:
            await self._fill_buffer()
            lines = self.buffer.split(b'\r\n', 1)
            if len(lines) != 2:
                abort(400)  # pragma: no cover
            line, self.buffer = lines
            if line == b'':
                # we reached the end of the headers
                break
            header, value = line.decode().split(':', 1)
            header = header.lower()
            value = value.strip()
            if header == 'content-disposition':
                parts = value.split(';')
                if len(parts) < 2 or parts[0] != 'form-data':
                    abort(400)  # pragma: no cover
                for part in parts[1:]:
                    part = part.strip()
                    if part.startswith('name="'):
                        name = part[6:-1]
                    elif part.startswith('filename="'):  # pragma: no branch
                        filename = part[10:-1]
            elif header == 'content-type':  # pragma: no branch
                content_type = value

        if filename is None:
            # this is a regular form field, so we read the value
            value = b''
            while True:
                v = await self._read_buffer(self.buffer_size)
                value += v
                if len(v) < self.buffer_size:  # pragma: no branch
                    break
            return name, value.decode()
        return name, FileUpload(filename, content_type, self._read_buffer)

    async def _fill_buffer(self):
        self.buffer += await self.request.stream.read(
            self.buffer_size + self.extra_size - len(self.buffer))

    async def _read_buffer(self, n=-1):
        data = b''
        while n == -1 or len(data) < n:
            await self._fill_buffer()
            s = self.buffer.split(self.boundary, 1)
            data += s[0][:n] if n != -1 else s[0]
            self.buffer = s[0][n:] if n != -1 else b''
            if len(s) == 2:  # pragma: no branch
                # the end of this part is in the buffer
                if len(self.buffer) < 2:
                    # we have read all the way to the end of this part
                    data = data[:-(2 - len(self.buffer))]  # remove last "\r\n"
                self.buffer += self.boundary + s[1]
                return data
        return data


class FileUpload:
    """Class that represents an uploaded file.

    :param filename: the name of the uploaded file.
    :param content_type: the content type of the uploaded file.
    :param read: a coroutine that reads from the uploaded file's stream.

    An uploaded file can be read from the stream using the :meth:`read()`
    method or saved to a file using the :meth:`save()` method.

    Instances of this class do not normally need to be created directly.
    """
    #: The size at which the file is copied to a temporary file.
    max_memory_size = 1024

    def __init__(self, filename, content_type, read):
        self.filename = filename
        self.content_type = content_type
        self._read = read
        self._close = None

    async def read(self, n=-1):
        """Read up to ``n`` bytes from the uploaded file's stream.

        :param n: the maximum number of bytes to read. If ``n`` is -1 or not
                  given, the entire file is read.
        """
        return await self._read(n)

    async def save(self, path_or_file):
        """Save the uploaded file to the given path or file object.

        :param path_or_file: the path to save the file to, or a file object
                             to which the file is to be written.

        The file is read and written in chunks of size
        :attr:`FormDataIter.buffer_size`.
        """
        if isinstance(path_or_file, str):
            f = open(path_or_file, 'wb')
        else:
            f = path_or_file
        while True:
            data = await self.read(FormDataIter.buffer_size)
            if not data:
                break
            f.write(data)
        if f != path_or_file:
            f.close()

    async def copy(self, max_memory_size=None):
        """Copy the uploaded file to a temporary file, to allow the parsing of
        the multipart form to continue.

        :param max_memory_size: the maximum size of the file to keep in memory.
                                If not given, then the class attribute of the
                                same name is used.
        """
        max_memory_size = max_memory_size or FileUpload.max_memory_size
        buffer = await self.read(max_memory_size)
        if len(buffer) < max_memory_size:
            f = AsyncBytesIO(buffer)
            self._read = f.read
            return self

        # create a temporary file
        while True:
            tmpname = "".join([
                choice('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ')
                for _ in range(12)
            ])
            try:
                f = open(tmpname, 'x+b')
            except OSError as e:  # pragma: no cover
                if e.errno == 17:
                    # EEXIST
                    continue
                elif e.errno == 2:
                    # ENOENT
                    # some MicroPython platforms do not support mode "x"
                    f = open(tmpname, 'w+b')
                    if f.read(1) != b'':
                        f.close()
                        continue
                else:
                    raise
            break
        f.write(buffer)
        await self.save(f)
        f.seek(0)

        async def read(n=-1):
            return f.read(n)

        async def close():
            f.close()
            os.remove(tmpname)

        self._read = read
        self._close = close
        return self

    async def close(self):
        """Close an open file.

        This method must be called to free memory or temporary files created by
        the ``copy()`` method.

        Note that when using the ``@with_form_data`` decorator this method is
        called automatically when the request ends.
        """
        if self._close:
            await self._close()
            self._close = None


def with_form_data(f):
    """Decorator that parses a ``multipart/form-data`` body and updates the
    request object with the parsed form fields and files.

    Example usage::

        from microdot.multipart import with_form_data

        @app.post('/upload')
        @with_form_data
        async def upload(request):
            print('form fields:', request.form)
            print('files:', request.files)

    Note: this decorator calls the :meth:`FileUpload.copy()
    <microdot.multipart.FileUpload.copy>` method on all uploaded files, so that
    the request can be parsed in its entirety. The files are either copied to
    memory or a temporary file, depending on their size. The temporary files
    are automatically deleted when the request ends.
    """
    @wraps(f)
    async def wrapper(request, *args, **kwargs):
        form = {}
        files = {}
        async for name, value in FormDataIter(request):
            if isinstance(value, FileUpload):
                files[name] = await value.copy()
            else:
                form[name] = value
        if form or files:
            request._form = form
            request._files = files
        try:
            ret = f(request, *args, **kwargs)
            if iscoroutine(ret):
                ret = await ret
        finally:
            if request.files:
                for file in request.files.values():
                    await file.close()
        return ret
    return wrapper
