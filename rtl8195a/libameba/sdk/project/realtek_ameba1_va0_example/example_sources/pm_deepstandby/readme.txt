Example Description

This example describes how to use deep standby api.

Requirement Components:
    a LED
    a push button

Pin name PA_5 and PC_5 map to GPIOA_5 and GPIOC_5:
 - PA_5 as input, connect a push button to this pin and 3v3.
 - PC_5 as output, connect a LED to this pin and ground.

In this example, LED is turned on after device initialize.
User push the button to turn off LED and trigger device enter deep standby mode for 10s.
If user press button before sleep timeout, the system will resume.
LED is turned on again after device initialize.

It can be easily measure power consumption in normal mode and deep standby mode before/after push the putton.
