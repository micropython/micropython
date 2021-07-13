import requests
import zipfile
import os.path
import shutil
import sys
import subprocess

for chip in ["samd21", "samd51"]:
    r = None
    filename = chip + ".zip"
    if not os.path.isfile(filename):
        with open("tools/" + chip + ".json", "r") as project_json:
            headers = {"content-type": "text/plain"}
            r = requests.post(
                "http://start.atmel.com/api/v1/generate/?format=atzip&compilers=[make]&file_name_base=My%20Project",
                headers=headers,
                data=project_json,
            )
        if not r.ok:
            # Double check that the JSON is minified. If its not, you'll get a 404.
            print(r.text)
            sys.exit(1)
        with open(filename, "wb") as out:
            out.write(r.content)

    # Extract to a temporary location and patch it before replacing the existing location.
    z = zipfile.ZipFile(filename)
    tmp_dir = "asf4/" + chip + "_vanilla"
    z.extractall(tmp_dir)

    # Remove all carriage returns.
    for dirpath, dirnames, filenames in os.walk(tmp_dir):
        for fn in filenames:
            fn = os.path.join(dirpath, fn)
            subprocess.run(["sed", "-i", "s/\r//g", fn])

    # Move files to match SAMD51 structure.
    if chip == "samd21":
        shutil.move("asf4/samd21_vanilla/samd21a/include", "asf4/samd21_vanilla")
        shutil.move("asf4/samd21_vanilla/samd21a/gcc/gcc", "asf4/samd21_vanilla/gcc")
        shutil.move("asf4/samd21_vanilla/samd21a/gcc/system_samd21.c", "asf4/samd21_vanilla/gcc")

    ok = True
    for patch in os.listdir("asf4/patches/" + chip):
        patch = "patches/" + chip + "/" + patch
        print(patch)
        result = subprocess.run(
            ["patch", "-l", "-F", "10", "-u", "-p", "1", "-d", tmp_dir, "-i", "../" + patch]
        )
        ok = ok and result.returncode == 0
        print()

    if ok:
        real_dir = "asf4/" + chip
        shutil.rmtree(real_dir)
        shutil.move(tmp_dir, real_dir)
        # delete the zip on success
        os.remove(filename)
    else:
        print("A patch failed!")
