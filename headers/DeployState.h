#ifndef DEPLOY_STATE_INCLUDE
#define DEPLOY_STATE_INCLUDE

#include "ArduinoController.h"
#include "ImageProcessing.h"
#include "IState.h"

class DeployState : public IState {
    private:
	int deployConfirmation;
	int destTime;
	int heading;
	int mode;
	int correctionCount;
	float surrenderTime;

    public:
	DeployState();
	IState* update( ImageProcessing* imgProc, ArduinoController* ard );
	float getTime();
};

#endif
