#include "../include/Header.h"
#include "../include/globals.h"
#include <iostream>
#include <string>
#include <cstring>


bool selectObject = false;
bool paused = false;
bool startSelection = false;
Rect2d boundingBox;
Mat image;
bool initBoxWasGivenInCommandLine = false;

//the following const char* keys are up to date, they will be used from now on 29/04/2018
static const char* keys =
{ "{@tracker_algorithm |TrackerMIL | Tracker algorithm }"
"{@video_path      |C:\\Bar_Path\\| }"
"{@solution_path   |C:\\Bar_Path\\Procesados\\| Save folder}"
"{@start_frame     |0| Start frame       }"
"{@bounding_frame  |0,0,0,0| Initial bounding frame}" };


int main(int argc, char** argv) {
	CommandLineParser parser(argc, argv, keys);

	//the following line read from the console the name of the tracker picked by the programmer, however,
	//it looks like the way of defining the tracker has to be hardcoded in the new version.
	//String tracker_algorithm = parser.get<String>(0);
	//String videoName = parser.get<String>(1);
	string videoName;
	string videoSolution;
	String videoNamePath = parser.get<String>(1);
	int start_frame = parser.get<int>(3);
	
	getFileNames(videoName, videoSolution, videoNamePath);

	if (videoName.empty())
	{
		help();
		return -1;
	}

	int coords[4] = { 0,0,0,0 };
	String initBoundingBox = parser.get<String>(4);
	getBoundingBox(initBoundingBox, coords);

	//open the capture
	VideoCapture cap;
	cap.open(videoNamePath + videoName);
	
	if (!cap.isOpened())
	{
		help();
		cout << "***Could not initialize capturing...***\n";
		cout << "Current parameter's value: \n";
		parser.printMessage();
		return -1;
	}
	cap.set(CAP_PROP_POS_FRAMES, start_frame);
	double dheight = cap.get(CAP_PROP_FRAME_HEIGHT);
	double dwidth = cap.get(CAP_PROP_FRAME_WIDTH);

	namedWindow("Tracking API", WINDOW_NORMAL);
	setMouseCallback("Tracking API", onMouse, 0);

	//instantiates the specific Tracker
	Ptr<Tracker> tracker = TrackerKCF::create();
	if (tracker == NULL)
	{
		cout << "***Error in the instantiation of the tracker...***\n";
		return -1;
	}

	Mat frame;
	paused = true;
	
	//get the first frame
	cap >> frame;
	frame.copyTo(image);
	if (initBoxWasGivenInCommandLine) {
		selectObject = true;
		paused = false;
		//create rectangle
		boundingBox.x = coords[0];
		boundingBox.y = coords[1];
		boundingBox.width = std::abs(coords[2] - coords[0]);
		boundingBox.height = std::abs(coords[3] - coords[1]);
		printf("bounding box with vertices (%d,%d) and (%d,%d) was given in command line\n", coords[0], coords[1], coords[2], coords[3]);
		rectangle(image, boundingBox, Scalar(255, 0, 0), 2, 1);
	}
	imshow("Tracking API", image);

	bool initialized = false;

	VideoWriter oVideoWriter(parser.get<String>(2) + videoSolution + ".avi", cv::VideoWriter::fourcc('M', 'P', '4', '2'), 30, Size(dwidth, dheight), true); //initialize the VideoWriter object 

	if (!oVideoWriter.isOpened()) //if not initialize the VideoWriter successfully, exit the program
	{
		cout << "ERROR: Failed to write the video" << endl;
		return -1;
	}
	
	double iLastX = -1;
	double iLastY = -1;
	Mat p;
	vector<Point> barPath;

	//Capture a temporary image from the camera
	Mat imgTmp;
	cap.read(imgTmp);

	//Create a black image with the size as the camera output
	Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);

	for (;; )
	{
		if (!paused)
		{
			if (initialized) {
				cap >> frame;
				if (frame.empty()) {
					break;
				}
				frame.copyTo(image);
			}

			if (!initialized && selectObject)
			{
				//initializes the tracker
				if (!tracker->init(frame, boundingBox))
				{
					cout << "***Could not initialize tracker...***\n";
					return -1;
				}
				initialized = true;
			}
			else if (initialized)
			{
				iLastX = boundingBox.x + boundingBox.width / 2;
				iLastY = boundingBox.y + boundingBox.height / 2;
				//updates the tracker
				if (tracker->update(frame, boundingBox))
				{
					rectangle(image, boundingBox, Scalar(255.0, 0.0, 0.0), 2, 1);
					//draw green lines to show path of bar
					line(imgLines, 
						Point2d(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2), 
						Point2d(iLastX, iLastY), Scalar(0.0, 255.0, 0.0), 2);
					//store points in barPath vector
					barPath.push_back( Point2d(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2));
				}
			}

			imshow("Tracking API", image + imgLines);
			p = image + imgLines;

			oVideoWriter.write(p); //writer the frame into the file
								   //imshow("MyVideo", p);
		}

		char c = (char)waitKey(2);
		if (c == 'q')
			break;
		if (c == 'p')
			paused = !paused;
	}
	return 0;
}