
#include "py/dynruntime.h"
#include <string.h>
#include <math.h>

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

float sqrtf(float a)
{
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

struct vec3
{
    float x;
    float y;
    float z;
};
struct vec3 vecadd(struct vec3 a, struct vec3 b)
{
    struct vec3 out;
    out.x = a.x + b.x;
    out.y = a.y + b.y;
    out.z = a.z + b.z;
    return out;
}
struct vec3 vecsub(struct vec3 a, struct vec3 b)
{
    struct vec3 out;
    out.x = a.x - b.x;
    out.y = a.y - b.y;
    out.z = a.z - b.z;
    return out;
}
struct vec3 vecmul(struct vec3 a, struct vec3 b)
{
    struct vec3 out;
    out.x = a.x * b.x;
    out.y = a.y * b.y;
    out.z = a.z * b.z;
    return out;
}
struct vec3 vecmuls(struct vec3 a, float b)
{
    struct vec3 out;
    out.x = a.x * b;
    out.y = a.y * b;
    out.z = a.z * b;
    return out;
}
struct vec3 vecneg(struct vec3 a)
{
    struct vec3 out;
    out = vecmuls(a, -1.0f);
    return out;
}
struct vec3 vecdiv(struct vec3 a, struct vec3 b)
{
    struct vec3 out;
    out.x = a.x / b.x;
    out.y = a.y / b.y;
    out.z = a.z / b.z;
    return out;
}
struct vec3 vecdivs(struct vec3 a, float b)
{
    struct vec3 out;
    out.x = a.x / b;
    out.y = a.y / b;
    out.z = a.z / b;
    return out;
}
float dot(struct vec3 a, struct vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
float mag(struct vec3 a)
{
    return sqrtf(dot(a, a));
}

struct vec3 vec3(float inx, float iny, float inz) {
    struct vec3 out;
    out.x = inx;
    out.y = iny;
    out.z = inz;
    return out;
}

struct vec3 getnormalized(struct vec3 a)
{
    float vectormag = mag(a);
    if (vectormag == 0.0f) {
        return vec3(0.0f, 0.0f, 0.0f);
    }
    else {
        return vecdivs(a, vectormag);
    }
}

struct vec3 vecrcp(struct vec3 a)
{
	return vec3(1.0f/a.x, 1.0f/a.y, 1.0f/a.z);
}

struct vec3 vecmin(struct vec3 a, struct vec3 b)
{
	return vec3(fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z));
}

struct vec3 vecmax(struct vec3 a, struct vec3 b)
{
	return vec3(fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z));
}

struct Ray {
    struct vec3 origin;
    struct vec3 direction;
};
struct Ray MakeRay (struct vec3 origin, struct vec3 direction) {
    struct Ray out;
    out.origin = origin;
    out.direction = getnormalized(direction);
    return out;
}
 struct vec3 point_at_parameter(struct Ray r, float t) {
    return vecadd(r.origin, vecmuls(r.direction, t));
}

struct Sphere {
    float radius;
    struct vec3 center;
	struct vec3 color;
};
struct Sphere MakeSphere(struct vec3 acenter, float aradius) {
    struct Sphere s;
    s.center = acenter;
    s.radius = aradius;
    return s;
}
bool intersect(const struct Sphere s, const struct Ray ray, float* t) {
    struct vec3 oc = vecsub(ray.origin, s.center);
    float a = dot(ray.direction, ray.direction);
    float b = 2.0f * dot(oc, ray.direction);
    float c = dot(oc, oc) - s.radius * s.radius;
    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f) {
        return false;
    }
    else {
        // t = (-b - sqrt(discriminant)) / (2.0f * a);
        *t = (-b - sqrtf(discriminant)) / (2.0f * a);
        return true;
    }
}

struct Plane {
    struct vec3 position;
    struct vec3 normal;
	struct vec3 color;
};
 struct Plane MakePlane(struct vec3 aposition, struct vec3 anormal, struct vec3 acolor) {
    struct Plane p;
    p.position = aposition;
    p.normal = anormal;
	p.color = acolor;
    return p;
}
bool intersectp(struct Plane p, struct Ray ray, float* t)
{
    float denom = dot(p.normal, ray.direction);
    if (denom > 0.0001f) {
        struct vec3 p0l0 = vecsub(p.position, ray.origin);
        *t = dot(p0l0, p.normal)/ denom;
        return ((*t) >= 0.0f);
    }
    return false;
}

bool visiblecheck(struct Ray ray, const struct Sphere* spheres, int numspheres, float minlength) {
    bool bvis = true;
    for (int i = 0; i < numspheres; ++i) {
        float t = 0.0f;
        if (intersect(spheres[i], ray, &t)) {
            if (t <= minlength + 0.001f) {
                bvis = false;
            }
        }
    }
    return bvis;
}

bool visiblecheck2(struct Ray ray, const struct Sphere* spheres, int numspheres, int spherei, float minlength) {
    bool bvis = true;
    for (int i = 0; i < numspheres; ++i) {
        float t = 0.0f;
		if(i != spherei) {
			if (intersect(spheres[i], ray, &t)) {
				if (t <= minlength + 0.001f) {
					bvis = false;
				}
			}
		}
    }
    return bvis;
}

struct vec3 reflect(struct vec3 invec, struct vec3 normal)
{
	struct vec3 outvec = vecsub(invec, vecmuls(vecmuls(normal, dot(invec, normal)), 2.0f));
	return outvec;
}

struct vec3 reflecttrace2(
	struct Ray ray, 
	const struct Sphere* spheres, 
	int numspheres, 
	int mirrorspherei,
	const struct Plane* planes, 	
	int numplanes,
	int mirrorplanei,
	struct vec3 lightpos)
{
	struct vec3 outcolor;
	outcolor.x = 0.0f;
	outcolor.y = 0.0f;
	outcolor.z = 0.0f;
	float t = 0.0f;
	float mint = 1000.0f;
	const float visbias = 0.001f;
	float diffuse = 0.0f;

	for (int spherei = 0; spherei < numspheres; ++spherei) {
		if (spherei == mirrorspherei) {
			continue;
		}
		if (intersect(spheres[spherei], ray, &t)) {
			if (spherei == 0) {
				if (t < mint) {
					mint = t;
					outcolor.x = 255.0f;
					outcolor.y = 255.0f;
					outcolor.z = 255.0f;
				}
			}
			else {
				if (t < mint) {
					struct vec3 point = point_at_parameter(ray, t);
					struct vec3 normal = getnormalized(vecsub(point, spheres[spherei].center));
					struct vec3 lightvec = vecneg(getnormalized(vecsub(lightpos, point)));
					float length = sqrtf(dot(vecsub(point, lightpos), vecsub(point, lightpos)));
					struct Ray visray = MakeRay(vecadd(point, vecmuls(normal, visbias)), lightvec);
					mint = t;
					if (visiblecheck2(visray, spheres, numspheres, spherei, length)) {
						float color = (dot(lightvec, normal) + 1.0f) / 2.0f;
						color += diffuse;
						color = fminf(1.0f, color);
						//color = 1.0f;
						outcolor.x = spheres[spherei].color.x * color;
						outcolor.y = spheres[spherei].color.y * color;
						outcolor.z = spheres[spherei].color.z * color;
					}
				}
			}
		}
	}
	for (int planei = 0; planei < numplanes; ++planei) {
		if(planei == mirrorplanei) {
			continue;
		}
		if (intersectp(planes[planei], ray, &t)) {
			if (t < mint) {
				struct vec3 point = point_at_parameter(ray, t);
				struct vec3 normal = planes[planei].normal;
				struct vec3 lightvec = getnormalized(vecsub(lightpos, point));
				float length = sqrtf(dot(vecsub(point, lightpos), vecsub(point, lightpos)));
				struct Ray visray = MakeRay(vecadd(point, vecmuls(normal, visbias)), lightvec);
				mint = t;
				if (visiblecheck2(visray, spheres, numspheres, -1, length)) {
					float color = (dot(lightvec, normal) + 1.0f) / 2.0f;
					color *= 1.0f / (length * length);
					color += diffuse;
					color = fminf(1.0f, color);
					//color = 1.0f;
					outcolor.x = planes[planei].color.x * color;
					outcolor.y = planes[planei].color.y * color;
					outcolor.z = planes[planei].color.z * color;
				}
			}
		}
	}
	return outcolor;
}

typedef struct {
	struct vec3 minbound;
	struct vec3 maxbound;
} AABB;

	
// https://medium.com/@bromanz/another-view-on-the-classic-ray-aabb-intersection-algorithm-for-bvh-traversal-41125138b525
bool boxintersect(AABB aabb, struct Ray ray, float tmin, float tmax, float* tminout) {
	struct vec3 invD = vecrcp(ray.direction);
	struct vec3 t0s = vecmul(vecsub(aabb.minbound, ray.origin), invD);
	struct vec3 t1s = vecmul(vecsub(aabb.maxbound, ray.origin), invD);
	
	struct vec3 tsmaller = vecmin(t0s, t1s);
	struct vec3 tbigger  = vecmax(t0s, t1s);

	float tmin3 = fmaxf(tmin, fmaxf(tsmaller.x, fmaxf(tsmaller.y, tsmaller.z)));
	float tmax3 = fminf(tmax, fminf(tbigger.x, fminf(tbigger.y, tbigger.z)));
	*tminout = tmin3;

	return (tmin3 < tmax3);
}

void upscale(unsigned short* colorbuffer, unsigned short* downscalebuffer, int downscalefactor) {
	const int width = 128;
	const int height = 128;
	for (int j = 0; j < width; ++j) {
        for (int i = 0; i < height; ++i) {
			int downscalei = i / downscalefactor;
			int downscalej = j / downscalefactor;
			unsigned short color565 = downscalebuffer[downscalej * (height/downscalefactor) + downscalei];
			colorbuffer[j * height + i] = color565;
		}
	}
}

void blur(unsigned short* colorbuffer, unsigned short* downscalebuffer) {
	const int width = 128;
	const int height = 128;
	for (int j = 1; j < width-1; ++j) {
        for (int i = 1; i < height-1; ++i) {
			unsigned short color565TL = downscalebuffer[(j/2 + 0) * 64 + i/2 + 0];
			unsigned short color565TR = downscalebuffer[(j/2 + 0) * 64 + i/2 + 1];
			unsigned short color565BL = downscalebuffer[(j/2 + 1) * 64 + i/2 + 0];
			unsigned short color565BR = downscalebuffer[(j/2 + 1) * 64 + i/2 + 1];
			
			float bint = color565TL & 0x1f;
			float gint = (color565TL >> 5) & 0x3f;
			float rint = (color565TL >> 11) & 0x1f;
			bint += color565TR & 0x1f;
			gint += (color565TR >> 5) & 0x3f;
			rint += (color565TR >> 11) & 0x1f;
			bint += color565BL & 0x1f;
			gint += (color565BL >> 5) & 0x3f;
			rint += (color565BL >> 11) & 0x1f;
			bint += color565BR & 0x1f;
			gint += (color565BR >> 5) & 0x3f;
			rint += (color565BR >> 11) & 0x1f;
			bint /= 4.0f;
			gint /= 4.0f;
			rint /= 4.0f;
			
			int color565 = ((int)rint << 11 | (int)gint << 5 | (int)bint);
			colorbuffer[j * height + i] = color565;
		}
	}
}

#define RENDER_UPSCALE 1

void raytracescene(
	unsigned short* colorbuffer,
	unsigned short* scratchbuffer,
	float* alights,
	int numlights,
	float* aspheres,
	int numspheres,
	float* aboxes,
	int numboxes,
	float* aplanes,
	int numplanes,
	float* renderproperties){
	
	#if RENDER_UPSCALE
	int downscalefactor = 2;
	#else
	int downscalefactor = 1;
	#endif
	const int width = 128/downscalefactor;
	const int height = 128/downscalefactor;
	
	const int starty = height - 1;
    const int endy = 0;
	const float diffuse = 0.25f;
	const float shadowdiffuse = 0.25f;
	const float visbias = 0.001f;
	const float maxt = 10.0f;
	const float lightalpha = 0.8f;
	const float oneminusa = 1.0f - lightalpha;
	
	const struct Sphere* lights = (const struct Sphere*)((void*)alights);
	const struct Sphere* spheres = (const struct Sphere*)((void*)aspheres);
	const AABB* boxes = (const AABB*)((void*)aboxes);
	const struct Plane* planes = (const struct Plane*)((void*)aplanes);
		
	for (int j = starty; j >= endy; --j) {
        for (int i = 0; i < width; ++i) {
            float u = (float)i / (float)width;
            float v = (float)j / (float)height;
            struct Ray ray = MakeRay(
                vec3(
                    0.0f,
                    0.0f,
                    0.0f),
                vec3(
                    2.0f * u - 1.0f,
                    2.0f * v - 1.0f,
                    -1.0f));
		    struct vec3 rgbcolor;
			rgbcolor.x = 0;
			rgbcolor.y = 0;
			rgbcolor.z = 0;
            float t = 0.0f;
            float mint = 1000.0f;
			
			numlights = 1;
			for (int lighti = 0; lighti < numlights; ++lighti) {
				if (intersect(lights[lighti], ray, &t)) {
					if (t < mint) {
						mint = t;
						rgbcolor.x = lights[lighti].color.x;
						rgbcolor.y = lights[lighti].color.y;
						rgbcolor.z = lights[lighti].color.z;
					}
				}
			}
			for (int spherei = 0; spherei < numspheres; ++spherei) {
				t = 0.0f;
				if (intersect(spheres[spherei], ray, &t)) {
					if (t < mint) {
						rgbcolor.x = 0.0f;
						rgbcolor.y = 0.0f;
						rgbcolor.z = 0.0f;
						mint = t;
						struct vec3 point = point_at_parameter(ray, t);
						struct vec3 normal = getnormalized(vecsub(spheres[spherei].center, point));
						for (int lighti = 0; lighti < numlights; ++lighti) {
							struct vec3 lightpos = lights[lighti].center;
							struct vec3 lightvec = vecneg(getnormalized(vecsub(lightpos, point)));
							float length = sqrtf(dot(vecsub(point, lightpos), vecsub(point, lightpos)));
							struct Ray visray = MakeRay(vecadd(point, vecmuls(normal, visbias)), lightvec);
							
							float reflectdiffuse = 0.0f;
							struct vec3 spherecolor =  spheres[spherei].color;
							if(spherecolor.x < 0.0f) {
								struct Ray reflectray = MakeRay(vecadd(point, vecmuls(normal, visbias)), reflect(ray.direction, normal));
								spherecolor = reflecttrace2(
									reflectray, 
									spheres, 
									numspheres, 
									spherei,
									planes, 
									numplanes,
									-1,
									lightpos);
							}
							
							if (visiblecheck2(visray, spheres, numspheres, spherei, length)) {
								float color = (dot(lightvec, normal) + 1.0f) / 2.0f;
								color *= 1.0f / (length * length);
								color += diffuse + reflectdiffuse;
								color = fminf(1.2f, color);
								//color = 1.0f;
								rgbcolor.x += (spherecolor.x * lightalpha + oneminusa * lights[lighti].color.x) * color;
								rgbcolor.y += (spherecolor.y * lightalpha + oneminusa * lights[lighti].color.y) * color;
								rgbcolor.z += (spherecolor.z * lightalpha + oneminusa * lights[lighti].color.z) * color;
							}
							else {
								rgbcolor.x += spherecolor.x * (shadowdiffuse + reflectdiffuse);
								rgbcolor.y += spherecolor.y * (shadowdiffuse + reflectdiffuse);
								rgbcolor.z += spherecolor.z * (shadowdiffuse + reflectdiffuse);
							}
						}
					}
				}
			}
			for (int boxi = 0; boxi < numboxes; ++boxi) {
				continue;
				t = 0.0f;
				if (boxintersect(boxes[boxi], ray, 0.0f, maxt, &t)) {
					if (t < mint) {
						rgbcolor.x = 0.0f;
						rgbcolor.y = 0.0f;
						rgbcolor.z = 0.0f;
						mint = t;
						struct vec3 point = point_at_parameter(ray, t);
						struct vec3 normal = vec3(0.0f, 0.0f, -1.0f);
						for (int lighti = 0; lighti < numlights; ++lighti) {
							struct vec3 lightpos = lights[lighti].center;
							struct vec3 lightvec = vecneg(getnormalized(vecsub(lightpos, point)));
							float length = sqrtf(dot(vecsub(point, lightpos), vecsub(point, lightpos)));
							struct Ray visray = MakeRay(vecadd(point, vecmuls(normal, visbias)), vecneg(lightvec));
							if (visiblecheck2(visray, spheres, numspheres, -1, length)) {
								float color = (dot(lightvec, normal) + 1.0f) / 2.0f;
								color *= 1.0f / (length * length);
								color += diffuse;
								color = fminf(1.0f, color);
								rgbcolor.x += 128.0f * color;
								rgbcolor.y += 128.0f * color;
								rgbcolor.z += 128.0f * color;
							}
							else {
								rgbcolor.x += 128.0f * shadowdiffuse;
								rgbcolor.y += 128.0f * shadowdiffuse;
								rgbcolor.z += 128.0f * shadowdiffuse;
							}
						}
					}
				}
			}
			for (int planei = 0; planei < numplanes; ++planei) {
                if (intersectp(planes[planei], ray, &t)) {
                    if (t < mint) {
						rgbcolor.x = 0.0f;
						rgbcolor.y = 0.0f;
						rgbcolor.z = 0.0f;
						mint = t;
                        struct vec3 point = point_at_parameter(ray, t);
                        struct vec3 normal = planes[planei].normal;
                        for (int lighti = 0; lighti < numlights; ++lighti) {
							struct vec3 lightpos = lights[lighti].center;
							struct vec3 lightvec = vecneg(getnormalized(vecsub(lightpos, point)));
							float length = sqrtf(dot(vecsub(point, lightpos), vecsub(point, lightpos)));
							struct Ray visray = MakeRay(vecadd(point, vecmuls(normal, visbias)), vecneg(lightvec));
							
							struct vec3 planecolor =  planes[planei].color;
							if(planecolor.x < 0.0f) {
								struct Ray reflectray = MakeRay(vecadd(point, vecmuls(normal, visbias)), reflect(ray.direction, normal));
								planecolor = reflecttrace2(
									reflectray, 
									spheres, 
									numspheres, 
									-1,
									planes, 
									numplanes,
									planei,
									lightpos);
							}
							
							if (visiblecheck2(visray, spheres, numspheres, -1, length)) {
								float color = (dot(lightvec, normal) + 1.0f) / 2.0f;
								color *= 1.0f / (length * length);
								color += diffuse;
								color = fminf(1.2f, color);
								//color = 1.0f;
								rgbcolor.x += (planecolor.x * lightalpha + oneminusa * planecolor.x) * color;
								rgbcolor.y += (planecolor.y * lightalpha + oneminusa * planecolor.y) * color;
								rgbcolor.z += (planecolor.z * lightalpha + oneminusa * planecolor.z) * color;
							}
							else {
								rgbcolor.x += planecolor.x * shadowdiffuse;
								rgbcolor.y += planecolor.y * shadowdiffuse;
								rgbcolor.z += planecolor.z * shadowdiffuse;
							}
						}
                    }
                }
            }
			int rint = rgbcolor.x * (31.0f / 255.0f);
			int gint = rgbcolor.y * (63.0f / 255.0f);
			int bint = rgbcolor.z * (31.0f / 255.0f);
			int color565 = (rint << 11 | gint << 5 | bint);
			#if RENDER_UPSCALE
			scratchbuffer[(height - j - 1) * height + i] = color565;
			#else
			colorbuffer[(height - j - 1) * height + i] = color565;
			#endif
		}
	}
	#if RENDER_UPSCALE
	upscale(colorbuffer, scratchbuffer, downscalefactor);
	blur(colorbuffer, scratchbuffer);
	#endif
}



