#pragma once

#include "stdafx.h"

#include "Finger.h"

class Hand
{
public:
	Hand();
    
    ~Hand();

	// init the tracker
	void initTracker();

	// assign new location to the hand
	void assignNewLocation(const cv::RotatedRect& newBbox);

	// process the hand contour and extract fingers. Returns the amount of fingers
	int extractFingers();

	// the hand's bounding box
	cv::RotatedRect handBox;

	// the hands ROI
	cv::Rect roiRectange;

	// the information for the gesture
	struct HandInformation {
		// the speed of hand movement (normalized to ROI width)
		double moveSpeed;
		// the angle of hand movenent in degrees (0..359)
		double moveAngle;
		// extimated movement direction (up, down, left, right)
		MovementDirection moveDirection;
		// the hand contour
		std::vector<cv::Point> handContour;
		// list of fingertips
		std::vector<Finger> fingers;
	} Parameters;

	// all the tracking information
	struct TrackingInformation 
	{
		// the tracking window for the CamShift algorithm
		cv::Rect trackWindow;

		// is the hand being tracked at the moment?
		bool isTracked;

		// hand histogram
		cv::Mat hist;

		// is it tracked by KalmanFilter?
		bool isKalman;

		// kalman track
		std::vector<cv::Point> kalmTrack;

		// Kalman Filter Data
		struct KalmanFilterTracker {
			cv::Mat_<float> measurement;
			cv::KalmanFilter KF;
			cv::Mat state;
			cv::Mat processNoise;
		} KalmanTracker;

	} Tracker;

private:

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

