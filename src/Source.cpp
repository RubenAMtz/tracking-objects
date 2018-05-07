#include "../include/Header.h"
#include "../include/globals.h"
#include <iostream>
#include <string>
#include <cstring>


bool selectObject = false;
bool paused = false;
Rect2d boundingBox;
Mat image;
//bool initBoxWasGivenInCommandLine = true;
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
	//getBoundingBox(initBoundingBox, coords); // ask for the coordinates through the console...

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
	double fps = cap.get(CAP_PROP_FPS);
	double dheight = cap.get(CAP_PROP_FRAME_HEIGHT);
	double dwidth = cap.get(CAP_PROP_FRAME_WIDTH);

	namedWindow("Tracking API", WINDOW_NORMAL);
	//setMouseCallback("Tracking API", onMouse, 0);

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

	//////////////////////////////////////////////////////////////// circle detection
	Mat src_gray;
	if (!image.data)
	{
		return -1;
	}

	/// Convert it to gray
	cvtColor(image, src_gray, CV_BGR2GRAY);

	/// Reduce the noise so we avoid false circle detection
	GaussianBlur(src_gray, src_gray, Size(15, 15), 2, 2);
	//imshow("Prueba", src_gray);
	//Setup a rectangle to define your region of interest
	cout << "Height : " << src_gray.size().height << endl;
	cout << "Width : " << src_gray.size().width << endl;
	double width = src_gray.cols;
	double height = src_gray.rows;
	Rect myROI(width / 4, height/2, (width / 4)*2, height/2 ); //the second x coordinate uses the previous x coordinate as reference not the origin.
													   // Crop the full image to that image contained by the rectangle myROI
													   // Note that this doesn't copy the data
	cv::Mat croppedImage = src_gray(myROI);
	//imshow("Cropped", croppedImage);

	vector<Vec3f> circles;

	/// Apply the Hough Transform to find the circles
	//HoughCircles(croppedImage, circles, HOUGH_GRADIENT, 1, croppedImage.rows / 15, 100, 50, 0, 0);
	HoughCircles(croppedImage, circles, HOUGH_GRADIENT, 1, src_gray.rows /2, 60, 30, 0, 0);
	//HoughCircles(croppedImage, circles, CV_HOUGH_GRADIENT, 1, src_gray.rows / 8, 200, 100, 0, 0);

	for (size_t i = 0; i < circles.size(); i++)
		/// Draw the circles detected
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		// circle center
		//circle(src, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		circle(croppedImage, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		// circle outline
		//circle(src, center, radius, Scalar(0, 0, 255), 3, 8, 0);
		circle(croppedImage, center, radius, Scalar(0, 0, 255), 3, 8, 0);
	}

	/// Show your results
	namedWindow("All circles", WINDOW_AUTOSIZE);
	//imshow("Hough Circle Transform Demo", src);
	imshow("All circles", croppedImage);


	cout <<"Circles cap: "<<circles[0][0]<<endl;
	double center_x = 0;
	double center_y = 0;
	double radius = 0;
	//detect the circle closest to the center
	if (circles[0][0] != 0) {
		double distance = abs(circles[0][0] - croppedImage.size().width / 2); //determines first distance with reference to the center of the picture
		int index = 0;
		for (size_t z = 0; z < circles.size(); z++)
		{
			if (abs(circles[z][0] - croppedImage.size().width / 2) < distance) { //starts looking for a better closest distance to the center of the picture
				index = z;
				distance = abs(circles[z][0] - croppedImage.size().width / 2);
			}
		}

		Point center(cvRound(circles[index][0]), cvRound(circles[index][1]));
		int rad = cvRound(circles[index][2]);
		// circle center
		//circle(src, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		circle(croppedImage, center, rad, Scalar(255, 0, 0), -1, 8, 0);
		// circle outline
		//circle(src, center, radius, Scalar(0, 0, 255), 3, 8, 0);
		circle(croppedImage, center, rad, Scalar(0, 255, 0), 3, 8, 0);
		/// Show your results
		namedWindow("Hough Circle Transform Demo", WINDOW_AUTOSIZE);
		//imshow("Hough Circle Transform Demo", src);
		imshow("Hough Circle Transform Demo", croppedImage);
		imwrite("foto.jpeg", croppedImage);

		cout << "Index : " << index << endl;
		cout << "Coordinates." << endl;
		center_x = circles[index][0];
		center_y = circles[index][1];
		radius = (circles[index][2])*1.1; //increase radius to get better tracking area.
		cout << "Center (x-coordinate):" << circles[index][0] << endl;
		cout << "Center (y-coordinate):" << circles[index][1] << endl;
		cout << "Radius (x-coordinate):" << circles[index][2] << endl;
		if (radius != 0)
		{
			initBoxWasGivenInCommandLine = true;
		}
		
	}
	/////////////////////////////////////////////////////////////////
	
	if (circles[0][0] != 0) {
	
		if (initBoxWasGivenInCommandLine) {
			selectObject = true;
			paused = false;
			//create rectangle
			boundingBox.x = center_x + width/4 - radius;
			boundingBox.y = center_y + height/2 - radius;
			boundingBox.width = radius*2;
			boundingBox.height = radius*2;
			//printf("bounding box with vertices (%d,%d) and (%d,%d) was given in command line\n", coords[0], coords[1], coords[2], coords[3]);
			rectangle(image, boundingBox, Scalar(255, 0, 0), 2, 1);
		}

		imshow("Tracking API", image);

		string videoSolutionPath = parser.get<String>(2) + videoSolution + ".avi";
		//initialize the VideoWriter object
		VideoWriter oVideoWriter(videoSolutionPath, cv::VideoWriter::fourcc('M', 'P', '4', '2'), fps, Size((int)dwidth, (int)dheight), true);

		if (!oVideoWriter.isOpened()) //if not initialize the VideoWriter successfully, exit the program
		{
			cout << "ERROR: Failed to write the video" << endl;
			return -1;
		}

		Mat p;
		vector<Point> barPath;
		bool initialized = false;
		double centerXa = -1;
		double centerYa = -1;

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
					//get center of rectangle
					centerXa = boundingBox.x + boundingBox.width / 2;
					centerYa = boundingBox.y + boundingBox.height / 2;

					//updates the tracker
					if (tracker->update(frame, boundingBox))
					{
						rectangle(image, boundingBox, Scalar(255.0, 0.0, 0.0), 2, 1);

						double centerXb = boundingBox.x + boundingBox.width / 2;
						double centerYb = boundingBox.y + boundingBox.height / 2;

						//draw green lines to show path of bar
						line(imgLines,
							Point2d(centerXb, centerYb),
							Point2d(centerXa, centerYa),
							Scalar(0.0, 255.0, 0.0), 2
						);

						//store points in barPath vector
						barPath.push_back(Point2d(centerXa, centerYa));
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
	}

	if (circles[0][0] == 0) {
		cout << "No circle found" << endl;
		cin.get();
	}
	return 0;
}