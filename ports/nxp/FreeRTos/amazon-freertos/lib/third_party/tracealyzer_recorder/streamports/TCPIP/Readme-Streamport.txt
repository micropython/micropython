Tracealyzer Stream Port for TCP/IP (lwIP example)
-------------------------------------------------

This directory contains a "stream port" for the Tracealyzer recorder library,
i.e., the specific code needed to use a particular interface for streaming a
Tracealyzer RTOS trace. The stream port is defined by a set of macros in
trcStreamingPort.h, found in the "include" directory.

This particular stream port targets TCP/IP. This example assumes lwIP but is
easy to modify for other TCP/IP stacks.

To use this stream port, make sure that include/trcStreamingPort.h is found
by the compiler (i.e., add this folder to your project's include paths) and
add all included source files to your build. Make sure no other versions of
trcStreamingPort.h are included by mistake!

Note that lwIP is not included, but assumed to exist in the project already.

See also http://percepio.com/2016/10/05/rtos-tracing.

Percepio AB
www.percepio.com