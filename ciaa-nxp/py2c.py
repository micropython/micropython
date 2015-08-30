print("py2c 1.0")

print("Reading python script...")
file = open("./py/Main.py", "r")
content = file.read()
file.close()

print("Generating data array...")
output="const char programScript[] = {"
for byte in content:
	output+=hex(ord(byte))+"," 	
output+="0x00};"


print("Creating output file...")
file = open("./ProgramScript.c", "w")
file.write(output)
file.close()

print(output)

print("Succesfull Operation.")

