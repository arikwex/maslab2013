import arduino
import time

ard = arduino.Arduino()

s0 = arduino.Servo(ard, 9)

ard.run()

while ( True ):
	for i in range(180):
		ard.notify()
		s0.setAngle(i)
		time.sleep(0.1)
