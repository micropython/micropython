Example Description

This example describes how to implement high/low level trigger on 1 gpio pin.

Pin name PC_4 and PC_5 map to GPIOC_4 and GPIOC_5:
Connect PC_4 and PC_5
 - PC_4 as gpio input high/low level trigger.
 - PC_5 as gpio output

In this example, PC_5 is signal source that change level to high and low periodically.

PC_4 setup to listen low level events in initial.
When PC_4 catch low level events, it disable the irq to avoid receiving duplicate events.
(NOTE: the level events will keep invoked if level keeps in same level)

Then PC_4 is configured to listen high level events and enable irq.
As PC_4 catches high level events, it changes back to listen low level events.

Thus PC_4 can handle both high/low level events.

In this example, you will see log that prints high/low level event periodically.
