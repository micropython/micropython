Example Description

This example describes how to disable JTAG module and use GPIO pin to blink led.

Requirement Components:
    a LED
    a push button

PC_4 as input with internal pull-high, connect a push button to this pin and ground.
If button is not pressed while device boot up, then jtag module is turned off.
If button is pressed while device boot up, then we don't turn off jtag module.

PE_0 as output, connect a LED to this pin and ground.
If jatg module is turned off, then we blink led.
