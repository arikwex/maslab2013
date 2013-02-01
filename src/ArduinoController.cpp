#include <iostream>
#include <math.h>
#include "../headers/ArduinoController.h"
#include "../headers/ExploreState.h"
#include "../headers/BallCollectState.h"
#include "../headers/DeployState.h"
#include "../headers/IState.h"


ArduinoController::ArduinoController() {
	behavior = new ExploreState();
	prevG = 0;
	intG = 0;
	gyro = 0;
	turbine = 180; 	//down = 180, up = 0
	gateway = 180;	//open = 20, closed = 180
	ballsCollected = 0;
	gameTimer = getTime();
}

ArduinoController::~ArduinoController() {
	delete behavior;
}

void ArduinoController::process( int* comm, ImageProcessing* imgProc, int* map ) {
	data = comm;
	behavior = behavior->update( imgProc, this );
	data[1] = turbine | (gateway<<8);
}

void ArduinoController::collectedBall() { ballsCollected++; }

void ArduinoController::setGyro( int g ) { gyro = g; }
void ArduinoController::setIR( int a ) { IR = a; }
int ArduinoController::getIR() {
	return IR;
}

void ArduinoController::setTurbine( int a ) {
	turbine = a;
}

void ArduinoController::setGateway( int a ) {
	gateway = a;
}

int ArduinoController::getGateway() {
	return gateway;
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
	intG += E;
	if ( intG>0.4 ) intG = 0.4;
	if ( intG<-0.4 ) intG = -0.4;
	float M = (E*5.0f+12.0f*(E-prevG)+intG*0.6f);
	if ( M>1 ) M=1;
	if ( M<-1 ) M=-1;
	prevG = E;
	int leftM = (int)(base+M*70);
	int rightM = (int)(base-M*70);

	setMotors(leftM,rightM);
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
	return ballsCollected;
}

void ArduinoController::clearedBalls() {
	ballsCollected = 0;
}

float ArduinoController::getGyro() {
	return gyro;
}

float ArduinoController::getGameTimer() { 
	return getTime()-gameTimer;
}

float ArduinoController::getTime() { 
	return ((float)clock())/CLOCKS_PER_SEC;
}


extern "C" {
    ArduinoController* ArduinoController_new(){ return new ArduinoController(); }
    void ArduinoController_setGyro(ArduinoController* arc, int gyro) { arc->setGyro(gyro); }
    void ArduinoController_setIR(ArduinoController* arc, int IR) { arc->setIR(IR); }
    void ArduinoController_process(ArduinoController* arc, int* data, ImageProcessing* imgProc, int* map){ arc->process(data,imgProc,map); }
}
