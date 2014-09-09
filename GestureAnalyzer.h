#pragma once
#include "Hand.h"

class GestureAnalyzer {
public:
	GestureAnalyzer();
	~GestureAnalyzer();

	// process a hand, fill in its parameters
	void analyzeHand(Hand& hand);

private:
	// process the hand contour and extract fingers. Returns the amount of fingers
	int extractFingers(Hand& hand);

	// Determine if two floating point values are ~equal, with a threshold
	bool isEqual(const double a, const double b);

	// get the angle between two points in a contour
	double getPointsAangle(std::vector<cv::Point>& contour, const int pt, const int r);

	// get the rotation between two points of the contour
	signed int getRotation(std::vector<cv::Point>& contour, const int pt, const int r);

	// the parameters, used for finger-tip extraction
	struct {
		int r;
		int step;
		double cosThreshold;
		double equalThreshold;
	} FingerParameters;
};

