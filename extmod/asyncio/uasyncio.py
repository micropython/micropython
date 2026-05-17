# This module just allows `import uasyncio` to work. It lazy-loads from
# `asyncio` without duplicating its globals dict.


def __getattr__(attr):
    import asyncio

    return getattr(asyncio, attr)
