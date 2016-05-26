from utime import sleep_ms
from OneButton import OneButton


def click(pin):
  print('click %s' % pin.name)

def dclick(pin):
  print('dclick')

def startLong(pin):
  print('startLong')

def stopLong(pin):
  print('stopLong')

def duringLong(pin):
  print('duringLong')

def startLoop():
  d3 = OneButton(0, True)
  d3.name = 'D3'

  d4 = OneButton(2, True)
  d4.name = 'D4'

  d3.attachClick(click)
  d4.attachClick(click)
  d3.attachDoubleClick(dclick)
  d3.attachLongPressStart(startLong)
  d3.attachDuringLongPress(duringLong)
  d3.attachLongPressStop(stopLong)

  while (True):
      d3.tick()
      d4.tick()
      sleep_ms(20)

startLoop()
