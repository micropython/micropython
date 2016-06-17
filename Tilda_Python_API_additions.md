# Additions to the Micropython API

## UGFX

```python
l=pyb.UGFX()
l.area(0,0,30,30,pyb.UGFX.RED)
btn = pyb.Button(40,40,40,30,"Hello",l)
ky = pyb.Keyboard(0,119,320,120,"")
win = pyb.Container(0,0,160,240,"")
win.show()
# win.hide()
l = pyb.List(0,50,100,100,"",win)
l.add_item("hello")
```


### ```pyb.UGFX()```
returns context

### ```context.area(x, y, width, height, color)```

### ```pyb.Button(x, y, width, height, text)```

### ```pyb.Keyboard(x, y, width, height, ???)```

### ```pyb.Container(x, y, width, height, ???)```
returns container

### ```container.show()```

### ```container.hide()```

### ```pyb.List(x, y, width, height, ???, container)```
returns list

### ```list.add_item(text)```

## WiFi

```python
import network
import socket
import pyb
nic = network.CC3100()
nic.connect("<<SSID>>","<<PASSWORD>>")
while (not nic.is_connected()):
    nic.update()
    pyb.delay(100)
    s=socket.socket()
    s.connect(socket.getaddrinfo('bowerham.net',80)[0][4])
    s.send("GET /hello_badge HTTP/1.1\r\nHost:bowerham.net\r\n\r\n")
    s.recv(1500)
```
