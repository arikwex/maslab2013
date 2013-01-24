#ifndef ISTATE_INCLUDE
#define ISTATE_INCLUDE

#include "ArduinoController.h"
#include "ImageProcessing.h"

class ArduinoController;

class IState {
    public:
	~IState() {}
        virtual IState* update( ImageProcessing* imgProc, ArduinoController* ard ) = 0;
};

#endif
