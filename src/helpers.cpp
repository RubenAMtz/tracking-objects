/**
  * helpers.cpp
  * 
  * Helper functions for Source.cpp
**/

#include "Header.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstring>

/**
  * Whenever mouse events related to the specified window occur, this callback 
  * function will be called.
  * @args:
  *		- int event: type of the mouse event
  *		- int x: x coordinate of the mouse event
  *		- int y: y coordinate of the mouse event
**/
void onMouse(int event, int x, int y, int, void*)
{
	if (!selectObject)
	{
		switch (event)
		{
		case EVENT_LBUTTONDOWN:
			//set origin of the bounding box
			startSelection = true;
			boundingBox.x = x;
			boundingBox.y = y;
			break;
		case EVENT_LBUTTONUP:
			//sei with and height of the bounding box
			boundingBox.width = std::abs(x - boundingBox.x);
			boundingBox.height = std::abs(y - boundingBox.y);
			paused = false;
			selectObject = true;
			break;
		case EVENT_MOUSEMOVE:

			if (startSelection && !selectObject)
			{
				//draw the bounding box
				Mat currentFrame;
				image.copyTo(currentFrame);
				rectangle(currentFrame, Point((int)boundingBox.x, (int)boundingBox.y), Point(x, y), Scalar(255, 0, 0), 2, 1);
				imshow("Tracking API", currentFrame);
			}
			break;
		}
	}
}

/**
* This function prints in stdout the specifications of the project
**/
void help()
{
	cout << "\nThis example shows the functionality of \"Long-term optical tracking API\""
		"-- pause video [p] and draw a bounding box around the target to start the tracker\n"
		"Example of <video_name> is in opencv_extra/testdata/cv/tracking/\n"
		"Call:\n"
		"./tracker <tracker_algorithm> <video_name> <start_frame> [<bounding_frame>]\n"
		"tracker_algorithm can be: MIL, BOOSTING, MEDIANFLOW, TLD\n"
		<< endl;

	cout << "\n\nHot keys: \n"
		"\tq - quit the program\n"
		"\tp - pause video\n";
}

/**
* This function creates a rectangle out of 2 pair of coordinates given by the user (mouse input)
* @args:
*		- String initBoundingBox: variable used to get the initial value of the rectangle
*		- int coords[4]: array used to store the 2 pair of coordinates
**/
void getBoundingBox(String initBoundingBox, int coords[4]) 
{
	for (size_t npos = 0, pos = 0, ctr = 0; ctr < 4; ctr++) {
		npos = initBoundingBox.find_first_of(',', pos);
		if (npos == string::npos && ctr < 3) {
			printf("bounding box should be given in format \"x1,y1,x2,y2\",where x's and y's are integer cordinates of opposed corners of bdd box\n");
			printf("got: %s\n", initBoundingBox.substr(pos, string::npos).c_str());
			printf("manual selection of bounding box will be employed\n");
			break;
		}
		int num = atoi(initBoundingBox.substr(pos, (ctr == 3) ? (string::npos) : (npos - pos)).c_str());
		if (num <= 0) {
			printf("bounding box should be given in format \"x1,y1,x2,y2\",where x's and y's are integer cordinates of opposed corners of bdd box\n");
			printf("got: %s\n", initBoundingBox.substr(pos, npos - pos).c_str());
			printf("manual selection of bounding box will be employed\n");
			break;
		}
		coords[ctr] = num;
		pos = npos + 1;
	}

	if (coords[0] > 0 && coords[1] > 0 && coords[2] > 0 && coords[3] > 0) {
		initBoxWasGivenInCommandLine = true;
	}
}