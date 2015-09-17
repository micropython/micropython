import sys

print("py2c 1.0")

if len(sys.argv) != 3:
	print("Usage %s <infile.py> <outfile.c>" % sys.argv[0]);
else:
	with open(sys.argv[2], 'w') as fout:
		fout.write("const char programScript[] = {")
		with open(sys.argv[1], 'rb') as fin:
			fout.write(','.join([hex(ord(x)) for x in fin.read()]));
		fout.write(",0x00};")
	print("%s created" % sys.argv[2]);
