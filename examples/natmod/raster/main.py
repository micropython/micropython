import os
import sys
import machine

import engine_main
import engine
import engine_io
import engine_draw
#import random
#import time
import math
import array
import sys
#import framebuf
import gc
#import rp2
import struct

sys.path.append("/Games/ThumbyRaster")

import thumbyraster

machine.freq(300 * 1000 * 1000)

width = 128
height = 128

depthbuffer = array.array('f', range(int(width * height)))

cameraposition = array.array('f', range(int(3)))
viewmatrix = array.array('f', range(int(4 * 4)))
projectionmatrix = array.array('f', range(int(4 * 4)))
meshverts = array.array('I')
vertices = array.array('f')
shapematerialmap = array.array('I')
globalmaterialmap = array.array('I')
globalmaterialdata = array.array('H')

def LoadMesh(filename):
    global meshverts
    global vertices
    global shapematerialmap
    file = open(filename, "rb")
    print(file)
    # all mesh data is dword sized
    # load num verts
    
    #meshverts.fromfile(file, 1)
    meshverts.append(int.from_bytes(file.read(4), "little"))
    print(meshverts[0])
    
    # load vertices with position + uv
    #vertices.fromfile(file, meshverts[0])
    for i in range(0, meshverts[0] * 5, 1):
        vertelement = file.read(4)
        unpackedverts = struct.unpack('<f', vertelement)
        vertices.append(unpackedverts[0])
    #print(vertices)
    
    # load number of shapes
    # numshapes.fromfile(file, 1)
    numshapes = array.array('I')
    numshapes.append(int.from_bytes(file.read(4), "little"))
    print(numshapes[0])
    
    # load shape materials
    for i in range(0, numshapes[0] * 2, 1):
        shapeelem = file.read(4)
        unpackedelem = struct.unpack('<I', shapeelem)
        shapematerialmap.append(unpackedelem[0])
    #print(shapematerialmap)

    file.close()
    
def LoadMaterials(filename):
    global globalmaterialmap
    global globalmaterialdata
    file = open(filename, "rb")
    print(file)
    # load number of materials
    nummaterials = array.array('I')
    #nummaterials.fromfile(file, 1)
    nummaterials.append(int.from_bytes(file.read(4), "little"))
    print(nummaterials[0])

    for i in range(0, nummaterials[0] * 3, 1):
        matelem = file.read(4)
        unpackedelem = struct.unpack('<I', matelem)
        globalmaterialmap.append(unpackedelem[0])
    #print(globalmaterialmap)

    totalpixels = array.array('I')
    #totalpixels.fromfile(file, 1)
    totalpixels.append(int.from_bytes(file.read(4), "little"))
    print(totalpixels[0])
    
    gc.collect()
    
    globalmaterialdata = array.array('H', range(int(totalpixels[0])))
    
    for i in range(0, totalpixels[0]//2, 1):
        colorelem = file.read(2)
        unpackedelem = struct.unpack('<H', colorelem)
        globalmaterialdata[i] = unpackedelem[0]
    #print(globalmaterialdata)

    file.close()

def Main():
    global cameraposition
    global viewmatrix
    global projectionmatrix
    global meshverts
    global vertices
    global shapematerialmap
    global globalmaterialmap
    global globalmaterialdata
    
    cameraposition[0] = -0.0
    cameraposition[1] = -5.0
    cameraposition[2] = 30.0
    
    near = 0.01
    far = 10.0
    angleOfView = 90.0
    fov = 1.0 / math.tan(angleOfView * 0.5 * math.pi / 180.0)
    aspectratio = 1.0
    projmat = array.array('f',
        [aspectratio * fov, 0.0, 0.0, 0.0,
        0.0, fov, 0.0, 0.0,
        0.0, 0.0, far / (far - near), (-far * near) / (far - near),
        0.0, 0.0, 1.0, 0.0])
    viewmat = array.array('f',
        [1.0, 0.0, 0.0, cameraposition[0],
        0.0, 1.0, 0.0, cameraposition[1],
        0.0, 0.0, 1.0, cameraposition[2],
        0.0, 0.0, 0.0, 1.0])
    
    #meshfilename = "Games/ThumbyRaster/Peaches_Castle.bin"
    #meshfilename = "Games/ThumbyRaster/bbb_room.bin"
    meshfilename = "dragon.bin"
    LoadMesh(meshfilename)
    gc.collect()
    #materialfilename = "Games/ThumbyRaster/Peaches_Castle_global_mat.bin"
    #materialfilename = "Games/ThumbyRaster/bbb_room_global_mat.bin"
    materialfilename = "dragon_global_mat.bin"
    LoadMaterials(materialfilename)
    
    numverts = int(meshverts[0])
    while(True):
        if engine.tick():
            
            movementscale = 0.3
            if engine_io.LEFT.is_pressed:
                cameraposition[0] -= movementscale
            if engine_io.RIGHT.is_pressed:
                cameraposition[0] += movementscale
            if engine_io.DOWN.is_pressed:
                cameraposition[2] += movementscale
            if engine_io.UP.is_pressed:
                cameraposition[2] -= movementscale
            if engine_io.A.is_pressed:
                cameraposition[1] -= movementscale
            if engine_io.B.is_pressed:
                cameraposition[1] += movementscale
                
            viewmat = array.array('f',
                [1.0, 0.0, 0.0, cameraposition[0],
                0.0, 1.0, 0.0, cameraposition[1],
                0.0, 0.0, 1.0, cameraposition[2],
                0.0, 0.0, 0.0, 1.0])
                
            colorbuffer = engine_draw.back_fb_data()
            thumbyraster.drawtriangles(
                colorbuffer,
                depthbuffer,
                cameraposition,
                viewmat,
                projmat,
                numverts,
                vertices,
                shapematerialmap,
                globalmaterialmap,
                globalmaterialdata)
    
Main()



