# Copyright (c) 2019-2020 Anton Morozenko
# 宋义深 2021/4/25 Modified
"""
Polling timers for functions.
Registers timers and performs run once or periodical function execution after defined time intervals.
"""
# select.select call used as polling waiter where it is possible
# cause time.sleep sometimes may load CPU up to 100% with small polling wait interval
try:
    # cpython
    import time
    import select

    polling_wait = lambda x: select.select([], [], [], x)
    polling_wait(0.01)
except OSError:
    # windows case where select.select call fails
    polling_wait = lambda x: time.sleep(x)

except ImportError:
    # micropython
    import time

    try:
        from uselect import select as s_select

        polling_wait = lambda x: s_select([], [], [], x)
    except ImportError:
        # case when micropython port does not support select.select
        polling_wait = lambda x: time.sleep(x)

WAIT_SEC = 0.05
MAX_TIMERS = 16
DEFAULT_INTERVAL = 10


class TimerError(Exception):
    pass


class Timer(object):
    timers = {}

    def __init__(self, no_timers_err=True):
        self.no_timers_err = no_timers_err

    def _get_func_name(self, obj):
        """retrieves a suitable name for a function"""
        if hasattr(obj, 'func'):
            # handles nested decorators
            return self._get_func_name(obj.func)
        # simply returns 'timer' if on port without function attrs
        return getattr(obj, '__name__', 'timer')

    def register(blynk, *args, **kwargs):
        # kwargs with defaults are used cause PEP 3102 no supported by Python2
        interval = kwargs.pop('interval', DEFAULT_INTERVAL)
        run_once = kwargs.pop('run_once', False)
        stopped = kwargs.pop('stopped', False)

        class Deco(object):
            def __init__(self, func):
                self.func = func
                if len(list(Timer.timers.keys())) >= MAX_TIMERS:
                    raise TimerError('Max allowed timers num={}'.format(MAX_TIMERS))
                _timer = _Timer(interval, func, run_once, stopped, *args, **kwargs)
                Timer.timers['{}_{}'.format(len(list(Timer.timers.keys())), blynk._get_func_name(func))] = _timer

            def __call__(self, *f_args, **f_kwargs):
                return self.func(*f_args, **f_kwargs)

        return Deco

    @staticmethod
    def stop(t_id):
        timer = Timer.timers.get(t_id, None)
        if timer is None:
            raise TimerError('Timer id={} not found'.format(t_id))
        Timer.timers[t_id].stopped = True

    @staticmethod
    def start(t_id):
        timer = Timer.timers.get(t_id, None)
        if timer is None:
            raise TimerError('Timer id={} not found'.format(t_id))
        Timer.timers[t_id].stopped = False
        Timer.timers[t_id].fire_time = None
        Timer.timers[t_id].fire_time_prev = None

    @staticmethod
    def is_stopped(t_id):
        timer = Timer.timers.get(t_id, None)
        if timer is None:
            raise TimerError('Timer id={} not found'.format(t_id))
        return timer.stopped

    def get_timers(self):
        states = {True: 'Stopped', False: 'Running'}
        return {k: states[v.stopped] for k, v in self.timers.items()}

    def run(self):
        polling_wait(WAIT_SEC)
        timers_intervals = [curr_timer.run() for curr_timer in Timer.timers.values() if not curr_timer.stopped]
        if not timers_intervals and self.no_timers_err:
            raise TimerError('Running timers not found')
        return timers_intervals


class _Timer(object):
    def __init__(self, interval, deco, run_once, stopped, *args, **kwargs):
        self.interval = interval
        self.deco = deco
        self.args = args
        self.run_once = run_once
        self.kwargs = kwargs
        self.fire_time = None
        self.fire_time_prev = None
        self.stopped = stopped

    def run(self):
        timer_real_interval = 0
        if self.fire_time is None:
            self.fire_time = time.time() + self.interval
        if self.fire_time_prev is None:
            self.fire_time_prev = time.time()
        curr_time = time.time()
        if curr_time >= self.fire_time:
            self.deco(*self.args, **self.kwargs)
            if self.run_once:
                self.stopped = True
            timer_real_interval = curr_time - self.fire_time_prev
            self.fire_time_prev = self.fire_time
            self.fire_time = curr_time + self.interval
        return timer_real_interval
