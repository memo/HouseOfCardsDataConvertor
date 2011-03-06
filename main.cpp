/***********************************************************************
 * -----------------------------------
 * 
 * Copyright (c) 2008, Memo Akten, www.memo.tv
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 	
 ***********************************************************************/


/***********************************************************************
USAGE:
 make sure all original CSVs are in a folder called 'txt'
 make sure there is a folder called 'data'
 
 run the app, all the .bin files go in the 'data' folder
 
***********************************************************************/

#include <stdio.h>
#include <math.h>
#include <memory.h>

#define		MAX_DOTS			12500		// max max number of dots

// trial and error cropping values
#define		MIN_X				0		// -70
#define		MAX_X				180		// 229
#define		MIN_Y				30		// -200
#define		MAX_Y				320		// 320
#define		MIN_Z				-150	// -1445
#define		MAX_Z				-10		// 403

#define		DIST_X				(MAX_X - MIN_X + 1)		// maximum distrubtion on x
#define		DIST_Y				(MAX_Y - MIN_Y + 1)		// maximum distrubtion on y
#define		DIST_Z				(MAX_Z - MIN_Z + 1)		// maximum distrubtion on z

#define		GRIDX				100					// new size of grid on x
#define		GRIDY				100					// new size of grid on y


//#define	FIND_BOUNDARIES							// uncomment this to find the boundaries of the data

struct OrigData {
	float x, y, z, i;
};

struct NewData {
	unsigned char z, i;
};



int main (int argc, char * const argv[]) {
	char szBuffer[150];				// temp string buffer to store any string
	FILE *fileIn;					// input CSV file

	OrigData origData[MAX_DOTS];	// store array of x,y,z,i

	int frameNum = 1;				// frame counter
	
#ifdef FIND_BOUNDARIES
	float minXAvg = 0;
	float maxXAvg = 0;
	float minYAvg = 0; 
	float maxYAvg = 0;
	float minZAvg = 0;
	float maxZAvg = 0;
#else 
	FILE *fileOut;					// output BIN file
	NewData newData[GRIDX][GRIDY];	// array of z and intensity
	memset(newData, 0, GRIDX * GRIDY * sizeof(NewData));	// clear array
#endif
	
	do {							// loop
#ifdef FIND_BOUNDARIES		
		float minX =  100000;
		float maxX = -100000;
		float minY =  100000;
		float maxY = -100000;
		float minZ =  100000;
		float maxZ = -100000;
#endif		
		printf("Processing frame %i", frameNum);
		
		sprintf(szBuffer, "txt/%i.csv", frameNum);	// construct filename
		fileIn = fopen(szBuffer, "r");				// open input text file for reading
		
		if(fileIn != NULL) {						// if input file is loaded
			int dotNum = 0;
			while(!feof(fileIn)) {					// loop through file contents
				fgets(szBuffer, 150, fileIn);		// read one line
				
				// read all the variables
				sscanf(szBuffer, "%f,%f,%f,%f\n", &origData[dotNum].x, &origData[dotNum].y, &origData[dotNum].z, &origData[dotNum].i);
				
#ifdef FIND_BOUNDARIES					
				if(minX > origData[dotNum].x) minX = origData[dotNum].x;
				if(maxX < origData[dotNum].x) maxX = origData[dotNum].x;
				if(minY > origData[dotNum].y) minY = origData[dotNum].y;
				if(maxY < origData[dotNum].y) maxY = origData[dotNum].y;
				if(minZ > origData[dotNum].z) minZ = origData[dotNum].z;
				if(maxZ < origData[dotNum].z) maxZ = origData[dotNum].z;
#endif	
				dotNum++;
			}
			fclose(fileIn);							// close input file
			
#ifdef FIND_BOUNDARIES	
			printf(" - minX: %.3f, maxX: %.3f, minY: %.3f, maxY: %.3f, minZ: %.3f, maxZ: %.3f  \n", minX, maxX, minY, maxY, minZ, maxZ);

			minXAvg += minX;
			maxXAvg += maxX;
			minYAvg += minY;
			maxYAvg += maxY;
			minZAvg += minZ;
			maxZAvg += maxZ;

#else		
			printf("\n");
			
			// prepare output file
			sprintf(szBuffer, "data/%i.bin", frameNum);
			fileOut = fopen(szBuffer, "wb");		// open output bin file for writing

			for(int c=0; c<dotNum; c++) {
				int ox = floor((origData[c].x - MIN_X) / DIST_X * GRIDX);			// now x is 0..GRIDX (except for wild ones)
				int oy = floor((origData[c].y - MIN_Y) / DIST_Y * GRIDY);			// now y is 0..GRIDY (except for wild ones)
				int oz = floor((origData[c].z - MIN_Z) / DIST_Z * 256);				// now z is 0..255 (except for wild ones)
				if(ox >=0 && ox < GRIDX && oy >=0 && oy < GRIDY && oz >= 0 && oz < 256) {
					newData[oy][ox].z = oz;			
					newData[oy][ox].i = origData[c].i;
				}
			}
			fwrite(newData, sizeof(NewData), GRIDX * GRIDY, fileOut);
				
			fclose(fileOut);						// close output file
#endif
			
			frameNum++;
		};
	} while(fileIn != NULL);

#ifdef FIND_BOUNDARIES
	float invNum = 1.0f / (frameNum-1);
	minXAvg *= invNum;
	maxXAvg *= invNum;
	minYAvg *= invNum;
	maxYAvg *= invNum;
	minZAvg *= invNum;
	maxZAvg *= invNum;
	printf(" - minXAvg: %.3f, maxXAvg: %.3f, minYAvg: %.3f, maxYAvg: %.3f, minZAvg: %.3f, maxZAvg: %.3f  \n", minXAvg, maxXAvg, minYAvg, maxYAvg, minZAvg, maxZAvg);
#endif
	
	printf(" *** FINISHED CONVERTING %i FRAMES *** \n", frameNum-1);

	
	return 0;
}
