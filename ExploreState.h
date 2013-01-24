#ifndef EXPLORE_STATE_INCLUDE
#define EXPLORE_STATE_INCLUDE

#include "ArduinoController.h"
#include "ImageProcessing.h"
#include "IState.h"
#include "BallCollectState.h"

class ExploreState : public IState {
    private:
	int ballConfirmation;
	int deployConfirmation;

    public:
	IState* update( ImageProcessing* imgProc, ArduinoController* ard );
};

#endif
