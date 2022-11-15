# evolve the RGEs of the standard model from electroweak scale up
# by dpgeorge

import math


class RungeKutta(object):
    def __init__(self, functions, initConditions, t0, dh, save=True):
        self.Trajectory, self.save = [[t0] + initConditions], save
        self.functions = [lambda *args: 1.0] + list(functions)
        self.N, self.dh = len(self.functions), dh
        self.coeff = [1.0 / 6.0, 2.0 / 6.0, 2.0 / 6.0, 1.0 / 6.0]
        self.InArgCoeff = [0.0, 0.5, 0.5, 1.0]

    def iterate(self):
        step = self.Trajectory[-1][:]
        istep, iac = step[:], self.InArgCoeff
        k, ktmp = self.N * [0.0], self.N * [0.0]
        for ic, c in enumerate(self.coeff):
            for if_, f in enumerate(self.functions):
                arguments = [(x + k[i] * iac[ic]) for i, x in enumerate(istep)]
                try:
                    feval = f(*arguments)
                except OverflowError:
                    return False
                if abs(feval) > 1e2:  # stop integrating
                    return False
                ktmp[if_] = self.dh * feval
            k = ktmp[:]
            step = [s + c * k[ik] for ik, s in enumerate(step)]
        if self.save:
            self.Trajectory += [step]
        else:
            self.Trajectory = [step]
        return True

    def solve(self, finishtime):
        while self.Trajectory[-1][0] < finishtime:
            if not self.iterate():
                break

    def solveNSteps(self, nSteps):
        for i in range(nSteps):
            if not self.iterate():
                break

    def series(self):
        return zip(*self.Trajectory)


# 1-loop RGES for the main parameters of the SM
# couplings are: g1, g2, g3 of U(1), SU(2), SU(3); yt (top Yukawa), lambda (Higgs quartic)
# see arxiv.org/abs/0812.4950, eqs 10-15
sysSM = (
    lambda *a: 41.0 / 96.0 / math.pi**2 * a[1] ** 3,  # g1
    lambda *a: -19.0 / 96.0 / math.pi**2 * a[2] ** 3,  # g2
    lambda *a: -42.0 / 96.0 / math.pi**2 * a[3] ** 3,  # g3
    lambda *a: 1.0
    / 16.0
    / math.pi**2
    * (
        9.0 / 2.0 * a[4] ** 3
        - 8.0 * a[3] ** 2 * a[4]
        - 9.0 / 4.0 * a[2] ** 2 * a[4]
        - 17.0 / 12.0 * a[1] ** 2 * a[4]
    ),  # yt
    lambda *a: 1.0
    / 16.0
    / math.pi**2
    * (
        24.0 * a[5] ** 2
        + 12.0 * a[4] ** 2 * a[5]
        - 9.0 * a[5] * (a[2] ** 2 + 1.0 / 3.0 * a[1] ** 2)
        - 6.0 * a[4] ** 4
        + 9.0 / 8.0 * a[2] ** 4
        + 3.0 / 8.0 * a[1] ** 4
        + 3.0 / 4.0 * a[2] ** 2 * a[1] ** 2
    ),  # lambda
)


def drange(start, stop, step):
    r = start
    while r < stop:
        yield r
        r += step


def phaseDiagram(system, trajStart, trajPlot, h=0.1, tend=1.0, range=1.0):
    tstart = 0.0
    for i in drange(0, range, 0.1 * range):
        for j in drange(0, range, 0.1 * range):
            rk = RungeKutta(system, trajStart(i, j), tstart, h)
            rk.solve(tend)
            # draw the line
            for tr in rk.Trajectory:
                x, y = trajPlot(tr)
                print(x, y)
            print()
            # draw the arrow
            continue
            l = (len(rk.Trajectory) - 1) / 3
            if l > 0 and 2 * l < len(rk.Trajectory):
                p1 = rk.Trajectory[l]
                p2 = rk.Trajectory[2 * l]
                x1, y1 = trajPlot(p1)
                x2, y2 = trajPlot(p2)
                dx = -0.5 * (y2 - y1)  # orthogonal to line
                dy = 0.5 * (x2 - x1)  # orthogonal to line
                # l = math.sqrt(dx*dx + dy*dy)
                # if abs(l) > 1e-3:
                #    l = 0.1 / l
                #    dx *= l
                #    dy *= l
                print(x1 + dx, y1 + dy)
                print(x2, y2)
                print(x1 - dx, y1 - dy)
                print()


def singleTraj(system, trajStart, h=0.02, tend=1.0):
    tstart = 0.0

    # compute the trajectory

    rk = RungeKutta(system, trajStart, tstart, h)
    rk.solve(tend)

    # print out trajectory

    for i in range(len(rk.Trajectory)):
        tr = rk.Trajectory[i]
        print(" ".join(["{:.4f}".format(t) for t in tr]))


# phaseDiagram(sysSM, (lambda i, j: [0.354, 0.654, 1.278, 0.8 + 0.2 * i, 0.1 + 0.1 * j]), (lambda a: (a[4], a[5])), h=0.1, tend=math.log(10**17))

# initial conditions at M_Z
singleTraj(
    sysSM, [0.354, 0.654, 1.278, 0.983, 0.131], h=0.5, tend=math.log(10**17)
)  # true values
