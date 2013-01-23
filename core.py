import sys
import time
sys.path.append("../..")
import arduino
from ctypes import cdll
from bitarray import bitarray
libArduino = cdll.LoadLibrary('./libarduino.so')
libImgProc = cdll.LoadLibrary('./libimgproc.so')

class ImageProcessing(object):
    def __init__(self):
        self.obj = libImgProc.ImageProcessing_new()
	self.ballTemp = str(bytearray(16*10))
	self.map = str(bytearray(4*320*240))
	self.ballData = ""
	self.ballCount = 0
	self.ballList = []

    def classify(self,data):
        libImgProc.ImageProcessing_classify(self.obj,data)

    def findWalls(self,data):
        libImgProc.ImageProcessing_findWalls(self.obj,data,self.map)

    def findBalls(self,data):
        libImgProc.ImageProcessing_findBalls(self.obj,data,self.ballTemp,self.map)


class ArduinoController(object):
    def __init__(self,ard):
        self.obj = libArduino.ArduinoController_new()
	self.data = str(bytearray(4*10))
	self.ard = ard

    def setGyro(self,gyro):
        libArduino.ArduinoController_setGyro(self.obj,gyro)

    def process(self,imgProc):
	self.ard.notify()
        libArduino.ArduinoController_process(self.obj,self.data,imgProc.obj,imgProc.map)


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
screen = pygame.display.set_mode((320,480))

#Initialize IMAGE PROCESSING
imgProc = ImageProcessing()

#Initialize ARDUINO
ard = arduino.Arduino()
mL = arduino.Motor(ard,0,38,39)
mR = arduino.Motor(ard,0,40,41)
imumu = arduino.IMU(ard)
ard.run()
commArd = ArduinoController(ard)

#Game timer
ENDTIME = time.time()+3*60
frames = 0
prev = time.time()

while ( time.time()<ENDTIME ):
	#Image Capture
	img = cam.get_image()
	data = pygame.image.tostring(img,"RGBX")

	#Image processing	
	imgProc.classify(data)
	imgProc.findWalls(data)
	imgProc.findBalls(data)

	#Arduino Interface
	gyro = imumu.getRawValues()[0]
	commArd.setGyro(gyro)

	commArd.process(imgProc)
	commData = commArd.data
	leftD = ord(commData[3])-1
	rightD = ord(commData[1])-1
	mL.setSpeed(60)#leftD*ord(commData[2]))
	mR.setSpeed(63)#rightD*ord(commData[0]))
	#print str(gyro)
	print str(gyro) + " --- " + str(leftD*ord(commData[2])) +", " + str(rightD*ord(commData[0]))

	#Pygame output
	img = pygame.image.fromstring(data,(320,240),"RGBX")
	imgM = pygame.image.fromstring(imgProc.map,(320,240),"RGBX")
	rect = img.get_rect()
	screen.blit(img,rect)
	rect = rect.move(0,240)
	screen.blit(imgM,rect)

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
