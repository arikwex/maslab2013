#ifndef ARDUINO_INCLUDE
#define ARDUINO_INCLUDE

#include <iostream>
#include <math.h>
#include "ImageProcessing.cpp"
#include "ExploreState.cpp"
#include "BallCollectState.cpp"
#include "IState.cpp"

class ArduinoController{
    public:
	float prevG;
	float intG;
	int gyro;
	IState* behavior;

    public:
	ArduinoController() {
		behavior = new BallCollectState();
		prevG = 0;
		intG = 0;
		gyro = 0;
	}

	~ArduinoController() {
		delete behavior;
	}

	void process( int* data, ImageProcessing* imgProc, int* map ) {
		//std::cout << "GOOGLE" << std::endl;
		
		behavior = behavior->update( imgProc, map, data, gyro );
		//setTurbine(0,data);
		//float E = getHeadingError(gyro,0);
//		if ( E*E<0.2 ) {
//			driveController(E,100,data);
//		} else {
//		driveController(E,40,data);
//		}
	}

	void setGyro( int g ) { gyro = g; }

	void setTurbine( int a, int* data ) {
		data[1] = a;
	}
};


extern "C" {
    ArduinoController* ArduinoController_new(){ return new ArduinoController(); }
    void ArduinoController_setGyro(ArduinoController* arc, int gyro) { arc->setGyro(gyro); }
    void ArduinoController_process(ArduinoController* arc, int* data, ImageProcessing* imgProc, int* map){ arc->process(data,imgProc,map); }
}
#endif
