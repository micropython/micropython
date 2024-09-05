import os
import math
import struct
import pygame

# distance between two points
def pointdistance(veca, vecb):
    return math.sqrt((veca[0] - vecb[0])**2 + (veca[1] - vecb[1])**2 + (veca[2] - vecb[2])**2)

# load obj
def loadobj(filename, flipwinding):
    vertices = []
    fixedvertices = []
    uvs = []
    fixeduvs = []
    fixeduvs2 = []
    shapes = []
    shapenames = []
    bounds = []
    faceindices = []
    normals = []
    lastvertexstart = 0
    currentvertex = 0
    materialfilename = ''
    with open(filename, "r") as file:
        for line in file:
            if line.startswith('usemtl '):
                shapes.append(lastvertexstart)
                shapes.append(currentvertex - 1)
                shapenames.append(line.strip().split()[1])
                lastvertexstart = currentvertex
            elif line.startswith('v '):
                vertices.append(list(map(float, line.strip().split()[1:])))
                currentvertex += 1
            elif line.startswith('vt '):
                uvs.append(list(map(float, line.strip().split()[1:])))
                fixeduvs.append(list(map(float, line.strip().split()[1:])))
            elif line.startswith('mtllib '):
                materialfilename = line.strip().split(" ", 1)[1]
            elif line.startswith('vn '):
                normals.append(list(map(float, line.strip().split()[1:])))
            elif line.startswith('f '):
                mapindices = list(map(lambda indexset: indexset.strip().split('/'), line.strip().split(" ")[1:]))
                for ind in mapindices:
                    for element in ind:
                        faceindices.append(int(element))
        # compute sphere bounds
        for shapei in range(0, len(shapes), 2):
            x = 0; y = 0; z = 0;
            for i in range(shapes[shapei + 0], shapes[shapei + 1], 1):
                x += vertices[i][0]
                y += vertices[i][1]
                z += vertices[i][2]
            numverts = shapes[shapei + 1] - shapes[shapei + 0]
            xave = x / numverts
            yave = y / numverts
            zave = z / numverts
            maxradius = 0
            for i in range(shapes[shapei + 0], shapes[shapei + 1], 1):
                x = vertices[i][0]
                y = vertices[i][1]
                z = vertices[i][2]
                radius = pointdistance([xave, yave, zave], [x, y, z])
                if(radius > maxradius):
                    maxradius = radius
            bounds.append(xave)
            bounds.append(yave)
            bounds.append(zave)
            bounds.append(maxradius)
        # fixup uv order to match uv order
        for shapei in range(0, len(shapes), 2):
            indicesperface = 3
            for i in range(shapes[shapei + 0]*indicesperface, shapes[shapei + 1]*indicesperface, indicesperface * 3):
                globaloffset = 0
                vert0 = faceindices[i + 0] + globaloffset
                uv0 = faceindices[i + 1] + globaloffset
                n0 = faceindices[i + 2] + globaloffset
                vert1 = faceindices[i + 3] + globaloffset
                uv1 = faceindices[i + 4] + globaloffset
                n1 = faceindices[i + 5] + globaloffset
                vert2 = faceindices[i + 6] + globaloffset
                uv2 = faceindices[i + 7] + globaloffset
                n2 = faceindices[i + 8] + globaloffset
                fixeduvs[vert0 - 1] = uvs[uv0 - 1]
                fixeduvs[vert1 - 1] = uvs[uv1 - 1]
                fixeduvs[vert2 - 1] = uvs[uv2 - 1]
        if(flipwinding):
            for i in range(0, len(vertices), 3):
                v0 = vertices[i + 0]
                v1 = vertices[i + 1]
                v2 = vertices[i + 2]
                vertices[i + 0] = v2
                vertices[i + 1] = v1
                vertices[i + 2] = v0
                uv0 = fixeduvs[i + 0]
                uv1 = fixeduvs[i + 1]
                uv2 = fixeduvs[i + 2]
                fixeduvs[i + 0] = uv2
                fixeduvs[i + 1] = uv1
                fixeduvs[i + 2] = uv0
        # fix vertex order so a rasterizer can access in linear order
        indexoffset = 0
        for i in range(0, len(vertices), 3):
            globaloffset = -1
            vert0 = faceindices[indexoffset + 0] + globaloffset
            uv0 = faceindices[indexoffset + 1] + globaloffset
            n0 = faceindices[indexoffset + 2] + globaloffset
            vert1 = faceindices[indexoffset + 3] + globaloffset
            uv1 = faceindices[indexoffset + 4] + globaloffset
            n1 = faceindices[indexoffset + 5] + globaloffset
            vert2 = faceindices[indexoffset + 6] + globaloffset
            uv2 = faceindices[indexoffset + 7] + globaloffset
            n2 = faceindices[indexoffset + 8] + globaloffset
            fixedvertices.append(vertices[vert0])
            fixedvertices.append(vertices[vert1])
            fixedvertices.append(vertices[vert2])
            fixeduvs2.append(fixeduvs[vert0])
            fixeduvs2.append(fixeduvs[vert1])
            fixeduvs2.append(fixeduvs[vert2])
            indexoffset += 9

    return fixedvertices, fixeduvs2, shapes, shapenames, materialfilename, bounds, normals
	
def loadmat(filename):
    materials = []
    with open(filename, "r") as file:
        hasmaterial = False
        lastmaterial = 'nullptr'
        for line in file:
            if line.startswith('newmtl '):
                if hasmaterial == False:
                    materials.append(lastmaterial)
                hasmaterial = False
                materials.append(line.strip().split(" ", 1)[1])
            elif line.startswith('map_Kd '):
                lastmaterial = line.strip().split(" ", 1)[1]
                if "\\" in lastmaterial:
                    lastmaterial = lastmaterial.strip().split("\\")[-1]
                materials.append(lastmaterial)
                hasmaterial = True
    return materials
	
def loadtexture(filename):
    data = []
    image = pygame.image.load(filename)
    width = image.get_width()
    height = image.get_height()
    for y in range(height):
        for x in range(width):
            pixelvalue = image.get_at([x, y])
            data.append(pixelvalue.r)
            data.append(pixelvalue.g)
            data.append(pixelvalue.b)
    return width, height, data

def Main():
    # Static data
    #dirlist = [".\\BBB_Room\\"]
    #filenamelist = ["bbb_room"]
    #dirlist = [".\\Peachs_Castle_Exterior\\"]
    #filenamelist = ["Peaches_Castle"]
    dirlist = [".\\dragon\\"]
    filenamelist = ["dragon"]
    flipwindinglist = [False]

    # Individual Meshes
    for filei in range(len(dirlist)):
        dir = dirlist[filei]
        filename = filenamelist[filei]
        flipwinding = flipwindinglist[filei]
        ProcessFile(dir, filename, flipwinding)
        
def ConvertTo565(color888):
    r = float(color888[0]) / 255.0
    g = float(color888[1]) / 255.0
    b = float(color888[2]) / 255.0
    rscaled = int(r * 31.0)
    gscaled = int(g * 63.0)
    bscaled = int(b * 31.0)
    color565 = (rscaled << 11) | (gscaled << 5) | (bscaled)
    return color565

def ProcessFile(dir, filename, flipwinding):
    if True:
        #names for the generated variables
        objext = '.obj'

        # TODO separate global materials file, need to combine textures across all meshes and write out at end of processing
        # TODO perform id fixup on shape materials
        f = open(filename + "_global_mat" + ".bin", 'wb')

        texturefilenames = []
        for file in os.listdir(dir):
            if file.endswith('.png'):
                texturefilenames.append(file)

        # TODO these are not unique outside this directory
        # create texture metadata and data
        globalbyteoffset = 0
        compacteddata = []
        f.write(struct.pack("I", len(texturefilenames)))
        materialmap = {}
        materialidx = 0
        for texturefilename in texturefilenames:
            width, height, data = loadtexture(dir + texturefilename)
            compacteddata = compacteddata + data
            f.write(struct.pack("I", width))
            f.write(struct.pack("I", height))
            f.write(struct.pack("I", globalbyteoffset))
            materialmap[texturefilename] = [materialidx, width, height, globalbyteoffset]
            materialidx = materialidx + 1
            globalbyteoffset += width * height * 2
            
        # write total size of all textures
        f.write(struct.pack("I", globalbyteoffset))
        # write all material data at once
        for i in range(0, len(compacteddata), 3):
            color888 = [compacteddata[i + 0], compacteddata[i + 1], compacteddata[i + 2]]
            color565 = ConvertTo565(color888)
            f.write(struct.pack("H", color565))
        f.close()
        
        print(materialmap)
        
        f = open(filename + ".bin", 'wb')
        # load object and materials from directory
        # shapes are in the form [... firtsvertex_i, lastvertex_i, shapename_i ...]
        vertices, uvs, shapes, shapenames, materialfilename, bounds, normals = loadobj(dir + filename + objext, flipwinding)

        # load material file 
        # materials are in the form [... shapename_i, texturename_i ...]
        materials = loadmat(dir + materialfilename)

        print(shapenames)

        # for each shape store the pointer to texture to use
        # the variable name of the pointer is created from the texture name
        for i in range(0, len(shapenames), 1):
            if(shapenames[i] in materials):
                shapenames[i] = materials[materials.index(shapenames[i]) + 1]
            else:
                print('error: missing shape material for shape: ' + shapenames[i])

        # write all shape verts together
        f.write(struct.pack("I", len(vertices)))
        for i in range(0, len(vertices), 1):
            x = vertices[i][0]            
            ba = bytearray(struct.pack("f", x))  
            f.write(ba)
            y = vertices[i][1]
            ba = bytearray(struct.pack("f", y))  
            f.write(ba)
            z = vertices[i][2]
            ba = bytearray(struct.pack("f", z))  
            f.write(ba)
            u = uvs[i][0]
            ba = bytearray(struct.pack("f", u))  
            f.write(ba)
            v = uvs[i][1]
            ba = bytearray(struct.pack("f", v))  
            f.write(ba)
            
        # write shape and material information     
        f.write(struct.pack("I", len(shapes)//2))
        for i in range(0, len(shapes), 2):
            start = shapes[i + 0]
            end = shapes[i + 1]
            f.write(struct.pack("I", end))
            texturename = shapenames[i//2]
            if(texturename == 'nullptr'):
                f.write(struct.pack("i", -1))
            else:
                # remove the texture file extension
                #texturename = texturename.strip().split(".")[0]
                print(texturename)
                textureid = materialmap[texturename][0]
                f.write(struct.pack("I", textureid))

        f.close()
        
        
Main()