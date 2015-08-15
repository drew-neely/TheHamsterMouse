

#ifndef LOCATOR_H
#define LOCATOR_H

#include <iostream>
#include "opencv2\highgui\highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;


void buttonPressed(int state, void* userdata) {
	userdata = &state;
}

class Locator
{

public:

	VideoCapture cam1;
	VideoCapture cam2;

	/*
	int getPoint() {

	if (!cam1.isOpened() || !cam2.isOpened()) {
	cout << "Camera is not opened.";
	}

	namedWindow("Cam1", WINDOW_AUTOSIZE);
	namedWindow("Cam2", WINDOW_AUTOSIZE);

	while (true) {
	Mat img1;
	Mat img2;

	cam1.read(img1);
	cam2.read(img2);

	imshow("Cam1", img1);
	imshow("Cam2", img2);

	if (waitKey(30) == 27)
	{
	break;
	}

	}
	return 0;
	}
	*/

	Locator(){

		init();
	}



	void init() {

		cam1 = VideoCapture(1);
		cam2 = VideoCapture(2);


		namedWindow("Morphed0", WINDOW_AUTOSIZE);
		namedWindow("Morphed2", WINDOW_AUTOSIZE);
		namedWindow("Control", WINDOW_NORMAL);

		cvCreateTrackbar("LowH", "Control", &lowH, 179); //Hue (0 - 179)
		cvCreateTrackbar("HighH", "Control", &highH, 179);

		cvCreateTrackbar("LowS", "Control", &lowS, 255); //Saturation (0 - 255)
		cvCreateTrackbar("HighS", "Control", &highS, 255);

		cvCreateTrackbar("LowV", "Control", &lowV, 255); //Value (0 - 255)
		cvCreateTrackbar("HighV", "Control", &highV, 255);


	}

	const Size imgSize = Size(640, 480);
	const double focalLen = 2.3;
	const double baseline = 177.8;
	const double mmpp = .003878424;

	vector<double> get3DPoint() {

		vector <double> finalPoint;
		Point PointLeft = get2DPoint(0);
		Point PointRight = get2DPoint(1);

		if (PointLeft.x == -1 && PointRight.x == -1) {
			finalPoint.push_back(0);
			finalPoint.push_back(0);
			finalPoint.push_back(0);
		}

		else {
			
			double dx = abs(PointLeft.x - PointRight.x) * mmpp;

			double z = (baseline * focalLen / dx);
			double x = ((PointLeft.x-320) * mmpp * z / focalLen) - (baseline / 2);
			double y = ((-PointLeft.y+240)* mmpp * z / focalLen);

			finalPoint.push_back(x);
			finalPoint.push_back(y);
			finalPoint.push_back(z);
		}
		return finalPoint;
	}


	int lowH = 92;
	int highH = 130;
	int lowS = 70;
	int highS = 206;
	int lowV = 60;
	int highV = 153;

	Point get2DPoint(int cameraNum)
	{

		VideoCapture camera;

		if (cameraNum == 0) {
			camera = cam1;
		}
		else if (cameraNum == 1) {
			camera = cam2;
		}
		else {
			return NULL;
		}

		Mat imgOrig;
		camera.read(imgOrig);
		resize(imgOrig, imgOrig, imgSize);
		//flip(imgOrig, imgOrig, 1);
		Mat imgBlurred;
		GaussianBlur(imgOrig, imgBlurred, Size(7, 7), 7, 7);
		cvtColor(imgBlurred, imgBlurred, COLOR_BGR2HSV);
		Mat threshold;
		Scalar low(lowH, lowS, lowV);
		Scalar high(highH, highS, highV);
		inRange(imgBlurred, low, high, threshold);
		Mat morphed;
		//open
		erode(threshold, morphed, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(morphed, morphed, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		//close
		dilate(morphed, morphed, getStructuringElement(MORPH_ELLIPSE, Size(9, 9)));
		erode(morphed, morphed, getStructuringElement(MORPH_ELLIPSE, Size(9, 9)));

		/*imshow("Blurred", imgBlurred);
		imshow("Threshold", threshold);*/
		if (cameraNum == 0) {
			imshow("Morphed0", morphed);
		}
		else if (cameraNum == 1) {
			imshow("Morphed2", morphed);
		}

		Moments moms = moments(morphed);
		if (moms.m00 > 2000) {
			int x = moms.m10 / moms.m00;
			int y = moms.m01 / moms.m00;
			return Point(x, y);
			
		}
		else {
			return Point(-1, -1);
		}
	}
};
#endif