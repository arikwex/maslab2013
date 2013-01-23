#ifndef HD_STATE_INCLUDE
#define HD_STATE_INCLUDE

#include "IState.cpp"
#include "ArduinoController.cpp"
#include "ImageProcessing.cpp"

class HeadingState : public IState {
    public:
	float prevG;
	float intG;
	int dest;
	int bias;
	int turbine;
	int ticks;
	IState* previous;

	HeadingState( int myDest, int biasD, int tick, int turb, IState* pSt ) {
		dest = myDest;
		previous = pSt;
		prevG = 0;
		intG = 0;
		turbine = turb;
		bias = biasD;
		ticks = tick;
	}

	IState* update( ImageProcessing* imgProc, int* map, int* data, int gyro ) {
		float E = getHeadingError(gyro,dest);
		//driveController(E,00,data);


		//if ( E*E<0.1 ) {
			std::cout << "E: " << E << " --- ticks " << ticks << std::endl;
			driveController(E,bias,data);
			ticks--;
		//}
		if ( ticks<=0 ) {
			data[1] = turbine;
			return previous;
		}		

		return this;
	}

float getHeadingError( int gyro, int dest ) {
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

	void driveController( float E, int base, int* data ) {
		// PID controller
		intG += E*0.2f;
		if ( intG>3 ) intG = 3;
		if ( intG<-3 ) intG = -3;
		float M = (E*2.0f+20.0f*(E-prevG)+intG*0.4f);
		if ( M>1 ) M=1;
		if ( M<-1 ) M=-1;
		prevG = E;
		int leftM = (int)(base+M*50);
		int rightM = (int)(base-M*50);

		setMotors(leftM,rightM,data);
	}


	void setMotors( int leftM, int rightM, int* data ) {
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
};

#endif
