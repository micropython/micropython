import pyb

if not hasattr(pyb, "Accel"):
    print("SKIP")
    raise SystemExit

accel = pyb.Accel()
print(accel)
accel.x()
accel.y()
accel.z()
accel.tilt()
accel.filtered_xyz()
