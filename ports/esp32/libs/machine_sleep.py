from machine import deepsleep as sleep


def deepsleep(time_s : int):
    print('sleeping for',time_s)
    sleep(time_s* 1000)
