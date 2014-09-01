#pragma once

#include "stdafx.h"

class Hand
{
public:
	Hand();

	// init the tracker
	void initTracker();

	void assignNewLocation(const cv::RotatedRect& newBbox);

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

		// Kalman Filter Data
		struct KalmanFilterTracker {
			cv::Mat_<float> measurement;
			cv::KalmanFilter KF;
			cv::Mat state;
			cv::Mat processNoise;
		} KalmanTracker;

	} Tracker;


};

