#include "../headers/ExploreState.h"

#include <stdio.h>
#include <stdlib.h>
#include "../headers/BallCollectState.h"
#include "../headers/DeployState.h"
#include "../headers/RepositionState.h"


ExploreState::ExploreState() {
	ballConfirmation = 0;
	deployConfirmation = 0;
	dontTalk = false;
}

IState* ExploreState::update( ImageProcessing* imgProc, ArduinoController* ard ) {
	/////////////////
	// EXPLORATION //
	/////////////////

	if ( ((int)ard->getGameTimer())%5==0 ) {
		if ( !dontTalk )
			system("echo \"Exploring...\" | espeak -s 120 -p 30 &");
		dontTalk = true;
	} else {
		dontTalk = false;
	}
	
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
	if ( ard->getIR() > 670 || ard->getIR() < 0 ) {
		int finalHeading = ((int)(ard->getGyro()+170))%360;
		system("echo \"Backing up...\" | espeak -s 120 -p 30 &");
		return new RepositionState(this,-55,finalHeading,finalHeading,2);
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
	//std::cout << "game timer: " << ard->getGameTimer() << std::endl;
	if ( (ard->numCollectedBalls()>0 && ard->getGameTimer()>90) || ard->numCollectedBalls()>=3 ) {
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
