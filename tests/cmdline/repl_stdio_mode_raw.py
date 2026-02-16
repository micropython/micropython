# Test micropython.stdio_mode_raw terminal mode switching.
# Must be a single statement: the REPL's own readline switches terminal modes
# between prompts, which would clobber the saved state between separate lines.
# fmt: off
import termios, micropython; c=termios.tcgetattr(0); micropython.stdio_mode_raw(True); r=termios.tcgetattr(0); micropython.stdio_mode_raw(False); d=termios.tcgetattr(0); print(c[3]!=0, r[3]==0, r[0]!=c[0], d[3]==c[3], d[0]==c[0])
