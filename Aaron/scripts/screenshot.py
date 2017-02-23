from picamera import PiCamera
from time import sleep
import os
import time

camera = PiCamera()

s = time.strftime("%H:%M:%S") + '-' + time.strftime("%d/%m/%Y") + '-img.jpg' #filename of the image with date and time

camera.start_preview()
camera.capture('~/' + s)
camera.stop_preview()
os.system('sshpass -p "password123" scp ~/' + s + ' Aaron@192.168.137.1:~/Desktop/ps/BTP600/videos/')