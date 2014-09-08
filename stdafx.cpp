#include "stdafx.h"

// get the distance between two cv::Point points
double getDistance(const cv::Point pt1, const cv::Point pt2) {
	return (sqrt(pow(pt1.x - pt2.x, 2) + pow(pt1.y - pt2.y, 2)));
}

// get the angle of a vector, from pt1 to pt2
double getAngle(const cv::Point pt1, const cv::Point pt2) {
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
double correctAngle(double alpha) {
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
bool isInRange(const double value, const double A, const double B) {
	if (value >= fmin(A, B) && value <= fmax(A, B))
		return true;
	else
		return false;
}