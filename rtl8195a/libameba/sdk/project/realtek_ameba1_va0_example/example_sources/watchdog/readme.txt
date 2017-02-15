Example Description

This example describes how to use watchdog api.

Requirement Components: None

In this example, watchdog is setup to 5s timeout.
Watchdog won't bark if we refresh it before timeout.
The timer is also reloaded after refresh.
Otherwise it will restart system in default or call callback function if registered.
