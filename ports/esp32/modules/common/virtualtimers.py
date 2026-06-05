# File: virtualtimers.py
# Version: 1
# Description: Uses one hardware timer to simulate timers
# License: MIT
# Authors: Renze Nicolai <renze@rnplus.nl>

import machine, sys

scheduler = []
timer = machine.Timer(1)
period = 0
debugEnabled = False

# Start the virtual timers scheduler
def begin(p=100):
    global timer
    global period
    if p<1:
        return
    period = p
    timer.init(period=p, mode=machine.Timer.PERIODIC, callback=timer_callback)

# Start the virtual timers scheduler (obselete)
def activate(p=100):
    begin(p)

# Stop the virtual timers scheduler
def stop():
    global timer, scheduler
    timer.deinit()
    scheduler = []

# Print the task list
def debug():
    global scheduler
    for i in range(0, len(scheduler)):
        timeUntilTaskExecution = scheduler[i]['target']-scheduler[i]['pos']
        print("idle time for task "+str(i)+" = "+str(timeUntilTaskExecution)+" - ",scheduler[i])

# Add a task to the scheduler
def new(target, callback, hfpm=False):
    ''' Creates new task. Arguments: time until callback is called, callback, hide from power management '''
    global scheduler
    item = {"pos":0, "target":target, "cb":callback, "hfpm":hfpm}
    scheduler.append(item)

# Remove a task from the scheduler
def delete(callback):
    global scheduler
    found = False
    for i in range(0, len(scheduler)):
        if (scheduler[i]["cb"]==callback):
            found = True
            break
    scheduler = list(task for task in scheduler if task['cb']!=callback)
    return found

# Change the time until the next execution of a task
def update(target, callback):
    global scheduler
    found = False
    for i in range(0, len(scheduler)):
        if (scheduler[i]["cb"]==callback):
            scheduler[i]["pos"] = 0
            scheduler[i]["target"] = target
            found = True
    return found

# Return the time left until the next task gets executed
def idle_time():
    ''' Returns time until next task in ms, ignores tasks hidden from power management '''
    global scheduler
    idleTime = 86400000 # One day (causes the badge to sleep forever)
    for i in range(0, len(scheduler)):
        timeUntilTaskExecution = scheduler[i]['target']-scheduler[i]['pos']
        
        global debugEnabled
        if debugEnabled:
            print("idle time for task "+str(i)+" = "+str(timeUntilTaskExecution)+" - ",scheduler[i])
            
        if not scheduler[i]["hfpm"]:
            if timeUntilTaskExecution<0:
                timeUntilTaskExecution = 0
            if timeUntilTaskExecution<idleTime:
                idleTime = timeUntilTaskExecution
    return idleTime

# Return the time left until the next hidden task gets executed
def pm_time():
    ''' Returns time until next pm task in ms '''
    global scheduler
    idleTime = 86400000 # One day (causes the badge to sleep forever)
    for i in range(0, len(scheduler)):
        if scheduler[i]["hfpm"]:
            timeUntilTaskExecution = scheduler[i]['target']-scheduler[i]['pos']
            if timeUntilTaskExecution<idleTime:
                idleTime = timeUntilTaskExecution
    return idleTime
    
# Internal function: timer callback
def timer_callback(tmr):
    global scheduler
    global period    
    s = len(scheduler)
    for i in range(0, len(scheduler)):
        scheduler[i]["pos"] += period
        if scheduler[i]["pos"] > scheduler[i]["target"]:
            try:
                newTarget = scheduler[i]["cb"]()
            except BaseException as e:
                sys.print_exception(e)
                newTarget = -1
            if newTarget > 0:
                scheduler[i]["pos"] = 0
                scheduler[i]["target"] = newTarget
            else:
                scheduler[i]["pos"] = -1
                scheduler[i]["target"] = -1
    scheduler = list(task for task in scheduler if task["pos"]>=0)
