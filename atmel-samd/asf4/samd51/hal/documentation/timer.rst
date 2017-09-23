============================
The Timer driver (bare-bone)
============================

The Timer driver provides means for delayed and periodical function invocation.

A timer task is a piece of code (function) executed at a specific time or periodically by the timer after the task has
been added to the timers task queue. The execution delay or period is set in ticks, where one tick is defined as a
configurable number of clock cycles in the hardware timer. Changing the number of clock cycles in a tick automatically
changes execution delays and periods for all tasks in the timers task queue.

A task has two operation modes, single-shot or repeating mode. In single-shot mode the task is removed from the task queue
and then is executed once, in repeating mode the task reschedules itself automatically after it has executed based on
the period set in the task configuration.
In single-shot mode a task is removed from the task queue before its callback is invoked. It allows an application to
reuse the memory of expired task in the callback.

Each instance of the Timer driver supports infinite amount of timer tasks, only limited by the amount of RAM available.

Features
--------
* Initialization and de-initialization
* Starting and stopping
* Timer tasks - periodical invocation of functions
* Changing and obtaining of the period of a timer

Applications
------------
* Delayed and periodical function execution for middle-ware stacks and applications.

Dependencies
------------
* Each instance of the driver requires separate hardware timer capable of generating periodic interrupt.

Concurrency
-----------
The Timer driver is an interrupt driven driver.This means that the interrupt that triggers a task may occur during
the process of adding or removing a task via the driver's API. In such case the interrupt processing is postponed
until the task adding or removing is complete.

The task queue is not protected from the access by interrupts not used by the driver. Due to this
it is not recommended to add or remove a task from such interrupts: in case if a higher priority interrupt supersedes
the driver's interrupt, adding or removing a task may cause unpredictable behavior of the driver.

Limitations
-----------
* The driver is designed to work outside of an operating system environment, the task queue is therefore processed in interrupt context which may delay execution of other interrupts.
* If there are a lot of frequently called interrupts with the priority higher than the driver's one, it may cause delay for triggering of a task.

Knows issues and workarounds
----------------------------
Not applicable
