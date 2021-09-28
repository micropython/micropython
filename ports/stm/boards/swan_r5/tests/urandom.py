import os


def main():
    print("Random number test")
    r = os.urandom(32)
    print(f"urandom TRNG string is {r}")


main()
