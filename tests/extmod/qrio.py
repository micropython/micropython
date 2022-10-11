try:
    import qrio
except:
    print("SKIP")
    raise SystemExit

loc = __file__.rsplit("/", 1)[0]
with open(f"{loc}/data/qr.pgm", "rb") as f:
    content = f.read()[-320 * 240 :]

decoder = qrio.QRDecoder(320, 240)
for r in decoder.decode(content):
    print(r)
