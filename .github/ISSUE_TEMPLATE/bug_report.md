---
name: Bug report
about: Create a report to help us improve
title: ''
labels: 'bug'
assignees: ''

---

<!-- Thanks! for testing out CircuitPython. Now that you have got a problem...
you can file a bug report for it. Feel free to modify the below format to better
suit your issue. :) -->

**Firmware**

<!-- Include the version of CircuitPython you're running. You can see it in the
`boot_out.txt` file, as well as in the REPL. -->

```python
Adafruit CircuitPython 6.2.0-beta.2 on 2021-03-01; Raspberry Pi Pico with rp2040
```

**Code/REPL**

<!-- Include your code that reproduces the bug here. Try to distill down to the
minimum possible to reproduce. -->

```python
import busio, bitbangio
i2c = bitbangio.I2C(board.GP1, board.GP0)
```

**Behavior**

<!-- What happens when you run the code above? Include any error messages. -->

```python
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TimeoutError: Clock stretch too long
```

**Description**

<!-- Optionally, describe the issue in more detail. Here are some examples: -->

- Error while using i2c...
- Only happens when...
- might be related to #4291...

**Additional Info**

<!-- Optionally, add any other information like hardware connection, scope output etc.
If you have already done some debugging, mention it here. -->

Removing [this](url) line resolves the issue.
