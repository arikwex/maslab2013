#ifndef EXPLORE_STATE_INCLUDE
#define EXPLORE_STATE_INCLUDE

#include "IState.cpp"
#include "ArduinoController.cpp"
#include "ImageProcessing.cpp"

class ExploreState : public IState {
    public:
	IState* update( ImageProcessing* imgProc, int* map, int* data, int gyro ) {
		return this;
	}
};

#endif
