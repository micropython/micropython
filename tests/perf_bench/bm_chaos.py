# Source: https://github.com/python/pyperformance
# License: MIT

# create chaosgame-like fractals
# Copyright (C) 2005 Carl Friedrich Bolz

import math
import random


class GVector(object):

    def __init__(self, x=0, y=0, z=0):
        self.x = x
        self.y = y
        self.z = z

    def Mag(self):
        return math.sqrt(self.x ** 2 + self.y ** 2 + self.z ** 2)

    def dist(self, other):
        return math.sqrt((self.x - other.x) ** 2
                         + (self.y - other.y) ** 2
                         + (self.z - other.z) ** 2)

    def __add__(self, other):
        if not isinstance(other, GVector):
            raise ValueError("Can't add GVector to " + str(type(other)))
        v = GVector(self.x + other.x, self.y + other.y, self.z + other.z)
        return v

    def __sub__(self, other):
        return self + other * -1

    def __mul__(self, other):
        v = GVector(self.x * other, self.y * other, self.z * other)
        return v
    __rmul__ = __mul__

    def linear_combination(self, other, l1, l2=None):
        if l2 is None:
            l2 = 1 - l1
        v = GVector(self.x * l1 + other.x * l2,
                    self.y * l1 + other.y * l2,
                    self.z * l1 + other.z * l2)
        return v

    def __str__(self):
        return "<%f, %f, %f>" % (self.x, self.y, self.z)

    def __repr__(self):
        return "GVector(%f, %f, %f)" % (self.x, self.y, self.z)


class Spline(object):
    """Class for representing B-Splines and NURBS of arbitrary degree"""

    def __init__(self, points, degree, knots):
        """Creates a Spline.

        points is a list of GVector, degree is the degree of the Spline.
        """
        if len(points) > len(knots) - degree + 1:
            raise ValueError("too many control points")
        elif len(points) < len(knots) - degree + 1:
            raise ValueError("not enough control points")
        last = knots[0]
        for cur in knots[1:]:
            if cur < last:
                raise ValueError("knots not strictly increasing")
            last = cur
        self.knots = knots
        self.points = points
        self.degree = degree

    def GetDomain(self):
        """Returns the domain of the B-Spline"""
        return (self.knots[self.degree - 1],
                self.knots[len(self.knots) - self.degree])

    def __call__(self, u):
        """Calculates a point of the B-Spline using de Boors Algorithm"""
        dom = self.GetDomain()
        if u < dom[0] or u > dom[1]:
            raise ValueError("Function value not in domain")
        if u == dom[0]:
            return self.points[0]
        if u == dom[1]:
            return self.points[-1]
        I = self.GetIndex(u)
        d = [self.points[I - self.degree + 1 + ii]
             for ii in range(self.degree + 1)]
        U = self.knots
        for ik in range(1, self.degree + 1):
            for ii in range(I - self.degree + ik + 1, I + 2):
                ua = U[ii + self.degree - ik]
                ub = U[ii - 1]
                co1 = (ua - u) / (ua - ub)
                co2 = (u - ub) / (ua - ub)
                index = ii - I + self.degree - ik - 1
                d[index] = d[index].linear_combination(d[index + 1], co1, co2)
        return d[0]

    def GetIndex(self, u):
        dom = self.GetDomain()
        for ii in range(self.degree - 1, len(self.knots) - self.degree):
            if u >= self.knots[ii] and u < self.knots[ii + 1]:
                I = ii
                break
        else:
            I = dom[1] - 1
        return I

    def __len__(self):
        return len(self.points)

    def __repr__(self):
        return "Spline(%r, %r, %r)" % (self.points, self.degree, self.knots)


def write_ppm(im, w, h, filename):
    with open(filename, "wb") as f:
        f.write(b'P6\n%i %i\n255\n' % (w, h))
        for j in range(h):
            for i in range(w):
                val = im[j * w + i]
                c = val * 255
                f.write(b'%c%c%c' % (c, c, c))


class Chaosgame(object):

    def __init__(self, splines, thickness, subdivs):
        self.splines = splines
        self.thickness = thickness
        self.minx = min([p.x for spl in splines for p in spl.points])
        self.miny = min([p.y for spl in splines for p in spl.points])
        self.maxx = max([p.x for spl in splines for p in spl.points])
        self.maxy = max([p.y for spl in splines for p in spl.points])
        self.height = self.maxy - self.miny
        self.width = self.maxx - self.minx
        self.num_trafos = []
        maxlength = thickness * self.width / self.height
        for spl in splines:
            length = 0
            curr = spl(0)
            for i in range(1, subdivs + 1):
                last = curr
                t = 1 / subdivs * i
                curr = spl(t)
                length += curr.dist(last)
            self.num_trafos.append(max(1, int(length / maxlength * 1.5)))
        self.num_total = sum(self.num_trafos)

    def get_random_trafo(self):
        r = random.randrange(int(self.num_total) + 1)
        l = 0
        for i in range(len(self.num_trafos)):
            if r >= l and r < l + self.num_trafos[i]:
                return i, random.randrange(self.num_trafos[i])
            l += self.num_trafos[i]
        return len(self.num_trafos) - 1, random.randrange(self.num_trafos[-1])

    def transform_point(self, point, trafo=None):
        x = (point.x - self.minx) / self.width
        y = (point.y - self.miny) / self.height
        if trafo is None:
            trafo = self.get_random_trafo()
        start, end = self.splines[trafo[0]].GetDomain()
        length = end - start
        seg_length = length / self.num_trafos[trafo[0]]
        t = start + seg_length * trafo[1] + seg_length * x
        basepoint = self.splines[trafo[0]](t)
        if t + 1 / 50000 > end:
            neighbour = self.splines[trafo[0]](t - 1 / 50000)
            derivative = neighbour - basepoint
        else:
            neighbour = self.splines[trafo[0]](t + 1 / 50000)
            derivative = basepoint - neighbour
        if derivative.Mag() != 0:
            basepoint.x += derivative.y / derivative.Mag() * (y - 0.5) * \
                self.thickness
            basepoint.y += -derivative.x / derivative.Mag() * (y - 0.5) * \
                self.thickness
        else:
            # can happen, especially with single precision float
            pass
        self.truncate(basepoint)
        return basepoint

    def truncate(self, point):
        if point.x >= self.maxx:
            point.x = self.maxx
        if point.y >= self.maxy:
            point.y = self.maxy
        if point.x < self.minx:
            point.x = self.minx
        if point.y < self.miny:
            point.y = self.miny

    def create_image_chaos(self, w, h, iterations, rng_seed):
        # Always use the same sequence of random numbers
        # to get reproductible benchmark
        random.seed(rng_seed)

        im = bytearray(w * h)
        point = GVector((self.maxx + self.minx) / 2,
                        (self.maxy + self.miny) / 2, 0)
        for _ in range(iterations):
            point = self.transform_point(point)
            x = (point.x - self.minx) / self.width * w
            y = (point.y - self.miny) / self.height * h
            x = int(x)
            y = int(y)
            if x == w:
                x -= 1
            if y == h:
                y -= 1
            im[(h - y - 1) * w + x] = 1

        return im


###########################################################################
# Benchmark interface

bm_params = {
    (100, 50): (0.25, 100, 50, 50, 50, 1234),
    (1000, 1000): (0.25, 200, 400, 400, 1000, 1234),
    (5000, 1000): (0.25, 400, 500, 500, 7000, 1234),
}

def bm_setup(params):
    splines = [
        Spline([
            GVector(1.597, 3.304, 0.0),
            GVector(1.576, 4.123, 0.0),
            GVector(1.313, 5.288, 0.0),
            GVector(1.619, 5.330, 0.0),
            GVector(2.890, 5.503, 0.0),
            GVector(2.373, 4.382, 0.0),
            GVector(1.662, 4.360, 0.0)],
            3, [0, 0, 0, 1, 1, 1, 2, 2, 2]),
        Spline([
            GVector(2.805, 4.017, 0.0),
            GVector(2.551, 3.525, 0.0),
            GVector(1.979, 2.620, 0.0),
            GVector(1.979, 2.620, 0.0)],
            3, [0, 0, 0, 1, 1, 1]),
        Spline([
            GVector(2.002, 4.011, 0.0),
            GVector(2.335, 3.313, 0.0),
            GVector(2.367, 3.233, 0.0),
            GVector(2.367, 3.233, 0.0)],
            3, [0, 0, 0, 1, 1, 1])
    ]

    chaos = Chaosgame(splines, params[0], params[1])
    image = None

    def run():
        nonlocal image
        _, _, width, height, iter, rng_seed = params
        image = chaos.create_image_chaos(width, height, iter, rng_seed)

    def result():
        norm = params[4]
        # Images are not the same when floating point behaviour is different,
        # so return percentage of pixels that are set (rounded to int).
        #write_ppm(image, params[2], params[3], 'out-.ppm')
        pix = int(100 * sum(image) / len(image))
        return norm, pix

    return run, result
