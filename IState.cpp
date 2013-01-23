#ifndef ISTATE_INCLUDE
#define ISTATE_INCLUDE

#include "ArduinoController.cpp"
#include "ImageProcessing.cpp"

class ArduinoController;

class IState {
    public:
	~IState() {}
        virtual IState* update( ImageProcessing* imgProc, int* map, int* data, int gyro ) = 0;
};

#endif
