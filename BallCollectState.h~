#ifndef BC_STATE_INCLUDE
#define BC_STATE_INCLUDE

#include <ctime>
#include "IState.h"
#include "ArduinoController.h"
#include "ImageProcessing.h"

class BallCollectState : public IState {
    private:
	int mode;
	int ballLost;
	float heading;
	float destTime;

    public:

	BallCollectState();

	IState* update( ImageProcessing* imgProc, ArduinoController* ard );

	float getTime();
};

#endif
