# A simple ray tracer
# MIT license; Copyright (c) 2019 Damien P. George

INF = 1e30
EPS = 1e-6


class Vec:
    def __init__(self, x, y, z):
        self.x, self.y, self.z = x, y, z

    def __neg__(self):
        return Vec(-self.x, -self.y, -self.z)

    def __add__(self, rhs):
        return Vec(self.x + rhs.x, self.y + rhs.y, self.z + rhs.z)

    def __sub__(self, rhs):
        return Vec(self.x - rhs.x, self.y - rhs.y, self.z - rhs.z)

    def __mul__(self, rhs):
        return Vec(self.x * rhs, self.y * rhs, self.z * rhs)

    def length(self):
        return (self.x**2 + self.y**2 + self.z**2) ** 0.5

    def normalise(self):
        l = self.length()
        return Vec(self.x / l, self.y / l, self.z / l)

    def dot(self, rhs):
        return self.x * rhs.x + self.y * rhs.y + self.z * rhs.z


RGB = Vec


class Ray:
    def __init__(self, p, d):
        self.p, self.d = p, d


class View:
    def __init__(self, width, height, depth, pos, xdir, ydir, zdir):
        self.width = width
        self.height = height
        self.depth = depth
        self.pos = pos
        self.xdir = xdir
        self.ydir = ydir
        self.zdir = zdir

    def calc_dir(self, dx, dy):
        return (self.xdir * dx + self.ydir * dy + self.zdir * self.depth).normalise()


class Light:
    def __init__(self, pos, colour, casts_shadows):
        self.pos = pos
        self.colour = colour
        self.casts_shadows = casts_shadows


class Surface:
    def __init__(self, diffuse, specular, spec_idx, reflect, transp, colour):
        self.diffuse = diffuse
        self.specular = specular
        self.spec_idx = spec_idx
        self.reflect = reflect
        self.transp = transp
        self.colour = colour

    @staticmethod
    def dull(colour):
        return Surface(0.7, 0.0, 1, 0.0, 0.0, colour * 0.6)

    @staticmethod
    def shiny(colour):
        return Surface(0.2, 0.9, 32, 0.8, 0.0, colour * 0.3)

    @staticmethod
    def transparent(colour):
        return Surface(0.2, 0.9, 32, 0.0, 0.8, colour * 0.3)


class Sphere:
    def __init__(self, surface, centre, radius):
        self.surface = surface
        self.centre = centre
        self.radsq = radius**2

    def intersect(self, ray):
        v = self.centre - ray.p
        b = v.dot(ray.d)
        det = b**2 - v.dot(v) + self.radsq
        if det > 0:
            det **= 0.5
            t1 = b - det
            if t1 > EPS:
                return t1
            t2 = b + det
            if t2 > EPS:
                return t2
        return INF

    def surface_at(self, v):
        return self.surface, (v - self.centre).normalise()


class Plane:
    def __init__(self, surface, centre, normal):
        self.surface = surface
        self.normal = normal.normalise()
        self.cdotn = centre.dot(normal)

    def intersect(self, ray):
        ddotn = ray.d.dot(self.normal)
        if abs(ddotn) > EPS:
            t = (self.cdotn - ray.p.dot(self.normal)) / ddotn
            if t > 0:
                return t
        return INF

    def surface_at(self, p):
        return self.surface, self.normal


class Scene:
    def __init__(self, ambient, light, objs):
        self.ambient = ambient
        self.light = light
        self.objs = objs


def trace_scene(canvas, view, scene, max_depth):
    for v in range(canvas.height):
        y = (-v + 0.5 * (canvas.height - 1)) * view.height / canvas.height
        for u in range(canvas.width):
            x = (u - 0.5 * (canvas.width - 1)) * view.width / canvas.width
            ray = Ray(view.pos, view.calc_dir(x, y))
            c = trace_ray(scene, ray, max_depth)
            canvas.put_pix(u, v, c)


def trace_ray(scene, ray, depth):
    # Find closest intersecting object
    hit_t = INF
    hit_obj = None
    for obj in scene.objs:
        t = obj.intersect(ray)
        if t < hit_t:
            hit_t = t
            hit_obj = obj

    # Check if any objects hit
    if hit_obj is None:
        return RGB(0, 0, 0)

    # Compute location of ray intersection
    point = ray.p + ray.d * hit_t
    surf, surf_norm = hit_obj.surface_at(point)
    if ray.d.dot(surf_norm) > 0:
        surf_norm = -surf_norm

    # Compute reflected ray
    reflected = ray.d - surf_norm * (surf_norm.dot(ray.d) * 2)

    # Ambient light
    col = surf.colour * scene.ambient

    # Diffuse, specular and shadow from light source
    light_vec = scene.light.pos - point
    light_dist = light_vec.length()
    light_vec = light_vec.normalise()
    ndotl = surf_norm.dot(light_vec)
    ldotv = light_vec.dot(reflected)
    if ndotl > 0 or ldotv > 0:
        light_ray = Ray(point + light_vec * EPS, light_vec)
        light_col = trace_to_light(scene, light_ray, light_dist)
        if ndotl > 0:
            col += light_col * surf.diffuse * ndotl
        if ldotv > 0:
            col += light_col * surf.specular * ldotv**surf.spec_idx

    # Reflections
    if depth > 0 and surf.reflect > 0:
        col += trace_ray(scene, Ray(point + reflected * EPS, reflected), depth - 1) * surf.reflect

    # Transparency
    if depth > 0 and surf.transp > 0:
        col += trace_ray(scene, Ray(point + ray.d * EPS, ray.d), depth - 1) * surf.transp

    return col


def trace_to_light(scene, ray, light_dist):
    col = scene.light.colour
    for obj in scene.objs:
        t = obj.intersect(ray)
        if t < light_dist:
            col *= obj.surface.transp
    return col


class Canvas:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.data = bytearray(3 * width * height)

    def put_pix(self, x, y, c):
        off = 3 * (y * self.width + x)
        self.data[off] = min(255, max(0, int(255 * c.x)))
        self.data[off + 1] = min(255, max(0, int(255 * c.y)))
        self.data[off + 2] = min(255, max(0, int(255 * c.z)))

    def write_ppm(self, filename):
        with open(filename, "wb") as f:
            f.write(bytes("P6 %d %d 255\n" % (self.width, self.height), "ascii"))
            f.write(self.data)


def main(w, h, d):
    canvas = Canvas(w, h)
    view = View(32, 32, 64, Vec(0, 0, 50), Vec(1, 0, 0), Vec(0, 1, 0), Vec(0, 0, -1))
    scene = Scene(
        0.5,
        Light(Vec(0, 8, 0), RGB(1, 1, 1), True),
        [
            Plane(Surface.dull(RGB(1, 0, 0)), Vec(-10, 0, 0), Vec(1, 0, 0)),
            Plane(Surface.dull(RGB(0, 1, 0)), Vec(10, 0, 0), Vec(-1, 0, 0)),
            Plane(Surface.dull(RGB(1, 1, 1)), Vec(0, 0, -10), Vec(0, 0, 1)),
            Plane(Surface.dull(RGB(1, 1, 1)), Vec(0, -10, 0), Vec(0, 1, 0)),
            Plane(Surface.dull(RGB(1, 1, 1)), Vec(0, 10, 0), Vec(0, -1, 0)),
            Sphere(Surface.shiny(RGB(1, 1, 1)), Vec(-5, -4, 3), 4),
            Sphere(Surface.dull(RGB(0, 0, 1)), Vec(4, -5, 0), 4),
            Sphere(Surface.transparent(RGB(0.2, 0.2, 0.2)), Vec(6, -1, 8), 4),
        ],
    )
    trace_scene(canvas, view, scene, d)
    return canvas


# For testing
# main(256, 256, 4).write_ppm('rt.ppm')

###########################################################################
# Benchmark interface

bm_params = {
    (100, 100): (5, 5, 2),
    (1000, 100): (18, 18, 3),
    (5000, 100): (40, 40, 3),
}


def bm_setup(params):
    return lambda: main(*params), lambda: (params[0] * params[1] * params[2], None)
