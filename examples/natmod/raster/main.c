#include <math.h>

#define RP2 1

#if RP2
#include "py/dynruntime.h"
#else
#include <windows.h>
#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <SDL.h>
#include <winsock.h>
#include <assert.h>
#pragma comment(lib, "Ws2_32.lib")
#define PI 3.14159265359f

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
#endif

#define screenmultiple 1

#define width 128 * screenmultiple
#define height 128 * screenmultiple
#undef near
#undef far
#define near 0.01f
#define far 10.0f
// TODO enable
#define farclip false

#if !RP2
void initrender() {
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	// Create a window
	window = SDL_CreateWindow("SDL Draw Line", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
	}
	// Create a renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
	}
}
void clearscreen(int value) {
	SDL_PumpEvents();
	// clear rt
	SDL_SetRenderDrawColor(renderer, value, value, value, 255);
	// Clear the screen
	SDL_RenderClear(renderer);
}
void updateframe() {
	SDL_RenderPresent(renderer);
}
#endif

#if RP2
// TODO byte only memcpy, add long copies
void* memcpy(void* dst, const void* src, size_t len) {
	size_t i;
	unsigned char* d = (unsigned char*)dst;
	const unsigned char* s = (unsigned char*)src;

	for (i = 0; i < len; i++) {
		d[i] = s[i];
	}
	return dst;
}
#endif

typedef struct {
	unsigned short* color;
	float* depth;
	float* cameraposition;
	float* viewmatrix;
	float* projectionmatrix;
	int numverts;
	float* vertices;
	unsigned int* shapematerialmap;			// mapping from triangle ranges to materials
	unsigned int* globalmaterialmap; 		// metadata for global materials
	unsigned short* globalmaterialdata; 	// data for global materials
} DrawMeshArgs;

void rastermesh(DrawMeshArgs* meshargs);

#if RP2

void clearscenedepth(DrawMeshArgs* meshargs);

static mp_obj_t drawtriangles(
	mp_obj_fun_bc_t* self,
	size_t n_args,
	size_t n_kw,
	mp_obj_t* args) {

	int argindex = 0;
	mp_buffer_info_t colorbuffer;
	mp_get_buffer_raise(args[argindex++], &colorbuffer, MP_BUFFER_RW);
	mp_buffer_info_t depthbuffer;
	mp_get_buffer_raise(args[argindex++], &depthbuffer, MP_BUFFER_RW);
	mp_buffer_info_t camerapositionbuffer;
	mp_get_buffer_raise(args[argindex++], &camerapositionbuffer, MP_BUFFER_RW);
	mp_buffer_info_t viewmatrixbuffer;
	mp_get_buffer_raise(args[argindex++], &viewmatrixbuffer, MP_BUFFER_RW);
	mp_buffer_info_t projectionmatrixbuffer;
	mp_get_buffer_raise(args[argindex++], &projectionmatrixbuffer, MP_BUFFER_RW);
	int numverts = mp_obj_get_int(args[argindex++]);
	mp_buffer_info_t vertexbuffer;
	mp_get_buffer_raise(args[argindex++], &vertexbuffer, MP_BUFFER_RW);
	mp_buffer_info_t shapematerialmapbuffer;
	mp_get_buffer_raise(args[argindex++], &shapematerialmapbuffer, MP_BUFFER_RW);
	mp_buffer_info_t globalmaterialmapbuffer;
	mp_get_buffer_raise(args[argindex++], &globalmaterialmapbuffer, MP_BUFFER_RW);
	mp_buffer_info_t globalmaterialdatabuffer;
	mp_get_buffer_raise(args[argindex++], &globalmaterialdatabuffer, MP_BUFFER_RW);

	unsigned short* color = (unsigned short*)colorbuffer.buf;
	float* depth = (float*)depthbuffer.buf;
	float* cameraposition = (float*)camerapositionbuffer.buf;
	float* viewmatrix = (float*)viewmatrixbuffer.buf;
	float* projectionmatrix = (float*)projectionmatrixbuffer.buf;
	float* vertices = (float*)vertexbuffer.buf;
	unsigned int* shapematerialmap = (unsigned int*)shapematerialmapbuffer.buf;
	unsigned int* globalmaterialmap = (unsigned int*)globalmaterialmapbuffer.buf;
	unsigned short* globalmaterialdata = (unsigned short*)globalmaterialdatabuffer.buf;

	DrawMeshArgs meshargs;
	meshargs.color = color;
	meshargs.depth = depth;
	meshargs.cameraposition = cameraposition;
	meshargs.viewmatrix = viewmatrix;
	meshargs.projectionmatrix = projectionmatrix;
	meshargs.numverts = numverts;
	meshargs.vertices = vertices;
	meshargs.shapematerialmap = shapematerialmap;				// mapping from triangle ranges to materials
	meshargs.globalmaterialmap = globalmaterialmap;		// metadata for global materials
	meshargs.globalmaterialdata = globalmaterialdata; 	// data for global materials

	clearscenedepth(&meshargs);

	rastermesh(&meshargs);

	return mp_const_none;
}

// This is the entry point and is called when the module is imported
mp_obj_t mpy_init(mp_obj_fun_bc_t* self, size_t n_args, size_t n_kw, mp_obj_t* args) {
	// This must be first, it sets up the globals dict and other things
	MP_DYNRUNTIME_INIT_ENTRY

		// The tracescene function uses the most general C argument interface	
		mp_store_global(MP_QSTR_drawtriangles, MP_DYNRUNTIME_MAKE_FUNCTION(drawtriangles));

	// This must be last, it restores the globals dict
	MP_DYNRUNTIME_INIT_EXIT
}
#endif

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

#if RP2
float sqrtf(float a) {
	return 1.0f / invsqrtf(a);
}
#endif

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
vec3 vecmuls(vec3 a, float b) {
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

vec3 vec3C(float inx, float iny, float inz) {
	vec3 out;
	out.x = inx;
	out.y = iny;
	out.z = inz;
	return out;
}
vec3 getnormalized(vec3 a) {
	float vectormag = mag(a);
	if (vectormag == 0.0f) {
		return vec3C(0.0f, 0.0f, 0.0f);
	}
	else {
		return vecdivs(a, vectormag);
	}
}
vec3 vecrcp(vec3 a) {
	return vec3C(1.0f / a.x, 1.0f / a.y, 1.0f / a.z);
}
vec3 vecmin(vec3 a, vec3 b) {
	return vec3C(fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z));
}
vec3 vecmax(vec3 a, vec3 b) {
	return vec3C(fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z));
}
vec3 cross(vec3 a, vec3 b) {
	vec3 out;
	out.x = a.y * b.z - a.z * b.y;
	out.y = a.z * b.x - a.x * b.z;
	out.z = a.x * b.y - a.y * b.x;
	return out;
}

typedef struct {
	float x;
	float y;
} vec2;
vec2 vec2C(float inx, float iny) {
	vec2 out;
	out.x = inx;
	out.y = iny;
	return out;
}
vec2 vecadd2(vec2 a, vec2 b) {
	vec2 out;
	out.x = a.x + b.x;
	out.y = a.y + b.y;
	return out;
}
vec2 vecsub2(vec2 a, vec2 b) {
	vec2 out;
	out.x = a.x - b.x;
	out.y = a.y - b.y;
	return out;
}
vec2 vecmuls2(vec2 a, float b) {
	vec2 out;
	out.x = a.x * b;
	out.y = a.y * b;
	return out;
}
vec2 vecdiv2(vec2 a, vec2 b) {
	vec2 out;
	out.x = a.x / b.x;
	out.y = a.y / b.y;
	return out;
}
vec2 vecdivs2(vec2 a, float b) {
	vec2 out;
	out.x = a.x / b;
	out.y = a.y / b;
	return out;
}

typedef struct {
	float x, y, z, w;
} vec4;
vec4 vec4C(float xin, float yin, float zin, float win) {
	vec4 out;
	out.x = xin;
	out.y = yin;
	out.z = zin;
	out.w = win;
	return out;
}
vec4 vec4CS(vec3 a, float win) {
	vec4 out;
	out.x = a.x;
	out.y = a.y;
	out.z = a.z;
	out.w = win;
	return out;
}
float dot4(vec4 a, vec4 b) {
	float out;
	out = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	return out;
}

typedef struct {
	vec4 rows[4];
} mat44;
mat44 mat44C(vec4 r0, vec4 r1, vec4 r2, vec4 r3)
{
	mat44 out;
	out.rows[0] = r0;
	out.rows[1] = r1;
	out.rows[2] = r2;
	out.rows[3] = r3;
	return out;
}
mat44 matmatmul44(mat44 mata, mat44 matb) {
	mat44 matout;
	vec4 colvec0 = vec4C(matb.rows[0].x, matb.rows[1].x, matb.rows[2].x, matb.rows[3].x);
	vec4 colvec1 = vec4C(matb.rows[0].y, matb.rows[1].y, matb.rows[2].y, matb.rows[3].y);
	vec4 colvec2 = vec4C(matb.rows[0].z, matb.rows[1].z, matb.rows[2].z, matb.rows[3].z);
	vec4 colvec3 = vec4C(matb.rows[0].w, matb.rows[1].w, matb.rows[2].w, matb.rows[3].w);
	for (int row = 0; row < 4; ++row)
	{
		matout.rows[row].x = dot4(mata.rows[row], colvec0);
		matout.rows[row].y = dot4(mata.rows[row], colvec1);
		matout.rows[row].z = dot4(mata.rows[row], colvec2);
		matout.rows[row].w = dot4(mata.rows[row], colvec3);
	}
	return matout;
}
vec4 matmul44(mat44 mat, vec4 vec) {
	vec4 vecout;
	vecout.x = dot4(mat.rows[0], vec);
	vecout.y = dot4(mat.rows[1], vec);
	vecout.z = dot4(mat.rows[2], vec);
	vecout.w = dot4(mat.rows[3], vec);
	return vecout;
}


void swap(float* a, float* b) {
	float temp = *a;
	*a = *b;
	*b = temp;
}
void swap2(vec2* a, vec2* b) {
	vec2 temp = *a;
	*a = *b;
	*b = temp;
}
void swap3(vec3* a, vec3* b) {
	vec3 temp = *a;
	*a = *b;
	*b = temp;
}
void swapptr(void** a, void** b) {
	void* temp = *a;
	*a = *b;
	*b = temp;
}

typedef struct {
	float x;
	float y;
	float z;
	float uvx;
	float uvy;
} VertexPUV;

// per triangle material map for a single material 
// requires rendering from triangle 0 to max triangles
typedef struct {
	int maxvertexindex;	// max index for range sweeping from 0
	int textureid;			// and index to a texture description
} ShapeMaterialMap;

typedef struct {
	int texwidth;
	int texheight;
	int globaloffset;
} GlobalMaterialMap;

typedef struct {
	vec3 a, b, c;
	vec2 auv, buv, cuv;
	float aw, bw, cw;
} TriangleUV;
TriangleUV TriangleUVC(
	vec3 ain,
	vec3 bin,
	vec3 cin,
	vec2 auvin,
	vec2 buvin,
	vec2 cuvin,
	float awin,
	float bwin,
	float cwin) {
	TriangleUV tri;
	tri.a = ain;
	tri.b = bin;
	tri.c = cin;
	tri.auv = auvin;
	tri.buv = buvin;
	tri.cuv = cuvin;
	tri.aw = awin;
	tri.bw = bwin;
	tri.cw = cwin;
	return tri;
}
void sortascending(TriangleUV* triangle) {
	if (triangle->a.y > triangle->b.y) {
		swap3(&triangle->a, &triangle->b);
		swap2(&triangle->auv, &triangle->buv);
		swap(&triangle->aw, &triangle->bw);
	}
	if (triangle->b.y > triangle->c.y) {
		swap3(&triangle->b, &triangle->c);
		swap2(&triangle->buv, &triangle->cuv);
		swap(&triangle->bw, &triangle->cw);
	}
	if (triangle->a.y > triangle->b.y) {
		swap3(&triangle->a, &triangle->b);
		swap2(&triangle->auv, &triangle->buv);
		swap(&triangle->aw, &triangle->bw);
	}
}
vec3 facenormal(TriangleUV triangles) {
	vec3 tricross = cross(vecsub(triangles.a, triangles.b),
		vecsub(triangles.c, triangles.a));
	return getnormalized(tricross);
}
typedef struct {
	vec3 a;
	vec3 b;
	vec2 uva;
	vec2 uvb;
	float wa;
	float wb;
} Edge3UV;
Edge3UV Edge3UVC(vec3 ain, vec3 bin, vec2 auvin, vec2 buvin, float wain, float wbin) {
	Edge3UV out;
	out.a = ain;
	out.b = bin;
	out.uva = auvin;
	out.uvb = buvin;
	out.wa = wain;
	out.wb = wbin;
	return out;
}

typedef struct {
	int texwidth;
	int texheight;
	unsigned short* data;
} Texture;
Texture TextureC(int widthin, int heightin, unsigned short* datain) {
	Texture out;
	out.texwidth = widthin;
	out.texheight = heightin;
	out.data = datain;
	return out;
}
inline unsigned short sampletextureuv(
	Texture texture,
	vec2 uv) {
	// source textures are y flipped
	uv.y = 1.0f - uv.y;
	int addrx = (int)(uv.x * (float)texture.texwidth);
	int addry = (int)(uv.y * (float)texture.texheight);
	if (addrx < 0) {
		addrx = addrx + texture.texwidth * (1 - addrx / texture.texwidth);
	}
	if (addry < 0) {
		addry = addry + texture.texheight * (1 - addry / texture.texheight);
	}
	addrx %= texture.texwidth;
	addry %= texture.texheight;
	int linearaddress = addry * texture.texwidth + addrx;
	if (linearaddress < 0) {
		linearaddress = 0;
	}
	unsigned short rgb = 0x0;
	if (texture.data) {
		rgb = texture.data[linearaddress];
	}
	return rgb;
}

Texture boundtexture;

void clearscenedepth(DrawMeshArgs* meshargs) {
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			meshargs->depth[x * height + y] = 100.0f;
			meshargs->color[x * height + y] = 0x0;
		}
	}
}

#if !RP2
void present(DrawMeshArgs* meshargs) {
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			unsigned int color565 = meshargs->color[x * height + y];
			float red = (float)((color565 >> 11) & 0x1F) / 31.0f;
			float green = (float)((color565 >> 5) & 0x3F) / 63.0f;
			float blue = (float)((color565) & 0x1F) / 31.0f;
			SDL_SetRenderDrawColor(renderer, 255.0f * red, 255.0f * green, 255.0f * blue, 255);
			SDL_RenderDrawPoint(renderer, x, y);
		}
	}
}
#endif

inline int clampi(int val, int min, int max) {
	int out = val;
	if (val < min) {
		out = min;
	}
	if (val > max) {
		out = max;
	}
	return out;
}

inline float clampf(float val, float min, float max) {
	float out = val;
	if (val < min) {
		out = min;
	}
	if (val > max) {
		out = max;
	}
	return out;
}

inline int mini(int a, int b) {
	return a < b ? a : b;
}

inline int maxi(int a, int b) {
	return a > b ? a : b;
}

// Return signed shortest distance from point to plane, plane normal must be normalised
inline float vertexplanedistance(vec3 vertex, vec3 planepoint, vec3 planenormal) {
	return dot(planenormal, vertex) - dot(planenormal, planepoint);
};

inline vec3 vectorplaneintersect(vec3 plane_p, vec3 plane_n, vec3 lineStart, vec3 lineEnd, float* t)
{
	plane_n = getnormalized(plane_n);
	float plane_d = -(dot(plane_n, plane_p));
	float ad = dot(lineStart, plane_n);
	float bd = dot(lineEnd, plane_n);
	*t = (-plane_d - ad) / (bd - ad);
	vec3 lineStartToEnd = vecsub(lineEnd, lineStart);
	vec3 lineToIntersect = vecmuls(lineStartToEnd, *t);
	return vecadd(lineStart, lineToIntersect);
}

inline void drawscandepthuv(
	DrawMeshArgs* meshargs,
	int y,
	int x1,
	int x2,
	float z1,
	float z2,
	vec2 uva,
	vec2 uvb,
	float wa,
	float wb) {
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
	int x1c = clampi(x1, 0, width - 1);
	int x2c = clampi(x2, 0, width - 1);
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
		if (meshargs->depth[y * height + x] > z / w) {
			// TODO flexible shader implementation should go here
			// Or write attributes to gbuffer
			vec2 uv = vecadd2(vecmuls2(uva, (1.0f - t)), vecmuls2(uvb, t));
			unsigned short rgb565 = sampletextureuv(boundtexture, vecdivs2(uv, w));
			if (rgb565 == 0xffff) {
				continue;
			}
			if (!(x >= 0 && x < width && y >= 0 && y < height)) {
				continue;
			}
			meshargs->depth[y * height + x] = z / w;
			meshargs->color[y * height + x] = rgb565;
		}
	}
}

// draw from top to bottom 
inline void fillBottomFlatTriangleuv(
	DrawMeshArgs* meshargs,
	TriangleUV tri) {
	vec3 v1 = tri.a;
	vec3 v2 = tri.b;
	vec3 v3 = tri.c;
	vec2 uv1 = tri.auv;
	vec2 uv2 = tri.buv;
	vec2 uv3 = tri.cuv;
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

	vec2 invuvslope1 = vecdivs2(vecsub2(uv2, uv1), v2.y - v1.y);
	vec2 invuvslope2 = vecdivs2(vecsub2(uv3, uv1), v3.y - v1.y);

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

	int minx = mini(mini((int)v1.x, (int)v2.x), (int)v3.x);
	int maxx = maxi(maxi((int)v1.x, (int)v2.x), (int)v3.x);

	v1.y = (int)clampi((int)v1.y, 0, (int)height-1);
	v2.y = (int)clampi((int)v2.y, 0, (int)height-1);

	for (int scanlineY = (int)v1.y; scanlineY <= (int)v2.y; scanlineY++)
	{
		curx1 = clampf(curx1, (float)minx, (float)maxx);
		curx2 = clampf(curx2, (float)minx, (float)maxx);
		drawscandepthuv(meshargs, scanlineY, (int)curx1, (int)curx2, curz1, curz2, curuv1, curuv2, curw1, curw2);
		curx1 += invslope1;
		curx2 += invslope2;
		curz1 += invdepthslope1;
		curz2 += invdepthslope2;
		curuv1 = vecadd2(curuv1, invuvslope1);
		curuv2 = vecadd2(curuv2, invuvslope2);
		curw1 += invwslope1;
		curw2 += invwslope2;
	}
}
// draw from bottom to top 
inline void fillTopFlatTriangleuv(
	DrawMeshArgs* meshargs,
	TriangleUV tri) {
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

	vec2 invuvslope1 = vecdivs2(vecsub2(uv3, uv1), v3.y - v1.y);
	vec2 invuvslope2 = vecdivs2(vecsub2(uv3, uv2), v3.y - v2.y);

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

	int minx = mini(mini((int)v1.x, (int)v2.x), (int)v3.x);
	int maxx = maxi(maxi((int)v1.x, (int)v2.x), (int)v3.x);

	for (int scanlineY = (int)v3.y; scanlineY > (int)v1.y; scanlineY--)
	{
		curx1 = clampf(curx1, (float)minx, (float)maxx);
		curx2 = clampf(curx2, (float)minx, (float)maxx);
		drawscandepthuv(meshargs, scanlineY, (int)curx1, (int)curx2, curz1, curz2, curuv1, curuv2, curw1, curw2);
		curx1 -= invslope1;
		curx2 -= invslope2;
		curz1 -= invdepthslope1;
		curz2 -= invdepthslope2;
		curuv1 = vecsub2(curuv1, invuvslope1);
		curuv2 = vecsub2(curuv2, invuvslope2);
		curw1 -= invwslope1;
		curw2 -= invwslope2;
	}
}

typedef enum {
	NONE,
	A,
	B,
	AB,
} ClipCode;
// fill a triangle using the standard fille algorithm filling top and bottom triangles
// convert to Brensenham for interger math speedup
// http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
inline void rastertriangleuv(
	DrawMeshArgs* meshargs,
	TriangleUV tri) {
	// sort verts in y order where the highest position y will be vetexa
	sortascending(&tri);
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
		fillBottomFlatTriangleuv(meshargs, tri);
	}
	/* check for trivial case of top-flat triangle */
	else if (tri.a.y == tri.b.y)
	{
		fillTopFlatTriangleuv(meshargs, tri);
	}
	else
	{
		float ylerp = (b.y - a.y) / (c.y - a.y);
		float depthinterp = (a.z + ylerp * (c.z - a.z));
		/* general case - split the triangle in a topflat and bottom-flat one */
		vec3 d = vec3C(
			a.x + ylerp * (c.x - a.x),
			b.y,
			depthinterp);

		vec2 duv = vec2C(
			tri.auv.x * (1.0f - ylerp) + tri.cuv.x * ylerp,
			tri.auv.y * (1.0f - ylerp) + tri.cuv.y * ylerp);

		float dw = tri.aw * (1.0f - ylerp) + tri.cw * ylerp;
		TriangleUV tria = TriangleUVC(a, b, d, auv, buv, duv, aw, bw, dw);
		fillBottomFlatTriangleuv(meshargs, tria);
		TriangleUV trib = TriangleUVC(b, d, c, buv, duv, cuv, bw, dw, cw);
		fillTopFlatTriangleuv(meshargs, trib);
	}
}

inline Edge3UV clipplanecodeduv(
	Edge3UV edge,
	vec3 planepoint,
	vec3 planenormal,
	ClipCode* clipcode) {
	*clipcode = AB;
	Edge3UV edgeout = edge;
	float d0 = vertexplanedistance(edge.a, planepoint, planenormal);
	float d1 = vertexplanedistance(edge.b, planepoint, planenormal);
	bool intersect = false;
	bool flippededge = false;
	if ((d0 >= 0.0f) ^ (d1 >= 0.0f)) {
		intersect = true;
		if (d1 >= 0.0f)
		{
			flippededge = true;
			edgeout.a = edge.b;
			edgeout.b = edge.a;
			edgeout.uva = edge.uvb;
			edgeout.uvb = edge.uva;
			edgeout.wa = edge.wb;
			edgeout.wb = edge.wa;
		}
	}
	if ((d0 < 0.0f) && (d1 < 0.0f)) {
		*clipcode = NONE;
	}
	if (intersect)
	{
		float t = 0.0f;
		vec3 intersect0 = vectorplaneintersect(planepoint, planenormal, edgeout.a, edgeout.b, &t);
		if (t >= 0.0f && t <= 1.0f) {
			*clipcode = flippededge ? B : A;
			edgeout.b = intersect0;
			edgeout.uvb = vecadd2(vecmuls2(edgeout.uva, (1.0f - t)), vecmuls2(edgeout.uvb, t));
			edgeout.wb = edgeout.wa * (1.0f - t) + edgeout.wb * t;
		}
	}
	return edgeout;
}

// Sutherland Hodgman clipping algorithm
// there are other algorithms with less comparison that could be used instead
// this algorithm can work with polygons not just triangles
#define MAX_CLIPPED_VERTS 16 // probably not actually the actual minimal max triangles that can be generated
// Passed verts initialized in camera space
void triangleclipuv(
	mat44 projmat,
	vec3* passedverts,
	vec2* passeduvs,
	float* passedws,
	int* numpassedverts) {
#define NUM_PLANES 5
	const vec3 planepoints[NUM_PLANES] = {
		vec3C(0.0f, 0.0f, near), // near
		vec3C(0.0f, 0.0f, 0.0f), // bottom
		vec3C(1.0f, 1.0f, 0.0f), // top
		vec3C(0.0f, 0.0f, 0.0f), // left
		vec3C(1.0f, 1.0f, 0.0f), // right
	};
	const vec3 planenormals[NUM_PLANES] = {
		vec3C(0.0f, 0.0f, 1.0f), // near
		vec3C(0.0f, 1.0f, 0.0f),
		vec3C(0.0f, -1.0f, 0.0f),
		vec3C(1.0f, 0.0f, 0.0f),
		vec3C(-1.0f, 0.0f, 0.0f),
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
		int numoutpassedverts = *numpassedverts;
		// reset passed verts for the clipping loops
		*numpassedverts = 0;
		for (int vertexi = 0; vertexi < numoutpassedverts; ++vertexi) {
			Edge3UV visitededge = Edge3UVC(
				passedverts[vertexi],
				passedverts[(vertexi + 1) % numoutpassedverts],
				passeduvs[vertexi],
				passeduvs[(vertexi + 1) % numoutpassedverts],
				1.0f,  // wa
				1.0f); // wb
			ClipCode clipcode = NONE;
			Edge3UV clippededge = clipplanecodeduv(visitededge, planepoints[0], planenormals[0], &clipcode);
			// returns a clip code that determines which verts if any should pass the ith clip plane
			switch (clipcode) {
			case NONE: // edge outside the plane, add no verts
				break;
			case AB:   // edge entirely inside the frustum, add only first vertex
					   // as the second vertex of the edge will be added in the next or previous iterration
				outputpasseduvs[*numpassedverts] = clippededge.uva;
				outputpassedverts[(*numpassedverts)++] = clippededge.a;
				break;
			case A: // edge starting inside, add both clipped verts
				outputpasseduvs[*numpassedverts] = clippededge.uva;
				outputpassedverts[(*numpassedverts)++] = clippededge.a;
				outputpasseduvs[*numpassedverts] = clippededge.uvb;
				outputpassedverts[(*numpassedverts)++] = clippededge.b;
				break;
			case B: // edge starting outside, record the vert on the plane
					// the other vert will be added in another iterration
				outputpasseduvs[*numpassedverts] = clippededge.uvb;
				outputpassedverts[(*numpassedverts)++] = clippededge.b;
				break;
			}
		}
		swapptr((void**)&passedverts, (void**)&outputpassedverts);
		swapptr((void**)&passeduvs, (void**)&outputpasseduvs);
	}
	if (*numpassedverts <= 0) {
		return;
	}

	bool hasoutside = false;
	bool hasinside = false;
	// transform the remaining triangles to clip space
	// project to 0 - 1 screen space to allow easy reject or accept
	// by looking at fixed point fraction bits only
	for (int vertexi = 0; vertexi < *numpassedverts; ++vertexi) {
		vec4 vecproj = matmul44(projmat, vec4CS(passedverts[vertexi], 1.0f));
		vec3 vecscreen = vecdivs(vec3C(vecproj.x, vecproj.y, vecproj.z), vecproj.w);
		float x0 = (1.0f - (vecscreen.x + 1.0f) * 0.5f);
		float y0 = (1.0f - (vecscreen.y + 1.0f) * 0.5f);
		vecscreen.x = x0;
		vecscreen.y = y0;
		passedverts[vertexi] = vecscreen;
		passeduvs[vertexi] = vecdivs2(passeduvs[vertexi], vecproj.w);
		passedws[vertexi] = 1.0f / vecproj.w;
		// check only integer bits for any point outside the 0..1 clip which requires more clip testing
		if ((vecscreen.x < 0.0f || vecscreen.y < 0.0f) ||
			(vecscreen.x > 1.0f || vecscreen.y > 1.0f)) {
			hasoutside = true;
		}
		// check for any point inside the 0..1 clip with fraction bits 
		// 
		if ((vecscreen.x >= 0.0f && vecscreen.y >= 0.0f) &&
			(vecscreen.x <= 1.0f && vecscreen.y <= 1.0f)) {
			hasinside = true;
		}
	}

	// area of 2d triangle in screen space after perspective divide
	// if 2 triangles are created from the near clip they should shader the same handedness
	// TODO program ccw in some render state object
	bool useccw = true;
	float sign = useccw ? 1.0f : -1.0f;
	float area = (passedverts[0].x - passedverts[1].x) * (passedverts[0].y - passedverts[2].y) -
		(passedverts[0].x - passedverts[2].x) * (passedverts[0].y - passedverts[1].y);

	// Backface culling
	if ((sign * area < 0.0f)) {
		//*numpassedverts = 0;
		//return;
	}

	// fast path trivial accept
	if (!hasoutside) {
		swapptr((void**)&passedverts, (void**)&outputpassedverts);
		swapptr((void**)&passeduvs, (void**)&outputpasseduvs);
		swapptr((void**)&passedws, (void**)&outputpassedws);
		memcpy(passedverts, outputpassedverts, sizeof(vec3) * *numpassedverts);
		memcpy(passeduvs, outputpasseduvs, sizeof(vec2) * *numpassedverts);
		memcpy(passedws, outputpassedws, sizeof(float) * *numpassedverts);
		// transform the remaining triangles to screen space
		for (int vertexi = 0; vertexi < *numpassedverts; ++vertexi) {
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
		*numpassedverts = 0;
		return;
	}

	// clip against 4 screen sides
	for (int planei = 1; planei < NUM_PLANES; ++planei) {
		int numoutpassedverts = *numpassedverts;
		// reset passed verts for the clipping loops
		*numpassedverts = 0;
		// for each pair of verts in order, check if they cross the axis of the plane
		// if neither vertex is inside the axis leave both verts from the list
		// if both verts are inside append both when they are the first vert in an edge
		// if only one vert is inside and another is outside append the intersected vert
		// when visiting some edges up to 2 verts are added which adds triangles to the the pass list
		// it is possible for many triangles to be generated from one initial triangle by 
		// clipping between 1 and 5 total planes
		for (int vertexi = 0; vertexi < numoutpassedverts; ++vertexi) {
			Edge3UV visitededge = Edge3UVC(
				passedverts[vertexi],
				passedverts[(vertexi + 1) % numoutpassedverts],
				passeduvs[vertexi],
				passeduvs[(vertexi + 1) % numoutpassedverts],
				passedws[vertexi],
				passedws[(vertexi + 1) % numoutpassedverts]);
			ClipCode clipcode = NONE;
			Edge3UV clippededge = clipplanecodeduv(visitededge, planepoints[planei], planenormals[planei], &clipcode);
			// returns a clip code that determines which verts if any should pass the ith clip plane
			switch (clipcode) {
			case NONE: // edge outside the plane, add no verts
				break;
			case AB:   // edge entirely inside the frustum, add only first vertex
					   // as the second vertex of the edge will be added in the next or previous iterration
				outputpassedws[*numpassedverts] = clippededge.wa;
				outputpasseduvs[*numpassedverts] = clippededge.uva;
				outputpassedverts[(*numpassedverts)++] = clippededge.a;
				break;
			case A: // edge starting inside, add both clipped verts
				outputpassedws[*numpassedverts] = clippededge.wa;
				outputpasseduvs[*numpassedverts] = clippededge.uva;
				outputpassedverts[(*numpassedverts)++] = clippededge.a;
				outputpassedws[*numpassedverts] = clippededge.wb;
				outputpasseduvs[*numpassedverts] = clippededge.uvb;
				outputpassedverts[(*numpassedverts)++] = clippededge.b;
				break;
			case B: // edge starting outside, record the vert on the plane
					// the other vert will be added in another iterration
				outputpassedws[*numpassedverts] = clippededge.wb;
				outputpasseduvs[*numpassedverts] = clippededge.uvb;
				outputpassedverts[(*numpassedverts)++] = clippededge.b;
				break;
			}
		}
		if (*numpassedverts <= 0) {
			return;
		}
		swapptr((void**)&passedverts, (void**)&outputpassedverts);
		swapptr((void**)&passeduvs, (void**)&outputpasseduvs);
		swapptr((void**)&passedws, (void**)&outputpassedws);
	}
	swapptr((void**)&passedverts, (void**)&outputpassedverts);
	swapptr((void**)&passeduvs, (void**)&outputpasseduvs);
	swapptr((void**)&passedws, (void**)&outputpassedws);
	memcpy(passedverts, outputpassedverts, sizeof(vec3) * *numpassedverts);
	memcpy(passeduvs, outputpasseduvs, sizeof(vec2) * *numpassedverts);
	memcpy(passedws, outputpassedws, sizeof(float) * *numpassedverts);
	// transform the remaining triangles to screen space
	for (int vertexi = 0; vertexi < *numpassedverts; ++vertexi) {
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

void rastermesh(DrawMeshArgs* meshargs) {
#if RP2
#else
	// Set draw color to black
	SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
#endif
	int numtriangles = meshargs->numverts;
	VertexPUV* vertices = (VertexPUV*)meshargs->vertices;

	ShapeMaterialMap* shapematerials = (ShapeMaterialMap*)meshargs->shapematerialmap;
	GlobalMaterialMap* globalmaterials = (GlobalMaterialMap*)meshargs->globalmaterialmap;

	int shapeindex = 0;
	int materialindex = shapematerials[shapeindex].textureid;
	boundtexture = TextureC(
		globalmaterials[materialindex].texwidth,
		globalmaterials[materialindex].texheight,
		meshargs->globalmaterialdata + globalmaterials[materialindex].globaloffset/sizeof(unsigned short));

	for (int vertind = 0; vertind < numtriangles; vertind += 3) {
		if (vertind > shapematerials[shapeindex].maxvertexindex) {
			shapeindex++;
			int materialindex = shapematerials[shapeindex].textureid;
			boundtexture = TextureC(
				globalmaterials[materialindex].texwidth,
				globalmaterials[materialindex].texheight,
				meshargs->globalmaterialdata + globalmaterials[materialindex].globaloffset/sizeof(unsigned short));
		}

		vec3 playerpos = vec3C(
			meshargs->cameraposition[0],
			meshargs->cameraposition[1],
			meshargs->cameraposition[2]);

		vec3 va = vec3C(vertices[vertind + 0].x, vertices[vertind + 0].y, vertices[vertind + 0].z);
		vec3 vb = vec3C(vertices[vertind + 1].x, vertices[vertind + 1].y, vertices[vertind + 1].z);
		vec3 vc = vec3C(vertices[vertind + 2].x, vertices[vertind + 2].y, vertices[vertind + 2].z);

		vec3 diff0 = vecneg(vecsub(playerpos, va));
		vec3 diff1 = vecneg(vecsub(playerpos, vb));
		vec3 diff2 = vecneg(vecsub(playerpos, vc));
		const float farsquared = far * far;
		bool farcull =
			dot(diff0, diff0) > farsquared &&
			dot(diff1, diff1) > farsquared &&
			dot(diff2, diff2) > farsquared;
		// far cull without clipping
		if (farcull && farclip) {
			//continue;
		}

		mat44 viewmatrix;
		memcpy(&viewmatrix, meshargs->viewmatrix, sizeof(mat44));
		//const vec3 planepoint = vec3C(0.0f, 0.0f, near);
		//const vec3 planenormal = vec3C(0.0f, 0.0f, 1.0f);
		vec4 viewtria = matmul44(viewmatrix, vec4C(va.x, va.y, va.z, 1.0f));
		vec4 viewtrib = matmul44(viewmatrix, vec4C(vb.x, vb.y, vb.z, 1.0f));
		vec4 viewtric = matmul44(viewmatrix, vec4C(vc.x, vc.y, vc.z, 1.0f));
		vec2 uva = vec2C(vertices[vertind + 0].uvx, vertices[vertind + 0].uvy);
		vec2 uvb = vec2C(vertices[vertind + 1].uvx, vertices[vertind + 1].uvy);
		vec2 uvc = vec2C(vertices[vertind + 2].uvx, vertices[vertind + 2].uvy);

		// fixed point starts here
		vec3 passedverts[MAX_CLIPPED_VERTS];
		vec2 passeduvs[MAX_CLIPPED_VERTS];
		float passedws[MAX_CLIPPED_VERTS];
		int numpassedverts = 3;
		passedverts[0] = vec3C(viewtria.x, viewtria.y, viewtria.z);
		passedverts[1] = vec3C(viewtrib.x, viewtrib.y, viewtrib.z);
		passedverts[2] = vec3C(viewtric.x, viewtric.y, viewtric.z);
		passeduvs[0] = vec2C(uva.x, uva.y);
		passeduvs[1] = vec2C(uvb.x, uvb.y);
		passeduvs[2] = vec2C(uvc.x, uvc.y);
		passedws[0] = 1.0f;
		passedws[1] = 1.0f;
		passedws[2] = 1.0f;

		mat44 projectionmatrix;
		memcpy(&projectionmatrix, meshargs->projectionmatrix, sizeof(mat44));
		// 5 sided triangle clip
		triangleclipuv(projectionmatrix, &passedverts[0], &passeduvs[0], &passedws[0], &numpassedverts);
		// fill triangles
		for (int i = 0; i < numpassedverts - 2; ++i) {
			// draw triangle fan
			TriangleUV tri = TriangleUVC(
				passedverts[0],
				passedverts[i + 1],
				passedverts[i + 2],
				passeduvs[0],
				passeduvs[i + 1],
				passeduvs[i + 2],
				passedws[0],
				passedws[i + 1],
				passedws[i + 2]);
			rastertriangleuv(
				meshargs,
				tri);
		}
	}
}

#if !RP2

vec3 playerpos = vec3C(0.0f, -1.0f, 0.0f);

typedef struct {
	unsigned int numvertices;
	float* vertices;
	unsigned int numshapes;
	unsigned int* shapematerialmap;		// max vertex index, material id in GlobalMaterials material map
} Mesh;
Mesh testmesh;

typedef struct {
	unsigned int nummaterials;		// number of unique materials
	unsigned int* materialmap;		// width, height, offset 
	unsigned int totalpixels;
	unsigned short* data;	// 565 format color data with no padding
} GlobalMaterials;
GlobalMaterials globalmats;

unsigned short colorbuffer[width * height * screenmultiple];
float depthbuffer[width * height * screenmultiple];

void loadtestmesh() {
	FILE* file;
	errno_t err = fopen_s(&file, "F:\\ThumbyRaster\\Data\\Peaches_Castle.bin", "rb");
	assert(file);
	// all mesh data is dword sized
	// load num verts
	fread((void*)&testmesh.numvertices, sizeof(unsigned int), 1, file);
	assert(testmesh.numvertices > 0);
	testmesh.vertices = new float[testmesh.numvertices * sizeof(VertexPUV)];
	// load vertices with position + uv
	fread(testmesh.vertices, sizeof(VertexPUV), testmesh.numvertices, file);
	// load number of shapes
	fread((void*)&testmesh.numshapes, sizeof(unsigned int), 1, file);
	assert(testmesh.numshapes > 0);
	testmesh.shapematerialmap = new unsigned int[testmesh.numshapes * sizeof(ShapeMaterialMap)];
	// load shape materials
	fread(testmesh.shapematerialmap, sizeof(ShapeMaterialMap), testmesh.numshapes, file);
	fclose(file);
}

void loadglobalmaterials() {
	FILE* file;
	errno_t err = fopen_s(&file, "F:\\ThumbyRaster\\Data\\Peaches_Castle_global_mat.bin", "rb");
	assert(file);
	// load number of materials
	fread((void*)&globalmats.nummaterials, sizeof(unsigned int), 1, file);
	assert(globalmats.nummaterials > 0);

	globalmats.materialmap = new unsigned int[globalmats.nummaterials * sizeof(GlobalMaterialMap)];
	fread(globalmats.materialmap, sizeof(GlobalMaterialMap), globalmats.nummaterials, file);

	fread((void*)&globalmats.totalpixels, sizeof(unsigned int), 1, file);
	assert(globalmats.totalpixels > 0);
	globalmats.data = new unsigned short[globalmats.totalpixels];

	fread(globalmats.data, sizeof(unsigned short), globalmats.totalpixels, file);

	fclose(file);
}

void GetMeshArgs(DrawMeshArgs* meshargs, vec3* position, mat44* projmat, mat44* viewmat, Mesh* mesh) {
	meshargs->color = colorbuffer;
	meshargs->depth = depthbuffer;
	meshargs->cameraposition = (float*)position;
	meshargs->projectionmatrix = (float*)projmat;
	meshargs->viewmatrix = (float*)viewmat;
	meshargs->vertices = mesh->vertices;
	meshargs->numverts = mesh->numvertices;
	meshargs->shapematerialmap = mesh->shapematerialmap;
	meshargs->globalmaterialmap = globalmats.materialmap;
	meshargs->globalmaterialdata = globalmats.data;
}

void loop() {
	clearscreen(0);

	int numkeys = 0;
	SDL_PumpEvents();

	bool wpress = false;
	bool spress = false;
	bool apress = false;
	bool dpress = false;
	bool abutton = false;
	bool bbutton = false;
	const Uint8* keystate = SDL_GetKeyboardState(&numkeys);
	if (keystate[SDL_SCANCODE_W]) {
		wpress = true;
	}
	if (keystate[SDL_SCANCODE_S]) {
		spress = true;
	}
	if (keystate[SDL_SCANCODE_A]) {
		apress = true;
	}
	if (keystate[SDL_SCANCODE_D]) {
		dpress = true;
	}
	if (keystate[SDL_SCANCODE_PERIOD]) {
		abutton = true;
	}
	if (keystate[SDL_SCANCODE_COMMA]) {
		bbutton = true;
	}

	if (wpress) {
		playerpos.z += 0.1f;
	}
	if (spress) {
		playerpos.z -= 0.1f;
	}
	if (apress) {
		playerpos.x += 0.1f;
	}
	if (dpress) {
		playerpos.x -= 0.1f;
	}
	if (abutton) {
		playerpos.y += 0.1f;
	}
	if (bbutton) {
		playerpos.y -= 0.1f;
	}

	float angleOfView = 90.0f;
	float fov = 1.0f / std::tan(angleOfView * 0.5f * PI / 180.0f);
	float aspectratio = 1.0f;
	mat44 projmat = mat44C(
		vec4C(aspectratio * fov, 0.0f, 0.0f, 0.0f),
		vec4C(0.0f, fov, 0.0f, 0.0f),
		vec4C(0.0f, 0.0f, (far / (far - near)), ((-far * near) / (far - near))),
		vec4C(0.0f, 0.0f, 1.0f, 0.0f));
	mat44 viewmap = mat44C(
		vec4C(1.0f, 0.0f, 0.0f, playerpos.x),
		vec4C(0.0f, 1.0f, 0.0f, playerpos.y),
		vec4C(0.0f, 0.0f, 1.0f, playerpos.z),
		vec4C(0.0f, 0.0f, 0.0f, 1.0f));

	DrawMeshArgs meshargs = {};
	GetMeshArgs(&meshargs, &playerpos, &projmat, &viewmap, &testmesh);
	clearscenedepth(&meshargs);

	rastermesh(&meshargs);

	present(&meshargs);

	updateframe();
}

#define main main  // Undo SDL main entry rename

int main() {
	initrender();
	loadtestmesh();
	loadglobalmaterials();
	bool isrunning = true;
	while (isrunning) {
		loop();
	}
	SDL_Quit();
	exit(0);
	return 0;
}
#endif