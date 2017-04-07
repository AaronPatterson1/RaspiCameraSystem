from picamera import PiCamera
from time import sleep
import os
import time
import sys

camera = PiCamera()
t = float(sys.argv[1])
resolution = sys.argv[2]
filter = sys.argv[3]
rotation = sys.argv[4]
framerate = sys.argv[5]
brightness = sys.argv[6]

#checking for resolution
if resolution == "1080p":
	camera.resolution = (1920, 1080)
elif resolution == "720p":
	camera.resolution = (1280, 720)
elif resolution == "480p":
	camera.resolution = (640, 480)
elif resolution == "360p":
	camera.resolution = (480, 360)

#checking for effect
if filter == "Black_and_white":
	camera.color_effects = (128,128)
elif filter == "Negative":
	camera.image_effect = 'negative'

#checking for rotation
if rotation == "0":
	camera.rotation = 0
elif rotation == "90":
	camera.rotation = 90
elif rotation == "180":
	camera.rotation = 180
elif rotation == "270":
	camera.rotation = 270

#checking for framerate
if framerate == "15":
	camera.framerate = 15
elif framerate == "24":
	camera.framerate = 24
elif framerate == "28":
	camera.framerate = 28
elif framerate == "30":
	camera.framerate = 30

#checking for brightness
if brightness == "0":
	camera.brightness = 0
elif brightness == "25":
	camera.brightness = 25
elif brightness == "50":
	camera.brightness = 50
elif brightness == "75":
	camera.brightness = 75
elif brightness == "100":
	camera.brightness = 100

s = time.strftime("%H:%M:%S") + '_' + time.strftime("%d-%m-%Y") + '-video.h264' #the filename of the clip, including the time and date

camera.start_preview()
camera.start_recording(s)
sleep(t)
camera.stop_recording()
camera.stop_preview()
os.system('echo ' + s + '> newfile.txt')
os.system('sshpass -p "ifidienowiwill543102" scp ./' + s + ' Aaron@169.254.227.33:~/Desktop/ps/Videos/')
os.system('sshpass -p "ifidienowiwill543102" scp newfile.txt Aaron@169.254.227.33:~/Desktop/ps/BTS630/M1Client/M1Client/')
