# Notes
The folder "config-validation-rules" are used to store board specific validation rules.
The folder name can't be changed.
The files in this folder can't be deleted.

## ota_image_generator.py
This program is used to generate the OTA descriptor and insert it at the beginning of the given image.

## factory_image_generator.py
This program provide the same functionality of ota_descriptor_generator, in addition, it will sign the
modified image and create a signature. After the signature is created, it will append the signature type,
signature size and the signature at the end of the modified image. 


# Steps to run 
## 1. Install python 
Install python 3 or higher  
[Python Download](https://www.python.org/downloads/)  
Check if you can run command: `python --version`, it should output 3.x.x

#### how to change the default python version to python3.x on MAC

If you get python version 3.x or above by running the command

    python --version
    
you can skip this section. Otherwise, you can follow those steps.


1 . run command 

    which python3
 
   we can get the path of python3  (we call it "path/of/python3" here)
 
2 . run command
    
    ls -l path/of/python3
    
   we can get the link path of python3  (we call it "link/path/of/python3" here)


3 . run command 

    which python

   we can get the path of python  (we call it "path/of/python" here)


4 . run command
 
    ln -sf link/path/of/python3  path/of/python
 

We also need to set the default package manager to pip3 

1 . run command 

    which pip3
 
   we can get the path of pip3  (we call it "path/of/pip3" here)
   
2 . run command
    
    ls -l path/of/pip3
    
   we can get the link path of pip3  (we call it "link/path/of/pip3" here)

3 . run command 

    which pip

   we can get the path of pip  (we call it "path/of/pip" here)
   
4 . run command
 
    ln -sf link/path/of/pip3  path/of/pip



## 2.Install pyOpenSSL
    pip install pyopenssl

## 3.Run the tool
### ota_image_generator.py
usage: 
    
    python ota_image_generator.py [-h] -b binary_path -p hardware_platform

example usages:

get help:
    
    python ota_image_generator.py -h

use folder/inputImage.bin and MCHP-Curiosity-PIC32MZEF as parameter :
		
	python ota_image_generator.py -b folder/inputImage.bin -p MCHP-Curiosity-PIC32MZEF
	
	
### factory_image_generator.py
usage: python factory_image_generator.py [-h] -b binary_path -p hardware_platform -k private_key_path -x bootloaderhex

example usages:

get help:

	python factory_image_generator.py -h
		
use inputImage.bin, MCHP-Curiosity-PIC32MZEF, private_key.pem & aws.bootloader.X.hex as parameter :


    python factory_image_generator.py -b inputImage.bin -p MCHP-Curiosity-PIC32MZEF -k private_key.pem -x aws.bootloader.X.hex
	