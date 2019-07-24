NOTE:
There is an ongoing bug with MCUXpresso IDE - In Debug mode, the wifi initialization fails
for the aws demo projects if program runs immediately after debugger.
See an example thread here: https://community.nxp.com/thread/472217

Until the bug is fixed, the workaround is to press the debugger restart button once after the
debugger reaches breakpoint 'main'. This will reset the program and then you can continue the debug/run normally. 
