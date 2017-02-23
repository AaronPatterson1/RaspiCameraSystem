from picamera import PiCamera
from time import sleep
import os
import time

camera = PiCamera()

s = time.strftime("%H:%M:%S") + '-' + time.strftime("%d/%m/%Y") + '-video.h264' #the filename of the clip, including the time and date

camera.start_preview()
camera.start_recording('~/' + s)
sleep(10)
camera.stop_recording()
camera.stop_preview()
os.system('sshpass -p "password123" scp ~/' + s + ' Aaron@192.168.137.1:~/Desktop/ps/BTP600/videos/')