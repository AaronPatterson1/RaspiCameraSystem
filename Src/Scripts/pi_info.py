#!/usr/bin/python
import os

# Return CPU temperature as a character string
def getCPUtemperature():
    res = os.popen('vcgencmd measure_temp').readline()
    return(res.replace("temp=","").replace("'C\n",""))

def getRAMinfo():
    p = os.popen('free')
    i = 0
    while 1:
        i = i + 1
        line = p.readline()
        if i==2:
            return(line.split()[1:4])

# Return % of CPU used by user as a character string
def getCPUuse():
    return(str(os.popen("top -n1 | awk '/Cpu\(s\):/ {print $2}'").readline().strip(\
)))

def getDiskSpace():
    p = os.popen("df -h /")
    i = 0
    while 1:
        i = i +1
        line = p.readline()
        if i==2:
            return(line.split()[1:5])

# CPU informatiom
CPU_temp = getCPUtemperature()
CPU_usage = getCPUuse()
# RAM information : Output is in kb, here I convert it in Mb for readability
RAM_stats = getRAMinfo()
RAM_total = round(int(RAM_stats[0]) / 1000,1)
RAM_used = round(int(RAM_stats[1]) / 1000,1)
RAM_free = round(int(RAM_stats[2]) / 1000,1)
# Disk information
DISK_stats = getDiskSpace()
DISK_total = DISK_stats[0]
DISK_free = DISK_stats[1]
DISK_perc = DISK_stats[3]

print "CPU temperature : %s" % CPU_temp
print "CPU usage       : %s" % CPU_usage
print "RAM stats       : %s" % RAM_stats
print "RAM total       : %s" % RAM_total
print "RAM used        : %s" % RAM_used
print "Ram free        : %s" % RAM_free
print "DISK stats      : %s" % DISK_stats
print "DISK total      : %s" % DISK_total
print "DISK free       : %s" % DISK_free
print "DISK percent    : %s" % DISK_perc
