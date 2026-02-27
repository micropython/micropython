print("Testing runtime aspects of collections.abc module")

try:
    import typing
except ImportError:
    print("SKIP")
    raise SystemExit


print("Testing : collections.abc.Mapping, Sequence")

# FIXME: from collections.abc import Mapping, Sequence
from typing import Mapping, Sequence


class Employee:
    ...


def notify_by_email(employees: Sequence[Employee], overrides: Mapping[str, str]) -> None:
    pass


notify_by_email([], {})


print("Testing : collections.abc.Callable, Awaitable")

# from collections.abc import Callable, Awaitable
from typing import Callable, Awaitable


def feeder(get_next_item: Callable[[], str]) -> None:
    ...  # Body


def async_query(
    on_success: Callable[[int], None], on_error: Callable[[int, Exception], None]
) -> None:
    ...  # Body


async def on_update(value: str) -> None:
    ...  # Body


callback: Callable[[str], Awaitable[None]] = on_update

# ...


def concat(x: str, y: str) -> str:
    return x + y


x: Callable[..., str]
x = str  # OK
x = concat  # Also OK


print("Testing : collections.abc.Iterable")

# FIXME: from collections.abc import Iterable
from typing import Iterable
from typing import Protocol


class Combiner(Protocol):
    def __call__(self, *vals: bytes, maxlen: int | None = None) -> list[bytes]:
        ...


def batch_proc(data: Iterable[bytes], cb_results: Combiner) -> bytes:
    for item in data:
        pass
    return b"".join(cb_results(*data))


def good_cb(*vals: bytes, maxlen: int | None = None) -> list[bytes]:
    return [val[:maxlen] for val in vals if maxlen is not None]


batch_proc([], good_cb)  # OK


print("Testing : collections.abc.")
print("Testing : collections.abc.")
print("Testing : collections.abc.")
