# Attempt to test the funky poll patch in modussl_mbedtls, but sadly this works
# without the patch...

try:
    import uasyncio as asyncio, ussl as ssl
except ImportError:
    try:
        import asyncio, ssl
    except ImportError:
        print("SKIP")
        raise SystemExit


# open a connection and start by reading, not writing
async def read_first(host, port):
    reader, writer = await asyncio.open_connection(host, port, ssl=True)

    print("read something")
    inbuf = b""
    while len(inbuf) < 20:
        try:
            b = await reader.read(100)
        except OSError as e:
            if e.args[0] < -120:
                print("read SSL error -%x : %s" % (-e.args[0], ssl.errstr(e.args[0])))
                raise OSError(e.args[0], bytes.decode(ssl.errstr(e.args[0])))
            else:
                print("read OSError: %d / -%x" % (e.args[0], -e.args[0]))
                raise
        print("read:", b)
        if b is None:
            continue
        elif len(b) == 0:
            print("EOF")
            break
        elif len(b) > 0:
            inbuf += b
        else:
            raise ValueError("negative length returned by recv")

    print("close")
    writer.close()
    await writer.wait_closed()
    print("done")


asyncio.run(read_first("aspmx.l.google.com", 25))
