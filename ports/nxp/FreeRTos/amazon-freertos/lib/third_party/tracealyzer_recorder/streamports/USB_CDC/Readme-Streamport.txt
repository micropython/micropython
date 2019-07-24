Tracealyzer Stream Port for USB CDC (STM32 example)
---------------------------------------------------

This directory contains a "stream port" for the Tracealyzer recorder library,
i.e., the specific code needed to use a particular interface for streaming a
Tracealyzer RTOS trace. The stream port is defined by a set of macros in
trcStreamingPort.h, found in the "include" directory.

This particular stream port targets USB CDC. This is an example for the STM32 USB
stack (from the STM32CubeMX code generation tool, v1.4.1) and has been tested on
a STM32F767ZI device on a Nucleo board. See this blog post:

http://percepio.com/2017/02/03/usb-trace-streaming-st-nucleo-f767zi-board/

However, it should be straight-forward to modify this for other USB stacks.

To use this stream port, make sure that include/trcStreamingPort.h is found
by the compiler (i.e., add this folder to your project's include paths) and
add all included source files to your build. Make sure no other versions of
trcStreamingPort.h are included by mistake!

Note that the USB stack not included, but assumed to exist in the project already.

See also http://percepio.com/2016/10/05/rtos-tracing.

Percepio AB
www.percepio.com