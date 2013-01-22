import sys
import time
sys.path.append("../..")
import arduino
from ctypes import cdll
from bitarray import bitarray
libArduino = cdll.LoadLibrary('./libarduino.so')

class ArduinoController(object):
    def __init__(self,ard):
        self.obj = libArduino.ArduinoController_new()
	self.data = str(bytearray(4*10))
	self.ard = ard

    def process(self,gyro,desired):
	self.ard.notify()
        libArduino.ArduinoController_process(self.obj,self.data,gyro,desired)


#Initialize Pygame
import pygame.camera
import pygame.image
pygame.camera.init()
cams = pygame.camera.list_cameras()
cam = pygame.camera.Camera(cams[1],(320,240))
cam.start()
cam.get_image()
frames = 0
prev = time.time()
screen = pygame.display.set_mode((320,240))


#Initialize ARDUINO
ard = arduino.Arduino()
m0 = arduino.Motor(ard,0,2,11)
m1 = arduino.Motor(ard,0,3,12)
imumu = arduino.IMU(ard)
ard.run()
commArd = ArduinoController(ard)

headings = [-110,-180-40,-100]
timers = [7.8,1.8,3]
RECKON = time.time()
ptr = 0

#Game timer
ENDTIME = time.time()+3*60


while ( time.time()<ENDTIME ):
	#Image Capture
	img = cam.get_image()
	data = pygame.image.tostring(img,"RGBX")

	#Image processing	
	
	#Arduino Interface
	gyro = imumu.getRawValues()[0]

	if ( time.time()-RECKON > timers[ptr] ):
		ptr+=1
		RECKON = time.time()
		if ( ptr >= len(timers) ):
			break

	commArd.process(gyro,headings[ptr])
	commData = commArd.data
	leftD = ord(commData[3])-1
	rightD = ord(commData[1])-1
	m0.setSpeed(leftD*ord(commData[2]))
	m1.setSpeed(rightD*ord(commData[0]))
	#print str(gyro) + " --- " + str(leftD*ord(commData[2])) +", " + str(rightD*ord(commData[0]))

	#Pygame output
	img = pygame.image.fromstring(data,(320,240),"RGBX")
	rect = img.get_rect()
	screen.blit(img,rect)

	#FPS
	pygame.display.update()
	frames+=1

	if ( time.time()-prev > 1 ):
		print "FPS: " + str(frames)
		frames = 0
		prev = time.time()

m0.setSpeed(0)
m1.setSpeed(0)
cam.stop()
pygame.quit()
