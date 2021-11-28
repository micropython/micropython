import aesio
from binascii import hexlify, unhexlify

# doc example
key = b"Sixteen byte key"
inp = b"CircuitPython!!!"  # Note: 16-bytes long
outp = bytearray(len(inp))
cipher = aesio.AES(key, aesio.MODE_ECB)
cipher.encrypt_into(inp, outp)
print(str(hexlify(outp), ""))

cipher = aesio.AES(key, aesio.MODE_ECB)
cipher.decrypt_into(outp, outp)
print(str(outp, ""))
print()

print("ECB")
# ECB mode test vector, from the aes.c source
plaintext = unhexlify(
    "6bc1bee22e409f96e93d7e117393172a"
    "ae2d8a571e03ac9c9eb76fac45af8e51"
    "30c81c46a35ce411e5fbc1191a0a52ef"
    "f69f2445df4f9b17ad2b417be66c3710"
)

key = unhexlify("2b7e151628aed2a6abf7158809cf4f3c")

cyphertext = bytearray(len(plaintext))
cipher = aesio.AES(key, aesio.MODE_ECB)
for i in range(0, len(plaintext), 16):
    output = memoryview(cyphertext)[i : i + 16]
    cipher.encrypt_into(plaintext[i : i + 16], output)
    print(str(hexlify(output), ""))
print()

plaintext = bytearray(len(plaintext))
cipher = aesio.AES(key, aesio.MODE_ECB)
for i in range(0, len(plaintext), 16):
    output = memoryview(plaintext)[i : i + 16]
    cipher.decrypt_into(cyphertext[i : i + 16], output)
    print(str(hexlify(output), ""))
print()

print("CBC")
# CBC128-AES128 test vector from NIST Special Publication 800-38A, 2001 edition, p50

plaintext = unhexlify(
    "6bc1bee22e409f96e93d7e117393172a"
    "ae2d8a571e03ac9c9eb76fac45af8e51"
    "30c81c46a35ce411e5fbc1191a0a52ef"
    "f69f2445df4f9b17ad2b417be66c3710"
)

key = unhexlify("2b7e151628aed2a6abf7158809cf4f3c")
iv = unhexlify("000102030405060708090a0b0c0d0e0f")
cyphertext = bytearray(len(plaintext))
cipher = aesio.AES(key, aesio.MODE_CBC, IV=iv)
for i in range(0, len(plaintext), 16):
    output = memoryview(cyphertext)[i : i + 16]
    cipher.encrypt_into(plaintext[i : i + 16], output)
    print(str(hexlify(output), ""))
print()

plaintext = bytearray(len(plaintext))
cipher = aesio.AES(key, aesio.MODE_CBC, IV=iv)
for i in range(0, len(plaintext), 16):
    output = memoryview(plaintext)[i : i + 16]
    cipher.decrypt_into(cyphertext[i : i + 16], output)
    print(str(hexlify(output), ""))
print()


print("CTR")
# CTR128-AES128 test vector from NIST Special Publication 800-38A, 2001 edition, p55

plaintext = unhexlify(
    "6bc1bee22e409f96e93d7e117393172a"
    "ae2d8a571e03ac9c9eb76fac45af8e51"
    "30c81c46a35ce411e5fbc1191a0a52ef"
    "f69f2445df4f9b17ad2b417be66c3710"
)

key = unhexlify("2b7e151628aed2a6abf7158809cf4f3c")
counter = unhexlify("f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff")
cyphertext = bytearray(len(plaintext))
cipher = aesio.AES(key, aesio.MODE_CTR, IV=counter)
for i in range(0, len(plaintext), 16):
    output = memoryview(cyphertext)[i : i + 16]
    cipher.encrypt_into(plaintext[i : i + 16], output)
    print(str(hexlify(output), ""))
print()

plaintext = bytearray(len(plaintext))
cipher = aesio.AES(key, aesio.MODE_CTR, IV=counter)
for i in range(0, len(plaintext), 16):
    output = memoryview(plaintext)[i : i + 16]
    cipher.decrypt_into(cyphertext[i : i + 16], output)
    print(str(hexlify(output), ""))
print()
