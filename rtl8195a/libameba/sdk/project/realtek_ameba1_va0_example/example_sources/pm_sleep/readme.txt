Example Description

This example describes how to use sleep api.

Requirement Components:
    a LED
    a push button

Pin name PC_4 and PC_5 map to GPIOC_4 and GPIOC_5:
 - PC_4 as input with internal pull-high, connect a push button to this pin and ground.
 - PC_5 as output, connect a LED to this pin and ground.

In this example, LED is turned on after device initialize.
User push the button to turn off LED and trigger device enter sleep mode for 10s.
If user push button before sleep timeout, the system will resume.
LED is turned on again after system resume without restart PC.

It can be easily measure power consumption in normal mode and sleep mode before/after push the putton.
