try:
    import usocket as socket
    import websocket
except ImportError:
    import sys
    print("SKIP")
    sys.exit()


s = socket.socket()
ws = websocket.websocket(s)

print(ws)

ws.close()
