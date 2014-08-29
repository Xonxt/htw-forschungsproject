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

