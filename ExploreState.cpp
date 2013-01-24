#include "ExploreState.h"

ExploreState::ExploreState() {
	ballConfirmation = 0;
	deplotConfirmation = 0;
}

IState* ExploreState::update( ImageProcessing* imgProc, ArduinoController* ard ) {
	/////////////////
	// EXPLORATION //
	/////////////////

	///////////////////////
	// STATE TRANSITIONS //
	///////////////////////

	//If ball confirmed, then begin approach
	if ( imgProc->ballCount>0 ) {
		ballConfirmation++;
		if ( ballConfirmation>=6 ) {
			return new BallCollectState();
		}
	} else {
		ballConfirmation = 0;
	}

	//If ball collected and I see a deployment region
	if ( ard->ballsCollected>0 ) {
		if ( imgProc->deploymentWallSeen ) {
			deployConfirmation++;
			if ( deployConfirmation>=6 ) {
				//return new DeployState();
			}
		} else {
			deployConfirmation = 0;
		}
	} else {
		ard->setTurbine(180);
	}
	return this;
}
