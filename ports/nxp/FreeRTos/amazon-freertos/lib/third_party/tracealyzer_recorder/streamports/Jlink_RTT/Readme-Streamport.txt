Tracealyzer Stream Port for SEGGER J-Link
-----------------------------------------

This directory contains a "stream port" for the Tracealyzer recorder library,
i.e., the specific code needed to use a particular interface for streaming a
Tracealyzer RTOS trace. The stream port is defined by a set of macros in
trcStreamingPort.h, found in the "include" directory.

This particular stream port targets SEGGER J-Link debug probes, using the RTT
interface provided by SEGGER.

To use this stream port, make sure that include/trcStreamingPort.h is found
by the compiler (i.e., add this folder to your project's include paths) and
add all included source files to your build. Make sure no other versions of
trcStreamingPort.h are included by mistake!

Note that this stream port also contains SEGGER's RTT driver.

See also http://percepio.com/2016/10/05/rtos-tracing.

Percepio AB
www.percepio.com