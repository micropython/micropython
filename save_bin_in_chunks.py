import gc
import binascii
import os, shutil, sys

chunk_size = 4096
UPSTREAM_VERSION = sys.argv[1]
PLATFORM = sys.argv[2]
try: multiple = int(sys.argv[3])
except: multiple = 1

def read_in_4kmult_chunks(file_object, folder, multiple=1):
	with open(folder+'/multiple', 'w') as multiple_f:
		multiple_f.write(str(multiple))
	while True:
		data = file_object.read(multiple*chunk_size)
		if not data:
			break
		yield data

cwd = os.getcwd()
folder = cwd+'/ota/'+PLATFORM

for filename in os.listdir(folder):
    file_path = os.path.join(folder, filename)
    try:
        if os.path.isfile(file_path) or os.path.islink(file_path):
            os.unlink(file_path)
        elif os.path.isdir(file_path):
            shutil.rmtree(file_path)
    except Exception as e:
        print('Failed to delete %s. Reason: %s' % (file_path, e))

shutil.copy2('ports/'+PLATFORM.split('_')[0].lower()+'/build-'+PLATFORM+'/firmware.bin', 'release/'+PLATFORM+'.bin')

with open('ports/'+PLATFORM.split('_')[0].lower()+'/build-'+PLATFORM+'/micropython.bin', 'rb') as bin_file:
	i = 1
	for piece in read_in_4kmult_chunks(bin_file, folder, multiple):
		temp = binascii.b2a_base64(piece).decode()
		with open(folder+'/chunk-'+str(i), 'w') as chunk_file:
			chunk_file.write(temp)
		del temp
		i += 1
		gc.collect()
	with open(folder+'/last_chunk', 'w') as last_chunk_f:
		last_chunk_f.write(str(i-1))

with open(folder+'/version', 'w') as version_f:
	version_f.write(UPSTREAM_VERSION)
