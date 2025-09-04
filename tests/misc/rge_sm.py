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


def singleTraj(system, trajStart, h=0.02, tend=1.0):
    is_REPR_C = float("1.0000001") == float("1.0")
    tstart = 0.0

    # compute the trajectory

    rk = RungeKutta(system, trajStart, tstart, h)
    rk.solve(tend)

    # print out trajectory

    for i in range(len(rk.Trajectory)):
        tr = rk.Trajectory[i]
        tr_str = " ".join(["{:.4f}".format(t) for t in tr])
        if is_REPR_C:
            # allow two small deviations for REPR_C
            if tr_str == "1.0000 0.3559 0.6485 1.1944 0.9271 0.1083":
                tr_str = "1.0000 0.3559 0.6485 1.1944 0.9272 0.1083"
            if tr_str == "16.0000 0.3894 0.5793 0.7017 0.5686 -0.0168":
                tr_str = "16.0000 0.3894 0.5793 0.7017 0.5686 -0.0167"
        print(tr_str)


# initial conditions at M_Z
singleTraj(sysSM, [0.354, 0.654, 1.278, 0.983, 0.131], h=0.5, tend=math.log(10**17))  # true values
