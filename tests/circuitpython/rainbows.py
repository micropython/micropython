import rainbowio

for i in range(0, 256, 15):
    print("{:3} {:06x} {:06x}".format(i, rainbowio.colorwheel(i), rainbowio.colorwheel(float(i))))
for i in range(256, 1024, 128):
    print("{:3} {:06x}".format(i, rainbowio.colorwheel(i)))
