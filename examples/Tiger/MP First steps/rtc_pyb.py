from pyb import RTC
import utime


local = utime.localtime()

datetime = local[0:6] + (0,0)

rtc = RTC()
rtc.datetime(datetime)



weekdays = ['Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday']
months = ['January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'October', 'November', 'December']


time = [0,0,0,0,0,0,0,0]
day = 0
month = months[0]
year = 0
weekday = weekdays[0]
hour = 0
minute = 0
second = 0
subseconds = 0
ordinal = 'th'

def callback(parameter):
    global rtc, datetime, time,day,month,year,weekday,hour,minute,second,subseconds,ordinal
    time = rtc.datetime()
    day = str(time[2])
    month = months[time[1]]
    year = str(time[0])
    weekday = weekdays[time[3]]
    hour = str(time[4])
    minute = str(time[5])
    second = str(time[6])
    subseconds = str(time[7])
    ordinal = 'th'
    if day == '1' or (len(day) == 2 and day[1] == '1'):
        ordinal = 'st'
    elif day == '2' or (len(day) == 2 and day[1] == '2'):
        ordinal = 'nd'
    elif day == '3' or (len(day) == 2 and day[1] == '3'):
        ordinal = 'rd'
    else:
        ordinal = 'th'

    print(weekday + ', ' + month + ' ' + day + ordinal + ', ' + year + '. ' + hour + ':' + minute + ':' + second + '.' + subseconds )

rtc.wakeup(1000, callback)