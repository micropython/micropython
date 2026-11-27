# uefi.Display: GOP surfaced as a framebuf.FrameBuffer (modes + draw + show). GOP is
# optional, so SKIP where the firmware publishes none; run with a display adapter (even
# headless) to exercise it.
try:
    import uefi
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    disp = uefi.Display()
except uefi.Error:
    print("SKIP")
    raise SystemExit

modes = disp.modes()
print(len(modes) >= 1)
disp.fb.fill(0)
disp.fb.ellipse(disp.width // 2, disp.height // 2, 40, 40, 0xFFFF)
disp.fb.line(0, 0, disp.width - 1, disp.height - 1, 0xF800)
disp.fb.text("UEFI", 8, 8, 0x07E0)
disp.show()
print("display ok")
