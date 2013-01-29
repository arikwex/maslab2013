#include "../headers/BallCollectState.h"
#include "../headers/ExploreState.h"
#include "../headers/IState.h"

BallCollectState::BallCollectState() {
	mode = 0;
	ballLost = 0;
	destTime = 0;
}

IState* BallCollectState::update( ImageProcessing* imgProc, ArduinoController* ard ) {

	//std::cout << "BALL COLLECT STATE" << std::endl;	

	////////////////////
	// CORE OPERATION //
	////////////////////

	if ( mode==0 ) {
		//A ball has been seen
		if ( imgProc->ballCount>0 ) {
			ballLost = 0;
			ard->setTurbine(180);

			//find closest
			float closest = 1000;
			int idx = 0;
			for ( int i = 0; i < imgProc->ballCount; i++ ){
				int base = 4*i;
				float a = imgProc->storedBalls[base+1];
				if ( a < closest ) {
					idx = base;
					closest = a;
				}
			}
			//std::cout << "Distance: " << closest << std::endl;
			//return this;
			//if ball is far, approach
			if ( closest>8 ) {
				if ( closest>20 ) closest = 20;
				mode = 1;
				heading = ard->getGyro()-imgProc->storedBalls[idx]*57.3;
				destTime = getTime()+0.5;
				//std::cout << imgProc->storedBalls[idx]*57.3 << std::endl;
			}
			//if ball is near, go for it
			else {
				mode = 2;
				heading = ard->getGyro()-imgProc->storedBalls[idx]*57.3;
				destTime = getTime()+closest*0.4f+0.5f;
				std::cout << "GO TO TOWN!" << std::endl;
			}

		} else {
			ballLost++;
		}
	
		//Go back to exploration
		if ( ballLost>10 ) {
			std::cout << "Ball Lost, returning to Explore state." << std::endl;
			return new ExploreState();
		}
	}

	///////////////////
	// APPROACH BALL //
	///////////////////
	else if ( mode==1 ) {
		float time = getTime();
		if ( time<destTime ) {
			float E = ard->getHeadingError(heading);
			ard->driveController(E,0);
			//std::cout << ard->getGyro() << " --> " << E*E << std::endl;
			
			if ( E*E<0.07 ) {
				ard->driveController(E,70);
				//std::cout << "THIS IS SPARTQA" << std::endl;
			} else {
				//std::cout << ard->getGyro() << " --> " << heading << std::endl;
				ard->driveController(E,0);
			}
		} else {
			mode = 0;
		}
	}

	//////////////////
	// COLLECT BALL //
	//////////////////
	else if ( mode==2 ) {
		float time = getTime();
		if ( time<destTime ) {
			float E = ard->getHeadingError(heading);
			ard->driveController(E,70);
			if ( time>destTime-0.5 ) {
				ard->setTurbine(0);
				ard->collectedBall();
				std::cout << "Captured!" << std::endl;
			}
		} else {
			return new ExploreState();
		}
	}

	return this;
}

float BallCollectState::getTime() { 
	return ((float)clock())/CLOCKS_PER_SEC;
}
