#ifndef BC_STATE_INCLUDE
#define BC_STATE_INCLUDE

#include "IState.cpp"
#include "HeadingState.cpp"
#include "ArduinoController.cpp"
#include "ImageProcessing.cpp"

class BallCollectState : public IState {
    public:
	float prevG = 0;
	float intG = 0;

	IState* update( ImageProcessing* imgProc, int* map, int* data, int gyro ) {
		//ard->setTurbine(180,data);
		if ( imgProc->ballCount>0 ) {
			//find closest
			float closest = 1000;
			int idx = 0;
			for ( int i = 0; i < imgProc->ballCount; i++ ){
				int base = 4*i;
				float a = imgProc->storedBalls[base+1];
				if ( a < closest ) {
					idx = base;
					closest = a;
				}
			}

			//std::cout << "gyro: " << gyro << " ---> " << (gyro-imgProc->storedBalls[idx+0]*57.3) << std::endl;

			//Go for it
			if ( closest>15 ) {
				data[1] = 180;
				return new HeadingState((int)(gyro-imgProc->storedBalls[idx+0]*57.3),40,closest/4,180,this);
			}
			else if ( closest<15 ) {
				return new HeadingState((int)(gyro-imgProc->storedBalls[idx+0]*57.3),40,closest*8,90,this);
			}
				
		} else {
			data[0] = 0;
		}
		return this;
	}
};

#endif