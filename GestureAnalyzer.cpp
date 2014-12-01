// Forschungsproject (teil 2) at HTW Berlin
// Recognition of short-time micro-gestures from a single-PoV video stream
// (c) Nikita "Xonxt" Kovalenko, 2013-2014, Berlin

#include "stdafx.h"
#include "GestureAnalyzer.h"

GestureAnalyzer::GestureAnalyzer() {

}

GestureAnalyzer::GestureAnalyzer(const cv::Size _frameSize) {
	FingerParameters.cosThreshold = 0.5;
	FingerParameters.equalThreshold = 1e-7;
	FingerParameters.r = 40; // 40;
	FingerParameters.step = 16; // 16;

	frameSize = _frameSize;
}


GestureAnalyzer::~GestureAnalyzer() {
}

void GestureAnalyzer::analyzeHand(Hand& hand) {



	// process the contour and extract fingers
	extractFingers(hand);
	
	// add amount of fingers
	switch (hand.Parameters.fingers.size()) {
	case 0:
		hand.handGesture.varFingers.addValue(FINGERS_ZERO);
		break;
	case 1:
		hand.handGesture.varFingers.addValue(FINGERS_ONE);
		break;
	case 2:
		hand.handGesture.varFingers.addValue(FINGERS_TWO);
		break;
	case 3:
		hand.handGesture.varFingers.addValue(FINGERS_THREE);
		break;
	case 4:
		hand.handGesture.varFingers.addValue(FINGERS_FOUR);
		break;
	default:
		hand.handGesture.varFingers.addValue(FINGERS_FIVE);
	}

	// set finger angles:
	if (hand.Parameters.fingers.size() == 2) {
		if (hand.Parameters.fingers[0].Angles.angle2next <= 30) {
			hand.handGesture.varAngle.addValue(ANGLE_CLOSE);
		}
		else {
			hand.handGesture.varAngle.addValue(ANGLE_FAR);
		}
	}
	else
		hand.handGesture.varAngle.addValue(ANGLE_NONE);

	// process the kalm track to find if it's stationary
	int size = hand.Tracker.kalmTrack.size();
	if (size >= 3) {

		cv::Point
			a = hand.Tracker.kalmTrack[size - 1],
			b = hand.Tracker.kalmTrack[size - 2],
			c = hand.Tracker.kalmTrack[size - 3];

		cv::Point2f
			af = cv::Point2f((float)a.x / (float)frameSize.width, (float)a.y / (float)frameSize.height),
			bf = cv::Point2f((float)b.x / (float)frameSize.width, (float)b.y / (float)frameSize.height),
			cf = cv::Point2f((float)c.x / (float)frameSize.width, (float)c.y / (float)frameSize.height);

		double mx, my, dx, dy;

		mx = (af.x + bf.x + cf.x) / 3; 		my = (af.y + bf.y + cf.y) / 3;

		dx = (pow(af.x - mx, 2) + pow(bf.x - mx, 2) + pow(bf.x - mx, 2)) / 3;
		dy = (pow(af.y - my, 2) + pow(bf.y - my, 2) + pow(bf.y - my, 2)) / 3;

		dx = sqrt(dx); dy = sqrt(dy);

		if (dx < 0.005 && dy < 0.005) {
			hand.Tracker.kalmTrack.clear();
			hand.Tracker.kalmTrack.push_back(a);
		}

	}

	// calculate movement speed
    if (hand.Tracker.kalmTrack.size() > 1) {
			hand.Parameters.moveSpeed = getDistance(hand.Tracker.kalmTrack[hand.Tracker.kalmTrack.size() - 1], hand.Tracker.kalmTrack[hand.Tracker.kalmTrack.size() - 2]);
        hand.Parameters.moveSpeed = fabs(hand.Parameters.moveSpeed);
        
        hand.Parameters.moveSpeed /= MAX(hand.handBox.size.width, hand.handBox.size.height);
    }
	else {
        hand.Parameters.moveSpeed = 0;
    }

	// set movement speed type
	if (hand.Parameters.moveSpeed < 0.05) {
		hand.handGesture.varSpeed.addValue(SPEED_NONE);
	}
	else if (isInRange(hand.Parameters.moveSpeed, 0.05, 0.2)) {
		hand.handGesture.varSpeed.addValue(SPEED_SLOW);
	}
	else {
		hand.handGesture.varSpeed.addValue(SPEED_FAST);
	}


	// calculate movement direction
	long int N = hand.Tracker.kalmTrack.size();
	if (N >= 2) {
		double angle = getAngle(hand.Tracker.kalmTrack[N - 2], hand.Tracker.kalmTrack[N - 1]);

		hand.Parameters.moveAngle = angle;

		if (isInRange(angle, NE + 1, NW - 1)) {
			//hand.Parameters.moveDirection = MOVEMENT_UP;
			hand.handGesture.varDirection.addValue(MOVEMENT_UP);
		}
		else if (isInRange(angle, NW + 1, SW - 1)) {
			//hand.Parameters.moveDirection = MOVEMENT_RIGHT;
			hand.handGesture.varDirection.addValue(MOVEMENT_RIGHT);
		}
		else if (isInRange(angle, SW + 1, SE - 1)) {
			//hand.Parameters.moveDirection = MOVEMENT_DOWN;
			hand.handGesture.varDirection.addValue(MOVEMENT_DOWN);
		}
		else if (isInRange(angle, 0, SE + 1) || isInRange(angle, 0, NE - 1)) {
			//hand.Parameters.moveDirection = MOVEMENT_LEFT;
			hand.handGesture.varDirection.addValue(MOVEMENT_LEFT);
		}
		else
			hand.handGesture.varDirection.addValue(MOVEMENT_NONE);
	}    
	/*
    // clear the tracks
    if (hand.Tracker.kalmTrack.size() > 8) {
        hand.Tracker.kalmTrack.erase(hand.Tracker.kalmTrack.begin());
    }
    if (hand.Tracker.camsTrack.size() > 8) {
        hand.Tracker.camsTrack.erase(hand.Tracker.camsTrack.begin());
    }*/
}

// Determine if two floating point values are ~equal, with a threshold
bool GestureAnalyzer::isEqual(const double a, const double b) {
	return fabs(a - b) <= FingerParameters.equalThreshold;
}

// get the angle between two points in a contour
double GestureAnalyzer::getPointsAangle(std::vector<cv::Point>& contour, const int pt, const int r) {
	int size = (int)contour.size();
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
	int size = (int)contour.size();
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

				if (hand.Parameters.fingers.size() > 1) {
					hand.Parameters.fingers[hand.Parameters.fingers.size() - 1].Angles.angle2next =
						fabs(hand.Parameters.fingers[hand.Parameters.fingers.size() - 1].Angles.orientationAngle - finger.Angles.orientationAngle);
				}

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