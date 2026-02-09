import asyncio
import json
from microdot.helpers import wraps


class SSE:
    """Server-Sent Events object.

    An object of this class is sent to handler functions to manage the SSE
    connection.
    """
    def __init__(self):
        self.event = asyncio.Event()
        self.queue = []

    async def send(self, data, event=None, event_id=None):
        """Send an event to the client.

        :param data: the data to send. It can be given as a string, bytes, dict
                     or list. Dictionaries and lists are serialized to JSON.
                     Any other types are converted to string before sending.
        :param event: an optional event name, to send along with the data. If
                      given, it must be a string.
        :param event_id: an optional event id, to send along with the data. If
                      given, it must be a string.
        """
        if isinstance(data, (dict, list)):
            data = json.dumps(data).encode()
        elif isinstance(data, str):
            data = data.encode()
        elif not isinstance(data, bytes):
            data = str(data).encode()
        data = b'data: ' + data + b'\n\n'
        if event_id:
            data = b'id: ' + event_id.encode() + b'\n' + data
        if event:
            data = b'event: ' + event.encode() + b'\n' + data
        self.queue.append(data)
        self.event.set()


def sse_response(request, event_function, *args, **kwargs):
    """Return a response object that initiates an event stream.

    :param request: the request object.
    :param event_function: an asynchronous function that will send events to
                           the client. The function is invoked with ``request``
                           and an ``sse`` object. The function should use
                           ``sse.send()`` to send events to the client.
    :param args: additional positional arguments to be passed to the response.
    :param kwargs: additional keyword arguments to be passed to the response.

    This is a low-level function that can be used to implement a custom SSE
    endpoint. In general the :func:`microdot.sse.with_sse` decorator should be
    used instead.
    """
    sse = SSE()

    async def sse_task_wrapper():
        await event_function(request, sse, *args, **kwargs)
        sse.event.set()

    task = asyncio.create_task(sse_task_wrapper())

    class sse_loop:
        def __aiter__(self):
            return self

        async def __anext__(self):
            event = None
            while sse.queue or not task.done():
                try:
                    event = sse.queue.pop(0)
                    break
                except IndexError:
                    await sse.event.wait()
                    sse.event.clear()
            if event is None:
                raise StopAsyncIteration
            return event

        async def aclose(self):
            task.cancel()

    return sse_loop(), 200, {'Content-Type': 'text/event-stream'}


def with_sse(f):
    """Decorator to make a route a Server-Sent Events endpoint.

    This decorator is used to define a route that accepts SSE connections. The
    route then receives a sse object as a second argument that it can use to
    send events to the client::

        @app.route('/events')
        @with_sse
        async def events(request, sse):
            # send an unnamed event with string data
            await sse.send('hello')

            # send an unnamed event with JSON data
            await sse.send({'foo': 'bar'})

            # send a named event
            await sse.send('hello', event='greeting')
    """
    @wraps(f)
    async def sse_handler(request, *args, **kwargs):
        return sse_response(request, f, *args, **kwargs)

    return sse_handler
