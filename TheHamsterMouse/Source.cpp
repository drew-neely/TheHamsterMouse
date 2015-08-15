#include "wtypes.h"
#include <iostream>
#include <math.h>
#include "Locator.h"

#define PI 3.1415926535

using namespace std;

double zref = 530;
double xref = 325;
double yref = 288;

int leftclick(DWORD dwx, DWORD dwy){

	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.dwExtraInfo = 0;
	input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
	input.mi.mouseData = 0;
	input.mi.time = 0;
	input.mi.dx = (65535 / GetSystemMetrics(SM_CXSCREEN)) * dwx;
	input.mi.dy = (65535 / GetSystemMetrics(SM_CYSCREEN)) * dwy;
	SendInput(1, &input, sizeof(input));
	return 0;
}

int main() {
	/*namedWindow("Left Camera", WINDOW_AUTOSIZE);
	namedWindow("Right Camera", WINDOW_AUTOSIZE);

	int cam1 = -1;
	int cam2 = -1;
	vector<vector<int>> perms;
	int numCams = 0;
	for (int i = 0;; i++) {
		VideoCapture cam(i);
		if (cam.isOpened()) {
			numCams++;
		}
		else {
			break;
		}
	}
	for (int i = 0; i < numCams - 1; i++) {
		for (int j = i + 1; j < numCams; j++) {
			vector<int> v1 = { i, j };
			vector<int> v2 = { j, i };
			perms.push_back(v1);
			perms.push_back(v2);
		}
	}

	for (int i = 0; cam1 < 0 || cam2 < 0; i++) {
		if (i = perms.size) {
			i = 0;
		}
		Mat img1;
		Mat img2;
		VideoCapture cam1(perms[i][0]);
		VideoCapture cam2(perms[i][1]);
		int key = -1;
		while (waitKey(0)) {
			cam1.read(img1);
			cam2.read(img2);
			imshow("Left Camera", img1);
			imshow("Right Camera", img2)
		}
	}*/


	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	int width = 0;
	int height = 0;
	GetWindowRect(hDesktop, &desktop);
	width = (int)desktop.right;
	int adjustedWidth = (int)(width*1.5);
	height = (int)desktop.bottom;
	int adjustedHeight = (int)(height*1.5);

	Locator locator = Locator();

	int screenx = 0;
	int screeny = 0;
	vector<vector<double>> frames;
	bool click = false;
	int i=0;

	bool makeMove = true;

	while (true) {
		vector<double> point = locator.get3DPoint();
		
		//cout << "(" << point[0] << " , " << point[1] << " , " << point[2] << ")" << endl;
		double xwidth = 2 * (point[2] - 164.75) * tan(28.35 / 180 * PI);
		double ywidth = 1.2 * point[2] * tan(34 * PI / 180);
		
		
		
		double zDiff = 0;
		
		if (frames.size()==10){
			zDiff = frames[0][2] - frames[9][2];
		}
		if (((point[0] == 0 && point[1] == 0 && point[2] == 0)||zDiff>=50)&&i>10) {
			if (!frames.empty()){
				if (!click){
					click = true;
					if (makeMove) {
						leftclick(frames[0][0], frames[0][1]);
					}
					i = 0;
				}
			}
		}
		else{
			if (click){ 
				click = false; 
			
			}
			screenx = (int)(((1 - (point[0] + xwidth / 2) / xwidth)* adjustedWidth) - adjustedWidth*.25);
			screeny = (int)(((1 - (point[1] + ywidth / 2) / ywidth) * adjustedHeight) - adjustedHeight*.25);
			//cout << "(" << screenx << " , " << screeny << ")" << endl;
			if (makeMove) {
				SetCursorPos(screenx, screeny);
			}
		}

		vector<double> fullPoint = { (double)screenx, (double)screeny, point[2] };

		frames.push_back(fullPoint);
		if (frames.size() == 11){
			frames.erase(frames.begin());
		}

		i++;

		if (waitKey(30) == 27)
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
		/*if (key == 32) {
			makeMove = false;
 		}
		else {
			makeMove = true;
		}*/
	}

	return 0;
}


