#include "stdafx.h"
#include "GestureAnalyzer.h"


GestureAnalyzer::GestureAnalyzer() {
	FingerParameters.cosThreshold = 0.5;
	FingerParameters.equalThreshold = 1e-7;
	FingerParameters.r = 40;
	FingerParameters.step = 16;
}


GestureAnalyzer::~GestureAnalyzer() {
}

void GestureAnalyzer::analyzeHand(Hand& hand) {
	// process the contour and extract fingers
	extractFingers(hand);
}


// Determine if two floating point values are ~equal, with a threshold
bool GestureAnalyzer::isEqual(const double a, const double b) {
	return fabs(a - b) <= FingerParameters.equalThreshold;
}

// get the angle between two points in a contour
double GestureAnalyzer::getPointsAangle(std::vector<cv::Point>& contour, const int pt, const int r) {
	int size = contour.size();
	cv::Point p0 = (pt>0) ? contour[pt%size] : contour[size - 1 + pt];
	cv::Point p1 = contour[(pt + r) % size];
	cv::Point p2 = (pt>r) ? contour[pt - r] : contour[size - 1 - r];

	double ux = p0.x - p1.x;
	double uy = p0.y - p1.y;
	double vx = p0.x - p2.x;
	double vy = p0.y - p2.y;
	return (ux*vx + uy*vy) / sqrt((ux*ux + uy*uy)*(vx*vx + vy*vy));
}

// get the rotation between two points of the contour	
signed int GestureAnalyzer::getRotation(std::vector<cv::Point>& contour, const int pt, const int r) {
	int size = contour.size();
	cv::Point p0 = (pt>0) ? contour[pt%size] : contour[size - 1 + pt];
	cv::Point p1 = contour[(pt + r) % size];
	cv::Point p2 = (pt>r) ? contour[pt - r] : contour[size - 1 - r];

	double ux = p0.x - p1.x;
	double uy = p0.y - p1.y;
	double vx = p0.x - p2.x;
	double vy = p0.y - p2.y;

	signed int res = (ux*vy - vx*uy);

	return res;
}

// extract the fingertips by processing the contour
int GestureAnalyzer::extractFingers(Hand& hand) {
	int countFingers = 0;
	hand.Parameters.fingers.clear();

	if (hand.Parameters.handContour.size() <= 0)
		return -1;

	for (int j = 0; j < hand.Parameters.handContour.size(); j += FingerParameters.step) {
		double cos0 = getPointsAangle(hand.Parameters.handContour, j, FingerParameters.r);

		if ((cos0 > 0.5) && (j + FingerParameters.step <hand.Parameters.handContour.size())) {
			double cos1 = getPointsAangle(hand.Parameters.handContour, j - FingerParameters.step, FingerParameters.r);
			double cos2 = getPointsAangle(hand.Parameters.handContour, j + FingerParameters.step, FingerParameters.r);
			double maxCos = std::max(std::max(cos0, cos1), cos2);
			bool equal = isEqual(maxCos, cos0);
			signed int z = getRotation(hand.Parameters.handContour, j, FingerParameters.r);

			if (equal == 1 && z < 0) {
				// < init finger >
				Finger finger;
				finger.coordinates = hand.Parameters.handContour[j];
				finger.Angles.orientationAngle = getAngle(hand.handBox.center, finger.coordinates);

				finger.length = getDistance(finger.coordinates, hand.handBox.center);
				// </ init finger>

				hand.Parameters.fingers.push_back(finger);

				if (++countFingers >= 5)
					break;
			}
		}
	}
	return countFingers;
}