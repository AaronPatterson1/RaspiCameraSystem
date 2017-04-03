from picamera import PiCamera
from time import sleep
import os
import time

camera = PiCamera()

s = time.strftime("%H:%M:%S") + '_' + time.strftime("%d-%m-%Y") + '-video.h264' #the filename of the clip, including the time and date

camera.start_preview()
camera.start_recording(s)
sleep(5)
camera.stop_recording()
camera.stop_preview()
os.system('echo ' + s + '> newfile.txt')
os.system('sshpass -p "ifidienowiwill543102" scp ./' + s + ' Aaron@169.254.227.33:~/Desktop/ps/Videos/')
os.system('sshpass -p "ifidienowiwill543102" scp newfile.txt Aaron@169.254.227.33:~/Desktop/ps/BTS630/M1Client/M1Client/')
