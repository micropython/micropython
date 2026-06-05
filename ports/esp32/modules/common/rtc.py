import machine, time

# Functions
def string(print_date=False, print_time=True, timestamp = -1):
    if timestamp<0:
        (year, month, mday, hour, minute, second, wday, yday) = time.localtime()
    else:
        (year, month, mday, hour, minute, second, wday, yday) = time.localtime(timestamp)
    #mday: day in month, wday: day in week, yday: day in year
    monthstr = str(month)
    if (month<10):
      monthstr = "0"+monthstr
    daystr = str(mday)
    if (mday<10):
      daystr = "0"+daystr
    hourstr = str(hour)
    if (hour<10):
      hourstr = "0"+hourstr
    minstr = str(minute)
    if (minute<10):
      minstr = "0"+minstr 
    output = ""
    if print_date:
        output += daystr+"-"+monthstr+"-"+str(year)
        if time:
            output += " "
    if print_time:
        output += hourstr+":"+minstr
    return output

def configure():
	import wifi
	return wifi.ntp()

def isSet():
	return time.time() > 1482192000
