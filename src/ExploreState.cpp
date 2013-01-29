#include "../headers/ExploreState.h"
#include "../headers/BallCollectState.h"
#include "../headers/DeployState.h"


ExploreState::ExploreState() {
	ballConfirmation = 0;
	deployConfirmation = 0;
}

IState* ExploreState::update( ImageProcessing* imgProc, ArduinoController* ard ) {
	/////////////////
	// EXPLORATION //
	/////////////////
	
	//std::cout << "EXPLORE STATE." << std::endl;
	int cnt = imgProc->rights - imgProc->lefts;
	float deviate = cnt*0.4;
	if ( deviate<-10 ) deviate = -10;
	if ( deviate>10 ) deviate = 10;
	//std::cout << "Explore Deviation: " << deviate << std::endl;
	float E = ard->getHeadingError( ard->getGyro()+deviate );
	ard->driveController(E,50);
	//ard->driveController(0,0);	

	///////////////////////
	// STATE TRANSITIONS //
	///////////////////////

	//If ball confirmed, then begin approach
	if ( imgProc->ballCount>0 ) {
		ballConfirmation++;
		if ( ballConfirmation>=6 ) {
			std::cout << "DETECTED BALL!" << std::endl;
			return new BallCollectState();
		}
	} else {
		ballConfirmation = 0;
	}

	//If ball collected and I see a deployment region
	if ( ard->numCollectedBalls()>=0 ) {
		//std::cout << "Ready for deploy. " << imgProc->deploymentRegionVisible << std::endl;
		if ( imgProc->deploymentRegionVisible ) {
			deployConfirmation++;
			if ( deployConfirmation>=10 ) {
				return new DeployState();
			}
		} else {
			deployConfirmation = 0;
		}
	}

	return this;
}
