#include <iostream>
#include <math.h>
#include "ImageProcessing.cpp"

class ArduinoController{
    private:
	float prevG = 0;
	float intG = 0;
	int gyro = 0;

    public:
	void process( int* data, ImageProcessing* imgProc ) {
		float E = getHeadingError(gyro,0);
		if ( E*E<0.2 ) {
			driveController(E,100,data);
		} else {
			driveController(E,0,data);
		}
	}

	void setGyro( int g ) { gyro = g; }

	float getHeadingError( int gyro, int dest ) {
		float desired = dest/57.3f;
		float Dx = -cos(desired);
		float Dy = -sin(desired);
		float ang = gyro/57.3f;
		float dx = cos(ang);
		float dy = sin(ang);
		float E = dx*Dy-dy*Dx;
		if ( dx*Dx+dy*Dy<0 ) E = -1;
		if ( dx*Dx+dy*Dy>0 ) E = 1;
		return E;		
	}

	void driveController( float E, int base, int* data ) {
		// PID controller
		intG += E;
		if ( intG>4 ) intG = 4;
		if ( intG<-4 ) intG = -4;
		float M = E*1.0f+0.7f*(E-prevG)+intG*0.2f;
		if ( M>1 ) M=1;
		if ( M<-1 ) M=-1;
		prevG = E;
		int leftM = (int)(base+M*100);
		int rightM = (int)(base-M*100);
		if ( leftM>255 ) leftM = 255;
		if ( leftM<-255 ) leftM = -255;
		if ( rightM>255 ) rightM = 255;
		if ( rightM<-255 ) rightM = -255;
		
		setMotors(leftM,rightM,data);
	}

	void setMotors( int leftM, int rightM, int* data ) {
		int leftD = 2;
		int rightD = 2;
		if ( leftM < 0 ) {
			leftM = -leftM;
			leftD = 0;
		}
		if ( rightM < 0 ) {
			rightM = -rightM;
			rightD = 0;
		}
		data[0] = (leftD<<24) | (leftM<<16) | (rightD<<8) | (rightM);
	}
};


extern "C" {
    ArduinoController* ArduinoController_new(){ return new ArduinoController(); }
    void ArduinoController_setGyro(ArduinoController* arc, int gyro) { arc->setGyro(gyro); }
    void ArduinoController_process(ArduinoController* arc, int* data, ImageProcessing* imgProc){ arc->process(data,imgProc); }
}
