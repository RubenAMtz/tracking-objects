#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/tracking/tracking.hpp>
#include <opencv2/tracking/tracker.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/opencv.hpp"
#include <iostream>
#include <string>
#include <cstring>

using namespace std;
using namespace cv;

static Mat image;
static Rect2d boundingBox;
static bool paused;
static bool selectObject = false;
static bool startSelection = false;

//the following const char* keys are up to date, they will be used from now on 29/04/2018
static const char* keys =
{ "{@tracker_algorithm |TrackerMIL | Tracker algorithm }"
"{@video_path      |C:\\Bar_Path\\| }"
"{@solution_path   |C:\\Bar_Path\\Procesados\\| Save folder}"
"{@start_frame     |0| Start frame       }"
"{@bounding_frame  |0,0,0,0| Initial bounding frame}" };


static void onMouse(int event, int x, int y, int, void*)
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

static void help()
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

int main(int argc, char** argv) {
	CommandLineParser parser(argc, argv, keys);

	//the following line read from the console the name of the tracker picked by the programmer, however,
	//it looks like the way of defining the tracker has to be hardcoded in the new version.
	//String tracker_algorithm = parser.get<String>(0);
	//String video_name = parser.get<String>(1);
	string video_name;
	cout << "Name of video file: (include extension, i.e.: .mp4, .avi).\nThe root is in: "+ parser.get<String>(1) +"\n";
	getline(cin, video_name);
	int start_frame = parser.get<int>(3);
	string nombre;
	cout << "Name of final video: (exclude the extension)\n";
	getline(cin, nombre);

	int iLastX = -1;
	int iLastY = -1;
	Mat p;

	//if (tracker_algorithm.empty() || video_name.empty())
	if (video_name.empty())
	{
		help();
		return -1;
	}

	int coords[4] = { 0,0,0,0 };
	bool initBoxWasGivenInCommandLine = false;
	{
		String initBoundingBox = parser.get<String>(4);
		for (size_t npos = 0, pos = 0, ctr = 0; ctr<4; ctr++) {
			npos = initBoundingBox.find_first_of(',', pos);
			if (npos == string::npos && ctr<3) {
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
		if (coords[0]>0 && coords[1]>0 && coords[2]>0 && coords[3]>0) {
			initBoxWasGivenInCommandLine = true;
		}
	}

	//open the capture
	VideoCapture cap;
	cap.open(parser.get<String>(1) + video_name);
	cap.set(CAP_PROP_POS_FRAMES, start_frame);
	double dheight = cap.get(CAP_PROP_FRAME_HEIGHT);
	double dwidth = cap.get(CAP_PROP_FRAME_WIDTH);
	Mat background = Mat::zeros(Size(dwidth, dheight), CV_8UC3);;
	//imshow("Background", background);
	//Mat logo = imread("C:\\Users\\hp\\Pictures\\toptier.png");
	//Mat logo = imread("C:\\Users\\R.Alvarez\\Documents\\Camtasia Studio\\Bar Path\\toptier.png");
	//resize(logo, logo, Size(logo.cols * 0.1, logo.rows * 0.1));
	//logo.copyTo(background(Rect(background.cols-logo.cols, background.rows-logo.rows, logo.cols, logo.rows)));
	//imshow("Back + logo", background);

	//check later (29/04/2018)
	//double fps = cap.get(CV_CAP_PROP_FPS);
	//cout << "FPS: " << fps << endl;
	//double msec = cap.get(CV_CAP_PROP_POS_MSEC);
	//cout << "Msec: " << fps << endl;

	if (!cap.isOpened())
	{
		help();
		cout << "***Could not initialize capturing...***\n";
		cout << "Current parameter's value: \n";
		parser.printMessage();
		return -1;
	}

	Mat frame;
	paused = true;
	//namedWindow("Tracking API", 1);
	namedWindow("Tracking API", WINDOW_NORMAL);
	setMouseCallback("Tracking API", onMouse, 0);

	//instantiates the specific Tracker
	//Ptr<Tracker> tracker = Tracker::init(tracker_algorithm);

	Ptr<Tracker> tracker = TrackerKCF::create();
	if (tracker == NULL)
	{
		cout << "***Error in the instantiation of the tracker...***\n";
		return -1;
	}

	//Capture a temporary image from the camera
	Mat imgTmp;
	cap.read(imgTmp);

	//Create a black image with the size as the camera output
	Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);;

	//get the first frame
	cap >> frame;
	frame.copyTo(image);
	if (initBoxWasGivenInCommandLine) {
		selectObject = true;
		paused = false;
		boundingBox.x = coords[0];
		boundingBox.y = coords[1];
		boundingBox.width = std::abs(coords[2] - coords[0]);
		boundingBox.height = std::abs(coords[3] - coords[1]);
		printf("bounding box with vertices (%d,%d) and (%d,%d) was given in command line\n", coords[0], coords[1], coords[2], coords[3]);
		rectangle(image, boundingBox, Scalar(255, 0, 0), 2, 1);
	}
	imshow("Tracking API", image);

	bool initialized = false;
	int frameCounter = 0;

	//VideoWriter oVideoWriter("C:\\Users\\hp\\Desktop\\Bar Path\\Procesados\\"+nombre+".avi", CV_FOURCC('M', 'P', '4', '2'), 30, Size(dwidth,dheight), true); //initialize the VideoWriter object 
	VideoWriter oVideoWriter(parser.get<String>(2) + nombre + ".avi", cv::VideoWriter::fourcc('M', 'P', '4', '2'), 30, Size(dwidth, dheight), true); //initialize the VideoWriter object 

	if (!oVideoWriter.isOpened()) //if not initialize the VideoWriter successfully, exit the program
	{
		cout << "ERROR: Failed to write the video" << endl;
		return -1;
	}

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
					rectangle(image, boundingBox, Scalar(255, 0, 0), 2, 1);
					line(imgLines, Point(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2), Point(iLastX, iLastY), Scalar(0, 255, 0), 2);
				}
			}
			imshow("Tracking API", image + imgLines);
			frameCounter++;

			//fps = cap.get(CV_CAP_PROP_FPS);
			//cout << "FPS: " << fps << endl;
			//msec = cap.get(CV_CAP_PROP_POS_MSEC);
			//cout << "Msec: " << fps << endl;

			p = image + imgLines + background*0.4;

			//water.copyTo(p(Rect(p.cols/2-water.cols/2, p.rows / 2 - water.rows / 2, water.cols, water.rows)));

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