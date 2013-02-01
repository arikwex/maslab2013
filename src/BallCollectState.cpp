#include <stdio.h>
#include <stdlib.h>
#include "../headers/BallCollectState.h"
#include "../headers/ExploreState.h"
#include "../headers/RepositionState.h"
#include "../headers/IState.h"

BallCollectState::BallCollectState() {
	mode = 0;
	ballLost = 0;
	destTime = 0;
	surrenderTime = getTime()+5;
	system("echo \"A ball has been detected.\" | espeak -s 120 -p 30 &");
}

IState* BallCollectState::update( ImageProcessing* imgProc, ArduinoController* ard ) {

	//std::cout << "BALL COLLECT STATE" << std::endl;	

	if ( getTime() > surrenderTime && mode!=2 ) {
		std::cout << "Surrender on ball collected." << std::endl;
		int spin = ((int)(ard->getGyro()+180))%360;
		system("echo \"Surrender on ball acquisition.\" | espeak -s 120 -p 30 &");
		return new RepositionState(new ExploreState(),0,spin,spin,1);
	}

	////////////////////
	// CORE OPERATION //
	////////////////////

	if ( mode==0 ) {
		//A ball has been seen
		if ( imgProc->ballCount>0 ) {
			ballLost = 0;
			ard->setTurbine(180);
			ard->setGateway(180);

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
			if ( closest>10 ) {
				if ( closest>20 ){
					closest = 20;
					surrenderTime += 0.27f; //if far away, give me time to reach it.
				}
				mode = 1;
				heading = ard->getGyro()-imgProc->storedBalls[idx]*57.3;
				destTime = getTime()+0.3;
				std::cout << "Target ball: angle=" << imgProc->storedBalls[idx]*57.3 << std::endl;
			}
			//if ball is near, go for it
			else {
				mode = 2;
				heading = ard->getGyro()-imgProc->storedBalls[idx]*57.3;
				float E = ard->getHeadingError(heading);
				destTime = getTime()+closest*0.20f+0.5f;
				if ( E*E>0.01 ) {
					mode = 1;
					destTime = getTime()+0.2f;
					std::cout << "Tuning before collection" << std::endl;
				} else {
					system("echo \"Engaging target!\" | espeak -s 120 -p 30 &");
					std::cout << "GO TO TOWN!" << std::endl;
				}
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
			//std::cout << ard->getGyro() << " --> " << E*E << std::endl;
			
			if ( E*E<0.07 ) {
				ard->driveController(E,80);
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
			ard->driveController(E,130);
			if ( time>destTime-0.5 ) {
				ard->setTurbine(0);
			}
		} else {
			system("echo \"Capture sequence complete.\" | espeak -s 120 -p 30 &");
			ard->collectedBall();
			std::cout << "Captured! Ball count = " << ard->numCollectedBalls() << std::endl;
			return new RepositionState(new ExploreState(),-70,(int)(ard->getGyro()),(int)(ard->getGyro()),1.5f);
		}
	}

	return this;
}

float BallCollectState::getTime() { 
	return ((float)clock())/CLOCKS_PER_SEC;
}
