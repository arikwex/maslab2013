#include <stdio.h>
#include <stdlib.h>
#include "../headers/DeployState.h"
#include "../headers/ExploreState.h"
#include "../headers/RepositionState.h"

DeployState::DeployState() {
	mode = 0;
	destTime = 0;
	deployConfirmation = 0;
	heading = 0;
	surrenderTime = getTime()+6;
	correctionCount = 0;
}

IState* DeployState::update( ImageProcessing* imgProc, ArduinoController* ard ) {
	/////////////////
	// DEPLOYMENT //
	/////////////////

	if ( getTime() > surrenderTime && mode!=2 ) {
		std::cout << "Surrender on deployment." << std::endl;
		int spin = ((int)(ard->getGyro()+180))%360;
		return new RepositionState(new ExploreState(),0,spin,spin,1);
	}

	if ( mode == 0 ) {

		//If deployment region visible, approach it
		if ( imgProc->deploymentRegionVisible ) {

			//If I'm not perpendicular to the deployment region, reorient!
			float NX = imgProc->deploymentRegionNX;
			float NY = imgProc->deploymentRegionNY;
			float angleR = imgProc->deploymentRegionAngle;
			float finalTheta = atan2(-NX,-NY);
			if ( correctionCount<1 && (-NY<0.9 || angleR*angleR>0.1) ) {
				float dist = imgProc->deploymentRegionDistance*1.4f;				
				//std::cout << "perp: " << angleR*angleR << ", Dist = " << dist << std::endl;
				float Dx = dist*sin(angleR);
				float Dy = dist*cos(angleR);
				float Q = 15;
				float DestX = (Dx+Q*NX);
				float DestY = (Dy+Q*NY);
				float travelDist = sqrt(DestX*DestX+DestY*DestY);
				float travelAngle = atan2(DestX,DestY);
				float travelTime = travelDist*0.2f+1;
				surrenderTime = getTime()+travelTime+6;
				correctionCount++;
				system("echo \"Reorienting for deployment.\" | espeak -s 120 -p 30 &");
				RepositionState* backup = new RepositionState(this,-80,(int)(ard->getGyro()-finalTheta*57.3f),(int)(ard->getGyro()-finalTheta*57.3f),2);
				return new RepositionState(backup,70,(int)(-travelAngle*57.3f+ard->getGyro()),(int)(ard->getGyro()-finalTheta*57.3f),travelTime);
				//std::cout << "travel dist: " << travelDist << ", ang = " << travelAngle*57.3 << std::endl;
			}

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
				destTime = getTime()+closest*0.12f+2.5f;
				system("echo \"Initiating deployment sequence.\" | espeak -s 120 -p 30 &");
				std::cout << "Engaging deployment region. Standby." << std::endl;
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
			if ( ard->getIR()>770 || time>destTime-1.2f ) {
				ard->driveController(E-(float)sin(time*2)*0.1,70);
				if ( ard->getGateway()==180 )
					std::cout << "Open release gateway." << std::endl;
				ard->setGateway(0);
			} else {
				ard->driveController(E,160);
			}
		} else {
			system("echo \"Payload has been delivered... Backing up.\" | espeak -s 120 -p 30 &");
			std::cout << "Payload deployed successfully." << std::endl;
			ard->clearedBalls();
			ard->setGateway(180);
			return new RepositionState(new ExploreState(),-70,(int)(ard->getGyro()+170),(int)(ard->getGyro()+170),2);
		}
	}


	return this;
}

float DeployState::getTime() { 
	return ((float)clock())/CLOCKS_PER_SEC;
}
