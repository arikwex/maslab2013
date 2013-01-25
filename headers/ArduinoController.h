#ifndef ARDUINO_INCLUDE
#define ARDUINO_INCLUDE

#include <iostream>
#include <math.h>
#include "ImageProcessing.h"
#include "ExploreState.h"
#include "BallCollectState.h"
#include "IState.h"

class ArduinoController{
    private:
	float prevG;
	float intG;
	float gyro;
	IState* behavior;
	int* data;

	//Robot Knowledge goes here
	int turbine;
	int ballsCollected;

    public:
	ArduinoController();

	~ArduinoController();

	void process( int* comm, ImageProcessing* imgProc, int* map );

	void collectedBall();

	void setGyro( int g );

	float getGyro();

	void setTurbine( int a );

	float getHeadingError( float dest );

	void driveController( float E, int base );

	void setMotors( int leftM, int rightM );

	int numCollectedBalls();
};

#endif
