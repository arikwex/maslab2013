#include <iostream>
#include <math.h>
#include "../headers/ArduinoController.h"
#include "../headers/ExploreState.h"
#include "../headers/BallCollectState.h"
#include "../headers/IState.h"


ArduinoController::ArduinoController() {
	behavior = new ExploreState();
	prevG = 0;
	intG = 0;
	gyro = 0;
}

ArduinoController::~ArduinoController() {
	delete behavior;
}

void ArduinoController::process( int* comm, ImageProcessing* imgProc, int* map ) {
	data = comm;
	behavior = behavior->update( imgProc, this );
}

void ArduinoController::collectedBall() { ballsCollected++; }

void ArduinoController::setGyro( int g ) { gyro = g/100.0f; }

void ArduinoController::setTurbine( int a ) {
	turbine = a;
	data[1] = a;
}

float ArduinoController::getHeadingError( float dest ) {
	float desired = dest/57.3f;
	float Dx = cos(desired);
	float Dy = sin(desired);
	float ang = gyro/57.3f;
	float dx = cos(ang);
	float dy = sin(ang);
	float E = dx*Dy-dy*Dx;
	if ( dx*Dx+dy*Dy<0 ) {
		if ( E<0 ) E=-1;
		else E=1;
	}
	return -E;		
}

void ArduinoController::driveController( float E, int base ) {
	// PID controller
	intG += E*0.2f;
	if ( intG>3 ) intG = 3;
	if ( intG<-3 ) intG = -3;
	float M = (E*2.5f+20.0f*(E-prevG)+intG*0.7f);
	if ( M>1 ) M=1;
	if ( M<-1 ) M=-1;
	prevG = E;
	int leftM = (int)(base+M*50);
	int rightM = (int)(base-M*50);

	setMotors(leftM,rightM,data);
}


void ArduinoController::setMotors( int leftM, int rightM ) {
	if ( leftM>255 ) leftM = 255;
	if ( leftM<-255 ) leftM = -255;
	if ( rightM>255 ) rightM = 255;
	if ( rightM<-255 ) rightM = -255;
	
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

int ArduinoController::numCollectedBalls() {
	return collectedBalls;
}

float ArduinoController::getGyro() {
	return gyro;
}

extern "C" {
    ArduinoController* ArduinoController_new(){ return new ArduinoController(); }
    void ArduinoController_setGyro(ArduinoController* arc, int gyro) { arc->setGyro(gyro); }
    void ArduinoController_process(ArduinoController* arc, int* data, ImageProcessing* imgProc, int* map){ arc->process(data,imgProc,map); }
}
