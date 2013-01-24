import arduino
import time

ard = arduino.Arduino()

s0 = arduino.Servo(ard, 37)
mL = arduino.Motor(ard,0,38,39)
mR = arduino.Motor(ard,0,40,41)

ard.run()

while ( True ):
	ard.notify()
	mR.setSpeed(100)
	'''	
	for i in range(18):
		ard.notify()
		s0.setAngle(i*10)
		time.sleep(1.0)
		print "ppeep " + str(i*10)
	'''
