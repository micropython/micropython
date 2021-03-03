---
name: Bug report
about: Create a report to help us improve
title: ''
labels: ''
assignees: ''

---

Thanks! for testing out CircuitPython. Now that you have got a problem... you can
file a bug report for it. Feel free to modify the below format to better suit your issue.
Also, don't forget to remove me... :)

**Firmware**

```python
Adafruit CircuitPython 6.2.0-beta.2 on 2021-03-01; Raspberry Pi Pico with rp2040
```
**Code/REPL**

```python
import busio, bitbangio
i2c = bitbangio.I2C(board.GP1, board.GP0)
```
**Behavior**

```python
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TimeoutError: Clock stretch too long
```
**Description** _(optional)_

- Error while using i2c...
- Only happens when...
- might be related to #4291...

**Additional Info** _(optional)_

Any other information like hardware connection, scope output etc.
