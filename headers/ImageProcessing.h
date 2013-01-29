#ifndef PROCESS_INCLUDE
#define PROCESS_INCLUDE

#include <iostream>
#include <math.h>

class ImageProcessing{
    public:
	float ballInfo[4];
	float* storedBalls;
	int ballCount;
	int minX;
	int minY;
	int maxX;
	int maxY;
	int Nblob;
	float lefts,rights;
	float deploymentRegionAngle;
	float deploymentRegionDistance;
	bool deploymentRegionVisible;	

    public:
	void classify( int* data );

	void findWalls( int* data, int* map );

	int findBalls( int* data, float* ballData, int* map );

	bool floodFind( int x, int y, int* data, int search);

	void blobFind( int x, int y, int* data, int search );
};

#endif
