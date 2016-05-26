# Library for detecting button clicks, doubleclicks and long press pattern on a single button.
# Copyright (c) by Matthias Hertel, http://www.mathertel.de
# This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
# More information on: http://www.mathertel.de/Arduino
# -----
# 02.10.2010 created by Matthias Hertel
# 21.04.2011 transformed into a library
# 01.12.2011 include file changed to work with the Arduino 1.0 environment
# 23.03.2014 Enhanced long press functionalities by adding longPressStart and longPressStop callbacks
# 21.09.2015 A simple way for debounce detection added.
# 21.05.2016 Ported to micropython by Thomas Gfüllner

from machine import Pin
from utime import ticks_ms, ticks_diff

class OneButton:

  def __init__(self, pinNr, activeLow, externalResistor=False):
    if externalResistor:
      self._pin = Pin(pinNr, Pin.IN) 
    else:
      self._pin = Pin(pinNr, Pin.IN, Pin.PULL_UP if activeLow else Pin.PULL_DOWN) 

    # number of millisec that have to pass by before a click is detected.
    self._clickTicks = 200

    # number of millisec that have to pass by before a long button press is detected.
    self._pressTicks = 1000

    self._debounceTicks = 50 # number of ticks for debounce times.
 
    # starting with state 0: waiting for button to be pressed
    self._state = 0
    self._isLongPressed = False # Keep track of long press state

    if (activeLow):
      # button connects ground to the pin when pressed.
      self._buttonReleased = 1  # notPressed
      self._buttonPressed = 0
    else:
      # button connects VCC to the pin when pressed.
      self._buttonReleased = 0
      self._buttonPressed = 1


    self._clickFunc = None
    self._doubleClickFunc = None
    self._longPressStartFunc = None
    self._longPressStopFunc = None
    self._duringLongPressFunc = None

    self._startTime = None

  
  # ----- Set runtime parameters -----

  # set number of millisec after single click is assumed.
  def setClickTicks(self, ticks):
    self._clickTicks = ticks

  # set number of millisec after press is assumed.
  def setPressTicks(self, ticks):
    self._pressTicks = ticks

  # attach functions that will be called when button was pressed in the specified way.
  def attachClick(self, newFunction):
    self._clickFunc = newFunction
  def attachDoubleClick(self, newFunction):
    self._doubleClickFunc = newFunction
  def attachLongPressStart(self, newFunction):
    self._longPressStartFunc = newFunction
  def attachLongPressStop(self, newFunction):
    self._longPressStopFunc = newFunction
  def attachDuringLongPress(self, newFunction):
    self._duringLongPressFunc = newFunction

  # ----- State machine functions -----

  def isLongPressed(self):
    return self._isLongPressed

  # call this function every some milliseconds for handling button events.
  def tick(self):
    # Detect the input information 
    buttonLevel = self._pin.value()
    now = ticks_ms()

    # Implementation of the state machine
    if self._state == 0: # waiting for menu pin being pressed.
      if (buttonLevel == self._buttonPressed):
        self._state = 1
        self._startTime = now # remember starting time

    elif self._state == 1: # waiting for menu pin being released.
      if (buttonLevel == self._buttonReleased) and (ticks_diff(self._startTime,now) < self._debounceTicks):
        # button was released to quickly so I assume some debouncing.
        # go back to state 0 without calling a function.
        self._state = 0

      elif buttonLevel == self._buttonReleased:
        self._state = 2

      elif (buttonLevel == self._buttonPressed) and (ticks_diff(self._startTime,now) > self._pressTicks):
        self._isLongPressed = True;  # Keep track of long press state
        if self._longPressStartFunc:
           self._longPressStartFunc(self)
        if self._duringLongPressFunc:
           self._duringLongPressFunc(self)
        self._state = 6
        
      else:
        # wait. Stay in this state.
        pass

    elif self._state == 2:
      # waiting for menu pin being pressed the second time or timeout.
      if ticks_diff(self._startTime, now) > self._clickTicks:
        # this was only a single short click
        if self._clickFunc:
          self._clickFunc(self)
        self._state = 0

      elif buttonLevel == self._buttonPressed:
        self._state = 3

    elif self._state == 3:
      # waiting for menu pin being released finally.
      if buttonLevel == self._buttonReleased:
        # this was a 2 click sequence.
        if self._doubleClickFunc:
          self._doubleClickFunc(self)
        self._state = 0

    elif self._state == 6:
      # waiting for menu pin being release after long press.
      if buttonLevel == self._buttonReleased:
        self._isLongPressed = False
        if self._longPressStopFunc:
          self._longPressStopFunc(self)
        self._state = 0
      else:
        # button is being long pressed
        self._isLongPressed = True
        if self._duringLongPressFunc:
          self._duringLongPressFunc(self)
  
