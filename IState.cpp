class IState {
    public:
	virtual ~IState() {}
        virtual IState update( ImageProcessing* imgProc, int* map, ArduinoController* ard ) = 0;
}
