import os
import sys
import pyhy

kp = pyhy.hydro_sign_keygen()

if len(sys.argv) > 1:
    path = sys.argv[1]
else:
    path = "."

secret_key = os.path.join(path, "key")
public_key = os.path.join(path, "key.pub")

print("Writing secret key to", secret_key)
with open(secret_key, "wb") as keyfile:
    keyfile.write(bytearray(kp.sk))

print("Writing public key to", public_key)
with open(public_key, "wb") as keyfile:
    keyfile.write(bytearray(kp.pk))
