#include <iostream>
#include <math.h>
#include "../headers/ImageProcessing.h"
#define spread 1.2f
#define scale_map 2.0f
#define camera_height 8.0f
#define alpha 320.0f/spread
#define beta 240.0f/spread

#define alphaC 180*100
#define betaC 140*100

#define blue_b 100		//B = <0,0,1>
#define red_r 100		//R = <1,0,0>
#define green_g 100		//G = <0,1,0>
#define purple_r 71		//P = <0.707,0,0.707>
#define purple_b 71		//P = <0.707,0,0.707>
#define white_r 58		//W = <x,x,x>
#define white_g 58		//W = <x,x,x>
#define white_b 58		//W = <x,x,x>
#define yellow_r 71		//Y = <0.707,0.707,0>
#define yellow_g 71		//Y = <0.707,0.707,0>


// Line - Point distance = D
//
// D = |(x2-x1) X (x1-x0)| / |(x2-x1)|
// D = |<R,G,B> X COLOR_NORM|
// D = |<23-32,31-13,12-21>|
//

int ImageProcessing::colorDist( int r, int g, int b, int cR, int cG, int cB ) {
	int dx = (g*cB-b*cG);
	int dy = (b*cR-r*cB);
	int dz = (r*cG-g*cR);
	return (dx*dx+dy*dy+dz*dz);
}

void ImageProcessing::classify( int* data ) {
	int darkest = 255*255;
	int avg = 0;
	int blR = 0;
	int blG = 0;
	int blB = 0;
	for ( int i = 0; i < 320*240; i++ ) {
		int p = data[i];
		int b = (p>>16)&0xff;
		int g = (p>>8)&0xff;
		int r = (p)&0xff;
		int lum = (r*r+g*g+b*b);
		if ( lum < darkest ) {
			darkest = lum;
			blR = r;
			blG = g;
			blB = b;
		}
	}

	for ( int i = 0; i < 320*240; i++ ) {
		int p = data[i];
		int b = (p>>16)&0xff;
		int g = (p>>8)&0xff;
		int r = (p)&0xff;
		int dist = (r-blR)*(r-blR)+(g-blG)*(g-blG)+(b-blB)*(b-blB);

		if ( r > (g+b)*5/8 & r > g*14/10 ) {		//RED
		 	data[i] = 0xff0000ff;
		} else if ( r > 15*b/10 & g > 15*b/10 ) {	//YELLOW
			data[i] = 0xff00ffff;	
		} else if ( g > (r+b)*5/8 ) {			//GREEN
			data[i] = 0xff00ff00;
		} else if ( b > (r+g)*9/16 & r > g & dist>150*150) {		//PURPLE
			data[i] = 0xffff00ff;	
		} else if ( b > (r+g)*6/8 ) {		//BLUE
			data[i] = 0xffff0000;
		} else if ( dist<180*180 ) {	//FLOOR
			data[i] = 0xffff0000;
		} else {
			data[i] = 0xff000000;
		}
	}
}

void ImageProcessing::findWalls( int* data, int* map ) {
	int Xc_save = 0;
	int Zc_save = 0;

	float yellowAngleSum = 0;
	float yellowDistanceSum = 0;
	float yellowDXSum = 0;
	float yellowDYSum = 0;
	int yellowN = 0;

	float purpleAngleSum = 0;
	float purpleDistanceSum = 0;
	float purpleDXSum = 0;
	float purpleDYSum = 0;
	int purpleN = 0;

	float Xprev = 0;

	lefts=0;
	rights=0;
	int wallBroken = 0;
	for ( int i= 0; i < 320*240; i++ ) {
		map[i] = 0x00000000;
	}
	float prev = 0;
	for ( int x= 0; x < 320; x+=1 ) {
		int my = 239;
		int mx = x;
		int color = 0;
		int bottom = 239;
		int top = 0;
		int consecutive = 0;
		float objHeight = 6;
		//detect blue termination
		while ( my>0 & mx >= 0 & mx < 320) {
			int idx = mx+320*my;
			int pix = data[idx];
			if ( pix==0xff00ff00 | pix==0xff0000ff ) {
				if ( x > 160 ) mx-=10;
				if ( x < 160 ) mx+=10;					
				idx = mx+320*my;
				pix = data[idx];
			}
			if ( pix==0xffff0000 ) {
				consecutive=0;
				bottom = my;
			} else {
				color = pix;
				consecutive++;
				
			}
			if ( consecutive>=7 ) {
				break;
			}
			my--;
		}
		if ( bottom > 0 && bottom<230 ) {
			//for ( int q = 0; q < 3; q++ )
			//	data[x+320*(bottom+q)] = 0xff00ff00;
		}

		//IF NOT PURPLE or YELLOW WALL
		if ( color!=0xffff00ff ) {
			if ( color==0xff000000 )
				color = 0xffffffff;
			//detect blue region bottom
			while ( my>0 ) {
				int idx = x+320*my;
				int pix = data[idx];
				if ( pix==0xff00ffff )
					color = 0xff00ffff;
				if ( pix==0xffff0000 ) {
					consecutive++;
					if ( consecutive>2 )
						break;
				} else {
					consecutive = 0;
					top = my;
				}
				my--;
			}
			objHeight = 4;
			/*
			int cutoff = (bottom-my)/2;
			//detect blue region top
			consecutive = 0;
			int start = my;
			while ( my>0 && start-my<cutoff ) {
				int idx = x+320*my;
				int pix = data[idx];
				if ( pix!=0xffff0000 ) {
					consecutive++;
				} else {
					consecutive = 0;
					top = my;
				}
				if ( consecutive>=2 ) {
					break;
				}
				my--;
			}
			*/
		} else if ( color==0xffff00ff ) {
			//detect purple region top ending
			consecutive = 0;
			while ( my>0 ) {
				int idx = x+320*my;
				int pix = data[idx];
				if ( pix!=0xffff00ff ) {
					consecutive++;
				} else {
					consecutive = 0;
					top = my;
				}
				if ( consecutive>=20 ) {
					break;
				}
				my--;
			}
		}

		if ( top > 0 && top<230 ) {
			//for ( int q = 0; q < 3; q++ )
			//	data[x+320*(top+q)] = 0xffffff00;
			for ( int fy = top; fy>=0; fy-- ) {
				data[x+fy*320]=0x00000000;
			}
		}

		//Draw to map
		if ( bottom-top < 7 || top<0 )
			continue;
		// NOT USNIG THIS FOR NOW
		// d^2 = h^2+x^2+z^2
		// tan(theta) = x/z
		// z^2*(tan(theta)+1) = d^2 - h^2
		int eta = bottom-top;
		int xi = x-160;
		float d = beta*objHeight/(eta);
		float theta = xi/320.0f*spread;

		float Z = d;
		float deviation = (Z-prev)*(Z-prev);
		if ( deviation<25 ) //smoothen mapping
			Z = (Z+prev)/2.0f;
		float X = (float)(Z*tan(theta));//Z*(xi)/alpha;

		if ( deviation<5 ) {
		if ( color==0xff00ffff ) {
			yellowAngleSum += theta;
			yellowDistanceSum += d;
			yellowN++;
			yellowDXSum += (X-Xprev);
			yellowDYSum += (Z-prev);
		} else if ( color==0xffff00ff ) {
			purpleAngleSum += theta;
			purpleDistanceSum += d;
			purpleN++;
			purpleDXSum += (X-Xprev);
			purpleDYSum += (Z-prev);
		}
		}
		prev = Z;
		Xprev = X;

		int Xcoord = (int)(X*scale_map+160);
		int Zcoord = (int)(-Z*scale_map+120);
		int dest = Xcoord+Zcoord*320;
		if ( Z<13 ) { //veer away from things that are too close
			if ( theta<0 ){ lefts+=1.0; }
			else{ rights+=1.0; }
		}
		if ( Xcoord>0 && Xcoord<320 && Zcoord>0 && Zcoord<240 ) {
			map[dest] = color;
		}
	}

	//set the deployment detection variables
	deploymentRegionVisible = false;
	if ( yellowN > 14 ) {
		deploymentRegionVisible = true;
		deploymentRegionAngle = yellowAngleSum / yellowN;
		deploymentRegionDistance = yellowDistanceSum / yellowN;
		float DX = yellowDXSum / yellowN;
		float DY = yellowDYSum / yellowN;
		float M = (float)sqrt(DX*DX+DY*DY);
		deploymentRegionNX = DY/M;
		deploymentRegionNY = -DX/M;
	}
	else if ( purpleN > 14 ) {
		deploymentRegionVisible = true;
		deploymentRegionAngle = purpleAngleSum / purpleN;
		deploymentRegionDistance = purpleDistanceSum / purpleN;
		float DX = purpleDXSum / purpleN;
		float DY = purpleDYSum / purpleN;
		float M = sqrt(DX*DX+DY*DY);
		deploymentRegionNX = DY/M;
		deploymentRegionNY = -DX/M;
	}

	//draw viewport
	for ( int d = 0; d < 100; d++ ) {
		int x = (int)(sin(spread/2.0f)*d)+160;
		int y = (int)(-cos(spread/2.0f)*d)+120;
		int idx = x+y*320;
		map[idx] = 0xffffff00;
		idx = -x+y*320;
		map[idx] = 0xffffff00;
	}
}

int ImageProcessing::findBalls( int* data, float* ballData, int* map ) {
	ballCount = 0;
	for ( int y = 20; y < 240; y+=5 ) {
		for ( int x = 0; x < 320; x+=5 ) {
			int pix = data[x+320*y];
			if ( pix==0xff0000ff | pix==0xff00ff00 ) {
				bool valid = floodFind(x,y,data,pix);
				if ( valid ) {
					int base = ballCount<<2;

					int eta = ballInfo[2];
					int xi = ballInfo[0]-160;
					float d = beta*2.1f/(eta);
					float theta = xi/320.0f*spread;
					float Z = d;
					float X = (float)(Z*tan(theta));			

					//last confirmation: Ball is reachable

					int Xcoord = (int)(X*scale_map+160);
					int Zcoord = (int)(-Z*scale_map+120);
					int dest = Xcoord+Zcoord*320;
					int unreachable = 0;
					if ( Xcoord>10 && Xcoord<310 && Zcoord>10 && Zcoord<230 ) {
						for ( int xx = -5; xx<=5; xx++ ) {
							for ( int yy = -5; yy<=5 yy++ ) {
								if ( map[dest+xx+yy*320]&0x00ffffff!=0x00000000 ) {
									unreachable++;
								}
							}
						}
						for ( int xx = -3; xx<=3; xx++ ) {
							for ( int yy = -3; yy<=3; yy++ ) {
								if ( unreachable<4 ) {
									//reachable means draw a circle
									if ( xx*xx+yy*yy>5 )
										continue;
								} else {
									//unreachable means draw an X
									if ( xx!=yy && xx!=-yy )
										continue;
								}
								map[dest+xx+yy*320] = pix;
							}
						}
					}

					if ( unreachable<4 ) {
						ballData[base] = theta;
						ballData[base+1] = d;
						ballData[base+2] = eta;
						ballData[base+3] = pix;
						this->storedBalls = ballData;
						ballCount++;
					}
				}
			}
		}
	}
	return ballCount;
}

bool ImageProcessing::floodFind( int x, int y, int* data, int search) {
	minX = 10000;
	minY = 10000;
	maxX = 0;
	maxY = 0;
	Nblob = 0;

	blobFind(x,y,data,search);

	int dx = maxX - minX;
	int dy = maxY - minY;		
	float ratio = (float)(dx)/(float)(dy);
	float fraction = (float)(Nblob)/(float)(dx*dy);

	bool sided = false;
	if ( maxX>=319 && Nblob>40 && dy>dx ) {sided = true; dx = dy;}
	if ( minX<=1 && Nblob>40 && dy>dx ) {sided = true; dx = dy;}
	if ( maxY>=239 && Nblob>30 ) {sided = true; dy = dx;}
	if ( dx>100 || dy>90 ) { dx=1000; sided=false; Nblob = 1; }

	if ( ratio>0.75 && ratio<1.25 && fraction > 0.55 && fraction < 0.95 && Nblob>=30  || sided ) {
		ballInfo[0] = (minX+maxX)/2;
		ballInfo[1] = (minY+maxY)/2;
		ballInfo[2] = (dx+dy)/2;
		ballInfo[3] = search;
		//data[(dx/2+minX)+320*(dy/2+minY)] = 0xffffffff;
		return true;
	}
	return false;
}

void ImageProcessing::blobFind( int x, int y, int* data, int search ) {
	if ( x<0 | x>=320 | y<0 | y>=240 )
		return;
	int pix = data[x+y*320];
	if ( pix==search ) {
		data[x+y*320] |= 0x00010101;
		Nblob++;
		if ( x < minX ) minX = x;
		if ( y < minY ) minY = y;
		if ( x > maxX ) maxX = x;
		if ( y > maxY ) maxY = y;
		blobFind(x+1,y,data,search);
		blobFind(x-1,y,data,search);
		blobFind(x,y+1,data,search);
		blobFind(x,y-1,data,search);
	}
}


extern "C" {
    ImageProcessing* ImageProcessing_new(){ return new ImageProcessing(); }
    void ImageProcessing_classify(ImageProcessing* imgProc, int* data){ imgProc->classify(data); }
    int ImageProcessing_findBalls(ImageProcessing* imgProc, int* data, float* ballData, int* map){ return imgProc->findBalls(data,ballData,map); }
    void ImageProcessing_findWalls(ImageProcessing* imgProc, int* data, int* map){ return imgProc->findWalls(data,map); }
}
