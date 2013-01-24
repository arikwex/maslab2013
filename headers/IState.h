#ifndef ISTATE_INCLUDE
#define ISTATE_INCLUDE

class ArduinoController;

#include "ArduinoController.h"
#include "ImageProcessing.h"

class IState {
    public:
	IState() {}
	~IState() {}
        virtual IState* update( ImageProcessing* imgProc, ArduinoController* ard ) = 0;
};

#endif
