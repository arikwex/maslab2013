import sys
import time
sys.path.append("../..")
import arduino
from ctypes import cdll
from bitarray import bitarray
libArduino = cdll.LoadLibrary('shared/libarduino.so')
libImgProc = cdll.LoadLibrary('shared/libimgproc.so')

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

    def setIR(self,IR):
        libArduino.ArduinoController_setIR(self.obj,IR)

    def process(self,imgProc):
	self.ard.notify()
        libArduino.ArduinoController_process(self.obj,self.data,imgProc.obj,imgProc.map)


#Initialize Pygame
import pygame.camera
import pygame.image
pygame.camera.init()
cams = pygame.camera.list_cameras()
cam = pygame.camera.Camera(cams[0],(320,240))
cam.start()
cam.get_image()
frames = 0
prev = time.time()
screen = pygame.display.set_mode((320,480))

#Initialize IMAGE PROCESSING
imgProc = ImageProcessing()

#Initialize ARDUINO
ard = arduino.Arduino()
colorSetting = arduino.AnalogInput(ard,0)
powerButton = arduino.AnalogInput(ard,1)
gateway = arduino.Servo(ard,8)
turbine = arduino.Servo(ard,9)
mR = arduino.Motor(ard,0,10,12)
mL = arduino.Motor(ard,0,11,13)
mR.setSpeed(0)
mL.setSpeed(0)
gateway.setAngle(180)
turbine.setAngle(180)

imumu = arduino.IMU(ard)
ard.run()
commArd = ArduinoController(ard)

#Wait for self intialization and gyro tuning
#Resume on power on

'''
consecutive = 0
calibration = time.time()+5
print "Calibrating Gyro..."
while ( time.time()<calibration ):
	ard.notify()
	gateway.setAngle(120)
	pass
print "Calibration Done!"
print "Waiting for ON switch..."

while ( consecutive<30 ):
	ard.notify()
	if ( powerButton.getValue()>500 ):
		consecutive+=1
	else:
		consecutive = 0;
'''
#colorChoice = 0
#if ( colorSetting.getValue()>500 ):
#	colorChoice = 1
#colorMap = ['Green','Red']
#print "Color Mode: " + colorMap[colorChoice]
print "Begin Primary Functionality."

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
	commArd.setIR(colorSetting.getValue())
	#print str(imumu.getRawValues())

	commArd.process(imgProc)
	commData = commArd.data
	#commData = [chr(0),chr(0),chr(0),chr(0),chr(180),chr(0)]	
	leftD = ord(commData[3])-1
	rightD = ord(commData[1])-1
	turbine.setAngle(ord(commData[4]))
	gateway.setAngle(ord(commData[5]))
	#print "gateway: ", ord(commData[5])
	#print "IR: ",colorSetting.getValue()
	mL.setSpeed(-1*leftD*ord(commData[2]))
	mR.setSpeed(-1*rightD*ord(commData[0]))
	#print str(gyro)
	#print str(gyro) + " --- " + str(leftD*ord(commData[2])) +", " + str(rightD*ord(commData[0]))

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
		if ( frames<18 ):
			print "Warning! Low FPS: " + str(frames)
		frames = 0
		prev = time.time()

mL.setSpeed(0)
mR.setSpeed(0)
cam.stop()
pygame.quit()
