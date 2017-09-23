import requests
import zipfile
import os.path
import sys

for chip in ["samd21", "samd51"]:
    r = None
    filename = chip + ".zip"
    if not os.path.isfile(filename):
        with open("tools/" + chip + ".json", "r") as project_json:
            headers = {"content-type": "text/plain"}
            r = requests.post("http://start.atmel.com/api/v1/generate/?format=atzip&compilers=[make]&file_name_base=My%20Project", headers=headers, data=project_json)
        if not r.ok:
            # Double check that the JSON is minified. If its not, you'll get a 404.
            print(r.text)
            sys.exit(1)
        with open(filename, "wb") as out:
            out.write(r.content)
    z = zipfile.ZipFile(filename)
    z.extractall("asf4/" + chip)
    print(z)

    # delete the zip on success
    #os.remove(filename)

# Remove LITTLE_ENDIAN define from ASF because it conflicts with GCC.
for fn in os.listdir("asf4/samd51/include"):
    contents = []
    path = "asf4/samd51/include/" + fn
    if not os.path.isfile(path):
        continue
    with open(path, "r") as f:
        for line in f:
            if line.startswith("#define LITTLE_ENDIAN"):
                contents.append("//" + line)
            else:
                contents.append(line)
    with open(path, "w") as f:
        for line in contents:
            f.write(line)

# Replace ASF's assert with asf_assert so it doesn't conflict with the C macro.

for chip in ["samd21", "samd51"]:
    contents = []
    path = "asf4/" + chip + "/hal/utils/include/utils_assert.h"
    with open(path, "r") as f:
        for line in f:
            contents.append(line.replace(" assert(", " asf_assert("))
    with open(path, "w") as f:
        for line in contents:
            f.write(line)

# samd21/samd21a/include is copied to samd21/include to match samd51
