import PIL.Image

i = PIL.Image.new("RGB", (32, 32))

for y in range(32):
    z = y * 255 // 31
    p = (z, z, z)
    for x in range(0, 8):
        i.putpixel((x, y), p)
    p = (0, 0, z)
    for x in range(8, 12):
        i.putpixel((x, y), p)
    p = (0, z, z)
    for x in range(12, 16):
        i.putpixel((x, y), p)
    p = (0, z, 0)
    for x in range(16, 20):
        i.putpixel((x, y), p)
    p = (z, z, 0)
    for x in range(20, 24):
        i.putpixel((x, y), p)
    p = (z, 0, 0)
    for x in range(24, 28):
        i.putpixel((x, y), p)
    p = (z, 0, z)
    for x in range(28, 32):
        i.putpixel((x, y), p)
i = i.resize((128, 128), PIL.Image.NEAREST)
i.save("testpattern.jpg", "JPEG", quality=90)
