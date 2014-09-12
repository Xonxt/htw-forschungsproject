// Forschungsproject (teil 2) at HTW Berlin
// Recognition of short-time micro-gestures from a single-PoV video stream
// (c) Nikita "Xonxt" Kovalenko, 2013-2014, Berlin

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

	// calculate movement speed
	float dx, dy;
	dx = hand.Tracker.KalmanTracker.KF.statePost.at<float>(2);
	dy = hand.Tracker.KalmanTracker.KF.statePost.at<float>(3);
	hand.Parameters.moveSpeed = sqrt(pow(dx, 2) + pow(dy, 2));

	// calculate movement direction
	long int N = hand.Tracker.kalmTrack.size();
	if (N >= 2) {
		double angle = getAngle(hand.Tracker.kalmTrack[N - 2], hand.Tracker.kalmTrack[N - 1]);

		hand.Parameters.moveAngle = angle;

		if (isInRange(angle, NE + 1, NW - 1)) {
			hand.Parameters.moveDirection = MOVEMENT_UP;
		}
		else if (isInRange(angle, NW + 1, SW - 1)) {
			hand.Parameters.moveDirection = MOVEMENT_RIGHT;
		}
		else if (isInRange(angle, SW + 1, SE - 1)) {
			hand.Parameters.moveDirection = MOVEMENT_DOWN;
		}
		else if (isInRange(angle, 0, SE + 1) || isInRange(angle, 0, NE - 1)) {
			hand.Parameters.moveDirection = MOVEMENT_LEFT;
		}
	}
}


// Determine if two floating point values are ~equal, with a threshold
bool GestureAnalyzer::isEqual(const double a, const double b) {
	return fabs(a - b) <= FingerParameters.equalThreshold;
}

// get the angle between two points in a contour
double GestureAnalyzer::getPointsAangle(std::vector<cv::Point>& contour, const int pt, const int r) {
	int size = contour.size();
	cv::Point p0 = (pt > 0) ? contour[pt % size] : contour[size - 1 + pt];
	cv::Point p1 = contour[(pt + r) % size];
	cv::Point p2 = (pt > r) ? contour[pt - r] : contour[size - 1 - r];

	double ux = p0.x - p1.x;
	double uy = p0.y - p1.y;
	double vx = p0.x - p2.x;
	double vy = p0.y - p2.y;
	return (ux * vx + uy * vy) / sqrt((ux * ux + uy * uy) * (vx * vx + vy * vy));
}

// get the rotation between two points of the contour	
signed int GestureAnalyzer::getRotation(std::vector<cv::Point>& contour, const int pt, const int r) {
	int size = contour.size();
	cv::Point p0 = (pt > 0) ? contour[pt % size] : contour[size - 1 + pt];
	cv::Point p1 = contour[(pt + r) % size];
	cv::Point p2 = (pt > r) ? contour[pt - r] : contour[size - 1 - r];

	double ux = p0.x - p1.x;
	double uy = p0.y - p1.y;
	double vx = p0.x - p2.x;
	double vy = p0.y - p2.y;

	signed int res = (ux * vy - vx * uy);

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

// get the angle of a vector, from pt1 to pt2
double GestureAnalyzer::getAngle(const cv::Point pt1, const cv::Point pt2) {
	double alpha; // result

	// calculate vector coordinates
	double	y = pt2.y - pt1.y,
		x = pt2.x - pt1.x;

	// hypotenuse
	double sqrtResult = 0;
	sqrtResult = sqrt(x * x + y * y);
	double angle;
	angle = asin(abs(x) / sqrtResult);

	alpha = angle * 180 / PI; // angle from North in degrees

	// correction
	if (x > 0) {
		// I or IV quadrant
		if (y < 0) {
			// IV quadrant
			alpha = 180 - alpha;
		}
	}
	else {
		// II or III quadrant
		if (y > 0) {
			// II quadrant
			alpha = -alpha;
		}
		else
			alpha = alpha - 180;
	}

	return correctAngle(alpha);
}

// correct the angle to a proper Descartes (0..360) orientation
double GestureAnalyzer::correctAngle(double alpha) {
	double angle = alpha;

	if (isInRange(alpha, 90, 180)) {		// 1st quadrant
		angle -= 90;
	}
	else if (isInRange(alpha, -90, -180)) {	// 2nd quadrant
		angle = (180 - abs(alpha)) + 90;
	}
	else if (isInRange(alpha, -0, -90)) {	// 3rd quadrant
		angle = 180 + (90 - abs(alpha));
	}
	else if (isInRange(alpha, 0, 90)) {		// 4th quadrant
		angle = 270 + abs(alpha);
		if (angle >= 360)
			angle = 0;
	}

	return (angle);
}

// check if the a value lies between two constrains
bool GestureAnalyzer::isInRange(const double value, const double A, const double B) {
	if (value >= fmin(A, B) && value <= fmax(A, B))
		return true;
	else
		return false;
}


// get the distance between two cv::Point points
double GestureAnalyzer::getDistance(const cv::Point pt1, const cv::Point pt2) {
	return (sqrt(pow(pt1.x - pt2.x, 2) + pow(pt1.y - pt2.y, 2)));
}