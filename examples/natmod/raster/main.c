
#include "py/dynruntime.h"
#include <string.h>
#include <math.h>

#define RP2 1

#define hardwarewidth 128;
#define hardwareheight 128
#define maximumwidth 128
#define maximumheight 128
#define width 128
#define height 128

float depthbuffer[maximumwidth * maximumheight];
unsigned short colorbuffer[maximumwidth * maximumheight];



// when creating verts
// header
// num verts, num materials
	// vertex buffer
	// material id array start end geo

	

// header, num materials
// global material assets
	// material id array
	// texture dimension array
	// texture offset array
	// texture array







void raytracescene(
	unsigned short* colorbuffer,
	unsigned short* scratchbuffer,
	float* alights,
	int numlights,
	float* aspheres,
	int numspheres,
	float* atriangles,
	int numtriangles,
	float* aplanes,
	int numplanes,
	float* renderproperties);

static mp_obj_t tracescenegeneric(
	mp_obj_fun_bc_t* self, 
	size_t n_args, 
	size_t n_kw, 
	mp_obj_t* args) {
    // Check number of arguments is valid
    //mp_arg_check_num(n_args, n_kw, 4, 4, false);
    // Extract buffer pointer and verify typecode
	int argindex = 0;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[argindex++], &bufinfo, MP_BUFFER_RW);
	mp_buffer_info_t scratchbufinfo;
    mp_get_buffer_raise(args[argindex++], &scratchbufinfo, MP_BUFFER_RW);
	mp_buffer_info_t lightbufinfo;
	mp_get_buffer_raise(args[argindex++], &lightbufinfo, MP_BUFFER_RW);
	int numlights = mp_obj_get_int(args[argindex++]);
	mp_buffer_info_t spherebufinfo;
	mp_get_buffer_raise(args[argindex++], &spherebufinfo, MP_BUFFER_RW);
	int numspheres = mp_obj_get_int(args[argindex++]);
	mp_buffer_info_t aabbbufinfo;
	mp_get_buffer_raise(args[argindex++], &aabbbufinfo, MP_BUFFER_RW);
	int numaabbs = mp_obj_get_int(args[argindex++]);
	mp_buffer_info_t planebufinfo;
	mp_get_buffer_raise(args[argindex++], &planebufinfo, MP_BUFFER_RW);
	int numplanes = mp_obj_get_int(args[argindex++]);

    //if (bufinfo.typecode != 'B') {
        //mp_raise_ValueError(MP_ERROR_TEXT("expecting byte array"));
    //}
    // Compute product, store result back in first element of array
    unsigned short* colorddata = (unsigned short*)bufinfo.buf;
	unsigned short* scratchdata = (unsigned short*)scratchbufinfo.buf;
	float* lightdata = (float*)lightbufinfo.buf;
	float* spheredata = (float*)spherebufinfo.buf;
	float* aabbdata = (float*)aabbbufinfo.buf;
	float* planes = (float*)planebufinfo.buf;
	
    raytracescene(
		colorddata,
		scratchdata,
		lightdata,
		numlights,
		spheredata,
		numspheres,
		aabbdata,
		numaabbs,
		planes,
		numplanes,
		NULL);

    return mp_const_none;
}

// This is the entry point and is called when the module is imported
mp_obj_t mpy_init(mp_obj_fun_bc_t* self, size_t n_args, size_t n_kw, mp_obj_t* args) {
    // This must be first, it sets up the globals dict and other things
    MP_DYNRUNTIME_INIT_ENTRY

    // The tracescene function uses the most general C argument interface	
	mp_store_global(MP_QSTR_tracescenegeneric, MP_DYNRUNTIME_MAKE_FUNCTION(tracescenegeneric));

    // This must be last, it restores the globals dict
    MP_DYNRUNTIME_INIT_EXIT
}

float invsqrtf(float n) {
    n = 1.0f / n;
    long i;
    float x = 0.0f;
	float y = 0.0f;
 
    x = n * 0.5f;
    y = n;
    memcpy(&i, &y, sizeof(float));
    i = 0x5f3759df - (i >> 1);
	memcpy(&y, &i, sizeof(float));
    y = y * (1.5f - (x * y * y));
 
    return y;
}

float sqrtf(float a) {
	#if 1
	return 1.0f / invsqrtf(a);
	#else
	float x = a;
	float y = 1;
	float e = 0.000001f;
	while(x-y > e)
	{
		x = (x + y)/2.0f;
		y = a/x;
	}
	return x;
	#endif
}

typedef struct {
    float x;
    float y;
    float z;
} vec3;
vec3 vecadd(vec3 a, vec3 b) {
    vec3 out;
    out.x = a.x + b.x;
    out.y = a.y + b.y;
    out.z = a.z + b.z;
    return out;
}
vec3 vecsub(vec3 a, vec3 b) {
    vec3 out;
    out.x = a.x - b.x;
    out.y = a.y - b.y;
    out.z = a.z - b.z;
    return out;
}
vec3 vecmul(vec3 a, vec3 b) {
    vec3 out;
    out.x = a.x * b.x;
    out.y = a.y * b.y;
    out.z = a.z * b.z;
    return out;
}
vec3 vecmuls(vec3 a, b) {
    vec3 out;
    out.x = a.x * b;
    out.y = a.y * b;
    out.z = a.z * b;
    return out;
}
vec3 vecneg(vec3 a) {
    vec3 out;
    out = vecmuls(a, -1.0f);
    return out;
}
vec3 vecdiv(vec3 a, vec3 b) {
    vec3 out;
    out.x = a.x / b.x;
    out.y = a.y / b.y;
    out.z = a.z / b.z;
    return out;
}
vec3 vecdivs(vec3 a, float b) {
    vec3 out;
    out.x = a.x / b;
    out.y = a.y / b;
    out.z = a.z / b;
    return out;
}
float dot(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
float mag(vec3 a) {
    return sqrtf(dot(a, a));
}
vec3 vec3(float inx, float iny, float inz) {
    vec3 out;
    out.x = inx;
    out.y = iny;
    out.z = inz;
    return out;
}
vec3 getnormalized(vec3 a) {
    float vectormag = mag(a);
    if (vectormag == 0.0f) {
        return vec3(0.0f, 0.0f, 0.0f);
    }
    else {
        return vecdivs(a, vectormag);
    }
}
vec3 vecrcp(vec3 a) {
	return vec3(1.0f/a.x, 1.0f/a.y, 1.0f/a.z);
}
vec3 vecmin(vec3 a, vec3 b) {
	return vec3(fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z));
}
vec3 vecmax(vec3 a, vec3 b) {
	return vec3(fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z));
}

typedef struct {
	float x;
	float y;
} vec2;
vec2 vec2(float inx, float iny) {
    vec2 out;
    out.x = inx;
    out.y = iny;
    return out;
}




void clearscenedepth() {
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            depthbuffer[x * height + y] = 100.0f;
            colorbuffer[x * height + y] = 0x0;
        }
    }
}
inline unsigned short sampletextureuv(Texture texture, vec2 uv) {
    // source textures are y flipped
    uv.y = 1.0f - uv.y;
    int addrx = (int)(uv.x * (float)texture.width);
    int addry = (int)(uv.y * (float)texture.height);
    if (addrx < 0) {
        addrx = addrx + texture.width * (1 - addrx / texture.width);
    }
    if (addry < 0) {
        addry = addry + texture.height * (1 - addry / texture.height);
    }
    addrx %= texture.width;
    addry %= texture.height;
    int linearaddress = addry * texture.width + addrx;
    if (linearaddress < 0) {
        linearaddress = 0;
    }
    unsigned short rgb = 0x0;
    if (texture.data) {
        rgb = texture.data[linearaddress]
    }
    return rgb;
}
inline void drawscandepthuv(
    int y, int x1, int x2,
    float z1, float z2,
    vec2 uva, vec2 uvb,
    float wa, float wb,
    bool alpha) {
    if (x1 > x2) {
        int temp = x1;
        x1 = x2;
        x2 = temp;
        vec2 tempuv = uva;
        uva = uvb;
        uvb = tempuv;
        float tempz = z1;
        z1 = z2;
        z2 = tempz;
        float tempw = wa;
        wa = wb;
        wb = tempw;
    }
    int x1c = clamp(x1, 0, width - 1);
    int x2c = clamp(x2, 0, width - 1);
    float totalwidth = (float)x2 - (float)x1;
    if (totalwidth <= 0.0f) {
        return;
    }
    for (int x = x1c; x <= x2c; ++x) {
        float t = ((float)x - (float)x1) / totalwidth;
        float z = z1 * ((float)1.0f - t) + z2 * t;
        float w = wa * ((float)1.0f - t) + wb * t;
        if (w == 0.0f) {
            continue;
        }
        if (depthbuffer[x * height + y] > z / w) {
            // TODO flexible shader implementation should go here
            // Or write attributes to gbuffer
            vec2 uv = uva * (1.0f - t) + uvb * t;
            unsigned short rgb565 = sampletextureuv(boundtexture, uv / w);
            if (rgb565 == 0xffff) {
                continue;
            }
            if (!(x >= 0 && x < width && y >= 0 && y < height)) {
                continue;
            }
			depthbuffer[x * height + y] = z / w;
			colorbuffer[x * height + y + 0] = rgb565;
#if RP2
#else
            Uint8 r, g, b, a;
            SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
            SDL_RenderDrawPoint(renderer, x, y);
#endif
        }
    }
}

// draw from top to bottom 
inline void fillBottomFlatTriangleuv(TriangleUVFP tri, bool alpha) {
	vec3 v1 = tri.a;
	vec3 v2 = tri.b;
	vec3 v3 = tri.c;
	vec3 uv1 = tri.auv;
	vec3 uv2 = tri.buv;
	vec3 uv3 = tri.cuv;
	float w1 = tri.aw;
	float w2 = tri.bw;
	float w3 = tri.cw;

	if (v2.y - v1.y == 0.0f) {
		return;
	}
	if (v3.y - v1.y == 0.0f) {
		return;
	}

	float invslope1 = (v2.x - v1.x) / (v2.y - v1.y);
	float invslope2 = (v3.x - v1.x) / (v3.y - v1.y);

	float invdepthslope1 = (v2.z - v1.z) / (v2.y - v1.y);
	float invdepthslope2 = (v3.z - v1.z) / (v3.y - v1.y);

	vec2 invuvslope1 = (uv2 - uv1) / (v2.y - v1.y);
	vec2 invuvslope2 = (uv3 - uv1) / (v3.y - v1.y);

	float invwslope1 = (w2 - w1) / (v2.y - v1.y);
	float invwslope2 = (w3 - w1) / (v3.y - v1.y);

	float curx1 = v1.x;
	float curx2 = v1.x;

	float curz1 = v1.z;
	float curz2 = v1.z;

	vec2 curuv1 = uv1;
	vec2 curuv2 = uv1;

	float curw1 = w1;
	float curw2 = w1;

	int minx = min(min((int)v1.x, (int)v2.x), (int)v3.x);
	int maxx = max(max((int)v1.x, (int)v2.x), (int)v3.x);

	v1.y = (int)clamp((int)v1.y, 0, (int)height);
	v2.y = (int)clamp((int)v2.y, 0, (int)height);

	for (int scanlineY = (int)v1.y; scanlineY <= (int)v2.y; scanlineY++)
	{
		curx1 = clamp(curx1, (float)minx, (float)maxx);
		curx2 = clamp(curx2, (float)minx, (float)maxx);
		drawscandepthuv(scanlineY, (int)curx1, (int)curx2, curz1, curz2, curuv1, curuv2, curw1, curw2, alpha);
		curx1 += invslope1;
		curx2 += invslope2;
		curz1 += invdepthslope1;
		curz2 += invdepthslope2;
		curuv1 += invuvslope1;
		curuv2 += invuvslope2;
		curw1 += invwslope1;
		curw2 += invwslope2;
	}
}
// draw from bottom to top 
inline void fillTopFlatTriangleuv(TriangleUVFP tri, bool alpha) {
	vec3 v1 = tri.a;
	vec3 v2 = tri.b;
	vec3 v3 = tri.c;
	vec2 uv1 = tri.auv;
	vec2 uv2 = tri.buv;
	vec2 uv3 = tri.cuv;
	float w1 = tri.aw;
	float w2 = tri.bw;
	float w3 = tri.cw;

	if (v3.y - v1.y == 0.0f) {
		return;
	}
	if (v3.y - v2.y == 0.0f) {
		return;
	}

	float invslope1 = (v3.x - v1.x) / (v3.y - v1.y);
	float invslope2 = (v3.x - v2.x) / (v3.y - v2.y);

	float invdepthslope1 = (v3.z - v1.z) / (v3.y - v1.y);
	float invdepthslope2 = (v3.z - v2.z) / (v3.y - v2.y);

	vec2 invuvslope1 = (uv3 - uv1) / (v3.y - v1.y);
	vec2 invuvslope2 = (uv3 - uv2) / (v3.y - v2.y);

	float invwslope1 = (w3 - w1) / (v3.y - v1.y);
	float invwslope2 = (w3 - w2) / (v3.y - v2.y);

	float curx1 = v3.x;
	float curx2 = v3.x;

	float curz1 = v3.z;
	float curz2 = v3.z;

	vec2 curuv1 = uv3;
	vec2 curuv2 = uv3;

	float curw1 = w3;
	float curw2 = w3;

	int minx = min(min((int)v1.x, (int)v2.x), (int)v3.x);
	int maxx = max(max((int)v1.x, (int)v2.x), (int)v3.x);

	for (int scanlineY = (int)v3.y; scanlineY > (int)v1.y; scanlineY--)
	{
		curx1 = clamp(curx1, (float)minx, (float)maxx);
		curx2 = clamp(curx2, (float)minx, (float)maxx);
		drawscandepthuv(corenum, scanlineY, (int)curx1, (int)curx2, curz1, curz2, curuv1, curuv2, curw1, curw2, alpha);
		curx1 -= invslope1;
		curx2 -= invslope2;
		curz1 -= invdepthslope1;
		curz2 -= invdepthslope2;
		curuv1 -= invuvslope1;
		curuv2 -= invuvslope2;
		curw1 -= invwslope1;
		curw2 -= invwslope2;
	}
}


// fill a triangle using the standard fille algorithm filling top and bottom triangles
// convert to Brensenham for interger math speedup
// http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
inline void rastertriangleuv(TriangleUVFP tri, bool alpha) {
	// sort verts in y order where the highest position y will be vetexa
	tri.sortascending();
	vec3 a = tri.a;
	vec3 b = tri.b;
	vec3 c = tri.c;
	vec2 auv = tri.auv;
	vec2 buv = tri.buv;
	vec2 cuv = tri.cuv;
	float aw = tri.aw;
	float bw = tri.bw;
	float cw = tri.cw;

	// fill each row 
	/* here we know that v1.y <= v2.y <= v3.y */
	/* check for trivial case of bottom-flat triangle */
	if (tri.b.y == tri.c.y)
	{
		fillBottomFlatTriangleuv(corenum, tri, alpha);
	}
	/* check for trivial case of top-flat triangle */
	else if (tri.a.y == tri.b.y)
	{
		fillTopFlatTriangleuv(corenum, tri, alpha);
	}
	else
	{
		float ylerp = (b.y - a.y) / (c.y - a.y);
		float depthinterp = (a.z + ylerp * (c.z - a.z));
		/* general case - split the triangle in a topflat and bottom-flat one */
		vec3 d = vec3(
			a.x + ylerp * (c.x - a.x),
			b.y,
			depthinterp);

		vec2 duv = vec2(
			tri.auv.x * (1.0f - ylerp) + tri.cuv.x * ylerp,
			tri.auv.y * (1.0f - ylerp) + tri.cuv.y * ylerp);

		float dw = tri.aw * (1.0f - ylerp) + tri.cw * ylerp;
		TriangleUVFP tria = TriangleUVFP(a, b, d, auv, buv, duv, aw, bw, dw);
		fillBottomFlatTriangleuv(tria, alpha);
		TriangleUVFP trib = TriangleUVFP(b, d, c, buv, duv, cuv, bw, dw, cw);
		fillTopFlatTriangleuv(trib, alpha);
	}
}

// Sutherland Hodgman clipping algorithm
// there are other algorithms with less comparison that could be used instead
// this algorithm can work with polygons not just triangles
#define MAX_CLIPPED_VERTS 16 // probably not actually the actual minimal max triangles that can be generated
// Passed verts initialized in camera space
inline void triangleclipuv(
	vec3* passedverts, 
	vec2* passeduvs, 
	float* passedws, 
	int* numpassedverts, 
	bool alpha) {
#define NUM_PLANES 5
	const vec3 planepoints[NUM_PLANES] = {
		vec3(0.0f, 0.0f, near), // near
		vec3(0.0f, 0.0f, 0.0f), // bottom
		vec3(1.0f, 1.0f, 0.0f), // top
		vec3(0.0f, 0.0f, 0.0f), // left
		vec3(1.0f, 1.0f, 0.0f), // right
	};
	const vec3 planenormals[NUM_PLANES] = {
		vec3(0.0f, 0.0f, 1.0f), // near
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, -1.0f, 0.0f),
		vec3(1.0f, 0.0f, 0.0f),
		vec3(-1.0f, 0.0f, 0.0f),
	};

	vec3 outputpassedvertsarray[MAX_CLIPPED_VERTS];
	vec2 outputpasseduvsarray[MAX_CLIPPED_VERTS];
	float outputpassedwsarray[MAX_CLIPPED_VERTS];
	vec3* outputpassedverts = outputpassedvertsarray;
	vec2* outputpasseduvs = outputpasseduvsarray;
	float* outputpassedws = outputpassedwsarray;
	// This should be done in clip space but is split between projection and camera space for now
	// clip the near plane in camera space to simplify
	{
		int numoutpassedverts = numpassedverts;
		// reset passed verts for the clipping loops
		numpassedverts = 0;
		for (int vertexi = 0; vertexi < numoutpassedverts; ++vertexi) {
			Edge3UVFP visitededge = Edge3UVFP(
				passedverts[vertexi],
				passedverts[(vertexi + 1) % numoutpassedverts],
				passeduvs[vertexi],
				passeduvs[(vertexi + 1) % numoutpassedverts],
				1.0f,  // wa
				1.0f); // wb
			ClipCode clipcode = NONE;
			Edge3UVFP clippededge = clipplanecodeduvfp(visitededge, planepoints[0], planenormals[0], clipcode);
			// returns a clip code that determines which verts if any should pass the ith clip plane
			switch (clipcode) {
			case NONE: // edge outside the plane, add no verts
				break;
			case AB:   // edge entirely inside the frustum, add only first vertex
					   // as the second vertex of the edge will be added in the next or previous iterration
				outputpasseduvs[numpassedverts] = clippededge.uva;
				outputpassedverts[numpassedverts++] = clippededge.a;
				break;
			case A: // edge starting inside, add both clipped verts
				outputpasseduvs[numpassedverts] = clippededge.uva;
				outputpassedverts[numpassedverts++] = clippededge.a;
				outputpasseduvs[numpassedverts] = clippededge.uvb;
				outputpassedverts[numpassedverts++] = clippededge.b;
				break;
			case B: // edge starting outside, record the vert on the plane
					// the other vert will be added in another iterration
				outputpasseduvs[numpassedverts] = clippededge.uvb;
				outputpassedverts[numpassedverts++] = clippededge.b;
				break;
			}
		}
		swap(passedverts, outputpassedverts);
		swap(passeduvs, outputpasseduvs);
	}
	if (numpassedverts <= 0) {
		return;
	}

	bool hasoutside = false;
	bool hasinside = false;
	// transform the remaining triangles to clip space
	// project to 0 - 1 screen space to allow easy reject or accet
	// by looking at fixed point fraction bits only
	for (int vertexi = 0; vertexi < numpassedverts; ++vertexi) {
		vec4 vecproj = projmatfp * vec4(passedverts[vertexi], 1.0f);
		vec3 vecscreen = vecproj.xyz() / vecproj.w;
		float x0 = (1.0f - (vecscreen.x + 1.0f) * 0.5f);
		float y0 = (1.0f - (vecscreen.y + 1.0f) * 0.5f);
		vecscreen.x = x0;
		vecscreen.y = y0;
		passedverts[vertexi] = vecscreen;
		passeduvs[vertexi] /= vecproj.w;
		passedws[vertexi] = 1.0f / vecproj.w;
		// check only integer bits for any point outside the 0..1 clip which requires more clip testing
		if (((vecscreen.x & 0xffff8000) != 0) ||
			((vecscreen.y & 0xffff8000) != 0)) {
			hasoutside = true;
		}
		// check for any point inside the 0..1 clip with fraction bits 
		// 
		if (((vecscreen.x & 0x0000ffff) != 0) ||
			((vecscreen.y & 0x0000ffff) != 0)) {
			hasinside = true;
		}
	}

	// area of 2d triangle in screen space after perspective divide
	// if 2 triangles are created from the near clip they should shader the same handedness
	float sign = useccw ? 1.0f : -1.0f;
	float area = (passedverts[0].x - passedverts[1].x) * (passedverts[0].y - passedverts[2].y) -
		(passedverts[0].x - passedverts[2].x) * (passedverts[0].y - passedverts[1].y);

	// Backface culling
	if ((sign * area < 0.0f) && !alpha) {
		//numpassedverts = 0;
		//return;
	}

	// fast path trivial accept
	if (!hasoutside) {
		swap(passedverts, outputpassedverts);
		swap(passeduvs, outputpasseduvs);
		swap(passedws, outputpassedws);
		arraycopy(passedverts, outputpassedverts, numpassedverts);
		arraycopy(passeduvs, outputpasseduvs, numpassedverts);
		arraycopy(passedws, outputpassedws, numpassedverts);
		// transform the remaining triangles to screen space
		for (int vertexi = 0; vertexi < numpassedverts; ++vertexi) {
			vec3 vecscreen = passedverts[vertexi];
			//float x0 = (1.0f - (vecscreen.x + 1.0f) * 0.5f);
			//float y0 = (1.0f - (vecscreen.y + 1.0f) * 0.5f);
			float x0 = vecscreen.x * (float)width;
			float y0 = vecscreen.y * (float)height;
			vecscreen.x = x0;
			vecscreen.y = y0;
			passedverts[vertexi] = vecscreen;
		}
		return;
	}
	// fast path trivial reject
	if (!hasinside) {
		numpassedverts = 0;
		return;
	}

	// clip against 4 screen sides
	for (int planei = 1; planei < NUM_PLANES; ++planei) {
		int numoutpassedverts = numpassedverts;
		// reset passed verts for the clipping loops
		numpassedverts = 0;
		// for each pair of verts in order, check if they cross the axis of the plane
		// if neither vertex is inside the axis leave both verts from the list
		// if both verts are inside append both when they are the first vert in an edge
		// if only one vert is inside and another is outside append the intersected vert
		// when visiting some edges up to 2 verts are added which adds triangles to the the pass list
		// it is possible for many triangles to be generated from one initial triangle by 
		// clipping between 1 and 5 total planes
		for (int vertexi = 0; vertexi < numoutpassedverts; ++vertexi) {
			Edge3UVFP visitededge = Edge3UVFP(
				passedverts[vertexi],
				passedverts[(vertexi + 1) % numoutpassedverts],
				passeduvs[vertexi],
				passeduvs[(vertexi + 1) % numoutpassedverts],
				passedws[vertexi],
				passedws[(vertexi + 1) % numoutpassedverts]);
			ClipCode clipcode = NONE;
			Edge3UVFP clippededge = clipplanecodeduvfp(visitededge, planepoints[planei], planenormals[planei], clipcode);
			// returns a clip code that determines which verts if any should pass the ith clip plane
			switch (clipcode) {
			case NONE: // edge outside the plane, add no verts
				break;
			case AB:   // edge entirely inside the frustum, add only first vertex
					   // as the second vertex of the edge will be added in the next or previous iterration
				outputpassedws[numpassedverts] = clippededge.wa;
				outputpasseduvs[numpassedverts] = clippededge.uva;
				outputpassedverts[numpassedverts++] = clippededge.a;
				break;
			case A: // edge starting inside, add both clipped verts
				outputpassedws[numpassedverts] = clippededge.wa;
				outputpasseduvs[numpassedverts] = clippededge.uva;
				outputpassedverts[numpassedverts++] = clippededge.a;
				outputpassedws[numpassedverts] = clippededge.wb;
				outputpasseduvs[numpassedverts] = clippededge.uvb;
				outputpassedverts[numpassedverts++] = clippededge.b;
				break;
			case B: // edge starting outside, record the vert on the plane
					// the other vert will be added in another iterration
				outputpassedws[numpassedverts] = clippededge.wb;
				outputpasseduvs[numpassedverts] = clippededge.uvb;
				outputpassedverts[numpassedverts++] = clippededge.b;
				break;
			}
		}
		if (numpassedverts <= 0) {
			return;
		}
		swap(passedverts, outputpassedverts);
		swap(passeduvs, outputpasseduvs);
		swap(passedws, outputpassedws);
	}
	swap(passedverts, outputpassedverts);
	swap(passeduvs, outputpasseduvs);
	swap(passedws, outputpassedws);
	arraycopy(passedverts, outputpassedverts, numpassedverts);
	arraycopy(passeduvs, outputpasseduvs, numpassedverts);
	arraycopy(passedws, outputpassedws, numpassedverts);
	// transform the remaining triangles to screen space
	for (int vertexi = 0; vertexi < numpassedverts; ++vertexi) {
		vec3 vecscreen = passedverts[vertexi];
		//float x0 = (1.0f - (vecscreen.x + 1.0f) * 0.5f);
		//float y0 = (1.0f - (vecscreen.y + 1.0f) * 0.5f);
		float x0 = vecscreen.x * (float)width;
		float y0 = vecscreen.y * (float)height;
		vecscreen.x = x0;
		vecscreen.y = y0;
		passedverts[vertexi] = vecscreen;
	}
}

inline void filltrianglesuv(
	mat44 cameraMat, 
	mat44 projMat, 
	bool alphapass) {
#if RP2
#else
	// Set draw color to black
	SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
#endif
	int materialindex = 0;

	int numtriangles = numverts / 3;
	int trianglesplit = numtriangles / 2;
	int vertstartindex = 0;
	int jobverts = 0;
	if (corenum == 0) {
		vertstartindex = 0;
		jobverts = trianglesplit * 3;
		boundtexture0 = Texture(
			textures[materialindex],
			texturedims[materialindex].x,
			texturedims[materialindex].y);
	}
	else {
		vertstartindex = trianglesplit * 3;
		jobverts = numverts;
		for (int i = 0; i < numshapes; ++i) {
			if (vertstartindex <= shapes[i].x) {
				materialindex = i - 1;
				boundtexture1 = Texture(
					textures[materialindex],
					texturedims[materialindex].x,
					texturedims[materialindex].y);
				break;
			}
		}
	}

	for (int vertind = vertstartindex; vertind < jobverts; vertind += 3) {
		if (vertind > shapes[materialindex].y) {
			materialindex++;
			if (corenum == 0) {
				boundtexture0 = Texture(
					textures[materialindex],
					texturedims[materialindex].x,
					texturedims[materialindex].y);
			}
			else {
				boundtexture1 = Texture(
					textures[materialindex],
					texturedims[materialindex].x,
					texturedims[materialindex].y);
			}
		}

		vec3 playerposfp = vec3(playerpos.x, playerpos.y, playerpos.z);

		vec3 va = vec3(vertices[vertind + 0].x, vertices[vertind + 0].y, vertices[vertind + 0].z);
		vec3 vb = vec3(vertices[vertind + 1].x, vertices[vertind + 1].y, vertices[vertind + 1].z);
		vec3 vc = vec3(vertices[vertind + 2].x, vertices[vertind + 2].y, vertices[vertind + 2].z);

		vec3 diff0 = -playerposfp - va;
		vec3 diff1 = -playerposfp - vb;
		vec3 diff2 = -playerposfp - vc;
		const float farsquared = far * far;
		bool farcull =
			diff0.dot(diff0) > farsquared &&
			diff1.dot(diff1) > farsquared &&
			diff2.dot(diff2) > farsquared;

		// far cull without clipping
		if (farcull && farclip) {
			continue;
		}

		const vec3 planepoint = vec3(0.0f, 0.0f, near);
		const vec3 planenormal = vec3(0.0f, 0.0f, 1.0f);
		vec4 viewtria = cameraMat * vec4(va.x, va.y, va.z, 1.0f);
		vec4 viewtrib = cameraMat * vec4(vb.x, vb.y, vb.z, 1.0f);
		vec4 viewtric = cameraMat * vec4(vc.x, vc.y, vc.z, 1.0f);
		vec2 uva = vec2(uvs[vertind + 0].x, uvs[vertind + 0].y);
		vec2 uvb = vec2(uvs[vertind + 1].x, uvs[vertind + 1].y);
		vec2 uvc = vec2(uvs[vertind + 2].x, uvs[vertind + 2].y);

		// fixed point starts here
		vec3 passedverts[MAX_CLIPPED_VERTS];
		vec2 passeduvs[MAX_CLIPPED_VERTS];
		float passedws[MAX_CLIPPED_VERTS];
		int numpassedverts = 3;
		passedverts[0] = vec3(viewtria.x, viewtria.y, viewtria.z);
		passedverts[1] = vec3(viewtrib.x, viewtrib.y, viewtrib.z);
		passedverts[2] = vec3(viewtric.x, viewtric.y, viewtric.z);
		passeduvs[0] = vec2(uva.x, uva.y);
		passeduvs[1] = vec2(uvb.x, uvb.y);
		passeduvs[2] = vec2(uvc.x, uvc.y);
		passedws[0] = 1.0f;
		passedws[1] = 1.0f;
		passedws[2] = 1.0f;

		vec3 offset = vec3(0.0f, 0.0f, 0.0f);
		// 5 sided triangle clip
		triangleclipuv(&passedverts[0], &passeduvs[0], &passedws[0], numpassedverts);
		// fill triangles
		for (int i = 0; i < numpassedverts - 2; ++i) {
			// draw triangle fan
			TriangleUVFP tri(
				passedverts[0] - offset,
				passedverts[i + 1] - offset,
				passedverts[i + 2] - offset,
				passeduvs[0],
				passeduvs[i + 1],
				passeduvs[i + 2],
				passedws[0],
				passedws[i + 1],
				passedws[i + 2]);
			// watertight test
			rastertriangleuv(tri);
		}
	}
}