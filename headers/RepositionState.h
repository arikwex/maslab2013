#ifndef REPOS_STATE_INCLUDE
#define REPOS_STATE_INCLUDE

#include <ctime>
#include "ArduinoController.h"
#include "ImageProcessing.h"
#include "IState.h"

class RepositionState : public IState {
    private:
	IState* previous;
	int base;
	float heading;
	float finalHeading;
	float destTime;

    public:

	RepositionState( IState* previous, int base, int target, int finalH, float timer );

	IState* update( ImageProcessing* imgProc, ArduinoController* ard );

	float getTime();
};

#endif
