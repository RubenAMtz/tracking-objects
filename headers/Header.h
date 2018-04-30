#pragma once
#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/tracking/tracking.hpp>
#include <opencv2/tracking/tracker.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

void onMouse(int event, int x, int y, int, void*);
void help();
void getBoundingBox(String initBoundingBox, int coords[4]);