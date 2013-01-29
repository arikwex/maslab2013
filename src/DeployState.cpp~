#include "../headers/DeployState.h"
#include "../headers/ExploreState.h"

DeployState::DeployState() {
	mode = 0;
	destTime = 0;
	deployConfirmation = 0;
	heading = 0;
}

IState* DeployState::update( ImageProcessing* imgProc, ArduinoController* ard ) {
	/////////////////
	// DEPLOYMENT //
	/////////////////

	if ( mode == 0 ) {
		//If deployment region visible, approach it
		if ( imgProc->deploymentRegionVisible ) {
			deployConfirmation = 0;
			float closest = imgProc->deploymentRegionDistance;
			float angle = imgProc->deploymentRegionAngle;

			if ( closest>17 ) {
				if ( closest>20 ) closest = 20;
				mode = 1;
				heading = ard->getGyro()-angle*57.3;
				destTime = getTime()+0.5;
			} else {
				mode = 2;
				heading = ard->getGyro()-angle*57.3;
				destTime = getTime()+closest*0.25f+3.5f;
			}

		} else {
			deployConfirmation++;
		}

		if ( deployConfirmation>=10 ) {
			return new ExploreState();
		}
	}

	///////////////////
	// APPROACH WALL //
	///////////////////
	else if ( mode==1 ) {
		float time = getTime();
		if ( time<destTime ) {
			float E = ard->getHeadingError(heading);
			ard->driveController(E,0);
			std::cout << "Approach Deployment region." << std::endl;
			
			if ( E*E<0.2 ) {
				ard->driveController(E,70);
			} else {
				ard->driveController(E,0);
			}
		} else {
			mode = 0;
		}
	}

	//////////////////
	// DEPLOY BALLS //
	//////////////////
	else if ( mode==2 ) {
		float time = getTime();
		if ( time<destTime ) {
			float E = ard->getHeadingError(heading);
			ard->driveController(E,70);
			if ( time>destTime-3.5 ) {
				ard->setGateway(180);
				std::cout << "Scoring!" << std::endl;
			}
		} else {
			ard->clearedBalls();
			ard->setGateway(0);
			return new ExploreState();
		}
	}


	return this;
}

float DeployState::getTime() { 
	return ((float)clock())/CLOCKS_PER_SEC;
}
