# Renode

Renode is an emulator targeting microcontroller-class devices. This port is a
minimal version of CircuitPython that runs under Renode. Renode is designed to
mimic full microcontrollers but CP uses more peripherals than what Renode has
implemented so far. This port allows us to run on a variety of CPUs without
worrying about peripherals.

## Running

1. Get Renode: https://renode.io/#downloads
2. `cd ports/renode`
3. `make BOARD=renode_cortex_m0plus`
4. In another tab: `tio /tmp/cp-uart`
5. `renode`
6. In renode: `include @renode.resc`
7. <Any other setup>
8. `start`
9. `pause`
10. `quit`

Step 4 sets up `tio` to talk to CircuitPython via UART <-> PTY bridge.

## Other stuff

### Emulator logging
Renode modules have debug logging that can be enabled with `logLevel` with an int
between `-1` for `NOISY` and `3` for errors only.

### GDB

Renode can provide a GDB server. It is very useful for precisely controlling the
emulator's execution.

```
machine StartGdbServer 3333 true
```

### Execution profiling

In renode do `cpu EnableProfiler CollapsedStack $ORIGIN/profile.folded` before starting
the emulation. You can view it using [Speedscope](https://www.speedscope.app/). CircuitPython calls
a lot of functions and may overwhelm speedscope. You can enable this tracing over a specific
section of CircuitPython execution to limit the capture size.

[Related Renode Docs](https://renode.readthedocs.io/en/latest/advanced/execution-tracing.html)

### Execution tracing
If you want to see every instruction run you can do: `cpu CreateExecutionTracing "tracer_name" $ORIGIN/instruction_trace.txt Disassembly`.
