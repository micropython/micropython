import pyb

accel = pyb.Accel()
print(accel)
accel.x()
accel.y()
accel.z()
accel.tilt()
accel.filtered_xyz()
