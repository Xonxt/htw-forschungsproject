// Forschungsproject (teil 2) at HTW Berlin
// Recognition of short-time micro-gestures from a single-PoV video stream
// (c) Nikita "Xonxt" Kovalenko, 2013-2014, Berlin

#pragma once
#include "Hand.h"

#if defined(__APPLE__)
	#include "GeometricRecognizer.h"
#else
	#include "OneDollar\GeometricRecognizer.h"
#endif

// angles
#define NW 135
#define NE 45
#define SE 315
#define SW 225

class GestureAnalyzer {
public:
	GestureAnalyzer();
	GestureAnalyzer(const cv::Size _frameSize);
	~GestureAnalyzer();

	// process a hand, fill in its parameters
	void analyzeHand(Hand& hand);

private:
	// process the hand contour and extract fingers. Returns the amount of fingers
	int extractFingers(Hand& hand);

	// frame size
	cv::Size frameSize;

	// geometric rcognizer
	DollarRecognizer::GeometricRecognizer geometricRecognizer;

	// Determine if two floating point values are ~equal, with a threshold
	bool isEqual(const double a, const double b);

	// get the angle between two points in a contour
	double getPointsAangle(std::vector<cv::Point>& contour, const int pt, const int r);

	// get the rotation between two points of the contour
	signed int getRotation(std::vector<cv::Point>& contour, const int pt, const int r);

	// get the angle of a vector, from pt1 to pt2
	double getAngle(const cv::Point pt1, const cv::Point pt2);

	// check if the a value lies between two constrains
	bool isInRange(const double value, const double A, const double B);

	// correct the angle to a proper Descartes (0..360) orientation
	double correctAngle(double alpha);

	// get the distance between two cv::Point points
	double getDistance(const cv::Point pt1, const cv::Point pt2);

	// the parameters, used for finger-tip extraction
	struct {
		int r;
		int step;
		double cosThreshold;
		double equalThreshold;
	} FingerParameters;
};

