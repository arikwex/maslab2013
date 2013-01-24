#ifndef EXPLORE_STATE_INCLUDE
#define EXPLORE_STATE_INCLUDE

#include "ArduinoController.h"
#include "ImageProcessing.h"
#include "IState.h"

class ExploreState : public IState {
    private:
	int ballConfirmation;
	int deployConfirmation;

    public:
	ExploreState();
	IState* update( ImageProcessing* imgProc, ArduinoController* ard );
};

#endif
