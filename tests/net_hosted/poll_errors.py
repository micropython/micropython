import time
try:
    import uselect as select, usocket as socket
except:
    import select, socket

ai=socket.getaddrinfo('micropython.org', 80)[0][-1]

s = socket.socket()
poller = select.poll()
poller.register(s)

print(poller.poll(0))

try:
    s.send(b'123')
except OSError as er:
    print(repr(er))

s.connect(ai)

print(poller.poll(0))

s.send(b'1234')

print(poller.poll(0))
time.sleep(1)
print(s.recv(10))
print(poller.poll(0))
time.sleep(1)
print(s.recv(10))
print(poller.poll(0))
time.sleep(1)
print(s.recv(10))
print(poller.poll(0))
time.sleep(1)
print(s.recv(10))
print(poller.poll(0))
for i in range(8):
    time.sleep(2)
    try:
        print('send', s.send(b'456'))
    except OSError as er:
        print(repr(er))
    print(poller.poll(0))
s.close()
