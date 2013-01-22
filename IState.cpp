#ifndef ISTATE_INCLUDE
#define ISTATE_INCLUDE

#include "ArduinoController.cpp"
#include "ImageProcessing.cpp"

class ArduinoController;

class IState {
    public:
	~IState() {}
        IState* update( ImageProcessing* imgProc, int* map, ArduinoController* ard ){return this;}
};

#endif
