#include <iostream>
#include <math.h>

class ArduinoController{
    private:
	float prevG = 0;
	float intG = 0;

    public:
	void process( int* data, int gyro, int ddd ) {
		int leftM = 140;
		int rightM = 140;

		// controls
		float desired = ddd/57.3;
		float Dx = -cos(desired);
		float Dy = -sin(desired);
		float ang = gyro/57.3;
		float dx = cos(ang);
		float dy = sin(ang);
		float E = dx*Dy-dy*Dx;
		intG += E;
		if ( intG>4 ) intG = 4;
		if ( intG<-4 ) intG = -4;
		float M = E*1.0f+0.7f*(E-prevG)+intG*0.2;
		if ( M>1 ) M=1;
		if ( M<-1 ) M=-1;
		prevG = E;
		leftM = (int)(0+M*100);
		rightM = (int)(0-M*100);
		std::cout << E << std::endl;
		if ( E*E<0.1 ) {
			leftM = (int)(100+M*60);
			rightM = (int)(100-M*60);
		}
		// 

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
    void ArduinoController_process(ArduinoController* arc, int* data, int gyro, int ddd){ arc->process(data,gyro,ddd); }
}
