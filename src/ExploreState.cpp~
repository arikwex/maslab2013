#include "../headers/ExploreState.h"
#include "../headers/BallCollectState.h"
#include "../headers/DeployState.h"
#include "../headers/RepositionState.h"


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
	if ( deviate<-30 ) deviate = -30;
	if ( deviate>30 ) deviate = 30;
	//std::cout << "Explore Deviation: " << deviate << std::endl;
	float E = ard->getHeadingError( ard->getGyro()+deviate/2 );
	ard->driveController(E,80 - (float)sqrt(deviate*deviate)*2);
	//ard->driveController(0,0);

	//////////////////////////
	// Avoid walls behavior //
	//////////////////////////
	//std::cout << "IR SENAZOR: " << ard->getIR() << std::endl;
	if ( ard->getIR() > 650 ) {
		int finalHeading = ((int)(ard->getGyro()+170))%360;
		return new RepositionState(this,-50,finalHeading,finalHeading,2);
	}


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
	//std::cout << "BALLS : " << ard->numCollectedBalls() << std::endl;
	if ( ard->numCollectedBalls()>0 ) {
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
